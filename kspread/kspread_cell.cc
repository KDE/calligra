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

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include <qpainter.h>
#include <qdrawutil.h>
#include <qpoint.h>
#include <qpointarray.h>
#include <qsimplerichtext.h>
#include <qpopupmenu.h>
#include <qdom.h>
#include <qstyle.h>

#include "kspread_table.h"
#include "kspread_canvas.h"
#include "kspread_map.h"
#include "kspread_cell.h"
#include "kspread_interpreter.h"
#include "kspread_doc.h"
#include "kspread_util.h"
#include "kspread_factory.h"

#include <koStream.h>
#include <kformula.h>
#include <kinstance.h>
#include <klocale.h>
#include <kglobal.h>
#include <kscript_parsenode.h>

#define UPDATE_BEGIN bool b_update_begin = m_bDisplayDirtyFlag; m_bDisplayDirtyFlag = true;
#define UPDATE_END if ( !b_update_begin && m_bDisplayDirtyFlag ) m_pTable->emit_updateCell( this, m_iColumn, m_iRow );
#define DO_UPDATE m_pTable->emit_updateCell( this, m_iColumn, m_iRow )

char KSpreadCell::decimal_point = '\0';

/*****************************************************************************
 *
 * KSpreadCell
 *
 *****************************************************************************/

KSpreadCell::KSpreadCell( KSpreadTable *_table, int _column, int _row )
  : KSpreadLayout( _table )
{
  m_pCode = 0;
  m_pPrivate = 0L;
  m_pQML = 0;
  m_pVisualFormula = 0;
  m_bError = false;

  m_lstDepends.setAutoDelete( TRUE );

  m_bLayoutDirtyFlag= FALSE;

  QFont font( "Times", 12 );
  m_textFont = font;

  m_style = KSpreadCell::ST_Normal;
  m_content = Text;

  m_iRow = _row;
  m_iColumn = _column;

  m_bCalcDirtyFlag = FALSE;
  m_bValue = FALSE;
  m_bBool = FALSE;
  m_bProgressFlag = FALSE;
  m_bDisplayDirtyFlag = false;

  m_bForceExtraCells = FALSE;
  m_iExtraXCells = 0;
  m_iExtraYCells = 0;
  m_iExtraWidth = 0;
  m_iExtraHeight = 0;
  m_pObscuringCell = 0;
  m_richWidth=0;
  m_richHeight=0;
  m_iPrecision = -1;
}

void KSpreadCell::copyLayout( int _column, int _row )
{
  KSpreadCell *o = m_pTable->cellAt( _column, _row );

  setAlign( o->align() );
  setTextFont( o->textFont() );
  setTextColor( o->textColor() );
  setBgColor( o->bgColor( _column, _row) );
  setLeftBorderWidth( o->leftBorderWidth( _column, _row ) );
  setLeftBorderStyle( o->leftBorderStyle( _column, _row ) );
  setLeftBorderColor( o->leftBorderColor( _column, _row ) );
  setTopBorderWidth( o->topBorderWidth( _column, _row ) );
  setTopBorderStyle( o->topBorderStyle( _column, _row ) );
  setTopBorderColor( o->topBorderColor( _column, _row ) );
  setBottomBorderWidth( o->bottomBorderWidth( _column, _row ) );
  setBottomBorderStyle( o->bottomBorderStyle( _column, _row ) );
  setBottomBorderColor( o->bottomBorderColor( _column, _row ) );
  setRightBorderWidth( o->rightBorderWidth( _column, _row ) );
  setRightBorderStyle( o->rightBorderStyle( _column, _row ) );
  setRightBorderColor( o->rightBorderColor( _column, _row ) );
  setFallDiagonalWidth( o->fallDiagonalWidth( _column, _row ) );
  setFallDiagonalStyle( o->fallDiagonalStyle( _column, _row ) );
  setFallDiagonalColor( o->fallDiagonalColor( _column, _row ) );
  setGoUpDiagonalWidth( o->goUpDiagonalWidth( _column, _row ) );
  setGoUpDiagonalStyle( o->goUpDiagonalStyle( _column, _row ) );
  setGoUpDiagonalColor( o->goUpDiagonalColor( _column, _row ) );
  setPrecision( o->precision() );
  setPrefix( o->prefix() );
  setPostfix( o->postfix() );
  setFloatFormat( o->floatFormat() );
  setFloatColor( o->floatColor() );
  setFaktor( o->faktor() );
  setMultiRow( o->multiRow() );
}

void KSpreadCell::copyAll( KSpreadCell *cell)
{

  setText(cell->text());
  setAlign( cell->align() );
  setTextFont( cell->textFont() );
  setTextColor( cell->textColor() );
  setBgColor( cell->bgColor( ) );
  //When you use column() and row() it don't work

  //setLeftBorderWidth( cell->leftBorderWidth( ) );
  //setLeftBorderStyle( cell->leftBorderStyle(  ) );
  //setLeftBorderColor( cell->leftBorderColor(  ) );
 // setTopBorderWidth( cell->topBorderWidth(  ) );
  //setTopBorderStyle( cell->topBorderStyle(  ) );
  //setTopBorderColor( cell->topBorderColor(  ) );
  //setBottomBorderWidth( cell->bottomBorderWidth( ) );
  //setBottomBorderStyle( cell->bottomBorderStyle(  ) );
  //setBottomBorderColor( cell->bottomBorderColor( ) );
  //setRightBorderWidth( cell->rightBorderWidth( ) );
  //setRightBorderStyle( cell->rightBorderStyle( ) );
  //setRightBorderColor( cell->rightBorderColor(  ) );
  setPrecision( cell->precision() );
  setPrefix( cell->prefix() );
  setPostfix( cell->postfix() );
  setFloatFormat( cell->floatFormat() );
  setFloatColor( cell->floatColor() );
  setFaktor( cell->faktor() );
  setMultiRow( cell->multiRow() );

}

void KSpreadCell::forceExtraCells( int _col, int _row, int _x, int _y )
{
  // Unobscure the objects we obscure right now
  for( int x = _col; x <= _col + m_iExtraXCells; x++ )
    for( int y = _row; y <= _row + m_iExtraYCells; y++ )
      if ( x != _col || y != _row )
      {
	KSpreadCell *cell = m_pTable->nonDefaultCell( x, y );
	cell->unobscure();
      }

  // disable forcing ?
  if ( _x == 0 && _y == 0 )
  {
    m_bForceExtraCells = FALSE;
    m_iExtraXCells = 0;
    m_iExtraYCells = 0;
	return;
    }

    m_bForceExtraCells = TRUE;
    m_iExtraXCells = _x;
    m_iExtraYCells = _y;

    // Obscure the cells
    for( int x = _col; x <= _col + _x; x++ )
	for( int y = _row; y <= _row + _y; y++ )
	    if ( x != _col || y != _row )
	    {
		KSpreadCell *cell = m_pTable->nonDefaultCell( x, y );
		cell->obscure( this, _col, _row );
	    }

    // Refresh the layout
    // QPainter painter;
    // painter.begin( m_pTable->gui()->canvasWidget() );

    m_bLayoutDirtyFlag = TRUE;
    makeLayout( m_pTable->painter(), _col, _row );
}

void KSpreadCell::setLayoutDirtyFlag()
{
    m_bLayoutDirtyFlag= TRUE;
	
    if ( m_pObscuringCell )
    {
	m_pObscuringCell->setLayoutDirtyFlag();
	return;
    }
}

bool KSpreadCell::isEmpty()
{
  if ( isDefault() )
	return TRUE;

  if ( m_strText.isEmpty() )
    return TRUE;

  return FALSE;
}

bool KSpreadCell::isObscuringForced()
{
    if ( !m_pObscuringCell )
	return FALSE;

    return m_pObscuringCell->isForceExtraCells();
}

void KSpreadCell::obscure( KSpreadCell *_cell, int _col, int _row )
{
  m_pObscuringCell = _cell;
  m_iObscuringCellsColumn = _col;
  m_iObscuringCellsRow = _row;
}

void KSpreadCell::unobscure()
{
  m_pObscuringCell = 0L;
  m_bLayoutDirtyFlag= TRUE;
}

void KSpreadCell::clicked( KSpreadCanvas *_canvas )
{
  if ( m_style == KSpreadCell::ST_Normal )
    return;
  else if ( m_style == KSpreadCell::ST_Select )
  {
    // We do only show a menu if the user himself clicked
    // on the cell.
    if ( !_canvas )
      return;

    QPopupMenu *popup = new QPopupMenu;
    SelectPrivate *s = (SelectPrivate*)m_pPrivate;
    QObject::connect( popup, SIGNAL( activated( int ) ),
		      s, SLOT( slotItemSelected( int ) ) );
    int id = 0;
    const char *t;
    for( t = s->m_lstItems.first(); t != 0L; t = s->m_lstItems.next() )
      popup->insertItem( t, id++ );
    RowLayout *rl = m_pTable->rowLayout( row() );
    int tx = m_pTable->columnPos( column(), _canvas );
    int ty = m_pTable->rowPos( row(), _canvas );
    int h = rl->height( _canvas );
    if ( m_iExtraYCells )
      h = m_iExtraHeight;
    ty += h;

    QPoint p( tx, ty );
    QPoint p2 = _canvas->mapToGlobal( p );
    popup->popup( p2 );
    return;
  }

  if ( m_strAction.isEmpty() )
    return;

  KSContext context;
  QList<KSpreadDepend> lst;
  lst.setAutoDelete( TRUE );
  KSParseNode* code = m_pTable->doc()->interpreter()->parse( context, m_pTable, m_strAction, lst );
  // Did a syntax error occur ?
  if ( context.exception() )
  {
    QString tmp("Error in cell %1\n\n");
    tmp = tmp.arg( util_cellName( m_pTable, m_iColumn, m_iRow ) );
    tmp += context.exception()->toString( context );
    QMessageBox::critical( 0, i18n("KSpread error"), tmp, i18n("OK"));
    return;
  }

  KSContext& context2 = m_pTable->doc()->context();
  if ( !m_pTable->doc()->interpreter()->evaluate( context2, code, m_pTable ) )
      // Print out exception if any
      if ( context2.exception() )
      {
	  QString tmp("Error in cell %1\n\n");
	  tmp = tmp.arg( util_cellName( m_pTable, m_iColumn, m_iRow ) );
	  tmp += context2.exception()->toString( context2 );
	  QMessageBox::critical( 0, i18n("KSpread error"), tmp, i18n("OK"));
      }
}

QString KSpreadCell::encodeFormular( int _col, int _row )
{
    if ( _col == -1 )
	_col = m_iColumn;
    if ( _row == -1 )
	_row = m_iRow;

    QString erg = "";

    const char *p = m_strText.data();
    if ( p == 0L )
	return QString();

    char buf[ 2 ];
    buf[ 1 ] = 0;

    bool fix1 = FALSE;
    bool fix2 = FALSE;

    while ( *p != 0 )
    {
	/*if ( *p == '\"' )
	{
	    while ( *p && *p != '\"' )
	    {
		buf[ 0 ] = *p++;
		erg += buf;
		if ( *p == '\\' )
		{
		    buf[ 0 ] = *p++;
		    erg += buf;
		}
	    }
	    p++;
	}*/
	/*else*/ if ( *p != '$' && !isalpha( *p ) )
	{
	    buf[0] = *p++;
	    erg += buf;
	    fix1 = fix2 = FALSE;
	}
	else
	{
	    QString tmp = "";
	    if ( *p == '$' )
	    {
		tmp = "$";
		p++;
		fix1 = TRUE;
	    }
	    if ( isalpha( *p ) )
	    {
		char buffer[ 1024 ];
		char *p2 = buffer;
		while ( *p && isalpha( *p ) )
		{
		    buf[ 0 ] = *p;
		    tmp += buf;
		    *p2++ = *p++;
		}
		*p2 = 0;
		if ( *p == '$' )
		{
		    tmp += "$";
		    p++;
		    fix2 = TRUE;
		}
		if ( isdigit( *p ) )
		{
		    const char *p3 = p;
		    int row = atoi( p );
		    while ( *p != 0 && isdigit( *p ) ) p++;
		    // Is it a table
		    if ( *p == '!' )
		    {
			erg += tmp;
			fix1 = fix2 = FALSE;
			p = p3;
		    }
		    else // It must be a cell identifier
		    {
			int col = 0;
			if ( strlen( buffer ) >= 2 )
			{
			    col += 26 * ( buffer[0] - 'A' + 1 );
			    col += buffer[1] - 'A' + 1;
			}
			else
			    col += buffer[0] - 'A' + 1;
			if ( fix1 )
			    sprintf( buffer, "$%i", col );
			else
			    sprintf( buffer, "#%i", col - _col );
			erg += buffer;
			if ( fix2 )
			    sprintf( buffer, "$%i#", row );
			else
			    sprintf( buffer, "#%i#", row - _row );
			erg += buffer;		
		    }
		}
		else
		{
		    erg += tmp;
		    fix1 = fix2 = FALSE;
		}
	    }
	    else
	    {
		erg += tmp;
		fix1 = FALSE;
	    }
	}
    }

    return erg;
}

QString KSpreadCell::decodeFormular( const char* _text, int _col, int _row )
{
    if ( _col == -1 )
	_col = m_iColumn;
    if ( _row == -1 )
	_row = m_iRow;

    QString erg = "";
    const char *p = _text;

    if ( p == 0L )
	return QString();

    char buf[ 2 ];
    buf[ 1 ] = 0;

    while ( *p != 0 )
    {
	/*if ( *p == '\"' )
	{
	    while ( *p && *p != '\"' )
	    {
		buf[ 0 ] = *p++;
		erg += buf;
		if ( *p == '\\' )
		{
		    buf[ 0 ] = *p++;
		    erg += buf;
		}
	    }
	    p++;
	}*/
	if ( *p == '#' || *p == '$' )
	{
	    bool fix1 = FALSE;
	    bool fix2 = FALSE;
	    if ( *p++ == '$' )
		fix1 = TRUE;
	    int col = atoi( p );
	    if ( !fix1 )
		col += _col;
	    if ( *p == '-' ) p++;
	    while ( *p != 0 && isdigit( *p ) ) p++;
	    // Skip '#' or '$'
	    if ( *p++ == '$' )
		fix2 = TRUE;
	    int row = atoi( p );
	    if ( !fix2 )
		row += _row;
	    if ( *p == '-' ) p++;
	    while ( *p != 0 && isdigit( *p ) ) p++;
	    // Skip '#' or '$'
	    p++;
	    if ( row <= 0 || col <= 0 )
	    {
		printf("ERROR: out of range\n");
		return QString( _text );
	    }
	    if ( fix1 )
		erg += "$";
	    char buffer[ 20 ];
	    char *p2 = buffer;
	    if ( col > 26 )
		*p2++ = 'A' + ( col / 26 ) - 1;
	    *p2++ = 'A' + ( col % 26 ) - 1;
	    *p2 = 0;
	    erg += buffer;
	    if ( fix2 )
		erg += "$";
	    sprintf( buffer, "%i", row );
	    erg += buffer;
	}
	else
	{
	    buf[ 0 ] = *p++;
	    erg += buf;
	}
    }

    return erg;
}

void KSpreadCell::makeLayout( QPainter &_painter, int _col, int _row )
{
  m_leftBorderPen.setWidth( leftBorderWidth( _col, _row ) );
  m_topBorderPen.setWidth( topBorderWidth( _col, _row ) );
  m_fallDiagonalPen.setWidth( fallDiagonalWidth( _col, _row) );
  m_goUpDiagonalPen.setWidth( goUpDiagonalWidth( _col, _row) );
  /**
   * RichText
   */
  if ( m_pQML )
  {
    // Calculate how many cells we could use in addition right hand
    // Never use more then 10 cells.
    int right = 0;
    int max_width = width( _col );
    bool ende = false;
    int c;
    m_pQML->setWidth( &_painter, max_width );
    for( c = _col + 1; !ende && c <= _col + 10; ++c )
    {
      KSpreadCell *cell = m_pTable->cellAt( c, _row );
      if ( cell && !cell->isEmpty() )
	ende = true;
      else
      {
	ColumnLayout *cl = m_pTable->columnLayout( c );
	max_width += cl->width();

	// Can we make use of extra cells ?
	int h = m_pQML->height();
	m_pQML->setWidth( &_painter, max_width );
	if ( m_pQML->height() < h )
	  ++right;
	else
	{
	  max_width -= cl->width();
	  m_pQML->setWidth( &_painter, max_width );
	  ende = true;
	}
      }
    }

    // How may space do we need now ?
    // m_pQML->setWidth( &_painter, max_width );
    int h = m_pQML->height();
    int w = m_pQML->width();
    debug("QML w=%i max=%i",w,max_width);

    m_richWidth=w;
    m_richHeight=h;
    // Occupy the needed extra cells in horizontal direction
    max_width = width( _col );
    ende = ( max_width >= w );
    for( c = _col + 1; !ende && c <= _col + right; ++c )
    {
      KSpreadCell *cell = m_pTable->nonDefaultCell( c, _row );
      cell->obscure( this, _col, _row );
      ColumnLayout *cl = m_pTable->columnLayout( c );
      max_width += cl->width();
      if ( max_width >= w )
	ende = true;
    }
    m_iExtraXCells = c - _col - 1;
    m_iExtraWidth = ( m_iExtraXCells == 0 ? 0 : max_width );

    // Occupy the needed extra cells in vertical direction
    int max_height = height( 0 );
    int r = _row;
    ende = ( max_height >= h );
    for( r = _row + 1; !ende && r < _row + 500; ++r )
    {
      bool empty = true;
      for( c = _col; !empty && c <= _col + m_iExtraXCells; ++c )
      {
	KSpreadCell *cell = m_pTable->cellAt( c, r );
	if ( cell && !cell->isEmpty() )
	  empty = false;
      }
      if ( !empty )
	ende = true;
      else
      {
	// Occupy this row
	for( c = _col; c <= _col + m_iExtraXCells; ++c )
	{
	  KSpreadCell *cell = m_pTable->nonDefaultCell( c, r );
	  cell->obscure( this, _col, _row );
	}
	RowLayout *rl = m_pTable->rowLayout( r );
	max_height += rl->height();
	if ( max_height >= h )
	  ende = true;
      }
    }
    m_iExtraYCells = r - _row - 1;
    m_iExtraHeight = ( m_iExtraYCells == 0 ? 0 : max_height );

    m_bLayoutDirtyFlag = false;
    return;
  }
  /**
   * A visual formula
   */
  else if ( m_pVisualFormula )
  {
    // Calculate how many cells we could use in addition right hand
    // Never use more then 10 cells.
    int right = 0;
    int max_width = width( _col );
    bool ende = false;
    int c;

    for( c = _col + 1; !ende && c <= _col + 10; ++c )
    {
      KSpreadCell *cell = m_pTable->cellAt( c, _row );
      if ( cell && !cell->isEmpty() )
	ende = true;
      else
      {
	ColumnLayout *cl = m_pTable->columnLayout( c );
	max_width += cl->width();
	++right;
      }
    }

    // How may space do we need now ?
    // TODO: We have to initialize sizes here ....
    _painter.save();
    _painter.setPen( m_textPen.color() );
    _painter.setFont( m_textFont );
    m_pVisualFormula->setPos( -1000, -1000 );
    m_pVisualFormula->redraw( _painter );
    _painter.restore();
    QSize size = m_pVisualFormula->size();
    int h = size.height();
    int w = size.width();
    printf("Formula w=%i h=%i\n",w,h);
    m_richWidth=w;
    m_richHeight=h;
    // Occupy the needed extra cells in horizontal direction
    max_width = width( _col );
    ende = ( max_width >= w );
    for( c = _col + 1; !ende && c <= _col + right; ++c )
    {
      KSpreadCell *cell = m_pTable->nonDefaultCell( c, _row );
      cell->obscure( this, _col, _row );
      ColumnLayout *cl = m_pTable->columnLayout( c );
      max_width += cl->width();
      if ( max_width >= w )
	ende = true;
    }
    m_iExtraXCells = c - _col - 1;
    m_iExtraWidth = ( m_iExtraXCells == 0 ? 0 : max_width );

    // Occupy the needed extra cells in vertical direction
    int max_height = height( 0 );
    int r = _row;
    ende = ( max_height >= h );
    for( r = _row + 1; !ende && r < _row + 500; ++r )
    {
      bool empty = true;
      for( c = _col; !empty && c <= _col + m_iExtraXCells; ++c )
      {
	KSpreadCell *cell = m_pTable->cellAt( c, r );
	if ( cell && !cell->isEmpty() )
	  empty = false;
      }
      if ( !empty )
	ende = true;
      else
      {
	// Occupy this row
	for( c = _col; c <= _col + m_iExtraXCells; ++c )
	{
	  KSpreadCell *cell = m_pTable->nonDefaultCell( c, r );
	  cell->obscure( this, _col, _row );
	}
	RowLayout *rl = m_pTable->rowLayout( r );
	max_height += rl->height();
	if ( max_height >= h )
	  ende = true;
      }
    }
    m_iExtraYCells = r - _row - 1;
    m_iExtraHeight = ( m_iExtraYCells == 0 ? 0 : max_height );

    m_bLayoutDirtyFlag = false;
    return;
  }

  /**
   * A usual numeric, boolean or string value.
   */
  const char *ptext;
  // If this is a select box, find out about the selected item
  // in the KSpreadPrivate data struct
  if ( m_style == ST_Select )
  {
    SelectPrivate *s = (SelectPrivate*)m_pPrivate;
    ptext = s->text();
  }
  else if ( isFormular() )
    ptext = m_strFormularOut;
  else
    ptext = m_strText;

  if ( ptext == 0L || *ptext == 0 )
  {
    m_strOutText = QString::null;
    if ( isDefault() )
      return;
  }

  if ( m_bBool )
  {
    m_textPen.setColor( m_textColor );
    if ( m_dValue == 0 )
      m_strOutText = "False";
    else
      m_strOutText = "True";
  }
  else if ( m_bValue )
  {
    // First get some locale information
    if (!decimal_point)
    { // (decimal_point is static)
      decimal_point = KGlobal::locale()->decimalSymbol()[0].latin1();
      // Hmm we should use QChar here and QString a lot more around
      // here... (David)
      qDebug( "decimal_point is '%c'", decimal_point );
      // HACK
      if ( decimal_point == 0 )
	  decimal_point = '.';
    }

    QString f2;


    double v = m_dValue * m_dFaktor;
	
    if ( floatFormat() == KSpreadCell::AlwaysUnsigned && v < 0.0)
      v *= -1.0;

    // if precision is -1, ask for a huge number of decimals, we'll remove
    // the zeros later. Is 8 ok ?
    int p = (m_iPrecision == -1) ? 8 : m_iPrecision;
    QString localizedNumber = KGlobal::locale()->formatNumber(v, p);
    qDebug("LOCALIZED NUMBER is %s", localizedNumber.latin1() );

    // Remove trailing zeros and the decimal point if necessary
    // unless the number has no decimal point
    if ( m_iPrecision == -1 && localizedNumber.find(decimal_point) >= 0 )
    {
      int i = localizedNumber.length();
      bool bend = FALSE;
      while ( !bend && i > 0 )
      {
	if ( localizedNumber[ i - 1 ] == '0' )
	  localizedNumber.truncate( --i );
	else if ( localizedNumber[ i - 1 ] == decimal_point )
	{
	  bend = TRUE;
	  localizedNumber.truncate( --i );
	}
	else
	  bend = TRUE;
      }
    }

    m_strOutText = "";
    if ( prefix() != 0L )
      m_strOutText += prefix();	
    m_strOutText += localizedNumber;
    if ( postfix() != 0L )
      m_strOutText += postfix();

    if ( floatColor() == KSpreadCell::NegRed && v < 0.0 )
      m_textPen.setColor( Qt::red );
    else
      m_textPen.setColor( m_textColor );
  }
  else
  {
    m_textPen.setColor( m_textColor );
    m_strOutText = ptext;
  }

  _painter.setPen( m_textPen );
  _painter.setFont( m_textFont );
  QFontMetrics fm = _painter.fontMetrics();
  m_iOutTextWidth = fm.width( m_strOutText );
  m_iOutTextHeight = fm.ascent() + fm.descent();

  // Calculate the size of the cell
  RowLayout *rl = m_pTable->rowLayout( m_iRow );
  ColumnLayout *cl = m_pTable->columnLayout( m_iColumn );

  int w = cl->width();
  int h = rl->height();

  // Calculate the extraWidth and extraHeight if we are forced to.
  if ( m_bForceExtraCells )
  {
    for ( int x = _col + 1; x <= _col + m_iExtraXCells; x++ )
    {
      ColumnLayout *cl = m_pTable->columnLayout( x );
      w += cl->width() - 1;
    }

    for ( int y = _row + 1; y <= _row + m_iExtraYCells; y++ )
    {
      RowLayout *rl = m_pTable->rowLayout( y );
      h += rl->height() - 1;
    }
  }
  m_iExtraWidth = w;
  m_iExtraHeight = h;

  if ( m_style == ST_Select )
    w -= 16;

  int a = m_eAlign;
  if ( a == KSpreadCell::Undefined )
  {
    if ( m_bValue )
      a = KSpreadCell::Right;
    else
      a = KSpreadCell::Left;
  }

  // Offset for alignment
  switch( a )
  {
  case KSpreadCell::Left:
    m_iTextX = leftBorderWidth( _col, _row) + BORDER_SPACE;
    break;
  case KSpreadCell::Right:
    m_iTextX = w - BORDER_SPACE - m_iOutTextWidth - rightBorderWidth( _col, _row );
    break;
  case KSpreadCell::Center:
    m_iTextX = ( w - m_iOutTextWidth ) / 2;
    break;
  }

  // Free all obscured cells if we are not forced to abscure them
  if ( !m_bForceExtraCells )
  {
    for ( int x = m_iColumn; x <= m_iColumn + m_iExtraXCells; ++x )
    {
	for ( int y = m_iRow; y <= m_iRow + m_iExtraYCells; ++y )
        {
	    if ( x != m_iColumn || y != m_iRow )
	    {
		KSpreadCell *cell = m_pTable->cellAt( x, y );
		cell->unobscure();
	    }
	}
    }

    m_iExtraXCells = 0;
    m_iExtraYCells = 0;
  }

  // Do we need to break the line into multiple lines and are we allowed to
  // do so?
  if ( m_iOutTextWidth > w - 2 * BORDER_SPACE - leftBorderWidth( _col, _row) -
       rightBorderWidth( _col, _row ) && m_bMultiRow )
  {
    int lines = 1;
    // copy of m_strOutText
    QString o = m_strOutText;
    // The stop character
    o += "\n";
    // current word
    QString ws = "";
    m_strOutText = "";
    const char *p = o;
    const char *start = o;
    // Length of the current word
    int wl = 0;
    // Length of the current line
    int l = 0;
    int spacew = fm.width( ' ' );
    // Loop over all chars
    while ( *p )
    {
      // Separator ?
      if ( *p == ' ' || *p == '\n' )
      {
	// Do we have to print a space or are we
	// at the beginning of the line
	int t = wl;
	if ( start != o.data() )
	  wl += spacew;

	// Does the new word fit in this line ?
	if ( t > 0 && l + wl <= w - 2 * BORDER_SPACE - leftBorderWidth( _col, _row) -
	     rightBorderWidth( _col, _row ) )
	{
	  if ( start != o.data() )
	    m_strOutText += " ";
	  m_strOutText += ws;
	  start = p + 1;
	  l += t;
	  wl = 0;
	}
	else // The word does not fit in the line
	{
	  // Did we print anything yet ?
	  if ( start != o.data() )
	  {
	    // We printed something yet => start a new line
	    lines++;
	    // New line
	    m_strOutText += "\n";
	  }

	  // Add the current word
	  m_strOutText += ws;
	  // Skip this word and the separator
	  start = p + 1;
	  l = wl;
	  wl = 0;
	}
	ws = "";
      }
      // A usual character
      else if ( *p != 0 )
      {
	// Add the character to the current line
	char buf[2];
	buf[0] = *p;
	buf[1] = 0;
	ws += buf;
	wl += fm.width( *p );
      }
      p++;
    }

    m_iOutTextHeight *= lines;
    m_iTextX = 0;
    // Calculate the maximum width
    QString t;
    int i;
    int pos = 0;
    m_iOutTextWidth = 0;
    do
    {
      i = m_strOutText.find( "\n", pos );
      if ( i == -1 )
	t = m_strOutText.mid( pos, m_strOutText.length() - pos );
      else
      {
	t = m_strOutText.mid( pos, i - pos );
	pos = i + 1;
      }
      int tw = fm.width( t );
      if ( tw > m_iOutTextWidth )
	m_iOutTextWidth = tw;
    }
    while ( i != -1 );
  }

  switch( m_eAlignY )
  {
  case KSpreadCell::Top:
    m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE+ fm.ascent();
    break;
  case KSpreadCell::Bottom:
    m_iTextY = h - BORDER_SPACE - bottomBorderWidth( _col, _row );
    break;
  case KSpreadCell::Middle:
    m_iTextY = ( h - m_iOutTextHeight ) / 2 + fm.ascent();
    break;
  }
  // Vertical alignment
  //m_iTextY = ( h - m_iOutTextHeight ) / 2 + fm.ascent();
  m_fmAscent=fm.ascent();

  // Do we have to occupy additional cells right hand ?
  if ( m_iOutTextWidth > w - 2 * BORDER_SPACE - leftBorderWidth( _col, _row) -
       rightBorderWidth( _col, _row ) )
  {
    // No chance. We can not obscure more/less cells.
    if ( m_bForceExtraCells )
    {
	// The text does not fit in the cell
	m_strOutText = "**";
    }
    else
    {
      int c = m_iColumn;
      int end = 0;
      // Find free cells right hand to this one
      while ( !end )
      {
	ColumnLayout *cl2 = m_pTable->columnLayout( c + 1 );
	KSpreadCell *cell = m_pTable->cellAt( c + 1, m_iRow );
	if ( cell->isEmpty() )
	{
	  w += cl2->width() - 1;
	  c++;

	  // Enough space ?
	  if ( m_iOutTextWidth <= w - 2 * BORDER_SPACE - leftBorderWidth( _col, _row) -
	       rightBorderWidth( _col, _row ) )
	    end = 1;
	}
	// Not enough space, but the next cell is not empty
	else
	  end = -1;
      }

      // Dont occupy additional space for right aligned or centered text or values.
      if ( /* end == 1 && !isFormular() && */ ( m_eAlign == KSpreadCell::Left || m_eAlign == KSpreadCell::Undefined ) )
      {
	m_iExtraWidth = w;
	for( int i = m_iColumn + 1; i <= c; ++i )
	{
	  KSpreadCell *cell = m_pTable->nonDefaultCell( i, m_iRow );
	  cell->obscure( this, m_iColumn, m_iRow );
	}
	m_iExtraXCells = c - m_iColumn + 1;
      }
      else
      {
	m_strOutText = "**";
      }
    }
  }

    /* for ( int x = 0; x <= m_iExtraXCells; x++ )
	for ( int y = 0; y <= m_iExtraYCells; y++ )
	    if ( x != 0 || y != 0 )
	    {
		KSpreadCell *cell = m_pTable->nonDefaultCell( column + x, row + y );
		cell->setKSpreadLayoutDirtyFlag();
		cell->setDisplayDirtyFlag();
	    }  */

  m_bLayoutDirtyFlag = FALSE;
}
void KSpreadCell::offsetAlign(int _col,int _row)
{
int a = m_eAlign;
RowLayout *rl = m_pTable->rowLayout( _row );
ColumnLayout *cl = m_pTable->columnLayout( _col );

int w = cl->width();
int h = rl->height();
switch( m_eAlignY )
  {
  case KSpreadCell::Top:
    m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +m_fmAscent;
    break;
  case KSpreadCell::Bottom:
    m_iTextY = h - BORDER_SPACE - bottomBorderWidth( _col, _row );
    break;
  case KSpreadCell::Middle:
    m_iTextY = ( h - m_iOutTextHeight ) / 2 +m_fmAscent;
    break;
  }
if ( a == KSpreadCell::Undefined )
  {
    if ( m_bValue )
      a = KSpreadCell::Right;
    else
      a = KSpreadCell::Left;
  }

switch( a )
  {
  case KSpreadCell::Left:
    m_iTextX = leftBorderWidth( _col, _row) + BORDER_SPACE;
    break;
  case KSpreadCell::Right:
    m_iTextX = w - BORDER_SPACE - m_iOutTextWidth - rightBorderWidth( _col, _row );
    break;
  case KSpreadCell::Center:
    m_iTextX = ( w - m_iOutTextWidth ) / 2;
    break;
  }
}
/*
bool KSpreadCell::makeDepend( const char *_p, KSpreadDepend ** _dep, bool _second )
{
    KSpreadTable *dest_table;

    // Test wethe we have a table name
    int pos = 0;
    while ( isalpha( _p[pos] ) || isdigit( _p[pos] ) )
	pos++;

    if ( _p[pos] == '!' )
    {
      QString n = _p;
      n = n.left( pos );

      _p += pos + 1;
      KSpreadTable *t = m_pTable->map()->findTable( n );
      if ( t == 0L )
      {
	printf("ERROR: Unknown table name '%s'\n",n.data() );
	return FALSE;
      }

      if ( _second )
      {
	printf("ERROR: KSpreadTable name not allowed in 2nd corner of a A1:D5 construct.\n");
	return FALSE;
      }

      dest_table = t;
    }
    else
      dest_table = m_pTable;

    if ( *_p == '$' )
      _p++;

    pos = 0;
    int col = 0;
    while ( isupper( _p[ pos ] ) )
    {
	if ( pos >= 2 )
	    return FALSE;
	else if ( pos == 0 )
	    col = _p[pos] - 'A' + 1;
	else if ( pos == 1 )
	    col += ( _p[pos] - 'A' + 1 ) * 26;
	pos++;
    }
    if ( pos == 0 )
	return FALSE;

    _p += pos;

    if ( *_p == '$' )
	_p++;

    pos = 0;
    int row = atoi( _p );
    while ( isdigit( _p[ pos ] ) )
    {
	if ( pos >= 5 )
	    return FALSE;
	pos++;
    }
    if ( pos == 0 )
	return FALSE;

    _p += pos;

    if ( isalpha( *_p ) || *_p == '$' || *_p == '!' )
	return FALSE;

    KSpreadDepend *dep;

    if ( _second )
    {
	dep = m_lstDepends.last();
	if ( dep == 0L )
	    return FALSE;
    }
    else
    {
	dep = new KSpreadDepend();
	m_lstDepends.append( dep );
	dep->m_iColumn2 = -1;
	dep->m_pTable = dest_table;
    }
    *_dep = dep;

    if ( _second )
	dep->m_iColumn2 = col;
    else
	dep->m_iColumn = col;
    if ( _second )
	dep->m_iRow2 = row;
    else
	dep->m_iRow = row;
	
    return TRUE;
}
*/

bool KSpreadCell::makeFormular()
{
  // m_strFormular = m_strText;
  // ::makeDepend( m_strText.data() + 1, m_pTable, &m_lstDepends );

  if ( m_pCode )
    delete m_pCode;

  KSContext context;

  // We have to transform the numerical values back into a non-localized form,
  // so that they can be parsed by kscript (David)
  // To be moved to a separate function when it is properly implemented...
  // or should we use strtod on each number found ? Impossible since kscript
  // would have to parse the localized version...
  // HACK (only handles decimal point)
  // ############# Torben: Incredible HACK. Separating parameters in a function call
  // will be horribly broken since "," -> "." :-((
  // ############# Torben: Do not replace stuff in strings.
  QString sDelocalizedText ( m_strText );
  int pos;
  while ( ( pos = sDelocalizedText.find( decimal_point, pos ) ) >= 0 )
    sDelocalizedText.replace( pos++, 1, "." );
  // At least,  =2,5+3,2  is turned into =2.5+3.2, which can get parsed...

  m_pCode = m_pTable->doc()->interpreter()->parse( context, m_pTable, sDelocalizedText, m_lstDepends );
  // Did a syntax error occur ?
  if ( context.exception() )
  {
    clearFormular();

    m_bError = true;
    m_strFormularOut = "####";
    m_bBool = false;
    m_bValue = false;
    m_dValue = 0.0;
    m_bLayoutDirtyFlag = true;
    DO_UPDATE;
    QString tmp("Error in cell %1\n\n");
    tmp = tmp.arg( util_cellName( m_pTable, m_iColumn, m_iRow ) );
    tmp += context.exception()->toString( context );
    QMessageBox::critical( 0, i18n("KSpread error"), tmp, i18n("OK"));
    return false;
  }

  return true;
}

void KSpreadCell::clearFormular()
{
  m_lstDepends.clear();
  if ( m_pCode )
  {
    delete m_pCode;
    m_pCode = 0;
  }
}

bool KSpreadCell::calc( bool _makedepend )
{
    qDebug("CALC r=%i c=%i", m_iRow, m_iColumn );
  if ( m_bProgressFlag )
  {
    printf("ERROR: Circle\n");
    m_bError = true;
    m_bValue = false;
    m_bBool = false;
    m_strFormularOut = "####";
    m_bLayoutDirtyFlag = true;
    if ( m_style == ST_Select )
    {
	SelectPrivate *s = (SelectPrivate*)m_pPrivate;
	s->parse( m_strFormularOut );
    }
    DO_UPDATE;
    return false;
  }

  if ( !isFormular() )
    return true;

  if ( !m_bCalcDirtyFlag )
    return true;

  m_bLayoutDirtyFlag= true;
  m_bProgressFlag = true;
  m_bCalcDirtyFlag = false;
  if ( _makedepend )
  {
    KSpreadDepend *dep;
    for ( dep = m_lstDepends.first(); dep != 0L; dep = m_lstDepends.next() )
    {
      bool ok;

      if ( dep->m_iColumn2 != -1 )
      {
	int left = dep->m_iColumn < dep->m_iColumn2 ? dep->m_iColumn : dep->m_iColumn2;
	int right = dep->m_iColumn > dep->m_iColumn2 ? dep->m_iColumn : dep->m_iColumn2;
	int top = dep->m_iRow < dep->m_iRow2 ? dep->m_iRow : dep->m_iRow2;
	int bottom = dep->m_iRow > dep->m_iRow2 ? dep->m_iRow : dep->m_iRow2;
	for ( int x = left; x <= right; x++ )
	  for ( int y = top; y <= bottom; y++ )
	  {
	    KSpreadCell *cell = dep->m_pTable->cellAt( x, y );
	    if ( cell == 0L )
	      return false;
	    ok = cell->calc( _makedepend );
	    if ( !ok )
	    {
	      m_strFormularOut = "####";
	      m_bValue = false;
	      m_bBool = false;
	      m_bProgressFlag = false;
	      if ( m_style == ST_Select )
	      {
		  SelectPrivate *s = (SelectPrivate*)m_pPrivate;
		  s->parse( m_strFormularOut );
	      }
	      // m_bLayoutDirtyFlag = true;
	      DO_UPDATE;
	      return false;
	    }
	  }
      }
      else
      {
	KSpreadCell *cell = dep->m_pTable->cellAt( dep->m_iColumn, dep->m_iRow );
	if ( cell == 0L )
	  return false;
	ok = cell->calc( _makedepend );
	if ( !ok )
	{
	  m_bError = true;
	  m_strFormularOut = "####";
	  m_bValue = false;
	  m_bBool = false;
	  m_bProgressFlag = false;
	  // m_bLayoutDirtyFlag = true;
	  if ( m_style == ST_Select )
          {
	      SelectPrivate *s = (SelectPrivate*)m_pPrivate;
	      s->parse( m_strFormularOut );
	  }
	  DO_UPDATE;
	  return false;
	}
      }
    }
  }

  KSContext& context = m_pTable->doc()->context();
  if ( !m_pCode || !m_pTable->doc()->interpreter()->evaluate( context, m_pCode, m_pTable ) )
  {
    // If we got an error during evaluation ...
    if ( m_pCode )
    {
      // Print out exception if any
      if ( context.exception() )
      {
	QString tmp("Error in cell %1\n\n");
	tmp = tmp.arg( util_cellName( m_pTable, m_iColumn, m_iRow ) );
	tmp += context.exception()->toString( context );
	QMessageBox::critical( 0, i18n("KSpread error"), tmp, i18n("OK"));
      }
      m_bError = true;
      m_strFormularOut = "####";
      m_bValue = false;
      m_bBool = false;
    }
    // m_bLayoutDirtyFlag = true;
    m_bProgressFlag = false;

    if ( m_style == ST_Select )
    {
	SelectPrivate *s = (SelectPrivate*)m_pPrivate;
	s->parse( m_strFormularOut );
    }
    DO_UPDATE;
    return false;
  }
  else if ( context.value()->type() == KSValue::DoubleType )
  {
    m_dValue = context.value()->doubleValue();
    m_bValue = true;
    m_bBool = false;
    // m_strFormularOut.sprintf( "%f", m_dValue );
    m_strFormularOut = KGlobal::locale()->formatNumber( m_dValue );
  }
  else if ( context.value()->type() == KSValue::IntType )
  {
    m_dValue = (double)context.value()->intValue();
    m_bValue = true;
    m_bBool = false;
    // m_strFormularOut.sprintf( "%f", m_dValue );
    m_strFormularOut = KGlobal::locale()->formatNumber( m_dValue );
  }
  else if ( context.value()->type() == KSValue::BoolType )
  {
    m_bValue = false;
    m_bBool = true;
    m_dValue = context.value()->boolValue() ? 1.0 : 0.0;
    m_strFormularOut = context.value()->boolValue() ? "True" : "False";
  }
  else
  {
    m_bValue = false;
    m_bBool = false;
    m_strFormularOut = context.value()->toString( context );
  }

  if ( m_style == ST_Select )
  {
      SelectPrivate *s = (SelectPrivate*)m_pPrivate;
      s->parse( m_strFormularOut );
  }

  m_bLayoutDirtyFlag = true;
  m_bProgressFlag = false;

  DO_UPDATE;

  return true;
}

QString KSpreadCell::valueString() const
{
  if ( m_style == ST_Select )
    return ((SelectPrivate*)m_pPrivate)->text();

  if ( isFormular() )
    return m_strFormularOut;

  return m_strText;
}

void KSpreadCell::paintEvent( KSpreadCanvas *_canvas, const QRect& _rect, QPainter &_painter,
			      int _col, int _row, QRect *_prect )
{
  RowLayout *rl = m_pTable->rowLayout( _row );
  ColumnLayout *cl = m_pTable->columnLayout( _col );
  int tx = m_pTable->columnPos( _col, _canvas );
  int ty = m_pTable->rowPos( _row, _canvas );

  paintEvent( _canvas, _rect, _painter, tx, ty, _col, _row, cl, rl, _prect );
}

void KSpreadCell::paintEvent( KSpreadCanvas *_canvas, const QRect& _rect, QPainter &_painter,
			      int _tx, int _ty,
			      int _col, int _row, ColumnLayout *cl, RowLayout *rl, QRect *_prect )
{
  bool selected = m_pTable->selectionRect().contains( QPoint( _col, _row ) );

  if ( m_pObscuringCell )
  {
    _painter.save();
    // QRect rect( 0, 0, _canvas->width(), _canvas->height() );
    // QRect r;
    m_pObscuringCell->paintEvent( _canvas, _rect, _painter,
				  m_iObscuringCellsColumn, m_iObscuringCellsRow, _prect );
    _painter.restore();
    m_bLayoutDirtyFlag = FALSE;
    return;
  }

  if ( m_bCalcDirtyFlag )
    calc();

  bool old_layoutflag = m_bLayoutDirtyFlag;
  if ( m_bLayoutDirtyFlag)
    makeLayout( _painter, _col, _row );

  int w = cl->width( _canvas );
  int h = rl->height( _canvas );
  if ( m_iExtraXCells )
    w = m_iExtraWidth;
  if ( m_iExtraYCells )
    h = m_iExtraHeight;

  // Do we really need to display this cell ?
  QRect r2( _tx, _ty, w, h );
  if ( !r2.intersects( _rect ) )
    return;

  if ( _prect )
    _prect->setRect( _tx, _ty, w, h );

  if ( selected )
    _painter.setBackgroundColor( Qt::black );
  else
    _painter.setBackgroundColor( m_bgColor );
  _painter.eraseRect( _tx, _ty, w, h );

  // Draw the border
  if ( m_leftBorderPen.style() == Qt::NoPen )
    _painter.setPen( table()->doc()->defaultGridPen() );
  else
    _painter.setPen( m_leftBorderPen );
  // Fix a 'bug' in the pens width setting. We still need the upper left corner
  // of the line but a width > 1 won't work for us.
  int dx = (int)ceil( (double)( m_leftBorderPen.width() - 1) / 2.0 );
  int dy = (int)ceil( (double)( m_topBorderPen.width() - 1) / 2.0 );
  _painter.drawLine( _tx + dx, _ty, _tx + dx, _ty + h );
  if ( m_topBorderPen.style() == Qt::NoPen )
    _painter.setPen( table()->doc()->defaultGridPen() );
  else
    _painter.setPen( m_topBorderPen );
  _painter.drawLine( _tx, _ty + dy, _tx + w, _ty + dy );

  if ( m_fallDiagonalPen.style() != Qt::NoPen )
    {
      _painter.setPen( m_fallDiagonalPen );
      _painter.drawLine( _tx, _ty + dy, _tx + dx + w, _ty + dy + h );
    }
 if ( m_goUpDiagonalPen.style() != Qt::NoPen )
    {
      _painter.setPen( m_goUpDiagonalPen );
      _painter.drawLine( _tx, _ty +h , _tx + w, _ty + dy  );
    }

  static QColorGroup g( Qt::black, Qt::white, Qt::white, Qt::darkGray, Qt::lightGray, Qt::black, Qt::black );
  static QBrush fill( Qt::lightGray );
  /**
   * Modification for drawing the button
   */
  if ( m_style == KSpreadCell::ST_Button )
  {
      QApplication::style().drawButton( &_painter, _tx + dx + 1, _ty + dy + 1, w - 2*dx - 1, h - 2*dy - 1, g, selected, &fill );
      // qDrawShadePanel( &_painter, _tx + dx + 1, _ty + dy + 1, w - 2*dx - 1, h - 2*dy - 1, g, selected, 1, &fill );
  }
  /**
   * Modification for drawing the combo box
   */
  else if ( m_style == KSpreadCell::ST_Select )
  {
      /* qDrawShadePanel( &_painter, _tx + w - dx - 16, _ty + dy + 1, 16, h - 2*dy - 1, g, selected, 1, &fill );
    QPointArray a;
    // qDrawArrow( &_painter, DownArrow, WindowsStyle, selected, _tx + w - dx - 16, _ty + dy + 1, 16, h - 2*dy - 1, g );
    int aw = 16;
    int ah = h - 2*dy - 1;
    int ax = _tx + w - dx - 16;
    int ay = _ty + dy - 1;
    a.setPoints( 7, -3,-1, 3,-1, -2,0, 2,0, -1,1, 1,1, 0,2 );
    a.translate( ax+aw/2, ay+ah/2 );
    _painter.setPen( Qt::black );
    _painter.drawLineSegments( a, 0, 3 );
    _painter.drawPoint( a[6] ); */
      QApplication::style().drawComboButton(  &_painter, _tx + dx + 1, _ty + dy + 1, w - 2*dx - 1, h - 2*dy - 1, g, selected, TRUE/*, &fill*/ );
  }

  /**
   * QML ?
   */
  if ( m_pQML )
  {
    _painter.save();
    m_pQML->draw( &_painter, _tx, _ty, QRegion( QRect( _tx, _ty, w, h ) ), g, 0 );
    _painter.restore();
  }
  /**
   * Visual Formula ?
   */
  else if ( m_pVisualFormula )
  {
    _painter.save();
    _painter.setPen( m_textPen.color() );
    _painter.setFont( m_textFont );
    // TODO: No better method to set new font ?
    if ( old_layoutflag )
      m_pVisualFormula->parse( m_strText.mid(1) );
    m_pVisualFormula->setPos( _tx + w/2, _ty + h/2 );
    m_pVisualFormula->redraw( _painter );
    _painter.restore();
  }
  /**
   * Usual Text
   */
  else if ( !m_strOutText.isEmpty() )
  {
    if ( selected )
    {
      QPen p( m_textPen );
      p.setColor( Qt::white );
      _painter.setPen( p );
    }
    else
      _painter.setPen( m_textPen );
    _painter.setFont( m_textFont );
    if ( !m_bMultiRow )
      _painter.drawText( _tx + m_iTextX, _ty + m_iTextY, m_strOutText );
    else
    {
      QString t;
      int i;
      int pos = 0;
      int dy = 0;
      int dx = 0;
      QFontMetrics fm = _painter.fontMetrics();
      do
      {
	i = m_strOutText.find( "\n", pos );
	if ( i == -1 )
	  t = m_strOutText.mid( pos, m_strOutText.length() - pos );
	else
	{
	  t = m_strOutText.mid( pos, i - pos );
	  pos = i + 1;
	}
	
	int a = m_eAlign;
	if ( a == KSpreadCell::Undefined )
	{
	  if ( m_bValue )
	    a = KSpreadCell::Right;
	  else
	    a = KSpreadCell::Left;
	}
	
	switch( a )
	{
	case KSpreadCell::Left:
	  m_iTextX = leftBorderWidth( _col, _row) + BORDER_SPACE;
	  break;
	case KSpreadCell::Right:
	  m_iTextX = w - BORDER_SPACE - fm.width( t )
	    - rightBorderWidth( _col, _row );
	  break;
	case KSpreadCell::Center:
	  m_iTextX = ( w - fm.width( t ) ) / 2;
	}

	_painter.drawText( _tx + m_iTextX + dx, _ty + m_iTextY + dy, t );
	dy += fm.descent() + fm.ascent();
      }
      while ( i != -1 );
    }
  }

  if ( m_pTable->isShowPageBorders() )
  {
    if ( m_pTable->isOnNewPageY( _row ) )
    {
      _painter.setPen( Qt::red );
      _painter.drawLine( _tx, _ty, _tx + w, _ty );
    }
    if ( m_pTable->isOnNewPageX( _col ) )
    {
      _painter.setPen( Qt::red );
      _painter.drawLine( _tx, _ty, _tx, _ty + h );
    }
  }
}

void KSpreadCell::print( QPainter &_painter, int _tx, int _ty, int _col, int _row,
			 ColumnLayout *cl, RowLayout *rl, bool _only_left,
			 bool _only_top, const QPen& _grid_pen )
{
  if ( m_bCalcDirtyFlag )
    calc();

  if ( m_bLayoutDirtyFlag)
    makeLayout( _painter, _col, _row );

  if ( !_only_left && !_only_top && m_bgColor != Qt::white )
  {
    _painter.setBackgroundColor( m_bgColor );
    _painter.eraseRect( _tx, _ty, cl->width(), rl->height() );
  }

//   if (m_bgColor != white)
//     {
//       _painter.setBackgroundColor( m_bgColor );
//       _painter.eraseRect( _tx, _ty, cl->width(), rl->height() );
//     }

  // Draw the border
  if ( !_only_top )
  {
    //_painter.setPen( leftBorderPen );
    // Fix a 'bug' in the pens width setting. We still need the upper left corner
    // of the line but a width > 1 won't work for us.
    QPen pen;
    pen.setColor( leftBorderColor( _col, _row) );
    if ( m_leftBorderPen.style() == Qt::NoPen )
      pen = _grid_pen;
    else
      pen = QPen( m_leftBorderPen );
    _painter.setPen( pen );
    int dx = 0;//int)ceil( (double)( m_leftBorderPen.width() - 1) / 2.0 );
    _painter.drawLine( _tx + dx, _ty, _tx + dx, _ty + rl->height() );
  }
  if ( !_only_left )
  {
    //_painter.setPen( topBorderPen );
    QPen pen;
    pen.setColor( topBorderColor( _col, _row ) );
    if ( m_topBorderPen.style() == Qt::NoPen )
      pen = _grid_pen;
    else
      pen = QPen( m_topBorderPen );
    _painter.setPen( pen );
    int dy = 0;//(int)ceil( (double)( m_topBorderPen.width() - 1) / 2.0 );
    _painter.drawLine( _tx, _ty + dy, _tx + cl->width() , _ty + dy );
  }
  if ( !_only_top && !_only_left )
    {
      int dy=0;
      int dx=0;
      if ( m_fallDiagonalPen.style() != Qt::NoPen )
	{
	  _painter.setPen( m_fallDiagonalPen );
	  _painter.drawLine( _tx + dx, _ty + dy, _tx + cl->width() , _ty + rl->height() );
	}
      if ( m_goUpDiagonalPen.style() != Qt::NoPen )
	{
	  _painter.setPen( m_goUpDiagonalPen );
	  _painter.drawLine( _tx , _ty +rl->height() , _tx + cl->width(), _ty  );
	}
    }
  if ( !_only_top && !_only_left )
    if ( !m_strOutText.isEmpty() )
    {
      _painter.setPen( m_textPen );
      _painter.setFont( m_textFont );
      _painter.drawText( _tx + m_iTextX, _ty + m_iTextY, m_strOutText );
    }
}

void KSpreadCell::setRightBorderStyle( Qt::PenStyle s )
{
    KSpreadCell * cell = m_pTable->nonDefaultCell( column() + 1, row() );
    cell->setLeftBorderStyle( s );
}

void KSpreadCell::setBottomBorderStyle( Qt::PenStyle s )
{
    KSpreadCell * cell = m_pTable->nonDefaultCell( column(), row() + 1 );
    cell->setTopBorderStyle( s );
}

void KSpreadCell::setRightBorderColor( const QColor & _c )
{
    KSpreadCell * cell = m_pTable->nonDefaultCell( column() + 1, row() );
    cell->setLeftBorderColor( _c );
}

void KSpreadCell::setBottomBorderColor( const QColor & _c )
{
    KSpreadCell * cell = m_pTable->nonDefaultCell( column(), row() + 1 );
    cell->setTopBorderColor( _c );
}

void KSpreadCell::setRightBorderWidth( int _w )
{
    KSpreadCell * cell = m_pTable->nonDefaultCell( column() + 1, row() );
    cell->setLeftBorderWidth( _w );
}

void KSpreadCell::setBottomBorderWidth( int _w )
{
    KSpreadCell * cell = m_pTable->nonDefaultCell( column(), row() + 1 );
    cell->setTopBorderWidth( _w );
}

int KSpreadCell::width( int _col, KSpreadCanvas *_canvas )
{
  if ( _col < 0 )
    _col = m_iColumn;

  if ( _canvas )
  {
    if ( m_bForceExtraCells )
      return (int)( m_iExtraWidth );

    ColumnLayout *cl = m_pTable->columnLayout( _col );
    return cl->width( _canvas );
  }

  if ( m_bForceExtraCells )
    return m_iExtraWidth;

  ColumnLayout *cl = m_pTable->columnLayout( _col );
  return cl->width();
}

int KSpreadCell::height( int _row, KSpreadCanvas *_canvas )
{
  if ( _row < 0 )
    _row = m_iRow;

  if ( _canvas )
  {
    if ( m_bForceExtraCells )
      return (int)( m_iExtraHeight );

    RowLayout *rl = m_pTable->rowLayout( _row );
    return rl->height( _canvas );
  }

  if ( m_bForceExtraCells )
    return m_iExtraHeight;

  RowLayout *rl = m_pTable->rowLayout( _row );
  return rl->height();
}

const QColor& KSpreadCell::bgColor( int _col, int _row )
{
  if ( isDefault() )
  {
    RowLayout *rl = m_pTable->rowLayout( _row );
    ColumnLayout *cl = m_pTable->columnLayout( _col );

    if ( rl->time() > cl->time() )
      return rl->bgColor();
    else
      return cl->bgColor();
  }
  else
    return m_bgColor;
}

int KSpreadCell::leftBorderWidth( int _col, int _row, KSpreadCanvas *_canvas )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->leftBorderWidth( _canvas );
	else
	    return cl->leftBorderWidth( _canvas );
    }

    if ( _canvas )
	return (int) ( m_iLeftBorderWidth );
    else
	return m_iLeftBorderWidth;
}

int KSpreadCell::topBorderWidth( int _col, int _row, KSpreadCanvas *_canvas )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->topBorderWidth( _canvas );
	else
	    return cl->topBorderWidth( _canvas );
    }

    if ( _canvas )
	return (int) ( m_iTopBorderWidth );
    else
	return m_iTopBorderWidth;
}

int KSpreadCell::rightBorderWidth( int _col, int _row, KSpreadCanvas *_canvas )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col + 1 );
	
	if ( rl->time() > cl->time() )
	    return rl->leftBorderWidth( _canvas );
	else
	    return cl->leftBorderWidth( _canvas );
    }

    KSpreadCell * cell = m_pTable->cellAt( column() + 1, row() );
    return cell->leftBorderWidth( column() + 1, row(), _canvas );
}

int KSpreadCell::bottomBorderWidth( int _col, int _row, KSpreadCanvas *_canvas )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row + 1 );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->topBorderWidth( _canvas );
	else
	    return cl->topBorderWidth( _canvas );
    }

    KSpreadCell * cell = m_pTable->cellAt( column(), row() + 1 );
    return cell->topBorderWidth( column() + 1, row(), _canvas );
}

int KSpreadCell::fallDiagonalWidth( int _col, int _row, KSpreadCanvas *_canvas )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->fallDiagonalWidth( _canvas );
	else
	    return cl->fallDiagonalWidth( _canvas );
    }

    if ( _canvas )
	return (int) ( m_iFallDiagonalWidth );
    else
	return m_iFallDiagonalWidth;
}

int KSpreadCell::goUpDiagonalWidth( int _col, int _row, KSpreadCanvas *_canvas )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->goUpDiagonalWidth( _canvas );
	else
	    return cl->goUpDiagonalWidth( _canvas );
    }

    if ( _canvas )
	return (int) ( m_iGoUpDiagonalWidth );
    else
	return m_iGoUpDiagonalWidth;
}

Qt::PenStyle KSpreadCell::leftBorderStyle( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->leftBorderStyle();
	else
	    return cl->leftBorderStyle();
    }

    return m_leftBorderPen.style();
}

Qt::PenStyle KSpreadCell::topBorderStyle( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->topBorderStyle();
	else
	    return cl->topBorderStyle();
    }

    return m_topBorderPen.style();
}


Qt::PenStyle KSpreadCell::rightBorderStyle( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col + 1 );
	
	if ( rl->time() > cl->time() )
	    return rl->leftBorderStyle();
	else
	    return cl->leftBorderStyle();
    }

    KSpreadCell * cell = m_pTable->cellAt( column() + 1, row() );
    return cell->leftBorderStyle( column() + 1, row() );
}

Qt::PenStyle KSpreadCell::bottomBorderStyle( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row + 1 );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->topBorderStyle();
	else
	    return cl->topBorderStyle();
    }

    KSpreadCell * cell = m_pTable->cellAt( column(), row() + 1 );
    return cell->topBorderStyle( column(), row() + 1 );
}

Qt::PenStyle KSpreadCell::fallDiagonalStyle( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->fallDiagonalStyle();
	else
	    return cl->fallDiagonalStyle();
    }

    return m_fallDiagonalPen.style();
}

Qt::PenStyle KSpreadCell::goUpDiagonalStyle( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->goUpDiagonalStyle();
	else
	    return cl->goUpDiagonalStyle();
    }

    return m_goUpDiagonalPen.style();
}

const QColor& KSpreadCell::leftBorderColor( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->leftBorderColor();
	else
	    return cl->leftBorderColor();
    }

    return m_leftBorderPen.color();
}

const QColor& KSpreadCell::topBorderColor( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->topBorderColor();
	else
	    return cl->topBorderColor();
    }

    return m_topBorderPen.color();
}

const QColor& KSpreadCell::rightBorderColor( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col + 1 );
	
	if ( rl->time() > cl->time() )
	    return rl->leftBorderColor();
	else
	    return cl->leftBorderColor();
    }

    KSpreadCell * cell = m_pTable->cellAt( column() + 1, row() );
    return cell->leftBorderColor( column() + 1, row() );
}

const QColor& KSpreadCell::bottomBorderColor( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row + 1 );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->topBorderColor();
	else
	    return cl->topBorderColor();
    }

    KSpreadCell * cell = m_pTable->cellAt( column(), row() + 1 );
    return cell->topBorderColor( column(), row() + 1 );
}

const QColor& KSpreadCell::fallDiagonalColor( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->fallDiagonalColor();
	else
	    return cl->fallDiagonalColor();
    }

    return m_fallDiagonalPen.color();
}

const QColor& KSpreadCell::goUpDiagonalColor( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->goUpDiagonalColor();
	else
	    return cl->goUpDiagonalColor();
    }

    return m_goUpDiagonalPen.color();
}


void KSpreadCell::incPrecision()
{
  if ( !isValue() )
    return;

  if ( m_iPrecision == -1 )
  {
    int pos = m_strOutText.find(decimal_point);
    if ( pos == -1 )
      m_iPrecision = 1;
    m_iPrecision = m_strOutText.length() - pos;
    if ( m_iPrecision < 0 )
      m_iPrecision = 0;
    m_bLayoutDirtyFlag = TRUE;
  }
  else if ( m_iPrecision < 10 )
  {
    m_iPrecision++;
    m_bLayoutDirtyFlag = TRUE;
  }
}

void KSpreadCell::decPrecision()
{
  if ( !isValue() )
    return;

  if ( m_iPrecision == -1 )
  {
    int pos = m_strOutText.find(decimal_point);
    if ( pos == -1 )
      return;
    m_iPrecision = m_strOutText.length() - pos - 2;
    if ( m_iPrecision < 0 )
      m_iPrecision = 0;
    m_bLayoutDirtyFlag = TRUE;
  }
  else if ( m_iPrecision > 0 )
  {
    m_iPrecision--;
    m_bLayoutDirtyFlag = TRUE;
  }
}

void KSpreadCell::setPrefix( const char * _prefix )
{
  m_strPrefix = _prefix;
  m_bLayoutDirtyFlag= TRUE;
}

void KSpreadCell::setPostfix( const char * _postfix )
{
  m_strPostfix = _postfix;
  m_bLayoutDirtyFlag= TRUE;
}

/*
void KSpreadCell::initAfterLoading()
{
    if ( text.isNull() )
	return;
    setText( text.data() );
}
*/

void KSpreadCell::setText( const QString& _text )
{
  m_bError = false;
  m_strText = _text;

  m_lstDepends.clear();

  // Free all content data
  if ( m_pQML )
    delete m_pQML;
  m_pQML = 0;
  if ( m_pVisualFormula )
    delete m_pVisualFormula;
  m_pVisualFormula = 0;
  if ( isFormular() )
    clearFormular();

  /**
   * A real formula "=A1+A2*3" was entered.
   */
  if ( !m_strText.isEmpty() && m_strText[0] == '=' )
  {
    m_bCalcDirtyFlag = true;
    m_bLayoutDirtyFlag= true;
    m_content = Formula;

    if ( !m_pTable->isLoading() )
	if ( !makeFormular() )
	    printf("ERROR: Syntax ERROR\n");
    // A Hack!!!! For testing only
    // QString ret = encodeFormular( column, row );
    // decodeFormular( ret, column, row );
  }
  /**
   * QML
   */
  else if ( !m_strText.isEmpty() && m_strText[0] == '!' )
  {
    m_pQML = new QSimpleRichText( m_strText.mid(1),  QApplication::font() );//, m_pTable->widget() );

    m_bValue = false;
    m_bBool = false;
    m_bLayoutDirtyFlag = true;
    m_content = RichText;
  }
  /**
   * A visual formula.
   */
  else if ( !m_strText.isEmpty() && m_strText[0] == '*' )
  {
    m_pVisualFormula = new KFormula();
    m_pVisualFormula->parse( m_strText.mid( 1 ) );

    m_bValue = false;
    m_bBool = false;
    m_bLayoutDirtyFlag = true;
    m_content = VisualFormula;
  }
  /**
   * Some numeric value or a string.
   */
  else
  {
    // Find out what it is
    checkValue();
		
    m_bLayoutDirtyFlag = true;
    m_content = Text;
  }

  /**
   *  Special handling for selection boxes
   */
  if ( m_style == ST_Select && !m_pTable->isLoading() )
  {
      if ( m_bCalcDirtyFlag )
	  calc();
      if ( m_bLayoutDirtyFlag )
	  makeLayout( m_pTable->painter(), column(), row() );

      SelectPrivate *s = (SelectPrivate*)m_pPrivate;
      if ( m_content == Formula )
	  s->parse( m_strFormularOut );
      else
	  s->parse( m_strText );
      checkValue();
      // m_bLayoutDirtyFlag = true;
  }

  // Do not update formulas and stuff here
  // if we are still loading
  if ( !m_pTable->isLoading() )
      update();
}

void KSpreadCell::setValue( double _d )
{
    m_bError = false;
    m_strText = QString::number( _d );

    // Free all content data
    if ( m_pQML )
	delete m_pQML;
    m_pQML = 0;
    if ( m_pVisualFormula )
	delete m_pVisualFormula;
    m_pVisualFormula = 0;
    if ( isFormular() )
	clearFormular();

    m_lstDepends.clear();

    m_bValue = true;
    m_bBool = false;
    m_dValue = _d;
    m_bLayoutDirtyFlag = true;
    m_content = Text;

    // Do not update formulas and stuff here
    if ( !m_pTable->isLoading() )
	update();
}

void KSpreadCell::update()
{
    if ( m_pObscuringCell )
    {
	m_pObscuringCell->setLayoutDirtyFlag();
	m_pObscuringCell->setDisplayDirtyFlag();
	m_pTable->emit_updateCell( m_pObscuringCell, m_pObscuringCell->column(), m_pObscuringCell->row() );
    }

    bool b_update_begin = m_bDisplayDirtyFlag;
    m_bDisplayDirtyFlag = true;

    updateDepending();

    if ( !b_update_begin && m_bDisplayDirtyFlag )
	m_pTable->emit_updateCell( this, m_iColumn, m_iRow );
}

void KSpreadCell::updateDepending()
{
    // Every cell that references us must set its calc dirty flag
    QListIterator<KSpreadTable> it( m_pTable->map()->tableList() );
    for( ; it.current(); ++it )
    {
	QIntDictIterator<KSpreadCell> it3( it.current()->m_dctCells );
	for ( ; it3.current(); ++it3 )
	    if ( it3.current() != this )
		it3.current()->setCalcDirtyFlag( m_pTable, m_iColumn, m_iRow );
    }

    // Recalculate every cell with calc dirty flag
    QListIterator<KSpreadTable> it2( m_pTable->map()->tableList() );
    for( ; it2.current(); ++it2 )
    {
	QIntDictIterator<KSpreadCell> it4( it2.current()->m_dctCells );
	for ( ; it4.current(); ++it4 )
	    it4.current()->calc( TRUE );
    }

    // Update a chart for example if it depends on this cell.
    if ( m_iRow != 0 && m_iColumn != 0 )
    {
	CellBinding *bind;
	for ( bind = m_pTable->firstCellBinding(); bind != 0L; bind = m_pTable->nextCellBinding() )
        {
	    if ( bind->contains( m_iColumn, m_iRow ) )
		bind->cellChanged( this );
	}
    }
}

void KSpreadCell::checkValue()
{
    // If the input is empty, we dont have a value
    if ( m_strText.isEmpty() )
    {
      m_bValue = false;
      m_dValue = 0;
      m_bBool = false;
      m_strOutText = "";
      return;
    }

    // Get the text that we actually display
    const char *p = m_strText.data();	
    if ( m_style == ST_Select )
      p = ((SelectPrivate*)m_pPrivate)->text();
    else if ( isFormular() )
      p = m_strFormularOut.data();
    const char *ptext = p;

    // If the output is empty, we dont have a value
    if ( p == 0L )
    {
      m_bValue = false;
      m_bBool = false;
      return;
    }

    // Test for boolean
    if ( strcasecmp( p, "true") == 0 )
    {
      m_bValue = false;
      m_dValue = 1.0;
      m_bBool = true;
      return;
    }
    else if ( strcasecmp( p, "false" ) == 0 )
    {
      m_bValue = false;
      m_dValue = 0.0;
      m_bBool = true;
      return;
    }
    m_bBool = false;

    // Test whether it is a numeric value
    m_bValue = TRUE;
    bool point = FALSE;
    if ( *p == '+' || *p == '-' )
	p++;
    while ( *p != 0 && m_bValue )
    {
	if ( *p == ',' || *p == '.' || *p == decimal_point )
	{
	    // Only one decimal point is allowed
	    if ( point )
	      m_bValue = FALSE;
	    else
	      point = TRUE;
	}
	else if ( !isdigit( *p ) )
	  m_bValue = FALSE;
	p++;
    }

    if ( m_bValue )
	m_dValue = atof( ptext );

    /* if ( old_value != bValue )
	displayDirtyFlag = TRUE; */
}

void KSpreadCell::setCalcDirtyFlag( KSpreadTable *_table, int _column, int _row )
{
  // Dont go in an infinite loop if the stupid user
  // entered some formulas with circular dependencies.
  if ( m_bCalcDirtyFlag )
    return;

  bool isdep = FALSE;

  KSpreadDepend *dep;
  for ( dep = m_lstDepends.first(); dep != 0L; dep = m_lstDepends.next() )
  {
    if ( dep->m_iColumn2 != -1 )
    {
      int left = dep->m_iColumn < dep->m_iColumn2 ? dep->m_iColumn : dep->m_iColumn2;
      int right = dep->m_iColumn > dep->m_iColumn2 ? dep->m_iColumn : dep->m_iColumn2;
      int top = dep->m_iRow < dep->m_iRow2 ? dep->m_iRow : dep->m_iRow2;
      int bottom = dep->m_iRow > dep->m_iRow2 ? dep->m_iRow : dep->m_iRow2;
      if ( _table == dep->m_pTable )
	if ( left <= _column && _column <= right )
	  if ( top <= _row && _row <= bottom )
	    isdep = TRUE;
    }
    else if ( dep->m_iColumn == _column && dep->m_iRow == _row && dep->m_pTable == _table )
      isdep = TRUE;
  }

  if ( isdep )
  {
    m_bCalcDirtyFlag = TRUE;

    QListIterator<KSpreadTable> it( m_pTable->map()->tableList() );
    for( ; it.current(); ++it )
    {
      QIntDictIterator<KSpreadCell> it2( it.current()->m_dctCells );
      for ( ; it2.current(); ++it2 )
	it2.current()->setCalcDirtyFlag( m_pTable, m_iColumn, m_iRow );
    }
  }
}

QDomElement KSpreadCell::saveBottomMostBorder( QDomDocument& doc, int _x_offset, int _y_offset )
{
    QDomElement cell = doc.createElement( "bottom-most-border" );
    cell.setAttribute( "row", m_iRow - _y_offset );
    cell.setAttribute( "column", m_iColumn - _x_offset );

    cell.appendChild( doc.createElement( "pen", m_topBorderPen ) );

    return cell;
}

QDomElement KSpreadCell::saveRightMostBorder( QDomDocument& doc, int _x_offset, int _y_offset )
{
    QDomElement cell = doc.createElement( "right-most-border" );
    cell.setAttribute( "row", m_iRow - _y_offset );
    cell.setAttribute( "column", m_iColumn - _x_offset );

    cell.appendChild( doc.createElement( "pen", m_leftBorderPen ) );

    return cell;
}

bool KSpreadCell::loadBottomMostBorder( const QDomElement& cell, int _xshift, int _yshift )
{
    bool ok;
    m_iRow = cell.attribute( "row" ).toInt( &ok ) + _yshift;
    if ( !ok ) return false;
    m_iColumn = cell.attribute( "column" ).toInt( &ok ) + _xshift;
    if ( !ok ) return false;

    // Validation
    if ( m_iRow < 1 || m_iRow > 0xFFFF )
    {
	cerr << "Value out of Range Cell:row=" << m_iRow << endl;
	return false;
    }
    if ( m_iColumn < 1 || m_iColumn > 0xFFFF )
    {
	cerr << "Value out of Range Cell:column=" << m_iColumn << endl;
	return false;
    }

    QDomElement pen = cell.namedItem( "pen" ).toElement();
    if ( !pen.isNull() )
	setTopBorderPen( pen.toPen() );

    return true;
}

bool KSpreadCell::loadRightMostBorder( const QDomElement& cell, int _xshift, int _yshift )
{
    bool ok;
    m_iRow = cell.attribute( "row" ).toInt( &ok ) + _yshift;
    if ( !ok ) return false;
    m_iColumn = cell.attribute( "column" ).toInt( &ok ) + _xshift;
    if ( !ok ) return false;

    // Validation
    if ( m_iRow < 1 || m_iRow > 0xFFFF )
    {
	cerr << "Value out of Range Cell:row=" << m_iRow << endl;
	return false;
    }
    if ( m_iColumn < 1 || m_iColumn > 0xFFFF )
    {
	cerr << "Value out of Range Cell:column=" << m_iColumn << endl;
	return false;
    }

    QDomElement pen = cell.namedItem( "pen" ).toElement();
    if ( !pen.isNull() )
	setLeftBorderPen( pen.toPen() );

    return true;
}

QDomElement KSpreadCell::save( QDomDocument& doc, int _x_offset, int _y_offset )
{
  QDomElement cell = doc.createElement( "cell" );
  cell.setAttribute( "row", m_iRow - _y_offset );
  cell.setAttribute( "column", m_iColumn - _x_offset );

  if ( !action().isEmpty() )
      cell.setAttribute( "action", action() );

  QDomElement format = doc.createElement( "format" );
  cell.appendChild( format );
  format.setAttribute( "align", (int)m_eAlign );
  format.setAttribute( "alignY", (int)m_eAlignY );

  if ( m_bgColor != Qt::white )
    format.setAttribute( "bgcolor", m_bgColor.name() );
  if ( multiRow() )
    format.setAttribute( "multirow", "yes" );
  if ( style() )
      format.setAttribute( "style", (int)style() );

  if ( isForceExtraCells() )
  {
    format.setAttribute( "colspan", extraXCells() );
    format.setAttribute( "rowspan", extraYCells() );
  }

  format.setAttribute( "precision", precision() );
  if ( !prefix().isEmpty() )
    format.setAttribute( "prefix", prefix() );
  if ( !postfix().isEmpty() )
    format.setAttribute( "postfix", postfix() );

  format.setAttribute( "float", (int)floatFormat() );
  format.setAttribute( "floatcolor", (int)floatColor() );
  format.setAttribute( "faktor", m_dFaktor );

  if ( m_textFont != m_pTable->defaultCell()->textFont() )
    format.appendChild( doc.createElement( "font", m_textFont ) );
  if ( m_textPen != m_pTable->defaultCell()->textPen() )
    format.appendChild( doc.createElement( "pen", m_textPen ) );

  QDomElement left = doc.createElement( "left-border" );
  left.appendChild( doc.createElement( "pen", m_leftBorderPen ) );
  format.appendChild( left );

  QDomElement top = doc.createElement( "top-border" );
  top.appendChild( doc.createElement( "pen", m_topBorderPen ) );
  format.appendChild( top );

  QDomElement fallDiagonal  = doc.createElement( "fall-diagonal" );
  fallDiagonal.appendChild( doc.createElement( "pen", m_fallDiagonalPen ) );
  format.appendChild( fallDiagonal );

  QDomElement goUpDiagonal = doc.createElement( "up-diagonal" );
  goUpDiagonal.appendChild( doc.createElement( "pen", m_goUpDiagonalPen ) );
  format.appendChild( goUpDiagonal );

  if ( !m_strText.isEmpty() )
  {
    if ( isFormular() )
    {
      QDomElement text = doc.createElement( "text" );
      text.appendChild( doc.createTextNode( encodeFormular() ) );
      cell.appendChild( text );
    }
    else if ( content() == RichText || content() == VisualFormula )
    {
      QDomElement text = doc.createElement( "text" );
      text.appendChild( doc.createCDATASection( m_strText ) );
      cell.appendChild( text );
    }
    else
    {
      QDomElement text = doc.createElement( "text" );
      text.appendChild( doc.createTextNode( m_strText ) );
      cell.appendChild( text );
    }
  }

  return cell;
}

bool KSpreadCell::load( const QDomElement& cell, int _xshift, int _yshift, PasteMode pm, Operation op )
{
    bool ok;
    m_iRow = cell.attribute( "row" ).toInt( &ok ) + _yshift;
    if ( !ok ) return false;
    m_iColumn = cell.attribute( "column" ).toInt( &ok ) + _xshift;
    if ( !ok ) return false;

    if ( cell.hasAttribute( "action" ) )
	setAction( cell.attribute("action") );

    // Validation
    if ( m_iRow < 1 || m_iRow > 0xFFFF )
    {
	cerr << "Value out of Range Cell:row=" << m_iRow << endl;
	return false;
    }
    if ( m_iColumn < 1 || m_iColumn > 0xFFFF )
    {
	cerr << "Value out of Range Cell:column=" << m_iColumn << endl;
	return false;
    }

    QDomElement f = cell.namedItem( "format" ).toElement();
    if ( !f.isNull() && ( pm == Normal || pm == Format || pm == NoBorder ) )
    {
	if ( f.hasAttribute( "align" ) )
        {
	    Align a = (Align)f.attribute("align").toInt( &ok );
	    if ( !ok )
		return false;
	    // Validation
	    if ( (unsigned int)a < 1 || (unsigned int)a > 4 )
	    {
		cerr << "Value of of range Cell::align=" << (unsigned int)a << endl;
		return false;
	    }
	    // Assignment
	    setAlign( a );
	}
        if ( f.hasAttribute( "alignY" ) )
        {
	    AlignY a = (AlignY)f.attribute("alignY").toInt( &ok );
	    if ( !ok )
		return false;
	    // Validation
	    if ( (unsigned int)a < 1 || (unsigned int)a > 4 )
	    {
		cerr << "Value of of range Cell::alignY=" << (unsigned int)a << endl;
		return false;
	    }
	    // Assignment
	    setAlignY( a );
	}
	if ( f.hasAttribute( "bgcolor" ) )
	    setBgColor( QColor( f.attribute( "bgcolor" ) ) );
	if ( f.hasAttribute( "multirow" ) )
	    setMultiRow( true );
	if ( f.hasAttribute( "colspan" ) )
        {
	    int i = f.attribute("colspan").toInt( &ok );
	    if ( !ok ) return false;
	    // Validation
	    if ( i < 0 || i > 0xFFF )
	    {
		cerr << "Value out of range Cell::colspan=" << i << endl;
		return false;
	    }
	    m_iExtraXCells = i;
	    if ( i > 0 )
		m_bForceExtraCells = true;	
	}
	if ( f.hasAttribute( "rowspan" ) )
        {
	    int i = f.attribute("rowspan").toInt( &ok );
	    if ( !ok ) return false;
	    // Validation
	    if ( i < 0 || i > 0xFFF )
	    {
		cerr << "Value out of range Cell::rowspan=" << i << endl;
		return false;
	    }
	    m_iExtraYCells = i;
	    if ( i > 0 )
		m_bForceExtraCells = true;	
	}
	if ( f.hasAttribute( "precision" ) )
        {
	    int i = f.attribute("precision").toInt( &ok );
	    if ( i < -1 )
	    {
		cerr << "Value out of range Cell::precision=" << i << endl;
		return false;
	    }
	    m_iPrecision = i;
	}
	if ( f.hasAttribute( "float" ) )
        {
	    FloatFormat a = (FloatFormat)f.attribute("float").toInt( &ok );
	    if ( !ok ) return false;
	    if ( (unsigned int)a < 1 || (unsigned int)a > 3 )
	    {
		cerr << "Value of of range Cell::float=" << (unsigned int)a << endl;
		return false;
	    }
	    // Assignment
	    setFloatFormat( a );
	}
	if ( f.hasAttribute( "floatcolor" ) )
        {
	    FloatColor a = (FloatColor)f.attribute("floatcolor").toInt( &ok );
	    if ( !ok ) return false;
	    if ( (unsigned int)a < 1 || (unsigned int)a > 2 )
	    {
		cerr << "Value of of range Cell::floatcolor=" << (unsigned int)a << endl;
		return false;
	    }
	    // Assignment
	    setFloatColor( a );
	}
	if ( f.hasAttribute( "faktor" ) )
        {
	    m_dFaktor = f.attribute("faktor").toDouble( &ok );
	    if ( !ok ) return false;
	}

	QDomElement pen = f.namedItem( "pen" ).toElement();
	if ( !pen.isNull() )
	    setTextPen( pen.toPen() );

	QDomElement font = f.namedItem( "font" ).toElement();
	if ( !font.isNull() )
	    setTextFont( font.toFont() );

	QDomElement left = f.namedItem( "left-border" ).toElement();
	if ( !left.isNull() && pm != NoBorder )
        {
	    QDomElement pen = left.namedItem( "pen" ).toElement();
	    if ( !pen.isNull() )
		setLeftBorderPen( pen.toPen() );
	}

	QDomElement top = f.namedItem( "top-border" ).toElement();
	if ( !top.isNull() && pm != NoBorder )
        {
	    QDomElement pen = top.namedItem( "pen" ).toElement();
	    if ( !pen.isNull() )
		setTopBorderPen( pen.toPen() );
	}

	QDomElement fallDiagonal = f.namedItem( "fall-diagonal" ).toElement();
	if ( !fallDiagonal.isNull() && pm != NoBorder )
        {
	    QDomElement pen = fallDiagonal.namedItem( "pen" ).toElement();
	    if ( !pen.isNull() )
		setFallDiagonalPen( pen.toPen() );
	}

	QDomElement goUpDiagonal = f.namedItem( "up-diagonal" ).toElement();
	if ( !goUpDiagonal.isNull() && pm != NoBorder )
        {
	    QDomElement pen = goUpDiagonal.namedItem( "pen" ).toElement();
	    if ( !pen.isNull() )
		setGoUpDiagonalPen( pen.toPen() );
	}

	m_strPrefix = f.attribute( "prefix" );
	m_strPostfix = f.attribute( "postfix" );
    }

    QDomElement text = cell.namedItem( "text" ).toElement();
    if ( !text.isNull() && ( pm == ::Normal || pm == ::Text || pm == ::NoBorder ) )
    {
	QString t = text.text();
	t = t.stripWhiteSpace();

	if ( t[0] == '=' )
	    t = decodeFormular( t, m_iColumn, m_iRow );

	setText( pasteOperation( t, m_strText, op ) );
    }

    if ( !f.isNull() && f.hasAttribute( "style" ) )
	setStyle( (Style)f.attribute("style").toInt() );

    return true;
}

QString KSpreadCell::pasteOperation( QString new_text, QString old_text, Operation op )
{
    if ( op == OverWrite )
	return new_text;

    QString tmp_op;
    QString tmp;
    QString old;
    if( new_text.find("=") == 0 )
    {
	tmp = new_text.right( new_text.length() - 1 );
    }
    else
    {
	tmp = new_text;
    }

    if( old_text.find("=") == 0 )
    {
	old = old_text.right( old_text.length() - 1 );
    }
    else
    {
	old = old_text;
    }

    QString inter;
    bool b1, b2;
    tmp.toDouble( &b1 );
    old.toDouble( &b2 );

    if( b1 && b2 )
    {
	switch( op )
        {
	case  Add:
	    tmp_op = "="+inter.setNum(old.toDouble()+tmp.toDouble());
	    break;
	case Mul :
	    tmp_op = "="+inter.setNum(old.toDouble()*tmp.toDouble());
	    break;
	case Sub:
	    tmp_op = "="+inter.setNum(old.toDouble()-tmp.toDouble());
	    break;
	case Div:
	    tmp_op = "="+inter.setNum(old.toDouble()/tmp.toDouble());
	    break;
	default:
	    ASSERT( 0 );
	}
	return tmp_op;
    }
    else if ( ( new_text[0] == '=' && old_text[0] == '=' ) ||
	      ( b1 && old_text[0] == '=' ) || ( new_text[0] == '=' && b2 ) )
    {
	switch( op )
        {
	case  Add:
	    tmp_op="=("+old+")+"+"("+tmp+")";
	    break;
	case Mul :
	    tmp_op="=("+old+")*"+"("+tmp+")";
	    break;
	case Sub:
	    tmp_op="=("+old+")-"+"("+tmp+")";
	    break;
	case Div:
	    tmp_op="=("+old+")/"+"("+tmp+")";
	    break;
	default :
	    ASSERT( 0 );
	}
	return tmp_op;
    }

    return new_text;
}

void KSpreadCell::setStyle( Style _s )
{
  if ( m_style == _s )
    return;

  m_style = _s;
  m_bLayoutDirtyFlag = true;

  if ( m_pPrivate )
    delete m_pPrivate;

  if ( _s != ST_Select )
    return;

  m_pPrivate = new SelectPrivate( this );

  SelectPrivate *s = (SelectPrivate*)m_pPrivate;
  if ( isFormular() )
      s->parse( m_strFormularOut );
  else
      s->parse( m_strText );
  checkValue();
  m_bLayoutDirtyFlag = true;

  if ( !m_pTable->isLoading() )
      update();
}

QString KSpreadCell::testAnchor( int _x, int _y, QWidget* _w )
{
  if ( !m_pQML )
    return QString();

  QPainter p( _w );
  return m_pQML->anchor( &p, QPoint( _x, _y ) );
}

KSpreadCell::~KSpreadCell()
{
  if ( m_pPrivate )
    delete m_pPrivate;
  if ( m_pQML )
    delete m_pQML;
  if ( m_pVisualFormula )
    delete m_pVisualFormula;

  for( int x = 0; x <= m_iExtraXCells; ++x )
    for( int y = 0; y <= m_iExtraYCells; ++y )
    {
      KSpreadCell* cell = m_pTable->cellAt( m_iColumn + x, m_iRow + y );
      if ( cell && cell != this )
	cell->unobscure();
    }
}

/***************************************************
 *
 * SelectPrivate
 *
 ***************************************************/

void SelectPrivate::parse( const char* _text )
{
  m_lstItems.clear();

  if ( !_text )
    return;

  char *p = new char[ strlen( _text ) + 1 ];
  strcpy( p, _text );
  char *str = p;

  char *s;
  while ( ( s = strchr( str, '\\' ) ) != 0L )
  {
    *s++ = 0;
    if ( strlen( str ) > 0 )
      m_lstItems.append( str );
    str = s;
  }

  if ( strlen( str ) > 0 )
    m_lstItems.append( str );

  if ( m_iIndex != -1 && m_iIndex < m_lstItems.count() )
  { }
  else if ( m_lstItems.count() > 0 )
    m_iIndex = 0;
  else
    m_iIndex = -1;

  delete[] p;
}

void SelectPrivate::slotItemSelected( int _id )
{
  m_iIndex = _id;

  m_pCell->setLayoutDirtyFlag();
  m_pCell->checkValue();
  m_pCell->update();

  m_pCell->table()->emit_updateCell( m_pCell, m_pCell->column(), m_pCell->row() );
}

const char* SelectPrivate::text()
{
  if ( m_iIndex == -1 )
    return "";

  const char* p = m_lstItems.at( m_iIndex );
  if ( p == 0L )
    return "";

  return p;
}

#include "kspread_cell.moc"
