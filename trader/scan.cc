#include "scan.h"
#include "trader_main.h"
#include "activator.h"
#include "init.h"
#include "trader.h"

#include <kapp.h>
#include <klocale.h>
#include "ksimpleconfig.h"

#include <qstring.h>
#include <qstrlist.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>    
#include <sys/stat.h>

/************************************************
 *
 * Helper functions
 *
 ************************************************/

static bool fillFromFile( CORBA::Any& _any, const char* _filename );
static void scanEntry( Trader *_trader, const char* _file, CORBA::ImplRepository_ptr _imr );
static void scanDirectory( Trader *_trader, const char* _path, CORBA::ImplRepository_ptr _imr );

static bool fillFromFile( CORBA::Any& _any, const char* _filename )
{
  struct stat buff;
  stat( _filename, &buff );
  int size = buff.st_size;
    
  FILE *f = fopen( _filename, "rb" );
  if ( !f )
  {
    cerr << "Could not open " << _filename << endl;
    return false;
  }

  char *p = new char[ size + 1 ];
  int n = fread( p, 1, size, f );
  p[n] = 0;
  fclose( f );
  _any <<= CORBA::Any::from_string( (char *) p, 0 );
  delete []p;

  return true;
}

static void scanError( const char *_file, const char *_entry )
{
  warning( i18n( "The OpenParts config file\n%s\ndoes not conatain a %s=... entry" ), _file, _entry );
}

static void scanDirectory( Trader *_trader, const char* _path,
			   CORBA::ImplRepository_ptr _imr )
{   
  DIR *dp;
  struct dirent *ep;
  dp = opendir( _path );
  if ( dp == 0L )
    return;
  
  // Loop thru all directory entries
  while ( ( ep = readdir( dp ) ) != 0L )
  {
    if ( strcmp( ep->d_name, "." ) != 0 && strcmp( ep->d_name, ".." ) != 0 )
    {
      QString tmp = ep->d_name;
      QString file = _path;
      file += "/";
      file += ep->d_name;
      struct stat buff;
      stat( file.data(), &buff );
      if ( S_ISDIR( buff.st_mode ) )
      {
	scanDirectory( _trader, file, _imr );
      }
      else if ( tmp.length() > 7 && tmp.right( 7 ) == ".kdelnk" )
      {
	FILE *f = fopen( file, "r" );
	if ( f == 0L )
	  continue;	
	else
	  fclose( f );

	scanEntry( _trader, file, _imr );
      }
    }
  }
  
  closedir( dp );
}

static void scanEntry( Trader *_trader, const char* _file, CORBA::ImplRepository_ptr _imr )
{
  KSimpleConfig config( _file, true );
  config.setGroup( "KDE Desktop Entry" );
  
  // Is it really one of the kdelnk files we want to see ?
  QString type = config.readEntry( "Type" );
  if ( type.isEmpty() )
  {
    scanError( _file, "Type" );
    return;
  }
  if ( type != "OpenPart" )
  {
    warning("Not an open part\n");
    return;
  }
  
  // Read informations which are needed to register this
  // service at the ImplRepo
  QString cmd = config.readEntry( "Exec" );
  if ( cmd.isEmpty() )
  {
    scanError( _file, "Exec" );
    return;
  }
  
  QStrList repoids;
  if ( config.readListEntry( "RepoID", repoids ) == 0 || repoids.isEmpty() )
  {
    scanError( _file, "RepoID" );
    return;
  }

  QString mode = config.readEntry( "ActivationMode" );
  if ( mode.isEmpty() )
  {
    scanError( _file, "ActivationMode" );
    return;
  }

  QString name = config.readEntry( "Name" );
  if ( name.isEmpty() )
  {
    scanError( _file, "Name" );
    return;
  }

  // Special handling for icons and mini icons
  QString tmp = config.readEntry( "Icon" );
  if ( tmp.isEmpty() )
  {
    scanError( _file, "Icon" );
    return;
  }
  QString icon = kapp->kde_icondir().data();
  icon += "/";
  icon += tmp;
  
  tmp = config.readEntry( "MiniIcon" );
  if ( tmp.isEmpty() )
  {
    scanError( _file, "MiniIcon" );
    return;
  }
  QString mini_icon = kapp->kde_icondir().data();
  mini_icon += "/mini/";
  mini_icon += tmp;

  CosTradingRepos::ServiceTypeRepository_var repo;
  repo = _trader->serviceTypeRepository();
  assert( !CORBA::is_nil( repo ) );

  // We need to know about the servicetype
  QString stype = config.readEntry( "ServiceType" );
  if ( stype.isEmpty() )
  {
    scanError( _file, "ServiceType" );
    return;
  }

  // Now search informations about this service type
  CosTradingRepos::ServiceTypeRepository::TypeStruct_var ts;
  try
  {    
    ts = repo->fully_describe_type( stype );
  }
  catch( ... )
  {
    warning( "Unknown ServiceType %s", stype.data() );
    return;
  }

  CosTrading::PropertySeq props;
  props.length( ts->props.length() );
  CORBA::ULong len = 0;
  
  // Iterate over all properties
  for( CORBA::ULong l = 0; l < ts->props.length(); ++l )
  {
    // TODO: Check for mandatory properties
    CORBA::String_var n = ts->props[l].name;
    
    if ( strcmp( n.in(), "Icon" ) == 0L )
    {
      if ( !fillFromFile( props[len].value, icon ) )
      {
	scanError( _file, "Icon" );
	return;
      }    
      props[len].name = CORBA::string_dup( "Icon" );
    }
    else if ( strcmp( n.in(), "MiniIcon" ) == 0L )
    {
      if ( !fillFromFile( props[len].value, mini_icon ) )
      {
	scanError( _file, "Icon" );
	return;
      }    
      props[len].name = CORBA::string_dup( "MiniIcon" );
    }
    else
    {
      if ( ts->props[l].value_type->equaltype( CORBA::_tc_long ) )
      {
	CORBA::Long v = config.readLongNumEntry( n.in() );
	props[len].name = CORBA::string_dup( n.in() );
	props[len].value <<= v;
      }
      else if ( ts->props[l].value_type->equaltype( CORBA::_tc_float ) )
      {
	CORBA::Float v = config.readDoubleNumEntry( n.in() );
	props[len].name = CORBA::string_dup( n.in() );
	props[len].value <<= v;
      }
      else if ( ts->props[l].value_type->equaltype( CORBA::_tc_string ) )
      {
	QString v = config.readEntry( n.in() );
	props[len].name = CORBA::string_dup( n.in() );
	props[len].value <<= CORBA::Any::from_string( v.data(), 0 );
      }
      else if ( ts->props[l].value_type->equaltype( CORBA::_tc_boolean ) )
      {
	bool v = config.readBoolEntry( n.in() );
	props[len].name = CORBA::string_dup( n.in() );
	props[len].value <<= CORBA::Any::from_boolean( v );	
      }
      else if ( ts->props[l].value_type->equaltype( CosTrading::_tc_LongList ) )
      {
	// TODO
      }
      else if ( ts->props[l].value_type->equaltype( CosTrading::_tc_FloatList ) )
      {
	// TODO
      }
      else if ( ts->props[l].value_type->equaltype( CosTrading::_tc_StringList ) )
      {
	QStrList lst;
	config.readListEntry( n.in(), lst );
	CosTrading::StringList l2;
	l2.length( lst.count() );
	const char* s = lst.first();
	for( CORBA::ULong k = 0; k < lst.count(); ++k )
	{    
	  l2[k] = CORBA::string_dup( s );
	  s = lst.next();
	}
	props[len].name = CORBA::string_dup( n.in() );
	props[len].value <<= l2;
      }
    }
    len++;
  }
  
  // Finally determine how many properties we read.
  props.length( len );
  
  // Create a virtual object reference
  CORBA::Object_var obj = Activator::self()->createReference( name, repoids.first() );

  // Register at IMR
  imrCreate( name, mode, cmd, repoids.first(), _imr );

  // Register the service
  CosTrading::OfferId_var id;
  CosTrading::Register_var reg = _trader->register_if();
  id = reg->export( obj, stype, props );
}

/********************************************************
 *
 * KOfficeParts
 *
 ********************************************************/

void koScanOfficeParts( Trader *_trader, CORBA::ORB_ptr _orb )
{
  CORBA::ImplRepository_var imr;
  CORBA::Object_var obj = _orb->resolve_initial_references( "ImplementationRepository" );
  imr = CORBA::ImplRepository::_narrow( obj );
  assert( !CORBA::is_nil( imr ) );

  QString p = kapp->kde_datadir().copy();
  p += "/koffice/partlnk";
  scanDirectory( _trader, p, imr );
}

/********************************************************
 *
 * Filters
 *
 ********************************************************/

void koScanFilters( Trader *_trader, CORBA::ORB_ptr _orb )
{
  CORBA::ImplRepository_var imr;
  CORBA::Object_var obj = _orb->resolve_initial_references( "ImplementationRepository" );
  imr = CORBA::ImplRepository::_narrow( obj );
  assert( !CORBA::is_nil( imr ) );

  QString p = kapp->kde_datadir().copy();
  p += "/koffice/filterlnk";
  scanDirectory( _trader, p, imr );
}

/********************************************************
 *
 * KParts
 *
 ********************************************************/

void koScanParts( Trader *_trader, CORBA::ORB_ptr _orb )
{
  CORBA::ImplRepository_var imr;
  CORBA::Object_var obj = _orb->resolve_initial_references( "ImplementationRepository" );
  imr = CORBA::ImplRepository::_narrow( obj );
  assert( !CORBA::is_nil( imr ) );

  QString p = kapp->kde_datadir().copy();
  p += "/../partlnk";
  scanDirectory( _trader, p, imr );
}
