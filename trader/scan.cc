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

void koScanPartsError( const char *_file, const char *_entry )
{
  warning( i18n( "The OpenParts config file\n%s\ndoes not conatain a %s=... entry" ), _file, _entry );
}

void koScanParts( Trader *_trader, CORBA::ORB_ptr _orb )
{
  CORBA::ImplRepository_var imr;
  CORBA::Object_var obj = _orb->resolve_initial_references( "ImplementationRepository" );
  imr = CORBA::ImplRepository::_narrow( obj );
  assert( !CORBA::is_nil( imr ) );
  
  koScanParts( _trader, imr );
}

void koScanParts( Trader *_trader, CORBA::ImplRepository_ptr _imr )
{
  QString p = kapp->kde_datadir().copy();
  p += "/koffice/partlnk";
  koScanParts( _trader, p, _imr );
}

void koScanParts( Trader *_trader, const char* _path, CORBA::ImplRepository_ptr _imr )
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
	koScanParts( _trader, file, _imr );
      }
      else if ( tmp.length() > 7 && tmp.right( 7 ) == ".kdelnk" )
      {
	FILE *f = fopen( file, "r" );
	if ( f == 0L )
	{
	  fclose( f );
	  continue;
	}
	
	koScanPartFile( _trader, file, _imr );
      }
    }
  }
  
  closedir( dp );
}

void koScanPartFile( Trader *_trader, const char* _file, CORBA::ImplRepository_ptr _imr )
{
  KSimpleConfig config( _file, true );
  config.setGroup( "KDE Desktop Entry" );
  
  QString type = config.readEntry( "Type" );
  if ( type.isEmpty() )
  {
    koScanPartsError( _file, "Type" );
    return;
  }
  if ( type != "OpenPart" )
  {
    warning("Not an open part\n");
    return;
  }
  
  QString cmd = config.readEntry( "Exec" );
  if ( cmd.isEmpty() )
  {
    koScanPartsError( _file, "Exec" );
    return;
  }
  QString icon = config.readEntry( "Icon" );
  if ( icon.isEmpty() )
  {
    koScanPartsError( _file, "Icon" );
    return;
  }
  QString mini_icon = config.readEntry( "MiniIcon" );
  if ( mini_icon.isEmpty() )
  {
    koScanPartsError( _file, "MiniIcon" );
    return;
  }
  QStrList repoids;
  if ( config.readListEntry( "RepoID", repoids ) == 0 || repoids.isEmpty() )
  {
    koScanPartsError( _file, "RepoID" );
    return;
  }
  QStrList mimes;
  if ( config.readListEntry( "MimeTypes", mimes ) == 0 || mimes.isEmpty() )
  {
    koScanPartsError( _file, "MimeTypes" );
    return;
  }
  QString name = config.readEntry( "Name" );
  if ( name.isEmpty() )
  {
    QString tmp( _file );
    int i = tmp.findRev( "/" );
    if ( i == -1 )
      return;
    name = tmp.right( tmp.length() - i - 1 );
    if ( name.isEmpty() )
      return;
  }
  
  QString comment = config.readEntry( "Comment" );
  if ( comment.isEmpty() )
    comment = name.data();
  QString mode = config.readEntry( "ActivationMode" );
  if ( mode.isEmpty() )
  {
    koScanPartsError( _file, "ActivationMode" );
    return;
  }

  imrCreate( name, mode, cmd, repoids.first(), _imr );

  CosTrading::PropertySeq props;
  props.length( 8 );
  props[0].is_file = false;
  props[0].name = CORBA::string_dup( "Comment" );
  props[0].value <<= CORBA::Any::from_string( (char*)comment.data(), 0 );
  props[1].is_file = false;
  props[1].name = CORBA::string_dup( "Name" );
  props[1].value <<= CORBA::Any::from_string( (char*)name.data(), 0 );
  props[2].is_file = false;
  props[2].name = CORBA::string_dup( "Exec" );
  props[2].value <<= CORBA::Any::from_string( (char*)cmd.data(), 0 );
  props[3].is_file = false;
  props[3].name = CORBA::string_dup( "ActivationMode" );
  props[3].value <<= CORBA::Any::from_string( (char*)mode.data(), 0 );
  props[4].is_file = true;
  props[4].name = CORBA::string_dup( "Icon" );
  props[4].value <<= CORBA::Any::from_string( (char*)icon.data(), 0 );
  props[5].is_file = true;
  props[5].name = CORBA::string_dup( "MiniIcon" );
  props[5].value <<= CORBA::Any::from_string( (char*)mini_icon.data(), 0 );
  CosTrading::StringList lst;
  const char *s = repoids.first();
  lst.length( repoids.count() );
  for( unsigned int i = 0; i < repoids.count(); ++i )
  {    
    lst[i] = CORBA::string_dup( s );
    s = repoids.next();
  }
  props[6].is_file = false;
  props[6].name = CORBA::string_dup( "RepoID" );
  props[6].value <<= lst;
  s = mimes.first();
  lst.length( mimes.count() );
  for( unsigned int i = 0; i < mimes.count(); ++i )
  {    
    lst[i] = CORBA::string_dup( s );
    s = mimes.next();
  }
  props[7].is_file = false;
  props[7].name = CORBA::string_dup( "MimeTypes" );
  props[7].value <<= lst;
  

  CosTrading::ServiceTypeName_var stype;
  stype = CORBA::string_dup( "KOfficeDocument" );
  
  CORBA::Object_var obj = Activator::self()->createReference( name, "IDL:KOffice/DocumentFactory:1.0" );

  // Register the service
  CosTrading::OfferId_var id;
  id = _trader->export( obj, stype, props );
}
