#include "trader_main.h"
#include "parse_tree.h"
#include "typerepo_impl.h"
#include "init.h"
#include "scan.h"  

#include <sys/stat.h>
#include <stdio.h>
#include <algorithm>
#include <time.h>
#include <unistd.h>

/*****************************************************
 *
 * Used for Preferences
 *
 *****************************************************/

class ProcessedOffer
{
public:
  ProcessedOffer( Offer* _o, PreferencesSortType _type, PreferencesReturn& _ret )
  {
    offer = _o;
    type = _type;
    ret = _ret;
  }
  
  ProcessedOffer( const ProcessedOffer &_o )
  {
    offer = _o.offer;
    type = _o.type;
    ret = _o.ret;
  }

  bool operator< ( ProcessedOffer& _o )
  {
    // Offers that caused an error in the preferences
    // are smaller then all the others => they are the last ones
    // in the list of offers returned to the client.
    if ( ret.type == PreferencesReturn::PRT_ERROR && _o.ret.type == PreferencesReturn::PRT_ERROR )
      return true;
    if ( ret.type == PreferencesReturn::PRT_ERROR )
      return false;
    if ( _o.ret.type == PreferencesReturn::PRT_ERROR )
      return true;
    
    // Assume that all are equal
    if ( type == PST_RANDOM || type == PST_FIRST )
      return true;
    
    if ( type == PST_WITH )
      return !( ret.i < _o.ret.i );
    
    if ( ret.type == PreferencesReturn::PRT_NUM && _o.ret.type == PreferencesReturn::PRT_NUM )
      return !( ret.i < _o.ret.i );
    if ( ret.type == PreferencesReturn::PRT_FLOAT && _o.ret.type == PreferencesReturn::PRT_FLOAT )
      return !( ret.f < _o.ret.f );
    if ( ret.type == PreferencesReturn::PRT_NUM && _o.ret.type == PreferencesReturn::PRT_FLOAT )
      return !( (float)ret.i < _o.ret.i );
    if ( ret.type == PreferencesReturn::PRT_FLOAT && _o.ret.type == PreferencesReturn::PRT_NUM )
      return !( ret.f < (float)_o.ret.i );

    // Never reached
    assert( 0 );
  }
  
  PreferencesReturn ret;
  PreferencesSortType type;
  Offer* offer;
};

/*****************************************************
 *
 * Trader implementation
 *
 *****************************************************/

Trader::Trader( CORBA::Object_ptr _obj )
{
  m_uniqueId = 0;

  ///////////
  // Create a unique id as good as possible
  ///////////
  char buffer[ 100 ];
  sprintf( buffer, "%i.%i", (int)time( 0L ), (int)getpid() );
  int len = strlen( buffer );
  m_requestIdStem.length( len );
  for( int i = 0; i < len; i++ )
    m_requestIdStem[i] = buffer[i];
  
  ///////////
  // Create the trader components
  ///////////
  m_pLookup = new Lookup( this, _obj );
  m_pTypeRepository = new TypeRepository( this );

  initServiceTypes( this );
  koScanParts( this, m_pLookup->_orbnc() );
}

Trader::Trader( const CORBA::BOA::ReferenceData &tag )
{
  m_uniqueId = 0;

  ///////////
  // Create a unique id as good as possible
  ///////////
  char buffer[ 100 ];
  sprintf( buffer, "%i.%i", (int)time( 0L ), (int)getpid() );
  int len = strlen( buffer );
  m_requestIdStem.length( len );
  for( int i = 0; i < len; i++ )
    m_requestIdStem[i] = buffer[i];
  
  m_pLookup = new Lookup( this, tag );
  m_pTypeRepository = new TypeRepository( this );

  initServiceTypes( this );
  koScanParts( this, m_pLookup->_orbnc() );
}
  
CosTrading::Lookup_ptr Trader::lookup_if()
{
  return CosTrading::Lookup::_duplicate( m_pLookup );
}

CosTrading::TypeRepository_ptr Trader::typeRepository()
{
  return CosTrading::TypeRepository::_duplicate( m_pTypeRepository );
}

CosTradingRepos::ServiceTypeRepository_ptr Trader::serviceTypeRepository()
{
  return CosTradingRepos::ServiceTypeRepository::_duplicate( m_pTypeRepository );
}

CORBA::ULong Trader::defSearchCard()
{
  return 100;
}

CORBA::ULong Trader::maxSearchCard()
{
  return 100;
}

CORBA::ULong Trader::defMatchCard()
{
  return 100;
}

CORBA::ULong Trader::maxMatchCard()
{
  return 100;
}

CORBA::ULong Trader::defReturnCard()
{
  return 100;
}

CORBA::ULong Trader::maxReturnCard()
{
  return 100;
}

CORBA::ULong Trader::maxList()
{
  return 100;
}

CORBA::ULong Trader::defHopCount()
{
  return 5;
}

CORBA::ULong Trader::maxHopCount()
{
  return 5;
}

CosTrading::FollowOption Trader::defLinkFollowPolicy()
{
  return CosTrading::if_no_local;
}

CosTrading::FollowOption Trader::maxLinkFollowPolicy()
{
  return CosTrading::always;
}

char* Trader::export( CORBA::Object_ptr reference, const char* type,
		      const CosTrading::PropertySeq& properties )
{
  if ( CORBA::is_nil( reference ) )
  {
    CosTrading::Register::InvalidObjectRef exc;
    exc.ref = reference;
    mico_throw( exc );
  }
       
  Offer *offer = new Offer;
  offer->vReference = CORBA::Object::_duplicate( reference );
  offer->vType = CORBA::string_dup( type );
  offer->properties = properties;
  
  export( offer );

  return CORBA::string_dup( offer->vOfferId.in() );
}

void Trader::export( Offer *_offer )
{
  // Check wether we know this service type
  if ( !m_pTypeRepository->isServiceTypeKnown( _offer->vType.in() ) )
  {    
    CosTrading::UnknownServiceType exc;
    exc.type = CORBA::string_dup( _offer->vType.in() );
    delete _offer;
    mico_throw( exc );
  }
  
  // This function can not raise an exception since we checked the service type already.
  // In addition its exceptions are a subset of ours
  CosTradingRepos::ServiceTypeRepository::TypeStruct_var desc = m_pTypeRepository->fully_describe_type( _offer->vType.in() );

  // Check wether the passed object has the correct interface
  if ( !_offer->vReference->_is_a( desc->if_name.in() ) )
  {
    CosTrading::Register::InterfaceTypeMismatch exc;
    exc.reference = _offer->vReference;
    exc.type = CORBA::string_dup( _offer->vType.in() );
    delete _offer;
    mico_throw( exc );
  }
  
  // Check wether property names are duplicated and
  // wether the types of the property values match the ones of the repository
  list<string> names;
  CORBA::ULong len = _offer->properties.length();
  for( CORBA::ULong l = 0; l < len; l++ )
  {
    // Check for dupes
    list<string>::iterator it = find( names.begin(), names.end(), _offer->properties[l].name.in() );
    if ( it != names.end() )
    {
      CosTrading::DuplicatePropertyName exc;
      exc.name = CORBA::string_dup( _offer->properties[l].name.in() );
      delete _offer;
      mico_throw( exc );
    }
    names.push_back( _offer->properties[l].name.in() );

    // Find the corresponding property entry in the service type
    CORBA::ULong tlen = desc->props.length();
    for( CORBA::ULong t = 0; t < tlen; t++ )
    {
      if ( strcmp( desc->props[t].name.in(), _offer->properties[l].name.in() ) == 0L )
      {  
	CORBA::TypeCode_var tc = _offer->properties[l].value.type();
	if ( !tc->equaltype( &*(desc->props[t].value_type )) )
	{
	  CosTrading::PropertyTypeMismatch exc;
	  exc.prop = _offer->properties[l];
	  exc.type = _offer->vType.in();
	  delete _offer;
	  mico_throw( exc );
	}
      }
    }
    // OMG does not specify an exception for properties which are not mentioned
    // in the type repository. So we just say: Ok 
  }
  
  // Check wether all mandatory properties are present
  len = desc->props.length();
  for( CORBA::ULong l = 0; l < len; l++ )
  {
    if ( desc->props[l].mode == CosTradingRepos::ServiceTypeRepository::PROP_MANDATORY ||
	 desc->props[l].mode == CosTradingRepos::ServiceTypeRepository::PROP_MANDATORY_READONLY )
      if ( find( names.begin(), names.end(), desc->props[l].name.in() ) == names.end() )
      {
	CosTrading::MissingMandatoryProperty exc;
	exc.type = CORBA::string_dup( _offer->vType.in() );
	exc.name = CORBA::string_dup( desc->props[l].name.in() );
	delete _offer;
	mico_throw( exc );
      }
  }

  char buffer[ 100 ];
  sprintf( buffer, "%i", m_uniqueId++ );

  _offer->vOfferId = CORBA::string_dup( buffer );

  m_lstOffers.push_back( _offer );
}

void Trader::import( const char* type, const char* constr, const char* pref, const CosTrading::PolicySeq& policies,
		     const CosTrading::Lookup::SpecifiedProps& desired_props, CORBA::ULong how_many,
		     CosTrading::OfferSeq*& offers, CosTrading::OfferIterator_ptr& offer_itr,
		     CosTrading::PolicyNameSeq*& limits_applied )
{ 
  /**
   * Variables which may be affected by policies.
   */
  CORBA::ULong search_card = defSearchCard();
  CORBA::ULong match_card = defMatchCard();
  CORBA::ULong return_card = defReturnCard();
  CORBA::ULong hop_count = defHopCount();
  CosTrading::FollowOption link_follow_rule = defLinkFollowPolicy();
  bool importer_link_follow_rule = false;
  string starting_trader;
  bool exact_type_match = false;
  CosTrading::Admin::OctetSeq stem;
  stem.length( 0 );
  
  /**
   * Policies
   */
  list<string> policy_names;
  CORBA::ULong max = policies.length();
  for ( CORBA::ULong i = 0; i < max; i++ )
  {
    // Check wether no policy has a dupe
    if ( find( policy_names.begin(), policy_names.end(), policies[i].name.in() ) != policy_names.end() )
    {
      CosTrading::DuplicatePolicyName exc;
      exc.name = CORBA::string_dup( policies[i].name.in() );
      mico_throw( exc );
    }
    policy_names.push_back( policies[i].name.in() );
    
    if ( strcmp( "hop_count", policies[i].name ) == 0 )
    {
      if ( ! ( policies[i].value >>= hop_count ) )
      {
	CosTrading::Lookup::PolicyTypeMismatch exc;
	exc.the_policy = policies[i];
	mico_throw( exc );
      }
      if ( hop_count > maxHopCount() )
	hop_count = maxHopCount();
    }
    else if ( strcmp( "search_card", policies[i].name ) == 0 )
    {
      if ( ! ( policies[i].value >>= search_card ) )
      {
	CosTrading::Lookup::PolicyTypeMismatch exc;
	exc.the_policy = policies[i];
	mico_throw( exc );
      }
      if ( search_card > maxSearchCard() )
	search_card = maxSearchCard();
    }
    else if ( strcmp( "match_card", policies[i].name ) == 0 )
    {
      if ( ! ( policies[i].value >>= match_card ) )
      {
	CosTrading::Lookup::PolicyTypeMismatch exc;
	exc.the_policy = policies[i];
	mico_throw( exc );
      }
      if ( match_card > maxMatchCard() )
	match_card = maxMatchCard();
    }
    else if ( strcmp( "return_card", policies[i].name ) == 0 )
    {
      if ( ! ( policies[i].value >>= return_card ) )
      {
	CosTrading::Lookup::PolicyTypeMismatch exc;
	exc.the_policy = policies[i];
	mico_throw( exc );
      }
      if ( return_card > maxReturnCard() )
	return_card = maxReturnCard();
    }
    else if ( strcmp( "link_follow_rule", policies[i].name ) == 0 )
    {
      if ( ! ( policies[i].value >>= link_follow_rule ) )
      {
	CosTrading::Lookup::PolicyTypeMismatch exc;
	exc.the_policy = policies[i];
	mico_throw( exc );
      }
      importer_link_follow_rule = true;
      // HACK: We rely on the fact the enums are numerated in an ascending manner
      if ( link_follow_rule > maxLinkFollowPolicy() )
	link_follow_rule = maxLinkFollowPolicy();
    }
    else if ( strcmp( "starting_trader", policies[i].name ) == 0 )
    {
      CORBA::String_var str;
      if ( ! ( policies[i].value >>= str ) )
      {
	CosTrading::Lookup::PolicyTypeMismatch exc;
	exc.the_policy = policies[i];
	mico_throw( exc );
      }
      starting_trader = str.in();
    }
    else if ( strcmp( "exact_type_match", policies[i].name ) == 0 )
    {
      CORBA::Boolean b;
      if ( ! ( policies[i].value >>= CORBA::Any::to_boolean( b ) ) )
      {
	CosTrading::Lookup::PolicyTypeMismatch exc;
	exc.the_policy = policies[i];
	mico_throw( exc );
      }
      exact_type_match = (bool)b;
    }
    else if ( strcmp( "RequestId", policies[i].name ) == 0 )
    {
      if ( ! ( policies[i].value >>= stem ) )
      {
	CosTrading::Lookup::PolicyTypeMismatch exc;
	exc.the_policy = policies[i];
	mico_throw( exc );
      }
    }
  }

  ////////////
  // Check wether we already processed this query.
  // We use the stem to find out
  ////////////
  bool cancel = true;
  CORBA::ULong stemlen = stem.length();
  if ( stemlen == m_requestIdStem.length() && stemlen != 0 )
  {
    for( CORBA::ULong i = 0; i < stemlen; i++ )
      if ( stem[i] != m_requestIdStem[i] )
      {
	cancel = false;
	break;
      }
  }
  else
    cancel = false;
  
  if ( cancel )
  {
    cerr << "Canceling request to avoid loops" << endl;
    limits_applied = new CosTrading::PolicyNameSeq;
    limits_applied->length( 0 );
    offer_itr = 0L;
    offers = new CosTrading::OfferSeq;
    offers->length( 0 );
    return;
  }
  
  /**
   * Service Type Names
   */
  // Do we know this service type ?
  if ( !m_pTypeRepository->isServiceTypeKnown( type ) )
  {
    CosTrading::UnknownServiceType exc;
    exc.type = CORBA::string_dup( type );
    mico_throw( exc );
  }
  
  // Find all offers matching the service type but not more then search_card
  list<Offer*> search;
  list<Offer*>::iterator it( m_lstOffers.begin() );
  for( ; it != m_lstOffers.end(); ++it )
  {
    if ( search.size() == search_card )
      break;
    cout << "Comparing " << (const char*)(*it)->vType << " and " << (const char*)type << endl;
    if ( ( !exact_type_match && m_pTypeRepository->isSubTypeOf( (*it)->vType.in(), type ) ) ||
	 ( exact_type_match && strcmp( type, (*it)->vType.in() ) == 0L ) )
    {
      search.push_back( (*it) );
    }
  }

  /**
   * Constraints
   */
  ParseTreeBase *constraint_tree = 0L;
  if ( strlen( constr ) > 0 )
  {
    constraint_tree = parseConstraints( constr );
    if ( constraint_tree == 0L )
    {
      CosTrading::IllegalConstraint exc;
      exc.constr = CORBA::string_dup( constr );
      mico_throw( exc );
    }
  }
  
  list<Offer*> match;
  list<Offer*>::iterator it2( search.begin() );
  for( ; it2 != search.end(); ++it2 )
  {
    if ( match.size() == match_card )
      break;
    if ( constraint_tree == 0L || ( matchConstraint( constraint_tree, &((*it2)->properties) ) == 1 ) )
    {
      cout << "Got match" << endl;;
      match.push_back( *it2 );
    }
    else
      cout << "Does not match constraint" << endl;
  }
       
  cerr << "Prefernces ...." << endl;
  
  /**
   * Preferences
   */
  ParseTreeBase *pref_tree = 0L;
  PreferencesSortType ptype = PST_ERROR;
  if ( strlen( pref ) > 0 )
  {
    pref_tree = parsePreferences( pref, ptype );
    if ( pref_tree == 0L  )
    {
      CosTrading::Lookup::IllegalPreference exc;
      exc.pref = CORBA::string_dup( pref );
      mico_throw( exc );
    }
  }
  
  list<ProcessedOffer> ret;
  list<Offer*>::iterator it3( match.begin() );
  for( ; it3 != match.end(); ++it3 )
  {
    // By default this struct is in error mode
    PreferencesReturn preturn;
    if ( pref_tree != 0L )
    {
      int i = matchPreferences( pref_tree, &((*it3)->properties), preturn );
      if ( preturn.type == PreferencesReturn::PRT_ERROR )
	cout << "Error in pref evaluation " << i << endl;
      else
	cout << "Got preference of " << preturn.i << endl;
    }
     
    if ( ret.size() == return_card )
      break;

    ret.push_back( ProcessedOffer( (*it3), ptype, preturn ) );
  }

  cerr << "Sorting" << endl;
  
  /**
   * Sort by preferences.
   */
  ret.sort();
  
  cerr << "match_card" << endl;
  
  /**
   * Limit number of returns and put the resulting offers in the out parameter "offers"
   * and in the OfferIterator.
   */
  max = return_card;
  if ( ret.size() < max )
    max = ret.size();
  
  offers = new CosTrading::OfferSeq;
  
  // Do we need an iterator ?
  CosTrading::OfferSeq* iterator_offers = 0L;
  if ( max > how_many )
  {
    offers->length( how_many );
    iterator_offers = new CosTrading::OfferSeq;
    iterator_offers->length( max - how_many );
  }
  else
    offers->length( max );

  cerr << "Moving offers" << endl;
  
  CORBA::ULong i = 0;
  list<ProcessedOffer>::iterator it4( ret.begin() );
  CosTrading::OfferSeq* dest = offers;
  for( ; it4 != ret.end(); ++it4 )
  {
    cerr << "i=" << i << endl;
    
    // Decide which offes have to go in the iterator
    if ( i == how_many && dest != iterator_offers )
    {
      cerr << "Putting in iterator now" << endl;
      i = 0;
      dest = iterator_offers;
      assert( dest != 0L );
    }
    cerr << "The reference thing ..." << endl;
    
    CosTrading::Offer o;
    o.reference = it4->offer->vReference;
      
    cerr << "Assigned reference" << endl;
    
    /**
     * Which properties should we deliver ?
     */
    if ( desired_props._d() == CosTrading::Lookup::all )
      o.properties = it4->offer->properties;
    else if ( desired_props._d() == CosTrading::Lookup::some )
    {
      CORBA::ULong size = 0;
      CORBA::ULong dlen = desired_props.prop_names().length();
      CORBA::ULong len = it4->offer->properties.length();
      o.properties.length( size );
      for( CORBA::ULong l = 0; l < len; l++ )
      {
	for( CORBA::ULong d = 0; d < dlen; d++ )
	  if ( strcmp( it4->offer->properties[l].name.in(), desired_props.prop_names()[d].in() ) == 0 )
	  {
	    o.properties.length( size + 1 );
	    o.properties[ size ] = it4->offer->properties[l];
	    size++;
	  }
      }
    }
    else if ( desired_props._d() == CosTrading::Lookup::none )
    {
      o.properties.length( 0 );
    }
    cerr << "Assiged props" << endl;
    (*dest)[i++] = o;
    cerr << "Copied" << endl;
  }

  cerr << "limits_applied" << endl;

  /**
   * Policies again
   */
  // HACK
  limits_applied = new CosTrading::PolicyNameSeq;
  limits_applied->length( 0 );

  cerr << "offer iterator" << endl;  

  /**
   * Offer Iterator
   */
  if ( iterator_offers == 0L )
    offer_itr = 0L;
  else
    offer_itr = CosTrading::OfferIterator::_duplicate( new OfferIterator( iterator_offers ) );

  cerr << "clean" << endl;
  
  /**
   * Clean up
   */
  if ( constraint_tree )
    delete constraint_tree;
  if ( pref_tree )
    delete pref_tree;

  cerr << "Xtension" << endl;
  
#ifndef OMG_KONFORM
  /**
   * EXTENSION:
   * Replace the values of all properties with the flag 'is_file' with 
   * the content of the file they reference.
   */
  for( unsigned int x = 0; x < offers->length(); x++ )
  {
    int len = (*offers)[x].properties.length();
    for( int y = 0; y < len; y++ )
    {
      if ( (*offers)[x].properties[y].is_file )
      {
	char *s;
	if ( !( (*offers)[x].properties[y].value >>= s ) )
	{
	  cerr << "Value " << (*offers)[x].properties[y].name << " is not of type string as expected" << endl;
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
	(*offers)[x].properties[y].value <<= CORBA::Any::from_string( (char *) p, 0 );
	delete []p;
      }
    }
  }
#endif

  cerr << "done" << endl;
}


/***********************************************************
 *
 * OfferIterator
 *
 ***********************************************************/

OfferIterator::OfferIterator( CosTrading::OfferSeq* _offers )
{
  m_pOffers = _offers;
  m_pos = 0;
}

OfferIterator::~OfferIterator()
{
  delete m_pOffers;
}

CORBA::ULong OfferIterator::max_left()
{
  return ( m_pOffers->length() - m_pos );
}

CORBA::Boolean OfferIterator::next_n( CORBA::ULong n, CosTrading::OfferSeq*& offers )
{
  CORBA::ULong len = n;
  if ( len > m_pOffers->length() - m_pos )
    len = m_pOffers->length() - m_pos;
  
  offers = new CosTrading::OfferSeq;
  offers->length( len );
  
  for( CORBA::ULong l = 0; l < len; l++ )
    (*offers)[l] = (*m_pOffers)[m_pos++];
  
  if ( m_pos < m_pOffers->length() )
    return true;
  return false;
}

void OfferIterator::destroy()
{
  CORBA::release( this );
}

