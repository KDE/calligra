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

#include "link.h"
#include "trader_main.h"

Link::Link( Trader* _trader ) : ::TraderComponents( _trader ), ::SupportAttributes( _trader ),
                                ::LinkAttributes( _trader ), CosTrading::Link_skel()
{
}

  
void Link::add_link( const char* name, CosTrading::Lookup_ptr target, CosTrading::FollowOption def_pass_on_follow_rule,
		     CosTrading::FollowOption limiting_follow_rule )
{
  map<string,CosTrading::Link::LinkInfo>::iterator it = m_mapLinks.find( name );
  if ( it != m_mapLinks.end() )
  {
    CosTrading::Link::DuplicateLinkName exc;
    exc.name = CORBA::string_dup( name );
    mico_throw( exc );
  }
  
  if ( CORBA::is_nil( target ) )
  {
    CosTrading::InvalidLookupRef exc;
    exc.target = CosTrading::Lookup::_duplicate( target );
    mico_throw( exc );
  }
  
  CosTrading::Register_var reg = target->register_if();
  if ( CORBA::is_nil( reg ) )
  {
    CosTrading::InvalidLookupRef exc;
    exc.target = CosTrading::Lookup::_duplicate( target );
    mico_throw( exc );
  }
  
  if ( def_pass_on_follow_rule > limiting_follow_rule )
  {
    CosTrading::Link::DefaultFollowTooPermissive exc;
    exc.def_pass_on_follow_rule = def_pass_on_follow_rule;
    exc.limiting_follow_rule = limiting_follow_rule;
    mico_throw( exc );
  }
  
  if ( limiting_follow_rule > m_pTrader->maxLinkFollowPolicy() )
  {
    CosTrading::Link::LimitingFollowTooPermissive exc;
    exc.limiting_follow_rule = limiting_follow_rule;
    exc.max_link_follow_policy = m_pTrader->maxLinkFollowPolicy();
    mico_throw( exc );
  }
  
  CosTrading::Link::LinkInfo info;
  info.target = CosTrading::Lookup::_duplicate( target );
  info.target_reg = reg;
  info.def_pass_on_follow_rule = def_pass_on_follow_rule;
  info.limiting_follow_rule = limiting_follow_rule;
 
  m_mapLinks[ name ] = info;
}

void Link::remove_link( const char* name )
{
  map<string,CosTrading::Link::LinkInfo>::iterator it = m_mapLinks.find( name );
  if ( it == m_mapLinks.end() )
  {
    CosTrading::Link::UnknownLinkName exc;
    exc.name = CORBA::string_dup( name );
    mico_throw( exc );
  }

  m_mapLinks.erase( it );
}

CosTrading::Link::LinkInfo* Link::describe_link( const char* name )
{
  map<string,CosTrading::Link::LinkInfo>::iterator it = m_mapLinks.find( name );
  if ( it == m_mapLinks.end() )
  {
    CosTrading::Link::UnknownLinkName exc;
    exc.name = CORBA::string_dup( name );
    mico_throw( exc );
  }

  CosTrading::Link::LinkInfo *info = new CosTrading::Link::LinkInfo;
  (*info) = it->second;
  
  return info;
}

CosTrading::LinkNameSeq* Link::list_links()
{
  CosTrading::LinkNameSeq* seq = new CosTrading::LinkNameSeq;
  unsigned int size = m_mapLinks.size();
  seq->length( size );
  
  map<string,CosTrading::Link::LinkInfo>::iterator it = m_mapLinks.begin();
  
  for( unsigned int i = 0; i < size; i++ )
  {    
    (*seq)[i] = CORBA::string_dup( it->first.c_str() );
    it++;
  }
  
  return seq;
}

void Link::modify_link( const char* name, CosTrading::FollowOption def_pass_on_follow_rule,
			CosTrading::FollowOption limiting_follow_rule )
{
  map<string,CosTrading::Link::LinkInfo>::iterator it = m_mapLinks.find( name );
  if ( it == m_mapLinks.end() )
  {
    CosTrading::Link::UnknownLinkName exc;
    exc.name = CORBA::string_dup( name );
    mico_throw( exc );
  }

  if ( def_pass_on_follow_rule > limiting_follow_rule )
  {
    CosTrading::Link::DefaultFollowTooPermissive exc;
    exc.def_pass_on_follow_rule = def_pass_on_follow_rule;
    exc.limiting_follow_rule = limiting_follow_rule;
    mico_throw( exc );
  }
  
  if ( limiting_follow_rule > m_pTrader->maxLinkFollowPolicy() )
  {
    CosTrading::Link::LimitingFollowTooPermissive exc;
    exc.limiting_follow_rule = limiting_follow_rule;
    exc.max_link_follow_policy = m_pTrader->maxLinkFollowPolicy();
    mico_throw( exc );
  }
  
  m_mapLinks[ name ].def_pass_on_follow_rule = def_pass_on_follow_rule;
  m_mapLinks[ name ].limiting_follow_rule = limiting_follow_rule;
}
