#ifndef KOOASISCONTEXT_H
#define KOOASISCONTEXT_H

class KoOasisStyles;
class QDomElement;
#include <koStyleStack.h>

/**
 * Used during loading of Oasis format (and discarded at the end of the loading).
 * Might move to kofficecore if used by non-text apps
 */
class KoOasisContext
{
public:
    /// Stores reference to the KoOasisStyles parsed by KoDocument.
    KoOasisContext( KoOasisStyles& styles )
        : m_styles( styles )
        {}

    KoOasisStyles& m_styles;
    KoStyleStack m_styleStack;

    void fillStyleStack( const QDomElement& object, const QString& attrName );
    void addStyles( const QDomElement* style );
};

#endif /* KOOASISCONTEXT_H */

