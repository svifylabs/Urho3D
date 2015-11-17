#include "Urho3D/RCCPP/RuntimeObjectSystem/ObjectInterfacePerModule.h"

#include "Urho3D/RCCPP/RuntimeObjectSystem/IObject.h"
#include "IUpdateable.h"
#include "InterfaceIds.h"
#include <iostream>


class RuntimeObject01 : public TInterface<IID_IUPDATEABLE,IUpdateable>
{
public:
	virtual void Update( float deltaTime )
	{		
		count +=deltaTime;
		if (count > 1.0f)
		{
			std::cout << "Runtime Object 01 update called!\n";
			count = 0.0f;
		}	
	}
	float count = 0.0f;
};

REGISTERCLASS(RuntimeObject01);
