#include <koOasisStyles.h>
#include "kooasiscontext.h"

void KoOasisContext::fillStyleStack( const QDomElement& object, const QString& attrName )
{
    // find all styles associated with an object and push them on the stack
    // OoImpressImport has more tests here, but I don't think they're relevant to OoWriterImport
    if ( object.hasAttribute( attrName ) )
        addStyles( m_styles.styles()[object.attribute( attrName )] );
}

void KoOasisContext::addStyles( const QDomElement* style )
{
    // this recursive function is necessary as parent styles can have parents themselves
    if ( style->hasAttribute( "style:parent-style-name" ) )
        addStyles( m_styles.styles()[style->attribute( "style:parent-style-name" )] );
    else if ( !m_styles.defaultStyle().isNull() ) // on top of all, the default style
        m_styleStack.push( m_styles.defaultStyle() );

    //kdDebug(30518) << "pushing style " << style->attribute( "style:name" ) << endl;
    m_styleStack.push( *style );
}
