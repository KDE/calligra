
#include "kwordutils.h"
#include "kwordframeset.h"


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
         << "\" info=\"" << m_frameType
         << "\" name=\"" << EscapeXmlDump( m_name ) <<"\"/>\n";
}


KWordNormalTextFrameset::KWordNormalTextFrameset( int frameType, int frameInfo, const QString& name )
    : KWordFrameset( frameType, frameInfo, name )
{    
}

KWordNormalTextFrameset::~KWordNormalTextFrameset( void )
{
}

bool KWordNormalTextFrameset::addParagraph(const KWordParagraph& para)
{
    m_paragraphGroup << para;
    return true;
}

void KWordNormalTextFrameset::xmldump( QTextStream& iostream )
{
    iostream << "  <frameset variant=\"Text\" type=\"" << m_frameType
         << "\" info=\"" << m_frameType
         << "\" name=\"" << EscapeXmlDump( m_name ) <<"\">\n";
    m_paragraphGroup.xmldump( iostream );
    iostream << "  </frameset>\n";
}
