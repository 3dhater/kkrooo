// SPDX-License-Identifier: GPL-3.0-only
#ifndef __GT_XML_DOCUMENT_IMPL_H__
#define __GT_XML_DOCUMENT_IMPL_H__

#include <vector>

class kkXMLDocumentImpl : public kkXMLDocument
{
	bool		m_isInit;
	kkXMLNode	m_root;
	kkString	m_fileName;
	kkString	m_text;

	kkString m_expect_apos;
	kkString m_expect_quot;
	kkString m_expect_eq;
	kkString m_expect_slash;
	kkString m_expect_lt;
	kkString m_expect_gt;
	kkString m_expect_sub;
	kkString m_expect_ex;

	u32 m_cursor, m_sz;

	enum _toke_type
	{
		tt_default,
		tt_string
	};
	struct _token
	{
		_token( kkString N, u32 R, u32 C, _toke_type t = _toke_type::tt_default ):
			name( N ), line( R ), col( C ), type( t )
		{
		}
		kkString name;
		u32 line;
		u32 col;
		_toke_type type;
	};

	std::vector<_token> m_tokens;

	void getTokens();
	void decodeEnts( kkString& outText );
		
	char16_t * getName( char16_t * ptr, kkString& outText, u32& line, u32& col );
	char16_t * getString( char16_t * ptr, kkString& outText, u32& line, u32& col );
	char16_t * skipSpace( char16_t * ptr, u32& line, u32& col );

	bool charForName( char16_t * ptr );
	bool charForString( char16_t * ptr );
	bool charIsSymbol( char16_t * ptr );

	bool analyzeTokens();
	bool buildXMLDocument();
	bool getSubNode( kkXMLNode * node );
	bool getAttributes( kkXMLNode * node );
	bool tokenIsName();
	bool nextToken();
	bool unexpectedToken( const _token& token, kkString expected );

	void skipPrologAndDTD();

	void printNode( kkXMLNode* node, u32 indent );

	bool tokenIsString();

	bool XPathGetTokens( std::vector<kkXPathToken> * arr, const kkString& XPath_expression );
	bool XPathIsName( char16_t * ptr );

	char16_t* XPathGetName( char16_t*ptr, kkString * name );
	void XPathGetNodes( u32 level, u32 maxLevel, std::vector<kkString*> elements, kkXMLNode* node, kkArray<kkXMLNode*>* outArr );
public:

	kkXMLDocumentImpl( const kkString& fileName );
	virtual ~kkXMLDocumentImpl();

	bool init();

	kkXMLNode* getRootNode();
	void print();
	const kkString& getText();
	kkArray<kkXMLNode*> selectNodes( const kkString& XPath_expression );
};

#endif