#include "kodom.h"

QDomElement KoDom::namedItemNS( const QDomNode& node, const char* nsURI, const char* localName )
{
    QDomNode n = node.firstChild();
    for ( ; !n.isNull(); n = n.nextSibling() ) {
        if ( n.isElement() && n.localName() == localName && n.namespaceURI() == nsURI )
            return n.toElement();
    }
    return QDomElement();
}
