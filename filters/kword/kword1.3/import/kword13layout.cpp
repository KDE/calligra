#include <qtextstream.h>

#include "kwordutils.h"
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
    
    iostream << "    </layout>\n";
}
