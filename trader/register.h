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

#ifndef __register_h__
#define __register_h__

class Trader;
class Register;

#include "misc.h"

class Register : virtual public TraderComponents, virtual public SupportAttributes,
		 virtual public CosTrading::Register_skel
{
public:
  Register( Trader *_trader );
  
  char* export( CORBA::Object_ptr reference, const char* type,
		const CosTrading::PropertySeq& properties );
  void withdraw( const char* id );
  CosTrading::Register::OfferInfo* describe( const char* id );
  void modify( const char* id, const CosTrading::PropertyNameSeq& del_list,
	       const CosTrading::PropertySeq& modify_list );
  void withdraw_using_constraint( const char* type, const char* constr );
  CosTrading::Register_ptr resolve( const CosTrading::TraderName& name );
};

#endif
