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

#include "koTraderParseTree.h"

#include <iostream>
#include <assert.h>

bool ParseTreeOR::eval( ParseContext *_context ) const
{
  ParseContext c1( _context );
  ParseContext c2( _context );
  if ( !m_pLeft->eval( &c1 ) )
    return false;
  if ( !m_pRight->eval( &c2 ) )
    return false;
  if ( c1.type != ParseContext::T_BOOL )
    return false;
  if ( c2.type != ParseContext::T_BOOL )
    return false;

  _context->b = ( c1.b || c2.b );
  _context->type = ParseContext::T_BOOL;

  return true;
}

bool ParseTreeAND::eval( ParseContext *_context ) const
{
  _context->type = ParseContext::T_BOOL;

  ParseContext c1( _context );
  ParseContext c2( _context );
  if ( !m_pLeft->eval( &c1 ) )
    return false;
  if ( c1.type != ParseContext::T_BOOL )
    return false;
  if ( !c1.b )
  {
    _context->b = false;
    return true;
  }

  if ( !m_pRight->eval( &c2 ) )
    return false;
  if ( c2.type != ParseContext::T_BOOL )
    return false;

  _context->b = ( c1.b && c2.b );

  return true;
}

bool ParseTreeCALC::eval( ParseContext *_context ) const
{
  ParseContext c1( _context );
  ParseContext c2( _context );
  if ( !m_pLeft->eval( &c1 ) )
    return false;
  if ( !m_pRight->eval( &c2 ) )
    return false;

  // Bool extension
  if ( c1.type != ParseContext::T_NUM && c1.type != ParseContext::T_DOUBLE && c1.type != ParseContext::T_BOOL )
    return false;
  // Bool extension
  if ( c2.type != ParseContext::T_NUM && c2.type != ParseContext::T_DOUBLE && c2.type != ParseContext::T_BOOL )
    return false;
  // Bool extension
  if ( c1.type == ParseContext::T_BOOL && c2.type == ParseContext::T_BOOL )
    return false;

  /**
   * Make types compatible
   */
  if ( c1.type == ParseContext::T_NUM && c2.type == ParseContext::T_DOUBLE )
  {
    c1.type = ParseContext::T_DOUBLE;
    c1.f = (double)c1.i;
  }
  else if ( c1.type == ParseContext::T_DOUBLE && c2.type == ParseContext::T_NUM )
  {
    c2.type = ParseContext::T_DOUBLE;
    c2.f = (double)c2.i;
  }
  // Bool extension
  else if ( c1.type == ParseContext::T_BOOL && c2.type == ParseContext::T_NUM )
  {
    c1.type = ParseContext::T_NUM;
    if ( c1.b )
      c1.i = 1;
    else
      c1.i = -1;
  }
  // Bool extension
  else if ( c1.type == ParseContext::T_BOOL && c2.type == ParseContext::T_DOUBLE )
  {
    c1.type = ParseContext::T_DOUBLE;
    if ( c1.b )
      c1.f = 1.0;
    else
      c1.f = -1.0;
  }
  // Bool extension
  else if ( c1.type == ParseContext::T_NUM && c2.type == ParseContext::T_BOOL )
  {
    c2.type = ParseContext::T_NUM;
    if ( c2.b )
      c2.i = 1;
    else
      c2.i = -1;
  }
  // Bool extension
  else if ( c1.type == ParseContext::T_DOUBLE && c2.type == ParseContext::T_BOOL )
  {
    c2.type = ParseContext::T_DOUBLE;
    if ( c2.b )
      c2.f = 1.0;
    else
      c2.f = -1.0;
  }

  _context->type = c1.type;

  /**
   * Calculate
   */
  switch( m_cmd )
  {
  case 1: /* Add */
    if ( c1.type == ParseContext::T_DOUBLE )
    {
      _context->f = ( c1.f + c2.f );
      return true;
    }
    if ( c1.type == ParseContext::T_NUM )
    {
      _context->i = ( c1.i + c2.i );
      return true;
    }
    break;
  case 2: /* Sub */
    if ( c1.type == ParseContext::T_DOUBLE )
    {
      _context->f = ( c1.f - c2.f );
      return true;
    }
    if ( c1.type == ParseContext::T_NUM )
    {
      _context->i = ( c1.i - c2.i );
      return true;
    }
    break;
  case 3: /* Mul */
    if ( c1.type == ParseContext::T_DOUBLE )
    {
      //cout << "Double Mult" << endl;
      _context->f = ( c1.f * c2.f );
      return true;
    }
    if ( c1.type == ParseContext::T_NUM )
    {
      _context->i = ( c1.i * c2.i );
      return true;
    }
    break;
  case 4: /* Div */
    if ( c1.type == ParseContext::T_DOUBLE )
    {
      _context->f = ( c1.f / c2.f );
      return true;
    }
    if ( c1.type == ParseContext::T_NUM )
    {
      _context->i = ( c1.i / c2.i );
      return true;
    }
    break;
  }

  return false;
}

bool ParseTreeCMP::eval( ParseContext *_context ) const
{
  //cout << "CMP 1 cmd=" << m_cmd << endl;
  ParseContext c1( _context );
  ParseContext c2( _context );
  if ( !m_pLeft->eval( &c1 ) )
    return false;

  if ( !m_pRight->eval( &c2 ) )
    return false;

  /**
   * Make types compatible
   */
  if ( c1.type == ParseContext::T_NUM && c2.type == ParseContext::T_DOUBLE )
  {
    c1.type = ParseContext::T_DOUBLE;
    c1.f = (double)c1.i;
  }
  else if ( c1.type == ParseContext::T_DOUBLE && c2.type == ParseContext::T_NUM )
  {
    c2.type = ParseContext::T_DOUBLE;
    c2.f = (double)c2.i;
  }

  /**
   * Compare
   */
  _context->type = ParseContext::T_BOOL;

  switch( m_cmd )
  {
  case 1: /* EQ */
    if ( c1.type != c2.type )
    {
      _context->b = false;
      return true;
    }
    if ( c1.type == ParseContext::T_STRING )
    {
      _context->b = ( c1.str == c2.str );
      return true;
    }
    if ( c1.type == ParseContext::T_BOOL )
    {
      _context->b = ( c1.b == c2.b );
      return true;
    }
    if ( c1.type == ParseContext::T_DOUBLE )
    {
      _context->b = ( c1.f == c2.f );
      return true;
    }
    if ( c1.type == ParseContext::T_NUM )
    {
      _context->b = ( c1.i == c2.i );
      return true;
    }
    break;
  case 2: /* NEQ */
    if ( c1.type != c2.type )
    {
      _context->b = true;
      return true;
    }
    if ( c1.type == ParseContext::T_STRING )
    {
      _context->b = ( c1.str != c2.str );
      return true;
    }
    if ( c1.type == ParseContext::T_BOOL )
    {
      _context->b = ( c1.b != c2.b );
      return true;
    }
    if ( c1.type == ParseContext::T_DOUBLE )
    {
      _context->b = ( c1.f != c2.f );
      return true;
    }
    if ( c1.type == ParseContext::T_NUM )
    {
      _context->b = ( c1.i != c2.i );
      return true;
    }
    break;
  case 3: /* GEQ */
    if ( c1.type != c2.type )
    {
      _context->b = false;
      return true;
    }
    if ( c1.type == ParseContext::T_DOUBLE )
    {
      _context->b = ( c1.f >= c2.f );
      return true;
    }
    if ( c1.type == ParseContext::T_NUM )
    {
      _context->b = ( c1.i >= c2.i );
      return true;
    }
    _context->b = false;
    return true;

  case 4: /* LEQ */
    if ( c1.type != c2.type )
    {
      _context->b = false;
      return true;
    }
    if ( c1.type == ParseContext::T_DOUBLE )
    {
      _context->b = ( c1.f <= c2.f );
      return true;
    }
    if ( c1.type == ParseContext::T_NUM )
    {
      _context->b = ( c1.i <= c2.i );
      return true;
    }
    _context->b = false;
    return true;

  case 5: /* < */
    if ( c1.type != c2.type )
    {
      _context->b = false;
      return true;
    }
    if ( c1.type == ParseContext::T_DOUBLE )
    {
      _context->b = ( c1.f < c2.f );
      return true;
    }
    if ( c1.type == ParseContext::T_NUM )
    {
      _context->b = ( c1.i < c2.i );
      return true;
    }
    _context->b = false;
    return true;

  case 6: /* > */
    if ( c1.type != c2.type )
    {
      _context->b = false;
      return true;
    }
    if ( c1.type == ParseContext::T_DOUBLE )
    {
      _context->b = ( c1.f > c2.f );
      return true;
    }
    if ( c1.type == ParseContext::T_NUM )
    {
      _context->b = ( c1.i > c2.i );
      return true;
    }
    _context->b = false;
    return true;

  }

  return false;
}

bool ParseTreeNOT::eval( ParseContext *_context ) const
{
  ParseContext c1( _context );
  if ( !m_pLeft->eval( &c1 ) )
    return false;
  if ( c1.type != ParseContext::T_BOOL )
    return false;

  _context->b = !c1.b;
  _context->type = ParseContext::T_BOOL;

  return true;
}

bool ParseTreeEXIST::eval( ParseContext *_context ) const
{
  _context->type = ParseContext::T_BOOL;

  KService::PropertyPtr prop = _context->service->property( m_id );
  _context->b = ( prop );

  return true;
}

bool ParseTreeMATCH::eval( ParseContext *_context ) const
{
  _context->type = ParseContext::T_BOOL;

  ParseContext c1( _context );
  ParseContext c2( _context );
  if ( !m_pLeft->eval( &c1 ) )
    return false;
  if ( !m_pRight->eval( &c2 ) )
    return false;
  if ( c1.type != ParseContext::T_STRING || c2.type != ParseContext::T_STRING )
    return false;

  _context->b = ( c2.str.find( c1.str ) != -1 );

  return true;
}

bool ParseTreeIN::eval( ParseContext *_context ) const
{
  _context->type = ParseContext::T_BOOL;

  ParseContext c1( _context );
  ParseContext c2( _context );
  if ( !m_pLeft->eval( &c1 ) )
    return false;
  if ( !m_pRight->eval( &c2 ) )
    return false;

  if ( c1.type == ParseContext::T_NUM && c2.type == ParseContext::T_NUM_SEQ )
  {
    _context->b = ( c2.numSeq.find( c1.i ) != c2.numSeq.end() );
    return true;
  }

  if ( c1.type == ParseContext::T_DOUBLE && c2.type == ParseContext::T_DOUBLE_SEQ )
  {
    _context->b = ( c2.doubleSeq.find( c1.f ) != c2.doubleSeq.end() );
    return true;
  }

  if ( c1.type == ParseContext::T_STRING && c2.type == ParseContext::T_STR_SEQ )
  {
    _context->b = ( c2.strSeq.find( c1.str ) != c2.strSeq.end() );
    return true;
  }

  return false;
}

bool ParseTreeID::eval( ParseContext *_context ) const
{
  KService::PropertyPtr prop = _context->service->property( m_str );
  if ( !prop )
    return false;

  if ( prop->type() == QVariant::String )
  {
    _context->str = prop->stringValue();
    _context->type = ParseContext::T_STRING;
    return true;
  }

  if ( prop->type() == QVariant::Int )
  {
    _context->i = prop->intValue();
    _context->type = ParseContext::T_NUM;
    return true;
  }

  if ( prop->type() == QVariant::Bool )
  {
    _context->b = prop->boolValue();
    _context->type = ParseContext::T_BOOL;
    return true;
  }

  if ( prop->type() == QVariant::Double )
  {
    _context->f = prop->doubleValue();
    _context->type = ParseContext::T_DOUBLE;
    return true;
  }

  if ( prop->type() == QVariant::IntList )
  {
    _context->numSeq = prop->intListValue();
    return true;
  }

  if ( prop->type() == QVariant::DoubleList )
  {
    _context->doubleSeq = prop->doubleListValue();
    _context->type = ParseContext::T_DOUBLE_SEQ;
    return true;
  }

  if ( prop->type() == QVariant::StringList )
  {
    _context->strSeq = prop->stringListValue();
    _context->type = ParseContext::T_STR_SEQ;
    return true;
  }

  // Value has unknown type
  return false;
}

bool ParseTreeMIN2::eval( ParseContext *_context ) const
{
  _context->type = ParseContext::T_DOUBLE;

  KService::PropertyPtr prop = _context->service->property( m_strId );
  if ( !prop )
    return false;

  if ( !_context->initMaxima( m_strId ) )
    return false;

  QMap<QString,PreferencesMaxima>::Iterator it = _context->maxima.find( m_strId );
  if ( it == _context->maxima.end() )
    return false;

  if ( prop->type() == QVariant::Int && it.data().type == PreferencesMaxima::PM_INT )
  {
    _context->f = (double)( prop->intValue() - it.data().iMin ) /
                  (double)(it.data().iMax - it.data().iMin ) * (-2.0) + 1.0;
    return true;
  }
  else if ( prop->type() == QVariant::Double && it.data().type == PreferencesMaxima::PM_DOUBLE )
  {
    _context->f = ( prop->doubleValue() - it.data().fMin ) / (it.data().fMax - it.data().fMin )
                  * (-2.0) + 1.0;
    return true;
  }

  return false;
}

bool ParseTreeMAX2::eval( ParseContext *_context ) const
{
  _context->type = ParseContext::T_DOUBLE;

  KService::PropertyPtr prop = _context->service->property( m_strId );
  if ( prop )
    return false;

  // Create extrema
  if ( !_context->initMaxima( m_strId ) )
    return false;

  // Find extrema
  QMap<QString,PreferencesMaxima>::Iterator it = _context->maxima.find( m_strId );
  if ( it == _context->maxima.end() )
    return false;

  if ( prop->type() == QVariant::Int && it.data().type == PreferencesMaxima::PM_INT )
  {
    _context->f = (double)( prop->intValue() - it.data().iMin ) /
                  (double)(it.data().iMax - it.data().iMin ) * 2.0 - 1.0;
    return true;
  }
  else if ( prop->type() == QVariant::Double && it.data().type == PreferencesMaxima::PM_DOUBLE )
  {
    _context->f = ( prop->doubleValue() - it.data().fMin ) /
                  (it.data().fMax - it.data().fMin ) * 2.0 - 1.0;
    return true;
  }

  return false;
}

int matchConstraint( const ParseTreeBase *_tree, const KService* _service,
		     const KServiceTypeProfile::OfferList& _list )
{
  // Empty tree matches always
  if ( !_tree )
    return 1;

  QMap<QString,PreferencesMaxima> maxima;
  ParseContext c( _service, _list, maxima );

  // Error during evaluation ?
  if ( !_tree->eval( &c ) )
    return -1;

  // Did we get a bool ?
  if ( c.type != ParseContext::T_BOOL )
    return -1;

  return ( c.b ? 1 : 0 );
}

PreferencesReturn matchPreferences( const ParseTreeBase *_tree, const KService* _service,
				    const KServiceTypeProfile::OfferList& _list )
{
  // By default: error
  PreferencesReturn ret;

  if ( !_tree )
    return ret;

  QMap<QString,PreferencesMaxima> maxima;
  ParseContext c( _service, _list, maxima );

  if ( !_tree->eval( &c ) )
    return ret;

  // Did we get a numeric return value ?
  if ( c.type == ParseContext::T_NUM )
  {
    ret.type = PreferencesReturn::PRT_DOUBLE;
    ret.f = (double)c.i;
  }
  else if ( c.type == ParseContext::T_DOUBLE )
  {
    ret.type = PreferencesReturn::PRT_DOUBLE;
    ret.f = c.f;
  }

  return ret;
}

bool ParseContext::initMaxima( const QString& _prop )
{
  // Is the property known ?
  KService::PropertyPtr prop = service->property( _prop );
  if ( !prop )
    return false;

  // Numeric ?
  if ( prop->type() != QVariant::Int && prop->type() != QVariant::Double )
    return false;

  // Did we cache the result ?
  QMap<QString,PreferencesMaxima>::Iterator it = maxima.find( _prop );
  if ( it != maxima.end() )
    return ( it.data().type == PreferencesMaxima::PM_DOUBLE ||
	     it.data().type == PreferencesMaxima::PM_INT );

  // Double or Int ?
  PreferencesMaxima extrema;
  if ( prop->type() != QVariant::Int )
    extrema.type = PreferencesMaxima::PM_INVALID_INT;
  else
    extrema.type = PreferencesMaxima::PM_INVALID_DOUBLE;

  // Iterate over all offers
  KServiceTypeProfile::OfferList::ConstIterator oit = offers.begin();
  for( ; oit != offers.end(); ++it )
  {
    KService::PropertyPtr p = (*oit).service()->property( _prop );
    if ( p )
    {
      // Determine new maximum/minimum
      if ( extrema.type == PreferencesMaxima::PM_INVALID_INT )
      {
	extrema.type = PreferencesMaxima::PM_INT;
	extrema.iMin = p->intValue();
	extrema.iMax = p->intValue();
      }
      // Correct existing extrema
      else if ( extrema.type == PreferencesMaxima::PM_INT )
      {
	if ( p->intValue() < extrema.iMin )
	  extrema.iMin = p->intValue();
	if ( p->intValue() > extrema.iMax )
	  extrema.iMax = p->intValue();
      }
      // Determine new maximum/minimum
      else if ( extrema.type == PreferencesMaxima::PM_INVALID_DOUBLE )
      {
	extrema.type = PreferencesMaxima::PM_DOUBLE;
	extrema.fMin = p->doubleValue();
	extrema.fMax = p->doubleValue();
      }
      // Correct existing extrema
      else if ( extrema.type == PreferencesMaxima::PM_DOUBLE )
      {
	if ( p->doubleValue() < it.data().fMin )
	  it.data().fMin = p->doubleValue();
	if ( p->doubleValue() > it.data().fMax )
	  it.data().fMax = p->doubleValue();
      }
    }
  }

  // Cache the result
  maxima.insert( _prop, extrema );

  // Did we succeed ?
  return ( extrema.type == PreferencesMaxima::PM_DOUBLE ||
	   extrema.type == PreferencesMaxima::PM_INT );
}

