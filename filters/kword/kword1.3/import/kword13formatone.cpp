#include <qtextstream.h>

#include "kword13utils.h"
#include "kword13formatone.h"

KWord13FormatOneData::KWord13FormatOneData( void )
{
}

KWord13FormatOneData::~KWord13FormatOneData( void )
{
}

void KWord13FormatOneData::xmldump( QTextStream& iostream )
{
    iostream << "     <formatone>"  << "\">\n";
    
    for ( QMap<QString,QString>::ConstIterator it = m_properties.begin();
        it != m_properties.end();
        ++it)
    {
        iostream << "       <param key=\"" << it.key() << "\" data=\"" << EscapeXmlDump( it.data() ) << "\"/>\n";
    }
    
    iostream << "    </formatone>\n";
}

QString KWord13FormatOneData::key( void ) const
{
    QString strKey;
    
    // At first, use the number of properties as it is an easy sorting value
    strKey += QString::number( m_properties.count(), 16 );
    strKey += ':';
  
    // use the worst key: the whole QMap (### FIXME)
    for ( QMap<QString,QString>::const_iterator it = m_properties.constBegin() ;
        it != m_properties.constEnd(); ++it )
    {
        strKey += it.key();
        strKey += '=';
        strKey += it.data();
        strKey += ';';
    }
      
    return strKey;
}

QString KWord13FormatOneData::getProperty( const QString& name ) const
{
    QMap<QString,QString>::ConstIterator it ( m_properties.find( name ) );
    if ( it == m_properties.end() )
    {
        // Property does not exist
        return QString::null;
    }
    else
    {
        return it.data();
    }
}

//
//
//

KWord13FormatOne::KWord13FormatOne(void) : m_length(1)
{
    m_id = 1;
}

KWord13FormatOne::~KWord13FormatOne(void)
{
}

int KWord13FormatOne::length(void)
{
    return m_length;
}

KWord13FormatOneData* KWord13FormatOne::getFormatOneData(void)
{
    return &m_formatOne;
}

