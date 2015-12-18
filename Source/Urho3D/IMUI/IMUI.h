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

	// TODO: Input handler for Touch, Gesture, Joystick
	// TODO: Replace font handling with Urho3d font
	// TODO: imgui saves window positions and sizes to an ini file, use urho3d file system for that.
	class IMUIContext : public Object
	{
		URHO3D_OBJECT(IMUIContext, Object);
	public:
		IMUIContext(Context* context);
		virtual	~IMUIContext();
		/// imgui render
		void RenderDrawLists(ImDrawData* data);
	protected:
		/// Initialize when screen mode initially set.
		void Initialize();
		/// Shutdown.
		void Shutdown();
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
