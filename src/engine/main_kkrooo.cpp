// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS

#include "kkrooo.engine.h"

#include "Classes/Patterns/kkSingleton.h"

#include "Memory/kkMemorySystemImpl.h"
#include "MainSystem/kkMainSystemImpl.h"

template<>
kkMemorySystemImpl* kkSingleton<kkMemorySystemImpl>::s_instance = nullptr;

template<>
kkMainSystemImpl* kkSingleton<kkMainSystemImpl>::s_instance = nullptr;




extern "C"
{
	KK_API int KK_C_DECL kkStartEngine(kkEventConsumer* ec)
	{
		
		kkSingleton<kkMemorySystemImpl>::s_instance = new kkMemorySystemImpl;
		kkSingleton<kkMainSystemImpl>::s_instance = new kkMainSystemImpl(ec);


		return 1;
	}

	KK_API void KK_C_DECL kkStopEngine()
	{
		delete kkSingleton<kkMainSystemImpl>::s_instance;

		delete kkSingleton<kkMemorySystemImpl>::s_instance;
	}

	KK_API kkMainSystem* KK_C_DECL kkGetMainSystem()
	{
		return kkSingleton<kkMainSystemImpl>::s_instance;
	}

	KK_API kkSceneSystem* KK_C_DECL kkGetSceneSystem()
	{
		return kkSingleton<kkMainSystemImpl>::s_instance->getSceneSystem();
	}

	KK_API kkInputSystem* KK_C_DECL kkGetInputSystem()
	{
		return kkSingleton<kkMainSystemImpl>::s_instance->getInputSystem();
	}

	KK_API kkEventSystem* KK_C_DECL kkGetEventSystem()
	{
		return kkSingleton<kkMainSystemImpl>::s_instance->getEventSystem();
	}

	KK_API kkCamera* KK_C_DECL kkCreateCamera()
	{
		return kkGetSceneSystem()->createCamera();
	}

	KK_API kkDummy* KK_C_DECL kkCreateDummy()
	{
		return kkGetSceneSystem()->createDummy();
	}


}

