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
#include <qapplication.h>
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
#include <koscript_parsenode.h>

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
  m_style = ST_Normal;
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
  m_bCellTooShort=false;
  m_Validity=0;
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
    setLeftBorderPen(o->leftBorderPen( _column, _row ));
    setTopBorderPen(o->topBorderPen( _column, _row ));
    setBottomBorderPen(o->bottomBorderPen( _column, _row ));
    setRightBorderPen(o->rightBorderPen( _column, _row ));
    setFallDiagonalPen(o->fallDiagonalPen( _column, _row ));
    setGoUpDiagonalPen(o->goUpDiagonalPen( _column, _row ));
    setBackGroundBrush(o->backGroundBrush( _column, _row));

    setPrecision( o->precision( _column, _row ) );
    setPrefix( o->prefix( _column, _row ) );
    setPostfix( o->postfix( _column, _row ) );
    setFloatFormat( o->floatFormat( _column, _row ) );
    setFloatColor( o->floatColor( _column, _row ) );
    setFaktor( o->faktor( _column, _row ) );
    setMultiRow( o->multiRow( _column, _row ) );
    setVerticalText( o->verticalText( _column, _row ) );
    setStyle( o->style());

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
    setComment( o->comment(_column, _row) );
    setAngle( o->getAngle(_column, _row) );
    setFormatNumber( o->getFormatNumber(_column, _row) );
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
  QPen pen( Qt::black,1,Qt::NoPen); // TODO set to QColor() and change painting to use default colors
  QBrush brush( Qt::red,Qt::NoBrush);
  setBottomBorderPen(pen);
  setRightBorderPen(pen);
  setLeftBorderPen(pen);
  setTopBorderPen(pen);
  setFallDiagonalPen(pen);
  setGoUpDiagonalPen(pen);
  setAlign( KSpreadCell::Undefined );
  setAlignY( KSpreadCell::Middle );
  setBackGroundBrush(brush);
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
  setComment("");
  setVerticalText(false);
  setAngle(0);
  setFormatNumber(Number);
  if(m_Validity!=0)
        delete m_Validity;
  m_Validity=0;
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
      m_iExtraWidth = 0;
      m_iExtraHeight = 0;
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

    //int ex = extraXCells();
    //int ey = extraYCells();

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
    //if ( m_bForceExtraCells )
      //  forceExtraCells( col, row, ex, ey );
}

void KSpreadCell::setLayoutDirtyFlag()
{
    m_bLayoutDirtyFlag= TRUE;

    if ( m_pObscuringCell )
        m_pObscuringCell->setLayoutDirtyFlag();
}

bool KSpreadCell::needsPrinting() const
{
    if ( isDefault() )
        return FALSE;

    if ( !m_strText.isEmpty() )
        return TRUE;

    if ( hasProperty( PTopBorder ) || hasProperty( PLeftBorder ) ||
         hasProperty( PRightBorder ) || hasProperty( PBottomBorder ) ||
         hasProperty( PFallDiagonal ) || hasProperty( PGoUpDiagonal ) ||
         hasProperty( PBackgroundBrush ) || hasProperty( PBackgroundColor ) )
        return TRUE;

    return FALSE;
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
    delete popup;
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

    if(m_strText.isEmpty())
        return m_strText;

    char buf[ 2 ];
    buf[ 1 ] = 0;

    bool fix1 = FALSE;
    bool fix2 = FALSE;

    // FIXME (Werner)
    const char *p=m_strText.latin1();
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
                        //used when there is a lower reference cell
                        //example =a1
                        int offset='a'-'A';
                        if ( strlen( buffer ) >= 2 )
                        {
                            if(buffer[0] >= 'A' && buffer[0] <= 'Z')
                                col += 26 * ( buffer[0] - 'A' + 1 );
                            else if( buffer[0]>= 'a' && buffer[0] <= 'z' )
                                 col += 26 * ( buffer[0] - 'A' + 1 -offset );
                            if(buffer[1] >= 'A' && buffer[1] <= 'Z')
                                col += buffer[1] - 'A' + 1;
                            else if( buffer[1]>= 'a' && buffer[1] <= 'z' )
                                 col +=  buffer[1] - 'A' + 1 -offset ;
                        }
                        else {
                            if(buffer[0] >= 'A' && buffer[0] <= 'Z')
                                col += buffer[0] - 'A' + 1;
                            else if( buffer[0]>= 'a' && buffer[0] <= 'z' )
                                col += buffer[0] - 'A' + 1 -offset ;
                        }
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
    setLeftBorderWidth( leftBorderWidth( _col, _row ));
    setTopBorderWidth( topBorderWidth( _col, _row ));
    setTopBorderWidth( topBorderWidth( _col, _row ));
    setFallDiagonalWidth( fallDiagonalWidth( _col, _row) );
    setGoUpDiagonalWidth( goUpDiagonalWidth( _col, _row) );

    m_nbLines = 0;
    m_bCellTooShort=false;
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
        _painter.setPen( textPen(_col,_row).color() );
        _painter.setFont( textFont(_col,_row ));
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
    QPen tmpPen;
    tmpPen.setColor( textColor( _col, _row ) );
    setTextPen(tmpPen);
    m_conditionIsTrue = false;
    tmpPen=textPen(_col,_row);
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
        m_strOutText=createDateFormat();
    }
    else if( isTime() )
    {
        m_strOutText=createTimeFormat();
    }
    else if ( isValue()  )
    {
        // First get some locale information
        if (!decimal_point)
        { // (decimal_point is static)
            decimal_point = locale()->decimalSymbol()[0];
            kdDebug(36001) << "decimal_point is '" << decimal_point.latin1() << "'" << endl;

            if ( decimal_point.isNull() )
                decimal_point = '.';
        }

        // Scale the value as desired by the user.
        double v = m_dValue * faktor(column(),row());

        // Always unsigned ?
        if ( floatFormat( _col, _row ) == KSpreadCell::AlwaysUnsigned && v < 0.0)
            v *= -1.0;

        // Make a string out of it.
        QString localizedNumber = createFormat( v, _col, _row );

        // Remove trailing zeros and the decimal point if necessary
        // unless the number has no decimal point
        if ( precision( _col, _row)== -1 && localizedNumber.find(decimal_point) >= 0 )
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
            else if(localizedNumber.find( locale()->currencySymbol())==(localizedNumber.length()-locale()->currencySymbol().length()))
                start=locale()->currencySymbol().length()+1;
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
        if( !prefix( _col, _row ).isEmpty())
                m_strOutText += prefix( _col, _row )+" ";

        m_strOutText += localizedNumber;

        if( !postfix( _col, _row ).isEmpty())
                m_strOutText += " "+postfix( _col, _row );

        verifyCondition();

        // Find the correct color which depends on the conditions
        // and the sign of the value.
        if ( floatColor( _col, _row ) == KSpreadCell::NegRed && v < 0.0 )
            tmpPen.setColor( Qt::red );
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
            tmpPen.setColor(tmpCondition->colorcond);
        }
    }
    else if ( isFormular() )
    {
        m_strOutText = m_strFormularOut;
    }
    else
    {
        if(m_strText[0]!='\'')
                m_strOutText = m_strText;
    }

    // Empty text?
    if ( m_strOutText.isEmpty() )
    {
        m_strOutText = QString::null;
        if ( isDefault() )
            return;
    }

    _painter.setPen(tmpPen);

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
        _painter.setFont( textFont(_col,_row ) );

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
         rightBorderWidth( _col, _row ) && multiRow(_col, _row ) )
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
    int indent=0;
    int a = defineAlignX();
    //apply indent if text is align to left not when text is at right or middle
    if(  a==KSpreadCell::Left && !isEmpty())
        indent=getIndent(column(),row());

    // Do we have to occupy additional cells right hand ?
    if ( m_iOutTextWidth+indent > w - 2 * BORDER_SPACE - leftBorderWidth( _col, _row) -
         rightBorderWidth( _col, _row ) )
    {
        // No chance. We can not obscure more/less cells.
        if ( m_bForceExtraCells )
        {
            // The text does not fit in the cell
            //m_strOutText = "**";
            m_bCellTooShort=true;
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
                    if ( m_iOutTextWidth+indent <= w - 2 * BORDER_SPACE - leftBorderWidth( _col, _row) -
                         rightBorderWidth( _col, _row ) )
                        end = 1;
                }
                // Not enough space, but the next cell is not empty
                else
                    end = -1;
            }

            // Dont occupy additional space for right aligned or centered text or values.
            // ##### Why ?
            if (( align(_col,_row) == KSpreadCell::Left || align(_col,_row) == KSpreadCell::Undefined) && !isValue())
            {
                m_iExtraWidth = w;
                for( int i = m_iColumn + 1; i <= c; ++i )
                {
                    KSpreadCell *cell = m_pTable->nonDefaultCell( i, m_iRow );
                    cell->obscure( this, m_iColumn, m_iRow );
                }
                m_iExtraXCells = c - m_iColumn;
                //Not enough space
                if(end==-1)
                        m_bCellTooShort=true;
            }
            else
            {
                m_bCellTooShort=true;
                //m_strOutText = "**";
            }
        }
    }
    m_bLayoutDirtyFlag = FALSE;
}

QString KSpreadCell::createFormat( double value, int _col, int _row )
{
    // if precision is -1, ask for a huge number of decimals, we'll remove
    // the zeros later. Is 8 ok ?
    int p = (precision(_col,_row) == -1) ? 8 : precision(_col,_row) ;
    QString localizedNumber= locale()->formatNumber( value, p );
    int pos = 0;

    switch( getFormatNumber(column(),row()))
    {
    case Number :
        localizedNumber = locale()->formatNumber(value, p);
        if( floatFormat( _col, _row ) == KSpreadCell::AlwaysSigned && value >= 0 )
        {
            if(locale()->positiveSign().isEmpty())
                localizedNumber='+'+localizedNumber;
        }
        break;
    case Percentage :
        localizedNumber = locale()->formatNumber(value, p)+ " %";
        if( floatFormat( _col, _row ) == KSpreadCell::AlwaysSigned && value >= 0 )
        {
            if(locale()->positiveSign().isEmpty())
                localizedNumber='+'+localizedNumber;
        }
        break;
    case Money :
        localizedNumber = locale()->formatMoney(value,locale()->currencySymbol(),p );
        if( floatFormat( _col, _row) == KSpreadCell::AlwaysSigned && value >= 0 )
        {
            if(locale()->positiveSign().isNull())
                localizedNumber='+'+localizedNumber;
        }
        break;
    case Scientific:
        localizedNumber= QString::number(value, 'E', p);
        if((pos=localizedNumber.find('.'))!=-1)
            localizedNumber=localizedNumber.replace(pos,1,decimal_point);
        if( floatFormat( _col, _row ) == KSpreadCell::AlwaysSigned && value >= 0 )
        {
            if(locale()->positiveSign().isEmpty())
                localizedNumber='+'+localizedNumber;
        }
        break;
    case ShortDate:
    case TextDate :
    case date_format1:
    case date_format2:
    case date_format3:
    case date_format4:
    case date_format5:
    case date_format6:
    case date_format7:
    case date_format8:
    case date_format9:
    case date_format10:
    case date_format11:
    case date_format12:
    case date_format13:
    case date_format14:
    case date_format15:
    case date_format16:
        break;
    case fraction_half:
    case fraction_quarter:
    case fraction_eighth:
    case fraction_sixteenth:
    case fraction_tenth:
    case fraction_hundredth:
    case fraction_one_digit:
    case fraction_two_digits:
    case fraction_three_digits:
        localizedNumber=createFractionFormat(value);
        if( floatFormat( _col, _row ) == KSpreadCell::AlwaysSigned && value >= 0 )
        {
            if(locale()->positiveSign().isEmpty())
                localizedNumber='+'+localizedNumber;
        }
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
        switch( getFormatNumber(column(),row()))
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
        case fraction_one_digit:
            index=3;
            break;
        case fraction_two_digits:
            index=4;
            break;
        case fraction_three_digits:
            index=5;
            break;
        default:
            kdDebug(36001)<<"Error in Fraction format\n";
            break;
        }
        formatNumber tmpFormat=getFormatNumber(column(),row());
        if( tmpFormat !=fraction_three_digits
        && tmpFormat !=fraction_two_digits
        && tmpFormat !=fraction_one_digit)
                {
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
         else
                {
                int limit=0;

                double precision=0;
                formatNumber tmpFormat=getFormatNumber(column(),row());
                if(tmpFormat ==fraction_three_digits)
                        limit=999;
                else if(tmpFormat ==fraction_two_digits)
                        limit=99;
                else if(tmpFormat ==fraction_one_digit)
                        limit=9;
                double denominator=0;
                double numerator=0;
                do
                        {
                        double val1=result;
                        double inter2=1;
                        double inter4=0;
                        double p=0;
                        double q=0;
                        double val2=rint(result);

                        precision=pow((float)10,(-1*index));
                        numerator=rint(result);
                        denominator=1;
                        while(fabs(numerator/denominator-result)>precision)
                                {
                                val1=(1/(val1-val2));
                                val2=rint(val1);
                                p= val2*numerator + inter2;
                                q= val2*denominator + inter4;
                                inter2=numerator;
                                inter4=denominator;
                                numerator=p;
                                denominator=q;
                                }
                        index--;
                        }
                while (fabs(denominator)>limit) ;
                if(fabs(denominator)==fabs(numerator))
                        tmp = tmp.setNum( floor(value+1) )+" ";
                else
                        tmp = tmp.setNum( floor(value) ) + " " + tmp.setNum( fabs(numerator) ) + "/" + tmp.setNum( fabs(denominator) );
                }

    }

    return tmp;
}

QString KSpreadCell::createDateFormat( )
{
    QString tmp,tmp2;
    formatNumber tmpFormat=getFormatNumber(column(),row());
    if(tmpFormat==ShortDate)
        tmp = locale()->formatDate(m_Date,true);
    else if(tmpFormat==TextDate)
        tmp=locale()->formatDate(m_Date,false);
    else if(tmpFormat==date_format1)/*18-Feb-99*/
    {
        tmp=QString().sprintf("%02d", m_Date.day());
        tmp=tmp+"-"+locale()->monthName(m_Date.month(), true)+"-";
        tmp=tmp+tmp2.setNum(m_Date.year()).right(2);
    }
    else if(tmpFormat==date_format2) /*18-Feb-1999*/
    {
        tmp=QString().sprintf("%02d", m_Date.day());
        tmp=tmp+"-"+locale()->monthName(m_Date.month(), true)+"-";
        tmp=tmp+tmp2.setNum(m_Date.year());
    }
    else if(tmpFormat==date_format3) /*18-Feb*/
    {
        tmp=QString().sprintf("%02d", m_Date.day());
        tmp=tmp+"-"+locale()->monthName(m_Date.month(), true);
    }
    else if(tmpFormat==date_format4) /*18-05*/
    {
        tmp=QString().sprintf("%02d", m_Date.day());
        tmp=tmp+"-"+QString().sprintf("%02d", m_Date.month());
    }
    else if(tmpFormat==date_format5) /*18/05/00*/
    {
        tmp=QString().sprintf("%02d", m_Date.day());
        tmp=tmp+"/"+ QString().sprintf("%02d", m_Date.month())+"/";
        tmp=tmp+tmp2.setNum(m_Date.year()).right(2);
    }
    else if(tmpFormat==date_format6) /*18/05/1999*/
    {
        tmp=QString().sprintf("%02d", m_Date.day());
        tmp=tmp+"/"+ QString().sprintf("%02d", m_Date.month())+"/";
        tmp=tmp+tmp2.setNum(m_Date.year());
    }
    else if(tmpFormat==date_format7) /*Feb-99*/
    {
        tmp=locale()->monthName(m_Date.month(), true)+"-";
        tmp=tmp+tmp2.setNum(m_Date.year()).right(2);
    }
    else if(tmpFormat==date_format8) /*February-99*/
    {
        tmp=locale()->monthName(m_Date.month())+"-";
        tmp=tmp+tmp2.setNum(m_Date.year()).right(2);
    }
    else if(tmpFormat==date_format9) /*February-1999*/
    {
        tmp=locale()->monthName(m_Date.month())+"-";
        tmp=tmp+tmp2.setNum(m_Date.year());
    }
    else if(tmpFormat==date_format10) /*F-99*/
    {
        tmp=locale()->monthName(m_Date.month()).at(0)+"-";
        tmp=tmp+tmp2.setNum(m_Date.year()).right(2);
    }
    else if(tmpFormat==date_format11) /*18/Feb*/
    {
        tmp=QString().sprintf("%02d", m_Date.day())+"/";
        tmp+=locale()->monthName(m_Date.month(),true);
    }
    else if(tmpFormat==date_format12) /*18/02*/
    {
        tmp=QString().sprintf("%02d", m_Date.day())+"/";
        tmp+=QString().sprintf("%02d", m_Date.month());
    }
    else if(tmpFormat==date_format13) /*18/Feb/1999*/
    {
        tmp=QString().sprintf("%02d", m_Date.day());
        tmp=tmp+"/"+locale()->monthName(m_Date.month(),true)+"/";
        tmp=tmp+tmp2.setNum(m_Date.year());
    }
    else if(tmpFormat==date_format14) /*2000/Feb/18*/
    {
        tmp=tmp2.setNum(m_Date.year());
        tmp=tmp+"/"+locale()->monthName(m_Date.month(),true)+"/";
        tmp=tmp+QString().sprintf("%02d", m_Date.day());
    }
    else if(tmpFormat==date_format15) /*2000-Feb-18*/
    {
        tmp=tmp2.setNum(m_Date.year());
        tmp=tmp+"-"+locale()->monthName(m_Date.month(),true)+"-";
        tmp=tmp+QString().sprintf("%02d", m_Date.day());
    }
    else if(tmpFormat==date_format16) /*2000-02-18*/
    {
        tmp=tmp2.setNum(m_Date.year());
        tmp=tmp+"-"+QString().sprintf("%02d", m_Date.month())+"-";
        tmp=tmp+QString().sprintf("%02d", m_Date.day());
    }
    else
        tmp = locale()->formatDate(m_Date,true);
    return tmp;
}

QString KSpreadCell::createTimeFormat( )
{
    QString tmp;
    formatNumber tmpFormat=getFormatNumber(column(),row());
    if( tmpFormat == Time )
        tmp = locale()->formatTime(m_Time,false);
    else if(tmpFormat == SecondeTime )
        tmp = locale()->formatTime(m_Time,true);
    else if(tmpFormat == Time_format1)
        {// 9 : 01 AM
        QString tmpTime;
        if( m_Time.hour()>12)
                tmp=QString().sprintf("%02d", m_Time.hour()-12)+":"+QString().sprintf("%02d",m_Time.minute()) +" "+ i18n("PM");
        else
                tmp=QString().sprintf("%02d", m_Time.hour())+":"+QString().sprintf("%02d",m_Time.minute()) +" "+ i18n("AM");
        }
    else if(tmpFormat == Time_format2)
        { //9:01:05 AM
        QString tmpTime;
        if( m_Time.hour()>12)
                tmp=QString().sprintf("%02d", m_Time.hour()-12)+":"+QString().sprintf("%02d",m_Time.minute()) +":"+QString().sprintf("%02d",m_Time.second())+" "+ i18n("PM");
        else
                tmp=QString().sprintf("%02d", m_Time.hour())+":"+QString().sprintf("%02d",m_Time.minute()) +":"+QString().sprintf("%02d",m_Time.second())+" "+ i18n("AM");
        }
    else if(tmpFormat == Time_format3)
        { // 9 h 01 min 28 s
        QString tmpTime;
        tmp=QString().sprintf("%02d",m_Time.hour())+" "+i18n("h")+" "+QString().sprintf("%02d",m_Time.minute())+" "+i18n("min")+" "+QString().sprintf("%02d",m_Time.second())  +" "+ i18n("s");
        }
    else
            tmp = locale()->formatTime(m_Time,false);
    return tmp;
}

void KSpreadCell::verifyCondition()
{
    m_numberOfCond=-1;
    double v = m_dValue * faktor(column(),row());
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
    int a = align(_col,_row);
    RowLayout *rl = m_pTable->rowLayout( _row );
    ColumnLayout *cl = m_pTable->columnLayout( _col );

    int w = cl->width();
    int h = rl->height();

    if ( m_iExtraXCells )
        w = m_iExtraWidth;
    if ( m_iExtraYCells )
        h = m_iExtraHeight;
    int tmpAngle=getAngle(_col,_row);
    switch( alignY(_col,_row) )
    {
    case KSpreadCell::Top:
        if(tmpAngle!=0)
            m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +m_fmAscent;
        else
        {
            if(tmpAngle<0)
                m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE ;
            else
                m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +(int)(m_fmAscent*cos(tmpAngle*M_PI/180));
        }
        break;
    case KSpreadCell::Bottom:
        if(!verticalText(_col,_row) && !multiRow(_col,_row) && !tmpAngle)
            m_iTextY = h - BORDER_SPACE - bottomBorderWidth( _col, _row );
        else if(tmpAngle!=0)
        {
            if((h - BORDER_SPACE - m_iOutTextHeight- bottomBorderWidth( _col, _row ))>0)
                {
                if( tmpAngle < 0 )
                        m_iTextY = h - BORDER_SPACE - m_iOutTextHeight- bottomBorderWidth( _col, _row );
                else
                        m_iTextY = h - BORDER_SPACE - m_iOutTextHeight- bottomBorderWidth( _col, _row )+(int)(m_fmAscent*cos(tmpAngle*M_PI/180));
                }
            else
                {
                if( tmpAngle < 0 )
                    m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE ;
                else
                    m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +(int)(m_fmAscent*cos(tmpAngle*M_PI/180));
                }
        }
        else if( multiRow(_col,_row) )
        {
            int tmpline=m_nbLines;
            if(m_nbLines>1)
                tmpline=m_nbLines-1;
            if((h - BORDER_SPACE - m_iOutTextHeight*m_nbLines- bottomBorderWidth( _col, _row ))>0)
                m_iTextY = h - BORDER_SPACE - m_iOutTextHeight*tmpline- bottomBorderWidth( _col, _row );
            else
                m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +m_fmAscent;
        }
        else
            if((h - BORDER_SPACE - m_iOutTextHeight- bottomBorderWidth( _col, _row ))>0)
                m_iTextY = h - BORDER_SPACE - m_iOutTextHeight- bottomBorderWidth( _col, _row )+m_fmAscent;
            else
                m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +m_fmAscent;
        break;
    case KSpreadCell::Middle:
        if(!verticalText(_col,_row) && !multiRow(_col,_row) && !tmpAngle)
            m_iTextY = ( h - m_iOutTextHeight ) / 2 +m_fmAscent;
        else if( tmpAngle != 0 )
        {
            if( ( h - m_iOutTextHeight ) > 0 )
            {
                if( tmpAngle < 0 )
                        m_iTextY = ( h - m_iOutTextHeight ) / 2 ;
                else
                        m_iTextY = ( h - m_iOutTextHeight ) / 2 +(int)(m_fmAscent*cos(tmpAngle*M_PI/180));
            }
            else
                {
                if( tmpAngle < 0 )
                    m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE ;
                else
                    m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +(int)(m_fmAscent*cos(tmpAngle*M_PI/180));
                }
        }
        else if( multiRow(_col,_row) )
        {
            int tmpline=m_nbLines;
            if(m_nbLines==0)
                tmpline=1;
            if(( h - m_iOutTextHeight*tmpline )>0)
                m_iTextY = ( h - m_iOutTextHeight*tmpline ) / 2 +m_fmAscent;
            else
                m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +m_fmAscent;
        }
        else
            if(( h - m_iOutTextHeight )>0)
                m_iTextY = ( h - m_iOutTextHeight ) / 2 +m_fmAscent;
            else
                m_iTextY = topBorderWidth( _col, _row) + BORDER_SPACE +m_fmAscent;
        break;
    }
    a=defineAlignX();
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
    int tmpAngle=getAngle(column(),row());
    if( !verticalText(column(),row()) && !tmpAngle )
    {
        m_iOutTextWidth = fm.width( m_strOutText );
        m_iOutTextHeight = fm.ascent() + fm.descent();
    }
    // Rotated text ?
    else if(  tmpAngle!= 0 )
    {
        m_iOutTextHeight = static_cast<int>(cos(tmpAngle*M_PI/180)*(fm.ascent() + fm.descent())+abs((int)(fm.width( m_strOutText )*sin(tmpAngle*M_PI/180))));
        m_iOutTextWidth = static_cast<int>(abs((int)(sin(tmpAngle*M_PI/180)*(fm.ascent() + fm.descent())))+fm.width( m_strOutText )*cos(tmpAngle*M_PI/180));
        //kdDebug(36001)<<"m_iOutTextWidth"<<m_iOutTextWidth<<"m_iOutTextHeight"<<m_iOutTextHeight<<endl;
    }
    // Vertical text ?
    else
    {
        int width=0;
        for(unsigned int i=0;i<m_strOutText.length();i++)
                width=QMAX(width,fm.width(m_strOutText.at(i)));
        m_iOutTextWidth = width;
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
        _paint.setFont( textFont(_col,_row ) );
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
    //    kdDebug(36002) << util_cellName( m_iColumn, m_iRow ) << " calc() Looking into dependencies..." << endl;
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
              m_bError=true;
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
              m_bLayoutDirtyFlag = true;
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
          m_bLayoutDirtyFlag = true;
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
  //  kdDebug(36002) << util_cellName( m_iColumn, m_iRow ) << " calc() Now calculating." << endl;

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
    m_bError =false;
    m_bValue = true;
    m_bBool = false;
    m_bDate =false;
    m_bTime=false;
    //m_strFormularOut = locale()->formatNumber( m_dValue );

    checkNumberFormat(); // auto-chooses number or scientific
    // Format the result appropriately
    m_strFormularOut = createFormat( m_dValue, m_iColumn, m_iRow );
  }
  else if ( context.value()->type() == KSValue::IntType )
  {
    m_dValue = (double)context.value()->intValue();
    m_bError =false;
    m_bValue = true;
    m_bBool = false;
    m_bDate = false;
    m_bTime=false;
    //m_strFormularOut = locale()->formatNumber( m_dValue );

    checkNumberFormat(); // auto-chooses number or scientific
    // Format the result appropriately
    m_strFormularOut = createFormat( m_dValue, m_iColumn, m_iRow );
  }
  else if ( context.value()->type() == KSValue::BoolType )
  {
    m_bError =false;
    m_bValue = false;
    m_bBool = true;
    m_bDate =false;
    m_bTime=false;
    m_dValue = context.value()->boolValue() ? 1.0 : 0.0;
    // (David): i18n'ed True and False - hope it's ok
    m_strFormularOut = context.value()->boolValue() ? i18n("True") : i18n("False");
    setFormatNumber(Number);
  }
  else if ( context.value()->type() == KSValue::TimeType )
  {
    m_bError =false;
    m_bValue = false;
    m_bBool = false;
    m_bDate =false;
    m_bTime=true;
    m_Time=context.value()->timeValue();

    //change format
    formatNumber tmpFormat= getFormatNumber(column(),row());
    if( tmpFormat != SecondeTime &&  tmpFormat != Time_format1 &&  tmpFormat != Time_format2
    && tmpFormat != Time_format3)
        {
        m_strFormularOut = locale()->formatTime(m_Time,false);
        setFormatNumber(Time);
        }
    else
        {
        //m_strFormularOut = locale()->formatTime(m_Time,true);
        m_strFormularOut = createTimeFormat();
        }
  }
  else if ( context.value()->type() == KSValue::DateType)
  {
    m_bError =false;
    m_bValue = false;
    m_bBool = false;
    m_bDate =true;
    m_bTime=false;
    m_Date=context.value()->dateValue();
    formatNumber tmpFormat= getFormatNumber(column(),row());
    if( tmpFormat != TextDate
    && !(tmpFormat>=200 &&tmpFormat<=215))
        {
        setFormatNumber(ShortDate);
        m_strFormularOut = locale()->formatDate(m_Date,true);
        }
    else
        {
        m_strFormularOut = createDateFormat();
        }
  }
  else
  {
    if ( m_pQML )
        delete m_pQML;
    m_pQML = 0;
    if ( m_pVisualFormula )
        delete m_pVisualFormula;
    m_pVisualFormula = 0;
    m_bError =false;
    m_bValue = false;
    m_bBool = false;
    m_bDate=false;
    m_bTime=false;
    m_strFormularOut = context.value()->toString( context );
    if( !m_strFormularOut.isEmpty() && m_strFormularOut[0] == '*' )
        {
        m_pVisualFormula = new KFormula();
        m_pVisualFormula->parse( m_strFormularOut.mid( 1 ) );
        }
  else if ( !m_strFormularOut.isEmpty() && m_strFormularOut[0] == '!' )
        {
        m_pQML = new QSimpleRichText( m_strFormularOut.mid(1),  QApplication::font() );//, m_pTable->widget() );
        }
  else if( !m_strFormularOut.isEmpty() && m_strFormularOut[0]=='\'')
        {
        m_strFormularOut=m_strFormularOut.right(m_strFormularOut.length()-1);
        }
    setFormatNumber(Number);
  }
  if ( m_style == ST_Select )
  {
      SelectPrivate *s = (SelectPrivate*)m_pPrivate;
      s->parse( m_strFormularOut );
  }
  m_bCalcDirtyFlag=false;
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

/**
 * @param pos describes what to draw. The value for edges are:
 *            top = 1, right = 2, bottom = 3, left = 4.
 *            The values for corners are:
 *            top left = 11, top_right = 12, bottom_right = 13, bottom_left = 14.
 */
static void paintCellHelper( QPainter& _painter, int _tx, int _ty, int col, int row,
                             int w, int h, int pos, const QRect& marker )
{
    QPoint p = marker.bottomRight();

    switch( pos )
    {
    // top
    case 1:
        if ( p.x() == col && p.y() == row - 1 )
        {
            _painter.drawLine( _tx, _ty, _tx + w - 3, _ty );
            _painter.fillRect( _tx + w - 2, _ty - 1, 5, 5, _painter.pen().color() );
        }
        else
            _painter.drawLine( _tx, _ty, _tx + w, _ty );
        break;
    // bottom
    case 3:
        if ( p.x() == col && p.y() == row )
        {
            _painter.drawLine( _tx, _ty + h, _tx + w - 3, _ty + h );
            _painter.fillRect( _tx + w - 2, _ty + h - 1, 5, 5, _painter.pen().color() );
        }
        else
            _painter.drawLine( _tx, _ty + h, _tx + w, _ty + h );
        break;
    // left
    case 4:
        if ( p.x() == col - 1 && p.y() == row )
        {
            _painter.drawLine( _tx, _ty - 1, _tx, _ty + h - 3 );
            _painter.fillRect( _tx - 2, _ty + h - 1, 5, 5, _painter.pen().color() );
        }
        else
            _painter.drawLine( _tx, _ty - 1, _tx, _ty + h + 2 );
        break;
    // right
    case 2:
        if ( p.x() == col && p.y() == row )
        {
            _painter.drawLine( _tx + w, _ty - 1, _tx + w, _ty + h - 3 );
            _painter.fillRect( _tx + w - 2, _ty + h - 1, 5, 5, _painter.pen().color() );
        }
        else
            _painter.drawLine( _tx + w, _ty - 1, _tx + w, _ty + h + 2 );
        break;
    // top left
    case 11:
        if ( p.x() == col - 1 && p.y() == row - 1 )
            _painter.fillRect( _tx - 2, _ty - 1, 5, 5, _painter.pen().color() );
        else
            _painter.drawLine( _tx, _ty, _tx, _ty + 1 );
        break;
    // top right
    case 12:
        _painter.drawLine( _tx + w, _ty - 1, _tx + w, _ty + 1 );
        break;
    // bottom right
    case 13:
        _painter.drawLine( _tx + w, _ty + h - 1, _tx + w, _ty + h + 1 );
        break;
    // bottom left
    case 14:
        _painter.drawLine( _tx, _ty + h - 1, _tx, _ty + h + 1 );
        break;
    }
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
                             int _col, int _row,
                             ColumnLayout *cl, RowLayout *rl,
                             QRect *_prect, bool override_obscured )
{
    // If this cell is obscured then draw the obscuring one instead.
    if ( m_pObscuringCell && !override_obscured )
    {
        _painter.save();
        m_pObscuringCell->paintCell( _rect, _painter,
                                     m_iObscuringCellsColumn, m_iObscuringCellsRow, _prect );
        _painter.restore();
        m_bLayoutDirtyFlag = FALSE;
        return;
    }

    // Need to recalculate ?
    if ( /*m_bCalcDirtyFlag*/calcDirtyFlag() && !override_obscured )
        calc();

    bool old_layoutflag = m_bLayoutDirtyFlag;
    // Need to make a new layout ?
    if ( m_bLayoutDirtyFlag && !override_obscured )
        makeLayout( _painter, _col, _row );

    // Determine the dimension of the cell.
    int w = cl->width();
    int h = rl->height();

    // Do we really need to display this cell ?
    // Obeye extra cells.
    int w2 = w;
    int h2 = h;
    if ( m_iExtraXCells )
        w2 = m_iExtraWidth;
    if ( m_iExtraYCells )
        h2 = m_iExtraHeight;

    QRect r2( _tx, _ty, w2, h2 );
    if ( !r2.intersects( _rect ) )
        return;

    // Tell our caller where we painted.
    // ## Torben: Where is this used ?
    if ( _prect )
        _prect->setRect( _tx, _ty, w, h );

    // is the cell selected ?
    bool selected = m_pTable->selectionRect().contains( QPoint( _col, _row ) );
    if ( m_pObscuringCell )
        selected = m_pTable->selectionRect().contains( QPoint( m_pObscuringCell->column(),
                                                                    m_pObscuringCell->row() ) );

    // Dont draw any selection when printing.
    if ( _painter.device()->isExtDev() )
        selected = FALSE;

    QColorGroup defaultColorGroup = QApplication::palette().active();
    QRect m = m_pTable->marker();

    int moveX=0;
    int moveY=0;
    if( isObscured() && isObscuringForced() )
    {
    moveX=obscuringCellsColumn();
    moveY=obscuringCellsRow();
    }

    // Determine the correct background color
    if ( selected && ( _col != m.left() || _row != m.top() )
    && (moveX!=m.left() || moveY!=m.top()))
        _painter.setBackgroundColor( defaultColorGroup.highlight() );
    else
    {
        QColor bg = bgColor( _col, _row );
        // if ( m_pObscuringCell )
        // bg = m_pObscuringCell->bgColor( m_pObscuringCell->column(),
        // m_pObscuringCell->row() );
        if (! _painter.device()->isExtDev() )
                {
                if ( bg.isValid() )
                        _painter.setBackgroundColor( bg );
                else
                        _painter.setBackgroundColor( defaultColorGroup.base() );
                }
        else
                {
                //bad hack but there is a qt bug
                //so I can print backgroundcolor
                if( bg.isValid())
                        {
                        QBrush bb( bg );
                        _painter.fillRect( _tx , _ty ,w ,h , bb );
                        }
                }
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
    // Erase the background of the cell.
    if ( !_painter.device()->isExtDev() )
        _painter.eraseRect( _tx + left_offset, _ty + top_offset,
                            w - left_offset - right_offset,
                            h - top_offset - bottom_offset );
    //
    // First draw the default borders so that they dont
    // overwrite any other border.
    //
    if ( left_pen.style() == Qt::NoPen &&
         ( !m_pObscuringCell || m_pObscuringCell->column() == _col ) )
    {
        if( table()->getShowGrid() && !_painter.device()->isExtDev())
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
    if ( top_pen.style() == Qt::NoPen &&
         ( !m_pObscuringCell || m_pObscuringCell->row() == _row ) )
    {
        if( table()->getShowGrid() && !_painter.device()->isExtDev())
        {
            top_offset = 1;

            QPen l = m_pTable->cellAt( _col, _row - 1 )->leftBorderPen( _col, _row - 1 );
            QPen r = m_pTable->cellAt( _col, _row - 1 )->rightBorderPen( _col, _row - 1 );

            int dl = 0;
            if ( l.style() != Qt::NoPen )
                dl = ( l.width() - 1 ) / 2 + 1;
            int dr = 0;
            if ( r.style() != Qt::NoPen )
                dr = r.width() / 2;

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
    if ( right_pen.style() != Qt::NoPen && extraXCells() == 0 )
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
    if ( bottom_pen.style() != Qt::NoPen && extraYCells() == 0 )
    {
        _painter.setPen( bottom_pen );
        _painter.drawLine( _tx, h + _ty, _tx + w, h + _ty );

        bottom_offset = bottom_pen.width() / 2;
    }

    // Erase the background of the cell.
    /*if ( !_painter.device()->isExtDev() )
        _painter.eraseRect( _tx + left_offset, _ty + top_offset,
                            w - left_offset - right_offset,
                            h - top_offset - bottom_offset );
    */
    // Draw a background brush
    QBrush bb = backGroundBrush( _col, _row );
    if( bb.style() != Qt::NoBrush )
    {
        _painter.fillRect( _tx + left_offset, _ty + top_offset,
                           w - left_offset - right_offset,
                           h - top_offset - bottom_offset, bb );
    }

    //
    // Look at the cells on our corners. It may happen that we
    // just erased parts of their borders corner, so we might need
    // to repaint these corners.
    //
    KSpreadCell* cell_t = m_pTable->cellAt( _col, _row - 1 );
    KSpreadCell* cell_r = m_pTable->cellAt( _col + 1, _row );
    KSpreadCell* cell_l = m_pTable->cellAt( _col - 1, _row );
    // Not yet used .... KSpreadCell* cell_b = m_pTable->cellAt( _col, _row + 1 );

    // Fix the borders which meat at the top left corner
    QPen vert_pen = cell_t->leftBorderPen( _col, _row - 1 );
    if ( vert_pen.style() != Qt::NoPen )
    {
        QPen horz_pen = cell_l->topBorderPen( _col - 1, _row );
        int bottom = ( QMAX( 0, -1 + (int)horz_pen.width() ) ) / 2 + 1;
        _painter.setPen( vert_pen );
        _painter.drawLine( _tx, _ty, _tx, _ty + bottom );
    }

    // Fix the borders which meat at the top right corner
    vert_pen = cell_t->rightBorderPen( _col, _row - 1 );
    if ( vert_pen.style() != Qt::NoPen )
    {
        QPen horz_pen = cell_r->topBorderPen( _col + 1, _row );
        int bottom = ( QMAX( 0, -1 + (int)horz_pen.width() ) ) / 2 + 1;
        _painter.setPen( vert_pen );
        _painter.drawLine( _tx + w, _ty, _tx + w, _ty + bottom );
    }

    // This cell is obscured? Then dont draw any content
    if ( override_obscured )
        goto drawmarker;
    // This cell is obscuring other ones? Then we redraw their
    // background and borders before we paint our content there.
    else if ( extraXCells() || extraYCells() )
    {
        int ypos = _ty;
        for( int y = 0; y <= extraYCells(); ++y )
        {
            int xpos = _tx;
            RowLayout* rl = m_pTable->rowLayout( _row + y );

            for( int x = 0; x <= extraXCells(); ++ x )
            {
                ColumnLayout* cl = m_pTable->columnLayout( _col + x );
                if ( y != 0 || x != 0 )
                {
                    KSpreadCell* cell = m_pTable->cellAt( _col + x, _row + y );
                    cell->paintCell( _rect, _painter, xpos, ypos, _col + x, _row + y, cl, rl, 0, true );
                }
                xpos += cl->width();
            }

            ypos += rl->height();
        }
    }

    //
    // Draw diagonal borders.
    //
    if ( m_fallDiagonalPen.style() != Qt::NoPen )
    {
        // Diagonal line go across other cells if this cell is
        // a multicol/row cell. So use "w2" instead of "w" ...
        _painter.setPen( m_fallDiagonalPen );
        _painter.drawLine( _tx, _ty, _tx + w2, _ty + h2 );
    }
    if ( m_goUpDiagonalPen.style() != Qt::NoPen )
    {
        _painter.setPen( m_goUpDiagonalPen );
        _painter.drawLine( _tx, _ty + h2 , _tx + w2, _ty );
    }

    // Point the little corner if there is a comment attached
    // to this cell.
    if( !comment(column(),row()).isEmpty())
    {
        QPointArray point( 3 );
        point.setPoint( 0,_tx + w2 - 10, _ty );
        point.setPoint( 1, _tx + w2,_ty );
        point.setPoint( 2, _tx + w2,_ty + 10 );
        _painter.setBrush( QBrush(Qt::red  ) );
        _painter.setPen( Qt::NoPen );
        _painter.drawPolygon( point );
    }

    //show  a red triangle when it's not possible to write all text in cell
    //don't print the red triangle
    if(m_bCellTooShort && !_painter.device()->isExtDev() )
    {
        QPointArray point( 3 );
        point.setPoint( 0,_tx + w2-4 , (_ty+h2/2)-4 );
        point.setPoint( 1, _tx + w2,(_ty+h2/2) );
        point.setPoint( 2, _tx + w2-4,(_ty+h2/2) + 4 );
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
                                          w2 - 1, h2 - 1,
                                          defaultColorGroup, selected, &fill );
    }
    /**
     * Modification for drawing the combo box
     */
    else if ( m_style == KSpreadCell::ST_Select )
    {
        QApplication::style().drawComboButton(  &_painter, _tx + 1, _ty + 1,
                                                w2 - 1, h2 - 1,
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
        _painter.setPen( textPen(_col,_row)/*.color()??*/ );
        _painter.setFont( textFont(_col,_row ) );
        // TODO: No better method to set new font ?
        if ( old_layoutflag )
            m_pVisualFormula->parse( m_strText.mid(1) );
        m_pVisualFormula->setPos( _tx + w2/2, _ty + h2/2 );
        m_pVisualFormula->redraw( _painter );
        _painter.restore();
    }
    /**
     * Usual Text
     */
    else if ( !m_strOutText.isEmpty() )
    {
        QPen tmpPen(textPen(_col,_row));
        if ( selected && ( _col != m.left() || _row != m.top() )  )
        {
            QPen p( textPen(_col,_row) );
            p.setColor( defaultColorGroup.highlightedText() );
            _painter.setPen( p );
        }
        else
            _painter.setPen( textPen(_col,_row) );

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
            tmpPen.setColor( tmpCondition->colorcond );
        }
        else
        {
            _painter.setFont( textFont(_col,_row ) );
            if( m_bValue && !m_pTable->getShowFormular() )
            {
                double v = m_dValue * faktor(column(),row());
                if ( floatColor( _col, _row) == KSpreadCell::NegRed && v < 0.0 && !m_pTable->getShowFormular() )
                    tmpPen.setColor( Qt::red );
                else
                    tmpPen.setColor( textColor( _col, _row) );
            }
            else
                tmpPen.setColor( textColor( _col, _row) );
        }
        _painter.setPen(tmpPen);
        //_painter.setFont( textFont(_col,_row ) );

        QString tmpText=m_strOutText;
        if(m_bCellTooShort)
                m_strOutText=textDisplaying(_painter);

        //hide zero
        if(m_pTable->getHideZero() &&  m_bValue &&   m_dValue * faktor(column(),row())==0)
                m_strOutText="";

        conditionAlign( _painter, _col, _row );

        int indent=0;
        int a = defineAlignX();
        //apply indent if text is align to left not when text is at right or middle
        if(  a==KSpreadCell::Left && !isEmpty())
                indent=getIndent(column(),row());
        int tmpAngle=getAngle(_col,_row);
        if ( !multiRow(_col,_row) && !verticalText(_col,_row) && !tmpAngle)
                {
                _painter.drawText( indent+_tx + m_iTextX, _ty + m_iTextY, m_strOutText );
                }
        else if( tmpAngle!=0)
        {
            int angle=tmpAngle;
            QFontMetrics fm = _painter.fontMetrics();
            _painter.rotate(angle);
            int x;
            if(angle>0)
                x=indent+_tx + m_iTextX;
            else
                x=indent+static_cast<int>(_tx + m_iTextX -(fm.descent() + fm.ascent())*sin(angle*M_PI/180));
            int y;
            if(angle>0)
                y=_ty + m_iTextY;
            else
                y=_ty + m_iTextY+m_iOutTextHeight;
            _painter.drawText( x*cos(angle*M_PI/180)+y*sin(angle*M_PI/180),
                               -x*sin(angle*M_PI/180) + y*cos(angle*M_PI/180) , m_strOutText );
            _painter.rotate(-angle);
        }
        else if( multiRow(_col,_row) && !verticalText(_col,_row))
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

                int a = defineAlignX();
                if(m_pTable->getShowFormular())
                    a = KSpreadCell::Left;

                // #### Torben: This looks duplicated for me
                switch( a )
                {
                case KSpreadCell::Left:
                    m_iTextX = leftBorderWidth( _col, _row) + BORDER_SPACE;
                    break;
                case KSpreadCell::Right:
                    m_iTextX = w2 - BORDER_SPACE - fm.width( t )
                               - rightBorderWidth( _col, _row );
                    break;
                case KSpreadCell::Center:
                    m_iTextX = ( w2 - fm.width( t ) ) / 2;
                }
                _painter.drawText( indent+_tx + m_iTextX + dx, _ty + m_iTextY + dy, t );
                dy += fm.descent() + fm.ascent();
            }
            while ( i != -1 );
        }
        else if(verticalText(_col,_row))
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
                _painter.drawText( indent +_tx + m_iTextX + dx, _ty + m_iTextY + dy, t );
                dy += fm.descent() + fm.ascent();
                j++;
            }
            while ( j != i );
        }

        if(m_bCellTooShort)
                m_strOutText=tmpText;
        if(m_pTable->getHideZero() &&  m_bValue &&   m_dValue * faktor(column(),row())==0)
                m_strOutText=tmpText;
    }

    // Dont draw page borders or the marker when printing
    if ( _painter.device()->isExtDev() )
        return;

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

 drawmarker:
    //
    // Draw the marker
    //
    // Some of this code is duplicated in KSpreadCanvas::updateSelection
    //
    QRect marker = m_pTable->markerRect();
    QRect larger;
    larger.setCoords( marker.left() - 1, marker.top() - 1, marker.right() + 1, marker.bottom() + 1 );

    QPen pen(Qt::black,3);
    _painter.setPen( pen );

    // The marker is exactly this cell ?
    if ( marker.left() == _col && marker.right() == _col &&
         marker.top() == _row && marker.bottom() == _row )
    {
        paintCellHelper( _painter, _tx, _ty, _col, _row, w, h, 1, marker );
        paintCellHelper( _painter, _tx, _ty, _col, _row, w, h, 2, marker );
        paintCellHelper( _painter, _tx, _ty, _col, _row, w, h, 3, marker );
        paintCellHelper( _painter, _tx, _ty, _col, _row, w, h, 4, marker );
    }
    else if ( marker.contains( QPoint(_col, _row) ) )
    {
        // int w = cl->width();
        // int h = rl->height();

        // Upper border ?
        if ( _row == marker.top() )
            paintCellHelper( _painter, _tx, _ty, _col, _row, w, h, 1, marker );
        // Left border ?
        if ( _col == marker.left() )
            paintCellHelper( _painter, _tx, _ty, _col, _row, w, h, 4, marker );
        // Lower border ?
        if ( _row == marker.bottom() )
            paintCellHelper( _painter, _tx, _ty, _col, _row, w, h, 3, marker );
        // Right border ?
        if ( _col == marker.right() )
            paintCellHelper( _painter, _tx, _ty, _col, _row, w, h, 2, marker );
    }
    // Dont obeye extra cells
    else if ( larger.contains( QPoint(_col, _row) ) )
    {
        // int w = cl->width();
        // int h = rl->height();

        // Upper border ?
        if ( _col >= marker.left() && _col <= marker.right() && _row - 1 == marker.bottom() )
            paintCellHelper( _painter, _tx, _ty, _col, _row, w, h, 1, marker );
        // Left border ?
        if ( _row >= marker.top() && _row <= marker.bottom() && _col - 1 == marker.right() )
            paintCellHelper( _painter, _tx, _ty, _col, _row, w, h, 4, marker );
        // Lower border ?
        if ( _col >= marker.left() && _col <= marker.right() && _row + 1 == marker.top() )
            paintCellHelper( _painter, _tx, _ty, _col, _row, w, h, 3, marker );
        // Right border ?
        if ( _row >= marker.top() && _row <= marker.bottom() && _col + 1 == marker.left() )
            paintCellHelper( _painter, _tx, _ty, _col, _row, w, h, 2, marker );
        // Top left corner ?
        if ( _row == marker.bottom() + 1 && _col == marker.right() + 1 )
            paintCellHelper( _painter, _tx, _ty, _col, _row, w, h, 11, marker );
        // Top right corner ?
        if ( _row == marker.bottom() + 1 && _col == marker.left() - 1 )
            paintCellHelper( _painter, _tx, _ty, _col, _row, w, h, 12, marker );
        // Bottom right corner ?
        if ( _row == marker.top() - 1 && _col == marker.left() - 1 )
            paintCellHelper( _painter, _tx, _ty, _col, _row, w, h, 13, marker );
        // Bottom left corner ?
        if ( _row == marker.top() - 1 && _col == marker.right() + 1 )
            paintCellHelper( _painter, _tx, _ty, _col, _row, w, h, 14, marker );
    }
}

int KSpreadCell::defineAlignX()
{
int a = align(column(),row());
if ( a == KSpreadCell::Undefined )
        {
        if ( m_bValue || m_bDate || m_bTime)
                a = KSpreadCell::Right;
        else
                a = KSpreadCell::Left;
        }
return a;
}

QString KSpreadCell::textDisplaying( QPainter &_painter)
{
QFontMetrics fm = _painter.fontMetrics();
int a=align(column(),row());
if (( a == KSpreadCell::Left || a == KSpreadCell::Undefined) && !isValue())
        {
        //not enough space but align to left
        int len=0;
        for (int i=column();i<=column()+m_iExtraXCells;i++)
                {
                ColumnLayout *cl2 = m_pTable->columnLayout( i );
                len+=cl2->width() - 1;
                }
        QString tmp;
        int tmpIndent=0;
        if(!isEmpty())
                tmpIndent=getIndent(column(),row());
        for (int i=m_strOutText.length();i!=0;i--)
                {
                tmp=m_strOutText.left(i);

                if((fm.width(tmp)+tmpIndent)<(len-4-1)) //4 equal lenght of red triangle +1 pixel
                        {
                        return tmp;
                        }
                }
        return QString("");
        }

ColumnLayout *cl = m_pTable->columnLayout( column() );
int w;
if(  m_iExtraWidth==0)
        w = cl->width();
else
        w = m_iExtraWidth;
if( isValue())
        {
        if( getFormatNumber(column(),row())!=Scientific)
                {
                int p = (precision(column(),row())  == -1) ? 8 : precision(column(),row());
                double value =m_dValue * faktor(column(),row());
                int pos=0;
                QString localizedNumber= QString::number( (value), 'E', p);
                if((pos=localizedNumber.find('.'))!=-1)
                        localizedNumber=localizedNumber.replace(pos,1,decimal_point);
                if( floatFormat( column(), row() ) == KSpreadCell::AlwaysSigned && value >= 0 )
                        {
                        if(locale()->positiveSign().isEmpty())
                                localizedNumber='+'+localizedNumber;
                        }
                if ( precision(column(),row()) == -1 && localizedNumber.find(decimal_point) >= 0 )
                        {
                        //duplicate code it's not good I know I will fix it
                        int start=0;
                        if((start=localizedNumber.find('E'))!=-1)
                                start=localizedNumber.length()-start;
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
                if(fm.width(localizedNumber)<w)
                        return localizedNumber;
                }
        return QString("###");
        }
else
        {
        QString tmp;
        for (int i=m_strOutText.length();i!=0;i--)
                {
                tmp=m_strOutText.left(i);
                if(fm.width(tmp)<(w-4-1)) //4 equals lenght of red triangle +1 pixel
                        return tmp;
                }
        }
return  QString("");
}

/*
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
        _painter.setFont( textFont(_col,_row ) );
      conditionAlign(_painter,_col,_row);
      _painter.drawText( _tx + m_iTextX, _ty + m_iTextY, m_strOutText );
    }
}
*/

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
// Misc Properties.
// Reimplementation of KSpreadLayout methods.
//
///////////////////////////////////////////

bool KSpreadCell::hasProperty( Properties p ) const
{
    if ( !m_pObscuringCell )
        return KSpreadLayout::hasProperty( p );

    // An obscured cell may only have a property if
    // the parent has it and if it is related to borders.

    if ( !m_pObscuringCell->hasProperty( p ) )
        return FALSE;

    switch( p )
    {
    case PBackgroundBrush:
    case PBackgroundColor:
        return TRUE;
    case PLeftBorder:
        if ( column() == m_pObscuringCell->column() )
            return TRUE;
        break;
    case PRightBorder:
        if ( column() == m_pObscuringCell->column() + m_pObscuringCell->extraXCells() )
            return TRUE;
        break;
    case PTopBorder:
        if ( row() == m_pObscuringCell->row() )
            return TRUE;
        break;
    case PBottomBorder:
        if ( row() == m_pObscuringCell->row() + m_pObscuringCell->extraYCells() )
            return TRUE;
        break;
    default:
        return FALSE;
    }

    return FALSE;
}

const QBrush& KSpreadCell::backGroundBrush( int _col, int _row ) const
{
    if ( m_pObscuringCell )
    {
        // Ask the obscuring cell for a right border
        if ( m_pObscuringCell->hasProperty( PBackgroundBrush ) )
            return m_pObscuringCell->backGroundBrush( m_pObscuringCell->column(), m_pObscuringCell->row() );

        return m_pTable->emptyBrush();
    }

    return KSpreadLayout::backGroundBrush( _col, _row );
}

const QColor& KSpreadCell::bgColor( int _col, int _row ) const
{
    if ( m_pObscuringCell )
    {
        // Ask the obscuring cell for a right border
        if ( m_pObscuringCell->hasProperty( PBackgroundColor ) )
            return m_pObscuringCell->bgColor( m_pObscuringCell->column(), m_pObscuringCell->row() );

        return m_pTable->emptyColor();
    }

    return KSpreadLayout::bgColor( _col, _row );
}

///////////////////////////////////////////
//
// Borders.
// Reimplementation of KSpreadLayout methods.
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
    if ( m_pObscuringCell )
    {
        // Is this cell at the right border of the "big" joined cell ?
        // If not then there is no right border.
        if ( _col != m_pObscuringCell->column() + m_pObscuringCell->extraXCells() )
            return m_pTable->emptyPen();

        // Ask the obscuring cell for a rigth border
        if ( m_pObscuringCell->hasProperty( PRightBorder ) )
            return m_pObscuringCell->rightBorderPen( m_pObscuringCell->column(), m_pObscuringCell->row() );

        // Ask the cell on the right
        KSpreadCell * cell = m_pTable->cellAt( _col + 1, _row );
        if ( cell->hasProperty( PLeftBorder ) )
            return cell->leftBorderPen( _col + 1, _row );

        return m_pTable->emptyPen();
    }

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
    if ( m_pObscuringCell )
    {
        // Is this cell at the left border of the "big" joined cell ?
        // If not then there is no left border.
        if ( _col != m_pObscuringCell->column() )
            return m_pTable->emptyPen();

        // Ask the obscuring cell for a left border
        if ( m_pObscuringCell->hasProperty( PLeftBorder ) )
            return m_pObscuringCell->leftBorderPen( m_pObscuringCell->column(), m_pObscuringCell->row() );

        // Ask the cell on the left
        KSpreadCell * cell = m_pTable->cellAt( _col - 1, _row );
        if ( cell->hasProperty( PRightBorder ) )
            return cell->rightBorderPen( _col - 1, _row );

        return m_pTable->emptyPen();
    }

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
    if ( m_pObscuringCell )
    {
        // Is this cell at the bottom border of the "big" joined cell ?
        // If not then there is no bottom border.
        if ( _row != m_pObscuringCell->row() + m_pObscuringCell->extraYCells() )
            return m_pTable->emptyPen();

        // Ask the obscuring cell for a bottom border
        if ( m_pObscuringCell->hasProperty( PBottomBorder ) )
            return m_pObscuringCell->bottomBorderPen( m_pObscuringCell->column(), m_pObscuringCell->row() );

        // Ask the cell below
        KSpreadCell * cell = m_pTable->cellAt( _col, _row + 1 );
        if ( cell->hasProperty( PTopBorder ) )
            return cell->topBorderPen( _col, _row + 1 );

        return m_pTable->emptyPen();
    }

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
    if ( m_pObscuringCell )
    {
        // Is this cell at the top border of the "big" joined cell ?
        // If not then there is no top border.
        if ( _row != m_pObscuringCell->row() )
            return m_pTable->emptyPen();

        // Ask the obscuring cell for a top border
        if ( m_pObscuringCell->hasProperty( PTopBorder ) )
            return m_pObscuringCell->topBorderPen( m_pObscuringCell->column(), m_pObscuringCell->row() );

        // Ask the cell above
        KSpreadCell * cell = m_pTable->cellAt( _col, _row - 1 );
        if ( cell->hasProperty( PBottomBorder ) )
            return cell->bottomBorderPen( _col, _row - 1 );

        return m_pTable->emptyPen();
    }

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
  int tmpPreci=precision(column(),row());
  if ( tmpPreci == -1 )
  {
    int pos = m_strOutText.find(decimal_point);
    if ( pos == -1 )
      setPrecision(1);
    else
    {
      int start=0;
      if(m_strOutText.find('%')!=-1)
        start=2;
      else if(m_strOutText.find(locale()->currencySymbol())==(m_strOutText.length()-locale()->currencySymbol().length()))
        start=locale()->currencySymbol().length()+1;
      else if((start=m_strOutText.find('E'))!=-1)
        start=m_strOutText.length()-start;
      else
        start=0;
      setPrecision(m_strOutText.length() - pos-start);
      if ( tmpPreci < 0 )
        setPrecision(0);
    }
    m_bLayoutDirtyFlag = TRUE;
  }
  else if ( tmpPreci < 10 )
  {
    setPrecision(++tmpPreci);
    m_bLayoutDirtyFlag = TRUE;
  }
}

void KSpreadCell::decPrecision()
{
  if ( !isValue() )
    return;
  int preciTmp=precision(column(),row());
  if ( precision(column(),row()) == -1 )
  {
    int pos = m_strOutText.find(decimal_point);
    int start=0;
    if(m_strOutText.find('%')!=-1)
        start=2;
    else if(m_strOutText.find(locale()->currencySymbol())==(m_strOutText.length()-locale()->currencySymbol().length()))
        start=locale()->currencySymbol().length()+1;
    else if((start=m_strOutText.find('E'))!=-1)
        start=m_strOutText.length()-start;
    else
        start=0;
    if ( pos == -1 )
      return;
    setPrecision(m_strOutText.length() - pos - 2-start);
    if ( preciTmp < 0 )
      setPrecision(preciTmp );
    m_bLayoutDirtyFlag = TRUE;
  }
  else if ( preciTmp > 0 )
  {
    setPrecision(--preciTmp);
    m_bLayoutDirtyFlag = TRUE;
  }
}



void KSpreadCell::setCellText( const QString& _text, bool updateDepends )
{
  QString oldText=m_strText;
  setDisplayText( _text, updateDepends );
  bool result;
  if(!m_pTable->isLoading())
        {
        result=testValidity();
        if(!result)
                {
                //reapply old value if action == stop
                setDisplayText( oldText, updateDepends );
                }
        }

}


void KSpreadCell::setDisplayText( const QString& _text, bool updateDepends )
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

    // David: don't change what the user typed !
    //checkFormat(true);

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
    m_bError =false;
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
    m_bError =false;
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

bool KSpreadCell::testValidity()
{
bool valid=false;
if(m_Validity!=0)
        {
        if(m_Validity->m_allow==Allow_Number)
                {
                if(isValue())
                        {
                        switch( m_Validity->m_cond)
                                {
                                case Equal:
                                        if(m_dValue ==m_Validity->valMin)
                                                valid=true;
                                        break;
                                case Superior:
                                        if(m_dValue >m_Validity->valMin)
                                                valid=true;
                                        break;
                                case Inferior:
                                        if(m_dValue <m_Validity->valMin)
                                                valid=true;
                                        break;
                                case SuperiorEqual:
                                        if(m_dValue >=m_Validity->valMin)
                                                valid=true;
                                        break;
                                case InferiorEqual:
                                        if(m_dValue <=m_Validity->valMin)
                                                valid=true;
                                        break;
                                case Between:
                                        if(m_dValue >=m_Validity->valMin && m_dValue <=m_Validity->valMax)
                                                valid=true;
                                        break;
                                case Different:
                                        if(m_dValue <m_Validity->valMin || m_dValue >m_Validity->valMax)
                                                valid=true;
                                        break;
                                default :
                                        break;

                                }

                        }

                }
        else if(m_Validity->m_allow==Allow_Text)
                {
                 if(!isValue() && !isBool() && !isDate() && !isTime())
                        valid=true;
                }
        else if(m_Validity->m_allow==Allow_Time)
                {
                 if(isTime())
                        valid=true;
                }
        else if(m_Validity->m_allow==Allow_Date)
                {
                 if(isDate())
                        valid=true;
                }

        }
else
        valid= true;

if(!valid &&m_Validity!=0 )
        {
        switch (m_Validity->m_action)
                {
                case Stop:
                        KMessageBox::error((QWidget*)0L , m_Validity->message,m_Validity->title);
                        break;
                case Warning:
                        KMessageBox::warningYesNo((QWidget*)0L , m_Validity->message,m_Validity->title);
                        break;
                case Information:
                        KMessageBox::information((QWidget*)0L , m_Validity->message,m_Validity->title);
                        break;
                }
        }
if(!valid && m_Validity!=0 && m_Validity->m_action==Stop)
        return false;
else
        return true;
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
    m_bError =false;
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

    updateChart();
}

bool KSpreadCell::updateChart(bool refresh)
{
    // Update a chart for example if it depends on this cell.
    if ( m_iRow != 0 && m_iColumn != 0 )
    {
        CellBinding *bind;
        for ( bind = m_pTable->firstCellBinding(); bind != 0L; bind = m_pTable->nextCellBinding() )
        {
            if ( bind->contains( m_iColumn, m_iRow ) )
                {
                if(!refresh)
                        return true;

                bind->cellChanged( this );
                return true;
                }
        }
    }
    return false;

}

void KSpreadCell::checkValue()
{
    m_bError =false;
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
    QString p = m_strText;
    if ( m_style == ST_Select )
      p = ((SelectPrivate*)m_pPrivate)->text();
    else if ( isFormular() )
        p = m_strFormularOut; // this should never happen

    // If the output is empty, we dont have a value
    if ( p.isEmpty() )
    {
      return;
    }

    // Treat anything starting with a quote as a litteral text
    if( p.at(0)=='\'')
    {
        m_strOutText=p.right(p.length()-1);
        setFormatNumber(Number); // default format
        return;
    }

    // Test for boolean
    if ( p == "true" )
    {
      m_dValue = 1.0;
      m_bBool = true;
      return;
    }
    else if ( p == "false" )
    {
      m_dValue = 0.0;
      m_bBool = true;
      return;
    }
    m_bBool = false;

    // Test whether it is a numeric value
    m_bValue = TRUE;

    // First try to understand the number using the locale
    double value = locale()->readNumber(p, &m_bValue);
    // If not, try with the '.' as decimal separator
    if (!m_bValue)
        value = p.toDouble(&m_bValue);

    if ( m_bValue )
    {
        m_dValue = value;
        if ( m_strText.contains('E') || m_strText.contains('e') )
            setFormatNumber(Scientific);
        else
            checkNumberFormat();
            //setFormatNumber(Number);
        return;
    }

    QString str = m_strText.stripWhiteSpace();
    //test if text is a percent value
    if(str.at(str.length()-1)=='%')
    {
        str=str.left(str.length()-1);
        // First try to understand the number using the locale
        double value = locale()->readNumber(str, &m_bValue);
        // If not, try with the '.' as decimal separator
        if (!m_bValue)
            value = str.toDouble(&m_bValue);

        if (m_bValue)
        {
            m_dValue=value/100.0;
            setFormatNumber(Percentage);
            setFaktor(100.0);
            setPrecision(0);
            return;
        }
    }

/*
    QString tmp;
    QString tmpCurrency=locale()->currencySymbol();
    int pos=0;
    bool ok=false;
    double val=0;
    // TODO: why not use KLocale here too ?
    if((pos=m_strText.find(tmpCurrency))!=-1)
        {
        if(pos==0) // example $ 154.545
                {
                tmp=m_strText.right(m_strText.length()-tmpCurrency.length());
                tmp=tmp.simplifyWhiteSpace();
                val=tmp.toDouble(&ok);
                if(ok)
                        {
                        m_bValue=true;
                        m_dValue=val;
                        setFormatNumber(Money);
                        m_strText=tmp.setNum(m_dValue);
                        setFaktor(1.0);
                        setPrecision(2);
                        return;
                        }
                }
        else if((unsigned int)pos==(m_strText.length()-tmpCurrency.length())) //example 125.55 F
                {
                tmp=m_strText.left(m_strText.length()-tmpCurrency.length());
                tmp=tmp.simplifyWhiteSpace();
                val=tmp.toDouble(&ok);
                if(ok)
                        {
                        m_bValue=true;
                        m_dValue=val;
                        setFormatNumber(Money);
                        m_strText=tmp.setNum(m_dValue);
                        setFaktor(1.0);
                        setPrecision(2);
                        return;
                        }
                }
        }
        */
    double money = locale()->readMoney(m_strText, &m_bValue);
    if (m_bValue)
    {
        m_dValue=money;
        setFormatNumber(Money);
        //m_strText=tmp.setNum(m_dValue); ?
        setFaktor(1.0);
        setPrecision(2);
        return;
    }

    QString tmp;
    QTime tmpTime;
    QString stringPm=i18n("pm");
    QString stringAm=i18n("am");
    int pos=0;
    bool valid=false;
    if((tmpTime=locale()->readTime(m_strText)).isValid())
        {
        valid=true;
        }
    else if(locale()->use12Clock())
    {
    if((pos=m_strText.find(stringPm))!=-1)
        {
         tmp=m_strText.mid(0,m_strText.length()-stringPm.length());
         tmp=tmp.simplifyWhiteSpace();
         if((tmpTime=locale()->readTime(tmp+" "+stringPm)).isValid())
                {
                valid=true;
                }
         else if((tmpTime=locale()->readTime(tmp+":00 "+stringPm)).isValid())
                {
                valid=true;
                }
        }
    else if((pos=m_strText.find(stringAm))!=-1)
        {
         tmp=m_strText.mid(0,m_strText.length()-stringAm.length());
         tmp=tmp.simplifyWhiteSpace();
         if((tmpTime=locale()->readTime(tmp+" "+stringAm)).isValid())
                {
                valid=true;
                }
         else if((tmpTime=locale()->readTime(tmp+":00 "+stringAm)).isValid())
                {
                valid=true;
                }
        }
    }
    if(valid)
    {
        m_bTime = true;
        m_dValue = 0;
        formatNumber tmpFormat=getFormatNumber(column(),row());
        if( tmpFormat!=SecondeTime &&tmpFormat!=Time_format1
        && tmpFormat!=Time_format2 && tmpFormat!=Time_format3)
                setFormatNumber(Time);
        m_Time=tmpTime;
        m_strText=locale()->formatTime(m_Time,true);
        return;
    }

    QDate tmpDate;
    if((tmpDate=locale()->readDate(m_strText)).isValid())
    {
        m_bDate = true;
        m_dValue = 0;
        formatNumber tmpFormat=getFormatNumber(column(),row());
        if(tmpFormat!=TextDate &&
        !(tmpFormat>=200&&tmpFormat<=215))
                setFormatNumber(ShortDate);
        m_Date=tmpDate;
        m_strText=locale()->formatDate(m_Date,true); //short format date
        return;
    }

    //default format
    setFormatNumber(Number);
}

void KSpreadCell::checkNumberFormat()
{
    if(getFormatNumber(column(),row())==Number)
    {
    if ( m_bValue )
        {
        if ( m_dValue > 1e+10 )
            setFormatNumber( Scientific );
        else
            setFormatNumber( Number );
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


QDomElement KSpreadCell::saveParameters( QDomDocument& doc ) const
{
    QDomElement format = doc.createElement( "format" );
    if ( align(m_iColumn,m_iRow)!= Undefined)
        format.setAttribute( "align", (int)align(m_iColumn,m_iRow) );
    if ( alignY(m_iColumn,m_iRow) != Middle)
        format.setAttribute( "alignY", (int)alignY(m_iColumn,m_iRow) );
    if (bgColor( m_iColumn,m_iRow ).isValid())
        format.setAttribute( "bgcolor", bgColor(m_iColumn,m_iRow).name() );
    if (multiRow( m_iColumn,m_iRow ))
        format.setAttribute( "multirow", "yes" );
    if ( verticalText(m_iColumn,m_iRow) )
        format.setAttribute( "verticaltext", "yes" );
    if ( precision(m_iColumn,m_iRow)!=-1)
        format.setAttribute( "precision", precision(m_iColumn,m_iRow) );
    if (  !prefix( m_iColumn,m_iRow ).isEmpty() )
        format.setAttribute( "prefix", prefix( m_iColumn,m_iRow ) );
    if (  !postfix( m_iColumn,m_iRow ).isEmpty() )
        format.setAttribute( "postfix", postfix( m_iColumn,m_iRow ) );
    format.setAttribute( "float", (int)floatFormat(m_iColumn,m_iRow ) );
    format.setAttribute( "floatcolor", (int)floatColor(m_iColumn,m_iRow )  );
    if ( faktor(m_iColumn,m_iRow )!=1.0 )
        format.setAttribute( "faktor", faktor(m_iColumn,m_iRow ) );
    if ( getFormatNumber(m_iColumn,m_iRow )!= Number)
        format.setAttribute( "format",(int) getFormatNumber(m_iColumn,m_iRow ));
    if ( getAngle(m_iColumn,m_iRow )!=0)
        format.setAttribute( "angle", getAngle(m_iColumn,m_iRow ) );
    if ( getIndent(m_iColumn,m_iRow )!=0 )
        format.setAttribute( "indent", getIndent(m_iColumn,m_iRow ) );
    format.appendChild( createElement( "font", textFont( m_iColumn,m_iRow ), doc ) );
    if ( textPen( m_iColumn,m_iRow).color().isValid())
        format.appendChild( createElement( "pen", textPen( m_iColumn,m_iRow), doc ) );
    if ( backGroundBrush(m_iColumn,m_iRow).color().isValid())
        format.setAttribute( "brushcolor", backGroundBrush(m_iColumn,m_iRow).color().name() );
    format.setAttribute( "brushstyle",(int)backGroundBrush(m_iColumn,m_iRow).style() );

    QDomElement left = doc.createElement( "left-border" );
    left.appendChild( createElement( "pen", leftBorderPen(m_iColumn,m_iRow), doc ) );
    format.appendChild( left );

    QDomElement top = doc.createElement( "top-border" );
    top.appendChild( createElement( "pen",topBorderPen(m_iColumn,m_iRow), doc ) );
    format.appendChild( top );

    QDomElement right = doc.createElement( "right-border" );
    right.appendChild( createElement( "pen", rightBorderPen(m_iColumn,m_iRow), doc ) );
    format.appendChild( right );

    QDomElement bottom = doc.createElement( "bottom-border" );
    bottom.appendChild( createElement( "pen", bottomBorderPen(m_iColumn,m_iRow), doc ) );
    format.appendChild( bottom );

    QDomElement fallDiagonal  = doc.createElement( "fall-diagonal" );
    fallDiagonal.appendChild( createElement( "pen", fallDiagonalPen(m_iColumn,m_iRow), doc ) );
    format.appendChild( fallDiagonal );

    QDomElement goUpDiagonal = doc.createElement( "up-diagonal" );
    goUpDiagonal.appendChild( createElement( "pen", goUpDiagonalPen(m_iColumn,m_iRow), doc ) );
    format.appendChild( goUpDiagonal );

return format;
}



QDomElement KSpreadCell::save( QDomDocument& doc, int _x_offset, int _y_offset, bool fallBack )
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
    QDomElement format;
    if( !fallBack)
        format = KSpreadLayout::save( doc );
    else
        format=saveParameters( doc );
    cell.appendChild( format );

    if ( isForceExtraCells() )
    {
        if ( extraXCells() )
            format.setAttribute( "colspan", extraXCells() );
        if ( extraYCells() )
            format.setAttribute( "rowspan", extraYCells() );
    }
    if ( style() )
        format.setAttribute( "style", (int)m_style );


    if( ( m_firstCondition != 0 ) || ( m_secondCondition != 0 ) || ( m_thirdCondition != 0 ) )
    {
        QDomElement condition = doc.createElement("condition");

        if( m_firstCondition != 0 )
        {
            QDomElement first=doc.createElement("first");
            first.setAttribute("cond",(int)m_firstCondition->m_cond);
            first.setAttribute("val1",m_firstCondition->val1);
            first.setAttribute("val2",m_firstCondition->val2);
            first.setAttribute("color",m_firstCondition->colorcond.name());
            first.appendChild( createElement( "font", m_firstCondition->fontcond, doc ) );

            condition.appendChild(first);
        }
        if( m_secondCondition != 0 )
        {
            QDomElement second=doc.createElement("second");
            second.setAttribute("cond",(int)m_secondCondition->m_cond);
            second.setAttribute("val1",m_secondCondition->val1);
            second.setAttribute("val2",m_secondCondition->val2);
            second.setAttribute("color",m_secondCondition->colorcond.name());
            second.appendChild( createElement( "font", m_secondCondition->fontcond, doc ) );


            condition.appendChild(second);
        }
        if( m_thirdCondition != 0 )
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

    if( m_Validity!=0 )
    {
        QDomElement validity = doc.createElement("validity");

        QDomElement param=doc.createElement("param");
        param.setAttribute("cond",(int)m_Validity->m_cond);
        param.setAttribute("action",(int)m_Validity->m_action);
        param.setAttribute("allow",(int)m_Validity->m_allow);
        param.setAttribute("valmin",m_Validity->valMin);
        param.setAttribute("valmax",m_Validity->valMax);
        validity.appendChild(param);
        QDomElement title = doc.createElement( "title" );
        title.appendChild( doc.createTextNode( m_Validity->title ) );
        validity.appendChild( title );
        QDomElement message = doc.createElement( "message" );
        message.appendChild( doc.createCDATASection( m_Validity->message ) );
        validity.appendChild( message );

        cell.appendChild( validity );
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
        else if( (getFormatNumber(m_iColumn,m_iRow)==ShortDate || getFormatNumber(m_iColumn,m_iRow)==TextDate
        || (getFormatNumber(m_iColumn,m_iRow)>=200 && getFormatNumber(m_iColumn,m_iRow)<=215))&& m_bDate )
        {
            QDomElement text = doc.createElement( "text" );
            QString tmp;
            tmp=tmp.setNum(m_Date.year())+"/"+tmp.setNum(m_Date.month())+"/"+tmp.setNum(m_Date.day());
            text.appendChild( doc.createTextNode( tmp ) );
            cell.appendChild( text );
        }
        else if( (getFormatNumber(m_iColumn,m_iRow)==Time || getFormatNumber(m_iColumn,m_iRow)==SecondeTime
        || getFormatNumber(m_iColumn,m_iRow)==Time_format1
        || getFormatNumber(m_iColumn,m_iRow)==Time_format2
        || getFormatNumber(m_iColumn,m_iRow)==Time_format3 )&& m_bTime )
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
        // send pm parameter. Didn't load Borders if pm==NoBorder
        if ( !KSpreadLayout::load( f,pm ) )
            return false;

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

    QDomElement validity = cell.namedItem( "validity" ).toElement();
    if ( !validity.isNull())
    {
        QDomElement param = validity.namedItem( "param" ).toElement();
        if(!param.isNull())
        {
        m_Validity=new KSpreadValidity;
        if ( param.hasAttribute( "cond" ) )
            {
            m_Validity->m_cond=(Conditional) param.attribute("cond").toInt( &ok );
            if ( !ok )
                return false;
            }
         if ( param.hasAttribute( "action" ) )
            {
            m_Validity->m_action=(Action) param.attribute("action").toInt( &ok );
            if ( !ok )
                return false;
            }
         if ( param.hasAttribute( "allow" ) )
            {
            m_Validity->m_allow=(Allow) param.attribute("allow").toInt( &ok );
            if ( !ok )
                return false;
            }
         if ( param.hasAttribute( "valmin" ) )
            {
            m_Validity->valMin=param.attribute("valmin").toDouble( &ok );
            if ( !ok )
                return false;
            }
         if ( param.hasAttribute( "valmax" ) )
            {
            m_Validity->valMax=param.attribute("valmax").toDouble( &ok );
            if ( !ok )
                return false;
            }
        }
        QDomElement title = validity.namedItem( "title" ).toElement();
        if(!title.isNull())
        {
                 m_Validity->title= title.text();
        }
        QDomElement message = validity.namedItem( "message" ).toElement();
        if(!message.isNull())
        {
                 m_Validity->message= message.text();
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
            t = decodeFormular( t.latin1(), m_iColumn, m_iRow );

            // Set the cell's text, and don't calc dependencies yet (see comment for setCellText)
            setCellText( pasteOperation( t, m_strText, op ), false );
        }
        // A date
        else if( getFormatNumber(m_iColumn,m_iRow) == ShortDate || getFormatNumber(m_iColumn,m_iRow) == TextDate
        || (getFormatNumber(m_iColumn,m_iRow)>=200 && getFormatNumber(m_iColumn,m_iRow)<=215))
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
                setCellText( locale()->formatDate( m_Date, true ), false );
            else
                setCellText( pasteOperation( t, m_strText, op ), false );
        }
        // A Time
        else if( getFormatNumber(m_iColumn,m_iRow) == Time || getFormatNumber(m_iColumn,m_iRow) == SecondeTime
        || getFormatNumber(m_iColumn,m_iRow) == Time_format1|| getFormatNumber(m_iColumn,m_iRow) == Time_format2
        || getFormatNumber(m_iColumn,m_iRow) == Time_format3 )
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
                setCellText(locale()->formatTime(m_Time,true),false);
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
  m_pPrivate = 0;

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

    if(m_firstCondition!=0)
        delete m_firstCondition;
    if(m_thirdCondition!=0)
        delete m_thirdCondition;
    if(m_secondCondition!=0)
        delete m_secondCondition;

    if(m_Validity!=0)
        delete m_Validity;

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

