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

// TODO: Torben: On error free memory!

extern "C"
{
#include "koTraderParse.h"

void mainParse( const char *_code );
}

#include "koTraderParseTree.h"

ParseTreeBase::Ptr pTree = 0L;

ParseTreeBase::Ptr parseConstraints( const QString& _constr )
{
  mainParse( _constr );
  return pTree;
}

ParseTreeBase::Ptr parsePreferences( const QString& _prefs )
{
  mainParse( _prefs );
  return pTree;
}

void setParseTree( void *_ptr1 )
{
  pTree = (ParseTreeBase*)_ptr1;
}

void* newOR( void *_ptr1, void *_ptr2 )
{
  return new ParseTreeOR( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2 );
}

void* newAND( void *_ptr1, void *_ptr2 )
{
  return new ParseTreeAND( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2 );
}

void* newCMP( void *_ptr1, void *_ptr2, int _i )
{
  return new ParseTreeCMP( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2, _i );
}

void* newIN( void *_ptr1, void *_ptr2 )
{
  return new ParseTreeIN( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2 );
}

void* newMATCH( void *_ptr1, void *_ptr2 )
{
  return new ParseTreeMATCH( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2 );
}

void* newCALC( void *_ptr1, void *_ptr2, int _i )
{
  return new ParseTreeCALC( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2, _i );
}

void* newBRACKETS( void *_ptr1 )
{
  return new ParseTreeBRACKETS( (ParseTreeBase*)_ptr1 );
}

void* newNOT( void *_ptr1 )
{
  return new ParseTreeNOT( (ParseTreeBase*)_ptr1 );
}

void* newEXIST( void *_ptr1 )
{
  return new ParseTreeEXIST( (char*)_ptr1 );
}

void* newID( char *_ptr1 )
{
  return new ParseTreeID( _ptr1 );
}

void* newSTRING( char *_ptr1 )
{
  return new ParseTreeSTRING( _ptr1 );
}

void* newNUM( int _i )
{
  return new ParseTreeNUM( _i );
}

void* newFLOAT( float _f )
{
  return new ParseTreeDOUBLE( _f );
}

void* newBOOL( char _b )
{
  return new ParseTreeBOOL( (bool)_b );
}

void* newMAX2( const char *_id )
{
  return new ParseTreeMAX2( _id );
}

void* newMIN2( const char *_id )
{
  return new ParseTreeMIN2( _id );
}
