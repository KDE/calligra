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
#include <kmessagebox.h>
#include <kscript_parsenode.h>

#define UPDATE_BEGIN bool b_update_begin = m_bDisplayDirtyFlag; m_bDisplayDirtyFlag = true;
#define UPDATE_END if ( !b_update_begin && m_bDisplayDirtyFlag ) m_pTable->updateCell( this, m_iColumn, m_iRow );
#define DO_UPDATE m_pTable->updateCell( this, m_iColumn, m_iRow )

QChar KSpreadCell::decimal_point = '\0';

/*****************************************************************************
 *
 * KSpreadCell
 *
 *****************************************************************************/

KSpreadCell::KSpreadCell( KSpreadTable *_table, int _column, int _row )
  : KSpreadLayout( _table )
{
    m_nextCell = 0;
    m_previousCell = 0;

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
  m_bDate = FALSE;
  m_bTime = FALSE;
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
  m_iOutTextWidth = 0;
  m_iOutTextHeight = 0;
  m_firstCondition = 0;
  m_secondCondition = 0;
  m_thirdCondition = 0;
  m_conditionIsTrue=false;
  m_numberOfCond=-1;
  m_nbLines=0;
  m_rotateAngle=0;
  m_eFormatNumber=KSpreadCell::Number;
}

void KSpreadCell::copyLayout( KSpreadCell *_cell )
{
    copyLayout( _cell->column(), _cell->row() );
}

void KSpreadCell::copyLayout( int _column, int _row )
{
    KSpreadCell *o = m_pTable->cellAt( _column, _row );

    setAlign( o->align( _column, _row ) );
    setAlignY( o->alignY( _column, _row ) );
    setTextFont( o->textFont( _column, _row ) );
    setTextColor( o->textColor( _column, _row ) );
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
    setBackGroundBrushColor(o->backGroundBrushColor( _column, _row) );
    setBackGroundBrushStyle(o->backGroundBrushStyle( _column, _row) );
    setPrecision( o->precision( _column, _row ) );
    setPrefix( o->prefix( _column, _row ) );
    setPostfix( o->postfix( _column, _row ) );
    setFloatFormat( o->floatFormat( _column, _row ) );
    setFloatColor( o->floatColor( _column, _row ) );
    setFaktor( o->faktor( _column, _row ) );
    setMultiRow( o->multiRow( _column, _row ) );
    setVerticalText( o->verticalText( _column, _row ) );
    setStyle( o->style() );

    KSpreadConditional *tmpCondition;
    if( o->getFirstCondition(0) )
    {
  	tmpCondition=getFirstCondition();
  	tmpCondition->val1=o->getFirstCondition(0)->val1;
  	tmpCondition->val2=o->getFirstCondition(0)->val2;
  	tmpCondition->colorcond=o->getFirstCondition(0)->colorcond;
  	tmpCondition->fontcond=o->getFirstCondition(0)->fontcond;
  	tmpCondition->m_cond=o->getFirstCondition(0)->m_cond;
    }
    if( o->getSecondCondition(0) )
    {
  	tmpCondition=getSecondCondition();
  	tmpCondition->val1=o->getSecondCondition(0)->val1;
  	tmpCondition->val2=o->getSecondCondition(0)->val2;
  	tmpCondition->colorcond=o->getSecondCondition(0)->colorcond;
  	tmpCondition->fontcond=o->getSecondCondition(0)->fontcond;
  	tmpCondition->m_cond=o->getSecondCondition(0)->m_cond;
    }
    if( o->getThirdCondition(0) )
    {
  	tmpCondition=getThirdCondition();
  	tmpCondition->val1=o->getThirdCondition(0)->val1;
  	tmpCondition->val2=o->getThirdCondition(0)->val2;
  	tmpCondition->colorcond=o->getThirdCondition(0)->colorcond;
  	tmpCondition->fontcond=o->getThirdCondition(0)->fontcond;
  	tmpCondition->m_cond=o->getThirdCondition(0)->m_cond;
    }
    setComment( o->comment() );
    setAngle( o->getAngle() );
    setFormatNumber( o->getFormatNumber() );
}

void KSpreadCell::copyAll( KSpreadCell *cell )
{
    ASSERT( !isDefault() ); // trouble ahead...
    copyLayout( cell );
    copyContent( cell );
}

void KSpreadCell::copyContent( KSpreadCell* cell )
{
    ASSERT( !isDefault() ); // trouble ahead...

    setCellText( cell->text() );
    setAction(cell->action() );

    if ( m_pPrivate )
	delete m_pPrivate;
    m_pPrivate = 0;
    if ( cell->m_pPrivate )
	m_pPrivate = cell->m_pPrivate->copy( this );
}

void KSpreadCell::defaultStyle()
{
  setBottomBorderStyle( Qt::NoPen );
  setBottomBorderColor( Qt::black ); // TODO set to QColor() and change painting to use default colors
  setBottomBorderWidth( 1 );
  setRightBorderStyle( Qt::NoPen );
  setRightBorderColor( Qt::black );
  setRightBorderWidth( 1 );
  setLeftBorderStyle( Qt::NoPen );
  setLeftBorderColor( Qt::black );
  setLeftBorderWidth( 1 );
  setTopBorderStyle( Qt::NoPen );
  setTopBorderColor( Qt::black );
  setTopBorderWidth( 1 );
  setFallDiagonalStyle( Qt::NoPen );
  setFallDiagonalColor( Qt::black );
  setFallDiagonalWidth( 1 );
  setGoUpDiagonalStyle(Qt::NoPen );
  setGoUpDiagonalColor( Qt::black );
  setGoUpDiagonalWidth( 1 );
  setAlign( KSpreadCell::Undefined );
  setAlignY( KSpreadCell::Middle );
  setBackGroundBrushColor(Qt::red);
  setBackGroundBrushStyle(Qt::NoBrush);
  QFont font( "Times", 12 );
  m_textFont = font;
  setTextColor( QColor() );
  setBgColor( QColor() );
  setFaktor( 1.0);
  setPrecision( -1 );
  setPostfix( "" );
  setPrefix( "" );
  if(m_firstCondition!=0)
  	delete m_firstCondition;
  m_firstCondition=0;

  if(m_thirdCondition!=0)
  	delete m_thirdCondition;
  m_thirdCondition=0;

  if(m_secondCondition!=0)
  	delete m_secondCondition;
  m_secondCondition=0;

  m_conditionIsTrue=false;
  m_numberOfCond=-1;
  m_strComment="";
  m_bVerticalText=false;
  m_rotateAngle=0;
  m_eFormatNumber=KSpreadCell::Number;
}

void KSpreadCell::layoutChanged()
{
    m_bLayoutDirtyFlag = TRUE;
}

KSpreadLayout* KSpreadCell::fallbackLayout( int, int row )
{
    return table()->rowLayout( row );
}

const KSpreadLayout* KSpreadCell::fallbackLayout( int, int row ) const
{
    return table()->rowLayout( row );
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

void KSpreadCell::move( int col, int row )
{
    setLayoutDirtyFlag();
    setCalcDirtyFlag();
    setDisplayDirtyFlag();

    int ex = extraXCells();
    int ey = extraYCells();

    if ( m_pObscuringCell )
	m_pObscuringCell = 0;

    // Unobscure the objects we obscure right now
    for( int x = m_iColumn; x <= m_iColumn + m_iExtraXCells; x++ )
	for( int y = m_iRow; y <= m_iRow + m_iExtraYCells; y++ )
	    if ( x != m_iColumn || y != m_iRow )
	    {
		KSpreadCell *cell = m_pTable->nonDefaultCell( x, y );
		cell->unobscure();
	    }

    m_iColumn = col;
    m_iRow = row;

    // Reobscure cells if we are forced to do so.
    if ( m_bForceExtraCells )
	forceExtraCells( col, row, ex, ey );
}

void KSpreadCell::setLayoutDirtyFlag()
{
    m_bLayoutDirtyFlag= TRUE;

    if ( m_pObscuringCell )
	m_pObscuringCell->setLayoutDirtyFlag();
}

bool KSpreadCell::isEmpty() const
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
    QStringList::ConstIterator it = s->m_lstItems.begin();
    for( ; it != s->m_lstItems.end(); ++it )
	popup->insertItem( *it, id++ );

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
    QString tmp(i18n("Error in cell %1\n\n"));
    tmp = tmp.arg( util_cellName( m_pTable, m_iColumn, m_iRow ) );
    tmp += context.exception()->toString( context );
    KMessageBox::error((QWidget*)0L , tmp);
    return;
  }

  KSContext& context2 = m_pTable->doc()->context();
  if ( !m_pTable->doc()->interpreter()->evaluate( context2, code, m_pTable ) )
      // Print out exception if any
      if ( context2.exception() )
      {
	  QString tmp(i18n("Error in cell %1\n\n"));
	  tmp = tmp.arg( util_cellName( m_pTable, m_iColumn, m_iRow ) );
	  tmp += context2.exception()->toString( context2 );
	  KMessageBox::error( (QWidget*)0L, tmp);
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
		kdError(36001) << "ERROR: out of range" << endl;
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

// ##### Are _col and _row really needed ?
void KSpreadCell::makeLayout( QPainter &_painter, int _col, int _row )
{
    m_leftBorderPen.setWidth( leftBorderWidth( _col, _row ) );
    m_topBorderPen.setWidth( topBorderWidth( _col, _row ) );
    m_fallDiagonalPen.setWidth( fallDiagonalWidth( _col, _row) );
    m_goUpDiagonalPen.setWidth( goUpDiagonalWidth( _col, _row) );

    m_nbLines = 0;

    //
    // Free all obscured cells.
    //
    if ( !m_bForceExtraCells )
    {
	for ( int x = m_iColumn; x <= m_iColumn + m_iExtraXCells; ++x )
	    for ( int y = m_iRow; y <= m_iRow + m_iExtraYCells; ++y )
		if ( x != m_iColumn || y != m_iRow )
	        {
		    KSpreadCell *cell = m_pTable->cellAt( x, y );
		    cell->unobscure();
		}

	m_iExtraXCells = 0;
	m_iExtraYCells = 0;
    }

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
	kdDebug(36001) << "QML w=" << w << " max=" << max_width << endl;

	m_richWidth = w;
	m_richHeight = h;

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

	kdDebug(36001) << "Formula w=" << w << " h=" << h << endl;
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
     * A usual numeric, boolean, date, time or string value.
     */

    m_textPen.setColor( textColor( _col, _row ) );
    m_conditionIsTrue = false;

    //
    // Turn the stored value in a string
    //

    if ( isFormular() && m_pTable->getShowFormular() )
    {
	m_strOutText = m_strText;
    }
    else if ( m_style == ST_Select )
    {
	// If this is a select box, find out about the selected item
	// in the KSpreadPrivate data struct
	SelectPrivate *s = (SelectPrivate*)m_pPrivate;
	m_strOutText = s->text();
    }
    else if ( isBool() )
    {
	if ( m_dValue == 0 )
	    m_strOutText = "False";
	else
	    m_strOutText = "True";
    }
    else if( isDate() )
    {
	if( m_eFormatNumber == ShortDate)
	    m_strOutText = KGlobal::locale()->formatDate(m_Date,true);
	else if( m_eFormatNumber == TextDate )
	    m_strOutText=KGlobal::locale()->formatDate(m_Date,false);
    }
    else if( isTime() )
    {
	if( m_eFormatNumber == Time )
	    m_strOutText = KGlobal::locale()->formatTime(m_Time,false);
	else if(m_eFormatNumber == SecondeTime )
	    m_strOutText = KGlobal::locale()->formatTime(m_Time,true);
    }
    else if ( isValue()  )
    {
	// First get some locale information
	if (!decimal_point)
        { // (decimal_point is static)
	    decimal_point = KGlobal::locale()->decimalSymbol()[0];
	    kdDebug(36001) << "decimal_point is '" << decimal_point.latin1() << "'" << endl;

	    if ( decimal_point.isNull() )
		decimal_point = '.';
	}

	// Scale the value as desired by the user.
	double v = m_dValue * m_dFaktor;

	// Always unsigned ?
	if ( floatFormat( _col, _row ) == KSpreadCell::AlwaysUnsigned && v < 0.0)
	    v *= -1.0;

	// Make a string out of it.
	QString localizedNumber = createFormat( v, _col, _row );

	// Remove trailing zeros and the decimal point if necessary
	// unless the number has no decimal point
	if ( m_iPrecision == -1 && localizedNumber.find(decimal_point) >= 0 )
        {
	/*    int i = localizedNumber.length();
	    bool bFinished = FALSE;
	    while ( !bFinished && i > 0 )
	    {
		QChar ch = localizedNumber[ i - 1 ];
		if ( ch == '0' )
		    localizedNumber.truncate( --i );
		else
	        {
		    bFinished = TRUE;
		    if ( ch == decimal_point )
			localizedNumber.truncate( --i );
		}
	    }*/
            int start=0;
            if(localizedNumber.find('%')!=-1)
                start=2;
            else if(localizedNumber.find(decimal_point)==(localizedNumber.length()-1))
                start=2;
            else if((start=localizedNumber.find('E'))!=-1)
                start=localizedNumber.length()-start;
            else
                start=0;
            int i = localizedNumber.length()-start;
	    bool bFinished = FALSE;


            while ( !bFinished && i > 0 )
	    {
		QChar ch = localizedNumber[ i - 1 ];
                if ( ch == '0' )
                    localizedNumber.remove(--i,1);
		else
	        {
		    bFinished = TRUE;
		    if ( ch == decimal_point )
                        localizedNumber.remove(--i,1);
                }
            }
	}

	// Start building the output string with prefix and postfix
	m_strOutText = "";
	m_strOutText += prefix( _col, _row );

	m_strOutText += localizedNumber;

	m_strOutText += postfix( _col, _row );

	verifyCondition();

	// Find the correct color which depends on the conditions
	// and the sign of the value.
	if ( floatColor( _col, _row ) == KSpreadCell::NegRed && v < 0.0 )
	    m_textPen.setColor( Qt::red );
	else if( m_conditionIsTrue )
    	{
	    KSpreadConditional *tmpCondition=0;
	    switch(m_numberOfCond)
	    {
	    case 0:
		tmpCondition=m_firstCondition;
		break;
	    case 1:
		tmpCondition=m_secondCondition;
		break;
	    case 2:
		tmpCondition=m_thirdCondition;
		break;
	    }

	    m_textPen.setColor(tmpCondition->colorcond);
 	}
    }
    else if ( isFormular() )
    {
	m_strOutText = m_strFormularOut;
    }
    else
    {
	m_strOutText = m_strText;
    }

    // Empty text?
    if ( m_strOutText.isEmpty() )
    {
	m_strOutText = QString::null;
	if ( isDefault() )
	    return;
    }

    _painter.setPen( m_textPen );

    // verifyCondition();

    //
    // Determine the correct font
    //
    if( m_conditionIsTrue && !m_pTable->getShowFormular() )
    {
        KSpreadConditional *tmpCondition=0;
        switch(m_numberOfCond)
        {
	case 0:
	    tmpCondition = m_firstCondition;
	    break;
	case 1:
	    tmpCondition = m_secondCondition;
	    break;
	case 2:
	    tmpCondition = m_thirdCondition;
	    break;
	}
        _painter.setFont( tmpCondition->fontcond );
    }
    else
        _painter.setFont( m_textFont );

    // Calculate text dimensions
    textSize(_painter);

    QFontMetrics fm = _painter.fontMetrics();

    //
    // Calculate the size of the cell
    //

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
	    w += cl->width() ;
	}
	for ( int y = _row + 1; y <= _row + m_iExtraYCells; y++ )
        {
	    RowLayout *rl = m_pTable->rowLayout( y );
	    h += rl->height() ;
	}
    }

    m_iExtraWidth = w;
    m_iExtraHeight = h;

    // Some space for the little button of the combo box
    if ( m_style == ST_Select )
	w -= 16;

    // Do we need to break the line into multiple lines and are we allowed to
    // do so?
    int lines = 1;
    if ( m_iOutTextWidth > w - 2 * BORDER_SPACE - leftBorderWidth( _col, _row) -
	 rightBorderWidth( _col, _row ) && m_bMultiRow )
    {
	// copy of m_strOutText
	QString o = m_strOutText;

	// No space ?
	if( o.find(' ') != -1 )
        {
	    o += ' ';
	    int start = 0;
	    int pos = 0;
	    int pos1 = 0;
	    m_strOutText = "";
	    do
            {
		pos = o.find(' ',pos );
		int width = fm.width( m_strOutText.mid( start, (pos1-start) ) + o.mid( pos1, (pos-pos1) ) );

		if ( width <= w - 2 * BORDER_SPACE - leftBorderWidth( _col, _row) - rightBorderWidth( _col, _row ) )
	        {
		    m_strOutText += o.mid(pos1,(pos-pos1));
		    pos1 = pos;
		}
		else
                {
		    if(o.at(pos1)==' ')
			pos1 = pos1 + 1;
                    if(pos1!=0 && pos!=-1)
                        {
                        m_strOutText += "\n" + o.mid( pos1, ( pos - pos1 ) );
                        lines++;
                        }
                    else
                        m_strOutText += o.mid( pos1, ( pos - pos1 ) );
                    start = pos1;
		    pos1 = pos;
		}
		pos++;
	    }
	    while( o.find( ' ', pos ) != -1 );
        }

        m_iOutTextHeight *= lines;

	if( lines != 1 )
	    m_nbLines = lines - 1;
	else
	    m_nbLines = lines;

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

    // Calculate m_iTextX and m_iTextY
    offsetAlign(_col,_row);

    m_fmAscent = fm.ascent();

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
	    // ##### Why ?
	    if ( m_eAlign == KSpreadCell::Left || m_eAlign == KSpreadCell::Undefined )
	    {
		m_iExtraWidth = w;
		for( int i = m_iColumn + 1; i <= c; ++i )
	        {
		    KSpreadCell *cell = m_pTable->nonDefaultCell( i, m_iRow );
		    cell->obscure( this, m_iColumn, m_iRow );
		}
		m_iExtraXCells = c - m_iColumn;
	    }
	    else
	    {
		m_strOutText = "**";
	    }
	}
    }

    m_bLayoutDirtyFlag = FALSE;
}

QString KSpreadCell::createFormat( double value, int _col, int _row )
{
    // if precision is -1, ask for a huge number of decimals, we'll remove
    // the zeros later. Is 8 ok ?
    int p = (m_iPrecision == -1) ? 8 : m_iPrecision;
    QString localizedNumber= KGlobal::locale()->formatNumber( value, p );
    int pos = 0;

    switch( m_eFormatNumber)
    {
    case Number :
	localizedNumber = KGlobal::locale()->formatNumber(value, p);
	if( floatFormat( _col, _row ) == KSpreadCell::AlwaysSigned && value >= 0 )
        {
	    if(KGlobal::locale()->positiveSign().isEmpty())
		localizedNumber='+'+localizedNumber;
	}
	break;
    case Percentage :
	localizedNumber = KGlobal::locale()->formatNumber(value, p)+ " %";
	break;
    case Money :
	localizedNumber = KGlobal::locale()->formatMoney(value,KGlobal::locale()->currencySymbol(),p );
	if( floatFormat( _col, _row) == KSpreadCell::AlwaysSigned && value >= 0 )
        {
	    if(KGlobal::locale()->positiveSign().isNull())
		localizedNumber='+'+localizedNumber;
	}
	break;
    case Scientific:
	localizedNumber= QString::number(value, 'E', p);
	if((pos=localizedNumber.find('.'))!=-1)
	    localizedNumber=localizedNumber.replace(pos,1,decimal_point);

	break;
    case ShortDate:
    case TextDate :
	break;
    case fraction_half:
    case fraction_quarter:
    case fraction_eighth:
    case fraction_sixteenth:
    case fraction_tenth:
    case fraction_hundredth:
	localizedNumber=createFractionFormat(value);
	break;
    default :
	kdDebug(36001)<<"Error in m_eFormatNumber\n";
	break;
    }

    return localizedNumber;
}

QString KSpreadCell::createFractionFormat(double value)
{
    double result = value-floor(value);
    int index = 0;
    QString tmp;
    if(result == 0 )
    {
        tmp = tmp.setNum( value );
    }
    else
    {
        switch( m_eFormatNumber)
        {
	case fraction_half:
	    index=2;
	    break;
	case fraction_quarter:
	    index=4;
	    break;
	case fraction_eighth:
	    index=8;
	    break;
	case fraction_sixteenth:
	    index=16;
	    break;
	case fraction_tenth:
	    index=10;
	    break;
	case fraction_hundredth:
	    index=100;
	    break;
	default:
	    kdDebug(36001)<<"Error in Fraction format\n";
	    break;
	}

        double calc = 0;
        int index1 = 1;
        double diff = result;
        for(int i=1;i<index;i++)
        {
	    calc = i*1.0 / index;
	    if( fabs( result - calc ) < diff )
	    {
		index1=i;
		diff = fabs(result-calc);
	    }
	}
        tmp = tmp.setNum( floor(value) ) + " " + tmp.setNum( index1 ) + "/" + tmp.setNum( index );
    }

    return tmp;
}

void KSpreadCell::verifyCondition()
{
    m_numberOfCond=-1;
    double v = m_dValue * m_dFaktor;
    m_conditionIsTrue = false;
    KSpreadConditional *tmpCondition = 0;

    if(m_bValue && !m_pTable->getShowFormular())
    {
	for(int i=0;i<3;i++)
        {
	    switch(i)
	    {
	    case 0:
		tmpCondition=m_firstCondition;
		break;
	    case 1:
		tmpCondition=m_secondCondition;
		break;
	    case 2:
		tmpCondition=m_thirdCondition;
		break;
	    }

	    if( tmpCondition != 0 && tmpCondition->m_cond != None )
            {

		switch(tmpCondition->m_cond)
                {
                case Equal :
		    if(v == tmpCondition->val1 )
		    {
                        m_conditionIsTrue=true;
                        m_numberOfCond=i;
		    }
		    break;

                case Superior :
		    if( v > tmpCondition->val1 )
		    {
                        m_conditionIsTrue=true;
                        m_numberOfCond=i;
		    }
		    break;

                case Inferior :
		    if(v < tmpCondition->val1 )
		    {
                        m_conditionIsTrue=true;
                        m_numberOfCond=i;
		    }
		    break;

                case SuperiorEqual :
		    if( v >= tmpCondition->val1 )
		    {
                        m_conditionIsTrue=true;
                        m_numberOfCond=i;
		    }
		    break;

                case InferiorEqual :
		    if(v <= tmpCondition->val1 )
		    {
                        m_conditionIsTrue=true;
                        m_numberOfCond=i;
		    }
		    break;

                case Between :
		    if( ( v > QMIN(tmpCondition->val1, tmpCondition->val2 ) ) &&
			( v < QMAX(tmpCondition->val1, tmpCondition->val2 ) ) )
		    {
                        m_conditionIsTrue=true;
                        m_numberOfCond=i;
		    }
		    break;

                case Different :
		    if( ( v < QMIN(tmpCondition->val1, tmpCondition->val2 ) ) ||
			( v > QMAX(tmpCondition->val1, tmpCondition->val2) ) )
		    {
                        m_conditionIsTrue=true;
                        m_numberOfCond=i;
		    }
		    break;

                default:
		    kdDebug(36001) << "Pb in Conditional" << endl;

		    m_conditionIsTrue = false;
		    break;
                }

	    }
	}
    }
}

void KSpreadCell::offsetAlign( int _col,int _row )
{
    int a = m_eAlign;
    RowLayout *rl = m_pTable->rowLayout( _row );
    ColumnLayout *cl = m_pTable->columnLayout( _col );

    int w = cl->width();
    int h = rl->height();

    if ( m_iExtraXCells )
	w = m_iExtraWidth;
    if ( m_iExtraYCells )
	h = m_iExtraHeight;

    switch( m_eAlignY )
    {
    case KSpreadCell::Top:
	if(!m_rotateAngle)
	    m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +m_fmAscent;
	else
        {
	    if(m_rotateAngle<0)
		m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE ;
	    else
		m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +(int)(m_fmAscent*cos(m_rotateAngle*M_PI/180));
	}
	break;
    case KSpreadCell::Bottom:
	if(!m_bVerticalText && !m_bMultiRow && !m_rotateAngle)
	    m_iTextY = h - BORDER_SPACE - bottomBorderWidth( _col, _row );
	else if(m_rotateAngle!=0)
        {
	    if((h - BORDER_SPACE - m_iOutTextHeight- bottomBorderWidth( _col, _row ))>0)
		m_iTextY = h - BORDER_SPACE - m_iOutTextHeight- bottomBorderWidth( _col, _row );
	    else
		if( m_rotateAngle < 0 )
		    m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE ;
		else
		    m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +(int)(m_fmAscent*cos(m_rotateAngle*M_PI/180));
	}
	else if( m_bMultiRow )
        {
	    if((h - BORDER_SPACE - m_iOutTextHeight*m_nbLines- bottomBorderWidth( _col, _row ))>0)
		m_iTextY = h - BORDER_SPACE - m_iOutTextHeight*m_nbLines- bottomBorderWidth( _col, _row );
	    else
		m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +m_fmAscent;
	}
	else
	    if((h - BORDER_SPACE - m_iOutTextHeight- bottomBorderWidth( _col, _row ))>0)
		m_iTextY = h - BORDER_SPACE - m_iOutTextHeight- bottomBorderWidth( _col, _row );
	    else
		m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +m_fmAscent;
	break;
    case KSpreadCell::Middle:
	if(!m_bVerticalText && !m_bMultiRow && !m_rotateAngle)
	    m_iTextY = ( h - m_iOutTextHeight ) / 2 +m_fmAscent;
	else if( m_rotateAngle != 0 )
	    if( ( h - m_iOutTextHeight ) > 0 )
		m_iTextY = ( h - m_iOutTextHeight ) / 2 +(int)(m_fmAscent*cos(m_rotateAngle*M_PI/180));
	    else
		if( m_rotateAngle < 0 )
		    m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE ;
		else
		    m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +(int)(m_fmAscent*cos(m_rotateAngle*M_PI/180));
                else if(m_bMultiRow)
		    if(( h - m_iOutTextHeight*m_nbLines )>0)
			m_iTextY = ( h - m_iOutTextHeight*m_nbLines ) / 2 +m_fmAscent;
		    else
			m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +m_fmAscent;
         else if( m_bMultiRow )
	    if(( h - m_iOutTextHeight*m_nbLines )>0)
		m_iTextY = ( h - m_iOutTextHeight*m_nbLines ) / 2 +m_fmAscent;
	    else
		m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +m_fmAscent;
        else
	    if(( h - m_iOutTextHeight )>0)
		m_iTextY = ( h - m_iOutTextHeight ) / 2 +m_fmAscent;
	    else
		m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +m_fmAscent;
	break;
    }

    if ( a == KSpreadCell::Undefined )
    {
	if ( m_bValue || m_bDate || m_bTime)
	    a = KSpreadCell::Right;
	else
	    a = KSpreadCell::Left;
    }
    if(m_pTable->getShowFormular())
	a = KSpreadCell::Left;

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

void KSpreadCell::textSize( QPainter &_paint )
{
    QFontMetrics fm = _paint.fontMetrics();
    // Horizontal text ?
    if( !m_bVerticalText && !m_rotateAngle )
    {
        m_iOutTextWidth = fm.width( m_strOutText );
        m_iOutTextHeight = fm.ascent() + fm.descent();
    }
    // Rotated text ?
    else if( m_rotateAngle != 0 )
    {
        m_iOutTextHeight = static_cast<int>(std::cos(m_rotateAngle*M_PI/180)*(fm.ascent() + fm.descent())+abs((int)(fm.width( m_strOutText )*sin(m_rotateAngle*M_PI/180))));
        m_iOutTextWidth = static_cast<int>(std::abs((int)(sin(m_rotateAngle*M_PI/180)*(fm.ascent() + fm.descent())))+fm.width( m_strOutText )*cos(m_rotateAngle*M_PI/180));
        //kdDebug(36001)<<"m_iOutTextWidth"<<m_iOutTextWidth<<"m_iOutTextHeight"<<m_iOutTextHeight<<endl;
    }
    // Vertical text ?
    else
    {
        m_iOutTextWidth = fm.width( m_strOutText.at(0));
        m_iOutTextHeight = (fm.ascent() + fm.descent())*(m_strOutText.length());
    }

    m_fmAscent=fm.ascent();
}

void KSpreadCell::conditionAlign(QPainter &_paint,int _col,int _row)
{
    KSpreadConditional *tmpCondition = 0;

    if( m_conditionIsTrue && !m_pTable->getShowFormular() )
    {
        switch(m_numberOfCond)
        {
	case 0:
	    tmpCondition=m_firstCondition;
	    break;
	case 1:
	    tmpCondition=m_secondCondition;
	    break;
	case 2:
	    tmpCondition=m_thirdCondition;
	    break;
	}
	_paint.setFont( tmpCondition->fontcond );
    }
    else
    {
	_paint.setFont( m_textFont );
    }

    textSize(_paint);

    offsetAlign(_col,_row);
}

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
  // ### David: Ouch ! Argl.
  //
  // ############# Torben: Do not replace stuff in strings.
  //
  // ###### David: we should use KLocale's conversion (there is a method there
  // for understanding numbers typed the localised way) for each number the
  // user enters, not once the full formula is set up, then ?
  // I don't see how we can do that...
  // Or do you see kscript parsing localized values ?
  //
  // Oh, Excel uses ';' to separate function arguments (at least when
  // the decimal separator is ','), so that it can process a formula with numbers
  // using ',' as a decimal separator...
  // Sounds like kscript should have configurable argument separator...
  //
  /*QString sDelocalizedText ( m_strText );
  int pos=0;
  while ( ( pos = sDelocalizedText.find( decimal_point, pos ) ) >= 0 )
    sDelocalizedText.replace( pos++, 1, "." );
  // At least,  =2,5+3,2  is turned into =2.5+3.2, which can get parsed...
  */
  m_pCode = m_pTable->doc()->interpreter()->parse( context, m_pTable, /*sDelocalizedText*/m_strText, m_lstDepends );
  // Did a syntax error occur ?
  if ( context.exception() )
  {
    clearFormular();

    m_bError = true;
    m_strFormularOut = "####";
    m_bBool = false;
    m_bValue = false;
    m_bDate=false;
    m_bTime=false;
    m_dValue = 0.0;
    m_bLayoutDirtyFlag = true;
    DO_UPDATE;
    QString tmp(i18n("Error in cell %1\n\n"));
    tmp = tmp.arg( util_cellName( m_pTable, m_iColumn, m_iRow ) );
    tmp += context.exception()->toString( context );
    KMessageBox::error( (QWidget*)0L, tmp);
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
  if ( m_bProgressFlag )
  {
    kdError(36002) << "ERROR: Circle" << endl;
    m_bError = true;
    m_bValue = false;
    m_bBool = false;
    m_bDate =false;
    m_bTime=false;
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
    kdDebug(36002) << util_cellName( m_iColumn, m_iRow ) << " calc() Looking into dependencies..." << endl;
    KSpreadDepend *dep;
    for ( dep = m_lstDepends.first(); dep != 0L; dep = m_lstDepends.next() )
    {
      // Dependency to a rectangular area
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
	    if ( !cell->calc( _makedepend ) )
	    {
	      m_strFormularOut = "####";
	      m_bValue = false;
	      m_bBool = false;
              m_bDate=false;
              m_bTime=false;
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
      else // Dependency to a single cell
      {
	KSpreadCell *cell = dep->m_pTable->cellAt( dep->m_iColumn, dep->m_iRow );
	if ( cell == 0L )
	  return false;
	if ( !cell->calc( _makedepend ) )
	{
	  m_bError = true;
	  m_strFormularOut = "####";
	  m_bValue = false;
	  m_bBool = false;
          m_bDate = false;
          m_bTime=false;
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
  kdDebug(36002) << util_cellName( m_iColumn, m_iRow ) << " calc() Now calculating." << endl;

  KSContext& context = m_pTable->doc()->context();
  if ( !m_pCode || !m_pTable->doc()->interpreter()->evaluate( context, m_pCode, m_pTable ) )
  {
    // If we got an error during evaluation ...
    if ( m_pCode )
    {
      m_bError = true;
      m_strFormularOut = "####";
      m_bValue = false;
      m_bBool = false;
      m_bDate =false;
      m_bTime=false;
      m_bLayoutDirtyFlag = true;
      DO_UPDATE;
      // Print out exception if any
      if ( context.exception() )
      {
	QString tmp(i18n("Error in cell %1\n\n"));
        tmp = tmp.arg( util_cellName( m_pTable, m_iColumn, m_iRow ) );
	tmp += context.exception()->toString( context );
	KMessageBox::error( (QWidget*)0L, tmp);
      }

    }
    // m_bLayoutDirtyFlag = true;
    m_bProgressFlag = false;

    if ( m_style == ST_Select )
    {
	SelectPrivate *s = (SelectPrivate*)m_pPrivate;
	s->parse( m_strFormularOut );
        DO_UPDATE;
    }
    return false;
  }
  else if ( context.value()->type() == KSValue::DoubleType )
  {
    m_dValue = context.value()->doubleValue();
    m_bValue = true;
    m_bBool = false;
    m_bDate =false;
    m_bTime=false;
    // m_strFormularOut.sprintf( "%f", m_dValue );
    m_strFormularOut = KGlobal::locale()->formatNumber( m_dValue );
  }
  else if ( context.value()->type() == KSValue::IntType )
  {
    m_dValue = (double)context.value()->intValue();
    m_bValue = true;
    m_bBool = false;
    m_bDate = false;
    m_bTime=false;
    // m_strFormularOut.sprintf( "%f", m_dValue );
    m_strFormularOut = KGlobal::locale()->formatNumber( m_dValue );
  }
  else if ( context.value()->type() == KSValue::BoolType )
  {
    m_bValue = false;
    m_bBool = true;
    m_bDate =false;
    m_bTime=false;
    m_dValue = context.value()->boolValue() ? 1.0 : 0.0;
    // (David): i18n'ed True and False - hope it's ok
    m_strFormularOut = context.value()->boolValue() ? i18n("True") : i18n("False");
  }
  else
  {
    m_bValue = false;
    m_bBool = false;
    m_bDate=false;
    m_bTime=false;
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

// Used by m_pObscuringCell->paintCell, in the next method
void KSpreadCell::paintCell( const QRect& _rect, QPainter &_painter,
			      int _col, int _row, QRect *_prect )
{
    RowLayout *rl = m_pTable->rowLayout( _row );
    ColumnLayout *cl = m_pTable->columnLayout( _col );
    int tx = m_pTable->columnPos( _col/*, _canvas*/ );
    int ty = m_pTable->rowPos( _row/*, _canvas*/ );

    paintCell( _rect, _painter, tx, ty, _col, _row, cl, rl, _prect );
}

void KSpreadCell::paintCell( const QRect& _rect, QPainter &_painter,
			      int _tx, int _ty,
			      int _col, int _row, ColumnLayout *cl, RowLayout *rl, QRect *_prect )
{
    // If this cell is obscured then draw the obscuring one instead.
    if ( m_pObscuringCell )
    {
	_painter.save();
	m_pObscuringCell->paintCell( _rect, _painter,
				     m_iObscuringCellsColumn, m_iObscuringCellsRow, _prect );
	_painter.restore();
	m_bLayoutDirtyFlag = FALSE;
	return;
    }

    // Need to recalculate ?
    if ( m_bCalcDirtyFlag )
	calc();

    bool old_layoutflag = m_bLayoutDirtyFlag;
    // Need to make a new layout ?
    if ( m_bLayoutDirtyFlag)
	makeLayout( _painter, _col, _row );

    // Determine the dimension of the cell.
    int w = cl->width();
    int h = rl->height();
    if ( m_iExtraXCells )
	w = m_iExtraWidth;
    if ( m_iExtraYCells )
	h = m_iExtraHeight;

    // Do we really need to display this cell ?
    QRect r2( _tx, _ty, w, h );
    if ( !r2.intersects( _rect ) )
	return;

    // Tell our caller where we painted.
    // ## Torben: Where is this used ?
    if ( _prect )
	_prect->setRect( _tx, _ty, w, h );

    bool selected = m_pTable->selectionRect().contains( QPoint( _col, _row ) );

    QColorGroup defaultColorGroup = QApplication::palette().active();

    // Determine the correct background color
    if ( selected )
	_painter.setBackgroundColor( defaultColorGroup.highlight() );
    else
    {
	if ( m_bgColor.isValid() )
	    _painter.setBackgroundColor( m_bgColor );
	else
	    _painter.setBackgroundColor( defaultColorGroup.base() );
    }

    //
    // Determine the pens that should be used for drawing
    // the borders.
    //
    QPen left_pen = leftBorderPen( _col, _row );
    QPen right_pen = rightBorderPen( _col, _row );
    QPen top_pen = topBorderPen( _col, _row );
    QPen bottom_pen = bottomBorderPen( _col, _row );

    // Calculate some offsets so that we know later which
    // rectangle of the cell still needs to be erased.
    int top_offset = 0;
    int bottom_offset = 0;
    int left_offset = 0;
    int right_offset = 0;

    //
    // First draw the default borders so that they dont
    // overwrite any other border.
    //
    if ( left_pen.style() == Qt::NoPen )
    {
	if( table()->getShowGrid() )
        {
	    left_offset = 1;

	    QPen t = m_pTable->cellAt( _col, _row - 1 )->leftBorderPen( _col, _row - 1 );
	    QPen b = m_pTable->cellAt( _col, _row + 1 )->leftBorderPen( _col, _row + 1 );

	    int dt = 0;
	    if ( t.style() != Qt::NoPen )
		dt = 1;
	    int db = 0;
	    if ( b.style() != Qt::NoPen )
		db = 1;

	    _painter.setPen( table()->doc()->defaultGridPen() );
	    _painter.drawLine( _tx, _ty + dt, _tx, _ty + h - db );
	}
    }
    if ( top_pen.style() == Qt::NoPen )
    {
	if( table()->getShowGrid() )
        {
	    top_offset = 1;

	    QPen l = m_pTable->cellAt( _col, _row - 1 )->leftBorderPen( _col, _row - 1 );
	    QPen r = m_pTable->cellAt( _col, _row - 1 )->rightBorderPen( _col, _row - 1 );

	    int dl = 0;
	    if ( l.style() != Qt::NoPen )
		dl = ( l.width() - 1 ) / 2 + 1;
	    int dr = 0;
	    if ( r.style() != Qt::NoPen )
		dr = ( r.width() - 1 ) / 2 + 1; //  + ( ( r.width() - 1 ) % 2 );

	    _painter.setPen( table()->doc()->defaultGridPen() );
	    _painter.drawLine( _tx + dl, _ty, _tx + w - dr, _ty );
	}
    }

    //
    // Now draw the borders which where set by the user.
    //
    if ( left_pen.style() != Qt::NoPen )
    {
	int top = ( QMAX( 0, -1 + (int)top_pen.width() ) ) / 2 + ( ( QMAX( 0, -1 + (int)top_pen.width() ) ) % 2 );
	int bottom = ( QMAX( 0, -1 + (int)bottom_pen.width() ) ) / 2 + 1;

	_painter.setPen( left_pen );
	_painter.drawLine( _tx, _ty - top, _tx, _ty + h + bottom );

	left_offset = left_pen.width() - ( left_pen.width() / 2 );
    }
    if ( right_pen.style() != Qt::NoPen )
    {
	int top = ( QMAX( 0, -1 + (int)top_pen.width() ) ) / 2 +  ( ( QMAX( 0, -1 + (int)top_pen.width() ) ) % 2 );
	int bottom = ( QMAX( 0, -1 + (int)bottom_pen.width() ) ) / 2 + 1;

	_painter.setPen( right_pen );
	_painter.drawLine( w + _tx, _ty - top, w + _tx, _ty + h + bottom );

	right_offset = right_pen.width() / 2;
    }
    if ( top_pen.style() != Qt::NoPen )
    {
	_painter.setPen( top_pen );
	_painter.drawLine( _tx, _ty, _tx + w, _ty );

	top_offset = top_pen.width() - ( top_pen.width() / 2 );
    }
    if ( bottom_pen.style() != Qt::NoPen )
    {
	_painter.setPen( bottom_pen );
	_painter.drawLine( _tx, h + _ty, _tx + w, h + _ty );

	bottom_offset = bottom_pen.width() / 2;
    }

    // Erase the background of the cell.
    _painter.eraseRect( _tx + left_offset, _ty + top_offset,
			w - left_offset - right_offset,
			h - top_offset - bottom_offset );

    // Draw a background brush
    if( m_backGroundBrush.style() != Qt::NoBrush )
    {
	_painter.fillRect( _tx + left_offset, _ty + top_offset,
			   w - left_offset - right_offset,
			   h - top_offset - bottom_offset,
			   backGroundBrush( _col, _row ) );
    }

    //
    // Draw diagonal borders.
    //
    if ( m_fallDiagonalPen.style() != Qt::NoPen )
    {
	_painter.setPen( m_fallDiagonalPen );
	_painter.drawLine( _tx, _ty, _tx + w, _ty + h );
    }
    if ( m_goUpDiagonalPen.style() != Qt::NoPen )
    {
	_painter.setPen( m_goUpDiagonalPen );
	_painter.drawLine( _tx, _ty + h , _tx + w, _ty );
    }

    // Point the little corner if there is a comment attached
    // to this cell.
    if( !m_strComment.isEmpty())
    {
	QPointArray point( 3 );
	point.setPoint( 0,_tx + w - 10, _ty );
	point.setPoint( 1, _tx + w,_ty );
	point.setPoint( 2, _tx + w,_ty + 10 );
	_painter.setBrush( QBrush(Qt::red  ) );
	_painter.setPen( Qt::NoPen );
	_painter.drawPolygon( point );
    }

    /**
     * Modification for drawing the button
     */
    if ( m_style == KSpreadCell::ST_Button )
    {
	QBrush fill( Qt::lightGray );
	QApplication::style().drawButton( &_painter, _tx + 1, _ty + 1,
					  w - 1, h - 1,
					  defaultColorGroup, selected, &fill );
    }
    /**
     * Modification for drawing the combo box
     */
    else if ( m_style == KSpreadCell::ST_Select )
    {
	QApplication::style().drawComboButton(  &_painter, _tx + 1, _ty + 1,
						w - 1, h - 1,
						defaultColorGroup, selected );
    }

    /**
     * QML ?
     */
    if ( m_pQML )
    {
	_painter.save();
	m_pQML->draw( &_painter, _tx, _ty, QRegion( QRect( _tx, _ty, w, h ) ), defaultColorGroup, 0 );
	_painter.restore();
    }
    /**
     * Visual Formula ?
     */
    else if ( m_pVisualFormula )
    {
	_painter.save();
	_painter.setPen( m_textPen/*.color()??*/ );
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
	    p.setColor( defaultColorGroup.highlightedText() );
	    _painter.setPen( p );
	}
	else
	    _painter.setPen( m_textPen );

	// #### Torben: This looks like duplication to me
	verifyCondition();
	if(m_conditionIsTrue && !m_pTable->getShowFormular())
        {
	    KSpreadConditional *tmpCondition=0;
	    switch(m_numberOfCond)
	    {
	    case 0:
		tmpCondition=m_firstCondition;
		break;
	    case 1:
		tmpCondition=m_secondCondition;
		break;
	    case 2:
		tmpCondition=m_thirdCondition;
		break;
	    }
	    _painter.setFont( tmpCondition->fontcond );
	    m_textPen.setColor( tmpCondition->colorcond );
        }
	else
        {
	    _painter.setFont( m_textFont );
	    if( m_bValue && !m_pTable->getShowFormular() )
	    {
		double v = m_dValue * m_dFaktor;
                if ( floatColor( _col, _row) == KSpreadCell::NegRed && v < 0.0 && !m_pTable->getShowFormular() )
		    m_textPen.setColor(Qt::red);
                else
		    m_textPen.setColor( textColor( _col, _row) );
	    }
	    else
                m_textPen.setColor( textColor( _col, _row) );
	}

	//_painter.setFont( m_textFont );
	conditionAlign( _painter, _col, _row );

	if ( !m_bMultiRow && !m_bVerticalText && !m_rotateAngle)
	    _painter.drawText( _tx + m_iTextX, _ty + m_iTextY, m_strOutText );
	else if( m_rotateAngle!=0)
        {
	    int angle=m_rotateAngle;
	    QFontMetrics fm = _painter.fontMetrics();
	    _painter.rotate(angle);
	    int x;
	    if(angle>0)
		x=_tx + m_iTextX;
	    else
		x=static_cast<int>(_tx + m_iTextX -(fm.descent() + fm.ascent())*sin(angle*M_PI/180));
	    int y;
	    if(angle>0)
		y=_ty + m_iTextY;
	    else
		y=_ty + m_iTextY+m_iOutTextHeight;
	    _painter.drawText( x*cos(angle*M_PI/180)+y*sin(angle*M_PI/180),
			       -x*sin(angle*M_PI/180) + y*cos(angle*M_PI/180) , m_strOutText );
	    _painter.rotate(-angle);
	}
	else if( m_bMultiRow && !m_bVerticalText)
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
		    if ( m_bValue || m_bDate || m_bTime)
			a = KSpreadCell::Right;
		    else
			a = KSpreadCell::Left;
		}
		if(m_pTable->getShowFormular())
		    a = KSpreadCell::Left;

		// #### Torben: This looks duplicated for me
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
	else if( m_bVerticalText)
        {
	    QString t;
	    int i=0;
	    int dy = 0;
	    int dx = 0;
	    int j=0;
	    QFontMetrics fm = _painter.fontMetrics();
	    do
	    {
		i=m_strOutText.length();
		t=m_strOutText.at(j);
		int a = m_eAlign;
		if ( a == KSpreadCell::Undefined )
	        {
		    if ( m_bValue || m_bDate ||m_bTime)
			a = KSpreadCell::Right;
		    else
			a = KSpreadCell::Left;
		}
		if(m_pTable->getShowFormular())
		    a = KSpreadCell::Left;

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
		j++;
	    }
	    while ( j != i );
	}
    }

    // Draw page borders
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
    // ###### Torben: This looks unbelievable bad!

  if ( m_bCalcDirtyFlag )
    calc();

  if ( m_bLayoutDirtyFlag)
    makeLayout( _painter, _col, _row );

  if ( !_only_left && !_only_top && m_bgColor.isValid() )
  {
    _painter.setBackgroundColor( m_bgColor );
    _painter.eraseRect( _tx, _ty, cl->width(), rl->height() );
  }

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
      if( m_backGroundBrush.style()!= Qt::NoBrush)
        {
        int left=leftBorderWidth( _col, _row) + BORDER_SPACE;
        int top=topBorderWidth(_col,_row) + BORDER_SPACE;
        _painter.setPen(Qt::NoPen);
        _painter.setBrush(m_backGroundBrush);
        _painter.drawRect( _tx + left, _ty + top,
                cl->width()-left-BORDER_SPACE, rl->height() - top - BORDER_SPACE);
        }

    }
  if ( !_only_top && !_only_left )
    if ( !m_strOutText.isEmpty() )
    {
      _painter.setPen( m_textPen );
      verifyCondition();
      if(m_conditionIsTrue && !m_pTable->getShowFormular())
      	{
        KSpreadConditional *tmpCondition=0;
        switch(m_numberOfCond)
		{
		case 0:
			tmpCondition=m_firstCondition;
			break;
		case 1:
			tmpCondition=m_secondCondition;
			break;
		case 2:
			tmpCondition=m_thirdCondition;
			break;
		}

        _painter.setFont( tmpCondition->fontcond );
      	}
      else
        _painter.setFont( m_textFont );
      conditionAlign(_painter,_col,_row);
      _painter.drawText( _tx + m_iTextX, _ty + m_iTextY, m_strOutText );
    }
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

///////////////////////////////////////////
//
// Borders
//
///////////////////////////////////////////

void KSpreadCell::setLeftBorderPen( const QPen& p )
{
    KSpreadCell* cell = m_pTable->cellAt( column() - 1, row() );
    if ( cell && cell->hasProperty( PRightBorder ) )
	cell->clearProperty( PRightBorder );

    KSpreadLayout::setLeftBorderPen( p );
}

void KSpreadCell::setTopBorderPen( const QPen& p )
{
    KSpreadCell* cell = m_pTable->cellAt( column(), row() - 1 );
    if ( cell && cell->hasProperty( PBottomBorder ) )
	cell->clearProperty( PBottomBorder );

    KSpreadLayout::setTopBorderPen( p );
}

void KSpreadCell::setRightBorderPen( const QPen& p )
{
    KSpreadCell* cell = m_pTable->cellAt( column() + 1, row() );
    if ( cell && cell->hasProperty( PLeftBorder ) )
	cell->clearProperty( PLeftBorder );

    KSpreadLayout::setRightBorderPen( p );
}

void KSpreadCell::setBottomBorderPen( const QPen& p )
{
    KSpreadCell* cell = m_pTable->cellAt( column(), row() + 1 );
    if ( cell && cell->hasProperty( PTopBorder ) )
	cell->clearProperty( PTopBorder );

    KSpreadLayout::setBottomBorderPen( p );
}

const QPen& KSpreadCell::rightBorderPen( int _col, int _row ) const
{
    if ( !hasProperty( PRightBorder ) )
    {
	KSpreadCell * cell = m_pTable->cellAt( _col + 1, _row );
	if ( cell->hasProperty( PLeftBorder ) )
	    return cell->leftBorderPen( _col + 1, _row );
    }

    return KSpreadLayout::rightBorderPen( _col, _row );
}

const QPen& KSpreadCell::leftBorderPen( int _col, int _row ) const
{
    if ( !hasProperty( PLeftBorder ) )
    {
	KSpreadCell * cell = m_pTable->cellAt( _col - 1, _row );
	if ( cell->hasProperty( PRightBorder ) )
	    return cell->rightBorderPen( _col - 1, _row );
    }

    return KSpreadLayout::leftBorderPen( _col, _row );
}

const QPen& KSpreadCell::bottomBorderPen( int _col, int _row ) const
{
    if ( !hasProperty( PBottomBorder ) )
    {
	KSpreadCell * cell = m_pTable->cellAt( _col, _row + 1 );
	if ( cell->hasProperty( PTopBorder ) )
	    return cell->topBorderPen( _col, _row + 1 );
    }

    return KSpreadLayout::bottomBorderPen( _col, _row );
}

const QPen& KSpreadCell::topBorderPen( int _col, int _row ) const
{
    if ( !hasProperty( PTopBorder ) )
    {
	KSpreadCell * cell = m_pTable->cellAt( _col, _row - 1 );
	if ( cell->hasProperty( PBottomBorder ) )
	    return cell->bottomBorderPen( _col, _row - 1 );
    }

    return KSpreadLayout::topBorderPen( _col, _row );
}

///////////////////////////////////////////
//
// Precision
//
///////////////////////////////////////////

void KSpreadCell::incPrecision()
{
  if ( !isValue() )
    return;

  if ( m_iPrecision == -1 )
  {
    int pos = m_strOutText.find(decimal_point);
    if ( pos == -1 )
      m_iPrecision = 1;
    else
    {
      m_iPrecision = m_strOutText.length() - pos;
      if ( m_iPrecision < 0 )
        m_iPrecision = 0;
    }
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

void KSpreadCell::setComment( const QString& c )
{
    m_strComment = c;
    m_bLayoutDirtyFlag = TRUE;
}

QString KSpreadCell::comment() const
{
    return m_strComment;
}

void KSpreadCell::setCellText( const QString& _text, bool updateDepends )
{
  m_bError = false;
  m_strText = _text;

  if (updateDepends)
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
    checkFormat(true);
    if ( !m_pTable->isLoading() )
	if ( !makeFormular() )
	    kdError(36002) << "ERROR: Syntax ERROR" << endl;
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
    m_bDate=false;
    m_bTime=false;
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
    m_bDate = false;
    m_bTime=false;
    m_bLayoutDirtyFlag = true;
    m_content = VisualFormula;
  }
  /**
   * Some numeric value or a string.
   */
  else
  {
    m_content = Text;
    // Find out what it is
    checkValue();

    m_bLayoutDirtyFlag = true;

  }

  /**
   *  Special handling for selection boxes
   */
  if ( m_style == ST_Select && !m_pTable->isLoading() )
  {
      if ( m_bCalcDirtyFlag )
	  calc();
      // if ( m_bLayoutDirtyFlag )
      // makeLayout( m_pTable->painter(), column(), row() );

      SelectPrivate *s = (SelectPrivate*)m_pPrivate;
      if ( m_content == Formula )
	  s->parse( m_strFormularOut );
      else
	  s->parse( m_strText );
      printf("SELECT %s\n", s->text().latin1() );
      checkValue();
      // m_bLayoutDirtyFlag = true;
  }

  if ( updateDepends )
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
    m_bDate =false;
    m_bTime=false;
    m_dValue = _d;
    m_bLayoutDirtyFlag = true;
    m_content = Text;

    // Do not update formulas and stuff here
    if ( !m_pTable->isLoading() )
	update();
}

void KSpreadCell::update()
{
    kdDebug(36002) << util_cellName( m_iColumn, m_iRow ) << " update" << endl;
    if ( m_pObscuringCell )
    {
	m_pObscuringCell->setLayoutDirtyFlag();
	m_pObscuringCell->setDisplayDirtyFlag();
	m_pTable->updateCell( m_pObscuringCell, m_pObscuringCell->column(), m_pObscuringCell->row() );
    }

    bool b_update_begin = m_bDisplayDirtyFlag;
    m_bDisplayDirtyFlag = true;

    updateDepending();

    if ( !b_update_begin && m_bDisplayDirtyFlag )
	m_pTable->updateCell( this, m_iColumn, m_iRow );
}

void KSpreadCell::updateDepending()
{
    kdDebug(36002) << util_cellName( m_iColumn, m_iRow ) << " updateDepending" << endl;

    // Every cell that references us must set its calc dirty flag
    QListIterator<KSpreadTable> it( m_pTable->map()->tableList() );
    for( ; it.current(); ++it )
    {
	KSpreadCell* c = it.current()->firstCell();
	for( ; c; c = c->nextCell() )
	    if ( c != this )
		c->setCalcDirtyFlag( m_pTable, m_iColumn, m_iRow );
    }

    // Recalculate every cell with calc dirty flag
    QListIterator<KSpreadTable> it2( m_pTable->map()->tableList() );
    for( ; it2.current(); ++it2 )
    {
	KSpreadCell* c = it2.current()->firstCell();
	for( ; c; c = c->nextCell() )
	    c->calc( TRUE );
    }
    kdDebug(36002) << util_cellName( m_iColumn, m_iRow ) << " updateDepending done" << endl;

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
    m_bValue = false;
    m_dValue = 0;
    m_bBool = false;
    m_bDate = false;
    m_bTime=false;
    // If the input is empty, we dont have a value
    if ( m_strText.isEmpty() )
    {
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
      return;
    }

    // Test for boolean
    if ( strcasecmp( p, "true") == 0 )
    {
      m_dValue = 1.0;
      m_bBool = true;
      return;
    }
    else if ( strcasecmp( p, "false" ) == 0 )
    {
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
        {
	m_dValue = atof( ptext );
        return;
        }
    QString tmp;
    QTime tmpTime;
    int pos;
    QString stringPm=i18n("pm");
    QString stringAm=i18n("am");
    bool valid=false;
    if((tmpTime=KGlobal::locale()->readTime(m_strText)).isValid())
        {
        valid=true;
        }
    else if(KGlobal::locale()->use12Clock())
    {
    if((pos=m_strText.find(stringPm))!=-1)
        {
         tmp=m_strText.mid(0,m_strText.length()-stringPm.length());
         tmp=tmp.simplifyWhiteSpace();
         if((tmpTime=KGlobal::locale()->readTime(tmp+" "+stringPm)).isValid())
                {
                valid=true;
                }
         else if((tmpTime=KGlobal::locale()->readTime(tmp+":00 "+stringPm)).isValid())
                {
                valid=true;
                }
        }
    else if((pos=m_strText.find(stringAm))!=-1)
        {
         tmp=m_strText.mid(0,m_strText.length()-stringAm.length());
         tmp=tmp.simplifyWhiteSpace();
         if((tmpTime=KGlobal::locale()->readTime(tmp+" "+stringAm)).isValid())
                {
                valid=true;
                }
         else if((tmpTime=KGlobal::locale()->readTime(tmp+":00 "+stringAm)).isValid())
                {
                valid=true;
                }
        }
    }
    if(valid)
        {
        m_bTime = true;
        m_dValue = 0;
        if( m_eFormatNumber!=SecondeTime)
                m_eFormatNumber=Time;
        m_Time=tmpTime;
        m_strText=KGlobal::locale()->formatTime(m_Time,true);
        return;
        }

    QDate tmpDate;
    if((tmpDate=KGlobal::locale()->readDate(m_strText)).isValid())
        {
        m_bDate = true;
        m_dValue = 0;
        if( m_eFormatNumber!=TextDate)
                m_eFormatNumber=ShortDate;
        m_Date=tmpDate;
        m_strText=KGlobal::locale()->formatDate(m_Date,true); //short format date
        return;
        }
    checkFormat();
    /* if ( old_value != bValue )
	displayDirtyFlag = TRUE; */
}

void KSpreadCell::checkFormat(bool _formular)
{
   QString tmpText=m_strText;
   double val=0;
   bool ok=false;
   int pos=0;
   QString tmp;
   if(_formular)
        tmpText=tmpText.right(tmpText.length()-1);
   //test if text is a percent value
    if(tmpText.at(tmpText.length()-1)=='%')
        {
        tmp=tmpText.left(tmpText.length()-1);
        tmp=tmp.simplifyWhiteSpace();
        val=tmp.toDouble(&ok);
        if(ok)
                {
                m_bValue=true;
                m_dValue=val;
                m_eFormatNumber=Percentage;
                m_dValue/=100.0;
                m_strText=tmp.setNum(m_dValue);
                setFaktor(100.0);
                setPrecision(2);
                if(_formular)
                        m_strText="="+m_strText;
                return;
                }

        }

    if((pos=tmpText.find(KGlobal::locale()->currencySymbol()))!=-1)
        {
        if(pos==0) // example $ 154.545
                {
                tmp=tmpText.right(tmpText.length()-1);
                tmp=tmp.simplifyWhiteSpace();
                val=tmp.toDouble(&ok);
                if(ok)
                        {
                        m_bValue=true;
                        m_dValue=val;
                        m_eFormatNumber=Money;
                        m_strText=tmp.setNum(m_dValue);
                        if(_formular)
                                m_strText="="+m_strText;
                        setFaktor(1.0);
                        setPrecision(2);
                        return;
                        }
                }
        else if(pos==(tmpText.length()-1)) //example 125.55 F
                {
                tmp=tmpText.left(tmpText.length()-1);
                tmp=tmp.simplifyWhiteSpace();
                val=tmp.toDouble(&ok);
                if(ok)
                        {
                        m_bValue=true;
                        m_dValue=val;
                        m_eFormatNumber=Money;
                        m_strText=tmp.setNum(m_dValue);
                        if(_formular)
                                m_strText="="+m_strText;
                        setFaktor(1.0);
                        setPrecision(2);
                        return;
                        }
                }
        }
    val=m_strText.toDouble(&ok);
    if(ok)
        {
        if(m_strText.contains('E')||m_strText.contains('e'))
                {
                m_dValue=val;
                m_bValue = true;
                m_eFormatNumber=Scientific;
                m_strText= QString::number(val, 'f',8);

                int i = m_strText.length();
	        bool bFinished = FALSE;
	        while ( !bFinished && i > 0 )
	        {
		QChar ch = m_strText[ i - 1 ];
		if ( ch == '0' )
		    m_strText.truncate( --i );
		else
	        {
		    bFinished = TRUE;
		    if ( ch == '.' )
			m_strText.truncate( --i );
		}
                }
                setFaktor(1.0);
                if(_formular)
                        m_strText="="+m_strText;
                return;
                }
        }

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
	KSpreadCell* c = it.current()->firstCell();
	for( ; c; c = c->nextCell() )
	    c->setCalcDirtyFlag( m_pTable, m_iColumn, m_iRow );
    }
  }
}

QDomElement KSpreadCell::save( QDomDocument& doc, int _x_offset, int _y_offset )
{
    // Save the position of this cell
    QDomElement cell = doc.createElement( "cell" );
    cell.setAttribute( "row", m_iRow - _y_offset );
    cell.setAttribute( "column", m_iColumn - _x_offset );

    if ( !action().isEmpty() )
	cell.setAttribute( "action", action() );

    //
    // Save the formatting information
    //
    QDomElement format = doc.createElement( "format" );
    cell.appendChild( format );
    format.setAttribute( "align", (int)m_eAlign );
    format.setAttribute( "alignY", (int)m_eAlignY );

    if ( m_bgColor.isValid() )
	format.setAttribute( "bgcolor", m_bgColor.name() );
    if ( m_bMultiRow )
	format.setAttribute( "multirow", "yes" );
    if ( m_style )
	format.setAttribute( "style", (int)m_style );
    if ( m_bVerticalText )
	format.setAttribute( "verticaltext", "yes" );
    if ( isForceExtraCells() )
    {
	format.setAttribute( "colspan", extraXCells() );
	format.setAttribute( "rowspan", extraYCells() );
    }

    format.setAttribute( "precision", m_iPrecision );
    if ( !m_strPrefix.isEmpty() )
	format.setAttribute( "prefix", m_strPrefix );
    if ( !m_strPostfix.isEmpty() )
	format.setAttribute( "postfix", m_strPostfix );

    format.setAttribute( "float", (int)m_eFloatFormat );
    format.setAttribute( "floatcolor", (int)m_eFloatColor );
    format.setAttribute( "faktor", m_dFaktor );

    format.setAttribute( "format",(int) getFormatNumber() );

    if( m_rotateAngle != 0 )
	format.setAttribute( "angle", m_rotateAngle );

    // if ( m_textFont != m_pTable->defaultCell()->textFont() )
    format.appendChild( createElement( "font", m_textFont, doc ) );

    // if ( m_textPen != m_pTable->defaultCell()->textPen() )
    {
	// if( m_conditionIsTrue )
        // {
	// m_textPen.setColor( m_textColor );
	// }
	format.appendChild( createElement( "pen", m_textPen, doc ) );
    }
    format.setAttribute( "brushcolor", m_backGroundBrush.color().name() );
    format.setAttribute( "brushstyle",(int)m_backGroundBrush.style() );

    QDomElement left = doc.createElement( "left-border" );
    left.appendChild( createElement( "pen", m_leftBorderPen, doc ) );
    format.appendChild( left );

    QDomElement top = doc.createElement( "top-border" );
    top.appendChild( createElement( "pen", m_topBorderPen, doc ) );
    format.appendChild( top );

    QDomElement right = doc.createElement( "right-border" );
    right.appendChild( createElement( "pen", m_rightBorderPen, doc ) );
    format.appendChild( right );

    QDomElement bottom = doc.createElement( "bottom-border" );
    bottom.appendChild( createElement( "pen", m_bottomBorderPen, doc ) );
    format.appendChild( bottom );

    QDomElement fallDiagonal  = doc.createElement( "fall-diagonal" );
    fallDiagonal.appendChild( createElement( "pen", m_fallDiagonalPen, doc ) );
    format.appendChild( fallDiagonal );

    QDomElement goUpDiagonal = doc.createElement( "up-diagonal" );
    goUpDiagonal.appendChild( createElement( "pen", m_goUpDiagonalPen, doc ) );
    format.appendChild( goUpDiagonal );

    if(m_rotateAngle!=0)
	format.setAttribute( "angle",m_rotateAngle);

    if((m_firstCondition!=0)||(m_secondCondition!=0)||(m_thirdCondition!=0))
    {
  	QDomElement condition = doc.createElement("condition");

  	if(m_firstCondition!=0)
        {
	    QDomElement first=doc.createElement("first");
	    first.setAttribute("cond",(int)m_firstCondition->m_cond);
	    first.setAttribute("val1",m_firstCondition->val1);
	    first.setAttribute("val2",m_firstCondition->val2);
	    first.setAttribute("color",m_firstCondition->colorcond.name());
	    first.appendChild( createElement( "font", m_firstCondition->fontcond, doc ) );

	    condition.appendChild(first);
	}
  	if(m_secondCondition!=0)
        {
	    QDomElement second=doc.createElement("second");
	    second.setAttribute("cond",(int)m_secondCondition->m_cond);
	    second.setAttribute("val1",m_secondCondition->val1);
	    second.setAttribute("val2",m_secondCondition->val2);
	    second.setAttribute("color",m_secondCondition->colorcond.name());
	    second.appendChild( createElement( "font", m_secondCondition->fontcond, doc ) );


	    condition.appendChild(second);
	}
	if(m_thirdCondition!=0)
        {
	    QDomElement third=doc.createElement("third");
	    third.setAttribute("cond",(int)m_thirdCondition->m_cond);
	    third.setAttribute("val1",m_thirdCondition->val1);
	    third.setAttribute("val2",m_thirdCondition->val2);
	    third.setAttribute("color",m_thirdCondition->colorcond.name());
	    third.appendChild( createElement( "font", m_thirdCondition->fontcond, doc ) );


	    condition.appendChild(third);
	}
  	cell.appendChild( condition );
    }
    if ( !m_strComment.isEmpty() )
    {
        QDomElement comment = doc.createElement( "comment" );
        comment.appendChild( doc.createCDATASection( m_strComment ) );
        cell.appendChild( comment );
    }

    //
    // Save the text
    //
    if ( !m_strText.isEmpty() )
    {
	// Formulars need to be encoded to enshure that they
	// are position independent.
	if ( isFormular() )
        {
	    QDomElement text = doc.createElement( "text" );
	    text.appendChild( doc.createTextNode( encodeFormular() ) );
	    cell.appendChild( text );
	}
	// Have to be saved in some CDATA section because of too many
	// special charatcers.
	else if ( content() == RichText || content() == VisualFormula )
        {
	    QDomElement text = doc.createElement( "text" );
	    text.appendChild( doc.createCDATASection( m_strText ) );
	    cell.appendChild( text );
	}
	else if( (getFormatNumber()==ShortDate || getFormatNumber()==TextDate)&& m_bDate )
        {
	    QDomElement text = doc.createElement( "text" );
	    QString tmp;
	    tmp=tmp.setNum(m_Date.year())+"/"+tmp.setNum(m_Date.month())+"/"+tmp.setNum(m_Date.day());
	    text.appendChild( doc.createTextNode( tmp ) );
	    cell.appendChild( text );
	}
	else if( (getFormatNumber()==Time || getFormatNumber()==SecondeTime)&& m_bTime )
        {
	    QDomElement text = doc.createElement( "text" );
	    QString tmp;
	    tmp=m_Time.toString();
	    text.appendChild( doc.createTextNode( tmp ) );
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

    //
    // First of all determine in which row and column this
    // cell belongs.
    //
    m_iRow = cell.attribute( "row" ).toInt( &ok ) + _yshift;
    if ( !ok ) return false;
    m_iColumn = cell.attribute( "column" ).toInt( &ok ) + _xshift;
    if ( !ok ) return false;

    if ( cell.hasAttribute( "action" ) )
	setAction( cell.attribute("action") );

    // Validation
    if ( m_iRow < 1 || m_iRow > 0xFFFF )
    {
	kdDebug(36001) << "Value out of Range Cell:row=" << m_iRow << endl;
	return false;
    }
    if ( m_iColumn < 1 || m_iColumn > 0xFFFF )
    {
	kdDebug(36001) << "Value out of Range Cell:column=" << m_iColumn << endl;
	return false;
    }

    //
    // Load formatting information.
    //
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
		kdDebug(36001) << "Value out of range Cell::align=" << (unsigned int)a << endl;
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
		kdDebug(36001) << "Value out of range Cell::alignY=" << (unsigned int)a << endl;
		return false;
	    }
	    // Assignment
	    setAlignY( a );
	}

	if ( f.hasAttribute( "bgcolor" ) )
	    setBgColor( QColor( f.attribute( "bgcolor" ) ) );

	if ( f.hasAttribute( "multirow" ) )
	    setMultiRow( true );

	if ( f.hasAttribute( "verticaltext" ) )
	    setVerticalText( true );

	if ( f.hasAttribute( "colspan" ) )
        {
	    int i = f.attribute("colspan").toInt( &ok );
	    if ( !ok ) return false;
	    // Validation
	    if ( i < 0 || i > 0xFFF )
	    {
		kdDebug(36001) << "Value out of range Cell::colspan=" << i << endl;
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
		kdDebug(36001) << "Value out of range Cell::rowspan=" << i << endl;
		return false;
	    }
	    m_iExtraYCells = i;
	    if ( i > 0 )
		m_bForceExtraCells = true;
	}

        if(m_bForceExtraCells)
        {
            forceExtraCells(m_iColumn,m_iRow,m_iExtraXCells,m_iExtraYCells);
        }

	if ( f.hasAttribute( "precision" ) )
        {
	    int i = f.attribute("precision").toInt( &ok );
	    if ( i < -1 )
	    {
		kdDebug(36001) << "Value out of range Cell::precision=" << i << endl;
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
		kdDebug(36001) << "Value out of range Cell::float=" << (unsigned int)a << endl;
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
		kdDebug(36001) << "Value out of range Cell::floatcolor=" << (unsigned int)a << endl;
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

        if ( f.hasAttribute( "format" ) )
        {
	    m_eFormatNumber=(formatNumber)f.attribute("format").toInt( &ok );
	    if ( !ok ) return false;
	}

        if ( f.hasAttribute( "brushcolor" ) )
	    setBackGroundBrushColor( QColor( f.attribute( "brushcolor" ) ) );

        if ( f.hasAttribute( "brushstyle" ) )
        {
	    setBackGroundBrushStyle((Qt::BrushStyle) f.attribute( "brushstyle" ).toInt(&ok)  );
	    if(!ok) return false;
	}

	QDomElement pen = f.namedItem( "pen" ).toElement();
	if ( !pen.isNull() )
	    setTextPen( toPen(pen) );

	QDomElement font = f.namedItem( "font" ).toElement();
	if ( !font.isNull() )
	    setTextFont( toFont(font) );

  	QDomElement underline = f.namedItem( "underline" ).toElement();
	if ( !underline.isNull() )
        {
	    if ( underline.hasAttribute( "val" ) )
	    {
		setTextFontUnderline((bool)underline.attribute("val").toInt( &ok ));
		if ( !ok ) return false;
	    }
	}

        QDomElement strike = f.namedItem( "strike" ).toElement();
	if ( !strike.isNull() )
        {
	    if ( strike.hasAttribute( "val" ) )
	    {
		setTextFontStrike((bool)strike.attribute("val").toInt( &ok ));
		if ( !ok ) return false;
	    }
	}

	QDomElement left = f.namedItem( "left-border" ).toElement();
	if ( !left.isNull() && pm != NoBorder )
        {
	    QDomElement pen = left.namedItem( "pen" ).toElement();
	    if ( !pen.isNull() )
		setLeftBorderPen( toPen(pen) );
	}

	QDomElement top = f.namedItem( "top-border" ).toElement();
	if ( !top.isNull() && pm != NoBorder )
        {
	    QDomElement pen = top.namedItem( "pen" ).toElement();
	    if ( !pen.isNull() )
		setTopBorderPen( toPen(pen) );
	}

	QDomElement right = f.namedItem( "right-border" ).toElement();
	if ( !right.isNull() && pm != NoBorder )
        {
	    QDomElement pen = right.namedItem( "pen" ).toElement();
	    if ( !pen.isNull() )
		setRightBorderPen( toPen(pen) );
	}

	QDomElement bottom = f.namedItem( "bottom-border" ).toElement();
	if ( !bottom.isNull() && pm != NoBorder )
        {
	    QDomElement pen = bottom.namedItem( "pen" ).toElement();
	    if ( !pen.isNull() )
		setBottomBorderPen( toPen(pen) );
	}

	QDomElement fallDiagonal = f.namedItem( "fall-diagonal" ).toElement();
	if ( !fallDiagonal.isNull() && pm != NoBorder )
        {
	    QDomElement pen = fallDiagonal.namedItem( "pen" ).toElement();
	    if ( !pen.isNull() )
		setFallDiagonalPen( toPen(pen) );
	}

	QDomElement goUpDiagonal = f.namedItem( "up-diagonal" ).toElement();
	if ( !goUpDiagonal.isNull() && pm != NoBorder )
        {
	    QDomElement pen = goUpDiagonal.namedItem( "pen" ).toElement();
	    if ( !pen.isNull() )
		setGoUpDiagonalPen( toPen(pen) );
	}

	m_strPrefix = f.attribute( "prefix" );
	m_strPostfix = f.attribute( "postfix" );
        if ( f.hasAttribute( "angle" ) )
        {
            setAngle(f.attribute( "angle").toInt( &ok ));
	    if ( !ok )
		return false;
        }
    }

    //
    // Load the condition section of a cell.
    //
    QDomElement condition = cell.namedItem( "condition" ).toElement();
    if ( !condition.isNull())
    {
	QDomElement first = condition.namedItem( "first" ).toElement();
	if(!first.isNull())
        {
	    m_firstCondition=new KSpreadConditional;
	    if ( first.hasAttribute( "cond" ) )
	    {
		m_firstCondition->m_cond =(Conditional) first.attribute("cond").toInt( &ok );
		if ( !ok ) return false;
	    }
	    if(first.hasAttribute("val1"))
	    {
		m_firstCondition->val1 =first.attribute("val1").toDouble( &ok );
		if ( !ok ) return false;
	    }
	    if(first.hasAttribute("val2"))
	    {
		m_firstCondition->val2 =first.attribute("val2").toDouble( &ok );
		if ( !ok ) return false;
	    }
	    if(first.hasAttribute("color"))
	    {
		m_firstCondition->colorcond=QColor(first.attribute( "color"));
	    }
	    QDomElement font = first.namedItem( "font" ).toElement();
	    if ( !font.isNull() )
		m_firstCondition->fontcond=toFont(font) ;

	}

	QDomElement second = condition.namedItem( "second" ).toElement();
	if(!second.isNull())
        {
	    m_secondCondition=new KSpreadConditional;
	    if ( second.hasAttribute( "cond" ) )
	    {
		m_secondCondition->m_cond =(Conditional) second.attribute("cond").toInt( &ok );
		if ( !ok ) return false;
	    }
	    if(second.hasAttribute("val1"))
	    {
		m_secondCondition->val1 =second.attribute("val1").toDouble( &ok );
		if ( !ok ) return false;
	    }
	    if(second.hasAttribute("val2"))
	    {
		m_secondCondition->val2 =second.attribute("val2").toDouble( &ok );
		if ( !ok ) return false;
	    }
	    if(second.hasAttribute("color"))
	    {
		m_secondCondition->colorcond=QColor(second.attribute( "color"));
	    }
	    QDomElement font = second.namedItem( "font" ).toElement();
	    if ( !font.isNull() )
		m_secondCondition->fontcond=toFont(font) ;

	}

	QDomElement third = condition.namedItem( "third" ).toElement();
	if(!third.isNull())
        {
	    m_thirdCondition=new KSpreadConditional;
	    if ( third.hasAttribute( "cond" ) )
	    {
		m_thirdCondition->m_cond =(Conditional) third.attribute("cond").toInt( &ok );
		if ( !ok ) return false;
	    }
	    if(third.hasAttribute("val1"))
	    {
		m_thirdCondition->val1 =third.attribute("val1").toDouble( &ok );
		if ( !ok ) return false;
	    }
	    if(third.hasAttribute("val2"))
	    {
		m_thirdCondition->val2 =third.attribute("val2").toDouble( &ok );
		if ( !ok ) return false;
	    }
	    if(third.hasAttribute("color"))
	    {
		m_thirdCondition->colorcond=QColor(third.attribute( "color"));
	    }
	    QDomElement font = third.namedItem( "font" ).toElement();
	    if ( !font.isNull() )
		m_thirdCondition->fontcond=toFont(font) ;
	}
    }

    //
    // Load the comment
    //
    QDomElement comment = cell.namedItem( "comment" ).toElement();
    if ( !comment.isNull() && ( pm == ::Normal || pm == ::Comment || pm == ::NoBorder ))
    {
        QString t = comment.text();
	//t = t.stripWhiteSpace();
        setComment( t );
    }

    //
    // The real content of the cell is loaded here. It is stored in
    // the "text" tag, which contains either a text or a CDATA section.
    //
    QDomElement text = cell.namedItem( "text" ).toElement();
    if ( !text.isNull() && ( pm == ::Normal || pm == ::Text || pm == ::NoBorder ) )
    {
	QString t = text.text();
	t = t.stripWhiteSpace();
	// A formula like =A1+A2 ?
        if( t[0] == '=' )
        {
	    t = decodeFormular( t, m_iColumn, m_iRow );

            // Set the cell's text, and don't calc dependencies yet (see comment for setCellText)
            setCellText( pasteOperation( t, m_strText, op ), false );
        }
	// A date
        else if( getFormatNumber() == ShortDate || getFormatNumber() == TextDate)
        {
	    int pos;
	    int pos1;
	    int year = -1;
	    int month = -1;
	    int day = -1;
	    pos = t.find('/');
	    year = t.mid(0,pos).toInt();
	    pos1 = t.find('/',pos+1);
	    month = t.mid(pos+1,((pos1-1)-pos)).toInt();
	    day = t.right(t.length()-pos1-1).toInt();
	    m_Date = QDate(year,month,day);
	    if(m_Date.isValid() )
                setCellText( KGlobal::locale()->formatDate( m_Date, true ), false );
	    else
                setCellText( pasteOperation( t, m_strText, op ), false );
        }
	// A Time
        else if( getFormatNumber() == Time || getFormatNumber() == SecondeTime )
        {
	    int hours = -1;
	    int minutes = -1;
	    int second = -1;
	    int pos, pos1;
	    pos = t.find(':');
	    hours = t.mid(0,pos).toInt();
	    pos1 = t.find(':',pos+1);
	    minutes = t.mid(pos+1,((pos1-1)-pos)).toInt();
	    second = t.right(t.length()-pos1-1).toInt();
	    m_Time = QTime(hours,minutes,second);
	    if(m_Time.isValid() )
                setCellText(KGlobal::locale()->formatTime(m_Time,true),false);
	    else
                setCellText( pasteOperation( t, m_strText, op ), false );
        }
	// A Text, QML or a visual formula
        else
        {
	    // Set the cell's text, and don't calc dependencies yet (see comment for setCellText)
	    setCellText( pasteOperation( t, m_strText, op ), false );
        }
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
    return QString::null;

  QPainter p( _w );
  return m_pQML->anchor( &p, QPoint( _x, _y ) );
}

void KSpreadCell::tableDies()
{
    // Avoid unobscuring the cells in the destructor.
    m_iExtraXCells = 0;
    m_iExtraYCells = 0;
    m_nextCell = 0;
    m_previousCell = 0;
}

KSpreadCell::~KSpreadCell()
{
    if ( m_nextCell )
	m_nextCell->setPreviousCell( m_previousCell );
    if ( m_previousCell )
	m_previousCell->setNextCell( m_nextCell );

    if ( m_pPrivate )
	delete m_pPrivate;
    if ( m_pQML )
	delete m_pQML;
    if ( m_pVisualFormula )
	delete m_pVisualFormula;

    // Unobscure cells.
    for( int x = 0; x <= m_iExtraXCells; ++x )
	for( int y = (x == 0) ? 1 : 0; // avoid looking at (+0,+0)
	     y <= m_iExtraYCells; ++y )
    {
        KSpreadCell* cell = m_pTable->cellAt( m_iColumn + x, m_iRow + y );
        if ( cell )
            cell->unobscure();
    }
}

bool KSpreadCell::operator > ( const KSpreadCell & cell ) const
{
  if ( isValue() )
    if ( cell.isValue() )
      return valueDouble() > cell.valueDouble();
    else
      return false; // numbers are always < than texts
  else
    return valueString().compare(cell.valueString()) > 0;
}

bool KSpreadCell::operator < ( const KSpreadCell & cell ) const
{
  if ( isValue() )
    if ( cell.isValue() )
      return valueDouble() < cell.valueDouble();
    else
      return true; // numbers are always < than texts
  else
    return valueString().compare(cell.valueString()) < 0;
}

QDomElement KSpreadCell::createElement( const QString &tagName, const QFont &font, QDomDocument &doc ) const {

    QDomElement e=doc.createElement( tagName );

    e.setAttribute( "family", font.family() );
    e.setAttribute( "size", font.pointSize() );
    e.setAttribute( "weight", font.weight() );
    if ( font.bold() )
	e.setAttribute( "bold", "yes" );
    if ( font.italic() )
	e.setAttribute( "italic", "yes" );
    if ( font.underline() )
    	e.setAttribute( "underline", "yes" );
    if ( font.strikeOut() )
    	e.setAttribute( "strikeout", "yes" );

    return e;
}

QDomElement KSpreadCell::createElement( const QString& tagname, const QPen& pen, QDomDocument &doc ) const {

    QDomElement e=doc.createElement( tagname );
    e.setAttribute( "color", pen.color().name() );
    e.setAttribute( "style", (int)pen.style() );
    e.setAttribute( "width", (int)pen.width() );
    return e;
}

QFont KSpreadCell::toFont(QDomElement &element) const {

    QFont f;
    f.setFamily( element.attribute( "family" ) );

    bool ok;
    f.setPointSize( element.attribute("size").toInt( &ok ) );
    if ( !ok ) return QFont();

    f.setWeight( element.attribute("weight").toInt( &ok ) );
    if ( !ok ) return QFont();

    if ( element.hasAttribute( "italic" ) && element.attribute("italic") == "yes" )
	f.setItalic( TRUE );

    if ( element.hasAttribute( "bold" ) && element.attribute("bold") == "yes" )
	f.setBold( TRUE );

    if ( element.hasAttribute( "underline" ) && element.attribute("underline") == "yes" )
	f.setUnderline( TRUE );

    if ( element.hasAttribute( "strikeout" ) && element.attribute("strikeout") == "yes" )
	f.setStrikeOut( TRUE );

    return f;
}

QPen KSpreadCell::toPen(QDomElement &element) const {

  bool ok;
  QPen p;
  p.setStyle( (Qt::PenStyle)element.attribute("style").toInt( &ok ) );
  if ( !ok ) return QPen();

  p.setWidth( element.attribute("width").toInt( &ok ) );
  if ( !ok ) return QPen();

  p.setColor( QColor( element.attribute("color") ) );

  return p;
}

bool KSpreadCell::isDefault() const
{
    return ( m_iColumn == 0 );
}

/***************************************************
 *
 * SelectPrivate
 *
 ***************************************************/

void SelectPrivate::parse( const QString& _text )
{
    m_lstItems.clear();

    if ( _text.isEmpty() )
	return;

    m_lstItems = QStringList::split( '\\', _text );

    if ( m_iIndex != -1 && m_iIndex < (int)m_lstItems.count() )
    { }
    else if ( m_lstItems.count() > 0 )
	m_iIndex = 0;
    else
	m_iIndex = -1;
}

void SelectPrivate::slotItemSelected( int _id )
{
    m_iIndex = _id;

    m_pCell->setLayoutDirtyFlag();
    m_pCell->checkValue();
    m_pCell->update();

    m_pCell->table()->updateCell( m_pCell, m_pCell->column(), m_pCell->row() );
}

QString SelectPrivate::text() const
{
    if ( m_iIndex == -1 )
	return QString::null;

    return m_lstItems[ m_iIndex ];
}

KSpreadCellPrivate* SelectPrivate::copy( KSpreadCell* cell )
{
    SelectPrivate* p = new SelectPrivate( cell );
    p->m_lstItems = m_lstItems;
    p->m_iIndex = m_iIndex;

    return p;
}


#include "kspread_cell.moc"
