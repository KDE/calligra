#include "koGenStyles.h"
#include <koxmlwriter.h>

KoGenStyles::KoGenStyles()
{
}

KoGenStyles::~KoGenStyles()
{
}

QString KoGenStyles::lookup( const KoGenStyle& style, const QString& name, bool forceNumbering )
{
    StyleMap::iterator it = m_styles.find( style );
    if ( it == m_styles.end() ) {
        QString styleName( name );
        if ( styleName.isEmpty() ) {
            styleName = 'A'; // for "auto".
            forceNumbering = true;
        }
        styleName = makeUniqueName( styleName, forceNumbering );
        m_names.insert( styleName, true );
        it = m_styles.insert( style, styleName );
    }
    return it.data();
}

QString KoGenStyles::makeUniqueName( const QString& base, bool forceNumbering ) const
{
    if ( !forceNumbering && m_names.find( base ) == m_names.end() )
        return base;
    int num = 1;
    QString name;
    do {
        name = base;
        name += QString::number( num++ );
    } while ( m_names.find( name ) != m_names.end() );
    return name;
}

void KoGenStyle::writeStyle( KoXmlWriter* writer, const char* elementName, const QString& name )
{
    writer->startElement( elementName );
    writer->addAttribute( "style:name", name );
    if ( !m_parentName.isEmpty() )
        writer->addAttribute( "style:parent-style-name", m_parentName );
    QMap<QString, QString>::const_iterator it = m_attributes.begin();
    for ( ; it != m_attributes.end(); ++it )
        writer->addAttribute( it.key().utf8(), it.data().utf8() );
    if ( !m_properties.isEmpty() ) {
        writer->startElement( "style:properties" );
        it = m_properties.begin();
        for ( ; it != m_properties.end(); ++it )
            writer->addAttribute( it.key().utf8(), it.data().utf8() );
        writer->endElement();
    }
    writer->endElement();
}
