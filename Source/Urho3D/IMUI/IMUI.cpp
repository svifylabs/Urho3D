#include "../Precompiled.h"

#include "../Core/Context.h"
#include "../Core/CoreEvents.h"
#include "../Core/ProcessUtils.h"
#include "../Core/Profiler.h"
#include "../Core/Timer.h"
#include "../Container/Sort.h"
#include "../Container/Vector.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/GraphicsEvents.h"
#include "../Graphics/Shader.h"
#include "../Graphics/ShaderVariation.h"
#include "../Graphics/Texture2D.h"
#include "../Graphics/VertexBuffer.h"
#include "../Graphics/VertexBuffer.h"
#include "../Input/Input.h"
#include "../Input/InputEvents.h"
#include "../IO/Log.h"
#include "../Math/Matrix3x4.h"
#include "../Resource/ResourceCache.h"
#include "../UI/CheckBox.h"
#include "../UI/Cursor.h"
#include "../UI/DropDownList.h"
#include "../UI/FileSelector.h"
#include "../UI/Font.h"
#include "../UI/LineEdit.h"
#include "../UI/ListView.h"
#include "../UI/MessageBox.h"
#include "../UI/ScrollBar.h"
#include "../UI/Slider.h"
#include "../UI/Sprite.h"
#include "../UI/Text.h"
#include "../UI/Text3D.h"
#include "../UI/ToolTip.h"
#include "../UI/UI.h"
#include "../UI/UIEvents.h"
#include "../UI/View3D.h"
#include "../UI/Window.h"
#include "../Resource/Image.h"

#include "IMUI.h"
#include "IMUIEvents.h"

#include "../Engine/Engine.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/RenderPath.h"
#include "../Scene/Scene.h"
#include "../Graphics/Camera.h"
#include "../Graphics/Viewport.h"
#include "../IO/PackageFile.h"
#include "../Network/Network.h"
#include "../Network/Connection.h"
#include "../Audio/Audio.h"
#include "../Graphics/IndexBuffer.h"

#include "../DebugNew.h"



namespace Urho3D
{
#define ToImVec2(v) ImVec2(v.x_,v.y_)
#define ToImVec4(v) ImVec4(v.x_,v.y_,v.z_,v.w_)

	static IMUIContext* g_imuiContext = NULL;
	static const char* ImGui_GetClipboardText()
	{
		return "";
	}

	static void ImGui_SetClipboardText(const char* text)
	{
	}
	// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
	// If text or lines are blurry when integrating ImGui in your engine:
	// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
	static void ImGui_RenderDrawLists(ImDrawData* data)
	{
		if (g_imuiContext)
		{
			g_imuiContext->RenderDrawLists(data);
		}
	}

	IMUIContext::IMUIContext(Context* context) : Object(context),
		initialized_(false),
		debugMenu_(false),
		graphics_(NULL),
		input_(NULL),
		fontTexture_(NULL)

	{
		SubscribeToEvent(E_SCREENMODE, URHO3D_HANDLER(IMUIContext, HandleScreenMode));
		// Try to initialize right now, but skip if screen mode is not yet set
		Initialize();
		g_imuiContext = this;
	}

	IMUIContext::~IMUIContext()
	{
		Shutdown();
	}

	void IMUIContext::Initialize()
	{
		Graphics* graphics = GetSubsystem<Graphics>();
		if (!graphics || !graphics->IsInitialized())
			return;
		graphics_ = graphics;
		screenSize_ = IntRect(0, 0, graphics->GetWidth(), graphics->GetHeight());
		vertexBuffer_ = new VertexBuffer(context_);
		indexBuffer_ = new IndexBuffer(context_);
		input_ = GetSubsystem<Input>();
		SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(IMUIContext, HandleBeginFrame));
		SubscribeToEvent(E_ENDRENDERING, URHO3D_HANDLER(IMUIContext, HandleEndRendering));

		SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(IMUIContext, HandleKeyUp));
		SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(IMUIContext, HandleKeyDown));
		SubscribeToEvent(E_TEXTINPUT, URHO3D_HANDLER(IMUIContext, HandleTextInput));

		
		//////////////////////////////////////////////////////////////////////////
		/// init imgui
		ImGuiIO& io = ImGui::GetIO();
		io.KeyMap[ImGuiKey_Tab] = SCANCODE_TAB;                 // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
		io.KeyMap[ImGuiKey_LeftArrow] = SCANCODE_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = SCANCODE_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = SCANCODE_UP;
		io.KeyMap[ImGuiKey_DownArrow] = SCANCODE_DOWN;
		io.KeyMap[ImGuiKey_Home] = SCANCODE_HOME;
		io.KeyMap[ImGuiKey_End] = SCANCODE_END;
		io.KeyMap[ImGuiKey_Delete] = SCANCODE_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = SCANCODE_BACKSPACE;
		io.KeyMap[ImGuiKey_Enter] = SCANCODE_RETURN;
		io.KeyMap[ImGuiKey_Escape] = SCANCODE_ESCAPE;
		io.KeyMap[ImGuiKey_A] = SCANCODE_A;
		io.KeyMap[ImGuiKey_C] = SCANCODE_C;
		io.KeyMap[ImGuiKey_V] = SCANCODE_V;
		io.KeyMap[ImGuiKey_X] = SCANCODE_X;
		io.KeyMap[ImGuiKey_Y] = SCANCODE_Y;
		io.KeyMap[ImGuiKey_Z] = SCANCODE_Z;

		io.RenderDrawListsFn = ImGui_RenderDrawLists;
		io.SetClipboardTextFn = ImGui_SetClipboardText;
		io.GetClipboardTextFn = ImGui_GetClipboardText;


		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.
		fontTexture_ = new Texture2D(context_);
		SharedPtr<Urho3D::Image> image(new Urho3D::Image(context_));
		image->SetSize(width, height, 4);
		image->SetData(pixels);
		fontTexture_->SetData(image, false);
		// Store our identifier
		io.Fonts->TexID = (void *)(intptr_t)fontTexture_;
		fontTexture_->SetFilterMode(FILTER_NEAREST);
		fontTexture_->SetAddressMode(COORD_U, ADDRESS_WRAP);
		fontTexture_->SetAddressMode(COORD_V, ADDRESS_WRAP);
		fontTexture_->SetAddressMode(COORD_W, ADDRESS_WRAP);


		URHO3D_LOGINFO("IMUI::Initialized");
	}

	void IMUIContext::Shutdown()
	{
		ImGui::Shutdown();
		if (fontTexture_)
		{
			delete fontTexture_;
			fontTexture_ = NULL;
		}
		URHO3D_LOGINFO("IMUI::Shutdown");
	}

	void IMUIContext::HandleScreenMode(StringHash eventType, VariantMap& eventData)
	{
		using namespace ScreenMode;

		if (!initialized_)
			Initialize();
		else
		{
			screenSize_ = IntRect(0, 0, eventData[P_WIDTH].GetInt(), eventData[P_HEIGHT].GetInt());
		}
	}

	void IMUIContext::HandleBeginFrame(StringHash eventType, VariantMap& eventData)
	{
		URHO3D_PROFILE(IMUI_BeginFrame);
		using namespace BeginFrame;
		float timeStep = eventData[P_TIMESTEP].GetFloat();

		// Setup display size (every frame to accommodate for window resizing)
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)graphics_->GetWidth(), (float)graphics_->GetHeight());

		// Setup time step
		io.DeltaTime = timeStep > 0.0f ? timeStep : 1.0f / 60.0f;

		// Setup inputs
		// (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
		if (input_->IsMouseVisible())
		{
			IntVector2 pos = input_->GetMousePosition();
			io.MousePos = ImVec2((float)pos.x_, (float)pos.y_);   // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
		}
		else
		{
			io.MousePos = ImVec2(-1, -1);
		}

		io.KeyCtrl = input_->GetQualifierDown(QUAL_CTRL);
		io.KeyShift = input_->GetQualifierDown(QUAL_SHIFT);
		io.KeyAlt = input_->GetQualifierDown(QUAL_ALT);

		io.MouseDown[0] = input_->GetMouseButtonDown(MOUSEB_LEFT);
		io.MouseDown[1] = input_->GetMouseButtonDown(MOUSEB_RIGHT);
		io.MouseDown[2] = input_->GetMouseButtonDown(MOUSEB_MIDDLE);
		io.MouseWheel = (float)input_->GetMouseMoveWheel();

		// Start the frame
		ImGui::NewFrame();
	}

	void IMUIContext::HandleEndRendering(StringHash eventType, VariantMap& eventData)
	{
		URHO3D_PROFILE(IMUI_Rendering);
		{
			URHO3D_PROFILE(IMUI_RenderEvent);
			using namespace IMUIRender;
			VariantMap& eventData = GetEventDataMap();
			eventData[P_IMUI] = this;
			SendEvent(E_IMUIRENDER, eventData);
		}
		{
			URHO3D_PROFILE(IMUI_Present);
			ImGui::Render();
		}	
	}

	void IMUIContext::HandleKeyUp(StringHash eventType, VariantMap& eventData)
	{
		using namespace KeyUp;

		ImGuiIO& io = ImGui::GetIO();
	//	int key = eventData[P_KEY].GetInt();
		int Scancode = eventData[P_SCANCODE].GetInt();
	//	int Qualifiers = eventData[P_QUALIFIERS].GetInt();
		if (Scancode < 512)
		io.KeysDown[Scancode] = false;

	}

	void IMUIContext::HandleKeyDown(StringHash eventType, VariantMap& eventData)
	{
		using namespace KeyDown;
		ImGuiIO& io = ImGui::GetIO();
	//	int key = eventData[P_KEY].GetInt();
		int Scancode = eventData[P_SCANCODE].GetInt();
	//	int Qualifiers = eventData[P_QUALIFIERS].GetInt();
		if (Scancode < 512)
		io.KeysDown[Scancode] = true;
	}

	void IMUIContext::HandleTextInput(StringHash eventType, VariantMap& eventData)
	{
		using namespace TextInput;
		const String& text = eventData[P_TEXT].GetString();
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharactersUTF8(text.CString());
	}

	void IMUIContext::RenderDrawLists(ImDrawData* data)
	{
		ImDrawList** const cmd_lists = data->CmdLists;
		int cmd_lists_count = data->CmdListsCount;

		// Engine does not render when window is closed or device is lost
		assert(graphics_ && graphics_->IsInitialized() && !graphics_->IsDeviceLost());

		if (cmd_lists_count == 0)
			return;

		Vector2 invScreenSize(1.0f / (float)graphics_->GetWidth(), 1.0f / (float)graphics_->GetHeight());
		Vector2 scale(2.0f * invScreenSize.x_, -2.0f * invScreenSize.y_);
		Vector2 offset(-1.0f, 1.0f);

		Matrix4 projection(Matrix4::IDENTITY);
		projection.m00_ = scale.x_;
		projection.m03_ = offset.x_;
		projection.m11_ = scale.y_;
		projection.m13_ = offset.y_;
		projection.m22_ = 1.0f;
		projection.m23_ = 0.0f;
		projection.m33_ = 1.0f;

		graphics_->ClearParameterSources();
		graphics_->SetColorWrite(true);
		graphics_->SetCullMode(CULL_NONE);
		graphics_->SetDepthTest(CMP_ALWAYS);
		graphics_->SetDepthWrite(false);
		graphics_->SetFillMode(FILL_SOLID);
		graphics_->SetStencilTest(false);
		graphics_->ResetRenderTargets();
		graphics_->SetBlendMode(BLEND_ALPHA);

		ShaderVariation* noTextureVS = graphics_->GetShader(VS, "IMGUI", "VERTEXCOLOR");
		ShaderVariation* diffTextureVS = graphics_->GetShader(VS, "IMGUI", "DIFFMAP VERTEXCOLOR");
		ShaderVariation* noTexturePS = graphics_->GetShader(PS, "IMGUI", "VERTEXCOLOR");
		ShaderVariation* diffTexturePS = graphics_->GetShader(PS, "IMGUI", "DIFFMAP VERTEXCOLOR");
		ShaderVariation* diffMaskTexturePS = graphics_->GetShader(PS, "IMGUI", "DIFFMAP ALPHAMASK VERTEXCOLOR");
		ShaderVariation* alphaTexturePS = graphics_->GetShader(PS, "IMGUI", "ALPHAMAP VERTEXCOLOR");

		unsigned alphaFormat = Graphics::GetAlphaFormat();


		/// resize buffers 
#if defined(URHO3D_D3D11)
		// TODO: for D3D11 : cannot set vertex/index buffer to dynamic. bug ? does it need to be dynamic ?
		if ((int)vertexBuffer_->GetVertexCount() < data->TotalVtxCount || (int)vertexBuffer_->GetVertexCount() > data->TotalVtxCount * 2)
			vertexBuffer_->SetSize(data->TotalVtxCount+1000, MASK_POSITION | MASK_COLOR | MASK_TEXCOORD1, false);

		if ((int)indexBuffer_->GetIndexCount() < data->TotalIdxCount || (int)indexBuffer_->GetIndexCount() > data->TotalIdxCount * 2)
			indexBuffer_->SetSize(data->TotalIdxCount+2000, false, false);
#else
		if ((int)vertexBuffer_->GetVertexCount() < data->TotalVtxCount || (int)vertexBuffer_->GetVertexCount() > data->TotalVtxCount * 2)
			vertexBuffer_->SetSize(data->TotalVtxCount + 1000, MASK_POSITION | MASK_COLOR | MASK_TEXCOORD1, true);

		if ((int)indexBuffer_->GetIndexCount() < data->TotalIdxCount || (int)indexBuffer_->GetIndexCount() > data->TotalIdxCount * 2)
			indexBuffer_->SetSize(data->TotalIdxCount + 2000, false, true);
#endif

		// Copy and convert all vertices into a single contiguous buffer

// 		int vtx_list_offset = 0;
// 		int idx_list_offset = 0;
// 		for (int n = 0; n < data->CmdListsCount; n++)
// 		{
// 			const ImDrawList* cmd_list = data->CmdLists[n];
// 
// 			vertexBuffer_->SetDataRange(&cmd_list->VtxBuffer[0], vtx_list_offset, cmd_list->VtxBuffer.size());
// 			indexBuffer_->SetDataRange(&cmd_list->IdxBuffer[0], idx_list_offset, cmd_list->IdxBuffer.size());
// 
// 			vtx_list_offset += cmd_list->VtxBuffer.size();
// 			idx_list_offset += cmd_list->IdxBuffer.size();
// 		}

#ifndef GL_ES_VERSION_2_0
		ImDrawVert* vtx_dst = (ImDrawVert*)vertexBuffer_->Lock(0, data->TotalVtxCount);	
		ImDrawIdx* idx_dst = (ImDrawIdx*)indexBuffer_->Lock(0, data->TotalIdxCount);
		for (int n = 0; n < data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = data->CmdLists[n];
			memcpy(vtx_dst, &cmd_list->VtxBuffer[0], cmd_list->VtxBuffer.size() * sizeof(ImDrawVert));
			memcpy(idx_dst, &cmd_list->IdxBuffer[0], cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx));
			vtx_dst += cmd_list->VtxBuffer.size();
			idx_dst += cmd_list->IdxBuffer.size();
		}
		vertexBuffer_->Unlock();
		indexBuffer_->Unlock();
#endif

		graphics_->SetVertexBuffer(vertexBuffer_);
		graphics_->SetIndexBuffer(indexBuffer_);


		// Render command lists
		int vtx_offset = 0;
		int idx_offset = 0;
		for (int n = 0; n < data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = data->CmdLists[n];
 #ifdef GL_ES_VERSION_2_0
			
			vertexBuffer_->SetDataRange(&cmd_list->VtxBuffer[0], 0, cmd_list->VtxBuffer.size());
			indexBuffer_->SetDataRange(&cmd_list->IdxBuffer[0], 0, cmd_list->IdxBuffer.size());

			idx_offset = 0;
#endif


			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				if (pcmd->UserCallback)
				{
					pcmd->UserCallback(cmd_list, pcmd);
				}
				else
				{
					Texture* texture = (Urho3D::Texture*)(intptr_t)pcmd->TextureId;
					ShaderVariation* ps;
					ShaderVariation* vs;
					if (!texture)
					{
						ps = noTexturePS;
						vs = noTextureVS;
					}
					else
					{
						vs = diffTextureVS;
						// If texture contains only an alpha channel, use alpha shader (for fonts)
						if (texture->GetFormat() == alphaFormat)
							ps = alphaTexturePS;
// 						else if (blendMode_ != BLEND_ALPHA && batch.blendMode_ != BLEND_ADDALPHA && batch.blendMode_ != BLEND_PREMULALPHA)
//			 						ps = diffMaskTexturePS;
						else
							ps = diffTexturePS;
					}

					graphics_->SetShaders(vs, ps);

					if (graphics_->NeedParameterUpdate(SP_OBJECT, this))
						graphics_->SetShaderParameter(VSP_MODEL, Matrix3x4::IDENTITY);
					if (graphics_->NeedParameterUpdate(SP_CAMERA, this))
						graphics_->SetShaderParameter(VSP_VIEWPROJ, projection);
					if (graphics_->NeedParameterUpdate(SP_MATERIAL, this))
						graphics_->SetShaderParameter(PSP_MATDIFFCOLOR, Color(1.0f, 1.0f, 1.0f, 1.0f));

 					graphics_->SetScissorTest(true, IntRect((int)pcmd->ClipRect.x, (int)(pcmd->ClipRect.y),
 											(int)(pcmd->ClipRect.z), (int)(pcmd->ClipRect.w)));

					graphics_->SetTexture(0, texture);
					// TODO: imgui needs base vertex index offset. implement for opengl 2 !
					/// for OpenGL : glDrawElementsBaseVertex only opengl 3.2
					/// for D3D11 :  void DrawIndexed( UINT IndexCount, UINT StartIndexLocation, INT  BaseVertexLocation);
					/// for D3D9 : DrawIndexedPrimitive(d3dPrimitiveType, baseVertexIndex, minVertexIndex, vertexCount, indexStart, primitiveCount);
			

#if defined(URHO3D_OPENGL)

#ifdef GL_ES_VERSION_2_0

					graphics_->Draw(TRIANGLE_LIST, idx_offset, pcmd->ElemCount, 0, cmd_list->VtxBuffer.size());
#else
					graphics_->Draw(TRIANGLE_LIST, idx_offset, pcmd->ElemCount, vtx_offset, 0, cmd_list->VtxBuffer.size());
#endif
				
#elif defined(URHO3D_D3D11)
					graphics_->Draw(TRIANGLE_LIST, idx_offset, pcmd->ElemCount, vtx_offset,0, cmd_list->VtxBuffer.size());
#else
					graphics_->Draw(TRIANGLE_LIST, idx_offset, pcmd->ElemCount, vtx_offset,0 , cmd_list->VtxBuffer.size());
#endif
				}
				idx_offset += pcmd->ElemCount;
			}
			vtx_offset += cmd_list->VtxBuffer.size();
		}
		
	}

}