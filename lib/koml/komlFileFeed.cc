#include "komlFileFeed.h"

KOMLFileFeed::KOMLFileFeed( const char* _filename )
{
  m_fh = fopen( _filename, "rb" );
}

KOMLFileFeed::~KOMLFileFeed()
{
  if ( m_fh )
    fclose( m_fh );
}

KOMLData* KOMLFileFeed::read()
{
  if ( !m_fh )
    return 0L;
  
  char* buffer = new char[ 11 ];
  size_t res = fread( buffer, 1, 10, m_fh );
  if ( res <= 0 )
  {
    fclose( m_fh );
    m_fh = 0L;
    return 0L;
  }
  
  buffer[ res ] = 0;
  return new KOMLData( buffer, res );
}

void KOMLFileFeed::free( KOMLData* _data )
{
  if ( _data )
    delete _data;
}
