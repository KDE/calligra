
#include "kwordparagraph.h"

void KWordParagraph::xmldump( QTextStream& iostream )
{
    iostream << "    <paragraph>" << m_text << "</paragraph>\n";
}

void KWordParagraph::setText( const QString& str)
{
    m_text = str;
}

void KWordParagraph::appendText( const QString& str)
{
    m_text += str;
}

QString KWordParagraph::text( void ) const
{
    return m_text;
}


void KWordParagraphGroup::xmldump( QTextStream& iostream )
{
    iostream << "   <paragraphgroup>\n";
    
    for( QValueList <KWordParagraph>::Iterator it = begin();
        it != end(); ++it )
    {
        (*it).xmldump( iostream );
    }
    
    iostream << "   </paragraphgroup>\n";
}

