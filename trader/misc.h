#ifndef __misc_h__
#define __misc_h__

#include "trader.h"

class Trader;

class TraderComponents : virtual public CosTrading::TraderComponents_skel
{
public:
  TraderComponents( Trader* _trader );
  
  CosTrading::Lookup_ptr lookup_if();
  CosTrading::Register_ptr register_if();
  CosTrading::Link_ptr link_if();
  CosTrading::Proxy_ptr proxy_if();
  CosTrading::Admin_ptr admin_if();

protected:
  Trader *m_pTrader;
};

class SupportAttributes : virtual public CosTrading::SupportAttributes_skel
{
public:
  SupportAttributes( Trader* _trader );
  
  CORBA::Boolean supports_modifiable_properties();
  CORBA::Boolean supports_dynamic_properties();
  CORBA::Boolean supports_proxy_offers();
  CosTrading::TypeRepository_ptr type_repos();

protected:
  /**
   * Renamed to solve problem with multiple inheritance. I dont want two member
   * variables with the same name.
   */
  Trader *m_pTrader3;
};

class ImportAttributes : virtual public CosTrading::ImportAttributes_skel
{
public:
  ImportAttributes( Trader *_trader );
  
  CORBA::ULong def_search_card();
  CORBA::ULong max_search_card();
  CORBA::ULong def_match_card();
  CORBA::ULong max_match_card();
  CORBA::ULong def_return_card();
  CORBA::ULong max_return_card();
  CORBA::ULong max_list();
  CORBA::ULong def_hop_count();
  CORBA::ULong max_hop_count();
  CosTrading::FollowOption def_follow_policy();
  CosTrading::FollowOption max_follow_policy();

private:
  /**
   * Renamed to solve problem with multiple inheritance. I dont want two member
   * variables with the same name.
   */
  Trader *m_pTrader2;
};

#endif
