#ifndef __trader_main_h__
#define __trader_main_h__

#include "trader.h"
#include "lookup.h"
#include "typerepo.h"

#include <list>

class TypeRepository;

struct Offer
{
  CORBA::Object_var vReference;
  CosTrading::ServiceTypeName_var vType;
  CosTrading::PropertySeq properties;
  CosTrading::OfferId_var vOfferId;
};

class Trader
{
public:
  Trader( CORBA::Object_ptr _obj );
  Trader( const CORBA::BOA::ReferenceData &tag );
  
  CosTrading::Lookup_ptr lookup_if();
  CosTrading::TypeRepository_ptr typeRepository();
  CosTradingRepos::ServiceTypeRepository_ptr serviceTypeRepository();
  
  // Hack
  TypeRepository* hack() { return m_pTypeRepository; }
  
  /**
   * @param _offer is deleted by this class if needed.
   *               After this call you are no longer allowed to delete
   *               the _offer yourself.
   *               The field 'offerid' in struct Offer does not need to have
   *               a meaningfull value. It is set by this function.
   *
   * @return _offer gets a valid OfferId.
   */
  void export( Offer *_offer );
  char* export( CORBA::Object_ptr reference, const char* type,
			const CosTrading::PropertySeq& properties );
  
  void import( const char* type, const char* constr, const char* pref, const CosTrading::PolicySeq& policies,
	       const CosTrading::Lookup::SpecifiedProps& desired_props, CORBA::ULong how_many,
	       CosTrading::OfferSeq*& offers, CosTrading::OfferIterator_ptr& offer_itr,
	       CosTrading::PolicyNameSeq*& limits_applied );
  
  CORBA::ULong defSearchCard();
  CORBA::ULong maxSearchCard();
  CORBA::ULong defMatchCard();
  CORBA::ULong maxMatchCard();
  CORBA::ULong defReturnCard();
  CORBA::ULong maxReturnCard();
  CORBA::ULong maxList();
  CORBA::ULong defHopCount();
  CORBA::ULong maxHopCount();
  CosTrading::FollowOption defLinkFollowPolicy();
  CosTrading::FollowOption maxLinkFollowPolicy();
  
protected:
  Lookup *m_pLookup;
  TypeRepository* m_pTypeRepository;
  
  CosTrading::Admin::OctetSeq m_requestIdStem;
  list<CosTrading::Admin::OctetSeq> m_lstStems;

  list<Offer*> m_lstOffers;
  
  int m_uniqueId;
};

class OfferIterator : virtual public CosTrading::OfferIterator_skel
{
public:
  OfferIterator( CosTrading::OfferSeq* _offers );
  ~OfferIterator();
  
  virtual CORBA::ULong max_left();
  virtual CORBA::Boolean next_n( CORBA::ULong n, CosTrading::OfferSeq*& offers );
  virtual void destroy();

protected:
  CosTrading::OfferSeq* m_pOffers;
  CORBA::ULong m_pos;
};

#endif
