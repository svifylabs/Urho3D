//
// Copyright (c) 2008-2015 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/ToolTip.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>

#include "../IMUI/IMUI.h"

#include "HelloIMUI.h"

#include <Urho3D/DebugNew.h>
#include "imgui/imgui.h"


DEFINE_APPLICATION_MAIN(HelloIMUI)

HelloIMUI::HelloIMUI(Context* context) :
    Sample(context)

{
}

void HelloIMUI::Setup()
{
	// Modify engine startup parameters
	engineParameters_["WindowTitle"] = GetTypeName();
	engineParameters_["LogName"] = GetTypeName() + ".log";
	engineParameters_["FullScreen"] = false;
	engineParameters_["Headless"] = false;
	engineParameters_["Sound"] = false;
}

void HelloIMUI::Start()
{
    // Execute base class startup
    Sample::Start();

    // Enable OS cursor
    GetSubsystem<Input>()->SetMouseVisible(true);

    // Load XML file containing default UI style sheet
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

	SubscribeToEvent(E_UPDATE, HANDLER(HelloIMUI, HandleUpdate));


}

void HelloIMUI::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	IMUIContext* imui = GetSubsystem<IMUIContext>();
	imui->ShowDebugMenuBar(true);
	return;

	imui->Text("Hallo world! ...");
	imui->Button("test", Vector2(100, 20));
	imui->TextColored(Vector4(1.0f,0.0f,0.0f,1.0f),"Hallo world! ...");
	imui->TextColored(Vector4(1.0f, 0.0f, 0.0f, 1.0f), "Hallo world! ...");
	imui->RadioButton("Double-click ",true);

	
	imui->BeginWindow("Hello");

	imui->Text("Hallo world! ...");
	imui->Button("test", Vector2(100,20));
	imui->EndWindow();

//	imui->BeginWindow("Hello2");
// 	imui->BulletText("Double-click on title bar to collapse window.");
// 	imui->BulletText("Click and drag on lower right corner to resize window.");
// 	imui->BulletText("Click and drag on any empty space to move window.");
// 	imui->BulletText("Mouse Wheel to scroll.");
// 
// 	imui->BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
// 	imui->BulletText("CTRL+Click on a slider or drag box to input text.");
// 	imui->BulletText(
// 		"While editing text:\n"
// 		"- Hold SHIFT or use mouse to select text\n"
// 		"- CTRL+Left/Right to word jump\n"
// 		"- CTRL+A or double-click to select all\n"
// 		"- CTRL+X,CTRL+C,CTRL+V clipboard\n"
// 		"- CTRL+Z,CTRL+Y undo/redo\n"
// 		"- ESCAPE to revert\n"
// 		"- You can apply arithmetic operators +,*,/ on numerical values.\n"
// 		"  Use +- to subtract.\n");
// 	imui->Text("Hallo world! ...");
// 	imui->Text("Hallo world! ...");
// 	imui->Button("test", Vector2(100, 20));
// 	imui->EndWindow();

	


// 	ImGui::ShowTestWindow();
// 	ImGui::ShowMetricsWindow();
}
