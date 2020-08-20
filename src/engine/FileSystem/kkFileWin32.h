// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_FILE_WIN_32_H__
#define __KK_FILE_WIN_32_H__

#if defined(KK_PLATFORM_WINDOWS)

#include <Windows.h>

class kkFileWin32 : public kkFile
{

	kkTextFileInfo	m_textInfo;
	bool			m_isTextFile = false;
	HANDLE			m_handle = nullptr;

		// GENERIC_READ or GENERIC_WRITE
	u64			m_pointerPosition = 0;
	DWORD			m_desiredAccess = 0;
public:
	kkFileWin32( const kkString& fileName,
		kkFileMode mode,
		kkFileAccessMode access,
		kkFileAction action,
		kkFileShareMode EFSM,
		u32 EFA );
	virtual ~kkFileWin32();


	kkTextFileInfo	getTextFileInfo();
	void			setTextFileInfo( kkTextFileInfo info );
	u32			write( u8 * data, u32 size );
	void		write( const kkStringA& string );
	void		write( const kkString& string );
	void		write( const kkString32& string );
	void		flush();
	u64			read( u8 * data, u64 size );
	u64			size();
	u64			tell();
	void 		seek( u64 distance, kkFileSeekPos pos );
};

#endif
#endif

