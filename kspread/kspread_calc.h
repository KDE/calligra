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

#ifndef __calc_h__
#define __calc_h__

#include "kspread_util.h"

class KSpreadTable;

/**
 * This struct is used to hold one of the KSpread data types.
 * It is used during evaluation of some formula.
 */
struct KSpreadValue
{
  enum ValueType { DoubleType, RangeType, BoolType, ErrorType };

  ValueType type;
  union
  {  
    KSpreadRange* r;
    double d;
    bool b;
  } value;
};

/**
 * Calculates the formula and returns the result. Which can
 * be of type boolean or double currently.
 */
KSpreadValue evalFormular( const char *_formular, KSpreadTable* _table );
/**
 * Creates a list of dependencies.
 */
bool makeDepend( const char* _formular, KSpreadTable* _table, QList<KSpreadDepend>* _list );

#include "kspread_calcerr.h"

#endif
