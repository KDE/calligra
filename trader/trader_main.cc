#include "trader_main.h"
#include "parse_tree.h"
#include "typerepo_impl.h"
#include "link.h"
#include "proxy.h"
#include "scan.h"
#include "init.h"

#include "kstrstream.h"
#include "kany.h"

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
  m_pRegister = new Register( this );
  m_pTypeRepository = new TypeRepository( this );
  m_pLink = new Link( this );
  m_pProxy = new Proxy( this );

  initServiceTypes( this );
  koScanParts( this, m_pLookup->_orbnc() );
  koScanFilters( this, m_pLookup->_orbnc() );
  koScanOfficeParts( this, m_pLookup->_orbnc() );
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
  m_pRegister = new Register( this );
  m_pTypeRepository = new TypeRepository( this );
  m_pLink = new Link( this );
  m_pProxy = new Proxy( this );

  initServiceTypes( this );
  koScanParts( this, m_pLookup->_orbnc() );
  koScanFilters( this, m_pLookup->_orbnc() );
  koScanOfficeParts( this, m_pLookup->_orbnc() );
}
  
CosTrading::Lookup_ptr Trader::lookup_if()
{
  return CosTrading::Lookup::_duplicate( m_pLookup );
}

CosTrading::Register_ptr Trader::register_if()
{
  return CosTrading::Register::_duplicate( m_pRegister );
}

CosTrading::Link_ptr Trader::link_if()
{
  return CosTrading::Link::_duplicate( m_pLink );
}

CosTrading::Proxy_ptr Trader::proxy_if()
{
  return CosTrading::Proxy::_duplicate( m_pProxy );
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
    list<string>::iterator it = find( names.begin(), names.end(),
				      _offer->properties[l].name.in() );
    if ( it != names.end() )
    {
      cerr << "Duplicate EXC" << endl;
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

void Trader::remove( const char* id )
{
  list<Offer*>::iterator it( m_lstOffers.begin() );
  for( ; it != m_lstOffers.end(); ++it )
  {
    if ( strcmp( (*it)->vOfferId.in(), id ) == 0 )
    {
      Offer* p = *it;
      m_lstOffers.erase( it );
      delete p;
      return;
    }
  }

  CosTrading::UnknownOfferId exc;
  exc.id = id;
  mico_throw( exc );
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

  /**
   * Linked Traders
   */
  // This variable is used to hold all linked offers and delete them
  // once we return from this function
  list<Offer> linked_offers;
  if ( hop_count > 0 && ( link_follow_rule == CosTrading::always ||
			  ( link_follow_rule == CosTrading::if_no_local && match.size() != 0 ) ) )
  {
    // Iterate over every link
    map<string,CosTrading::Link::LinkInfo>::iterator it = m_pLink->begin();
    for( ; it != m_pLink->end(); ++it )
    {
      /////////
      // Find the correct link_follow_rule
      /////////
      CosTrading::FollowOption follow = it->second.def_pass_on_follow_rule;
      // Did the user specify a whish for the link_follow_rule ?
      if ( importer_link_follow_rule )
      {  
	// Do we have to limit the users wish or not ?
	if ( link_follow_rule <= it->second.limiting_follow_rule )
	  follow = link_follow_rule;
	else
	  follow = it->second.limiting_follow_rule;
      }

      ////////
      // Add/change the link_follow_rule policy
      ////////
      CosTrading::PolicySeq pols = policies;
      CORBA::ULong len = pols.length();
      bool found = false;
      for ( CORBA::ULong i = 0; i < len; i++ )
      {
	if ( strcmp( "link_follow_rule", policies[i].name ) == 0 )
	{
	  found = true;
	  pols[i].value <<= follow;
	  break;
	}
      }
      if ( !found )
      {
	CosTrading::Policy p;
	p.name = CORBA::string_dup( "link_follow_rule" );
	p.value <<= follow;
	pols.length( len + 1 );
	pols[ len ] = p;
      }
      
      /////////
      // We want to see ALL properties. Otherwise I can later on not evaluate
      // the preferences
      /////////
      CosTrading::Lookup::SpecifiedProps desired;
      desired._d( CosTrading::Lookup::all );
      
      /////////
      // Ask for as much offers as we are allowed to return at all.
      // Since we ask every link for this amount of offers we might get too many
      // of them here. But it is unpredictable how many offers the other links will deliver.
      /////////
      CORBA::ULong amount = return_card - match.size();

      /////////
      // Add/change the return_card policy to match our needs. It may happen that the default return
      // card of linked trader is too low, so we may have to ask for more using the return_card policy.
      /////////
      len = pols.length();
      found = false;
      for ( CORBA::ULong i = 0; i < len; i++ )
      {
	if ( strcmp( "return_card", policies[i].name ) == 0 )
	{
	  found = true;
	  pols[i].value <<= amount;
	  break;
	}
      }
      if ( !found )
      {
	CosTrading::Policy p;
	p.name = CORBA::string_dup( "return_card" );
	p.value <<= amount;
	pols.length( len + 1 );
	pols[ len ] = p;
      }

      /////////
      // Add/change the hop counts
      /////////
      len = pols.length();
      found = false;
      for ( CORBA::ULong i = 0; i < len; i++ )
      {
	if ( strcmp( "hop_count", policies[i].name ) == 0 )
	{
	  found = true;
	  pols[i].value <<= hop_count - 1;
	  break;
	}
      }
      if ( !found )
      {
	CosTrading::Policy p;
	p.name = CORBA::string_dup( "hop_count" );
	p.value <<= hop_count - 1;
	pols.length( len + 1 );
	pols[ len ] = p;
      }

      /////////
      // If there is no RequestId policy yet, then we create one,
      // otherwise we wont touch an existing one
      /////////
      found = false;
      len = pols.length();
      for ( CORBA::ULong i = 0; i < len; i++ )
      {
	if ( strcmp( "RequestId", policies[i].name ) == 0 )
	  break;
      }
      if ( !found )
      {
	CosTrading::Policy p;
	p.name = CORBA::string_dup( "RequestId" );
	p.value <<= m_requestIdStem;
	pols.length( len + 1 );
	pols[ len ] = p;
      }

      /////////
      // Call the linked trader
      /////////
      CosTrading::OfferSeq_var result;
      CosTrading::OfferIterator_var itr;
      CosTrading::PolicyNameSeq_var limits;
      it->second.target->query( type, constr, pref, pols, desired, amount, result, itr, limits );
      
      // Release the iterator if there is any. We dont need it.
      if ( !CORBA::is_nil( itr ) )
      {  
	itr->destroy();
	itr = 0L;
      }      

      /////////
      // Merge the result of the linked trader with the offers of this trader
      /////////
      len = result->length();
      for( CORBA::ULong l = 0; l < len; l++ )
      {
	Offer o;
	o.vReference = result[l].reference;
	o.properties = result[l].properties;
	o.vType = CORBA::string_dup( type );
	// We fake an offer id here. It is not used here so it does not matter
	o.vOfferId = CORBA::string_dup( "" );
	// We move the offer in this list so it becomes delete once the function returns or exits
	linked_offers.push_back( o );
	match.push_back( &(linked_offers.back()) );
      }      
    }  
  }
       
  /**
   * Preferences
   */
  // Parse preferences
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
  
  // Find all properties which are numerical
  map<string,PreferencesMaxima> maxima;
  CosTradingRepos::ServiceTypeRepository::TypeStruct_var ts;
  ts = m_pTypeRepository->fully_describe_type( type );
  for( CORBA::ULong l = 0; l < ts->props.length(); ++l )
    if ( ts->props[l].value_type->equaltype( CORBA::_tc_long ) )
    {
      PreferencesMaxima m;
      m.type = PreferencesMaxima::PM_INVALID_INT;
      maxima[ ts->props[l].name.in() ] = m;
    }
    else if ( ts->props[l].value_type->equaltype( CORBA::_tc_float ) )
    {
      PreferencesMaxima m;
      m.type = PreferencesMaxima::PM_INVALID_FLOAT;
      maxima[ ts->props[l].name.in() ] = m;
    }

  // Find maximum and minimum of numerical values
  list<Offer*>::iterator it3( match.begin() );
  // Iterate over all offers
  for( ; it3 != match.end(); ++it3 )
  {
    // Iterate over all properties
    for( CORBA::ULong l = 0; l < (*it3)->properties.length(); ++l )
    {
      map<string,PreferencesMaxima>::iterator it;
      it = maxima.find( (*it3)->properties[l].name.in() );
      // Is the property numerical ?
      if ( it != maxima.end() )
      {
	// Determine new maximum/minimum
	if ( it->second.type == PreferencesMaxima::PM_INVALID_INT )
	{
	  CORBA::Long v;
	  if ( (*it3)->properties[l].value >>= v )
	  {
	    it->second.type = PreferencesMaxima::PM_INT;
	    it->second.iMin = v;
	    it->second.iMax = v;
	  }
	}
	else if ( it->second.type == PreferencesMaxima::PM_INT )
	{
	  CORBA::Long v;
	  if ( (*it3)->properties[l].value >>= v )
	  {
	    if ( v < it->second.iMin )
	      it->second.iMin = v;
	    if ( v > it->second.iMax )
	      it->second.iMax = v;
	  }
	}
	else if ( it->second.type == PreferencesMaxima::PM_INVALID_FLOAT )
	{
	  CORBA::Float v;
	  if ( (*it3)->properties[l].value >>= v )
	  {
	    it->second.type = PreferencesMaxima::PM_FLOAT;
	    it->second.fMin = v;
	    it->second.fMax = v;
	  }
	}
	else if ( it->second.type == PreferencesMaxima::PM_FLOAT )
	{
	  CORBA::Float v;
	  if ( (*it3)->properties[l].value >>= v )
	  {
	    if ( v < it->second.fMin )
	      it->second.fMin = v;
	    if ( v > it->second.fMax )
	      it->second.fMax = v;
	  }
	}
      }
    }
  }
  
  // Apply the preferences tree
  list<ProcessedOffer> ret;
  it3 = match.begin();
  for( ; it3 != match.end(); ++it3 )
  {
    // By default this struct is in error mode
    PreferencesReturn preturn;
    if ( pref_tree != 0L )
    {
      int i = matchPreferences( pref_tree, &((*it3)->properties), preturn, maxima );
      /** DEBUG **/
      if ( preturn.type == PreferencesReturn::PRT_ERROR )
	cout << "Error in pref evaluation " << i << endl;
      else if ( preturn.type == PreferencesReturn::PRT_NUM )
	cout << "Got preference of " << preturn.i << endl;
      else if ( preturn.type == PreferencesReturn::PRT_FLOAT )
	cout << "Got preference of " << preturn.f << endl;
      /** DEBUG **/
    }
     
    if ( ret.size() == return_card )
      break;

    ret.push_back( ProcessedOffer( (*it3), ptype, preturn ) );
  }

  /**
   * Sort by preferences.
   */
  ret.sort();
  
  /**
   * Limit number of returns and put the resulting offers in the out parameter "offers"
   * and in the OfferIterator.
   */
  max = return_card;
  if ( ret.size() < max )
    max = ret.size();
  
  offers = new CosTrading::OfferSeq;
  
  CosTrading::OfferSeq* iterator_offers = 0L;
  if ( max > how_many )
  {
    offers->length( how_many );
    iterator_offers = new CosTrading::OfferSeq;
    iterator_offers->length( max - how_many );
  }
  else
    offers->length( max );

  CORBA::ULong i = 0;
  list<ProcessedOffer>::iterator it4( ret.begin() );
  CosTrading::OfferSeq* dest = offers;
  for( ; it4 != ret.end(); ++it4 )
  {
    // Decide which offes have to go in the iterator
    if ( i == how_many && dest == offers )
    {
      i = 0;
      dest = iterator_offers;
    }

    /**
     * Proxy handling
     **/
    CosTrading::Offer o;
    // Do we have to receive the object reference from a proxy ?
    if ( !it4->offer->isProxy )
      o.reference = it4->offer->vReference;
    else
    {
      CosTrading::PolicySeq pols;
      CosTrading::OfferSeq_var seq;
      CosTrading::OfferIterator_var itr;
      CosTrading::PolicyNameSeq_var limits;
      
      // Append the proxies policies
      pols = policies;
      CORBA::ULong pl = pols.length();
      pols.length( pl + it4->offer->vProxy->policies_to_pass_on.length() );
      for( CORBA::ULong l = 0; l < it4->offer->vProxy->policies_to_pass_on.length(); l++ )
	pols[ pl + l ] = it4->offer->vProxy->policies_to_pass_on[l];
      
      // Use recipe to construct the constraints
      string constraint = "";
      string recipe = it4->offer->vProxy->recipe.in();
      if ( recipe.empty() )
	constraint = CORBA::string_dup( constr );
      else
      {
	char buf[2] = { 0, 0 };
	// Iterate over recipe
	for ( unsigned int p = 0; p < recipe.size(); ++p )
	{
	  if ( recipe[p] != '$' )
	  {
	    buf[0] = recipe[p];
	    constraint += buf;
	  }
	  else
	  {
	    p++;
	    // No character after "$"
	    if ( p == recipe.size() )
	    {
	      CosTrading::Proxy::IllegalRecipe exc;
	      exc.recipe = CORBA::string_dup( recipe.c_str() );
	      mico_throw( exc );
	    }
	    char n = recipe[ p++ ];
	    // Add complete primary constraint
	    if ( n == '*' )
	      constraint += constr;
	    // "$" was used as escape character
	    else if ( n != '(' )
	    {
	      buf[0] = n;
	      constraint += buf;
	    }
	    // Insert value of some property
	    else
	    {
	      // Find the name in brackets
	      int start = p;
	      while ( p < recipe.size() && recipe[p] != ')' ) p++;
	      if ( p == recipe.size() )
	      {
		CosTrading::Proxy::IllegalRecipe exc;
		exc.recipe = CORBA::string_dup( recipe.c_str() );
		mico_throw( exc );
	      }
	      p++;
	      string name;
	      name.assign( recipe, start, p - start );
	      // Find property with that name
	      CORBA::ULong l = 0;
	      while( l < it4->offer->properties.length() &&
		     name != it4->offer->properties[l].name.in() )
		l++;
	      if ( l == it4->offer->properties.length() )
	      {
		CosTrading::Proxy::IllegalRecipe exc;
		exc.recipe = CORBA::string_dup( recipe.c_str() );
		mico_throw( exc );
	      }
	      // Append CORBA Any to string
	      KOStrStream str( constraint );
	      str << it4->offer->properties[l].value;
	    }
	  }
	}
      }
      
      it4->offer->vProxy->target->query( type, constraint.c_str(), pref, policies, desired_props, 1, seq, itr, limits );
      // TODO: catch all exceptions here

      // Release iterator at once
      if ( !CORBA::is_nil( itr ) )
      {
	itr->destroy();
	CORBA::release( itr );
      }

      // Check wether we got a result. Otherwise skip this offer completely
      if ( seq->length() > 1 )
      {
	cerr << "Oooops, the proxy did give us more than one return." << endl;
	dest->length( dest->length() - 1 );
	continue;
      }
      else if ( seq->length() == 0 )
      {
	cerr << "Oooops, the proxy did not give us a return" << endl;
	dest->length( dest->length() - 1 );
	continue;
      }
      
      o.reference = seq[0].reference;
    }
      
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
    (*dest)[i++] = o;
  }

  /**
   * Policies again
   */
  // HACK
  limits_applied = new CosTrading::PolicyNameSeq;
  limits_applied->length( 0 );
  
  /**
   * Offer Iterator
   */
  if ( iterator_offers == 0L )
    offer_itr = 0L;
  else
    offer_itr = CosTrading::OfferIterator::_duplicate( new OfferIterator( iterator_offers ) );
  
  /**
   * Clean up
   */
  if ( constraint_tree )
    delete constraint_tree;
  if ( pref_tree )
    delete pref_tree;

#ifndef OMG_KONFORM
  /**
   * EXTENSION:
   * Replace the values of all properties with the flag 'is_file' with 
   * the content of the file they reference.
   */
  for( int x = 0; x < offers->length(); x++ )
  {
    int len = (*offers)[x].properties.length();
    for( int y = 0; y < len; y++ )
    {
      if ( (*offers)[x].properties[y].is_file )
      {
	char *s;
	if ( !( (*offers)[x].properties[y].value >>= s ) )
	{
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
}

CosTrading::Register::OfferInfo* Trader::describe( const char* id )
{
  list<Offer*>::iterator it = m_lstOffers.begin();
  for( ; it != m_lstOffers.end(); ++it )
  {
    if ( strcmp( (*it)->vOfferId.in(), id ) == 0L )
    {
      CosTrading::Register::OfferInfo* info = new CosTrading::Register::OfferInfo;
      info->reference = (*it)->vReference;
      info->type = CORBA::string_dup( (*it)->vType.in() );
      info->properties = (*it)->properties;
      return info;
    }
  }
  
  CosTrading::UnknownOfferId exc;
  exc.id = CORBA::string_dup( id );
  mico_throw( exc );

  // never reached, but we want the compiler to shutup
  return 0;
}

void Trader::modify( const char* id, const CosTrading::PropertyNameSeq& del_list,
		     const CosTrading::PropertySeq& modify_list )
{
  list<Offer*>::iterator it = m_lstOffers.begin();
  for( ; it != m_lstOffers.end(); ++it )
  {
    if ( strcmp( (*it)->vOfferId.in(), id ) == 0L )
    {
      CosTradingRepos::ServiceTypeRepository::TypeStruct_var desc = m_pTypeRepository->fully_describe_type( (*it)->vType.in() );

      CORBA::ULong plen = (*it)->properties.length();
      CORBA::ULong ptlen = desc->props.length();

      /**
       * Check del_list properties.
       */
      list<string> del;
      CORBA::ULong len = del_list.length();
      for( CORBA::ULong l = 0; l < len; l++ )
      {
	// Check for dupes in del_list ?
	if( find( del.begin(), del.end(), del_list[l].in() ) != del.end() )
	{
	  CosTrading::DuplicatePropertyName exc;
	  exc.name = del_list[l].in();
	  mico_throw( exc );
	}
	del.push_back( del_list[l].in() );

	// Check wether the property is mandatory.
	// In this case we may not remove it.
	for( CORBA::ULong t = 0; t < ptlen; t++ )
	{
	  if( strcmp( del_list[l].in(), desc->props[t].name.in() ) == 0L )
	  {
	    if( desc->props[t].mode == CosTradingRepos::ServiceTypeRepository::PROP_MANDATORY_READONLY ||
		desc->props[t].mode == CosTradingRepos::ServiceTypeRepository::PROP_MANDATORY )
	    {
	      CosTrading::Register::MandatoryProperty exc;
	      exc.type = CORBA::string_dup( (*it)->vType.in() );
	      exc.name = CORBA::string_dup( del_list[l].in() );
	      mico_throw( exc );
	    }
	  }
	}

	// Does the property exist ?
	bool found = false;
	for( CORBA::ULong p = 0; p < plen; p++ )
	{
	  if( strcmp( del_list[l].in(), (*it)->properties[p].name.in() ) == 0L )
	  {
	    found = true;
	    break;
	  }
	}
	if ( !found )
	{
	  CosTrading::Register::UnknownPropertyName exc;
	  exc.name = CORBA::string_dup( del_list[l].in() );
	  mico_throw( exc );
	}
      }
      
      /**
       * Which modified properties do right now not exist and have to be added
       * and which ones have to be modified
       */
      map<string,CosTrading::Property> add;
      map<string,CosTrading::Property> modify;
      len = modify_list.length();
      for( CORBA::ULong l = 0; l < len; l++ )
      {
	// Check for dupes in modify_list
	if ( add.find( modify_list[l].name.in() ) != add.end() ||
	     modify.find( modify_list[l].name.in() ) != modify.end() )
	{
	  CosTrading::DuplicatePropertyName exc;
	  exc.name = del_list[l].in();
	  mico_throw( exc );
	}
	
	// Does the offer already contain this property
	bool found = false;
	for( CORBA::ULong p = 0; p < plen; p++ )
	{
	  if( strcmp( modify_list[l].name.in(), (*it)->properties[p].name.in() ) == 0L )
	  {
	    found = true;
	    break;
	  }
	}
	if ( !found )
	  add[ modify_list[l].name.in() ] = modify_list[l];
	else
	{
	  // Check wether the property is readonly
	  // In this case we may not modify it
	  for( CORBA::ULong t = 0; t < ptlen; t++ )
	  {
	    if( strcmp( modify_list[l].name.in(), desc->props[t].name.in() ) == 0L )
	    {
	      if( desc->props[t].mode == CosTradingRepos::ServiceTypeRepository::PROP_MANDATORY_READONLY ||
		  desc->props[t].mode == CosTradingRepos::ServiceTypeRepository::PROP_READONLY )
	      {
		CosTrading::Register::ReadonlyProperty exc;
		exc.type = CORBA::string_dup( (*it)->vType.in() );
		exc.name = CORBA::string_dup( modify_list[l].name.in() );
		mico_throw( exc );
	      }
	    }
	  }
	  modify[ modify_list[l].name.in() ] = modify_list[l];
	}

      }
      
      /**
       * Copy the properties, exclude deleted one and change modified ones.
       */
      CosTrading::PropertySeq props;
      props.length( (*it)->properties.length() - del_list.length() + add.size() );
      CORBA::ULong idx = 0;
      for( CORBA::ULong p = 0; p < plen; p++ )
      {
	list<string>::iterator sit = find( del.begin(), del.end(), (*it)->properties[p].name.in() );
	if ( sit != del.end() )
	  continue;
	map<string,CosTrading::Property>::iterator mit = modify.find( (*it)->properties[p].name.in() );
	if ( mit != modify.end() )
	  props[idx++] = mit->second;
	else
	  props[idx++] = (*it)->properties[p];
      }
      
      // Add new properties
      map<string,CosTrading::Property>::iterator mit = add.begin();
      for( ; mit != add.end(); ++mit )
	props[idx++] = mit->second;

      (*it)->properties = props;
      
      return;
    }
  }
  
  CosTrading::UnknownOfferId exc;
  exc.id = CORBA::string_dup( id );
  mico_throw( exc );
}

void Trader::withdraw_using_constraint( const char* type, const char* constr )
{
  // Check wether we know this service type
  if ( !m_pTypeRepository->isServiceTypeKnown( type ) )
  {    
    CosTrading::UnknownServiceType exc;
    exc.type = CORBA::string_dup( type );
    mico_throw( exc );
  }

  /**
   * Constraints
   */
  ParseTreeBase *constraint_tree = 0L;
  if ( strlen( constr ) > 0 )
    constraint_tree = parseConstraints( constr );
  if ( constraint_tree == 0L )
  {
    CosTrading::IllegalConstraint exc;
    exc.constr = CORBA::string_dup( constr );
    mico_throw( exc );
  }

  // Find matching offers
  list<Offer*> del;
  list<Offer*>::iterator it( m_lstOffers.begin() );
  for( ; it != m_lstOffers.end(); ++it )
  {
    if ( strcmp( type, (*it)->vType.in() ) == 0L )
    {	
      if ( constraint_tree == 0L || ( matchConstraint( constraint_tree, &((*it)->properties) ) == 1 ) )
	del.push_back( *it );
    }
  }
  
  delete constraint_tree;

  // Did we match no offers => raise an exception
  if ( del.size() == 0 )
  {
    CosTrading::Register::NoMatchingOffers exc;
    exc.constr = CORBA::string_dup( constr );
    mico_throw( exc );
  }
  
  // Remove matching offers
  it = del.begin();
  for( ; it != del.end(); ++it )
  {
    list<Offer*>::iterator it2 = find( m_lstOffers.begin(), m_lstOffers.end(), *it );
    if ( it2 != m_lstOffers.end() )
    {
      m_lstOffers.erase( it2 );
      delete *it2;
    }
  }
}

/***********************************************************
 *
 * Proxy
 *
 ***********************************************************/

char* Trader::export_proxy( CosTrading::Lookup_ptr target, const char* type,
			    const CosTrading::PropertySeq& properties, CORBA::Boolean if_match_all,
			    const char* recipe, const CosTrading::PolicySeq& policies_to_pass_on )
{
  CosTrading::Proxy::ProxyInfo* info = new CosTrading::Proxy::ProxyInfo;
  info->target = CosTrading::Lookup::_duplicate( target );
  info->type = CORBA::string_dup( type );
  info->properties = properties;
  info->if_match_all = if_match_all;
  info->recipe = CORBA::string_dup( recipe );
  info->policies_to_pass_on = policies_to_pass_on;
  
  Offer* o = new Offer;
  o->vProxy = info;
  o->vType = CORBA::string_dup( type );
  o->properties = properties;
  o->isProxy = true;
  
  char buffer[ 100 ];
  sprintf( buffer, "%i", m_uniqueId++ );
  o->vOfferId = CORBA::string_dup( buffer );

  m_lstOffers.push_back( o );
  
  return CORBA::string_dup( buffer );
}

void Trader::withdraw_proxy( const char* id )
{
  list<Offer*>::iterator it( m_lstOffers.begin() );
  for( ; it != m_lstOffers.end(); ++it )
  {
    if ( strcmp( (*it)->vOfferId.in(), id ) == 0 )
    {
      Offer* p = *it;
      if ( !p->isProxy )
      {
	CosTrading::Proxy::NotProxyOfferId exc;
	exc.id = CORBA::string_dup( id );
	mico_throw( exc );
      }
      
      m_lstOffers.erase( it );
      delete p;
      return;
    }
  }

  CosTrading::UnknownOfferId exc;
  exc.id = id;
  mico_throw( exc );
}

CosTrading::Proxy::ProxyInfo* Trader::describe_proxy( const char* id )
{
  list<Offer*>::iterator it( m_lstOffers.begin() );
  for( ; it != m_lstOffers.end(); ++it )
  {
    if ( strcmp( (*it)->vOfferId.in(), id ) == 0 )
    {
      Offer* p = *it;
      if ( !p->isProxy )
      {
	CosTrading::Proxy::NotProxyOfferId exc;
	exc.id = CORBA::string_dup( id );
	mico_throw( exc );
      }
      CosTrading::Proxy::ProxyInfo* info = new CosTrading::Proxy::ProxyInfo;
      (*info) = p->vProxy;
      return info;
    }
  }

  CosTrading::UnknownOfferId exc;
  exc.id = id;
  mico_throw( exc );  

  // Never reached, but we want the compiler to shutup
  return 0;
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

