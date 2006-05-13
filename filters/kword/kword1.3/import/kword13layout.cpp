#include <QTextStream>

#include "kword13utils.h"
#include "kword13layout.h"

KWord13Layout::KWord13Layout( void ) : m_outline( false )
{
}

KWord13Layout::~KWord13Layout( void )
{
}

void KWord13Layout::xmldump( QTextStream& iostream )
{
    iostream << "    <layout name=\"" << EscapeXmlDump( m_name )
        << "\" outline=\"" << ( m_outline ? QString("true") : QString("false") ) << "\">\n";
    
    for ( QMap<QString,QString>::ConstIterator it = m_layoutProperties.begin();
        it != m_layoutProperties.end();
        ++it)
    {
        iostream << "     <param key=\"" << it.key() << "\" data=\"" << EscapeXmlDump( it.data() ) << "\"/>\n";
    }
    
    m_format.xmldump( iostream );
    
    iostream << "    </layout>\n";
}

QString KWord13Layout::key( void ) const
{
    QString strKey;
    
    strKey += m_name;
    strKey += '@';
    
    // Use the number of properties as it is an easy sorting value
    strKey += QString::number( m_layoutProperties.count(), 16 );
    strKey += ':';
    
    if ( m_outline )
        strKey += "O1,";
    else
        strKey += "O0,";
    
    // use the worst key: the whole QMap (### FIXME)
    for ( QMap<QString,QString>::const_iterator it = m_layoutProperties.constBegin() ;
        it != m_layoutProperties.constEnd(); ++it )
    {
        strKey += it.key();
        strKey += '=';
        strKey += it.data();
        strKey += ';';
    }
    
    strKey += '@';
    // At the end, the key from the <FORMAT id="1">
    strKey += m_format.key();
    
    return strKey;
}

QString KWord13Layout::getProperty( const QString& name ) const
{
    QMap<QString,QString>::ConstIterator it ( m_layoutProperties.find( name ) );
    if ( it == m_layoutProperties.end() )
    {
        // Property does not exist
        return QString::null;
    }
    else
    {
        return it.data();
    }
}

