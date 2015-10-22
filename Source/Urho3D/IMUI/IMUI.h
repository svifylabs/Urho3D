#pragma once

#include "../Core/Object.h"
#include "imgui/imgui.h"
#include "../Core/Timer.h"

namespace Urho3D
{
	class Graphics;
	class VertexBuffer;
	class Cursor;
	class ResourceCache;
	class Timer;
	class UIBatch;
	class UIElement;
	class XMLElement;
	class XMLFile;
	class Input;
	class Texture;
	class Texture2D;
	class IndexBuffer;

	/// \todo: Input handler for Touch, Gesture, Joystick
	class IMUIContext : public Object
	{
		OBJECT(IMUIContext,Object);
	public:
		IMUIContext(Context* context);
		virtual	~IMUIContext();

		/// show debug menu bar. \todo: put in other class. 
		void ShowDebugMenuBar(bool show);
		bool IsDebugMenuBarVisible() const { return debugMenu_; }
	
		//////////////////////////////////////////////////////////////////////////
		///  Window
		///
		///		return false when window is collapsed, so you can early out in your code. 'bool* p_opened' creates a widget on the upper-right to close the window (which sets your bool to false).
		bool BeginWindow(const char* name = "Debug", bool* p_opened = NULL, ImGuiWindowFlags flags = 0);
		void EndWindow();
		///		size==0.0f: use remaining window size,
		///		size<0.0f: use remaining window size minus abs(size).
		///		size>0.0f: fixed size. each axis can use a different mode, e.g. Vector2(0,400).
		bool BeginChildWindow(const char* str_id, const Vector2& size = Vector2::ZERO, bool border = false, ImGuiWindowFlags extra_flags = 0);
		bool BeginChildWindow(ImGuiID id, const Vector2& size = Vector2::ZERO, bool border = false, ImGuiWindowFlags extra_flags = 0);
		void EndChildWindow();

		//////////////////////////////////////////////////////////////////////////
		///  Tooltip
		///
		///		set tooltip under mouse-cursor, typically use with ImGui::IsHovered(). last call wins
		void SetTooltip(const char* fmt, ...);
		///		use to create full-featured tooltip windows that aren't just text
		void BeginTooltip();
		void EndTooltip();

		//////////////////////////////////////////////////////////////////////////
		///  Popup
		///
		///		 mark popup as open. popup identifiers are relative to the current ID-stack (so OpenPopup and BeginPopup needs to be at the same level).
		///		close childs popups if any. will close popup when user click outside, or activate a pressable item, or CloseCurrentPopup()
		///		is called within a BeginPopup()/EndPopup() block.
		void OpenPopup(const char* str_id);
		//		return true if popup if opened and start outputting to it. only call EndPopup() if BeginPopup() returned true!
		bool BeginPopup(const char* str_id);
		//		open popup when clicked on last item
		bool BeginPopupContextItem(const char* str_id, int button = 1);
		//		open popup when clicked on current window
		bool BeginPopupContextWindow(bool in_empty_space_only = false, const char* str_id = "window_context_menu", int button = 1);
		//		open popup when clicked in void (no window)
		bool BeginPopupContextVoid(const char* str_id = "void_context_menu", int button = 1);
		void EndPopup();
		//		close the popup we have begin-ed into. clicking on a MenuItem or Selectable automatically close the current popup.
		void CloseCurrentPopup();


		//////////////////////////////////////////////////////////////////////////
		/// Widgets
		///
		void Text(const char* fmt, ...);
		void TextColored(const Vector4& col, const char* fmt, ...);               // shortcut for PushStyleColor(ImGuiCol_Text, col); Text(fmt, ...); PopStyleColor();
		void TextDisabled(const char* fmt, ...);                                 // shortcut for PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]); Text(fmt, ...); PopStyleColor();
		void TextWrapped(const char* fmt, ...);                                  // shortcut for PushTextWrapPos(0.0f); Text(fmt, ...); PopTextWrapPos();. Note that this won't work on an auto-resizing window if there's no other widgets to extend the window width, yoy may need to set a size using SetNextWindowSize().
		void TextUnformatted(const char* text, const char* text_end = NULL);     // doesn't require null terminated string if 'text_end' is specified. no copy done to any bounded stack buffer, recommended for long chunks of text
		void LabelText(const char* label, const char* fmt, ...);                 // display text+label aligned the same way as value+label widgets
		void Bullet();
		void BulletText(const char* fmt, ...);
		bool Button(const char* label, const Vector2& size = Vector2::ZERO);
		bool SmallButton(const char* label);
		bool InvisibleButton(const char* str_id, const Vector2& size);
		void Image(ImTextureID user_texture_id, const Vector2& size, const Vector2& uv0 = Vector2::ZERO, const Vector2& uv1 = Vector2(1, 1), const Vector4& tint_col = Vector4(1, 1, 1, 1), const Vector4& border_col = Vector4(0, 0, 0, 0));
		bool ImageButton(ImTextureID user_texture_id, const Vector2& size, const Vector2& uv0 = Vector2::ZERO, const Vector2& uv1 = Vector2(1, 1), int frame_padding = -1, const Vector4& bg_col = Vector4(0, 0, 0, 1), const Vector4& tint_col = Vector4(1, 1, 1, 1));    // <0 frame_padding uses default frame padding settings. 0 for no padding
		bool CollapsingHeader(const char* label, const char* str_id = NULL, bool display_frame = true, bool default_open = false);
		bool Checkbox(const char* label, bool* v);
		bool CheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value);
		bool RadioButton(const char* label, bool active);
		bool RadioButton(const char* label, int* v, int v_button);
		bool Combo(const char* label, int* current_item, const char** items, int items_count, int height_in_items = -1);
		bool Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items = -1);      // separate items with \0, end item-list with \0\0
		bool Combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1);
		bool ColorButton(const Vector4& col, bool small_height = false, bool outline_border = true);
		bool ColorEdit3(const char* label, float col[3]);
		bool ColorEdit4(const char* label, float col[4], bool show_alpha = true);
		void ColorEditMode(ImGuiColorEditMode mode);
		void PlotLines(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, Vector2 graph_size = Vector2::ZERO, size_t stride = sizeof(float));
		void PlotLines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, Vector2 graph_size = Vector2::ZERO);
		void PlotHistogram(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, Vector2 graph_size = Vector2::ZERO, size_t stride = sizeof(float));
		void PlotHistogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, Vector2 graph_size = Vector2::ZERO);

		//////////////////////////////////////////////////////////////////////////
		/// imgui render
		void RenderDrawLists(ImDrawData* data);

	protected:
		/// Initialize when screen mode initially set.
		void Initialize();
		/// Shutdown.
		void Shutdown();

		//////////////////////////////////////////////////////////////////////////
		/// render debug menu bar
		void RenderDebugMenuBar();
		void ShowEngineInfo(bool* opened = NULL);
		void ShowContextInfo(bool* opened = NULL);
		void ShowGraphicsInfo(bool* opened = NULL);
		void ShowRendererInfo(bool* opened = NULL);
		void ShowResourcesInfo(bool* opened = NULL);
		void ShowNetworkInfo(bool* opened = NULL);
		void ShowAudioInfo(bool* opened = NULL);
		void ShowSystemInfo(bool* opened = NULL);
		void ShowCPUProfilerkInfo(bool* opened = NULL);
		void ShowGPUProfilerInfo(bool* opened = NULL);
		void ShowMemoryInfo(bool* opened = NULL);

		//////////////////////////////////////////////////////////////////////////
		///
		/// Handle Rendering and Update Loop
		///
		/// Handle screen mode event.
		void HandleScreenMode(StringHash eventType, VariantMap& eventData);
		/// Handle frame begin event.
		void HandleBeginFrame(StringHash eventType, VariantMap& eventData);
		/// Handle end rendering event.
		void HandleEndRendering(StringHash eventType, VariantMap& eventData);
		/// Handle key event.
		void HandleKeyUp(StringHash eventType, VariantMap& eventData);
		/// Handle key event.
		void HandleKeyDown(StringHash eventType, VariantMap& eventData);
		/// Handle Text Input event.
		void HandleTextInput(StringHash eventType, VariantMap& eventData);

		/// Initialized flag.
		bool initialized_;
		/// show debug ui menu bar.
		bool debugMenu_;
		/// Graphics subsystem.
		Graphics* graphics_;
		/// Input subsystem.
		Input* input_;
		/// screen size
		IntRect screenSize_;
		/// imgui font Texture
		Texture2D* fontTexture_;
		/// UI vertex buffer.
		SharedPtr<VertexBuffer> vertexBuffer_;
		/// UI index buffer.
		SharedPtr<IndexBuffer> indexBuffer_;
		/// Profiler timer.
		Timer profilerTimer_;
	private:
	};
}
