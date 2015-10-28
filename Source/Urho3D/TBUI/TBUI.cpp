

#include "../Precompiled.h"
#include "../Core/Context.h"
#include "../Core/CoreEvents.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/GraphicsEvents.h"
#include "../Graphics/VertexBuffer.h"
#include "../Graphics/Texture2D.h"
#include "../Input/Input.h"
#include "../Input/InputEvents.h"
#include "../Resource/ResourceCache.h"


#include "TBUI.h"

#include <TurboBadger/tb_core.h>
#include <TurboBadger/tb_debug.h>
#include <TurboBadger/tb_font_renderer.h>
#include <TurboBadger/tb_language.h>
#include <TurboBadger/tb_skin.h>
#include <TurboBadger/tb_widgets_reader.h>

#include <TurboBadger/tb_message_window.h>
#include <TurboBadger/tb_node_tree.h>
#include <TurboBadger/animation/tb_widget_animation.h>

// TB global functions
void register_freetype_font_renderer();

namespace tb
{
	static Urho3D::WeakPtr<Urho3D::Context> _context;

	void TBSystem::RescheduleTimer(double fire_time) { }

	TBFile* TBFile::Open(const char *filename, TBFileMode mode)
	{
		auto cache = _context->GetSubsystem<Urho3D::ResourceCache>();

		Urho3D::SharedPtr<Urho3D::File> file = cache->GetFile(filename);

		if (!file || !file->IsOpen())
		{
			//LOGERRORF("TBUI: Unable to load file: %s", filename);
			return 0;
		}

		JRPG::UI::TBUrho3DFile* tbUrho3DFile = new JRPG::UI::TBUrho3DFile(file);

		return tbUrho3DFile;
	}
}

namespace JRPG
{
	namespace UI
	{
		Bitmap::Bitmap(Urho3D::Context* context) :
			Urho3D::Object(context)
		{
		}

		Bitmap::~Bitmap()
		{

		}

		void Bitmap::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<Bitmap>();
		}

		void Bitmap::Init(int width, int height, tb::uint32 *data)
		{
			size = Urho3D::Vector2(width, height);

			SetData(data);
		}

		void Bitmap::SetData(tb::uint32 *data)
		{
			auto ui = GetSubsystem<TBUI>();

			ui->FlushBitmap(this);

			if (texture.Null())
			{
				texture = new Urho3D::Texture2D(context_);

				// Needs to be called BEFORE Texture2D::SetSize
				texture->SetAddressMode(Urho3D::COORD_U, Urho3D::ADDRESS_BORDER);
				texture->SetAddressMode(Urho3D::COORD_V, Urho3D::ADDRESS_BORDER),
					texture->SetBorderColor(Urho3D::Color(0.0f, 0.0f, 0.0f, 0.0f));
				texture->SetMipsToSkip(Urho3D::QUALITY_LOW, 0);
				texture->SetNumLevels(1);

				texture->SetSize(size.x_, size.y_, Urho3D::Graphics::GetRGBAFormat(), Urho3D::TEXTURE_STATIC);
			}

			texture->SetData(0, 0, 0, size.x_, size.y_, data);
		}

		TBUrho3DFile::TBUrho3DFile(Urho3D::File* file) :
			_file(file)
		{

		}
		TBUrho3DFile::~TBUrho3DFile()
		{
			_file->Close();
		}

		long TBUrho3DFile::Size()
		{
			return _file->GetSize();
		}

		size_t TBUrho3DFile::Read(void *buf, size_t elemSize, size_t count)
		{
			size_t size = elemSize * count;
			size_t totalSize = 0;

			totalSize += _file->Read(buf, size);

			return totalSize;
		}

		TBUI::TBUI(Urho3D::Context* context) :
			Urho3D::Object(context),
			_root(0)
		{
			tb::_context = context;
		}

		TBUI::~TBUI()
		{
			TBWidgetListener::RemoveGlobalListener(this);
			tb::TBWidgetsAnimationManager::Shutdown();

			delete _root;

			tb::tb_core_shutdown();
		}

		void TBUI::RegisterObject(Urho3D::Context* context)
		{
			context->RegisterFactory<TBUI>();
		}

		void TBUI::RegisterSystem(Urho3D::Context* context)
		{
			TBUI::RegisterObject(context);

			UI::Bitmap::RegisterObject(context);
		}

		void TBUI::Init()
		{
			SetupTurboBadger();
			SubscribeToEvents();
		}

		void TBUI::AddFontInfo(const Urho3D::String& fileName, const Urho3D::String& fontName)
		{
			tb::g_font_manager->AddFontInfo(fileName.CString(), fontName.CString());
		}

		void TBUI::SetFont(const Urho3D::String& fontName, int size)
		{
			tb::TBFontDescription fd;
			fd.SetID(tb::TBID(fontName.CString()));
			fd.SetSize(tb::g_tb_skin->GetDimensionConverter()->DpToPx(size));

			tb::g_font_manager->SetDefaultFontDescription(fd);
			tb::g_font_manager->CreateFontFace(tb::g_font_manager->GetDefaultFontDescription());
		}

		void TBUI::SetRoot(tb::TBWidget* widget)
		{
			auto graphics = GetSubsystem<Urho3D::Graphics>();

			_root = widget;
			_root->SetSize(graphics->GetWidth(), graphics->GetHeight());
		}

		tb::TBWidget* TBUI::GetWidget(const Urho3D::String& name, tb::TBWidget* parent)
		{
			return parent->GetWidgetByID(tb::TBID(name.CString()));
		}

		tb::TBWidget* TBUI::LoadWidget(const Urho3D::String& fileName, tb::TBWidget* widget)
		{
			auto cache = GetSubsystem<Urho3D::ResourceCache>();

			// Load widget
			tb::TBNode node;
			if (node.ReadFile(fileName.CString()))
			{
				tb::g_widgets_reader->LoadNodeTree(widget, &node);
			}

			return widget;
		}

		void TBUI::ResizeWidgetToFitContent(tb::TBWidget* widget, tb::TBWindow::RESIZE_FIT fit)
		{
			tb::PreferredSize ps = widget->GetPreferredSize();
			int new_w = ps.pref_w;
			int new_h = ps.pref_h;
			if (fit == tb::TBWindow::RESIZE_FIT_MINIMAL)
			{
				new_w = ps.min_w;
				new_h = ps.min_h;
			}
			else if (fit == tb::TBWindow::RESIZE_FIT_CURRENT_OR_NEEDED)
			{
				new_w = tb::Clamp(widget->GetRect().w, ps.min_w, ps.max_w);
				new_h = tb::Clamp(widget->GetRect().h, ps.min_h, ps.max_h);
			}
			if (widget->GetParent())
			{
				new_w = tb::Min(new_w, widget->GetParent()->GetRect().w);
				new_h = tb::Min(new_h, widget->GetParent()->GetRect().h);
			}
			widget->SetRect(tb::TBRect(widget->GetRect().x, widget->GetRect().y, new_w, new_h));
		}

		tb::TBBitmap* TBUI::CreateBitmap(int width, int height, tb::uint32* data)
		{
			auto bitmap = new Bitmap(context_);
			bitmap->Init(width, height, data);

			return bitmap;
		}

		void TBUI::RenderBatch(Batch* batch)
		{
			if (!batch->vertex_count)
				return;

			Urho3D::Texture2D* texture = NULL;

			if (batch->bitmap)
			{
				Bitmap* tbuibitmap = (Bitmap*)batch->bitmap;
				texture = tbuibitmap->texture;
			}

			Urho3D::PODVector<float> vertexData;
			Urho3D::SharedPtr<Urho3D::VertexBuffer> vertexBuffer(new Urho3D::VertexBuffer(context_));

			Urho3D::UIBatch newBatch;
			newBatch.blendMode_ = Urho3D::BLEND_ALPHA;
			newBatch.scissor_ = _scissor;
			newBatch.texture_ = texture;
			newBatch.vertexData_ = &vertexData;
			newBatch.invTextureSize_ = (texture ? Urho3D::Vector2(1.0f / (float)texture->GetWidth(), 1.0f / (float)texture->GetHeight()) : Urho3D::Vector2::ONE);

			unsigned begin = newBatch.vertexData_->Size();
			newBatch.vertexData_->Resize(begin + batch->vertex_count * Urho3D::UI_VERTEX_SIZE);
			float* dest = &(newBatch.vertexData_->At(begin));
			newBatch.vertexEnd_ = newBatch.vertexData_->Size();

			for (int i = 0; i < batch->vertex_count; i++)
			{
				Vertex* v = &batch->vertex[i];
				dest[0] = v->x; dest[1] = v->y; dest[2] = 0.0f;
				((unsigned&)dest[3]) = v->col;
				dest[4] = v->u; dest[5] = v->v;
				dest += Urho3D::UI_VERTEX_SIZE;
			}

			SetVertexData(vertexBuffer, vertexData);

			auto graphics = GetSubsystem<Urho3D::Graphics>();

			Urho3D::Vector2 invScreenSize(1.0f / (float)graphics->GetWidth(), 1.0f / (float)graphics->GetHeight());
			Urho3D::Vector2 scale(2.0f * invScreenSize.x_, -2.0f * invScreenSize.y_);
			Urho3D::Vector2 offset(-1.0f, 1.0f);

			Urho3D::Matrix4 projection(Urho3D::Matrix4::IDENTITY);
			projection.m00_ = scale.x_;
			projection.m03_ = offset.x_;
			projection.m11_ = scale.y_;
			projection.m13_ = offset.y_;
			projection.m22_ = 1.0f;
			projection.m23_ = 0.0f;
			projection.m33_ = 1.0f;

			graphics->ClearParameterSources();
			graphics->SetColorWrite(true);
			graphics->SetCullMode(Urho3D::CULL_NONE);
			graphics->SetDepthTest(Urho3D::CMP_ALWAYS);
			graphics->SetDepthWrite(false);
			graphics->SetFillMode(Urho3D::FILL_SOLID);
			graphics->SetStencilTest(false);

			graphics->ResetRenderTargets();

			graphics->SetVertexBuffer(vertexBuffer);

			Urho3D::ShaderVariation* noTextureVS = graphics->GetShader(Urho3D::VS, "Basic", "VERTEXCOLOR");
			Urho3D::ShaderVariation* diffTextureVS = graphics->GetShader(Urho3D::VS, "Basic", "DIFFMAP VERTEXCOLOR");
			Urho3D::ShaderVariation* noTexturePS = graphics->GetShader(Urho3D::PS, "Basic", "VERTEXCOLOR");
			Urho3D::ShaderVariation* diffTexturePS = graphics->GetShader(Urho3D::PS, "Basic", "DIFFMAP VERTEXCOLOR");
			Urho3D::ShaderVariation* diffMaskTexturePS = graphics->GetShader(Urho3D::PS, "Basic", "DIFFMAP ALPHAMASK VERTEXCOLOR");
			Urho3D::ShaderVariation* alphaTexturePS = graphics->GetShader(Urho3D::PS, "Basic", "ALPHAMAP VERTEXCOLOR");

			unsigned alphaFormat = Urho3D::Graphics::GetAlphaFormat();

			if (newBatch.vertexStart_ == newBatch.vertexEnd_)
				return;

			Urho3D::ShaderVariation* ps;
			Urho3D::ShaderVariation* vs;

			if (!newBatch.texture_)
			{
				ps = noTexturePS;
				vs = noTextureVS;
			}
			else
			{
				// If texture contains only an alpha channel, use alpha shader (for fonts)
				vs = diffTextureVS;

				if (newBatch.texture_->GetFormat() == alphaFormat)
					ps = alphaTexturePS;
				else if (newBatch.blendMode_ != Urho3D::BLEND_ALPHA && newBatch.blendMode_ != Urho3D::BLEND_ADDALPHA && newBatch.blendMode_ != Urho3D::BLEND_PREMULALPHA)
					ps = diffMaskTexturePS;
				else
					ps = diffTexturePS;
			}

			graphics->SetShaders(vs, ps);
			if (graphics->NeedParameterUpdate(Urho3D::SP_OBJECT, this))
				graphics->SetShaderParameter(Urho3D::VSP_MODEL, Urho3D::Matrix3x4::IDENTITY);
			if (graphics->NeedParameterUpdate(Urho3D::SP_CAMERA, this))
				graphics->SetShaderParameter(Urho3D::VSP_VIEWPROJ, projection);
			if (graphics->NeedParameterUpdate(Urho3D::SP_MATERIAL, this))
				graphics->SetShaderParameter(Urho3D::PSP_MATDIFFCOLOR, Urho3D::Color(1.0f, 1.0f, 1.0f, 1.0f));

			graphics->SetBlendMode(newBatch.blendMode_);
			graphics->SetScissorTest(true, newBatch.scissor_);
			graphics->SetTexture(0, newBatch.texture_);
			graphics->Draw(Urho3D::TRIANGLE_LIST, newBatch.vertexStart_ / Urho3D::UI_VERTEX_SIZE, (newBatch.vertexEnd_ - newBatch.vertexStart_) /
				Urho3D::UI_VERTEX_SIZE);
		}

		void TBUI::SetClipRect(const tb::TBRect& rect)
		{
			_scissor.top_ = rect.y;
			_scissor.left_ = rect.x;
			_scissor.bottom_ = rect.y + rect.h;
			_scissor.right_ = rect.x + rect.w;
		}

		void TBUI::SetupTurboBadger()
		{
			auto cache = GetSubsystem<Urho3D::ResourceCache>();
			auto graphics = GetSubsystem<Urho3D::Graphics>();

			tb::TBWidgetsAnimationManager::Init();
			tb::TBWidgetListener::AddGlobalListener(this);

			// Initialize Turbo Badger
			tb::tb_core_init(this);

			// Setup localization
			tb::g_tb_lng = new tb::TBLanguage;
			tb::g_tb_lng->Load("UI/Language/lng_en.tb.txt");

			// Setup font
			register_freetype_font_renderer();
			AddFontInfo("UI/vera.ttf", "Vera");
			SetFont("Vera");

			if (cache->Exists("UI/override_skin/skin.tb.txt"))
			{
				tb::g_tb_skin->Load("UI/default_skin/skin.tb.txt", "UI/override_skin/skin.tb.txt");
			}
			else
			{
				tb::g_tb_skin->Load("UI/default_skin/skin.tb.txt");
			}
		}

		void TBUI::SubscribeToEvents()
		{
			SubscribeToEvent(Urho3D::E_UPDATE, HANDLER(TBUI, HandleUpdate));
			SubscribeToEvent(Urho3D::E_ENDRENDERING, HANDLER(TBUI, HandleRenderUpdate));

			SubscribeToEvent(Urho3D::E_SCREENMODE, HANDLER(TBUI, HandleScreenMode));
			SubscribeToEvent(Urho3D::E_MOUSEBUTTONDOWN, HANDLER(TBUI, HandleMouseButtonDown));
			SubscribeToEvent(Urho3D::E_MOUSEBUTTONUP, HANDLER(TBUI, HandleMouseButtonUp));
			SubscribeToEvent(Urho3D::E_MOUSEMOVE, HANDLER(TBUI, HandleMouseMove));
			SubscribeToEvent(Urho3D::E_MOUSEWHEEL, HANDLER(TBUI, HandleMouseWheel));
			SubscribeToEvent(Urho3D::E_KEYDOWN, HANDLER(TBUI, HandleKeyDown));
			SubscribeToEvent(Urho3D::E_KEYUP, HANDLER(TBUI, HandleKeyUp));
			SubscribeToEvent(Urho3D::E_TEXTINPUT, HANDLER(TBUI, HandleTextInput));
		}

		void TBUI::SetVertexData(Urho3D::VertexBuffer* dest, const Urho3D::PODVector<float>& vertexData)
		{
			if (vertexData.Empty())
				return;

			// Update quad geometry into the vertex buffer
			// Resize the vertex buffer first if too small or much too large
			unsigned numVertices = vertexData.Size() / Urho3D::UI_VERTEX_SIZE;
			if (dest->GetVertexCount() < numVertices || dest->GetVertexCount() > numVertices * 2)
				dest->SetSize(numVertices, Urho3D::MASK_POSITION | Urho3D::MASK_COLOR | Urho3D::MASK_TEXCOORD1, true);

			dest->SetData(&vertexData[0]);
		}

		void TBUI::HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			tb::TBMessageHandler::ProcessMessages();
		}

		void TBUI::HandleRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			// Setup scissor
			tb::TBRect rect = _root->GetRect();
			_scissor = Urho3D::IntRect(0, 0, rect.w, rect.h);

			tb::TBAnimationManager::Update();

			// Update all widgets
			_root->InvokeProcessStates();
			_root->InvokeProcess();

			// Render UI
			tb::g_renderer->BeginPaint(rect.w, rect.h);

			_root->InvokePaint(tb::TBWidget::PaintProps());

			tb::g_renderer->EndPaint();

			//_root->Invalidate();
		}

		void TBUI::HandleScreenMode(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::ScreenMode;

			int width = eventData[P_WIDTH].GetInt();
			int height = eventData[P_HEIGHT].GetInt();

			_root->SetSize(width, height);
		}

		void TBUI::HandleMouseButtonDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::MouseButtonDown;

			unsigned button = eventData[P_BUTTON].GetUInt();
			unsigned qualifiers = eventData[P_QUALIFIERS].GetUInt();

			auto input = GetSubsystem<Urho3D::Input>();
			auto mousePosition = input->GetMousePosition();

			tb::MODIFIER_KEYS modifiers = tb::TB_MODIFIER_NONE;
			if (qualifiers & Urho3D::QUAL_CTRL)
				modifiers |= tb::TB_CTRL;
			if (qualifiers & Urho3D::QUAL_SHIFT)
				modifiers |= tb::TB_SHIFT;
			if (qualifiers & Urho3D::QUAL_ALT)
				modifiers |= tb::TB_ALT;

			switch (button)
			{
			case Urho3D::MOUSEB_LEFT:
				_root->InvokePointerDown(mousePosition.x_, mousePosition.y_, 1, modifiers, false);
				break;
			}

		}

		void TBUI::HandleMouseButtonUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::MouseButtonUp;

			unsigned button = eventData[P_BUTTON].GetUInt();
			unsigned qualifiers = eventData[P_QUALIFIERS].GetUInt();

			auto input = GetSubsystem<Urho3D::Input>();
			auto mousePosition = input->GetMousePosition();

			tb::MODIFIER_KEYS modifiers = tb::TB_MODIFIER_NONE;
			if (qualifiers & Urho3D::QUAL_CTRL)
				modifiers |= tb::TB_CTRL;
			if (qualifiers & Urho3D::QUAL_SHIFT)
				modifiers |= tb::TB_SHIFT;
			if (qualifiers & Urho3D::QUAL_ALT)
				modifiers |= tb::TB_ALT;

			switch (button)
			{
			case Urho3D::MOUSEB_LEFT:
				_root->InvokePointerUp(mousePosition.x_, mousePosition.y_, modifiers, false);
				break;
			case Urho3D::MOUSEB_RIGHT:
				_root->InvokeEvent(tb::TBWidgetEvent(tb::EVENT_TYPE_CONTEXT_MENU, mousePosition.x_, mousePosition.y_, false, modifiers));
				break;
			}
		}

		void TBUI::HandleMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::MouseMove;

			int x = eventData[P_X].GetInt();
			int y = eventData[P_Y].GetInt();

			unsigned qualifiers = eventData[P_QUALIFIERS].GetUInt();

			auto input = GetSubsystem<Urho3D::Input>();
			auto mousePosition = input->GetMousePosition();

			tb::MODIFIER_KEYS modifiers = tb::TB_MODIFIER_NONE;
			if (qualifiers & Urho3D::QUAL_CTRL)
				modifiers |= tb::TB_CTRL;
			if (qualifiers & Urho3D::QUAL_SHIFT)
				modifiers |= tb::TB_SHIFT;
			if (qualifiers & Urho3D::QUAL_ALT)
				modifiers |= tb::TB_ALT;

			_root->InvokePointerMove(mousePosition.x_, mousePosition.y_, modifiers, false);
			//_root->InvokeEvent(tb::TBWidgetEvent(tb::EVENT_TYPE_POINTER_MOVE, x, y, false));
		}

		void TBUI::HandleMouseWheel(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::MouseWheel;

			int wheel = eventData[P_WHEEL].GetInt();
			unsigned qualifiers = eventData[P_QUALIFIERS].GetInt();

			auto input = GetSubsystem<Urho3D::Input>();
			auto mousePosition = input->GetMousePosition();

			tb::MODIFIER_KEYS modifiers = tb::TB_MODIFIER_NONE;
			if (qualifiers & Urho3D::QUAL_CTRL)
				modifiers |= tb::TB_CTRL;
			if (qualifiers & Urho3D::QUAL_SHIFT)
				modifiers |= tb::TB_SHIFT;
			if (qualifiers & Urho3D::QUAL_ALT)
				modifiers |= tb::TB_ALT;

			_root->InvokeWheel(mousePosition.x_, mousePosition.y_, 0, -wheel, tb::TB_MODIFIER_NONE);
		}

		void TBUI::HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::KeyDown;

			int key = eventData[P_KEY].GetInt();
			unsigned qualifiers = eventData[P_QUALIFIERS].GetUInt();

			auto input = GetSubsystem<Urho3D::Input>();
			auto mousePosition = input->GetMousePosition();

			tb::MODIFIER_KEYS modifiers = tb::TB_MODIFIER_NONE;
			if (qualifiers & Urho3D::QUAL_CTRL)
				modifiers |= tb::TB_CTRL;
			if (qualifiers & Urho3D::QUAL_SHIFT)
				modifiers |= tb::TB_SHIFT;
			if (qualifiers & Urho3D::QUAL_ALT)
				modifiers |= tb::TB_ALT;

			switch (key)
			{
			case Urho3D::KEY_RETURN:
			case Urho3D::KEY_RETURN2:
			case Urho3D::KEY_KP_ENTER:
				_root->InvokeKey(0, tb::TB_KEY_ENTER, modifiers, true);
				break;
			case Urho3D::KEY_F1:
				_root->InvokeKey(0, tb::TB_KEY_F1, modifiers, true);
				break;
			case Urho3D::KEY_F2:
				_root->InvokeKey(0, tb::TB_KEY_F2, modifiers, true);
				break;
			case Urho3D::KEY_F3:
				_root->InvokeKey(0, tb::TB_KEY_F3, modifiers, true);
				break;
			case Urho3D::KEY_F4:
				_root->InvokeKey(0, tb::TB_KEY_F4, modifiers, true);
				break;
			case Urho3D::KEY_F5:
				_root->InvokeKey(0, tb::TB_KEY_F5, modifiers, true);
				break;
			case Urho3D::KEY_F6:
				_root->InvokeKey(0, tb::TB_KEY_F6, modifiers, true);
				break;
			case Urho3D::KEY_F7:
				_root->InvokeKey(0, tb::TB_KEY_F7, modifiers, true);
				break;
			case Urho3D::KEY_F8:
				_root->InvokeKey(0, tb::TB_KEY_F8, modifiers, true);
				break;
			case Urho3D::KEY_F9:
				_root->InvokeKey(0, tb::TB_KEY_F9, modifiers, true);
				break;
			case Urho3D::KEY_F10:
				_root->InvokeKey(0, tb::TB_KEY_F10, modifiers, true);
				break;
			case Urho3D::KEY_F11:
				_root->InvokeKey(0, tb::TB_KEY_F11, modifiers, true);
				break;
			case Urho3D::KEY_F12:
				_root->InvokeKey(0, tb::TB_KEY_F12, modifiers, true);
				break;
			case Urho3D::KEY_LEFT:
				_root->InvokeKey(0, tb::TB_KEY_LEFT, modifiers, true);
				break;
			case Urho3D::KEY_UP:
				_root->InvokeKey(0, tb::TB_KEY_UP, modifiers, true);
				break;
			case Urho3D::KEY_RIGHT:
				_root->InvokeKey(0, tb::TB_KEY_RIGHT, modifiers, true);
				break;
			case Urho3D::KEY_DOWN:
				_root->InvokeKey(0, tb::TB_KEY_DOWN, modifiers, true);
				break;
			case Urho3D::KEY_PAGEUP:
				_root->InvokeKey(0, tb::TB_KEY_PAGE_UP, modifiers, true);
				break;
			case Urho3D::KEY_PAGEDOWN:
				_root->InvokeKey(0, tb::TB_KEY_PAGE_DOWN, modifiers, true);
				break;
			case Urho3D::KEY_HOME:
				_root->InvokeKey(0, tb::TB_KEY_HOME, modifiers, true);
				break;
			case Urho3D::KEY_END:
				_root->InvokeKey(0, tb::TB_KEY_END, modifiers, true);
				break;
			case Urho3D::KEY_INSERT:
				_root->InvokeKey(0, tb::TB_KEY_INSERT, modifiers, true);
				break;
			case Urho3D::KEY_TAB:
				_root->InvokeKey(0, tb::TB_KEY_TAB, modifiers, true);
				break;
			case Urho3D::KEY_DELETE:
				_root->InvokeKey(0, tb::TB_KEY_DELETE, modifiers, true);
				break;
			case Urho3D::KEY_BACKSPACE:
				_root->InvokeKey(0, tb::TB_KEY_BACKSPACE, modifiers, true);
				break;
			case Urho3D::KEY_ESC:
				_root->InvokeKey(0, tb::TB_KEY_ESC, modifiers, true);
				break;
			default:
				if (modifiers & tb::TB_SUPER)
					_root->InvokeKey(key, tb::TB_KEY_UNDEFINED, modifiers, true);
			}
		}

		void TBUI::HandleKeyUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::KeyUp;

			int key = eventData[P_KEY].GetInt();
			unsigned qualifiers = eventData[P_QUALIFIERS].GetUInt();

			auto input = GetSubsystem<Urho3D::Input>();
			auto mousePosition = input->GetMousePosition();

			tb::MODIFIER_KEYS modifiers = tb::TB_MODIFIER_NONE;
			if (qualifiers & Urho3D::QUAL_CTRL)
				modifiers |= tb::TB_CTRL;
			if (qualifiers & Urho3D::QUAL_SHIFT)
				modifiers |= tb::TB_SHIFT;
			if (qualifiers & Urho3D::QUAL_ALT)
				modifiers |= tb::TB_ALT;

			switch (key)
			{
			case Urho3D::KEY_RETURN:
			case Urho3D::KEY_RETURN2:
			case Urho3D::KEY_KP_ENTER:
				_root->InvokeKey(0, tb::TB_KEY_ENTER, modifiers, false);
				break;
			case Urho3D::KEY_F1:
				_root->InvokeKey(0, tb::TB_KEY_F1, modifiers, false);
				break;
			case Urho3D::KEY_F2:
				_root->InvokeKey(0, tb::TB_KEY_F2, modifiers, false);
				break;
			case Urho3D::KEY_F3:
				_root->InvokeKey(0, tb::TB_KEY_F3, modifiers, false);
				break;
			case Urho3D::KEY_F4:
				_root->InvokeKey(0, tb::TB_KEY_F4, modifiers, false);
				break;
			case Urho3D::KEY_F5:
				_root->InvokeKey(0, tb::TB_KEY_F5, modifiers, false);
				break;
			case Urho3D::KEY_F6:
				_root->InvokeKey(0, tb::TB_KEY_F6, modifiers, false);
				break;
			case Urho3D::KEY_F7:
				_root->InvokeKey(0, tb::TB_KEY_F7, modifiers, false);
				break;
			case Urho3D::KEY_F8:
				_root->InvokeKey(0, tb::TB_KEY_F8, modifiers, false);
				break;
			case Urho3D::KEY_F9:
				_root->InvokeKey(0, tb::TB_KEY_F9, modifiers, false);
				break;
			case Urho3D::KEY_F10:
				_root->InvokeKey(0, tb::TB_KEY_F10, modifiers, false);
				break;
			case Urho3D::KEY_F11:
				_root->InvokeKey(0, tb::TB_KEY_F11, modifiers, false);
				break;
			case Urho3D::KEY_F12:
				_root->InvokeKey(0, tb::TB_KEY_F12, modifiers, false);
				break;
			case Urho3D::KEY_LEFT:
				_root->InvokeKey(0, tb::TB_KEY_LEFT, modifiers, false);
				break;
			case Urho3D::KEY_UP:
				_root->InvokeKey(0, tb::TB_KEY_UP, modifiers, false);
				break;
			case Urho3D::KEY_RIGHT:
				_root->InvokeKey(0, tb::TB_KEY_RIGHT, modifiers, false);
				break;
			case Urho3D::KEY_DOWN:
				_root->InvokeKey(0, tb::TB_KEY_DOWN, modifiers, false);
				break;
			case Urho3D::KEY_PAGEUP:
				_root->InvokeKey(0, tb::TB_KEY_PAGE_UP, modifiers, false);
				break;
			case Urho3D::KEY_PAGEDOWN:
				_root->InvokeKey(0, tb::TB_KEY_PAGE_DOWN, modifiers, false);
				break;
			case Urho3D::KEY_HOME:
				_root->InvokeKey(0, tb::TB_KEY_HOME, modifiers, false);
				break;
			case Urho3D::KEY_END:
				_root->InvokeKey(0, tb::TB_KEY_END, modifiers, false);
				break;
			case Urho3D::KEY_INSERT:
				_root->InvokeKey(0, tb::TB_KEY_INSERT, modifiers, false);
				break;
			case Urho3D::KEY_TAB:
				_root->InvokeKey(0, tb::TB_KEY_TAB, modifiers, false);
				break;
			case Urho3D::KEY_DELETE:
				_root->InvokeKey(0, tb::TB_KEY_DELETE, modifiers, false);
				break;
			case Urho3D::KEY_BACKSPACE:
				_root->InvokeKey(0, tb::TB_KEY_BACKSPACE, modifiers, false);
				break;
			case Urho3D::KEY_ESC:
				_root->InvokeKey(0, tb::TB_KEY_ESC, modifiers, false);
				break;
			default:
				if (modifiers & tb::TB_SUPER)
					_root->InvokeKey(key, tb::TB_KEY_UNDEFINED, modifiers, false);
			}
		}

		void TBUI::HandleTextInput(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
		{
			using namespace Urho3D::TextInput;

			auto text = eventData[P_TEXT].GetString();

			for (unsigned i = 0; i < text.Length(); i++)
			{
				_root->InvokeKey(text[i], tb::TB_KEY_UNDEFINED, tb::TB_MODIFIER_NONE, true);
				_root->InvokeKey(text[i], tb::TB_KEY_UNDEFINED, tb::TB_MODIFIER_NONE, false);
			}
		}
	}
}
