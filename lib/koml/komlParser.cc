#include "komlParser.h"

KOMLParser::KOMLParser( const QDomDocument& doc )
    : m_doc( doc )
{
    m_node = m_doc.documentElement();
}

KOMLParser::~KOMLParser()
{
}

bool KOMLParser::open( const char *_search, string& )
{
    if ( m_node.isNull() )
	return FALSE;

    if ( !_search )
    {
	if ( m_node.isNull() )
	    return FALSE;

	m_stack.push( m_node );
	m_node = m_node.firstChild();

	return TRUE;
    }

    QDomNode n = m_node;
    do
    {
	QDomElement e = n.toElement();
	if ( !e.isNull() && e.tagName() == _search )
        {
	    m_stack.push( m_node );
	    m_node = n.firstChild();
	
	    return TRUE;
	}
	
	n = n.nextSibling();
    } while( !n.isNull() );

    return FALSE;
}

bool KOMLParser::close( string& )
{
    m_node = m_stack.pop().nextSibling();

    return TRUE;
}

bool KOMLParser::readText( string& text )
{
    QDomText t = m_node.toText();
    if ( t.isNull() )
	return FALSE;

    text = t.data().latin1();

    return TRUE;
}

bool KOMLParser::parseTag( const char *, string& name, std::vector<KOMLAttrib>& _attribs )
{
    _attribs.erase( _attribs.begin(), _attribs.end() );

    QDomElement element = m_stack.top().toElement();
    if ( element.isNull() )
	return FALSE;

    name = element.tagName().latin1();

    QDomNamedNodeMap attrs = element.attributes();
    uint len = attrs.length();
    for( uint i = 0; i < len; ++i )
    {
	QDomAttr a = attrs.item( i ).toAttr();
	if ( !a.isNull() )
        {
	    KOMLAttrib attrib;
	    attrib.m_strName = a.name().latin1();
	    attrib.m_strValue = a.value().latin1();
	    _attribs.push_back( attrib );
	}
    }

    return TRUE;
}

