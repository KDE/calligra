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
