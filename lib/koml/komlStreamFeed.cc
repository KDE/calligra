#include "komlStreamFeed.h"

KOMLStreamFeed::KOMLStreamFeed( istream *_stream )
{
  m_pStream = _stream;
}

KOMLStreamFeed::~KOMLStreamFeed()
{
}

KOMLData* KOMLStreamFeed::read()
{
  if ( !m_pStream || !(*m_pStream) )
    return 0L;
  
  char* buffer = new char[ 11 ];
  streampos p1 = m_pStream->tellg();
  m_pStream->read( buffer, 10 );
  streampos p2 = m_pStream->tellg();
  if ( ( p2 - p1 ) == 0 )
  {
    m_pStream = 0L;
    return 0L;
  }
  
  buffer[ p2 - p1 ] = 0;
  
  return new KOMLData( buffer, p2 - p1 );
}

void KOMLStreamFeed::free( KOMLData* _data )
{
  if ( _data )
    delete _data;
}

