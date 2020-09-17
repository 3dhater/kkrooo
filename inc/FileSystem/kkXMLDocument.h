// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#ifndef __KK_XML_DOCUMENT_H__
#define __KK_XML_DOCUMENT_H__

#include <vector>

#include "Classes/Strings/kkString.h"

enum class kkXPathTokenType
{
	Slash,
	Double_slash,
	Name,
	Equal,			// price=9.80
	Not_equal,		// price!=9.80
	More,			// price>9.80
	Less,			// price<9.80
	More_eq,		// price>=9.80
	Less_eq,		// price<=9.80
	Apos,
	Number,
	Comma, //,
	Function,
	Function_open,  //(
	Function_close, //)
	Attribute,
	Bit_or,			// //book | //cd
	Sq_open,		// [
	Sq_close,		// ]
	Div,			// 8 div 4
	Mod,			// 5 mod 2
	Add,			// 6 + 4
	Sub,			// 6 - 4
	Mul,			// 6 * 4
	And,			// price>9.00 and price<9.90
	Or,				// price=9.80 or price=9.70,
	Axis_namespace,	//::
	Axis,
	NONE = 0xFFFFFFF
};

enum class kkXPathAxis
{
	Ancestor,			// parent, grandparent, etc.
	Ancestor_or_self,	// parent, grandparent, etc. + current node
	Attribute,
	Child,
	Descendant,			// children, grandchildren, etc.
	Descendant_or_self,	// children, grandchildren, etc. + current node
	Following,
	Following_sibling,
	Namespace,
	Parent,
	Preceding,
	Preceding_sibling,
	Self,
	NONE = 0xFFFFFFF
};

struct kkXPathToken
{
	kkXPathToken():
		m_type( kkXPathTokenType::NONE ),
		m_axis(kkXPathAxis::NONE),
		m_number( 0.f )
		{}
	
	kkXPathToken( kkXPathTokenType type,
		kkString string,
		f32 number )
	: m_type( type ),
	m_axis(kkXPathAxis::NONE),
	m_string( string ),
	m_number( number )
	{}

	kkXPathTokenType    m_type;
	kkXPathAxis         m_axis;
	kkString            m_string;
	f32          m_number;
};

struct kkXMLAttribute
{
	kkXMLAttribute(){}
	kkXMLAttribute( const kkString& Name,
		const kkString& Value ):
		name( Name ),
		value( Value )
	{}
	kkString name;
	kkString value;
};

struct kkXMLNode
{
	kkXMLNode(){}
	kkXMLNode( const kkString& Name ):
		name( Name )
	{}
	kkXMLNode( const kkXMLNode& node )
	{
		name = node.name;
		text = node.text;
		attributeList = node.attributeList;
		nodeList = node.nodeList;
	}

	~kkXMLNode()
	{
		clear();
	}

	kkString name;
	kkString text;
	std::vector<kkXMLAttribute*> attributeList;
	std::vector<kkXMLNode*> nodeList;

	void addAttribute( const kkString& Name,
		const kkString& Value )
	{
		attributeList.push_back( new kkXMLAttribute( Name, Value ) );
	}

	void addAttribute( kkXMLAttribute* a )
	{
		attributeList.push_back( a );
	}

	void addNode( kkXMLNode* node )
	{
		nodeList.push_back( node );
	}

	kkXMLNode& operator=( const kkXMLNode& node )
	{
		name = node.name;
		text = node.text;
		attributeList = node.attributeList;
		nodeList = node.nodeList;

		return *this;
	}

	kkXMLAttribute*	getAttribute( const kkString& Name )
	{
		u32 sz = (u32)attributeList.size();
		for( u32 i = 0; i < sz; ++i )
		{
			if( attributeList[ i ]->name == Name )
			{
				return attributeList[ i ];
			}
		}
		return nullptr;
	}

	void clear()
	{
		name.clear();
		text.clear();
		u32 sz = (u32)attributeList.size();
		for( u32 i = 0; i < sz; ++i )
		{
			kkDestroy(attributeList[ i ]);
		}
		sz =  (u32)nodeList.size();
		for( u32 i = 0; i < sz; ++i )
		{
			kkDestroy(nodeList[ i ]);
		}
		attributeList.clear();
		nodeList.clear();
	}
};

class kkXMLDocument
{
public:

	kkXMLDocument(){}
	virtual ~kkXMLDocument(){}

	virtual              kkXMLNode* getRootNode() = 0;
	virtual                    void print() = 0;
	virtual         const kkString& getText() = 0;
	virtual kkArray<kkXMLNode*> selectNodes( const kkString& XPath_expression ) = 0;
};

#endif
