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
#include <kdebug.h>
#include <koGlobal.h>

using namespace std;

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
    m_bgColor = QColor();
    m_eAlign = KSpreadLayout::Undefined;
    m_eAlignY = KSpreadLayout::Middle;
    m_iLeftBorderWidth = 1;
    m_iTopBorderWidth = 1;
    m_iFallDiagonalWidth = 1;
    m_iGoUpDiagonalWidth = 1;
    m_leftBorderPen.setColor( Qt::black );
    m_leftBorderPen.setWidth( leftBorderWidth() );
    m_leftBorderPen.setStyle( Qt::NoPen );
    m_topBorderPen.setColor( Qt::black );
    m_topBorderPen.setWidth( topBorderWidth() );
    m_topBorderPen.setStyle( Qt::NoPen );
    m_fallDiagonalPen.setColor( Qt::black );
    m_fallDiagonalPen.setWidth( fallDiagonalWidth() );
    m_fallDiagonalPen.setStyle( Qt::NoPen );
    m_goUpDiagonalPen.setColor( Qt::black );
    m_goUpDiagonalPen.setWidth( goUpDiagonalWidth() );
    m_goUpDiagonalPen.setStyle( Qt::NoPen );
    m_backGroundBrush.setStyle( Qt::NoBrush);
    m_backGroundBrush.setColor( Qt::red );
    m_dFaktor = 1.0;
    m_bMultiRow = FALSE;
    m_bVerticalText=FALSE;
    m_textColor = QColor();
    //m_textPen.setColor( m_textColor );
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
  m_eAlignY = _l.alignY();
  m_iLeftBorderWidth = _l.leftBorderWidth();
  m_iTopBorderWidth = _l.topBorderWidth();
  m_iFallDiagonalWidth = _l.fallDiagonalWidth();
  m_iGoUpDiagonalWidth = _l.goUpDiagonalWidth();
  m_leftBorderPen.setColor( _l.leftBorderColor() );
  m_leftBorderPen.setStyle( _l.leftBorderStyle() );
  m_leftBorderPen.setWidth( _l.leftBorderWidth() );
  m_topBorderPen.setColor( _l.topBorderColor() );
  m_topBorderPen.setStyle( _l.topBorderStyle() );
  m_topBorderPen.setWidth( _l.topBorderWidth() );
  m_fallDiagonalPen.setColor( _l.fallDiagonalColor() );
  m_fallDiagonalPen.setStyle( _l.fallDiagonalStyle() );
  m_fallDiagonalPen.setWidth( _l.fallDiagonalWidth() );
  m_goUpDiagonalPen.setColor( _l.goUpDiagonalColor() );
  m_goUpDiagonalPen.setStyle( _l.goUpDiagonalStyle() );
  m_goUpDiagonalPen.setWidth( _l.goUpDiagonalWidth() );
  m_backGroundBrush.setStyle( _l.backGroundBrushStyle());
  m_backGroundBrush.setColor( _l.backGroundBrushColor());
  m_dFaktor = _l.faktor();
  m_bMultiRow = _l.multiRow();
  m_textColor = _l.textColor();
  m_textPen.setColor( m_textColor );
  setTextFontSize( _l.textFontSize() );
  setTextFontFamily( _l.textFontFamily() );
  setTextFontBold( _l.textFontBold() );
  setTextFontItalic( _l.textFontItalic() );
  setTextFontUnderline( _l.textFontUnderline() );
  setTextFontStrike( _l.textFontStrike() );
  setPrefix( _l.prefix() );
  setPostfix( _l.postfix() );
  m_strComment = _l.getComment();
  m_bVerticalText = _l.verticalText();
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

int KSpreadLayout::fallDiagonalWidth( KSpreadCanvas *_canvas )
{
    if ( _canvas )
      return (int) ( m_iFallDiagonalWidth * _canvas->zoom() );
    else
	return m_iFallDiagonalWidth;
}

int KSpreadLayout::goUpDiagonalWidth( KSpreadCanvas *_canvas )
{
    if ( _canvas )
      return (int) ( m_iGoUpDiagonalWidth * _canvas->zoom() );
    else
	return m_iGoUpDiagonalWidth;
}

const QColor & KSpreadLayout::bgColor() const
{
  return m_bgColor.isValid() ? m_bgColor : QApplication::palette().active().base();
}

const QColor & KSpreadLayout::textColor() const
{
  return m_textColor.isValid() ? m_textColor : QApplication::palette().active().text();
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
    m_next = 0;
    m_prev = 0;
    
    m_bDisplayDirtyFlag = false;
    m_fHeight = POINT_TO_MM(20.0);
    m_iRow = _row;
    m_bDefault = false;
}

RowLayout::~RowLayout()
{
    if ( m_next )
	m_next->setPrevious( m_prev );
    if ( m_prev )
	m_prev->setNext( m_next );
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
    m_fHeight = POINT_TO_MM( _h / _canvas->zoom() );
  else
    m_fHeight = POINT_TO_MM( _h  );

  UPDATE_END;
}

int RowLayout::height( KSpreadCanvas *_canvas )
{
  if ( _canvas )
    return (int)( MM_TO_POINT(_canvas->zoom() * m_fHeight));
  else
    return (int)(MM_TO_POINT(m_fHeight));
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
    kdDebug(36001) << "Value height=" << m_fHeight << " out of range" << endl;
    return false;
  }
  if ( m_iRow < 1 || m_iRow >= 0xFFFF )
  {
    kdDebug(36001) << "Value row=" << m_iRow << " out of range" << endl;
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
  m_fWidth = POINT_TO_MM(60.0);
  m_iColumn = _column;
  m_bDefault=false;
  m_prev = 0;
  m_next = 0;
}

ColumnLayout::~ColumnLayout()
{
    if ( m_next )
	m_next->setPrevious( m_prev );
    if ( m_prev )
	m_prev->setNext( m_next );
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
      m_fWidth = POINT_TO_MM(_w / _canvas->zoom());
  else
      m_fWidth = POINT_TO_MM(_w);

  UPDATE_END;
}

int ColumnLayout::width( KSpreadCanvas *_canvas )
{
  if ( _canvas )
    return (int)(MM_TO_POINT( _canvas->zoom() * m_fWidth));
  else
    return (int)(MM_TO_POINT( m_fWidth ));
}

QDomElement ColumnLayout::save( QDomDocument& doc )
{
  QDomElement col = doc.createElement( "column" );
  col.setAttribute( "width", m_fWidth );
  col.setAttribute( "column", m_iColumn );

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

  m_iColumn = col.attribute( "column" ).toInt( &ok );
  if ( !ok ) return false;

  // Validation
  if ( m_fWidth < 1 )
  {
    kdDebug(36001) << "Value width=" << m_fWidth << " out of range" << endl;
    return false;
  }
  if ( m_iColumn < 1 || m_iColumn >= 0xFFFF )
  {
    kdDebug(36001) << "Value col=" << m_iColumn << " out of range" << endl;
    return false;
  }

  return true;
}

#undef UPDATE_BEGIN
#undef UPDATE_END


