// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS

#include "kkrooo.engine.h"

#include "Classes/Strings/kkString.h"
#include "Classes/Patterns/kkSingleton.h"

#include "FileSystem/kkFileSystem.h"

#include "kkFileSystemCommon.h"
#ifdef KK_PLATFORM_WINDOWS
#include "kkFileSystemWin32.h"
#else
#error NEED IMPLEMENT!!!!
#endif


template<>
kkFileSystemCommon * kkSingleton<kkFileSystemCommon>::s_instance = nullptr;

kkFileSystemCommon::kkFileSystemCommon(){}
kkFileSystemCommon::~kkFileSystemCommon(){}

void kkFileSystem_init()
{
	if( !kkSingleton<kkFileSystemCommon>::s_instance )
	{
#ifdef KK_PLATFORM_WINDOWS
		kkSingleton<kkFileSystemCommon>::s_instance = new kkFileSystemWin32;
#else
#error NEED IMPLEMENT!!!!
#endif
	}
}

kkFile* kkFileSystem::createFile( const kkString& fileName, 
			kkFileMode mode,
			kkFileAccessMode access,
			kkFileAction action,
			kkFileShareMode EFSM,
			u32 EFA
		)
{
	kkFileSystem_init();
	return kkSingleton<kkFileSystemCommon>::s_instance->createFile( fileName, mode, access, action, EFSM, EFA );
}

bool kkFileSystem::deleteFile( const kkString& fileName )
{
	kkFileSystem_init();
	return kkSingleton<kkFileSystemCommon>::s_instance->deleteFile( fileName );
}

bool kkFileSystem::deleteDir( const kkString& dir )
{
	kkFileSystem_init();
	return kkSingleton<kkFileSystemCommon>::s_instance->deleteDir( dir );
}

bool kkFileSystem::existFile( const kkString& fileName )
{
	kkFileSystem_init();
	return kkSingleton<kkFileSystemCommon>::s_instance->existFile( fileName );
}

bool kkFileSystem::existDir( const kkString& dir )
{
	kkFileSystem_init();
	return kkSingleton<kkFileSystemCommon>::s_instance->existDir( dir );
}

bool kkFileSystem::createDir( const kkString& dir )
{
	kkFileSystem_init();
	return kkSingleton<kkFileSystemCommon>::s_instance->createDir( dir );
}

void kkFileSystem::scanDirBegin( kkString dir )
{
	kkFileSystem_init();
	kkString dir2 = dir;
	kkSingleton<kkFileSystemCommon>::s_instance->scanDirBegin( dir2 );
}

void kkFileSystem::scanDirEnd()
{
	kkFileSystem_init();
	kkSingleton<kkFileSystemCommon>::s_instance->scanDirEnd();
}

bool kkFileSystem::getDirObject( kkFileSystem::DirObject* o )
{
	kkFileSystem_init();
	return kkSingleton<kkFileSystemCommon>::s_instance->getDirObject( o );
}

bool kkFileSystem::copyFile( const kkString& existingFileName, const kkString& newFileName, bool overwrite )
{
	kkFileSystem_init();
	return kkSingleton<kkFileSystemCommon>::s_instance->copyFile( existingFileName, newFileName, overwrite );
}

kkString kkFileSystem::getProgramPath()
{
	kkFileSystem_init();
	return kkSingleton<kkFileSystemCommon>::s_instance->getProgramPath();
}

kkString kkFileSystem::getSystemPath()
{
	kkFileSystem_init();
	return kkSingleton<kkFileSystemCommon>::s_instance->getSystemPath();
}

kkString kkFileSystem::getRealPath( const kkString& in )
{
	kkFileSystem_init();
	return kkSingleton<kkFileSystemCommon>::s_instance->getRealPath( in );
}
