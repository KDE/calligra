#include "ktar.h"


KTar::KTar( const QString& filename )
{
  m_filename = filename;
  m_open = false;
  m_dir = 0;
}

KTar::~KTar()
{
  if ( m_open )
    close();
  if ( m_dir )
    delete m_dir;
}

bool KTar::open( int mode )
{
  if ( m_open )
    close();

  m_dirList.clear();

  const char* m;
  if ( mode == IO_ReadOnly )
    m = "rb";
  else if ( mode == IO_WriteOnly )
    m = "wb";
  else
  {
    qWarning("KTar::open: You can only pass IO_ReadOnly or IO_WriteOnly as mode\n");
    return false;
  }
  m_mode = mode;

  m_f = gzopen( m_filename, m );
  if ( !m_f )
    return false;

  if ( mode == IO_ReadOnly )
  {
    // Find infos about the tar file itself
    struct stat buf;
    stat( m_filename, &buf );

    struct passwd* pw =  getpwuid( buf.st_uid );
    struct group* grp = getgrgid( buf.st_gid );

    m_dir = new KTarDirectory( this, "/", (int)buf.st_mode, (int)buf.st_mtime, pw->pw_name , grp->gr_name );

    // read dir infos
    char buffer[ 0x200 ];
    bool ende = false;
    do
    {
      // Read header
      int n = gzread( m_f, buffer, 0x200 );
      if ( n == 0x200 && buffer[0] != 0 )
      {
	QString name( buffer );

	bool isdir = false;
	QString nm;

	if ( name.right(1) == "/" )
	{
	  isdir = true;
	  name = name.left( name.length() - 1 );
	}

	int pos = name.findRev( '/' );
	if ( pos == -1 )
	  nm = name;
	else
	  nm = name.mid( pos + 1 );

	// read access
	buffer[ 0x6a ] = 0;
	char *dummy;
	const char* p = buffer + 0x64;
	while( *p == ' ' ) ++p;
	int access = (int)strtol( p, &dummy, 8 );

	// read user and group
	QString user( buffer + 0x109 );
	QString group( buffer + 0x129 );

	// read time
	buffer[ 0x93 ] = 0;
	p = buffer + 0x88;
	while( *p == ' ' ) ++p;
	int time = (int)strtol( p, &dummy, 8 );

	// Skip header
	// gzseek( m_f, 0x200, SEEK_CUR );

	KTarEntry* e;
	if ( isdir )
	  e = new KTarDirectory( this, nm, access, time, user, group );
	else
	{
	  // read size
	  buffer[ 0x87 ] = 0;
	  char *dummy;
	  const char* p = buffer + 0x7c;
	  while( *p == ' ' ) ++p;
	  int size = (int)strtol( p, &dummy, 8 );

	  // Skip data
	  int rest = size % 0x200;
	  /* if ( rest )
	    gzseek( m_f, ( size / 0x200 + 1 ) * 0x200, SEEK_CUR );
	  else
	  gzseek( m_f, size, SEEK_CUR ); */

	  // Read content
	  QByteArray arr( size );
	  int n = gzread( m_f, arr.data(), size );
	  if ( n != size )
	    arr.resize( n );

	  // Skip align bytes
	  if ( rest )
	    gzseek( m_f, 0x200 - rest, SEEK_CUR );

	  e = new KTarFile( this, nm, access, time, user, group, (int)gztell( m_f ), size, arr );
	}

	if ( pos == -1 )
	  m_dir->addEntry( e );
	else
	{
	  KTarEntry* d = m_dir->entry( name.left( pos ) );
	  if ( d && d->isDirectory() )
	    ((KTarDirectory*)d)->addEntry( e );
	}
      }
      else
	ende = true;
    } while( !ende );
  }

  m_open = true;
  return true;
}

/* QByteArray KTar::data( int pos, int size ) const
{
  // gzseek( m_f, pos, SEEK_SET );
  gzrewind( m_f );
  gzseek( m_f, pos, SEEK_CUR );

  QByteArray arr( size + 1 );
  
  int n = gzread( m_f, arr.data(), size );
  if ( n < 0 )
    n = 0;
  if ( n != size )
    arr.resize( n );
  arr[n] = 0;
  printf("Read %i bytes\n%s\n",n,arr.data()+1);
  return arr;
  } */

void KTar::close()
{
  if ( !m_open )
    return;

  m_dirList.clear();

  gzclose( m_f );

  if ( m_dir )
  {
    delete m_dir;
    m_dir = 0;
  }

  m_open = false;
}

const KTarDirectory* KTar::directory() const
{
  return m_dir;
}


void KTar::writeDir( const QString& name, const QString& user, const QString& group )
{
  if ( !isOpen() )
  {
    qWarning( "KTar::writeDir: You must open the tar file before writing to it\n");
    return;
  }

  if ( m_mode != IO_WriteOnly )
  {
    qWarning( "KTar::writeDir: You must open the tar file for writing\n");
    return;
  }

  // Need trailing '/'
  QString n( name );
  if ( n.right(1) != "/" )
    n += "/";

  char buffer[ 0x201 ];
  for( uint i = 0; i < 0x200; ++i )
    buffer[i] = 0;

  // Write name
  strcpy( buffer, n );
  
  // Write type
  buffer[ 0x64 ] = 0x20;
  buffer[ 0x65 ] = 0x34;
  buffer[ 0x66 ] = 0x30;

  // Permissions
  buffer[ 0x67 ] = 0x37;
  buffer[ 0x68 ] = 0x35;
  buffer[ 0x69 ] = 0x35;
  buffer[ 0x6a ] = 0x20;

  // ??
  buffer[ 0x6c ] = 0x20;
  buffer[ 0x6d ] = 0x20;
  buffer[ 0x6e ] = 0x20;
  buffer[ 0x6f ] = 0x37;
  buffer[ 0x70 ] = 0x36;
  buffer[ 0x71 ] = 0x35;
  buffer[ 0x72 ] = 0x20;

  buffer[ 0x74 ] = 0x20;
  buffer[ 0x75 ] = 0x20;
  buffer[ 0x76 ] = 0x20;
  buffer[ 0x77 ] = 0x31;
  buffer[ 0x78 ] = 0x34;
  buffer[ 0x79 ] = 0x34;
  buffer[ 0x7a ] = 0x20;

  // Size
  strcpy( buffer + 0x7c, "          0" );

  buffer[ 0x87 ] = 0x20;

  QString s;
  s.setNum( time( 0 ), 8 );
  s = s.rightJustify( 11, ' ' );
  strcpy( buffer + 0x88, s.latin1() );

  // check sum
  buffer[ 0x93 ] = 0x20;
  buffer[ 0x94 ] = 0x20;
  buffer[ 0x95 ] = 0x20;
  buffer[ 0x96 ] = 0x20;
  buffer[ 0x97 ] = 0x20;
  buffer[ 0x98 ] = 0x20;
  buffer[ 0x99 ] = 0x20;

  // It is a directory
  buffer[ 0x9b ] = 0x20;
  buffer[ 0x9c ] = 0x35;

  strcpy( buffer + 0x101, "ustar  " );

  // user
  strcpy( buffer + 0x109, user );
  // group
  strcpy( buffer + 0x129, group );

  // Header check sum
  int check = 32;
  for( uint j = 0; j < 0x200; ++j )
    check += buffer[j];
  s.setNum( check, 8 );
  s = s.rightJustify( 7, ' ' );
  strcpy( buffer + 0x93, s.latin1() );

  // Write header
  gzwrite( m_f, buffer, 0x200 );

  m_dirList.append( name );
}

void KTar::writeFile( const QString& name, const QString& user, const QString& group, uint size, const char* data )
{
  if ( !isOpen() )
  {
    qWarning( "KTar::writeDir: You must open the tar file before writing to it\n");
    return;
  }

  if ( m_mode != IO_WriteOnly )
  {
    qWarning( "KTar::writeDir: You must open the tar file for writing\n");
    return;
  }

  QString n( name );
  int i = n.findRev( '/' );
  if ( i != -1 )
  {
    QString d = n.left( i );
    if ( !m_dirList.contains( d ) )
    {
      n = n.mid( i + 1 );
      writeDir( d, user, group );
    }
  }

  char buffer[ 0x201 ];
  for( uint i = 0; i < 0x200; ++i )
    buffer[i] = 0;

  // Write name
  strcpy( buffer, name );
  
  // Write type
  buffer[ 0x64 ] = 0x31;
  buffer[ 0x65 ] = 0x30;
  buffer[ 0x66 ] = 0x30;

  // Permissions
  buffer[ 0x67 ] = 0x36;
  buffer[ 0x68 ] = 0x34;
  buffer[ 0x69 ] = 0x34;
  buffer[ 0x6a ] = 0x20;

  // check sum
  buffer[ 0x6c ] = 0x20;
  buffer[ 0x6d ] = 0x20;
  buffer[ 0x6e ] = 0x20;
  buffer[ 0x6f ] = 0x37;
  buffer[ 0x70 ] = 0x36;
  buffer[ 0x71 ] = 0x35;
  buffer[ 0x72 ] = 0x20;

  buffer[ 0x74 ] = 0x20;
  buffer[ 0x75 ] = 0x20;
  buffer[ 0x76 ] = 0x20;
  buffer[ 0x77 ] = 0x31;
  buffer[ 0x78 ] = 0x34;
  buffer[ 0x79 ] = 0x34;
  buffer[ 0x7a ] = 0x20;

  // Size
  QString s;
  s.setNum( size, 8 );
  s = s.rightJustify( 11, ' ' );
  strcpy( buffer + 0x7c, s.latin1() );

  buffer[ 0x87 ] = 0x20;

  s.setNum( time( 0 ), 8 );
  s = s.rightJustify( 11, ' ' );
  strcpy( buffer + 0x88, s.latin1() );

  // ??
  buffer[ 0x93 ] = 0x20;
  buffer[ 0x94 ] = 0x20;
  buffer[ 0x95 ] = 0x20;
  buffer[ 0x96 ] = 0x20;
  buffer[ 0x97 ] = 0x20;
  buffer[ 0x98 ] = 0x20;
  buffer[ 0x99 ] = 0x20;

  // It is a file
  buffer[ 0x9b ] = 0x20;
  buffer[ 0x9c ] = 0x30;

  strcpy( buffer + 0x101, "ustar  " );

  // user
  strcpy( buffer + 0x109, user );
  // group
  strcpy( buffer + 0x129, group );

  // Header check sum
  int check = 32;
  for( uint j = 0; j < 0x200; ++j )
    check += buffer[j];
  s.setNum( check, 8 );
  s = s.rightJustify( 7, ' ' );
  strcpy( buffer + 0x93, s.latin1() );

  // Write header
  gzwrite( m_f, buffer, 0x200 );

  // Write data
  gzwrite( m_f, (void*)data, size );

  // Write alignment
  int rest = size % 200;
  if ( rest )
  {
    for( uint i = 0; i < 0x200; ++i )
      buffer[i] = 0;
    gzwrite( m_f, buffer, 0x200 - rest );
  }
}

KTarEntry::KTarEntry( KTar* t, const QString& name, int access, int date,
		      const QString& user, const QString& group )
{
  m_name = name;
  m_access = access;
  m_date = date;
  m_user = user;
  m_group = group;
  m_tar = t;
}

QDateTime KTarEntry::date() const
{
  QDateTime d;
  d.setTime_t( m_date );
  return d;
}

QString KTarEntry::name() const
{
  return m_name;
}

mode_t KTarEntry::permissions() const
{
  return m_access;
}

QString KTarEntry::user() const
{
  return m_user;
}

QString KTarEntry::group() const
{
  return m_group;
}


KTarFile::KTarFile( KTar* t, const QString& name, int access, int date,
		    const QString& user, const QString& group,
		    int pos, int size, const QByteArray& data )
  : KTarEntry( t, name, access, date, user, group ), m_data( data )
{
  m_pos = pos;
  m_size = size;
}

int KTarFile::position() const
{
  return m_pos;
}

int KTarFile::size() const
{
  return m_size;
}

QByteArray KTarFile::data() const
{
  // return ((KTarFile*)this)->tar()->data( m_pos, m_size );
  return m_data;
}

void KTarFile::print( int indent ) const
{
  for( int i = 0; i < indent; ++i )
    printf(" ");
  
  QString p;
  p.setNum( permissions(), 8 );

  printf("%s %s/%s   %8i %s %s %i\n",p.latin1(), user().latin1(), group().latin1(), size(), date().toString().latin1(), name().latin1(), position() );
}

KTarDirectory::KTarDirectory( KTar* t, const QString& name, int access, int date,
			      const QString& user, const QString& group )
  : KTarEntry( t, name, access, date, user, group )
{
  m_entries.setAutoDelete( true );
}

QStringList KTarDirectory::entries() const
{
  QStringList l;

  QDictIterator<KTarEntry> it( m_entries );
  for( ; it.current(); ++it )
    l.append( it.currentKey() );

  return l;
}

KTarEntry* KTarDirectory::entry( const QString& name )
{
  int pos = name.find( '/' );
  if ( pos != -1 )
  {
    QString left = name.left( pos );
    QString right = name.mid( pos + 1 );
    
    KTarEntry* e = m_entries[ left ];
    if ( !e || !e->isDirectory() )
      return 0;
    return ((KTarDirectory*)e)->entry( right );
  }

  return m_entries[ name ];
}

const KTarEntry* KTarDirectory::entry( const QString& name ) const
{
  return ((KTarDirectory*)this)->entry( name );
}

void KTarDirectory::addEntry( KTarEntry* entry )
{
  m_entries.insert( entry->name(), entry );
}

void KTarDirectory::print( int indent ) const
{
  for( int i = 0; i < indent; ++i )
    printf(" ");
  
  QString p;
  p.setNum( permissions(), 8 );
  p = p.rightJustify( 6, ' ' );
  printf("%s %s/%s   %8i %s %s\n",p.latin1(), user().latin1(), group().latin1(), 0, date().toString().latin1(), name().latin1() );

  QDictIterator<KTarEntry> it( m_entries );
  for( ; it.current(); ++it )
    it.current()->print( indent + 8 );
}


/*int main( int argc, char** argv )
{
  KTar tar( argv[1] );

  if ( !tar.open( IO_WriteOnly ) )
  {
    printf("Could not open %s for writing\n", argv[1] );
    return 1;
  }

  tar.writeFile( "test1", "weis", "users", 5, "Hallo" );
  tar.writeFile( "test2", "weis", "users", 8, "Hallo Du" );
  tar.writeFile( "mydir/test3", "weis", "users", 13, "Noch so einer" );

  tar.close();

  printf("-----------------------\n");

  if ( !tar.open( IO_ReadOnly ) )
  {
    printf("Could not open %s for reading\n", argv[1] );
    return 1;
  }

  const KTarDirectory* dir = tar.directory();
  dir->print();

  const KTarEntry* e = dir->entry( "mydir/test3" );
  ASSERT( e && e->isFile() );
  const KTarFile* f = (KTarFile*)e;

  QByteArray arr( f->data() );
  printf("SIZE=%i\n",arr.size() );
  QString str( arr );
  printf("DATA=%s\n", str.latin1());

  tar.close();

  return 0;
}*/
