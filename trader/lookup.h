#ifndef __lookup_h__
#define __lookup_h__

#include "misc.h"
#include "trader.h"

class Trader;

class Lookup : virtual public TraderComponents,
	       virtual public SupportAttributes,
	       virtual public ImportAttributes,
	       virtual public CosTrading::Lookup_skel
{
public:
  Lookup( Trader *_trader, CORBA::Object_ptr _obj );
  Lookup( Trader *_trader, const CORBA::BOA::ReferenceData &tag );
  
  virtual void query( const char* type, const char* constr, const char* pref, const CosTrading::PolicySeq& policies,
		      const CosTrading::Lookup::SpecifiedProps& desired_props, CORBA::ULong how_many,
		      CosTrading::OfferSeq*& offers, CosTrading::OfferIterator_ptr& offer_itr,
		      CosTrading::PolicyNameSeq*& limits_applied );
};

#endif
