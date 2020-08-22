// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS

#include "kkrooo.engine.h"

#if defined(KK_PLATFORM_WINDOWS)

#include "Classes/Strings/kkString.h"
#include "FileSystem/kkFileSystem.h"
#include "kkFileWin32.h"

kkFileWin32::kkFileWin32( const kkString& fileName,
			kkFileMode mode,
			kkFileAccessMode access,
			kkFileAction action,
			kkFileShareMode EFSM,
			u32 EFA )
{
	if( mode == kkFileMode::Text )
	{
		m_isTextFile = true;
	}

	switch( access )
	{
	case kkFileAccessMode::Read:
		m_desiredAccess |= GENERIC_READ;
		break;
	case kkFileAccessMode::Write:
		m_desiredAccess |= GENERIC_WRITE;
		break;
	case kkFileAccessMode::Both:
		m_desiredAccess |= GENERIC_READ | GENERIC_WRITE;
		break;
	case kkFileAccessMode::Append:
		m_desiredAccess |= FILE_APPEND_DATA;
		break;
	}

	DWORD ShareMode = 0;
	switch( EFSM ){
	default:
	case kkFileShareMode::None:
		break;
	case kkFileShareMode::Delete:
		ShareMode = FILE_SHARE_DELETE;
		break;
	case kkFileShareMode::Read:
		ShareMode = FILE_SHARE_READ;
		break;
	case kkFileShareMode::Write:
		ShareMode = FILE_SHARE_WRITE;
		break;
	}

	DWORD CreationDisposition = 0;
	switch( action ){
	case kkFileAction::Open:
		CreationDisposition = OPEN_ALWAYS;
		break;
	case kkFileAction::Open_new:
		CreationDisposition = CREATE_ALWAYS;
		break;
	}

	DWORD FlagsAndAttributes = 0;
	if( EFA & (u32)kkFileAttribute::Normal ){
		FlagsAndAttributes |= FILE_ATTRIBUTE_NORMAL;
	}else{
		if( EFA & (u32)kkFileAttribute::Hidden ){
			FlagsAndAttributes |= FILE_ATTRIBUTE_HIDDEN;
		}
		if( EFA & (u32)kkFileAttribute::Readonly ){
			FlagsAndAttributes |= FILE_ATTRIBUTE_READONLY;
		}
	}

	m_handle = CreateFileW( (wchar_t*)fileName.data(), m_desiredAccess, ShareMode, NULL,
		CreationDisposition, FlagsAndAttributes, NULL );

	if( !m_handle )
	{
		KK_PRINT_FAILED;
		//printWarning( u"Can not create file [%s], error code[%u]",fileName.data(), GetLastError() );
	}
}


kkFileWin32::~kkFileWin32()
{
	if( m_handle )
	{
		CloseHandle( m_handle );
		m_handle = nullptr;
	}
}


u32	kkFileWin32::write( u8 * data, u32 size )
{

	assert(m_desiredAccess & GENERIC_WRITE);

	if( !m_handle )
	{
		//printWarning( u"Can not write text to file. m_handle == nullptr" );
		KK_PRINT_FAILED;
		return 0;
	}

	DWORD bytesWritten = 0;
	if( WriteFile( m_handle, data, size, &bytesWritten, NULL ) == FALSE )
	{
		//printWarning( u"Can not write text to file. Error code [%u]", GetLastError() );
		KK_PRINT_FAILED;
	}
	else
		m_pointerPosition += size;

	return bytesWritten;
}

void	kkFileWin32::write( const kkStringA& string )
{
	assert( m_isTextFile );
	if( !m_handle )
	{
		fprintf( stderr, "Can not write text to file. m_handle == nullptr\n" );
		return;
	}

	DWORD bytesWritten;
	if( WriteFile( m_handle, string.c_str(), (DWORD)string.size(), &bytesWritten, NULL ) == FALSE )
	{
		fprintf( stderr, "Can not write text to file. Error code [%u]\n", GetLastError() );
	}else
		m_pointerPosition += string.size();
}

void	kkFileWin32::write( const kkString& string )
{
	assert( m_isTextFile );
	if( !m_handle )
	{
		fprintf( stderr, "Can not write text to file. m_handle == nullptr\n" );
		return;
	}

	DWORD bytesWritten;
	if( WriteFile( m_handle, string.c_str(), (DWORD)string.size() * sizeof(char16_t), &bytesWritten, NULL ) == FALSE )
	{
		fprintf( stderr, "Can not write text to file. Error code [%u]\n", GetLastError() );
	}else
		m_pointerPosition += string.size() * sizeof(char16_t);
}

void	kkFileWin32::write( const kkString32& string )
{
	assert( m_isTextFile );
	if( !m_handle )
	{
		fprintf( stderr, "Can not write text to file. m_handle == nullptr\n" );
		return;
	}

	DWORD bytesWritten;
	if( WriteFile( m_handle, string.c_str(), (DWORD)string.size() * sizeof(char32_t), &bytesWritten, NULL ) == FALSE )
	{
		fprintf( stderr, "Can not write text to file. Error code [%u]\n", GetLastError() );
	}else
		m_pointerPosition += string.size() * sizeof(char32_t);
}

void	kkFileWin32::flush()
{
	if( m_handle )
	{
		FlushFileBuffers( m_handle );
	}
}


u64	kkFileWin32::read( u8 * data, u64 size )
{
	assert( m_desiredAccess & GENERIC_READ );
	if( m_handle )
	{
		DWORD readBytesNum = 0;
		if( ReadFile(	m_handle,
						data, (DWORD)size,
						&readBytesNum,
						NULL
			) == FALSE )
		{
			//printWarning( u"Can not read file. Error code [%u]", GetLastError() );
			KK_PRINT_FAILED;
		}
		return readBytesNum;
	}
	return 0;
}

u64		kkFileWin32::size()
{
	if( !m_handle )
	{
		//printWarning( u"Can not get file size. m_handle == nullptr" );
		KK_PRINT_FAILED;
		return 0;
	}
	return static_cast<u64>( GetFileSize( m_handle, NULL ) );
}


u64		kkFileWin32::tell()
{
	return m_pointerPosition;
}


void		kkFileWin32::seek( u64 distance, kkFileSeekPos pos )
{
	if( (m_desiredAccess & GENERIC_READ) || (m_desiredAccess & GENERIC_WRITE) )
	{
		if( m_handle )
		{

			LARGE_INTEGER li;
			li.QuadPart = distance;

			li.LowPart = SetFilePointer( m_handle, li.LowPart, (PLONG)&li.HighPart, (DWORD)pos );
			m_pointerPosition = li.QuadPart;

		}
	}
}

kkTextFileInfo	kkFileWin32::getTextFileInfo()
{
	return m_textInfo;
}

void			kkFileWin32::setTextFileInfo( kkTextFileInfo info )
{
	m_textInfo = info;
}

#endif
