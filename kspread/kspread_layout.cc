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

#include "kspread_canvas.h"
#include "kspread_layout.h"
#include "kspread_table.h"

#include <stdlib.h>

#include <qdom.h>

/*****************************************************************************
 *
 * KSpreadLayout
 *
 *****************************************************************************/

KSpreadLayout::KSpreadLayout( KSpreadTable *_table )
{
    m_pTable = _table;

    m_eFloatColor = KSpreadLayout::AllBlack;
    m_eFloatFormat = KSpreadLayout::OnlyNegSigned;
    m_iPrecision = -1;
    m_bgColor = Qt::white;
    m_eAlign = KSpreadLayout::Undefined;
    m_iLeftBorderWidth = 1;
    m_iTopBorderWidth = 1;
    m_leftBorderPen.setColor( Qt::black );
    m_leftBorderPen.setWidth( leftBorderWidth() );
    m_leftBorderPen.setStyle( Qt::NoPen );
    m_topBorderPen.setColor( Qt::black );
    m_topBorderPen.setWidth( topBorderWidth() );
    m_topBorderPen.setStyle( Qt::NoPen );
    m_dFaktor = 1.0;
    m_bMultiRow = FALSE;

    m_textColor = Qt::black;
    m_textPen.setColor( m_textColor );
}

KSpreadLayout::~KSpreadLayout()
{
}

void KSpreadLayout::copy( KSpreadLayout &_l )
{
  m_eFloatColor = _l.floatColor();
  m_eFloatFormat = _l.floatFormat();
  m_iPrecision = _l.precision();
  m_bgColor = _l.bgColor();
  m_eAlign = _l.align();
  m_iLeftBorderWidth = _l.leftBorderWidth();
  m_iTopBorderWidth = _l.topBorderWidth();
  m_leftBorderPen.setColor( _l.leftBorderColor() );
  m_leftBorderPen.setStyle( _l.leftBorderStyle() );
  m_leftBorderPen.setWidth( _l.leftBorderWidth() );
  m_topBorderPen.setColor( _l.topBorderColor() );
  m_topBorderPen.setStyle( _l.topBorderStyle() );
  m_topBorderPen.setWidth( _l.topBorderWidth() );
  m_dFaktor = _l.faktor();
  m_bMultiRow = _l.multiRow();
  m_textColor = _l.textColor();
  m_textPen.setColor( m_textColor );
  setTextFontSize( _l.textFontSize() );
  setTextFontFamily( _l.textFontFamily() );
  setTextFontBold( _l.textFontBold() );
  setTextFontItalic( _l.textFontItalic() );
  setPrefix( _l.prefix() );
  setPostfix( _l.postfix() );
}

QString KSpreadLayout::prefix() const
{
    return m_strPrefix;
}

QString KSpreadLayout::postfix() const
{
    return m_strPostfix;
}

int KSpreadLayout::leftBorderWidth( KSpreadCanvas *_canvas )
{
    if ( _canvas )
      return (int) ( m_iLeftBorderWidth * _canvas->zoom() );
    else
	return m_iLeftBorderWidth;
}

int KSpreadLayout::topBorderWidth( KSpreadCanvas *_canvas )
{
    if ( _canvas )
	return (int) ( m_iTopBorderWidth * _canvas->zoom() );
    else
	return m_iTopBorderWidth;
}


/*****************************************************************************
 *
 * KRowLayout
 *
 *****************************************************************************/

#define UPDATE_BEGIN bool b_update_begin = m_bDisplayDirtyFlag; m_bDisplayDirtyFlag = true;
#define UPDATE_END if ( !b_update_begin && m_bDisplayDirtyFlag ) m_pTable->emit_updateRow( this, m_iRow );

RowLayout::RowLayout( KSpreadTable *_table, int _row ) : KSpreadLayout( _table )
{
  m_bDisplayDirtyFlag = false;
  m_fHeight = 20 * POINT_TO_MM;
  m_iRow = _row;
  m_bDefault=false;
}

void RowLayout::setMMHeight( float _h )
{
  UPDATE_BEGIN;

  m_fHeight = _h;

  UPDATE_END;
}
void RowLayout::setHeight( int _h, KSpreadCanvas *_canvas )
{
  UPDATE_BEGIN;

  if ( _canvas )
    m_fHeight = ( float)_h / _canvas->zoom() * POINT_TO_MM;
  else
    m_fHeight = ( float)_h / POINT_TO_MM;

  UPDATE_END;
}

int RowLayout::height( KSpreadCanvas *_canvas )
{
  if ( _canvas )
    return (int)( _canvas->zoom() * m_fHeight * MM_TO_POINT );
  else
    return (int)(m_fHeight * MM_TO_POINT);
}

QDomElement RowLayout::save( QDomDocument& doc )
{
  QDomElement row = doc.createElement( "row" );
  row.setAttribute( "height", m_fHeight );
  row.setAttribute( "row", m_iRow );

  return row;
}

bool RowLayout::load( const QDomElement& row )
{
  bool ok;
  if ( row.hasAttribute( "height" ) )
  {
    m_fHeight = row.attribute( "height" ).toFloat( &ok );
    if ( !ok ) return false;
  }

  m_iRow = row.attribute( "row" ).toInt( &ok );
  if ( !ok ) return false;

  // Validation
  if ( m_fHeight < 1 )
  {
    cerr << "Value height=" << m_fHeight << " out of range" << endl;
    return false;
  }
  if ( m_iRow < 1 || m_iRow >= 0xFFFF )
  {
    cerr << "Value row=" << m_iRow << " out of range" << endl;
    return false;
  }

  return true;
}

/*****************************************************************************
 *
 * KColumnLayout
 *
 *****************************************************************************/

#undef UPDATE_BEGIN
#undef UPDATE_END

#define UPDATE_BEGIN bool b_update_begin = m_bDisplayDirtyFlag; m_bDisplayDirtyFlag = true;
#define UPDATE_END if ( !b_update_begin && m_bDisplayDirtyFlag ) m_pTable->emit_updateColumn( this, m_iColumn );

ColumnLayout::ColumnLayout( KSpreadTable *_table, int _column ) : KSpreadLayout( _table )
{
  m_bDisplayDirtyFlag = false;
  m_fWidth = 60.0 * POINT_TO_MM;
  m_iColumn = _column;
  m_bDefault=false;
}

void ColumnLayout::setMMWidth( float _w )
{
  UPDATE_BEGIN;

  m_fWidth = _w;

  UPDATE_END;
}

void ColumnLayout::setWidth( int _w, KSpreadCanvas *_canvas )
{
  UPDATE_BEGIN;

  if ( _canvas )
    m_fWidth = ( float)_w / _canvas->zoom() * POINT_TO_MM;
  else
    m_fWidth = ( float)_w / POINT_TO_MM;

  UPDATE_END;
}

int ColumnLayout::width( KSpreadCanvas *_canvas )
{
  if ( _canvas )
    return (int)( _canvas->zoom() * m_fWidth * MM_TO_POINT );
  else
    return (int)( m_fWidth * MM_TO_POINT );
}

QDomElement ColumnLayout::save( QDomDocument& doc )
{
  QDomElement col = doc.createElement( "col" );
  col.setAttribute( "width", m_fWidth );
  col.setAttribute( "col", m_iColumn );

  return col;
}

bool ColumnLayout::load( const QDomElement& col )
{
  bool ok;
  if ( col.hasAttribute( "width" ) )
  {
    m_fWidth = col.attribute( "width" ).toFloat( &ok );
    if ( !ok ) return false;
  }

  m_iColumn = col.attribute( "col" ).toInt( &ok );
  if ( !ok ) return false;

  // Validation
  if ( m_fWidth < 1 )
  {
    cerr << "Value width=" << m_fWidth << " out of range" << endl;
    return false;
  }
  if ( m_iColumn < 1 || m_iColumn >= 0xFFFF )
  {
    cerr << "Value col=" << m_iColumn << " out of range" << endl;
    return false;
  }

  return true;
}

#undef UPDATE_BEGIN
#undef UPDATE_END


