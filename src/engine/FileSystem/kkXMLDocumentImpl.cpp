// SPDX-License-Identifier: GPL-3.0-only
#define KK_EXPORTS
#include "kkrooo.engine.h"

#include "FileSystem/kkFileSystem.h"
#include "kkXMLDocumentImpl.h"
#include "Common/kkUtil.h"

//kkString NAME;
kkXMLDocumentImpl::kkXMLDocumentImpl( const kkString& fileName ):
m_isInit( false ){
	m_fileName = fileName;
	m_expect_apos = u"\'";
	m_expect_quot = u"\"";
	m_expect_eq   = u"=";
	m_expect_slash= u"/";
	m_expect_lt   = u"<";
	m_expect_gt   = u">";
	m_expect_sub  = u"-";
	m_expect_ex   = u"!";
	m_cursor = m_sz = 0;
}

kkXMLDocumentImpl::~kkXMLDocumentImpl()
{
}

bool kkXMLDocumentImpl::init()
{
	if( kkFileSystem::existFile( m_fileName ) )
	{
		if( !util::readTextFromFileForUnicode( m_fileName, m_text ) )
		{
			return false;
		}
	}
	else
	{
		m_text = m_fileName;
	}

	getTokens();

	if( !analyzeTokens() ) 
		return false;

	m_tokens.clear();
	m_isInit = true;

	return true;
}

void kkXMLDocumentImpl::skipPrologAndDTD()
{
	u32 sz = (u32)m_tokens.size();
	while( m_cursor < sz ){
		if( m_tokens[ m_cursor ].name == m_expect_gt ){
			++m_cursor;
			return;
		}else{
			++m_cursor;
		}
	}
}

bool kkXMLDocumentImpl::tokenIsName()
{
	if( m_tokens[ m_cursor ].name == m_expect_lt ) return false;
	if( m_tokens[ m_cursor ].name == m_expect_gt ) return false;
	if( m_tokens[ m_cursor ].name == m_expect_sub ) return false;
	if( m_tokens[ m_cursor ].name == m_expect_ex ) return false;
	if( m_tokens[ m_cursor ].name == m_expect_eq ) return false;
	if( m_tokens[ m_cursor ].name == m_expect_apos ) return false;
	if( m_tokens[ m_cursor ].name == m_expect_quot ) return false;
	if( m_tokens[ m_cursor ].name == m_expect_slash ) return false;
	return true;
}

bool kkXMLDocumentImpl::nextToken()
{
	++m_cursor;
	if( m_cursor >= m_sz )
	{
		fprintf( stderr, "End of XML\n" );
		return true;
	}
//	NAME = m_tokens[ m_cursor ].name;
	return false;
}

bool kkXMLDocumentImpl::unexpectedToken( const _token& token, kkString expected )
{
	fprintf( stderr, "XML: Unexpected token: %s Line:%u Col:%u\n", token.name.to_kkStringA().data(), token.line, token.col );
	fprintf( stderr, "XML: Expected: %s\n", expected.to_kkStringA().data() );
	return false;
}

bool kkXMLDocumentImpl::tokenIsString()
{
	return m_tokens[ m_cursor ].type == kkXMLDocumentImpl::_toke_type::tt_string;
}

bool kkXMLDocumentImpl::getAttributes( kkXMLNode * node )
{
	for(;;)
	{
		kkPtr<kkXMLAttribute> at = kkCreate<kkXMLAttribute>();
		if( nextToken() )
			return false;
		if( tokenIsName() )
		{
			at->name = m_tokens[ m_cursor ].name;
		
			if( nextToken() ) 
				return false;

			if( m_tokens[ m_cursor ].name == m_expect_eq )
			{
				if( nextToken() ) 
					return false;

				if( m_tokens[ m_cursor ].name == m_expect_apos )
				{
					if( nextToken() ) 
						return false;

					//if( tokenIsName() ){
					if( tokenIsString() )
					{
						at->value = m_tokens[ m_cursor ].name;
						if( nextToken() ) 
							return false;

						if( m_tokens[ m_cursor ].name == m_expect_apos )
						{
///							at->addRef();
							node->addAttribute( at.ptr() );
							at = nullptr; /// גלוסעמ at->addRef();
							continue;
						}
						else
						{
							return unexpectedToken( m_tokens[ m_cursor ], m_expect_apos );
						}
					}
				}
				else if( m_tokens[ m_cursor ].name == m_expect_quot )
				{
					if( nextToken() ) 
						return false;

					//if( tokenIsName() ){ //is string
					if( tokenIsString() )
					{
						at->value = m_tokens[ m_cursor ].name;
						if( nextToken() )
							return false;

						if( m_tokens[ m_cursor ].name == m_expect_quot )
						{
///							at->addRef();
							node->addAttribute( at.ptr() );
							at = nullptr; /// גלוסעמ at->addRef();
							continue;
						}
						else
						{
							return unexpectedToken( m_tokens[ m_cursor ], m_expect_quot );
						}
					}
				}
				else
				{
					return unexpectedToken( m_tokens[ m_cursor ], u"\' or \"" );
				}
			}
			else
			{
				return unexpectedToken( m_tokens[ m_cursor ], m_expect_eq );
			}
		}
		else if( m_tokens[ m_cursor ].name == m_expect_gt || m_tokens[ m_cursor ].name == m_expect_slash )
		{
			return true;
		}
		else
		{
			return unexpectedToken( m_tokens[ m_cursor ], u"attribute or / or >" );
		}
	}
	return false;
}

bool kkXMLDocumentImpl::getSubNode( kkXMLNode * node )
{	
	kkPtr<kkXMLNode> subNode = kkCreate<kkXMLNode>();

	kkString name;

	bool next = false;
	while( m_cursor < m_sz )
	{
		if( m_tokens[ m_cursor ].name == m_expect_lt )
		{
			if( nextToken() )
				return false;

			if( tokenIsName() )
			{
				name = m_tokens[ m_cursor ].name;
				node->name = name;
				if( nextToken() ) 
					return false;

				// First - attributes
				if( tokenIsName() )
				{
					--m_cursor;
					if( !getAttributes( node ) )
					{
						return false;
					}
				}

				if( m_tokens[ m_cursor ].name == m_expect_gt )
				{
					if( nextToken() )
						return false;

					if( tokenIsName() )
					{
						node->text = m_tokens[ m_cursor ].name;

						if( nextToken() )
							return false;

closeNode:
						if( m_tokens[ m_cursor ].name == m_expect_lt )
						{
							if( nextToken() )
								return false;

							if( m_tokens[ m_cursor ].name == m_expect_slash )
							{
								if( nextToken() )
									return false;

								if( m_tokens[ m_cursor ].name == name )
								{
									if( nextToken() ) 
										return false;

									if( m_tokens[ m_cursor ].name == m_expect_gt )
									{
										++m_cursor;
										return true;
									}
									else
									{
										return unexpectedToken( m_tokens[ m_cursor ], m_expect_gt );
									}
								}
								else
								{
									return unexpectedToken( m_tokens[ m_cursor ], name );
								}
							}
							else if( tokenIsName() )
							{
								--m_cursor;
								subNode = kkCreate<kkXMLNode>();
								goto newNode;
							}
							else
							{
								return unexpectedToken( m_tokens[ m_cursor ], m_expect_slash );
							}
						}
						else
						{
							return unexpectedToken( m_tokens[ m_cursor ], m_expect_lt );
						}
					}
					else if( m_tokens[ m_cursor ].name == m_expect_lt ){ // next or </
						if( nextToken() ) 
							return false;

						if( tokenIsName() )
						{ // next node
							next = true;
							--m_cursor;
						}
						else if( m_tokens[ m_cursor ].name == m_expect_slash ){ // return true
							if( nextToken() )
								return false;

							if( m_tokens[ m_cursor ].name == name )
							{
								if( nextToken() )
									return false;

								if( m_tokens[ m_cursor ].name == m_expect_gt )
								{
									++m_cursor;
									return true;
								}
								else
								{
									return unexpectedToken( m_tokens[ m_cursor ], m_expect_gt );
								}
							}
							else
							{
								return unexpectedToken( m_tokens[ m_cursor ], name );
							}
						}
						else
						{
							return unexpectedToken( m_tokens[ m_cursor ], u"/ or <entity>" );
						}
					}
					else
					{
						return unexpectedToken( m_tokens[ m_cursor ], u"\"text\" or <entity>" );
					}
				}
				else if( m_tokens[ m_cursor ].name == m_expect_slash )
				{
					if( nextToken() ) 
						return false;

					if( m_tokens[ m_cursor ].name == m_expect_gt )
					{
						++m_cursor;
						return true;
					}
					else
					{
						return unexpectedToken( m_tokens[ m_cursor ], m_expect_gt );
					}
				}
				else
				{
					return unexpectedToken( m_tokens[ m_cursor ], u"> or /" );
				}
			}
			else
			{
				return unexpectedToken( m_tokens[ m_cursor ], u"name" );
			}

		}
		else
		{
			return unexpectedToken( m_tokens[ m_cursor ], m_expect_lt );
		}

		if( next )
		{
newNode:
			if( getSubNode( subNode.ptr() ) ){
///				subNode->addRef();

				node->addNode( subNode.ptr() );

				subNode = nullptr; /// גלוסעמ subNode->addRef();

				--m_cursor;
				if( nextToken() )
					return false;

				if( m_tokens[ m_cursor ].name == m_expect_lt )
				{
					if( nextToken() )
						return false;

					if( m_tokens[ m_cursor ].name == m_expect_slash )
					{
						--m_cursor;
						goto closeNode;
					}
					else if( tokenIsName() )
					{
						--m_cursor;
						subNode = kkCreate<kkXMLNode>();
						goto newNode;
					}
					else
					{
						return unexpectedToken( m_tokens[ m_cursor ], u"</close tag> or <new tag>" );
					}
				}
				else if( tokenIsName() )
				{
					node->text = m_tokens[ m_cursor ].name;
					
					if( nextToken() ) 
						return false;

					if( m_tokens[ m_cursor ].name == m_expect_lt )
					{
						if( nextToken() ) 
							return false;

						if( m_tokens[ m_cursor ].name == m_expect_slash )
						{
							--m_cursor;
							goto closeNode;
						}
						else if( tokenIsName() )
						{
							--m_cursor;
							//subNode.clear();
							subNode = kkCreate<kkXMLNode>();
							goto newNode;
						}
						else
						{
							return unexpectedToken( m_tokens[ m_cursor ], u"</close tag> or <new tag>" );
						}
					}
					else
					{
						return unexpectedToken( m_tokens[ m_cursor ], m_expect_lt );
					}
				}

			}else{
				return false;
			}
		}
	}

	return true;
}

bool kkXMLDocumentImpl::analyzeTokens()
{
	u32 sz = (u32)m_tokens.size();
	if( !sz )
	{
		fprintf( stderr, "Empty XML\n" );
		return false;
	}

	m_cursor = 0;
	if( m_tokens[ 0 ].name == u"<" )
	{
		if( m_tokens[ 1 ].name == u"?" )
		{
			if( m_tokens[ 2 ].name == u"xml" )
			{
				m_cursor = 2;
				skipPrologAndDTD();
			}
		}
	}

	if( m_tokens[ m_cursor ].name == u"<" )
	{
		if( m_tokens[ m_cursor + 1 ].name == u"!" )
		{
			if( m_tokens[ m_cursor + 2 ].name == u"DOCTYPE" )
				skipPrologAndDTD();
		}
	}

	return buildXMLDocument();
}

bool kkXMLDocumentImpl::buildXMLDocument()
{
	m_sz = (u32)m_tokens.size();
	return getSubNode( &m_root);
}

kkXMLNode* kkXMLDocumentImpl::getRootNode()
{
	return &m_root;
}

bool kkXMLDocumentImpl::charForName( char16_t *ptr )
{
	char16_t c = *ptr;
	if( c > 0x80 ) return true;
	if( util::isAlpha( *ptr ) 
			|| util::isDigit( *ptr )
			|| (*ptr == u'_')
			|| (*ptr == u'.'))
	{
		return true;
	}
	return false;
}

bool kkXMLDocumentImpl::charForString( char16_t * ptr )
{
	char16_t c = *ptr;
	if( c > 0x80 ) return true;
	if( util::isAlpha( *ptr ) 
			|| util::isDigit( *ptr )
			|| (*ptr == u'_')
			|| (*ptr == u'.'))
	{
		return true;
	}
	return false;
}

bool kkXMLDocumentImpl::charIsSymbol( char16_t * ptr )
{
	char16_t c = *ptr;
	if( (c == u'<') || (c == u'>')
		|| (c == u'/') || (c == u'\'')
		|| (c == u'\"') || (c == u'=')
		|| (c == u'?') || (c == u'!')
		|| (c == u'-') )
	{
		return true;
	}
	return false;
}

char16_t * kkXMLDocumentImpl::getName( char16_t * ptr, kkString& outText, u32& /*line*/, u32& col )
{
	while( *ptr )
	{
		if( charForName( ptr ) )
		{
			outText += *ptr;
		}
		else
		{
			return ptr;
		}
		++ptr;
		++col;
	}
	return ptr;
}

char16_t * kkXMLDocumentImpl::getString( char16_t * ptr, kkString& outText, u32& line, u32& col )
{
	while( *ptr )
	{
		if( *ptr == u'\n' )
		{
			++line;
			col = 1;
			outText += *ptr;
			++ptr;
		}
		else if( *ptr == u'<' )
		{
			break;
		}
		else
		{
			outText += *ptr;
			++col;
			++ptr;
		}
	}
	return ptr;
}

char16_t * kkXMLDocumentImpl::skipSpace( char16_t * ptr, u32& line, u32& col )
{
	while( *ptr )
	{
		if( *ptr == u'\n' )
		{
			++line;
			col = 1;
			++ptr;
		}
		else if( (*ptr == u'\r')
			|| (*ptr == u'\t')
			|| (*ptr == u' '))
		{
			++col;
			++ptr;
		}
		else 
			break;
	}
	return ptr;
}

void kkXMLDocumentImpl::decodeEnts( kkString& str )
{
	util::stringReplaseSubString( str, kkString(u"&apos;"), kkString(u"\'") );
	util::stringReplaseSubString( str, kkString(u"&quot;"), kkString(u"\"") );
	util::stringReplaseSubString( str, kkString(u"&lt;"), kkString(u"<") );
	util::stringReplaseSubString( str, kkString(u"&kk;"), kkString(u">") );
	util::stringReplaseSubString( str, kkString(u"&amp;"), kkString(u"&") );
}

void kkXMLDocumentImpl::getTokens()
{
	char16_t * ptr = m_text.data();
	u32 line = 1;
	u32 col = 1;

	bool isString = false;

	bool stringType = false; // "

	kkString str;
	
	u32 oldCol = 0;

	while( *ptr )
	{
		if( *ptr == u'\n' )
		{
			col = 0;
			++line;
		}
		else
		{
			if( !isString )
			{
				if( charIsSymbol( ptr ) )
				{
					m_tokens.push_back( _token( kkString( *ptr ), line, col ) );

					if( *ptr == u'\'' )
					{
						oldCol = col;
						str.clear();
						isString = true;
						stringType = true;
					}
					else if( *ptr == u'\"' )
					{
						oldCol = col;
						isString = true;
						stringType = false;
						str.clear();
					}
					else if( *ptr == u'>' )
					{
						++ptr;
						++col;
						ptr = skipSpace( ptr, line, col );
						oldCol = col;
						ptr = getString( ptr, str, line, col );
						if( str.size() )
						{
							util::stringTrimSpace( str );
							decodeEnts( str );
							m_tokens.push_back( _token( str, line, oldCol ) );
							str.clear();
						}
						continue;
					}

				}
				else if( charForName( ptr ) )
				{
					oldCol = col;
					kkString name;
					ptr = getName( ptr, name, line, col );
					m_tokens.push_back( _token( name, line, oldCol ) );
					continue;
				}
			}
			else
			{
				if( stringType ){ // '
					if( *ptr == u'\'' )
					{
						decodeEnts( str );
						m_tokens.push_back( _token( str, line, oldCol+1, kkXMLDocumentImpl::_toke_type::tt_string ) );
						m_tokens.push_back( _token( kkString( *ptr ), line, col ) );
						str.clear();
						isString = false;
						goto chponk;
					}
				}
				else{ // "
					if( *ptr == u'\"' )
					{
						decodeEnts( str );
						m_tokens.push_back( _token( str, line, oldCol+1, kkXMLDocumentImpl::_toke_type::tt_string ) );
						m_tokens.push_back( _token( kkString( *ptr ), line, col ) );
						str.clear();
						isString = false;
						goto chponk;
					}
				}
				str += *ptr;
			}
		}
chponk:

		++col;
		++ptr;

	}
}

void kkXMLDocumentImpl::printNode( kkXMLNode* node, u32 indent )
{
	if( node->name.size() )
	{
		kkString line;
		for( u32 i = 0; i < indent; ++i )
		{
			line += u" ";
		}
		
		line += u"<";
		line += node->name;
		line += u">";

		if( node->attributeList.size() )
		{
			line += u" ( ";

			for( u32 i = 0; i < node->attributeList.size(); ++i )
			{
				const kkXMLAttribute * at = node->attributeList[ i ];

				if( at->name.size() )
				{
					line += at->name;
					line += u":";
					if( at->value.size() ){
						line += u"\"";
						line += at->value;
						line += u"\"";
						line += u" ";
					}else{
						line += u"ERROR ";
					}
				}

			}
			line += u" )";
		}

		if( node->text.size() )
		{
			line += u" = ";
			line += node->text;
		}
		fprintf( stdout, "%s\n", line.to_kkStringA().data() );

		if( node->nodeList.size() )
		{
			for( u32 i = 0; i < node->nodeList.size(); ++i )
			{
				printNode( node->nodeList[ i ], ++indent );
				--indent;
			}
		}

	}
}

void kkXMLDocumentImpl::print()
{
	printf( "XML:n" );
	printNode( &m_root, 0 );
}

const kkString& kkXMLDocumentImpl::getText(){
	return m_text;
}

bool kkXMLDocumentImpl::XPathIsName( char16_t * ptr )
{

	if( *ptr == u':' )
	{
		if( *(ptr + 1) == u':' )
		{
			return false;
		}
	}

	switch( *ptr )
	{
	case u'/':
	case u'*':
	case u'\'':
	case u',':
	case u'=':
	case u'+':
	case u'-':
	case u'@':
	case u'[':
	case u']':
	case u'(':
	case u')':
	case u'|':
	case u'!':
		return false;
	}

	return true;
}

bool kkXMLDocumentImpl::XPathGetTokens( std::vector<kkXPathToken> * arr, const kkString& XPath_expression )
{
	kkString name;
	char16_t * ptr = XPath_expression.data();
	char16_t next;
	while( *ptr )
	{		
		name.clear();
		next = *(ptr + 1);

		kkXPathToken token;

		if( *ptr == u'/' )
		{
			if( next )
			{
				if( next == u'/' )
				{
					++ptr;
					token.m_type = kkXPathTokenType::Double_slash;
				}
				else
				{
					token.m_type = kkXPathTokenType::Slash;
				}
			}
			else
			{
				token.m_type = kkXPathTokenType::Slash;
			}
		}else if( *ptr == u'*' ){
			token.m_type = kkXPathTokenType::Mul;
		}else if( *ptr == u'=' ){
			token.m_type = kkXPathTokenType::Equal;
		}else if( *ptr == u'\'' ){
			token.m_type = kkXPathTokenType::Apos;
		}else if( *ptr == u'@' ){
			token.m_type = kkXPathTokenType::Attribute;
		}else if( *ptr == u'|' ){
			token.m_type = kkXPathTokenType::Bit_or;
		}else if( *ptr == u',' ){
			token.m_type = kkXPathTokenType::Comma;
		}else if( *ptr == u'+' ){
			token.m_type = kkXPathTokenType::Add;
		}else if( *ptr == u'+' ){
			token.m_type = kkXPathTokenType::Sub;
		}else if( *ptr == u'[' ){
			token.m_type = kkXPathTokenType::Sq_open;
		}else if( *ptr == u']' ){
			token.m_type = kkXPathTokenType::Sq_close;
		}else if( *ptr == u'(' ){
			token.m_type = kkXPathTokenType::Function_open;
		}else if( *ptr == u')' ){
			token.m_type = kkXPathTokenType::Function_close;
		}else if( *ptr == u'<' ){
			if( next ){
				if( next == u'=' ){
					++ptr;
					token.m_type = kkXPathTokenType::Less_eq;
				}else{
					token.m_type = kkXPathTokenType::Less;
				}
			}else{
				token.m_type = kkXPathTokenType::Less;
			}
		}else if( *ptr == u'>' ){
			if( next ){
				if( next == u'/' ){
					++ptr;
					token.m_type = kkXPathTokenType::More_eq;
				}else{
					token.m_type = kkXPathTokenType::More;
				}
			}else{
				token.m_type = kkXPathTokenType::More;
			}
		}else if( *ptr == u':' ){
			if( next ){
				if( next == u':' ){
					++ptr;
					token.m_type = kkXPathTokenType::Axis_namespace;
				}else{
					fprintf( stderr, "XPath: Bad token\n" );
					return false;
				}
			}else{
				fprintf( stderr, "XPath: Bad tokenn" );
				return false;
			}
		}else if( *ptr == u'!' )
		{
			if( next ){
				if( next == u'=' ){
					++ptr;
					token.m_type = kkXPathTokenType::Not_equal;
				}else{
					fprintf( stderr, "XPath: Bad token\n" );
					return false;
				}
			}else{
				fprintf( stderr, "XPath: Bad token\n" );
				return false;
			}
		}else if( XPathIsName( ptr ) )
		{
			ptr = XPathGetName( ptr, &name );
			token.m_type = kkXPathTokenType::Name;
			token.m_string = name;
		}else{
			fprintf( stderr, "XPath: Bad token\n" );
			return false;
		}

		arr->push_back( token );

		++ptr;
	}

	return true;
}

char16_t* kkXMLDocumentImpl::XPathGetName( char16_t*ptr, kkString * name )
{
	while( *ptr )
	{
		if( XPathIsName( ptr ) ) *name += *ptr;
		else{
			break;
		}
		++ptr;
	}
	--ptr;
	return ptr;
}

std::vector<kkXMLNode*> kkXMLDocumentImpl::selectNodes( const kkString& XPath_expression )
{
	std::vector<kkXMLNode*> a;

	if( !m_isInit )
	{
		fprintf( stderr, "Bad kkXMLDocument\n" );
		return a;
	}

	std::vector<kkXPathToken> XPathTokens;

	if( !XPathGetTokens( &XPathTokens, XPath_expression ) )
	{
		fprintf( stderr, "Bad XPath expression\n" );
		return a;
	}

	std::vector<kkString*> elements;

	u32 next = 0;
	u32 sz = (u32)XPathTokens.size();
	for( u32 i = 0; i < sz; ++i )
	{
		next = i + 1;
		if( i == 0 )
		{
			if( XPathTokens[ i ].m_type != kkXPathTokenType::Slash && XPathTokens[ i ].m_type != kkXPathTokenType::Double_slash)
			{
				fprintf( stderr, "Bad XPath expression \"%s\". Expression must begin with `/`\n", XPath_expression.to_kkStringA().data() );
				return a;
			}
		}

		switch( XPathTokens[ i ].m_type ){
			case kkXPathTokenType::Slash:
			if( next >= sz ){
				fprintf( stderr, "Bad XPath expression\n" );
				return a;
			}
			if( XPathTokens[ next ].m_type == kkXPathTokenType::Name ){
				elements.push_back( &XPathTokens[ next ].m_string );
				++i;
			}else{
				fprintf( stderr, "Bad XPath expression \"%s\". Expected XML element name\n", XPath_expression.to_kkStringA().data() );
				return a;
			}
			break;

			case kkXPathTokenType::Double_slash:
			break;
			case kkXPathTokenType::Name:
			break;
			case kkXPathTokenType::Equal:
			break;
			case kkXPathTokenType::Not_equal:
			break;
			case kkXPathTokenType::More:
			break;
			case kkXPathTokenType::Less:
			break;
			case kkXPathTokenType::More_eq:
			break;
			case kkXPathTokenType::Less_eq:
			break;
			case kkXPathTokenType::Apos:
			break;
			case kkXPathTokenType::Number:
			break;
			case kkXPathTokenType::Comma:
			break;
			case kkXPathTokenType::Function:
			break;
			case kkXPathTokenType::Function_open:
			break;
			case kkXPathTokenType::Function_close:
			break;
			case kkXPathTokenType::Attribute:
			break;
			case kkXPathTokenType::Bit_or:
			break;
			case kkXPathTokenType::Sq_open:
			break;
			case kkXPathTokenType::Sq_close:
			break;
			case kkXPathTokenType::Div:
			break;
			case kkXPathTokenType::Mod:
			break;
			case kkXPathTokenType::Add:
			break;
			case kkXPathTokenType::Sub:
			break;
			case kkXPathTokenType::Mul:
			break;
			case kkXPathTokenType::And:
			break;
			case kkXPathTokenType::Or:
			break;
			case kkXPathTokenType::Axis_namespace:
			break;
			case kkXPathTokenType::Axis:
			break;
			case kkXPathTokenType::NONE:
			break;
		}

	}

	if( elements.size() )
	{
		sz = (u32)elements.size();
		XPathGetNodes( 0, sz - 1, elements, &m_root, &a );

	}
	return a;
}

void kkXMLDocumentImpl::XPathGetNodes(
	u32 level, 
	u32 maxLevel, 
	std::vector<kkString*> elements, 
	kkXMLNode* node, 
	std::vector<kkXMLNode*>* outArr ){
//_______________________________
	if( node->name == *elements[ level ] )
	{	
		if( level == maxLevel ){
			outArr->push_back( node );
			return;
		}else{
			u32 sz = (u32)node->nodeList.size();
			for( u32  i = 0; i < sz; ++i ){
				XPathGetNodes( level + 1, maxLevel, elements, node->nodeList[ i ], outArr );
			}
		}

	}
}