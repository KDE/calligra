

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

