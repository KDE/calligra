

#include "kwordframeset.h"

KWordFrameset::KWordFrameset( int frameType, int frameInfo, const QString& name )
    : m_frameType( frameType ), m_frameInfo( frameInfo ), m_name( name )
{    
}

KWordFrameset::~KWordFrameset( void )
{
}

KWordNormalTextFrameset::KWordNormalTextFrameset( int frameType, int frameInfo, const QString& name )
    : KWordFrameset( frameType, frameInfo, name )
{    
}

KWordNormalTextFrameset::~KWordNormalTextFrameset( void )
{
}

