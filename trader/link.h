/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/     

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
