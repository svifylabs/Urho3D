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
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/EngineEvents.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/UI/Button.h>

#include "RuntimeCompiledCpp.h"


#include "Urho3D/RCCPP/RuntimeCompiler/AUArray.h"
#include "Urho3D/RCCPP/RuntimeCompiler/BuildTool.h"
#include "Urho3D/RCCPP/RuntimeCompiler/ICompilerLogger.h"
#include "Urho3D/RCCPP/RuntimeCompiler/FileChangeNotifier.h"
#include "Urho3D/RCCPP/RuntimeObjectSystem/IObjectFactorySystem.h"
#include "Urho3D/RCCPP/RuntimeObjectSystem/ObjectFactorySystem/ObjectFactorySystem.h"
#include "Urho3D/RCCPP/RuntimeObjectSystem/RuntimeObjectSystem.h"
#include "Urho3D/RCCPP/RuntimeObjectSystem/IObject.h"

#include "IUpdateable.h"
#include "InterfaceIds.h"
#include "../DebugNew.h"
// Remove windows.h define of GetObject which conflicts with EntitySystem GetObject
#if defined _WINDOWS_ && defined GetObject
#undef GetObject
#endif


struct Uroh3dCompilerLogger : ICompilerLogger
{
	virtual void LogError(const char * format, ...)
	{
		String ret("RCCpp: ");
		va_list args;
		va_start(args, format);
		ret.AppendWithFormatArgs(format, args);
		URHO3D_LOGERROR(ret);
		va_end(args);

	}
	virtual void LogWarning(const char * format, ...)
	{
		String ret("RCCpp: ");
		va_list args;
		va_start(args, format);
		ret.AppendWithFormatArgs(format, args);
		URHO3D_LOGWARNING(ret);
		va_end(args);

	}
	virtual void LogInfo(const char * format, ...)
	{
		String ret("RCCpp: ");
		va_list args;
		va_start(args, format);
		ret.AppendWithFormatArgs(format, args);
		URHO3D_LOGINFO(ret);
		va_end(args);

	}
	virtual ~Uroh3dCompilerLogger() {}
};

// Expands to this example's entry-point
URHO3D_DEFINE_APPLICATION_MAIN(RuntimeCompiledCpp)

RuntimeCompiledCpp::RuntimeCompiledCpp(Context* context) :
    Sample(context)
	, m_pCompilerLogger(0)
	, m_pRuntimeObjectSystem(0)
	, m_pUpdateable(0)
{
}

RuntimeCompiledCpp::~RuntimeCompiledCpp()
{
	if (m_pRuntimeObjectSystem)
	{
		// clean temp object files
		m_pRuntimeObjectSystem->CleanObjectFiles();
	}

	if (m_pRuntimeObjectSystem && m_pRuntimeObjectSystem->GetObjectFactorySystem())
	{
		m_pRuntimeObjectSystem->GetObjectFactorySystem()->RemoveListener(this);

		// delete object via correct interface
		IObject* pObj = m_pRuntimeObjectSystem->GetObjectFactorySystem()->GetObject(m_ObjectId);
		delete pObj;
	}

	delete m_pRuntimeObjectSystem;
	delete m_pCompilerLogger;
}

void RuntimeCompiledCpp::Start()
{
	OpenConsoleWindow();
    // Execute base class startup
    Sample::Start();


    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(RuntimeCompiledCpp, HandleUpdate));


    // Hide logo to make room for the console
    SetLogoVisible(false);


	//Initialise the RuntimeObjectSystem
	m_pRuntimeObjectSystem = new RuntimeObjectSystem();
	m_pCompilerLogger = new Uroh3dCompilerLogger();
	if (!m_pRuntimeObjectSystem->Initialise(m_pCompilerLogger, 0))
	{
		m_pRuntimeObjectSystem = 0;
	}
	else
	{
		m_pRuntimeObjectSystem->GetObjectFactorySystem()->AddListener(this);

//		m_pRuntimeObjectSystem->AddIncludeDir("C:/Users/scorvi/Documents/GitHub/libs/RuntimeCompiledCPlusPlus-master/RuntimeCompiledCPlusPlus-master/Aurora");
		
		/// add Urho3d Build include dir
		m_pRuntimeObjectSystem->AddIncludeDir("C:/SvifyLabs/Engine/Build/Features/include");
		/// add Urho3d ThirdParty dir so that "ObjectInterfacePerModuleSource.cpp" finds "../DebugNew.h"
		m_pRuntimeObjectSystem->AddIncludeDir("C:/SvifyLabs/Engine/Build/Features/include/Urho3D/ThirdParty");
		// construct first object
		IObjectConstructor* pCtor = m_pRuntimeObjectSystem->GetObjectFactorySystem()->GetConstructor("RuntimeObject01");
		if (pCtor)
		{
			IObject* pObj = pCtor->Construct();
			pObj->GetInterface(&m_pUpdateable);
			if (0 == m_pUpdateable)
			{
				delete pObj;
				m_pCompilerLogger->LogError("Error - no updateable interface found\n");
				return ;
			}
			m_ObjectId = pObj->GetObjectId();

		}
	}
}



void RuntimeCompiledCpp::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	using namespace Update;
	// Take the frame time step, which is stored as a float
	float timeStep = eventData[P_TIMESTEP].GetFloat();

	//check status of any compile
	if (m_pRuntimeObjectSystem->GetIsCompiledComplete())
	{
		// load module when compile complete
		m_pRuntimeObjectSystem->LoadCompiledModule();
	}

	if (!m_pRuntimeObjectSystem->GetIsCompiling())
	{	
		m_pRuntimeObjectSystem->GetFileChangeNotifier()->Update(timeStep);
		m_pUpdateable->Update(timeStep);
	}
}

void RuntimeCompiledCpp::OnConstructorsAdded()
{
	// This could have resulted in a change of object pointer, so release old and get new one.
	if (m_pUpdateable)
	{
		IObject* pObj = m_pRuntimeObjectSystem->GetObjectFactorySystem()->GetObject(m_ObjectId);
		pObj->GetInterface(&m_pUpdateable);
		if (0 == m_pUpdateable)
		{
			delete pObj;
			m_pCompilerLogger->LogError("Error - no updateable interface found\n");
		}
	}
}

