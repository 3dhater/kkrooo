// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS

#include "kkrooo.engine.h"

#include "FileSystem/kkFileSystem.h"
#include "FileSystem/kkXMLDocument.h"
#include "kkXMLDocumentImpl.h"

kkXMLDocument* kkFileSystem::XMLRead( const kkString& file )
{
	kkXMLDocumentImpl* xml = kkCreate<kkXMLDocumentImpl>(file);
	if( !xml )
	{
		fprintf( stderr, "Can not create XML document.\n" );
		return nullptr;
	}

	if( !xml->init() )
	{
		kkDestroy(xml);
		return nullptr;
	}

	//xml->addRef();
	return xml;
}

void writeText( kkString& outText, const kkString& inText )
{
	u64 sz = inText.size();
	for( u64 i = 0; i < sz; ++i ){
		if( inText[ i ] == u'\'' ){
			outText += u"&apos;";
		}else if( inText[ i ] == u'\"' ){
			outText += u"&quot;";
		}else if( inText[ i ] == u'<' ){
			outText += u"&lt;";
		}else if( inText[ i ] == u'>' ){
			outText += u"&gt;";
		}else if( inText[ i ] == u'&' ){
			outText += u"&amp;";
		}else{
			outText += inText[ i ];
		}
	}
}

void writeName( kkString& outText, const kkString& inText )
{
	outText += "<";
	outText += inText;
}

bool writeNodes( kkString& outText, kkXMLNode* node, u32 tabCount )
{
	for( u32 i = 0; i < tabCount; ++i )
	{
		outText += u"\t";
	}

	++tabCount;

	writeName( outText, node->name );
	
	u32 sz = (u32)node->attributeList.size();
	if( sz )
	{
		for( u32 i = 0; i < sz; ++i ){
			outText += u" ";
			outText += node->attributeList[ i ]->name;
			outText += u"=";
			outText += u"\"";
			writeText( outText, node->attributeList[ i ]->value );
			outText += u"\"";
		}
	}

	if( !node->nodeList.size() && !node->text.size() )
	{
		outText += u"/>\r\n";
		return true;
	}else
	{
		outText += u">\r\n";
		sz = (u32)node->nodeList.size();
		for( u32 i = 0; i < sz; ++i ){
			if( !writeNodes( outText, node->nodeList[ i ], tabCount ) ){
				for( u32 o = 0; o < tabCount; ++o ){
					outText += u"\t";
				}
				outText += u"</";
				outText += node->nodeList[ i ]->name;
				outText += u">\n";
			}
		}
	}

	if( node->text.size() )
	{
		for( u32 o = 0; o < tabCount; ++o )
		{
			outText += u"\t";
		}
		writeText( outText, node->text );
		outText += u"\n";
	}
	--tabCount;

	return false;
}

void kkFileSystem::XMLWrite( const kkString& file, kkXMLNode* rootNode, bool utf8 )
{
	kkString outText( u"<?xml version=\"1.0\"" );
	if( utf8 ) outText += " encoding=\"UTF-8\"";
	outText += " ?>\r\n";

	writeNodes( outText, rootNode, 0 );
	outText += u"</";
	outText += rootNode->name;
	outText += u">\n";

	auto out = kkCreatePtr( util::createFileForWriteText( file ));

	kkTextFileInfo ti;
	ti.m_hasBOM = true;
	if( utf8 )
	{
		ti.m_format = kkTextFileFormat::UTF_8;
		out->setTextFileInfo( ti );
		
		kkStringA mbstr;
		util::string_UTF16_to_UTF8( outText, mbstr );
		
		if( ti.m_hasBOM )
			out->write( kkStringA("\xEF\xBB\xBF") );

		out->write( mbstr );

	}else{
		ti.m_endian = kkTextFileEndian::Little;
		ti.m_format = kkTextFileFormat::UTF_16;

		if( ti.m_hasBOM )
			out->write( kkStringA("\xFF\xFE") );

		out->setTextFileInfo( ti );
		out->write( outText );
	}
}