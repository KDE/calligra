#include "komlStreamFeed.h"

KOMLStreamFeed::KOMLStreamFeed( istream &_stream ) : m_in( _stream )
{
}

KOMLStreamFeed::~KOMLStreamFeed()
{
}

KOMLData* KOMLStreamFeed::read()
{
  if ( !m_in )
    return 0L;
  
  // HACK make buffer larger
  char* buffer = new char[ 11 ];
  m_in.read( buffer, 10 );
  int anz = m_in.gcount();
  if ( anz == 0 )
    return 0L;
  
  buffer[ anz ] = 0;
  
  return new KOMLData( buffer, anz );
}

void KOMLStreamFeed::free( KOMLData* _data )
{
  if ( _data )
    delete _data;
}

