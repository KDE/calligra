#include <qtextstream.h>

#include "kword13utils.h"
#include "kword13formatone.h"

KWord13FormatOne::KWord13FormatOne( void )
{
}

KWord13FormatOne::~KWord13FormatOne( void )
{
}

void KWord13FormatOne::xmldump( QTextStream& iostream )
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
