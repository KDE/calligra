#ifndef __ko_query_trader_h__
#define __ko_query_trader_h__

#include "trader.h"

#include "koQueryTypes.h"

void koQueryTrader( const char *_service_type, const char *_constr, unsigned int _count,
		    CosTrading::OfferSeq*& offers, CosTrading::OfferIterator_ptr& offer_itr,
		    CosTrading::PolicyNameSeq*& limits_applied );

#endif
