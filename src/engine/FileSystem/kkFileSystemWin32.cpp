// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS
#include "kkrooo.engine.h"

#if defined(KK_PLATFORM_WINDOWS)

#include "Classes/Strings/kkString.h"
#include "FileSystem/kkFileSystem.h"

#include "Common/kkUtil.h"
#include "FileSystem/kkFileSystem.h"

#include "kkFileWin32.h"
#include "kkFileSystemWin32.h"

kkFileSystemWin32::kkFileSystemWin32()
{
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName( NULL, szFileName, MAX_PATH );
	m_exePath.assign( (char16_t*)szFileName );

	util::stringFlipSlash<kkString>( m_exePath );
	util::stringPopBackBefore<kkString>( m_exePath, '/' );

	GetSystemDirectory( szFileName, MAX_PATH );
	m_systemPath.assign( (char16_t*)szFileName );
	m_systemPath += u"/";
	util::stringFlipSlash<kkString>( m_systemPath );
}


kkFileSystemWin32::~kkFileSystemWin32()
{
	if( hFind )
		CloseHandle( hFind );
}

kkFile* kkFileSystemWin32::createFile( const kkString& fileName,
			kkFileMode mode,
			kkFileAccessMode access,
			kkFileAction action,
			kkFileShareMode EFSM,
			u32 EFA
		)
{
	//return new kkFileWin32( fileName, access, action, EFSM, EFA );
	return kkCreate<kkFileWin32>( fileName, mode, access, action, EFSM, EFA );
}

bool kkFileSystemWin32::deleteFile( const kkString& fileName )
{
	if( DeleteFileW( (wchar_t*)fileName.data() ) == FALSE )
	{
		//printWarning( u"Can not delete file [%s]. Error code [%u]", fileName.data(), GetLastError() );
		KK_PRINT_FAILED;
		return false;
	}

	return true;
}

bool kkFileSystemWin32::existFile( const kkString& fileName )
{
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile( (wchar_t*)fileName.data(), &FindFileData );
	int found = handle != INVALID_HANDLE_VALUE;
	if( found )
	{
		FindClose( handle );
		return true;
	}
	return false;
}

bool kkFileSystemWin32::existDir( const kkString& dir )
{
	DWORD dwAttrib = GetFileAttributes( (wchar_t*)dir.data() );
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void kkFileSystemWin32::deleteFolder( const kkString& dir )
{
	std::vector<kkFileSystem::DirObject> objs;
	kkFileSystem::scanDirBegin( dir );
 	kkFileSystem::DirObject ob;
 	while( kkFileSystem::getDirObject( &ob ) )
	{
 		objs.push_back( ob );
 	}
 	kkFileSystem::scanDirEnd();
	u32 sz = (u32)objs.size();

 	for( u32 i = 0; i < sz; ++i )
	{
 		auto * o = &objs[ i ];
  		if( o->type == kkFileSystem::DirObjectType::info )
		{
  			continue;
		}
		else if( o->type == kkFileSystem::DirObjectType::folder )
		{
			deleteFolder( (char16_t*)o->path );
		}
		else if( o->type == kkFileSystem::DirObjectType::file )
		{
			kkString path( (char16_t*)o->path );
			if( DeleteFile( o->path ) == FALSE )
			{
				fprintf( stderr, "Can not delete file [%s]. Error code [%u]\n", kkString(o->path).to_kkStringA().data(), GetLastError() );
			}
		}
	}

	if( RemoveDirectory( (wchar_t*)dir.data() ) == FALSE )
	{
		fprintf( stderr, "Can not remove directory [%s]. Error code [%u]\n", dir.to_kkStringA().data(), GetLastError() );
	}
}

bool kkFileSystemWin32::deleteDir( const kkString& dir )
{
	deleteFolder( dir );
	return true;
}

bool kkFileSystemWin32::createDir( const kkString& dir )
{
	if( CreateDirectory( (wchar_t*)dir.data(), NULL ) == FALSE )
	{
		DWORD error = GetLastError();
		fprintf( stderr, "Can not create directory [%s]. Error code [%u]\n", dir.to_kkStringA().c_str(), error );
		if( error == ERROR_ALREADY_EXISTS )
			fprintf( stderr, "Directory already exists.\n" );
		else if( error == ERROR_PATH_NOT_FOUND )
			fprintf( stderr, "One or more intermediate directories do not exist.\n" );
		return false;
	}
	return true;
}

void kkFileSystemWin32::scanDirBegin( kkString dir )
{
	if( !m_dirScanBegin ){
		m_dirScanBegin = true;

		m_dir.clear();

		m_dir = dir;

		if( m_dir[ m_dir.size() - 1 ] != u'/'	&& m_dir[ m_dir.size() - 1 ] != u'\\' )
			m_dir.append( u"\\*" );
		else m_dir.append( u"*" );

	}
}

void kkFileSystemWin32::scanDirEnd()
{
	if( m_dirScanBegin )
	{
		m_dirScanBegin = false;
		m_firstCall = false;

		if( hFind )
			FindClose( hFind );
		hFind = nullptr;
	}
}


bool kkFileSystemWin32::getDirObject( kkFileSystem::DirObject* o )
{
	WIN32_FIND_DATA ffd;

	if( !m_firstCall ){
		m_firstCall = true;
		if( hFind )
			FindClose( hFind );

		hFind = FindFirstFile( (wchar_t*)m_dir.data(), &ffd );
		if( INVALID_HANDLE_VALUE == hFind ){
			DWORD error = GetLastError();
			fprintf( stderr, "Can not scan dir. Error code [%u].\n", error );
			if( error == ERROR_FILE_NOT_FOUND ){
				fprintf( stderr, "No matching files can be found.\n" );
			}
			m_dirScanBegin = false;
			return false;
		}
	}
	else
	{
		if( FindNextFile( hFind, &ffd ) == FALSE )
		{
			return false;
		}
	}

	if( ffd.cFileName[ 0 ] == L'.' )
	{
		o->size = 0;
		o->type = kkFileSystem::DirObjectType::info;
	}else{
		kkString fullPath( m_dir );
		fullPath.pop_back();// '*'
		fullPath += (char16_t*)ffd.cFileName;
#ifdef _MSC_VER
		wcscpy_s( o->path, KK_MAX_PATH, (wchar_t*)fullPath.data() );
#else
		wcscpy( o->path, (wchar_t*)fullPath.data() );
#endif

		if( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			o->type = kkFileSystem::DirObjectType::folder;
			o->size = 0;
		}
		else
		{
			o->type = kkFileSystem::DirObjectType::file;

			LARGE_INTEGER i;
			i.LowPart = ffd.nFileSizeLow;
			i.HighPart = static_cast<LONG>( ffd.nFileSizeHigh );

			o->size = static_cast<u32>(i.QuadPart);
		}
	}

	return true;
}

bool kkFileSystemWin32::copyFile( const kkString& existingFileName, const kkString& newFileName, bool overwrite )
{
	if( !this->existFile( existingFileName ) )
	{
		fprintf( stderr, "Can not copy file [%s]. File not exist.\n", existingFileName.to_kkStringA().data() );
		return false;
	}

	DWORD flag = COPY_FILE_FAIL_IF_EXISTS;
	if( overwrite ) flag = 0;

	if( CopyFileEx( (LPWCH)existingFileName.data(),
					(LPWCH)newFileName.data(),
					NULL, NULL, NULL, flag )
		== FALSE )
	{
		fprintf( stderr, "Can not copy file [%s]. Error code [%u].\n", existingFileName.to_kkStringA().data(), GetLastError() );
		return false;
	}

	return true;
}

kkString kkFileSystemWin32::getProgramPath()
{
	return m_exePath;
}


kkString kkFileSystemWin32::getSystemPath()
{
	return m_systemPath;
}

kkString kkFileSystemWin32::getRealPath( const kkString& in )
{
	u32 dots_count = 0;

	u64 in_sz = in.size();

	u64 last = in_sz - 1;

	for( u64 i = 0; i < in_sz; ++i )
	{
		if( in[ i ] == u'.' )
		{
			if( i < last )
			{
				if( in[ i + 1 ] == u'.' )
				{
					++dots_count;
					++i;// skip second .
				}
			}
		}
	}

	kkString realPath = m_exePath;
	kkString relativePath = in;
	for( u32 i = 0; i < dots_count; ++i )
	{
		util::stringPopBackBefore( realPath, '/' );
		relativePath.erase( 0, 2 );
	}

	realPath += relativePath;


	return realPath;
}

#endif