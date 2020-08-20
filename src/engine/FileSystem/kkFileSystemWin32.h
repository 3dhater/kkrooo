// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_FILE_SYSTEM_WIN_32_H__
#define __KK_FILE_SYSTEM_WIN_32_H__

#ifdef KK_PLATFORM_WINDOWS

#include <Windows.h>

#include "kkFileSystemCommon.h"

class kkFileSystemWin32 : public kkFileSystemCommon
{

	bool		m_dirScanBegin = false;
	bool		m_firstCall = false;
	HANDLE		hFind = nullptr;
	kkString	m_dir;
	kkString	m_exePath, m_systemPath;

	void		deleteFolder( const kkString& dir );

public:

	kkFileSystemWin32();
	virtual ~kkFileSystemWin32();


	kkFile* createFile( const kkString& fileName, 
		kkFileMode mode,
		kkFileAccessMode access,
		kkFileAction action,
		kkFileShareMode EFSM,
		u32 EFA
	);

	bool		deleteFile( const kkString& fileName );
	bool		deleteDir( const kkString& dir );
	bool		existFile( const kkString& fileName );
	bool		existDir( const kkString& dir );
	bool		createDir( const kkString& dir );
	void		scanDirBegin( kkString dir );
	void		scanDirEnd();
	bool		getDirObject( kkFileSystem::DirObject* );
	bool		copyFile( const kkString& existingFileName, const kkString& newFileName, bool overwrite );
	kkString	getProgramPath();
	kkString	getSystemPath();
	kkString	getRealPath( const kkString& in );
};

#endif
#endif
