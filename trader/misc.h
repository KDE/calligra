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

class LinkAttributes : virtual public CosTrading::LinkAttributes
{
public:
  LinkAttributes( Trader *_trader );

  virtual CosTrading::FollowOption max_link_follow_policy();

private:
  /**
   * Renamed to solve problem with multiple inheritance. I dont want two member
   * variables with the same name.
   */
  Trader* m_pTrader5;
};

#endif
