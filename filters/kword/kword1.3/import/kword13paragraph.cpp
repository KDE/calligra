
#include "kword13paragraph.h"

KWord13Paragraph::KWord13Paragraph( void )
{
    m_formats.setAutoDelete ( true );
}

KWord13Paragraph::~KWord13Paragraph( void )
{
}

void KWord13Paragraph::xmldump( QTextStream& iostream )
{
    iostream << "    <paragraph>\n";
    iostream << "      <text>" << m_text << "</text>\n";
    m_layout.xmldump( iostream );
    iostream << "    </paragraph>\n";
}

void KWord13Paragraph::setText( const QString& str)
{
    m_text = str;
}

void KWord13Paragraph::appendText( const QString& str)
{
    m_text += str;
}

QString KWord13Paragraph::text( void ) const
{
    return m_text;
}


void KWord13ParagraphGroup::xmldump( QTextStream& iostream )
{
    iostream << "   <paragraphgroup>\n";
    
    for( QValueList <KWord13Paragraph>::Iterator it = begin();
        it != end(); ++it )
    {
        (*it).xmldump( iostream );
    }
    
    iostream << "   </paragraphgroup>\n";
}

