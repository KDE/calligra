#include "koStore.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

KoStore::KoStore( const char* _filename, KOStore::Mode _mode )
{
  m_bIsOpen = false;
  m_mode = _mode;
  m_id = 0;
  
  if ( _mode == KOStore::Write )
  {
    m_out.open( _filename, ios::out | ios::trunc );
    m_out.write( "KS01", 4 );
  }
  else if ( _mode == KOStore::Append )
  {
    m_in.open( _filename, ios::in );
    // Skip header
    m_in.seekg( 4 );
    while( !m_in.eof() )
    {
      Entry e;
      readHeader( e );
      m_in.seekg( e.size, ios::cur );
      m_map[ e.name ] = e;
    }

    m_in.close();
    
    m_out.open( _filename, ios::out | ios::in );
    m_out.seekp( 0, ios::end );
  }
  else if ( _mode == KOStore::Read )
  {
    m_in.open( _filename, ios::in );
    // Skip header
    m_in.seekg( 4 );

    while( !m_in.eof() )
    {
      Entry e;
      if ( readHeader( e ) )
      {  
	m_in.seekg( e.size, ios::cur );
	m_map[ e.name ] = e;
      }
    }
  }
  else
    assert( 0 );
}

KoStore::~KoStore()
{
  cerr << "###################### DESTRUCTOR ####################" << endl;
  
  if ( m_mode == KOStore::Write )
  {
    m_out.close();
  }
  else
    m_in.close();
}

void KoStore::writeHeader( const KoStore::Entry& _entry )
{
  int len = _entry.name.size() + 1 + _entry.mimetype.size() + 1 + 4 + 4 + 4 + 4 + 4;
  putULong( len );
  putULong( _entry.size );
  putULong( _entry.flags );
  putULong( _entry.mimetype.size() );
  m_out.write( _entry.mimetype.c_str(), _entry.mimetype.size() + 1 );
  putULong( _entry.name.size() );
  m_out.write( _entry.name.c_str(), _entry.name.size() + 1 );
}

unsigned long KoStore::readHeader( KoStore::Entry& _entry )
{
  _entry.pos = m_in.tellg();
  unsigned long len = getULong();
  if ( m_in.eof() )
    return 0L;
  
  _entry.size = getULong();
  _entry.flags = getULong();

  unsigned int s = getULong();
  char *str = new char[ s + 1 ];
  m_in.read( str, s + 1 );
  _entry.mimetype = str;
  delete []str;

  s = getULong();
  str = new char[ s + 1 ];
  m_in.read( str, s + 1 );
  _entry.name = str;
  delete []str;
  _entry.data = m_in.tellg();

  return len;
}
  
void KoStore::putULong( unsigned long x )
{
  int n;
  for ( n = 0; n < 4; n++ )
  {
    m_out.put( (char)(x & 0xff) );
    x >>= 8;
  }
}

unsigned long KoStore::getULong()
{
  unsigned long x = m_in.get();
  x += ( (unsigned long)m_in.get() ) << 8;
  x += ( (unsigned long)m_in.get() ) << 16;
  x += ( (unsigned long)m_in.get() ) << 24;
  
  return x;
}

char* KoStore::createFileName()
{
  char buffer[ 100 ];
  sprintf( buffer, "entry%i", m_id++ );
  return CORBA::string_dup( buffer );
}

void KoStore::list()
{
  cout << "Size\tFlags\tType\t\tName" << endl;
  cout << "--------------------------------------------------------------------" << endl;

  unsigned int size = 0;
  
  map<string,Entry>::iterator it = m_map.begin();
  for( ; it != m_map.end(); ++it )
  {
    size += it->second.size;
    cout << it->second.size << "\t" << it->second.flags << "\t" << it->second.mimetype << "\t" << it->second.name << endl;
  }

  cout << "--------------------------------------------------------------------" << endl;
  cout << "Total Size: " << size << endl;
}

CORBA::Boolean KoStore::open( const char* _name, const char *_mime_type )
{
  if ( m_bIsOpen )
  {
    cerr << "KoStore: File is already opened" << endl;
    return false;
  }
    
  if ( !_mime_type && m_mode != KOStore::Read )
  {
    cerr << "KoStore: Mimetype omitted while opening entry " << _name << " for writing" << endl;
    return false;
  }
  
  if ( strlen( _name ) > 512 )
  {
    cerr << "KoStore: Filename " << _name << " is too long" << endl;
    return false;
  }
  
  if ( m_mode == KOStore::Write || m_mode == KOStore::Append )
  {
    if ( m_map.find( _name ) != m_map.end() )
    {
      cerr << "KoStore: Duplicate filename " << _name << endl;
      return false;
    }
    
    m_current.pos = m_out.tellp();
    m_current.name = _name;
    m_current.mimetype = _mime_type;
    m_current.flags = 0;
    // We will write this header again later once we know the size
    writeHeader( m_current );
    m_current.data = m_out.tellp();
  }
  else if ( m_mode == KOStore::Read )
  { 
    cerr << "Opening for reading " << _name << endl;
    
    map<string,Entry>::iterator it = m_map.find( _name );
    if ( it == m_map.end() )
    {
      cerr << "Unknown filename " << _name << endl;
      return false;
    }
    if ( _mime_type && strlen( _mime_type ) != 0 && it->second.mimetype != _mime_type )
    {
      cerr << "Wrong mime_type in file " << _name << endl;
      cerr << "Expected " << _mime_type << " but got " << it->second.mimetype << endl;
      return false;
    }
    m_in.seekg( it->second.data );
    m_readBytes = 0;
    m_current = it->second;
    m_in.clear();
  }
  else
    assert( 0 );
  
  m_bIsOpen = true;

  return true;
}

void KoStore::close()
{
  cerr << "CLOSED" << endl;
  
  if ( !m_bIsOpen )
  {
    cerr << "KoStore: You must open before closing" << endl;
    return;
  }

  if ( m_mode == KOStore::Write || m_mode == KOStore::Append )
  {
    m_current.size = m_out.tellp() - m_current.data;
    m_out.seekp( m_current.pos );
    writeHeader( m_current );
    m_out.seekp( 0, ios::end );
  }

  m_bIsOpen = false;
}

KOStore::Data* KoStore::read( CORBA::ULong max )
{
  cerr << "KOStore::Data* KoStore::read( CORBA::ULong max )" << endl;

  KOStore::Data* data = new KOStore::Data;

  if ( !m_bIsOpen )
  {
    cerr << "KoStore: You must open before reading" << endl;
    data->length( 0 );
    return data;
  }
  if ( m_mode != KOStore::Read )
  {
    cerr << "KoStore: Can not read from store that is opened for writing" << endl;
    data->length( 0 );
    return data;
  }
  
  if ( m_in.eof() )
  {
    cerr << "EOF" << endl;
    data->length( 0 );
    return data;
  }
  
  if ( max > m_current.size - m_readBytes )
    max = m_current.size - m_readBytes;
  if ( max == 0 )
  {
    cerr << "EOF 2" << endl;
    data->length( 0 );
    return data;
  }
  
  unsigned char *p = new unsigned char[ max ];
  m_in.read( p, max );
  unsigned int len = m_in.gcount();
  if ( len != max )
  {
    cerr << "KoStore: Error while reading" << endl;
    data->length( 0 );
    return data;
  }
  
  m_readBytes += max;
  data->length( max );
  for( unsigned int i = 0; i < max; i++ )
    (*data)[i] = p[i];
  delete [] p;

  cerr << "...KOStore::Data* KoStore::read( CORBA::ULong max )" << endl;
  
  return data;
}

long KoStore::read( char *_buffer, unsigned long _len )
{
  if ( !m_bIsOpen )
  {
    cerr << "KoStore: You must open before reading" << endl;
    return -1;
  }
  if ( m_mode != KOStore::Read )
  {
    cerr << "KoStore: Can not read from store that is opened for writing" << endl;
    return -1;
  }
  
  if ( m_in.eof() )
    return 0;
  
  if ( _len > m_current.size - m_readBytes )
    _len = m_current.size - m_readBytes;
  if ( _len == 0 )
    return 0;
  
  m_in.read( _buffer, _len );
  unsigned int len = m_in.gcount();
  if ( len != _len )
  {
    cerr << "KoStore: Error while reading" << endl;
    return -1;
  }
  
  m_readBytes += len;
  
  return len;
}

CORBA::Boolean KoStore::write( const KOStore::Data& data )
{
  if ( !m_bIsOpen )
  {
    cerr << "KoStore: You must open before writing" << endl;
    return 0L;
  }
  if ( m_mode != KOStore::Write && m_mode != KOStore::Append )
  {
    cerr << "KoStore: Can not write to store that is opened for reading" << endl;
    return 0L;
  }

  int len = data.length();
  unsigned char *p = new unsigned char[ len ];
  for( int i = 0; i < len; i++ )
    p[i] = data[i];
    
  m_out.write( p, len );
  m_current.size += len;
  
  delete [] p;

  if ( m_out.bad() )
  {
    cerr << "KoStore: Error while writing" << endl;
    return false;
  }
  
  return true;
}

bool KoStore::write( const char* _data, unsigned long _len )
{
  if ( !m_bIsOpen )
  {
    cerr << "KoStore: You must open before writing" << endl;
    return 0L;
  }
  if ( m_mode != KOStore::Write && m_mode != KOStore::Append )
  {
    cerr << "KoStore: Can not write to store that is opened for reading" << endl;
    return 0L;
  }
  m_out.write( _data, _len );
  m_current.size += _len;

  if ( m_out.bad() )
  {
    cerr << "KoStore: Error while writing" << endl;
    return false;
  }

  return true;
}

int istorestreambuffer::underflow ()
{
    // Leseposition vor Puffer-Ende ?
    if (gptr() < egptr() )
    {
      return *gptr();
    }

    /* Anzahl Putback-Bereich ermitteln
     *  - Anzahl bereits gelesener Zeichen
     *  - maximal 4
     */
    int anzPutback;
    anzPutback = gptr() - eback();
    if (anzPutback > 4)
    {
      anzPutback = 4;
    }

    /* die bis zu vier vorher gelesenen Zeichen nach vorne
     * in den Putback-Puffer (erste 4 Zeichen)
     */
    memcpy( puffer + ( 4 - anzPutback ), gptr()-anzPutback, anzPutback );

    /* neue Zeichen lesen
     */
    long anz = 8192;
    if ( !CORBA::is_nil( m_vStore ) )
    {
      KOStore::Data* p;
      cerr << "--->read" << endl;
      p = m_vStore->read( pufferSize - 4 );
      cerr << "<---" << endl;
      if ( !p )
	return EOF;
      anz = p->length();
      for( int i = 0; i < anz; i++ )
	puffer[ 4 + i ] = (char)((*p)[i]);
      delete p;
    }
    else
      anz = m_pStore->read( puffer + 4, pufferSize - 4 );
    if ( anz <= 0 )
    {
      // Fehler oder EOF
      return EOF;
    }
    else
      cerr << "Read " << anz << " bytes" << endl;
     
    /* Puffer-Zeiger neu setzen
     */
    setg (puffer+(4-anzPutback),   // Putback-Anfang
	  puffer+4,                // Leseposition
	  puffer+4+anz);           // Puffer-Ende
    
    // naechstes Zeichen zurueckliefern
    unsigned char c = *((unsigned char*)gptr());
    return c;
    // return *gptr();
}

int ostorestreambuffer::emptybuffer()
{
    int anz = pptr()-pbase();
    if ( !CORBA::is_nil( m_vStore ) )
    {
      KOStore::Data data;
      data.length( anz );
      for( int i = 0; i < anz; i++ )
	  data[ i ] = m_buffer[ i ];
      m_vStore->write( data );
    }
    else
      m_pStore->write( m_buffer, anz );

    pbump (-anz);    // Schreibzeiger entspr. zuruecksetzen
    return anz;
}
