#include "typerepo_impl.h"

#include <string>
#include <list>
#include <iostream>

#include <string.h>

bool operator< ( const CosTradingRepos::ServiceTypeRepository::IncarnationNumber& i1,
		 const CosTradingRepos::ServiceTypeRepository::IncarnationNumber& i2 );

bool operator!= ( const CosTradingRepos::ServiceTypeRepository::PropStruct& p1,
		  const CosTradingRepos::ServiceTypeRepository::PropStruct& p2 );

TypeRepository::TypeRepository( Trader* _trader )
{
  m_pTrader = _trader;

  m_incarnation.high = 0;
  m_incarnation.low = 0;
}

CosTradingRepos::ServiceTypeRepository::IncarnationNumber TypeRepository::incarnation()
{
  return m_incarnation;
}

CosTradingRepos::ServiceTypeRepository::IncarnationNumber
TypeRepository::add_type( const char* name,
			  const char* if_name,
			  const CosTradingRepos::ServiceTypeRepository::PropStructSeq& props,
			  const CosTradingRepos::ServiceTypeRepository::ServiceTypeNameSeq& super_types,
			  const CosTradingRepos::ServiceTypeRepository::PropertySeq& values )
{
  /**
   * Find duplicate entries
   */
  map<string,CosTradingRepos::ServiceTypeRepository::TypeStruct>::iterator it = m_mapTypes.find( static_cast<const char*>(name) );
  if ( it != m_mapTypes.end() )
  {
    CosTradingRepos::ServiceTypeRepository::DuplicateServiceTypeName exc;
    exc.name = CORBA::string_dup( name );
    throw exc;
  }

  /**
   * Do all super types exist ?
   */
  int len = super_types.length();
  for( int i = 0; i < len; i++ )
  {
    map<string,CosTradingRepos::ServiceTypeRepository::TypeStruct>::iterator it =
      m_mapTypes.find( static_cast<const char*>( super_types[ i ] ) );

    if ( it == m_mapTypes.end() )
    {
      CosTrading::UnknownServiceType exc;
      exc.type = CORBA::string_dup( super_types[ i ] );
      throw exc;
    }
  }
    
  /**
   * Create the new type
   */
  TypeStruct type;

  type.if_name = CORBA::string_dup( if_name );
  type.props = props;
  type.super_types = super_types;
  type.masked = false;
  type.incarnation = m_incarnation;
  type.values = values;
  
  /**
   * Check for duplicate property names
   */
  map<string,bool> mp;
  for( unsigned int j = 0; j < type.props.length(); j++ )
  {
    map<string,bool>::iterator it = mp.find( static_cast<const char*>(type.props[j].name) );
    if ( it != mp.end() )
    {
      CosTrading::DuplicatePropertyName exc;
      exc.name = CORBA::string_dup( type.props[j].name );
      throw exc;
    }
    mp[ static_cast<const char*>(type.props[j].name) ] = true;
  }
  
  /**
   * Check for redefinitions of properties
   */
  // Make a copy
  TypeStruct t2 = type;
  // Recursion over all super types to get all properties
  for( unsigned int k = 0; k < type.super_types.length(); k++ )
  {
    map<string,CosTradingRepos::ServiceTypeRepository::TypeStruct>::iterator it2 = checkServiceType( type.super_types[ k ] );
    fully_describe_type( &t2, it2->second );
  }
  // Find duplicate properties
  map<string,CosTradingRepos::ServiceTypeRepository::PropStruct> map2;
  for( unsigned int l = 0; l < t2.props.length(); l++ )
  {
    map<string,CosTradingRepos::ServiceTypeRepository::PropStruct>::iterator it =
      map2.find( static_cast<const char*>(t2.props[l].name) );
    if ( it != map2.end() )
    {
      // Compare properties
      if ( t2.props[l] != it->second )
      {
	CosTradingRepos::ServiceTypeRepository::ValueTypeRedefinition exc;
	exc.type_1 = CORBA::string_dup( "HACK" );
	exc.type_2 = CORBA::string_dup( "HACK" );
	exc.definition_1 = t2.props[l];
	exc.definition_2 = it->second;
	throw exc;
      }
    }
    map2[ static_cast<const char*>(t2.props[l].name) ] = t2.props[l];
  }

  // TODO: Check wethe the interface is really a subtype of the super types interfaces.
  //       Use the IR. Dont throw an error if the interfaces are unknown

  /**
   * Add the new type
   */
  m_mapTypes[ static_cast<const char*>( name ) ] = type;
  
  incIncarnationNumber();
  
  return type.incarnation;
}

void TypeRepository::remove_type( const char* name )
{
  /**
   * Does the service type exist ?
   */
  map<string,CosTradingRepos::ServiceTypeRepository::TypeStruct>::iterator it = checkServiceType( name );
  
  /**
   * Check wether it has sub types.
   */
  // Iterate over all registered sub types
  map<string,CosTradingRepos::ServiceTypeRepository::TypeStruct>::iterator it2 = m_mapTypes.begin();
  for( ; it2 != m_mapTypes.end(); ++it2 )
  {
    // Iterate over all super types
    for( int i = 0; it2->second.super_types.length(); i++ )
    {
      if ( strcmp( it2->second.super_types[i], name ) == 0  )
      {
	CosTradingRepos::ServiceTypeRepository::HasSubTypes exc;
	exc.the_type = name;
	exc.sub_type = CORBA::string_dup( it2->first.c_str() );
	throw exc;
      }
    }
  }
  
  /**
   * Remove the service type.
   */
  m_mapTypes.erase( it );
}

CosTradingRepos::ServiceTypeRepository::ServiceTypeNameSeq*
TypeRepository::list_types( const CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes& which_types )
{
  /**
   * Handle the switch
   */
  bool all = true;
  CosTradingRepos::ServiceTypeRepository::IncarnationNumber inc;
  
  if ( which_types._d() == CosTradingRepos::ServiceTypeRepository::since )
  {
    all = false;
    inc = which_types.incarnation();
  }

  /**
   * Find list of service types
   */
  list<string> lst;
  map<string,CosTradingRepos::ServiceTypeRepository::TypeStruct>::iterator it = m_mapTypes.begin();
  for( ; it != m_mapTypes.end(); ++it )
  {
    if ( all )
    {
      lst.push_back( it->first );
    }
    else
    {
      CosTradingRepos::ServiceTypeRepository::IncarnationNumber i = it->second.incarnation;
      if ( i > m_incarnation )
	lst.push_back( it->first );
    }
  }
  
  /**
   * Create answer
   */
  CosTradingRepos::ServiceTypeRepository::ServiceTypeNameSeq *seq = new CosTradingRepos::ServiceTypeRepository::ServiceTypeNameSeq;
  seq->length( lst.size() );
  
  list<string>::iterator it2 = lst.begin();
  int k = 0;
  for( ; it2 != lst.end(); ++it2 )
    (*seq)[ k++ ] = CORBA::string_dup( it2->c_str() );
    
  return seq;
}

CosTradingRepos::ServiceTypeRepository::TypeStruct* TypeRepository::describe_type( const char* name )
{
  /**
   * Does the service type exist ?
   */
  map<string,CosTradingRepos::ServiceTypeRepository::TypeStruct>::iterator it = checkServiceType( name );
  
  CosTradingRepos::ServiceTypeRepository::TypeStruct* t = new CosTradingRepos::ServiceTypeRepository::TypeStruct;
  *t = it->second;

  completeTypeStruct( t );
  
  return t;
}

CosTradingRepos::ServiceTypeRepository::TypeStruct*
TypeRepository::fully_describe_type( const char* name )
{
  /**
   * Does the service type exist ?
   */
  map<string,CosTradingRepos::ServiceTypeRepository::TypeStruct>::iterator it = checkServiceType( name );
  
  CosTradingRepos::ServiceTypeRepository::TypeStruct* t = new CosTradingRepos::ServiceTypeRepository::TypeStruct;
  *t = it->second;

  /**
   * Recursion over all super types
   */
  for( unsigned int i = 0; i < t->super_types.length(); i++ )
  {
    map<string,CosTradingRepos::ServiceTypeRepository::TypeStruct>::iterator it2 = checkServiceType( t->super_types[ i ] );
    fully_describe_type( t, it2->second );
  }
    
  completeTypeStruct( t );

  return t;
}

void TypeRepository::fully_describe_type( CosTradingRepos::ServiceTypeRepository::TypeStruct* result,
					  CosTradingRepos::ServiceTypeRepository::TypeStruct& super_type )
{
  /**
   * Add properties
   */
  int len = result->props.length();
  result->props.length( len + super_type.props.length() );
  for( unsigned int k = 0; k < super_type.props.length(); k++ )
  {
    result->props[ len + k ] = super_type.props[ k ];
  }

  /**
   * Add values.
   */
  len = result->values.length();
  result->values.length( len + super_type.values.length() );
  for( unsigned int j = 0; j < super_type.values.length(); j++ )
  {
    result->values[ len + j ] = super_type.values[ j ];
  }

  /**
   * Recursion over all super types
   */
  for( unsigned int i = 0; i < super_type.super_types.length(); i++ )
  {
    // Add supertypes
    int len = result->super_types.length();
    result->super_types.length( len + 1 );
    result->super_types[ len ] = super_type.super_types[ i ];

    // Recursion
    map<string,CosTradingRepos::ServiceTypeRepository::TypeStruct>::iterator it = checkServiceType( super_type.super_types[ i ] );
    fully_describe_type( result, it->second );
  }
}

void TypeRepository::mask_type( const char* name )
{
  /**
   * Does the service type exist ?
   */
  map<string,CosTradingRepos::ServiceTypeRepository::TypeStruct>::iterator it = checkServiceType( name );

  /**
   * Is it already masked ?
   */
  if ( it->second.masked )
  {
    CosTradingRepos::ServiceTypeRepository::AlreadyMasked esc;
    esc.name = CORBA::string_dup( name );
    throw esc;
  }
  
  /**
   * Mask it
   */
  it->second.masked = true;
}

void TypeRepository::unmask_type( const char* name )
{
  /**
   * Does the service type exist ?
   */
  map<string,CosTradingRepos::ServiceTypeRepository::TypeStruct>::iterator it = checkServiceType( name );

  /**
   * Is it not masked ?
   */
  if ( !it->second.masked )
  {
    CosTradingRepos::ServiceTypeRepository::NotMasked esc;
    esc.name = CORBA::string_dup( name );
    throw esc;
  }
  
  /**
   * Unmask it
   */
  it->second.masked = false;
}

/*******************************************************************
 *
 * Extensions for the internal use
 *
 *******************************************************************/

bool TypeRepository::isSubTypeOf( const char* sub, const char* super )
{
  // Check the trivial case
  if ( strcmp( sub, super ) == 0 )
    return true;
  
  CosTradingRepos::ServiceTypeRepository::TypeStruct_var v = fully_describe_type( sub );
  
  for( unsigned int i = 0; i < v->super_types.length(); i++ )
    if ( strcmp( v->super_types[i], super ) == 0 )
      return true;
  
  return false;
}

bool TypeRepository::isServiceTypeKnown( const char* name )
{
  /**
   * Does the service type exist ?
   */
  map<string,CosTradingRepos::ServiceTypeRepository::TypeStruct>::iterator it = m_mapTypes.find( static_cast<const char*>(name) );

  return ( it != m_mapTypes.end() );
}

/*******************************************************************
 *
 * Helper functions
 *
 *******************************************************************/

#include <stdio.h>
#include <sys/stat.h>

void TypeRepository::incIncarnationNumber()
{
  m_incarnation.low++;
  if ( m_incarnation.low == 0 )
    m_incarnation.high++;
}

void TypeRepository::completeTypeStruct( TypeStruct *t )
{
  for( unsigned int i = 0; i < t->values.length(); i++ )
  {
    if ( t->values[ i ].is_file )
    {
      char *s;
      if ( !( t->values[ i ].value >>= s ) )
      {
	cerr << "Value " << t->values[ i ].name << " is not of type string as expected" << endl;
	continue;
      }
      string filename = (const char*)s;
      CORBA::string_free( s );
      
      struct stat buff;
      stat( filename.c_str(), &buff );
      int size = buff.st_size;
    
      FILE *f = fopen( filename.c_str(), "rb" );
      if ( !f )
      {
	cerr << "Could not open " << filename << endl;
	continue;
      }

      char *p = new char[ size + 1 ];
      int n = fread( p, 1, size, f );
      p[n] = 0;
      fclose( f );
      t->values[i].value <<= CORBA::Any::from_string( (char *) p, 0 );
      delete []p;
    }
  }
}

bool operator!= ( const CosTradingRepos::ServiceTypeRepository::PropStruct& p1,
		  const CosTradingRepos::ServiceTypeRepository::PropStruct& p2 )
{
  if ( p1.mode != p2.mode )
    return true;
  
  if ( !p1.value_type->equal( p2.value_type ) )
    return true;
  
  return false;
}

bool operator< ( const CosTradingRepos::ServiceTypeRepository::IncarnationNumber& i1,
		 const CosTradingRepos::ServiceTypeRepository::IncarnationNumber& i2 )
{
  if ( i1.high < i2.high )
    return true;
  if ( i1.high > i2.high )
    return false;
  if ( i1.low < i2.low )
    return true;
  return false;
}

map<string,CosTradingRepos::ServiceTypeRepository::TypeStruct>::iterator
TypeRepository::checkServiceType( const char* name )
{
  /**
   * Does the service type exist ?
   */
  map<string,CosTradingRepos::ServiceTypeRepository::TypeStruct>::iterator it = m_mapTypes.find( static_cast<const char*>(name) );
  if ( it == m_mapTypes.end() )
  {
    CosTrading::UnknownServiceType exc;
    exc.type = CORBA::string_dup( name );
    throw exc;
  }

  return it;
}
