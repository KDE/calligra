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

#ifndef __typerepo_impl_h__
#define __typerepo_impl_h__

#include "typerepo.h"

#include <string>
#include <map>

class Trader;

class TypeRepository : virtual public CosTradingRepos::ServiceTypeRepository_skel
{
public:
  TypeRepository( Trader* _trader );
  
  virtual CosTradingRepos::ServiceTypeRepository::IncarnationNumber incarnation();
  virtual CosTradingRepos::ServiceTypeRepository::IncarnationNumber
          add_type( const char* name,
		    const char* if_name,
		    const CosTradingRepos::ServiceTypeRepository::PropStructSeq& props,
		    const CosTradingRepos::ServiceTypeRepository::ServiceTypeNameSeq& super_types );
  virtual void remove_type( const char* name );
  virtual CosTradingRepos::ServiceTypeRepository::ServiceTypeNameSeq*
          list_types( const CosTradingRepos::ServiceTypeRepository::SpecifiedServiceTypes& which_types );
  virtual CosTradingRepos::ServiceTypeRepository::TypeStruct* describe_type( const char*name );
  virtual CosTradingRepos::ServiceTypeRepository::TypeStruct* fully_describe_type( const char* name );
  virtual void mask_type( const char* name );
  virtual void unmask_type( const char* name );

  // Extensions
  /**
   * @return true if 'sub' is a subtype of super. This means that one can return 'sub' if
   *         the client wants to have 'super'.
   */
  bool TypeRepository::isSubTypeOf( const char* sub, const char* super );
  /**
   * @return true if the requested service type is known in the repository. This function does NOT raise
   *         an exception if the servicetype is unknown like all the others do.
   */
  bool TypeRepository::isServiceTypeKnown( const char* name );
  
protected:
  /**
   * Helper function for the recursion.
   */
  void fully_describe_type( CosTradingRepos::ServiceTypeRepository::TypeStruct* result,
			    CosTradingRepos::ServiceTypeRepository::TypeStruct& super_type );
  /**
   * Increases the incarnation number.
   */
  void incIncarnationNumber();
  /**
   * Checks wether a certain service type is registered and returns an iterator
   * pointing at this service type. If the service type does not exist, then
   * the exception @ref CosTading::UnknownServiceType is thrown.
   */
  map<string,CosTradingRepos::ServiceTypeRepository::TypeStruct>::iterator
          checkServiceType( const char* name );
    
  Trader* m_pTrader;

  CosTradingRepos::ServiceTypeRepository::IncarnationNumber m_incarnation;

  map<string,CosTradingRepos::ServiceTypeRepository::TypeStruct> m_mapTypes;
};

#endif
