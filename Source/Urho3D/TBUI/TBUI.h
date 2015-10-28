#ifndef _TBUI_TBUI_H
#define _TBUI_TBUI_H


#include "../Core/Object.h"
#include "../Graphics/ShaderVariation.h"

#include <TurboBadger/tb_types.h>
#include <TurboBadger/tb_system.h>
#include <TurboBadger/tb_widgets_listener.h>
#include <TurboBadger/tb_window.h>
#include <TurboBadger/renderers/tb_renderer_batcher.h>

namespace Urho3D
{
	class File;
	class UIBatch;
	class VertexBuffer;
	class Texture2D;
}

namespace JRPG
{
	namespace UI
	{
		class Bitmap : public Urho3D::Object, public tb::TBBitmap
		{
			OBJECT(Bitmap, Urho3D::Object);

		public:
			Bitmap(Urho3D::Context* context);
			~Bitmap();

			static void RegisterObject(Urho3D::Context* context);

			void Init(int width, int height, tb::uint32 *data);

			virtual int Width() { return size.x_; }
			virtual int Height() { return size.y_; }

			virtual void SetData(tb::uint32 *data);

			Urho3D::Vector2 size;
			Urho3D::SharedPtr<Urho3D::Texture2D> texture;
		};

		class TBUrho3DFile : public tb::TBFile
		{
		public:
			TBUrho3DFile(Urho3D::File* file);
			virtual ~TBUrho3DFile();

			virtual long Size();
			virtual size_t Read(void *buf, size_t elemSize, size_t count);

		private:
			Urho3D::SharedPtr<Urho3D::File> _file;
		};

		// Turbo Badger UI subsystem
		class TBUI : public Urho3D::Object, public tb::TBRendererBatcher, public tb::TBWidgetListener
		{
			OBJECT(TBUI, Urho3D::Object);

		public:
			TBUI(Urho3D::Context* context);
			~TBUI();

			static void RegisterObject(Urho3D::Context* context);
			static void RegisterSystem(Urho3D::Context* context);

			void Init();

			void AddFontInfo(const Urho3D::String& fileName, const Urho3D::String& fontName);
			void SetFont(const Urho3D::String& fontName, int size = 12);

			tb::TBWidget* GetRoot() { return _root; }
			void SetRoot(tb::TBWidget* widget);

			tb::TBWidget* GetWidget(const Urho3D::String& name, tb::TBWidget* parent = 0);
			template<class T> T* GetWidget(const Urho3D::String& name, tb::TBWidget* parent = 0)
			{
				if (!parent)
					parent = _root;

				return static_cast<T*>(GetWidget(name, parent));
			}

			tb::TBWidget* LoadWidget(const Urho3D::String& fileName, tb::TBWidget* widget);
			template<class T> T* LoadWidget(const Urho3D::String& fileName)
			{
				T* newWidget = new T();
				return (T*)LoadWidget(fileName, newWidget);
			}

			void ResizeWidgetToFitContent(tb::TBWidget* widget, tb::TBWindow::RESIZE_FIT fit = tb::TBWindow::RESIZE_FIT_PREFERRED);

			// TB Renderer Batcher overrides
		public:

			tb::TBBitmap *CreateBitmap(int width, int height, tb::uint32* data) override;
			void RenderBatch(Batch* batch) override;
			void SetClipRect(const tb::TBRect& rect) override;

			// Internal functions
		protected:

			void SetupTurboBadger();

			void SubscribeToEvents();


		protected:

			void SetVertexData(Urho3D::VertexBuffer* dest, const Urho3D::PODVector<float>& vertexData);

		protected:

			void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

			void HandleScreenMode(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleMouseButtonDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleMouseButtonUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleMouseWheel(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleKeyUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
			void HandleTextInput(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		private:

			tb::TBWidget* _root;

			Urho3D::IntRect _scissor;
		};
	}
}
#endif
