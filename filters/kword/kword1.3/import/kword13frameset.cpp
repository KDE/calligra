
#include "kword13utils.h"
#include "kword13frameset.h"


KWordFrameset::KWordFrameset( int frameType, int frameInfo, const QString& name )
    : m_numFrames(0), m_frameType( frameType ), m_frameInfo( frameInfo ), m_name( name )
{    
}

KWordFrameset::~KWordFrameset( void )
{
}

bool KWordFrameset::addParagraph(const KWordParagraph&)
{
    qDebug("Cannot add paragraph! Not a text frameset!");
    return false;
}

void KWordFrameset::xmldump( QTextStream& iostream )
{
    iostream << "  <frameset variant=\"None\" type=\"" << m_frameType
         << "\" info=\"" << m_frameInfo
         << "\" name=\"" << EscapeXmlDump( m_name ) <<"\"/>\n";
}


KWordTextFrameset::KWordTextFrameset( int frameType, int frameInfo, const QString& name )
    : KWordFrameset( frameType, frameInfo, name )
{    
}

KWordTextFrameset::~KWordTextFrameset( void )
{
}

bool KWordTextFrameset::addParagraph(const KWordParagraph& para)
{
    m_paragraphGroup << para;
    return true;
}

void KWordTextFrameset::xmldump( QTextStream& iostream )
{
    iostream << "  <frameset variant=\"Text\" type=\"" << m_frameType
         << "\" info=\"" << m_frameInfo
         << "\" name=\"" << EscapeXmlDump( m_name ) <<"\">\n";
    m_paragraphGroup.xmldump( iostream );
    iostream << "  </frameset>\n";
}
