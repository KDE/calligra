#include "trader_main.h"
#include "typerepo_impl.h"

#include <k2config.h>
#include <k2url.h>
#include <string.h>
#include <kapp.h>

map<string,string>* g_mapServiceType2Interface = 0L;

bool imrCreate( const char* _name, const char* _mode, const char *_exec, const char* _iface, CORBA::ImplRepository_ptr _imr )
{
  CORBA::ImplRepository::ImplDefSeq_var impls = _imr->find_by_name( _name );
  if ( impls->length() > 0 )
  {
    cerr << "there is already a server called '"
	 << _name << "', please choose a unique name!"
	 << endl;
    return false;
  }

  CORBA::ImplementationDef::ActivationMode mode;
  if ( strcmp( _mode, "persistent" ) == 0 )
    mode = CORBA::ImplementationDef::ActivatePersistent;
  else if ( strcmp( _mode, "shared") == 0 )
    mode = CORBA::ImplementationDef::ActivateShared;
  else if ( strcmp( _mode, "unshared") == 0 )
    mode = CORBA::ImplementationDef::ActivateUnshared;
  else if ( strcmp( _mode, "permethod") == 0 )
    mode = CORBA::ImplementationDef::ActivatePerMethod;
  else if ( strcmp( _mode, "library") == 0 )
    mode = CORBA::ImplementationDef::ActivateLibrary;
  else
  {
    cerr << "invalid activation mode: " << _mode << endl;
    cerr << "valid modes are: persistent, shared, unshared, "
	 << "permethod, library"
	 << endl;
    return false;
  }

  CORBA::ImplementationDef::RepoIdList repoids;
  repoids.length( 1 );
  repoids[0] = CORBA::string_dup( (const char*)_iface );

  CORBA::ImplementationDef_var impl = _imr->create( mode, repoids, _name, _exec );
  assert (!CORBA::is_nil (impl));
  return true;
}

void initServiceTypes( Trader* _trader )
{
  g_mapServiceType2Interface = new map<string,string>;
  
  TypeRepository* repo = _trader->hack();

  QString path = KApplication::kde_datadir().copy();
  path += "/koffice/trader/servicetypes";
  K2Config g( path );
  K2Config::iterator it = g.begin();
  for( ; it != g.end(); it++ )
  {
    CosTradingRepos::ServiceTypeRepository::PropStructSeq seq;
    CosTradingRepos::ServiceTypeRepository::ServiceTypeNameSeq superseq;
    CosTradingRepos::ServiceTypeRepository::PropertySeq valueseq;

    if ( it.group() && strcmp( it.group()->type(), "servicetype" ) == 0 )
    {       
      cerr << "TYPE: " << it.group()->name() << endl;
      list<string> lst;
      if ( it.group()->readStringList( "SuperTypes", lst ) )
      {
	superseq.length( lst.size() );
	list<string>::iterator i = lst.begin();
	int j = 0;
	for( ; i != lst.end(); i++ )
	  superseq[ j++ ] = CORBA::string_dup( i->c_str() );
      }
      else
	superseq.length( 0 );
      
      K2Config* props;
      if ( ( props = it.group()->findGroup( "properties" ) ) )
      {
	list<CosTradingRepos::ServiceTypeRepository::PropStruct> plist;
	K2Config::iterator it2 = props->begin();
	for( ; it2 != props->end(); it2++ )
        {
	  string type;
	  if ( it2.group() && strcmp( it2.group()->type(), "property" ) == 0 &&
	       it2.group()->readString( "Type", type ) )
	  {
	    cerr << "  PROP: " << it2.group()->name() << endl;
	    cerr << "    Type: " << type << endl;
	    
	    CosTradingRepos::ServiceTypeRepository::PropStruct p;
	    p.name = CORBA::string_dup( it2.group()->name() );

	    p.mode = CosTradingRepos::ServiceTypeRepository::PROP_NORMAL;
	    if ( type == "boolean" )
	      p.value_type = CORBA::_tc_boolean;
	    else if ( type == "string" )
	      p.value_type = CORBA::_tc_string;
	    else if ( type == "long" )
	      p.value_type = CORBA::_tc_long;
	    else if ( type == "float" )
	      p.value_type = CORBA::_tc_float;
	    else if ( type == "@string" )
	      p.value_type = CosTrading::_tc_StringList;
	    else if ( type == "@long" )
	      p.value_type = CosTrading::_tc_LongList;	    
	    else if ( type == "@float" )
	      p.value_type = CosTrading::_tc_FloatList;
	    else
	    {
	      cerr << "Unknown property type " << it2.item()->text() << " in property " << it2.item()->name()
		   << " in servicetype " << it.group()->name() << endl;
	      continue;
	    }
	    
	    plist.push_back( p );
	  }
	}
	seq.length( plist.size() );
	list<CosTradingRepos::ServiceTypeRepository::PropStruct>::iterator it3 = plist.begin();
	int i = 0;  
	for( ; it3 != plist.end(); it3++ )
	  seq[ i++ ] = *it3;
      }
      else
	seq.length( 0 );

      string interface;
      if ( !it.group()->readString( "Interface", interface ) )
      {
	cerr << it.group()->name() << " does not have an interface entry" << endl;
	continue;
      }
      /*
      list<CosTradingRepos::ServiceTypeRepository::Property> values;
      K2Config::iterator it2 = it.group()->begin();
      for( ; it2 != it.group()->end(); it2++ )
      {
	if ( it2.item() && strcmp( it2.item()->name(), "Interface" ) != 0 &&
	     strcmp( it2.item()->name(), "SuperTypes" ) != 0 )
	{	
	  K2ConfigItem *item = it2.item();
	  CosTradingRepos::ServiceTypeRepository::Property p;
	  p.name = CORBA::string_dup( item->name() );
	  p.is_file = false;
	  
	  switch( item->type() )
	  {
	  case K2ConfigItem::TBOOLEAN:
	    p.value <<= CORBA::Any::from_boolean( item->boolean() );
	    p.value_type = CORBA::_tc_boolean;
	    break;
	  case K2ConfigItem::TSTRING:
	    p.value <<= CORBA::Any::from_string( (char *) item->text(), 0 );
	    p.value_type = CORBA::_tc_string;
	    break;
	  case K2ConfigItem::TSTRING_LIST:
	    {	      
	      CosTrading::StringList l;
	      l.length( item->stringList().size() );
	      list<string>::iterator it = item->stringList().begin();
	      int i = 0;
	      for( ; it != item->stringList().end(); ++it )
		l[ i++ ] = CORBA::string_dup( it->c_str() );
	      
	      p.value <<= l;
	      p.value_type = CosTrading::_tc_StringList;
	    }
	    break;
	  case K2ConfigItem::TLONG:
	    p.value <<= (CORBA::Long)item->integer();
	    p.value_type = CORBA::_tc_long;
	    break;
	  case K2ConfigItem::TFLOAT:
	    p.value <<= (CORBA::Float)item->fp();
	    p.value_type = CORBA::_tc_float;
	    break;
	  case K2ConfigItem::TFILE:
	    p.value <<= CORBA::Any::from_string( (char *) item->fileName(), 0 );
	    p.value_type = CORBA::_tc_string;
	    p.is_file = true;
	    break;
	  default:
	    cerr << "Type of item " << item->name() << " in service type " << it.group()->name() << " not allowed" << endl;
	    continue;
	  }

	  values.push_back( p );
	}
      }
      valueseq.length( values.size() );
      list<CosTradingRepos::ServiceTypeRepository::Property>::iterator it3 = values.begin();
      int k = 0;
      for( ; it3 != values.end(); it3++ )
	valueseq[ k++ ] = *it3;
      */
      valueseq.length( 0 );
      
      repo->add_type( (char*)it.group()->name(), (char*)interface.c_str(), seq, superseq, valueseq );

      (*g_mapServiceType2Interface)[ it.group()->name() ] = interface;
    }
  }
}
