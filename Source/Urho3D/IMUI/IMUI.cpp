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

	void IMUIContext::ShowDebugMenuBar(bool show)
	{
		debugMenu_ = show;
	}

	bool IMUIContext::BeginWindow(const char* name /*= "Debug"*/, bool* p_opened /*= NULL*/, ImGuiWindowFlags flags /*= 0*/)
	{
		return ImGui::Begin(name, p_opened, flags);
	}

	void IMUIContext::EndWindow()
	{
		ImGui::End();
	}

	bool IMUIContext::BeginChildWindow(const char* str_id, const Vector2& size /*= ImVec2(0, 0)*/, bool border /*= false*/, ImGuiWindowFlags extra_flags /*= 0*/)
	{
		return ImGui::BeginChild(str_id, ToImVec2(size), border, extra_flags);
	}

	bool IMUIContext::BeginChildWindow(ImGuiID id, const Vector2& size /*= ImVec2(0, 0)*/, bool border /*= false*/, ImGuiWindowFlags extra_flags /*= 0*/)
	{
		return ImGui::BeginChild(id, ToImVec2(size), border, extra_flags);
	}

	void IMUIContext::EndChildWindow()
	{
		ImGui::EndChild();
	}

	void IMUIContext::SetTooltip(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		ImGui::SetTooltipV(fmt, args);
		va_end(args);
	}

	void IMUIContext::BeginTooltip()
	{
		ImGui::BeginTooltip();
	}

	void IMUIContext::EndTooltip()
	{
		ImGui::EndTooltip();
	}

	void IMUIContext::OpenPopup(const char* str_id)
	{
		ImGui::OpenPopup(str_id);
	}

	bool IMUIContext::BeginPopup(const char* str_id)
	{
		return ImGui::BeginPopup(str_id);
	}

	bool IMUIContext::BeginPopupContextItem(const char* str_id, int button /*= 1*/)
	{
		return ImGui::BeginPopupContextItem(str_id, button);
	}

	bool IMUIContext::BeginPopupContextWindow(bool in_empty_space_only /*= false*/, const char* str_id /*= "window_context_menu"*/, int button /*= 1*/)
	{
		return ImGui::BeginPopupContextWindow(in_empty_space_only, str_id, button);
	}

	bool IMUIContext::BeginPopupContextVoid(const char* str_id /*= "void_context_menu"*/, int button /*= 1*/)
	{
		return ImGui::BeginPopupContextVoid(str_id, button);
	}

	void IMUIContext::EndPopup()
	{
		ImGui::EndPopup();
	}

	void IMUIContext::CloseCurrentPopup()
	{
		ImGui::CloseCurrentPopup();
	}

	void IMUIContext::Text(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		ImGui::TextV(fmt, args);
		va_end(args);
	}

	void IMUIContext::TextColored(const Vector4& col, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		ImGui::TextColoredV(ToImVec4(col), fmt, args);
		va_end(args);
	}

	void IMUIContext::TextDisabled(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		ImGui::TextDisabledV(fmt, args);
		va_end(args);
	}

	void IMUIContext::TextWrapped(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		ImGui::TextWrappedV(fmt, args);
		va_end(args);
	}

	void IMUIContext::TextUnformatted(const char* text, const char* text_end /*= NULL*/)
	{
		ImGui::TextUnformatted(text, text_end);
	}

	void IMUIContext::LabelText(const char* label, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		ImGui::LabelText(label, fmt, args);
		va_end(args);
	}

	void IMUIContext::Bullet()
	{
		ImGui::Bullet();
	}

	void IMUIContext::BulletText(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		ImGui::BulletText(fmt, args);
		va_end(args);
	}

	bool IMUIContext::Button(const char* label, const Vector2& size /*= Vector2::ZERO*/)
	{
		return ImGui::Button(label, ToImVec2(size));
	}

	bool IMUIContext::SmallButton(const char* label)
	{
		return ImGui::SmallButton(label);
	}

	bool IMUIContext::InvisibleButton(const char* str_id, const Vector2& size)
	{
		return ImGui::InvisibleButton(str_id, ToImVec2(size));
	}

	void IMUIContext::Image(ImTextureID user_texture_id, const Vector2& size, const Vector2& uv0 /*= Vector2::ZERO*/, const Vector2& uv1 /*= Vector2(1, 1)*/, const Vector4& tint_col /*= Vector4(1, 1, 1, 1)*/, const Vector4& border_col /*= Vector4(0, 0, 0, 0)*/)
	{
		ImGui::Image(user_texture_id, ToImVec2(size), ToImVec2(uv0), ToImVec2(uv1), ToImVec4(tint_col), ToImVec4(border_col));
	}

	bool IMUIContext::ImageButton(ImTextureID user_texture_id, const Vector2& size, const Vector2& uv0 /*= Vector2::ZERO*/, const Vector2& uv1 /*= Vector2(1, 1)*/, int frame_padding /*= -1*/, const Vector4& bg_col /*= Vector4(0, 0, 0, 1)*/, const Vector4& tint_col /*= Vector4(1, 1, 1, 1)*/)
	{
		return ImGui::ImageButton(user_texture_id, ToImVec2(size), ToImVec2(uv0), ToImVec2(uv1), frame_padding, ToImVec4(bg_col), ToImVec4(tint_col));
	}

	bool IMUIContext::CollapsingHeader(const char* label, const char* str_id /*= NULL*/, bool display_frame /*= true*/, bool default_open /*= false*/)
	{
		return ImGui::CollapsingHeader(label, str_id, display_frame, default_open);
	}

	bool IMUIContext::Checkbox(const char* label, bool* v)
	{
		return ImGui::Checkbox(label, v);
	}

	bool IMUIContext::CheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value)
	{
		return ImGui::CheckboxFlags(label, flags, flags_value);
	}

	bool IMUIContext::RadioButton(const char* label, bool active)
	{
		return ImGui::RadioButton(label, active);
	}

	bool IMUIContext::RadioButton(const char* label, int* v, int v_button)
	{
		return ImGui::RadioButton(label, v, v_button);
	}

	bool IMUIContext::Combo(const char* label, int* current_item, const char** items, int items_count, int height_in_items /*= -1*/)
	{
		return ImGui::Combo(label, current_item, items, items_count, height_in_items);
	}

	bool IMUIContext::Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items /*= -1*/)
	{
		return ImGui::Combo(label, current_item, items_separated_by_zeros, height_in_items);
	}

	bool IMUIContext::Combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items /*= -1*/)
	{
		return ImGui::Combo(label, current_item, items_getter, data, items_count, height_in_items);
	}

	bool IMUIContext::ColorButton(const Vector4& col, bool small_height /*= false*/, bool outline_border /*= true*/)
	{
		return ImGui::ColorButton(ToImVec4(col), small_height, outline_border);
	}

	bool IMUIContext::ColorEdit3(const char* label, float col[3])
	{
		return ImGui::ColorEdit3(label, col);
	}

	bool IMUIContext::ColorEdit4(const char* label, float col[4], bool show_alpha /*= true*/)
	{
		return ImGui::ColorEdit4(label, col, show_alpha);
	}

	void IMUIContext::ColorEditMode(ImGuiColorEditMode mode)
	{
		ImGui::ColorEditMode(mode);
	}

	void IMUIContext::PlotLines(const char* label, const float* values, int values_count, int values_offset /*= 0*/, const char* overlay_text /*= NULL*/, float scale_min /*= FLT_MAX*/, float scale_max /*= FLT_MAX*/, Vector2 graph_size /*= Vector2::ZERO*/, size_t stride /*= sizeof(float)*/)
	{
		ImGui::PlotLines(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, ToImVec2(graph_size), stride);
	}

	void IMUIContext::PlotLines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset /*= 0*/, const char* overlay_text /*= NULL*/, float scale_min /*= FLT_MAX*/, float scale_max /*= FLT_MAX*/, Vector2 graph_size /*= Vector2::ZERO*/)
	{
		ImGui::PlotLines(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, ToImVec2(graph_size));
	}

	void IMUIContext::PlotHistogram(const char* label, const float* values, int values_count, int values_offset /*= 0*/, const char* overlay_text /*= NULL*/, float scale_min /*= FLT_MAX*/, float scale_max /*= FLT_MAX*/, Vector2 graph_size /*= Vector2::ZERO*/, size_t stride /*= sizeof(float)*/)
	{
		ImGui::PlotHistogram(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, ToImVec2(graph_size), stride);
	}

	void IMUIContext::PlotHistogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset /*= 0*/, const char* overlay_text /*= NULL*/, float scale_min /*= FLT_MAX*/, float scale_max /*= FLT_MAX*/, Vector2 graph_size /*= Vector2::ZERO*/)
	{
		ImGui::PlotHistogram(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, ToImVec2(graph_size));
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

	void IMUIContext::RenderDebugMenuBar()
	{
		static bool show_SystemInfo = false;
		static bool show_Resources = false;
		static bool show_Network = false;
		static bool show_Audio = false;
		static bool show_UI = false;
		static bool show_Graphics = false;
		static bool show_Renderer = false;
		static bool show_Engine = false;
		static bool show_Context = false;
		static bool show_ImGuiStyleEditor = false;
		static bool show_ImGuiMetricsWindow = false;
		static bool show_ImGuiUserGuide = false;
		static bool show_ImGuiTestWindow = false;

		static bool show_CPUProfiler = false;
		static bool show_GPUProfiler = false;
		static bool show_Memory = false;


		if (show_ImGuiUserGuide)
		{
			ImGui::Begin("ImGui User Guide", &show_ImGuiUserGuide);
			ImGui::ShowUserGuide();
			ImGui::End();
		}
		if (show_ImGuiMetricsWindow)
			ImGui::ShowMetricsWindow(&show_ImGuiMetricsWindow);
		if (show_ImGuiTestWindow)
			ImGui::ShowTestWindow(&show_ImGuiTestWindow);
		if (show_ImGuiStyleEditor)
		{
			ImGui::Begin("ImGui Style Editor", &show_ImGuiStyleEditor);
			ImGui::ShowStyleEditor();
			ImGui::End();
		}

		if (show_Engine)
			ShowEngineInfo(&show_Engine);
		if (show_Context)
			ShowContextInfo(&show_Context);
		if (show_Graphics)
			ShowGraphicsInfo(&show_Graphics);
		if (show_Renderer)
			ShowRendererInfo(&show_Renderer);
		if (show_Resources)
			ShowResourcesInfo(&show_Resources);
		if (show_Network)
			ShowNetworkInfo(&show_Network);
		if (show_Audio)
			ShowAudioInfo(&show_Audio);
		if (show_SystemInfo)
			ShowSystemInfo(&show_SystemInfo);

		if (show_CPUProfiler)
			ShowCPUProfilerkInfo(&show_CPUProfiler);
		if (show_GPUProfiler)
			ShowGPUProfilerInfo(&show_GPUProfiler);
		if (show_Memory)
			ShowMemoryInfo(&show_Memory);

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Debug"))
			{
				if (ImGui::BeginMenu("ImGui"))
				{
					ImGui::MenuItem("UserGuide", "", &show_ImGuiUserGuide);
					ImGui::MenuItem("MetricsWindow", "", &show_ImGuiMetricsWindow);
					ImGui::MenuItem("TestWindow", "", &show_ImGuiTestWindow);
					ImGui::MenuItem("StyleEditor", "", &show_ImGuiStyleEditor);
					ImGui::EndMenu();
				}

				ImGui::Separator();
				ImGui::MenuItem("System", "", &show_SystemInfo);
				ImGui::MenuItem("Engine", "", &show_Engine);
				ImGui::MenuItem("Context", "", &show_Context);
				ImGui::MenuItem("Graphics", "", &show_Graphics);
				ImGui::MenuItem("Renderer", "", &show_Renderer);
				ImGui::MenuItem("Resources", "", &show_Resources);
				ImGui::MenuItem("Network", "", &show_Network);
				ImGui::MenuItem("Audio", "", &show_Audio);


				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Profiler"))
			{
				ImGui::MenuItem("CPU Profiler", "", &show_CPUProfiler);
				ImGui::MenuItem("GPU Profiler", "", &show_GPUProfiler);
				ImGui::MenuItem("Memory", "", &show_Memory);
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void IMUIContext::ShowEngineInfo(bool* opened /*= NULL*/)
	{
		Engine* engine = GetSubsystem<Engine>();
		ImGui::Begin("Urho3D Engine Info", opened);

		ImGui::Value("NextTimeStep", engine->GetNextTimeStep());
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("timestep of the next frame. Updated by ApplyFrameLimit().");
		ImGui::Value("MinFps", engine->GetMinFps());
		ImGui::Value("MaxFps", engine->GetMaxFps());
		ImGui::Value("MaxInactiveFps", engine->GetMaxInactiveFps());
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("maximum frames per second when the application does not have input focus");
		ImGui::Value("TimeStepSmoothing", engine->GetTimeStepSmoothing());
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("how many frames to average for timestep smoothing");
		ImGui::Value("PauseMinimized", engine->GetPauseMinimized());
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("whether to pause update events and audio when minimized");
		ImGui::Value("AutoExit", engine->GetAutoExit());
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("whether to exit automatically on exit request");

		ImGui::End();
	}

	void IMUIContext::ShowContextInfo(bool* opened /*= NULL*/)
	{

		ImGui::Begin("Urho3D Context Info", opened);

		if (ImGui::CollapsingHeader("Subsystems"))
		{
			const HashMap<StringHash, SharedPtr<Object> >& subsystems = context_->GetSubsystems();
			const Vector< SharedPtr<Object> >& values = subsystems.Values();

			for (unsigned i = 0; i < values.Size(); i++)
			{
				ImGui::BulletText(values[i]->GetTypeName().CString());
			}

		}
		if (ImGui::CollapsingHeader("Object Factories"))
		{
			const HashMap<String, Vector<StringHash> >& objectCategories = context_->GetObjectCategories();
			HashMap<String, Vector<StringHash> >::ConstIterator it;

			for (it = objectCategories.Begin(); it != objectCategories.End(); it++)
			{
				ImGui::Text(it->first_.CString());
				for (unsigned i = 0; i < it->second_.Size(); i++)
				{
					ImGui::BulletText(context_->GetTypeName(it->second_[i]).CString());
				}
			}
		}

		ImGui::End();
	}

	void IMUIContext::ShowGraphicsInfo(bool* opened /*= NULL*/)
	{
		// 		/// Return supported fullscreen resolutions.
		// 		PODVector<IntVector2> GetResolutions() const;
		// 		/// Return supported multisampling levels.
		// 		PODVector<int> GetMultiSampleLevels() const;
		// 		/// Return the desktop resolution.
		// 		IntVector2 GetDesktopResolution() const;

		Graphics* graphics = GetSubsystem<Graphics>();
		ImGui::Begin("Urho3d Graphics Info", opened);
		ImGui::Text("Api Name : %s", graphics->GetApiName().CString());
		ImGui::Value("NumPrimitives", graphics->GetNumPrimitives());
		ImGui::Value("NumBatches", graphics->GetNumBatches());
		if (ImGui::CollapsingHeader("Window"))
		{
			ImGui::Text("WindowTitle : %s", graphics->GetWindowTitle().CString());
			ImGui::Text("Window Position : %s", graphics->GetWindowPosition().ToString().CString());
			ImGui::Text("Window Size : %d %d", graphics->GetWidth(), graphics->GetHeight());
			ImGui::Value("MultiSample", graphics->GetMultiSample());
			ImGui::Value("Fullscreen", graphics->GetFullscreen());
			ImGui::Value("Borderless", graphics->GetBorderless());
			ImGui::Value("Resizable", graphics->GetResizable());
			ImGui::Text("Orientations : %s", graphics->GetOrientations().CString());
		}
		if (ImGui::CollapsingHeader("Supported Features"))
		{
			ImGui::Value("MaxBones", Graphics::GetMaxBones());
			ImGui::Value("FlushGPU", graphics->GetFlushGPU());
			ImGui::Value("VSync", graphics->GetVSync());
			ImGui::Value("TripleBuffer", graphics->GetTripleBuffer());
			ImGui::Value("SRGB", graphics->GetSRGB());
			ImGui::Value("InstancingSupport", graphics->GetInstancingSupport());
			ImGui::Value("LightPrepassSupport", graphics->GetLightPrepassSupport());
			ImGui::Value("DeferredSupport", graphics->GetDeferredSupport());
			ImGui::Value("HardwareShadowSupport", graphics->GetHardwareShadowSupport());
			ImGui::Value("ReadableDepthSupport", graphics->GetReadableDepthSupport());
			ImGui::Value("SRGBSupport", graphics->GetSRGBSupport());
			ImGui::Value("SRGBWriteSupport", graphics->GetSRGBWriteSupport());
		}
		if (ImGui::CollapsingHeader("Supported Formats"))
		{
			ImGui::Value("DummyColorFormat", graphics->GetDummyColorFormat());
			ImGui::Value("ShadowMapFormat", graphics->GetShadowMapFormat());
			ImGui::Value("HiresShadowMapFormat", graphics->GetHiresShadowMapFormat());
		}

		if (ImGui::CollapsingHeader("Other Parameters"))
		{
			ImGui::Value("DepthConstantBias", graphics->GetDepthConstantBias());
			ImGui::Value("DepthSlopeScaledBias", graphics->GetDepthSlopeScaledBias());
		}



		ImGui::End();
	}

	void IMUIContext::ShowRendererInfo(bool* opened /*= NULL*/)
	{

		Renderer* renderer = GetSubsystem<Renderer>();
		ImGui::Begin("Urho3d Renderer Info", opened);
		ImGui::Value("NumViews", renderer->GetNumViews());
		ImGui::Value("NumPrimitives", renderer->GetNumPrimitives());
		ImGui::Value("NumBatches", renderer->GetNumBatches());
		ImGui::Value("NumGeometries", renderer->GetNumGeometries(true));
		ImGui::Value("NumLights", renderer->GetNumLights(true));
		ImGui::Value("NumShadowMaps", renderer->GetNumShadowMaps(true));
		ImGui::Value("NumOccluders", renderer->GetNumOccluders(true));
		ImGui::Value("NumViewports", renderer->GetNumViewports());
		if (ImGui::CollapsingHeader("Other Parameters"))
		{
			ImGui::Value("HDRRendering", renderer->GetHDRRendering());
			ImGui::Value("SpecularLighting", renderer->GetSpecularLighting());
			ImGui::Value("DynamicInstancing", renderer->GetDynamicInstancing());
			ImGui::Value("MinInstances", renderer->GetMinInstances());
			ImGui::Value("MaxSortedInstances", renderer->GetMaxSortedInstances());
			ImGui::Value("MaxOccluderTriangles", renderer->GetMaxOccluderTriangles());
			ImGui::Value("OcclusionBufferSize", renderer->GetOcclusionBufferSize());
			ImGui::Value("OccluderSizeThreshold", renderer->GetOccluderSizeThreshold());
			ImGui::Value("MobileShadowBiasMul", renderer->GetMobileShadowBiasMul());
			ImGui::Value("MobileShadowBiasAdd", renderer->GetMobileShadowBiasAdd());
		}
		if (ImGui::CollapsingHeader("Texture Shadow Parameters"))
		{
			ImGui::Value("TextureAnisotropy", renderer->GetTextureAnisotropy());
			ImGui::Value("TextureQuality", renderer->GetTextureQuality());
			ImGui::Value("TextureFilterMode", renderer->GetTextureFilterMode());
			ImGui::Separator();
			ImGui::Value("DrawShadows", renderer->GetDrawShadows());
			ImGui::Value("ShadowMapSize", renderer->GetShadowMapSize());
			ImGui::Value("ShadowQuality", renderer->GetShadowQuality());
			ImGui::Value("ReuseShadowMaps", renderer->GetReuseShadowMaps());
			ImGui::Value("MaxShadowMaps", renderer->GetMaxShadowMaps());
		}

		if (ImGui::CollapsingHeader("Viewport"))
		{
			for (unsigned i = 0; i < renderer->GetNumViewports(); i++)
			{
				Viewport* viewport = renderer->GetViewport(i);
				if (viewport)
				{
					ImGui::Text("Rect : %s", viewport->GetRect().ToString().CString());
					/// \todo scene info 
					ImGui::Text("Scene : %s", viewport->GetScene() ? viewport->GetScene()->GetName().CString() : "NULL");
					Camera* cam = viewport->GetCamera();
					if (cam)
					{
						/// \todo camera info 
						ImGui::Text("Camera : not NULL ^^ ");
					}
					else
						ImGui::Text("Camera : NULL");
					View* view = viewport->GetView();
					if (view)
					{

						/// \todo view info 
						ImGui::Text("view : not NULL ^^ ");
					}
					else
						ImGui::Text("view : NULL");
					ImGui::Separator();
					if (ImGui::TreeNode("RenderPath"))
					{
						RenderPath* renderPath = viewport->GetRenderPath();
						if (renderPath)
						{
							ImGui::Value("NumRenderTargets", renderPath->GetNumRenderTargets());
							ImGui::Value("NumCommands", renderPath->GetNumCommands());
							for (unsigned i = 0; i < renderPath->GetNumCommands(); i++)
							{
								RenderPathCommand* com = renderPath->GetCommand(i);
								if (com)
									if (ImGui::TreeNode(String(i).CString()))
									{
										ImGui::Text("tag : %s", com->tag_.CString());
										ImGui::Text("pass : %s", com->pass_.CString());
										ImGui::Text("DepthStencilName : %s", com->GetDepthStencilName().CString());
										ImGui::Text("metadata : %s", com->metadata_.CString());
										ImGui::Text("vertexShaderName : %s", com->vertexShaderName_.CString());
										ImGui::Text("pixelShaderName : %s", com->pixelShaderName_.CString());
										ImGui::Text("vertexShaderDefines : %s", com->vertexShaderDefines_.CString());
										ImGui::Text("pixelShaderDefines : %s", com->pixelShaderDefines_.CString());
										ImGui::Value("NumOutputs", com->GetNumOutputs());
										String str;
										for (unsigned i = 0; i < com->GetNumOutputs(); i++)
													str.Append(com->GetOutputName(i));
										
										ImGui::Text("NumOutputNames : %s", str.CString());
										ImGui::Separator();
										ImGui::Value("clearDepth", com->clearDepth_);
										ImGui::Value("clearStencil", com->clearStencil_);
										ImGui::Value("enabled", com->enabled_);
										ImGui::Value("useFogColor", com->useFogColor_);
										ImGui::Value("markToStencil", com->markToStencil_);
										ImGui::Value("useLitBase", com->useLitBase_);
										ImGui::Value("vertexLights", com->vertexLights_);

										ImGui::TreePop();
									}
							}

						}
						ImGui::TreePop();
					}
				}

			}

		}
		if (ImGui::CollapsingHeader("Default RenderPath"))
		{
			RenderPath* renderPath = renderer->GetDefaultRenderPath();
			if (renderPath)
			{
				ImGui::Value("NumRenderTargets", renderPath->GetNumRenderTargets());
				ImGui::Value("NumCommands", renderPath->GetNumCommands());
				for (unsigned i = 0; i < renderPath->GetNumCommands(); i++)
				{
					RenderPathCommand* com = renderPath->GetCommand(i);
					if (com)
						if (ImGui::TreeNode(String(i).CString()))
						{
							ImGui::Text("tag : %s", com->tag_.CString());
							ImGui::Text("pass : %s", com->pass_.CString());
							ImGui::Text("DepthStencilName : %s", com->GetDepthStencilName().CString());
							ImGui::Text("metadata : %s", com->metadata_.CString());
							ImGui::Text("vertexShaderName : %s", com->vertexShaderName_.CString());
							ImGui::Text("pixelShaderName : %s", com->pixelShaderName_.CString());
							ImGui::Text("vertexShaderDefines : %s", com->vertexShaderDefines_.CString());
							ImGui::Text("pixelShaderDefines : %s", com->pixelShaderDefines_.CString());
							ImGui::Value("NumOutputs", com->GetNumOutputs());
							String str;
							for (unsigned i = 0; i < com->GetNumOutputs(); i++)
								str.Append(com->GetOutputName(i));
							ImGui::Text("NumOutputNames : %s", str.CString());
							ImGui::Separator();
							ImGui::Value("clearDepth", com->clearDepth_);
							ImGui::Value("clearStencil", com->clearStencil_);
							ImGui::Value("enabled", com->enabled_);
							ImGui::Value("useFogColor", com->useFogColor_);
							ImGui::Value("markToStencil", com->markToStencil_);
							ImGui::Value("useLitBase", com->useLitBase_);
							ImGui::Value("vertexLights", com->vertexLights_);

							ImGui::TreePop();
						}
				}

			}


		}







		// 		/// Return backbuffer viewport by index.
		//		Viewport* GetViewport(unsigned index) const;
		// 		/// Return default renderpath.
		// 		RenderPath* GetDefaultRenderPath() const;


		ImGui::End();
	}

	void IMUIContext::ShowResourcesInfo(bool* opened /*= NULL*/)
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		ImGui::Begin("Urho3d ResourceCache Info", opened);


		ImGui::Text("TotalMemoryUse: %s", String(cache->GetTotalMemoryUse()).CString());
		ImGui::Value("AutoReloadResources", cache->GetAutoReloadResources());
		ImGui::Value("ReturnFailedResources", cache->GetReturnFailedResources());
		ImGui::Value("SearchPackagesFirst", cache->GetSearchPackagesFirst());
		ImGui::Value("FinishBackgroundResourcesMs", cache->GetFinishBackgroundResourcesMs());

		if (ImGui::CollapsingHeader("Resource Dirs"))
		{
			/// Return added resource load directories.
			const Vector<String>& dirs = cache->GetResourceDirs();
			for (unsigned i = 0; i < dirs.Size(); i++)
			{
				ImGui::BulletText(dirs[i].CString());
			}
		}
		if (ImGui::CollapsingHeader("Package Files"))
		{
			/// Return added resource load directories.
			const Vector<SharedPtr<PackageFile> >& packages = cache->GetPackageFiles();
			for (unsigned i = 0; i < packages.Size(); i++)
			{
				ImGui::BulletText(packages[i]->GetName().CString());
			}
		}

		// TODO: GetAllResources

		ImGui::End();
	}

	void IMUIContext::ShowNetworkInfo(bool* opened /*= NULL*/)
	{
		Network* network = GetSubsystem<Network>();
		ImGui::Begin("Urho3d Network Info", opened);
		ImGui::Value("UpdateFps", network->GetUpdateFps());
		ImGui::Value("SimulatedLatency", network->GetSimulatedLatency());
		ImGui::Value("SimulatedPacketLoss", network->GetSimulatedPacketLoss());
		ImGui::Value("Is Server Running", network->IsServerRunning());
		ImGui::Text("PackageCacheDir : %s", network->GetPackageCacheDir().CString());
		ImGui::Text("ServerConnection : %s", (network->GetServerConnection() ? "connected" : "disconnected"));
		ImGui::Value("GetClientConnections", network->GetClientConnections().Size());

		ImGui::End();
	}

	void IMUIContext::ShowAudioInfo(bool* opened /*= NULL*/)
	{
		Audio* audio = GetSubsystem<Audio>();


		ImGui::Begin("Urho3d Audio Info", opened);
		ImGui::Value("Is Initialized", audio->IsInitialized());
		ImGui::Value("Is Playing", audio->IsPlaying());
		ImGui::Value("Is Stereo", audio->IsStereo());

		ImGui::Value("SampleSize", audio->GetSampleSize());
		ImGui::Value("MixRate", audio->GetMixRate());
		ImGui::Value("Interpolation", audio->GetInterpolation());
		ImGui::Value("SoundSources", audio->GetSoundSources().Size());

		ImGui::Value("MASTER Gain", audio->GetMasterGain("MASTER"));

		ImGui::End();


	}

	void IMUIContext::ShowSystemInfo(bool* opened /*= NULL*/)
	{
		String str;
		str.Join(GetArguments(), " ,");
		ImGui::Begin("Urho3d System Info", opened);
		ImGui::Text("Platform : %s", GetPlatform().CString());
		ImGui::Value("Num Physical CPUs", GetNumPhysicalCPUs());
		ImGui::Value("Num Logical CPUs", GetNumLogicalCPUs());
		ImGui::Text("Parsed Arguments : %s", str.CString());
		// TODO: more cpu info, gpu info, memory info 
		ImGui::End();


	}

	void IMUIContext::ShowCPUProfilerkInfo(bool* opened /*= NULL*/)
	{
		static String profilerOutput("");

		static unsigned intervalFrames_ = 0;
		ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Urho3d CPU Profiler", opened);
		Profiler* profiler = GetSubsystem<Profiler>();
		if (profiler)
		{
			if (profilerTimer_.GetMSec(false) >= 1000)
			{
				profilerTimer_.Reset();
				profilerOutput = profiler->PrintData(false, false);
				profiler->BeginInterval();
				intervalFrames_ = 0;
			}
			ImGui::TextWrapped(profilerOutput.CString());
		}
		else
			ImGui::Text(" Profiler is not enabled ! ");

		ImGui::End();
	}

	void IMUIContext::ShowGPUProfilerInfo(bool* opened /*= NULL*/)
	{
		// TODO: gpu profiler
		ImGui::Begin("Urho3d GPU Profiler", opened);
		ImGui::Text(" not implemented yet ! :-/ ");
		ImGui::End();
	}

	void IMUIContext::ShowMemoryInfo(bool* opened /*= NULL*/)
	{
		// TODO: memory profiler
		ImGui::Begin("Urho3d Memory Profiler", opened);
		ImGui::Text(" not implemented yet ! :-/ ");
		ImGui::End();
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

		if (debugMenu_)
		{
			URHO3D_PROFILE(IMUI_DebugMenuBar);
			RenderDebugMenuBar();
		}
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

		graphics_->SetVertexBuffer(vertexBuffer_);
		graphics_->SetIndexBuffer(indexBuffer_);


		// Render command lists
		int vtx_offset = 0;
		int idx_offset = 0;
		for (int n = 0; n < data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = data->CmdLists[n];
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
					graphics_->Draw(TRIANGLE_LIST, idx_offset, pcmd->ElemCount, vtx_offset,0, cmd_list->VtxBuffer.size());
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