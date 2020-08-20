// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_FILE_SYSTEM_COMMON_H__
#define __KK_FILE_SYSTEM_COMMON_H__

class kkFileSystemCommon
{
public:
	kkFileSystemCommon();
	virtual ~kkFileSystemCommon();


	virtual kkFile* createFile( const kkString& fileName,
		kkFileMode mode,
		kkFileAccessMode access,
		kkFileAction action,
		kkFileShareMode EFSM,
		u32 EFA
	) = 0;


	virtual bool		deleteFile( const kkString& fileName ) = 0;
	virtual bool		deleteDir( const kkString& dir ) = 0;
	virtual bool		existFile( const kkString& fileName ) = 0;
	virtual bool		existDir( const kkString& dir ) = 0;
	virtual bool		createDir( const kkString& dir ) = 0;
	virtual void		scanDirBegin( kkString dir ) = 0;
	virtual void		scanDirEnd() = 0;
	virtual bool		getDirObject( kkFileSystem::DirObject* ) = 0;
	virtual bool		copyFile( const kkString& existingFileName, const kkString& newFileName, bool overwrite ) = 0;
	virtual kkString	getProgramPath() = 0;
	virtual kkString	getSystemPath() = 0;
	virtual kkString	getRealPath( const kkString& in ) = 0;
};

#endif