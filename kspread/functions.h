/* This file is part of the KDE project
   Copyright (C) 2003,2004 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#ifndef KSPREAD_FUNCTIONS
#define KSPREAD_FUNCTIONS

#include <qvaluevector.h>
#include "kspread_value.h"

namespace KSpread
{

class Formula;

typedef KSpreadValue (*FunctionPtr)( const Formula*, QValueVector<KSpreadValue> );

class Function
{
public:
  Function( const QString& name, FunctionPtr ptr ); 
  ~Function();
  QString name() const;
  QString localizedName() const;
  QString helpText() const;
  void setHelpText( const QString& text );
  KSpreadValue exec( const Formula* formula, QValueVector<KSpreadValue> args );
  
private:
  class Private;
  Private* d;
};

class FunctionRepository
{
public:

  FunctionRepository();
  ~FunctionRepository();
    
  static FunctionRepository* self();
    
  void add( Function* function );
  
  Function* function( const QString& name );
    
private:

  class Private;
  Private* d;

  static FunctionRepository* s_self;
    
  // no copy or assign
  FunctionRepository( const FunctionRepository& );
  FunctionRepository& operator=( const FunctionRepository& );
};


}; // namespace KSpread


#endif // KSPREAD_FUNCTIONS
