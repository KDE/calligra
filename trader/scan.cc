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

bool fillFromFile( CORBA::Any& _any, const char* _filename )
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

/********************************************************
 *
 * Parts
 *
 ********************************************************/

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

  QString tmp = config.readEntry( "Icon" );
  if ( tmp.isEmpty() )
  {
    koScanPartsError( _file, "Icon" );
    return;
  }
  QString icon = kapp->kde_icondir().data();
  icon += "/";
  icon += tmp;
  
  tmp = config.readEntry( "MiniIcon" );
  if ( tmp.isEmpty() )
  {
    koScanPartsError( _file, "MiniIcon" );
    return;
  }
  QString mini_icon = kapp->kde_icondir().data();
  mini_icon += "/mini/";
  mini_icon += tmp;
  
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

  CosTrading::PropertySeq props;
  props.length( 8 );
  props[0].name = CORBA::string_dup( "Comment" );
  props[0].value <<= CORBA::Any::from_string( (char*)comment.data(), 0 );
  props[1].name = CORBA::string_dup( "Name" );
  props[1].value <<= CORBA::Any::from_string( (char*)name.data(), 0 );
  props[2].name = CORBA::string_dup( "Exec" );
  props[2].value <<= CORBA::Any::from_string( (char*)cmd.data(), 0 );
  props[3].name = CORBA::string_dup( "ActivationMode" );
  props[3].value <<= CORBA::Any::from_string( (char*)mode.data(), 0 );
  props[4].name = CORBA::string_dup( "Icon" );
  if ( !fillFromFile( props[4].value, icon ) )
  {
    koScanPartsError( _file, "Icon" );
    return;
  }    
  // props[4].value <<= CORBA::Any::from_string( (char*)icon.data(), 0 );
  props[5].name = CORBA::string_dup( "MiniIcon" );
  if ( !fillFromFile( props[5].value, mini_icon ) )
  {
    koScanPartsError( _file, "MiniIcon" );
    return;
  }    
  // props[5].value <<= CORBA::Any::from_string( (char*)mini_icon.data(), 0 );
  CosTrading::StringList lst;
  const char *s = repoids.first();
  lst.length( repoids.count() );
  for( unsigned int i = 0; i < repoids.count(); ++i )
  {    
    lst[i] = CORBA::string_dup( s );
    s = repoids.next();
  }
  props[6].name = CORBA::string_dup( "RepoID" );
  props[6].value <<= lst;
  s = mimes.first();
  lst.length( mimes.count() );
  for( unsigned int i = 0; i < mimes.count(); ++i )
  {    
    lst[i] = CORBA::string_dup( s );
    s = mimes.next();
  }
  props[7].name = CORBA::string_dup( "MimeTypes" );
  props[7].value <<= lst;
  

  CosTrading::ServiceTypeName_var stype;
  stype = CORBA::string_dup( "KOfficeDocument" );
  
  CORBA::Object_var obj = Activator::self()->createReference( name, "IDL:KOffice/DocumentFactory:1.0" );

  // Register at IMR
  imrCreate( name, mode, cmd, repoids.first(), _imr );

  // Register the service
  CosTrading::OfferId_var id;
  CosTrading::Register_var reg = _trader->register_if();
  id = reg->export( obj, stype, props );
}

/********************************************************
 *
 * Filters
 *
 ********************************************************/

void koScanFiltersError( const char *_file, const char *_entry )
{
  warning( i18n( "The filters config file\n%s\ndoes not conatain a %s=... entry" ), _file, _entry );
}

void koScanFilters( Trader *_trader, CORBA::ORB_ptr _orb )
{
  CORBA::ImplRepository_var imr;
  CORBA::Object_var obj = _orb->resolve_initial_references( "ImplementationRepository" );
  imr = CORBA::ImplRepository::_narrow( obj );
  assert( !CORBA::is_nil( imr ) );
  
  koScanFilters( _trader, imr );
}

void koScanFilters( Trader *_trader, CORBA::ImplRepository_ptr _imr )
{
  QString p = kapp->kde_datadir().copy();
  p += "/koffice/filterlnk";
  koScanFilters( _trader, p, _imr );
}

void koScanFilters( Trader *_trader, const char* _path, CORBA::ImplRepository_ptr _imr )
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
	koScanFilters( _trader, file, _imr );
      }
      else if ( tmp.length() > 7 && tmp.right( 7 ) == ".kdelnk" )
      {
	FILE *f = fopen( file, "r" );
	if ( f == 0L )
	{
	  fclose( f );
	  continue;
	}
	
	koScanFilterFile( _trader, file, _imr );
      }
    }
  }
  
  closedir( dp );
}

void koScanFilterFile( Trader *_trader, const char* _file, CORBA::ImplRepository_ptr _imr )
{
  KSimpleConfig config( _file, true );
  config.setGroup( "KDE Desktop Entry" );
  
  QString type = config.readEntry( "Type" );
  if ( type.isEmpty() )
  {
    koScanFiltersError( _file, "Type" );
    return;
  }
  if ( type != "KOfficeFilter" )
  {
    warning("Not an open Filter\n");
    return;
  }
  
  QString cmd = config.readEntry( "Exec" );
  if ( cmd.isEmpty() )
  {
    koScanFiltersError( _file, "Exec" );
    return;
  }

  QString tmp = config.readEntry( "Icon" );
  if ( tmp.isEmpty() )
  {
    koScanPartsError( _file, "Icon" );
    return;
  }
  QString icon = kapp->kde_icondir().data();
  icon += "/";
  icon += tmp;
  
  tmp = config.readEntry( "MiniIcon" );
  if ( tmp.isEmpty() )
  {
    koScanPartsError( _file, "MiniIcon" );
    return;
  }
  QString mini_icon = kapp->kde_icondir().data();
  mini_icon += "/mini/";
  mini_icon += tmp;

  QStrList repoids;
  if ( config.readListEntry( "RepoID", repoids ) == 0 || repoids.isEmpty() )
  {
    koScanFiltersError( _file, "RepoID" );
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
  QString imports = config.readEntry( "Import" );
  if ( imports.isEmpty() )
  {
    koScanFiltersError( _file, "Import" );
    return;
  }
  QString importDescription = config.readEntry( "ImportDescription" );
  if ( importDescription.isEmpty() )
  {
    koScanFiltersError( _file, "ImportDescription" );
    return;
  }
  QString exports = config.readEntry( "Export" );
  if ( exports.isEmpty() )
  {
    koScanFiltersError( _file, "Export" );
    return;
  }
  QString exportDescription = config.readEntry( "ExportDescription" );
  if ( exportDescription.isEmpty() )
  {
    koScanFiltersError( _file, "ExportDescription" );
    return;
  }
  
  QString comment = config.readEntry( "Comment" );
  if ( comment.isEmpty() )
    comment = name.data();
  QString mode = config.readEntry( "ActivationMode" );
  if ( mode.isEmpty() )
  {
    koScanFiltersError( _file, "ActivationMode" );
    return;
  }

  imrCreate( name, mode, cmd, repoids.first(), _imr );

  CosTrading::PropertySeq props;
  props.length( 11 );
  props[0].name = CORBA::string_dup( "Comment" );
  props[0].value <<= CORBA::Any::from_string( (char*)comment.data(), 0 );
  props[1].name = CORBA::string_dup( "Name" );
  props[1].value <<= CORBA::Any::from_string( (char*)name.data(), 0 );
  props[2].name = CORBA::string_dup( "Exec" );
  props[2].value <<= CORBA::Any::from_string( (char*)cmd.data(), 0 );
  props[3].name = CORBA::string_dup( "ActivationMode" );
  props[3].value <<= CORBA::Any::from_string( (char*)mode.data(), 0 );
  props[4].name = CORBA::string_dup( "Icon" );
  if ( !fillFromFile( props[4].value, icon ) )
  {
    koScanPartsError( _file, "Icon" );
    return;
  }    
  props[5].name = CORBA::string_dup( "MiniIcon" );
  if ( !fillFromFile( props[5].value, mini_icon ) )
  {
    koScanPartsError( _file, "MiniIcon" );
    return;
  }    

  CosTrading::StringList lst;
  const char *s = repoids.first();
  lst.length( repoids.count() );
  for( unsigned int i = 0; i < repoids.count(); ++i )
  {    
    lst[i] = CORBA::string_dup( s );
    s = repoids.next();
  }
  props[6].name = CORBA::string_dup( "RepoID" );
  props[6].value <<= lst;
  props[7].name = CORBA::string_dup( "Import" );
  props[7].value <<= CORBA::Any::from_string( (char*)imports.data(), 0 );
  props[8].name = CORBA::string_dup( "Export" );
  props[8].value <<= CORBA::Any::from_string( (char*)exports.data(), 0 );
  props[9].name = CORBA::string_dup( "ImportDescription" );
  props[9].value <<= CORBA::Any::from_string( (char*)importDescription.data(), 0 );
  props[10].name = CORBA::string_dup( "ExportDescription" );
  props[10].value <<= CORBA::Any::from_string( (char*)exportDescription.data(), 0 );

  CosTrading::ServiceTypeName_var stype;
  stype = CORBA::string_dup( "KOfficeFilter" );
  
  CORBA::Object_var obj = Activator::self()->createReference( name, "IDL:KOffice/FilterFactory:1.0" );

  // Register the service
  CosTrading::OfferId_var id;
  CosTrading::Register_var reg = _trader->register_if();
  id = reg->export( obj, stype, props );
}
