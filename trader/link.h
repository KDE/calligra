#ifndef __trader_link_h__
#define __trader_link_h__

class Link;
class Trader;

#include "misc.h"
#include "trader.h"

#include <map>
#include <string>

class Link :  virtual public TraderComponents,
	      virtual public SupportAttributes,
	      virtual public LinkAttributes,
	      virtual public CosTrading::Link_skel
{
public:
  Link( Trader* trader );
  
  virtual void add_link( const char* name, CosTrading::Lookup_ptr target, CosTrading::FollowOption def_pass_on_follow_rule,
			 CosTrading::FollowOption limiting_follow_rule );
  virtual void remove_link( const char* name );
  virtual CosTrading::Link::LinkInfo* describe_link( const char* name );
  virtual CosTrading::LinkNameSeq* list_links();
  virtual void modify_link( const char* name, CosTrading::FollowOption def_pass_on_follow_rule,
			    CosTrading::FollowOption limiting_follow_rule );

  /////////////////
  // Extensions
  /////////////////
  /**
   * This iterator allows the trader to traverse all linked traders.
   */
  map<string,CosTrading::Link::LinkInfo>::iterator begin() { return m_mapLinks.begin(); }
  map<string,CosTrading::Link::LinkInfo>::iterator end() { return m_mapLinks.end(); }
  
protected:
  map<string,CosTrading::Link::LinkInfo> m_mapLinks;
};

#endif
