// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_FILE_SYSTEM_H__
#define __KK_FILE_SYSTEM_H__

#include "FileSystem/kkXMLDocument.h"

constexpr u32 KK_MAX_PATH = 0xffff;

enum class kkTextFileFormat : u32
{
	UTF_8,
	UTF_16,
	UTF_32
};

enum class kkTextFileEndian : u32
{
	Little, //	0xFFFE0000
	Big // 0x0000FEFF, not implemented !
};

struct kkTextFileInfo
{
	kkTextFileFormat m_format;
	kkTextFileEndian m_endian;
	bool m_hasBOM;
};

enum class kkFileSeekPos : u32{
	Begin,
	Current,
	End
};

class kkFile
{
public:
	kkFile(){}
	virtual ~kkFile(){}

	virtual void	flush() = 0;
	virtual kkTextFileInfo	getTextFileInfo() = 0;
	virtual void	setTextFileInfo( kkTextFileInfo info ) = 0;
	virtual u64	read( u8 * data, u64 size ) = 0;
	virtual void	seek( u64 distance, kkFileSeekPos pos ) = 0;
	virtual u64	size() = 0;
	virtual u64	tell() = 0;
	virtual u32	write( u8 * data, u32 size ) = 0;
	virtual void	write( const kkStringA& string ) = 0;
	virtual void	write( const kkString& string ) = 0;
	virtual void	write( const kkString32& string ) = 0;
};

enum class kkFileAccessMode : u32
{
	Read,
	Write,
	Both,
	Append
};
	
enum class kkFileMode : u32
{
	Text,
	Binary
};

enum class kkFileShareMode : u32
{
	None,
	Delete,
	Read,
	Write
};
	
enum class kkFileAction : u32
{
	Open,
	Open_new,
};

enum class kkFileAttribute : u32
{
	Normal,
	Hidden,
	Readonly
};

class kkFileSystem
{
public:

	kkFileSystem(){}
	virtual ~kkFileSystem(){}

	KK_API static kkFile* createFile( 
		const kkString& fileName,
		kkFileMode mode,
		kkFileAccessMode access,
		kkFileAction action,
		kkFileShareMode shareMode = kkFileShareMode::None,
		u32 attributeFlags = 0u
	);

	KK_API static bool copyFile( const kkString& existingFileName, const kkString& newFileName, bool overwrite );
	KK_API static bool createDir( const kkString& dir );
	KK_API static bool deleteDir( const kkString& dir );
	KK_API static bool deleteFile( const kkString& file );
	KK_API static bool existDir( const kkString& dir );
	KK_API static bool existFile( const kkString& file );
	KK_API static kkString getProgramPath();
	KK_API static kkString getRealPath( const kkString& in );
	KK_API static kkString getSystemPath();

	enum class DirObjectType : u32
	{
		info,
		folder, 
		file 
	};

	struct DirObject{
		DirObject(){ memset( path, 0u, KK_MAX_PATH * sizeof(wchar_t) ); }
		wchar_t path[KK_MAX_PATH];
		DirObjectType type;
		u32 size;
	};
	
	KK_API static bool getDirObject( DirObject* inout );
	KK_API static void scanDirBegin( kkString dir );
	KK_API static void scanDirEnd();

	KK_API static kkXMLDocument* XMLRead( const kkString& file );
	KK_API static void XMLWrite( const kkString& file, kkXMLNode* rootNode, bool utf8 = false );

};

namespace util
{
	KK_FORCE_INLINE kkFile* openFileForReadText( const kkString& fileName ){
		return kkFileSystem::createFile( fileName, kkFileMode::Text, kkFileAccessMode::Read, kkFileAction::Open );
	}
		
	KK_FORCE_INLINE kkFile* createFileForReadText( const kkString& fileName ){
		return kkFileSystem::createFile( fileName, kkFileMode::Text, kkFileAccessMode::Read, kkFileAction::Open_new );
	}

	KK_FORCE_INLINE kkFile* openFileForWriteText( const kkString& fileName ){
		return kkFileSystem::createFile( fileName, kkFileMode::Text, kkFileAccessMode::Append, kkFileAction::Open );
	}
		
	KK_FORCE_INLINE	kkFile* createFileForWriteText( const kkString& fileName ){
		return kkFileSystem::createFile( fileName, kkFileMode::Text, kkFileAccessMode::Write,kkFileAction::Open_new );
	}

	KK_FORCE_INLINE kkFile* openFileForReadBin( const kkString& fileName ){
		return kkFileSystem::createFile( fileName, kkFileMode::Binary, kkFileAccessMode::Read, kkFileAction::Open );
	}

	KK_FORCE_INLINE kkFile* openFileForReadBinShared( const kkString& fileName ){
		return kkFileSystem::createFile( fileName, kkFileMode::Binary, kkFileAccessMode::Read, kkFileAction::Open, kkFileShareMode::Read );
	}

	KK_FORCE_INLINE kkFile* openFileForWriteBin( const kkString& fileName ){
		return kkFileSystem::createFile( fileName, kkFileMode::Binary, kkFileAccessMode::Write, kkFileAction::Open );
	}

	KK_FORCE_INLINE kkFile* createFileForWriteBin( const kkString& fileName ){
		return kkFileSystem::createFile( fileName, kkFileMode::Binary, kkFileAccessMode::Write, kkFileAction::Open_new );
	}

	KK_FORCE_INLINE kkFile* createFileForWriteBinShared( const kkString& fileName ){
		return kkFileSystem::createFile( fileName, kkFileMode::Binary, kkFileAccessMode::Write, kkFileAction::Open_new, kkFileShareMode::Read );
	}
}

#endif