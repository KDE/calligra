#ifndef KODOM_H
#define KODOM_H

#include <qdom.h>

/**
 * This namespace contains a few convenience functions to simplify code using QDom
 * (when loading OASIS documents, in particular).
 */
namespace KoDom {

    /**
     * A namespace-aware version of QDomNode::namedItem(),
     * which also takes care of casting to a QDomElement.
     * Use this when a domelement is known to have only *one* child element
     * with a given tagname.
     *
     * Note: do *NOT* use getElementsByTagNameNS, it's recursive!
     */
    QDomElement namedItemNS( const QDomNode& node, const char* nsURI, const char* localName );

};

#endif /* KODOM_H */

