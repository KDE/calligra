#include <qiodevice.h>

#include "kwordutils.h"
#include "kworddocument.h"

KWordDocument::KWordDocument( void )
{
    m_normalTextFramesetList.setAutoDelete( true );
}

KWordDocument::~KWordDocument( void )
{
}

void KWordDocument::xmldump( QIODevice* io )
{
    QTextStream iostream( io );
    iostream.setEncoding( QTextStream::UnicodeUTF8 );
    
    iostream << "<?xml encoding='UTF-8'?>\n";
    iostream << "<kworddocument>\n";
    
    for ( QMap<QString,QString>::ConstIterator it = m_documentProperties.begin();
        it != m_documentProperties.end();
        ++it)
    {
        iostream << " <param key=\"" << it.key() << "\" data=\"" << EscapeXmlDump( it.data() ) << "\"/>\n";
    }
    
    for ( KWordNormalTextFrameset* item = m_normalTextFramesetList.first();
        item;
        item = m_normalTextFramesetList.next() )
    {
        item->xmldump( iostream );
    }
    
    iostream << "</kworddocument>\n";
}
