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

#include <komlWriter.h>

#include <stdlib.h>

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

const char* KSpreadLayout::prefix()
{
    if ( m_strPrefix.data() == 0 )
	return 0L;
    if ( m_strPrefix.data()[0] == 0 )
	return 0L;

    return m_strPrefix.data();
}

const char* KSpreadLayout::postfix()
{
    if ( m_strPostfix.data() == 0 )
	return 0L;
    if ( m_strPostfix.data()[0] == 0 )
	return 0L;

    return m_strPostfix.data();
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

bool RowLayout::save( ostream &out )
{
  out << indent << "<ROW height=\"" << m_fHeight << "\" row=\"" << m_iRow << "\"/>" << endl;

  return true;
}

bool RowLayout::load( KOMLParser& , vector<KOMLAttrib>& _attribs )
{
  vector<KOMLAttrib>::const_iterator it = _attribs.begin();
  for( ; it != _attribs.end(); it++ )
  {
    if ( (*it).m_strName == "height" )
    {
      m_fHeight = atof( (*it).m_strValue.c_str() );
    }
    else if ( (*it).m_strName == "row" )
    {
      m_iRow = atoi( (*it).m_strValue.c_str() );
    }
    else
      cerr << "Unknown attrib '" << (*it).m_strName << "'" << endl;
  }

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

/*
bool RowLayout::load( KorbSession *korb, OBJECT o_rl )
{
    PROPERTY p_height = korb->findProperty( "KDE:kxcl:Height" );
    PROPERTY p_row = korb->findProperty( "KDE:kxcl:Row" );

    if ( p_height == 0L )
    {
	printf("ERROR: Could not find Property KDE:kxcl:Height \n");
	return FALSE;
    }

    PROPERTY prop = 0;
    do
    {
	prop = korb->getNextObjectProperty( o_rl, prop );
	if ( prop == p_height )
	{
	    float h;
	    if ( !korb->readFloatValue( o_rl, prop, h ) )
		return FALSE;
	    height = h;
	}
	else if ( prop == p_row )
	{
	    unsigned int r;
	    if ( !korb->readUIntValue( o_rl, prop, r ) )
		return FALSE;
	    row = r;
	}
    } while ( prop );

    return TRUE;
}
*/

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

bool ColumnLayout::save( ostream &out )
{
  out << indent << "<COLUMN width=\"" << m_fWidth << "\" column=\"" << m_iColumn << "\"/>" << endl;

  return true;
}

bool ColumnLayout::load( KOMLParser& , vector<KOMLAttrib>& _attribs )
{
  vector<KOMLAttrib>::const_iterator it = _attribs.begin();
  for( ; it != _attribs.end(); it++ )
  {
    if ( (*it).m_strName == "width" )
    {
      m_fWidth = atof( (*it).m_strValue.c_str() );
    }
    else if ( (*it).m_strName == "column" )
    {
      m_iColumn = atoi( (*it).m_strValue.c_str() );
    }
    else
      cerr << "Unknown attrib '" << (*it).m_strName << "'" << endl;
  }

  // Validation
  if ( m_fWidth < 1 )
  {
    cerr << "Value height=" << m_fWidth << " out of range" << endl;
    return false;
  }
  if ( m_iColumn < 1 || m_iColumn >= 0xFFFF )
  {
    cerr << "Value row=" << m_iColumn << " out of range" << endl;
    return false;
  }

  return true;
}

/*
OBJECT ColumnLayout::save( KorbSession *korb )
{
    // For use as values in the ObjectType property
    TYPE t_cl  =  korb->registerType( "KDE:kxcl:ColumnLayout" );

    PROPERTY p_width = korb->registerProperty( "KDE:kxcl:Width" );
    PROPERTY p_column = korb->registerProperty( "KDE:kxcl:Column" );

    OBJECT o_cl( korb->newObject( t_cl ) );
	
    korb->writeFloatValue( o_cl, p_width, width );
    korb->writeUIntValue( o_cl, p_column, column );

    return o_cl;
}

bool ColumnLayout::load( KorbSession *korb, OBJECT o_cl )
{
    PROPERTY p_width = korb->findProperty( "KDE:kxcl:Width" );
    PROPERTY p_column = korb->findProperty( "KDE:kxcl:Column" );

    if ( p_width == 0L )
    {
	printf("ERROR: Could not find Property KDE:kxcl:Width \n");
	return FALSE;
    }
    if ( p_column == 0L )
    {
	printf("ERROR: Could not find Property KDE:kxcl:Column \n");
	return FALSE;
    }

    PROPERTY prop = 0;
    do
    {
	prop = korb->getNextObjectProperty( o_cl, prop );
	if ( prop == p_width )
	{
	    float w;
	    if ( !korb->readFloatValue( o_cl, prop, w ) )
		return FALSE;
	    printf("Width = %f\n",w);
	    width = w;
	}
	else if ( prop == p_column )
	{
	    unsigned int c;
	    if ( !korb->readUIntValue( o_cl, prop, c ) )
		return FALSE;
	    printf("Width = %i\n",c);
	    column = c;
	}
    } while ( prop );

    return TRUE;
}
*/



