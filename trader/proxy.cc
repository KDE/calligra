#include "proxy.h"
#include "trader_main.h"

Proxy::Proxy( Trader* _trader ) : ::TraderComponents( _trader ), ::SupportAttributes( _trader ),
	                          CosTrading::Proxy_skel()
{
  m_pTrader5 = _trader;
}
  
char* Proxy::export_proxy( CosTrading::Lookup_ptr target, const char* type,
			   const CosTrading::PropertySeq& properties, CORBA::Boolean if_match_all,
			   const char* recipe, const CosTrading::PolicySeq& policies_to_pass_on )
{
  return m_pTrader5->export_proxy( target, type, properties, if_match_all, recipe, policies_to_pass_on );
}

void Proxy::withdraw_proxy( const char* id )
{
  m_pTrader5->withdraw_proxy( id );
}

CosTrading::Proxy::ProxyInfo* Proxy::describe_proxy( const char* id )
{
  return m_pTrader5->describe_proxy( id );
}
