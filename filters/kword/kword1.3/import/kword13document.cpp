#include <qiodevice.h>

#include "kword13utils.h"
#include "kword13layout.h"
#include "kword13document.h"

KWordDocument::KWordDocument( void )
{
    m_normalTextFramesetList.setAutoDelete( true );
    m_tableFramesetList.setAutoDelete( true );
    m_otherFramesetList.setAutoDelete( true );
    m_headerFooterFramesetList.setAutoDelete( true );
    m_footEndNoteFramesetList.setAutoDelete( true );
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
    
    iostream << " <normalframesets>\n";
    for ( KWordTextFrameset* item = m_normalTextFramesetList.first();
        item;
        item = m_normalTextFramesetList.next() )
    {
        item->xmldump( iostream );
    }
    iostream << " </normalframesets>\n";
    
    iostream << " <tableframesets>\n";
    for ( KWordTextFrameset* item12 = m_tableFramesetList.first();
        item12;
        item12 = m_tableFramesetList.next() )
    {
        item12->xmldump( iostream );
    }
    iostream << " </tableframesets>\n";
    
    iostream << " <headerfooterframesets>\n";
    for ( KWordTextFrameset* item2 = m_headerFooterFramesetList.first();
        item2;
        item2 = m_headerFooterFramesetList.next() )
    {
        item2->xmldump( iostream );
    }
    iostream << " </headerfooterframesets>\n";
    
    iostream << " <footendnoteframesets>\n";
    for ( KWordTextFrameset* item3 = m_footEndNoteFramesetList.first();
        item3;
        item3 = m_footEndNoteFramesetList.next() )
    {
        item3->xmldump( iostream );
    }
    iostream << " </footendnoteframesets>\n";
    
    iostream << " <otherframesets>\n";
    for ( KWordFrameset* item4 = m_otherFramesetList.first();
        item4;
        item4 = m_otherFramesetList.next() )
    {
        item4->xmldump( iostream );
    }
    iostream << " </otherframesets>\n";
    
    iostream << " <styles>\n";
    
    for ( QValueList<KWord13Layout>::Iterator it2 = m_styles.begin();
        it2 != m_styles.end();
        ++it2)
    {
        (*it2).xmldump( iostream );
    }
    
    iostream << " </styles>\n";
    
    iostream << "</kworddocument>\n";
}
