#include "lookup.h"
#include "trader_main.h"

Lookup::Lookup( Trader *_trader, CORBA::Object_ptr _obj ) :  
  ::TraderComponents( _trader ), ::SupportAttributes( _trader ),
  ::ImportAttributes( _trader ), CosTrading::Lookup_skel( _obj )
{
}

Lookup::Lookup( Trader *_trader, const CORBA::BOA::ReferenceData &tag ) :
  ::TraderComponents( _trader ), ::SupportAttributes( _trader ),
  ::ImportAttributes( _trader ), CosTrading::Lookup_skel( tag )
{
}

void Lookup::query( const char* type, const char* constr, const char* pref, const CosTrading::PolicySeq& policies,
		    const CosTrading::Lookup::SpecifiedProps& desired_props, CORBA::ULong how_many,
		    CosTrading::OfferSeq*& offers, CosTrading::OfferIterator_ptr& offer_itr,
		    CosTrading::PolicyNameSeq*& limits_applied )
{
  m_pTrader->import( type, constr, pref, policies, desired_props, how_many, offers, offer_itr, limits_applied );
}
