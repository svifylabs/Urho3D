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



#include "TBImplementation.h"

#include "../TBUI/TBUI.h"

#include <Urho3D/DebugNew.h>
#include "TurboBadger/tb_widgets.h"




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
	// Setup Turbo Badger UI
	JRPG::UI::TBUI::RegisterSystem(context_);
	context_->RegisterSubsystem(new JRPG::UI::TBUI(context_));
	JRPG::UI::TBUI* tbui = 	context_->GetSubsystem<JRPG::UI::TBUI>();
	tbui->Init();

	tb::TBWidget* _editorUI = tbui->LoadWidget<tb::TBWidget>("UI/resources/about.tb.txt");
	tbui->SetRoot(_editorUI);

    // Enable OS cursor
    GetSubsystem<Input>()->SetMouseVisible(true);

	SubscribeToEvent(E_UPDATE, HANDLER(HelloIMUI, HandleUpdate));

}

void HelloIMUI::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	

	


}
