#ifndef __KKROO_H__
#define __KKROO_H__

#include <exception>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <iostream>
#include <cassert>
#include <unordered_set>
#include <list>

#include "Common/kkConfig.h"
#include "Common/kkInfo.h"
#include "Common/kkTypes.h"
#include "Common/kkError.h"
#include "Memory/kkMemory.h"
#include "Classes/Common/kkAllocator.h"
#include "Classes/Containers/kkList.h"
#include "Classes/Containers/kkArray.h"
#include "Classes/Patterns/kkSingleton.h"
#include "Classes/Common/kkBaseObject.h"
#include "Classes/Common/kkRefObject.h"
#include "Classes/Common/kkPtr.h"
#include "Classes/kkColor.h"
#include "Classes/Math/kkVector4.h"
#include "Classes/Math/kkMatrix.h"
#include "Classes/Math/kkRay.h"
#include "Events/kkEvent.h"

#include "MainSystem/kkMainSystem.h"

#include "Common/kkForward.h"



extern "C"
{
	KK_API int  KK_C_DECL kkStartEngine(kkEventConsumer* ec = nullptr);
	KK_API void KK_C_DECL kkStopEngine();
	
	KK_API kkMainSystem*  KK_C_DECL kkGetMainSystem();
	KK_API kkSceneSystem* KK_C_DECL kkGetSceneSystem();
	KK_API kkInputSystem* KK_C_DECL kkGetInputSystem();
	KK_API kkEventSystem* KK_C_DECL kkGetEventSystem();

	KK_API kkCamera* KK_C_DECL kkCreateCamera();
	KK_API kkDummy*  KK_C_DECL kkCreateDummy();
}

/// Use only in .exe
struct kkCreator
{
	kkCreator()
	{
		if( kkStartEngine() == 0 )
		{
			KK_PRINT_FAILED;

			throw std::runtime_error("kkStartEngine = failed");
		}
	}

	kkCreator(kkEventConsumer* ec)
	{
		if( kkStartEngine(ec) == 0 )
		{
			KK_PRINT_FAILED;
			throw std::runtime_error("kkStartEngine = failed");
		}
	}


	~kkCreator()
	{
		kkStopEngine();
	}
};





#endif