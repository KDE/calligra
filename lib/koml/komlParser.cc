#include <komlParser.h>
#include <qstring.h>


KOMLParser::KOMLParser( const QDomDocument& doc )
    : m_doc( doc )
{
    m_node = m_doc.documentElement();
}

KOMLParser::~KOMLParser()
{
}

bool KOMLParser::open( const QString &_search, QString& )
{
    if ( m_node.isNull() )
        return false;

    if ( _search.isEmpty() )
    {
        m_stack.push( m_node );
        m_node = m_node.firstChild();

        return true;
    }

    QDomNode n = m_node;
    do
    {
        QDomElement e = n.toElement();
        if ( !e.isNull() && e.tagName() == _search )
        {
            m_stack.push( m_node );
            m_node = n.firstChild();

            return true;
        }

        n = n.nextSibling();
    } while( !n.isNull() );

    return false;
}

bool KOMLParser::close( QString& )
{
    m_node = m_stack.pop().nextSibling();
    return true;
}

bool KOMLParser::readText( QString& text )
{
    QDomText t = m_node.toText();
    if ( t.isNull() )
        return false;

    text = t.data();

    return true;
}

bool KOMLParser::parseTag( const QString &, QString& name, QValueList<KOMLAttrib>& _attribs )
{
    _attribs.clear();

    QDomElement element = m_stack.top().toElement();
    if ( element.isNull() )
        return false;

    name = element.tagName();

    QDomNamedNodeMap attrs = element.attributes();
    uint len = attrs.length();
    for( uint i = 0; i < len; ++i )
    {
        QDomAttr a = attrs.item( i ).toAttr();
        if ( !a.isNull() )
        {
            KOMLAttrib attrib;
            attrib.m_strName = a.name();
            attrib.m_strValue = a.value();
            _attribs.append( attrib );
        }
    }

    return true;
}

QDomNode KOMLParser::currentNode()
{
    return m_stack.top();
}

QDomElement KOMLParser::currentElement()
{
    return m_stack.top().toElement();
}
