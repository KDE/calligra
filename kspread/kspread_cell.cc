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
#include <ctype.h>
#include <float.h>
#include <math.h>

#include <qapplication.h>
#include <qsimplerichtext.h>
#include <qpopupmenu.h>

#include "kspread_global.h"
#include "kspread_canvas.h"
#include "kspread_map.h"
#include "kspread_doc.h"
#include "kspread_util.h"

#include <kmessagebox.h>

#include <kdebug.h>

#define DO_UPDATE m_pTable->updateCell( this, m_iColumn, m_iRow )

QChar KSpreadCell::decimal_point = '\0';

/*****************************************************************************
 *
 * KSpreadCell
 *
 *****************************************************************************/

KSpreadCell::KSpreadCell( KSpreadTable *_table, int _column, int _row )
  : KSpreadLayout( _table ),
    conditions(this),
    m_bShrinkToSize(false)
{
  m_nextCell = 0;
  m_previousCell = 0;

  m_pCode = 0;
  m_pPrivate = 0L;
  m_pQML = NULL;

  m_ObscuringCells.clear();

  m_lstDepends.setAutoDelete( TRUE );
  m_lstDependingOnMe.setAutoDelete( TRUE );

  m_content = Text;
  m_dataType = StringData; // we use this for empty cells
  m_dValue = 0.0;

  m_iRow = _row;
  m_iColumn = _column;

  m_style = ST_Normal;
  m_iExtraXCells = 0;
  m_iExtraYCells = 0;
  m_iMergedXCells = 0;
  m_iMergedYCells = 0;
  m_iExtraWidth = 0;
  m_iExtraHeight = 0;
  m_iPrecision = -1;
  m_iOutTextWidth = 0;
  m_iOutTextHeight = 0;

  m_nbLines=0;
  m_Validity=0;

  clearAllErrors();
}

int KSpreadCell::row() const
{
  /* Make sure this isn't called for the default cell.  This assert can save you
     (could have saved me!) the hassle of some very obscure bugs.
  */
  Q_ASSERT(!isDefault());
  return m_iRow;
}

int KSpreadCell::column() const
{
  Q_ASSERT(!isDefault());
  /* Make sure this isn't called for the default cell.  This assert can save you
     (could have saved me!) the hassle of some very obscure bugs.
  */
  return m_iColumn;
}

void KSpreadCell::copyLayout( KSpreadCell *_cell )
{
    copyLayout( _cell->column(), _cell->row() );
}

void KSpreadCell::copyLayout( int _column, int _row )
{
    KSpreadCell * cell = m_pTable->cellAt( _column, _row );

    setAlign( cell->align( _column, _row ) );
    setAlignY( cell->alignY( _column, _row ) );
    setTextFont( cell->textFont( _column, _row ) );
    setTextColor( cell->textColor( _column, _row ) );
    setBgColor( cell->bgColor( _column, _row) );
    setLeftBorderPen(cell->leftBorderPen( _column, _row ));
    setTopBorderPen(cell->topBorderPen( _column, _row ));
    setBottomBorderPen(cell->bottomBorderPen( _column, _row ));
    setRightBorderPen(cell->rightBorderPen( _column, _row ));
    setFallDiagonalPen(cell->fallDiagonalPen( _column, _row ));
    setGoUpDiagonalPen(cell->goUpDiagonalPen( _column, _row ));
    setBackGroundBrush(cell->backGroundBrush( _column, _row));

    setPrecision( cell->precision( _column, _row ) );
    setPrefix( cell->prefix( _column, _row ) );
    setPostfix( cell->postfix( _column, _row ) );
    setFloatFormat( cell->floatFormat( _column, _row ) );
    setFloatColor( cell->floatColor( _column, _row ) );
    setFactor( cell->factor( _column, _row ) );
    setMultiRow( cell->multiRow( _column, _row ) );
    setVerticalText( cell->verticalText( _column, _row ) );
    setStyle( cell->style());
    setDontPrintText(cell->getDontprintText(_column, _row ) );
    setIndent( cell->getIndent(_column, _row ) );

    QValueList<KSpreadConditional> conditionList = cell->GetConditionList();
    conditions.SetConditionList(conditionList);

    setComment( cell->comment(_column, _row) );
    setAngle( cell->getAngle(_column, _row) );
    setFormatType( cell->getFormatType(_column, _row) );
}

void KSpreadCell::copyAll( KSpreadCell *cell )
{
    Q_ASSERT( !isDefault() ); // trouble ahead...
    copyLayout( cell );
    copyContent( cell );
}

void KSpreadCell::copyContent( KSpreadCell* cell )
{
    Q_ASSERT( !isDefault() ); // trouble ahead...

    if (cell->isFormula() && cell->column() > 0 && cell->row() > 0)
    {
      // change all the references, e.g. from A1 to A3 if copying
      // from e.g. B2 to B4
      QString d = cell->encodeFormula();
      setCellText( cell->decodeFormula( d ), true );
    }
    else
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
  defaultStyleLayout();

  QValueList<KSpreadConditional> emptyList;
  conditions.SetConditionList(emptyList);

  if(m_Validity != NULL)
        delete m_Validity;
  m_Validity = NULL;
}

void KSpreadCell::layoutChanged()
{
  setFlag(Flag_LayoutDirty);
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
        cell->unobscure(this);
      }

  // disable forcing ?
  if ( _x == 0 && _y == 0 )
  {
      clearFlag(Flag_ForceExtra);
      m_iExtraXCells = 0;
      m_iExtraYCells = 0;
      m_iExtraWidth = 0;
      m_iExtraHeight = 0;
      m_iMergedXCells = 0;
      m_iMergedYCells = 0;
      return;
  }

    setFlag(Flag_ForceExtra);
    m_iExtraXCells = _x;
    m_iExtraYCells = _y;
    m_iMergedXCells = _x;
    m_iMergedYCells = _y;

    // Obscure the cells
    for( int x = _col; x <= _col + _x; x++ )
        for( int y = _row; y <= _row + _y; y++ )
            if ( x != _col || y != _row )
            {
                KSpreadCell *cell = m_pTable->nonDefaultCell( x, y );
                cell->obscure( this, true );
            }

    // Refresh the layout
    // QPainter painter;
    // painter.begin( m_pTable->gui()->canvasWidget() );

    setFlag(Flag_LayoutDirty);
}

void KSpreadCell::move( int col, int row )
{
    setLayoutDirtyFlag();
    setCalcDirtyFlag();
    setDisplayDirtyFlag();

    //int ex = extraXCells();
    //int ey = extraYCells();

    m_ObscuringCells.clear();

    // Unobscure the objects we obscure right now
    for( int x = m_iColumn; x <= m_iColumn + m_iExtraXCells; x++ )
        for( int y = m_iRow; y <= m_iRow + m_iExtraYCells; y++ )
            if ( x != m_iColumn || y != m_iRow )
            {
                KSpreadCell *cell = m_pTable->nonDefaultCell( x, y );
                cell->unobscure(this);
            }

    m_iColumn = col;
    m_iRow = row;

    m_iExtraXCells = 0;
    m_iExtraYCells = 0;
    m_iMergedXCells = 0;
    m_iMergedYCells = 0;

    // Reobscure cells if we are forced to do so.
    //if ( m_bForceExtraCells )
      //  forceExtraCells( col, row, ex, ey );
}

void KSpreadCell::setLayoutDirtyFlag()
{
    setFlag(Flag_LayoutDirty);

    KSpreadCell* cell = NULL;
    for (cell = m_ObscuringCells.first(); cell != NULL;
         cell = m_ObscuringCells.next() )
    {
      cell->setLayoutDirtyFlag();
    }
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
    return isDefault() || m_strText.isEmpty();
}


bool KSpreadCell::isObscured() const
{
  return !( m_ObscuringCells.isEmpty() );
}

bool KSpreadCell::isObscuringForced()
{
  KSpreadCell *cell = NULL;

  for (cell = m_ObscuringCells.first(); cell != NULL;
       cell = m_ObscuringCells.next())
  {
    if (cell->isForceExtraCells())
    {
      /* the cell might force extra cells, and then overlap even beyond that
         so just knowing that the obscuring cell forces extra isn't enough.
         We have to know that this cell is one of the ones it is forcing over.
      */
      if (column() <= cell->column() + cell->mergedXCells() &&
          row() <= cell->row() + cell->mergedYCells())
      {
        return true;
      }
    }
  }
  return false;
}

void KSpreadCell::obscure( KSpreadCell *cell, bool isForcing )
{
  while (m_ObscuringCells.removeRef(cell));

  if (isForcing)
  {
    m_ObscuringCells.prepend(cell);
  }
  else
  {
    m_ObscuringCells.append(cell);
  }
}

void KSpreadCell::unobscure( KSpreadCell *cell )
{
  m_ObscuringCells.remove(cell);
  setFlag(Flag_LayoutDirty);
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

    QPopupMenu *popup = new QPopupMenu(_canvas);
    SelectPrivate *s = (SelectPrivate*)m_pPrivate;

    int id = 0;
    QStringList::ConstIterator it = s->m_lstItems.begin();
    for( ; it != s->m_lstItems.end(); ++it )
        popup->insertItem( *it, id++ );
    QObject::connect( popup, SIGNAL( activated( int ) ),
                      s, SLOT( slotItemSelected( int ) ) );
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
    //delete popup;
    return;
  }

  if ( m_strAction.isEmpty() )
    return;

  KSContext context;
  QPtrList<KSpreadDependency> lst;
  lst.setAutoDelete( TRUE );
  KSParseNode* code = m_pTable->doc()->interpreter()->parse( context, m_pTable, m_strAction, lst );
  // Did a syntax error occur ?
  if ( context.exception() )
  {
    kdDebug(36001) << "Failed action in cell " <<
      util_cellName(m_iColumn, m_iRow) << endl;
    if (m_pTable->doc()->getShowMessageError())
    {
      QString tmp(i18n("Error in cell %1\n\n"));
      tmp = tmp.arg( util_cellName( m_pTable, m_iColumn, m_iRow ) );
      tmp += context.exception()->toString( context );
      KMessageBox::error((QWidget*)0L , tmp);
    }
    return;
  }

  KSContext& context2 = m_pTable->doc()->context();
  if ( !m_pTable->doc()->interpreter()->evaluate( context2, code, m_pTable ) )
      // Print out exception if any
      if ( context2.exception() &&m_pTable->doc()->getShowMessageError())
      {
          QString tmp(i18n("Error in cell %1\n\n"));
          tmp = tmp.arg( util_cellName( m_pTable, m_iColumn, m_iRow ) );
          tmp += context2.exception()->toString( context2 );
          KMessageBox::error( (QWidget*)0L, tmp);
      }
}

QString KSpreadCell::encodeFormula( int _col, int _row )
{
    if ( _col == -1 )
        _col = m_iColumn;
    if ( _row == -1 )
        _row = m_iRow;

    QString erg = "";

    if(m_strText.isEmpty())
        return m_strText;

    bool fix1 = FALSE;
    bool fix2 = FALSE;
    bool onNumber = false;
    unsigned int pos = 0;
    const unsigned int length = m_strText.length();

    // All this can surely be made 10 times faster, but I just "ported" it to QString
    // without any attempt to optimize things -- this is really brittle (Werner)
    while ( pos < length )
    {
        if ( m_strText[pos] == '"' )
        {
            erg += m_strText[pos++];
            while ( pos < length && m_strText[pos] != '"' )  // till the end of the world^H^H^H "string"
            {
                erg += m_strText[pos++];
                // Allow escaped double quotes (\")
                if ( pos < length && m_strText[pos] == '\\' && m_strText[pos+1] == '"' )
                {
                    erg += m_strText[pos++];
                    erg += m_strText[pos++];
                }
            }
            if ( pos < length )  // also copy the trailing double quote
                erg += m_strText[pos++];

            onNumber = false;
        }
        else if ( m_strText[pos].isDigit() )
        {
          erg += m_strText[pos++];
          fix1 = fix2 = FALSE;
          onNumber = true;
        }
        else if ( m_strText[pos] != '$' && !m_strText[pos].isLetter() )
        {
            erg += m_strText[pos++];
            fix1 = fix2 = FALSE;
            onNumber = false;
        }
        else
        {
            QString tmp = "";
            if ( m_strText[pos] == '$' )
            {
                tmp = "$";
                pos++;
                fix1 = TRUE;
            }
            if ( m_strText[pos].isLetter() )
            {
                QString buffer;
                unsigned int pos2 = 0;
                while ( pos < length && m_strText[pos].isLetter() )
                {
                    tmp += m_strText[pos];
                    buffer[pos2++] = m_strText[pos++];
                }
                if ( m_strText[pos] == '$' )
                {
                    tmp += "$";
                    pos++;
                    fix2 = TRUE;
                }
                if ( m_strText[pos].isDigit() )
                {
                    const unsigned int oldPos = pos;
                    while ( pos < length && m_strText[pos].isDigit() ) ++pos;
                    int row = 0;
                    if ( pos != oldPos )
                        row = m_strText.mid(oldPos, pos-oldPos).toInt();
                    // Is it a table name || is it a function name like DEC2HEX
                    /* or if we're parsing a number, this could just be the
                       exponential part of it  (1.23E4) */
                    if ( ( m_strText[pos] == '!' ) ||
                         m_strText[pos].isLetter() ||
                         onNumber )
                    {
                        erg += tmp;
                        fix1 = fix2 = FALSE;
                        pos = oldPos;
                    }
                    else // It must be a cell identifier
                    {
                        //now calculate the row as integer value
                        int col = 0;
                        col = util_decodeColumnLabelText( buffer );
                        if ( fix1 )
                            erg += QString( "$%1" ).arg( col );
                        else
                            erg += QString( "#%1" ).arg( col - _col );

                        if ( fix2 )
                            erg += QString( "$%1#").arg( row );
                        else
                            erg += QString( "#%1#" ).arg( row - _row );
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
            onNumber = false;
        }
    }

    return erg;
}

QString KSpreadCell::decodeFormula( const QString &_text, int _col, int _row )
{
    if ( _col == -1 )
        _col = m_iColumn;
    if ( _row == -1 )
        _row = m_iRow;

    QString erg = "";
    unsigned int pos = 0;
    const unsigned int length = _text.length();

    if ( _text.isEmpty() )
        return QString();

    while ( pos < length )
    {
        if ( _text[pos] == '"' )
        {
            erg += _text[pos++];
            while ( pos < length && _text[pos] != '"' )
            {
                erg += _text[pos++];
                // Allow escaped double quotes (\")
                if ( pos < length && _text[pos] == '\\' && _text[pos+1] == '"' )
                {
                    erg += _text[pos++];
                    erg += _text[pos++];
                }
            }
            if ( pos < length )
                erg += _text[pos++];
        }
        else if ( _text[pos] == '#' || _text[pos] == '$' )
        {
            bool fix1 = FALSE;
            bool fix2 = FALSE;
            if ( _text[pos++] == '$' )
                fix1 = TRUE;
            int col = 0;
            unsigned int oldPos = pos;
            while ( pos < length && ( _text[pos].isDigit() || _text[pos] == '-' ) ) ++pos;
            if ( pos != oldPos )
                col = _text.mid(oldPos, pos-oldPos).toInt();
            if ( !fix1 )
                col += _col;
            // Skip '#' or '$'
            if ( _text[pos++] == '$' )
                fix2 = TRUE;
            int row = 0;
            oldPos = pos;
            while ( pos < length && ( _text[pos].isDigit() || _text[pos] == '-' ) ) ++pos;
            if ( pos != oldPos )
                row = _text.mid(oldPos, pos-oldPos).toInt();
            if ( !fix2 )
                row += _row;
            // Skip '#' or '$'
            ++pos;
            if ( row < 1 || col < 1 || row > KS_rowMax || col > KS_colMax )
            {
                kdDebug(36001) << "KSpreadCell::decodeFormula: row or column out of range (col: " << col << " | row: " << row << ")" << endl;
                erg = "=\"#### " + i18n("REFERENCE TO COLUMN OR ROW IS OUT OF RANGE") + "\"";
                return erg;
            }
            if ( fix1 )
                erg += "$";
            erg += util_encodeColumnLabelText(col); //Get column text

            if ( fix2 )
                erg += "$";
            erg += QString::number( row );
        }
        else
            erg += _text[pos++];
    }

    return erg;
}


void KSpreadCell::freeAllObscuredCells()
{
    //
    // Free all obscured cells.
    //

  for ( int x = m_iColumn + m_iMergedXCells;
        x <= m_iColumn + m_iExtraXCells; ++x )
    for ( int y = m_iRow + m_iMergedYCells;
          y <= m_iRow + m_iExtraYCells; ++y )
      if ( x != m_iColumn || y != m_iRow )
      {
        KSpreadCell *cell = m_pTable->cellAt( x, y );
        cell->unobscure(this);
      }

  m_iExtraXCells = m_iMergedXCells;
  m_iExtraYCells = m_iMergedYCells;

}

// ##### Are _col and _row really needed ?
void KSpreadCell::makeLayout( QPainter &_painter, int _col, int _row )
{
    // Yes they are: they are useful if this is the default layout,
    // in which case m_iRow and m_iColumn are 0 and 0, but col and row
    // are the real coordinates of the cell.

    if (!testFlag(Flag_LayoutDirty))
      return;

    m_nbLines = 0;
    clearFlag(Flag_CellTooShort);

    freeAllObscuredCells();
    /* but reobscure the ones that are forced obscuring */
    forceExtraCells(m_iColumn, m_iRow, m_iMergedXCells, m_iMergedYCells);

    ColumnLayout *cl1 = m_pTable->columnLayout( column() );
    RowLayout *rl1 = m_pTable->rowLayout( row() );
    if( cl1->isHide() || (rl1->height()<=2))
        return;
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

        // Occupy the needed extra cells in horizontal direction
        max_width = width( _col );
        ende = ( max_width >= w );
        for( c = _col + 1; !ende && c <= _col + right; ++c )
        {
            KSpreadCell *cell = m_pTable->nonDefaultCell( c, _row );
            cell->obscure( this );
            ColumnLayout *cl = m_pTable->columnLayout( c );
            max_width += cl->width();
            if ( max_width >= w )
                ende = true;
        }
        m_iExtraXCells = c - _col - 1;

        /* we may have used extra cells, but only cells that we were already
           merged to.
        */
        if (m_iExtraXCells < m_iMergedXCells)
        {
          m_iExtraXCells = m_iMergedXCells;
        }
        else
        {
          m_iExtraWidth = max_width;
        }
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
                    cell->obscure( this );
                }
                RowLayout *rl = m_pTable->rowLayout( r );
                max_height += rl->height();
                if ( max_height >= h )
                    ende = true;
            }
        }
        m_iExtraYCells = r - _row - 1;
        /* we may have used extra cells, but only cells that we were already
           merged to.
        */
        if (m_iExtraYCells < m_iMergedYCells)
        {
          m_iExtraYCells = m_iMergedYCells;
        }
        else
        {
          m_iExtraHeight = max_height;
        }
        clearFlag(Flag_LayoutDirty);
        return;
    }

    setOutputText();

    // Empty text?
    if ( m_strOutText.isEmpty() )
    {
        m_strOutText = QString::null;
        if ( isDefault() )
            return;
    }

    //
    // Determine the correct font
    //
    KSpreadConditional condition;
    if( conditions.GetCurrentCondition(condition) &&
	!m_pTable->getShowFormula() )
    {
        _painter.setFont( condition.fontcond );
    }
    else
    {
        _painter.setFont( textFont(_col,_row ) );
    }
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
    /* TODO - use m_iExtraWidth/height here? Isn't it already calculated?*/
    if ( testFlag(Flag_ForceExtra) )
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

    if( verticalText( column(), row() ) ||getAngle(column(), row())!=0)
    {
       RowLayout *rl = m_pTable->rowLayout( row() );

       if(m_iOutTextHeight>=rl->height())
       {
         setFlag(Flag_CellTooShort);
       }
    }

    // Do we have to occupy additional cells right hand ?
    if ( m_iOutTextWidth+indent > w - 2 * BORDER_SPACE -
         leftBorderWidth( _col, _row) - rightBorderWidth( _col, _row ) )
    {
      int c = m_iColumn;
      int end = 0;
      // Find free cells right hand to this one
      while ( !end )
      {
        ColumnLayout *cl2 = m_pTable->columnLayout( c + 1 );
        KSpreadCell *cell = m_pTable->visibleCellAt( c + 1, m_iRow );
        if ( cell->isEmpty() )
        {
          w += cl2->width() - 1;
          c++;

          // Enough space ?
          if ( m_iOutTextWidth+indent <= w - 2 * BORDER_SPACE -
               leftBorderWidth( _col, _row) - rightBorderWidth( _col, _row ) )
            end = 1;
        }
        // Not enough space, but the next cell is not empty
        else
          end = -1;
      }

      /* Dont occupy additional space for right aligned or centered text or
         values.  Nor for numeric or boolean, apparently.  Also check to make
         sure we haven't already force-merged enough cells
      */
      /* ##### Why not right/center aligned text?  No one knows.  Perhaps it
         has something to do with calculating how much room the text needs in
         those cases?
      */
      if ( align(_col,_row) == KSpreadCell::Left ||
           align(_col,_row) == KSpreadCell::Undefined)
      {
        if (c - m_iColumn > m_iMergedXCells)
        {
          m_iExtraXCells = c - m_iColumn;
          m_iExtraWidth = w;
          for( int i = m_iColumn + 1; i <= c; ++i )
          {
            KSpreadCell *cell = m_pTable->nonDefaultCell( i, m_iRow );
            cell->obscure( this );
          }
          //Not enough space
          if(end==-1)
          {
            setFlag(Flag_CellTooShort);
          }
        }
        else
        {
          setFlag(Flag_CellTooShort);
        }
      }
      else
      {
        setFlag(Flag_CellTooShort);
      }
    }
    clearFlag(Flag_LayoutDirty);

    return;
}

void KSpreadCell::setOutputText()
{
  if (isDefault())
  {
    m_strOutText = QString::null;
    return;
  }
  if (hasError())
  {
    if (testFlag(Flag_ParseError))
    {
      m_strOutText = "####Parse";
    }
    else if (testFlag(Flag_CircularCalculation))
    {
      m_strOutText = "####Circle";
    }
    else if (testFlag(Flag_DependancyError))
    {
      m_strOutText = "####Depend";
    }
    else
    {
      m_strOutText = "####";
      kdDebug(36001) << "Unhandled error type." << endl;
    }
    return;
  }



// Apply text format
  // (this is the only format that dictates the datatype, it's usually the other way round)
  if ( formatType() == Text_format )
    m_dataType = StringData;

  /**
   * A usual numeric, boolean, date, time or string value.
   */

  //
  // Turn the stored value in a string
  //

  if ( isFormula() && m_pTable->getShowFormula() )
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
    m_strOutText = (valueBool()) ? i18n("True") : i18n("False");
  }
  else if( isDate() )
  {
    m_strOutText=util_dateFormat( locale(), valueDate(), formatType() );
  }
  else if( isTime() )
  {
    m_strOutText=util_timeFormat( locale(), valueTime(), formatType() );
  }
  else if ( isNumeric() )
  {
    // First get some locale information
    if (!decimal_point)
    { // (decimal_point is static)
      decimal_point = locale()->decimalSymbol()[0];
      kdDebug(36001) << "decimal_point is '" << decimal_point.unicode() << "'" << endl;

      if ( decimal_point.isNull() )
        decimal_point = '.';
    }

    // Scale the value as desired by the user.
    double v = valueDouble() * factor(column(),row());

    // Always unsigned ?
    if ( floatFormat( column(), row() ) == KSpreadCell::AlwaysUnsigned &&
         v < 0.0)
      v *= -1.0;

    // Make a string out of it.
    QString localizedNumber = createFormat( v, column(), row() );

    // Remove trailing zeros and the decimal point if necessary
    // unless the number has no decimal point
    if ( precision( column(), row())== -1 && localizedNumber.find(decimal_point) >= 0 )
    {
      int start=0;
      if(localizedNumber.find('%')!=-1)
        start=2;
      else if(localizedNumber.find( locale()->currencySymbol())==((int)(localizedNumber.length()-locale()->currencySymbol().length())))
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
    if( !prefix( column(), row() ).isEmpty())
      m_strOutText += prefix( column(), row() )+" ";

    m_strOutText += localizedNumber;

    if( !postfix( column(), row() ).isEmpty())
      m_strOutText += " "+postfix( column(), row() );


    // This method only calculates the text, and its width.
    // No need to bother about the color (David)
  }
  else if ( isFormula() )
  {
    m_strOutText = m_strFormulaOut;
  }
  else if( isString() )
  {
    if (!m_strText.isEmpty() && m_strText[0]=='\'' )
      m_strOutText = m_strText.right(m_strText.length()-1);
    else
      m_strOutText = m_strText;
  }
  else // When does this happen ?
  {
    kdDebug(36001) << "Please report: final case of makeLayout ... m_dataType=" << m_dataType << " m_strText=" << m_strText << endl;
    m_strOutText = m_strText;
  }
}

QString KSpreadCell::createFormat( double value, int _col, int _row )
{
    // if precision is -1, ask for a huge number of decimals, we'll remove
    // the zeros later. Is 8 ok ?
    int p = (precision(_col,_row) == -1) ? 8 : precision(_col,_row) ;
    QString localizedNumber= locale()->formatNumber( value, p );
    int pos = 0;

    switch( formatType() )
    {
    case Number:
        localizedNumber = locale()->formatNumber(value, p);
        if( floatFormat( _col, _row ) == KSpreadCell::AlwaysSigned && value >= 0 )
        {
            if(locale()->positiveSign().isEmpty())
                localizedNumber='+'+localizedNumber;
        }
        break;
    case Percentage:
        localizedNumber = locale()->formatNumber(value, p)+ " %";
        if( floatFormat( _col, _row ) == KSpreadCell::AlwaysSigned && value >= 0 )
        {
            if(locale()->positiveSign().isEmpty())
                localizedNumber='+'+localizedNumber;
        }
        break;
    case Money:
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
    case TextDate:
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
    case date_format17:
    case Text_format:
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
        localizedNumber=util_fractionFormat( value, formatType() );
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
    if(m_pTable->getShowFormula())
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
    int tmpAngle=getAngle( column(), row() );
    if( !verticalText( column(), row() ) && !tmpAngle )
    {
        m_iOutTextWidth = fm.width( m_strOutText );
	int offsetFont=0;
	if((alignY(column(),row())==KSpreadCell::Bottom)&& textFontUnderline(column(), row() ))
	   {
	     offsetFont=fm.underlinePos()+1;
	   }
        m_iOutTextHeight = fm.ascent() + fm.descent()+offsetFont ;
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
    KSpreadConditional condition;

    if( conditions.GetCurrentCondition(condition) &&
	!m_pTable->getShowFormula() )
    {
        _paint.setFont( condition.fontcond );
    }
    else
    {
        _paint.setFont( textFont(_col,_row ) );
    }

    textSize(_paint);

    offsetAlign(_col,_row);
}

bool KSpreadCell::makeFormula()
{
  clearFormula();

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
    clearFormula();

    setFlag(Flag_ParseError);
    m_strFormulaOut = "####";
    m_dataType = StringData; // correct?
    m_dValue = 0.0;
    setFlag(Flag_LayoutDirty);
    DO_UPDATE;
    if (m_pTable->doc()->getShowMessageError())
    {
      QString tmp(i18n("Error in cell %1\n\n"));
      tmp = tmp.arg( util_cellName( m_pTable, m_iColumn, m_iRow ) );
      tmp += context.exception()->toString( context );
      KMessageBox::error( (QWidget*)0L, tmp);
    }
    return false;
  }

  /* notify the new dependancy list that we are depending on them now */
  NotifyDependancyList(m_lstDepends, true);

  return true;
}

void KSpreadCell::clearFormula()
{
  /*notify dependancies that we're not depending on them any more */
  NotifyDependancyList(m_lstDepends, false);

  m_lstDepends.clear();
  if ( m_pCode )
  {
    delete m_pCode;
    m_pCode = NULL;
  }
}

bool KSpreadCell::calc(bool delay)
{
  if ( testFlag(Flag_Progress) )
  {
    kdError(36001) << "ERROR: Circle" << endl;
    setFlag(Flag_CircularCalculation);
    m_strFormulaOut = "####";
    m_dataType = StringData; // correct?
    setFlag(Flag_LayoutDirty);
    if ( m_style == ST_Select )
    {
        SelectPrivate *s = (SelectPrivate*)m_pPrivate;
        s->parse( m_strFormulaOut );
    }
//    DO_UPDATE;
    return false;
  }

  if ( !testFlag(Flag_CalcDirty) )
    return true;

  if (delay)
  {
    if ( m_pTable->doc()->delayCalculation() )
    {
      return true;
    }
  }

  setFlag(Flag_LayoutDirty);
  clearFlag(Flag_CalcDirty);

  if ( !isFormula() )
  {
    DO_UPDATE;
    return true;
  }

  setFlag(Flag_Progress);


  if (m_pCode == NULL)
  {
    makeFormula();
  }

  KSpreadDependency *dep;
  for ( dep = m_lstDepends.first(); dep != 0L; dep = m_lstDepends.next() )
  {
    for ( int x = dep->Left(); x <= dep->Right(); x++ )
    {
      for ( int y = dep->Top(); y <= dep->Bottom(); y++ )
      {
	KSpreadCell *cell = dep->Table()->cellAt( x, y );
	if ( !cell->calc( delay ) )
        {
	  m_strFormulaOut = "####";
	  setFlag(Flag_DependancyError);
	  m_dataType = StringData; //correct?
          clearFlag(Flag_Progress);
	  if ( m_style == ST_Select )
          {
	    SelectPrivate *s = (SelectPrivate*)m_pPrivate;
	    s->parse( m_strFormulaOut );
	  }
	  setFlag(Flag_LayoutDirty);
          clearFlag(Flag_CalcDirty);
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
// these should be set by the evaluate routine...
//      setFlag(Flag_Error);
//      m_strFormulaOut = "####";
//      m_dataType = StringData; //correct?
      setFlag(Flag_LayoutDirty);
      DO_UPDATE;
      // Print out exception if any
      if ( context.exception() && m_pTable->doc()->getShowMessageError())
      {
        QString tmp(i18n("Error in cell %1\n\n"));
        tmp = tmp.arg( util_cellName( m_pTable, m_iColumn, m_iRow ) );
        tmp += context.exception()->toString( context );
        KMessageBox::error( (QWidget*)0L, tmp);
      }

    }
    // setFlag(Flag_LayoutDirty);
    clearFlag(Flag_Progress);
    clearFlag(Flag_CalcDirty);

    if ( m_style == ST_Select )
    {
        SelectPrivate *s = (SelectPrivate*)m_pPrivate;
        s->parse( m_strFormulaOut );
        DO_UPDATE;
    }
    return false;
  }
  else if ( context.value()->type() == KSValue::DoubleType )
  {
    m_dValue = context.value()->doubleValue();
    clearAllErrors();
    m_dataType = NumericData;
    checkNumberFormat(); // auto-chooses number or scientific
    // Format the result appropriately
    m_strFormulaOut = createFormat( valueDouble(), m_iColumn, m_iRow );
  }
  else if ( context.value()->type() == KSValue::IntType )
  {
    m_dValue = (double)context.value()->intValue();
    clearAllErrors();
    m_dataType = NumericData;

    checkNumberFormat(); // auto-chooses number or scientific
    // Format the result appropriately
    m_strFormulaOut = createFormat( valueDouble(), m_iColumn, m_iRow );
  }
  else if ( context.value()->type() == KSValue::BoolType )
  {
    clearAllErrors();
    m_dataType = BoolData;
    m_dValue = context.value()->boolValue() ? 1.0 : 0.0;
    m_strFormulaOut = context.value()->boolValue() ? i18n("True") : i18n("False");
    setFormatType(Number);
  }
  else if ( context.value()->type() == KSValue::TimeType )
  {
    clearAllErrors();
    m_dataType = TimeData;
    m_Time = context.value()->timeValue();

    //change format
    FormatType tmpFormat = formatType();
    if( tmpFormat != SecondeTime &&  tmpFormat != Time_format1 &&  tmpFormat != Time_format2
        && tmpFormat != Time_format3)
    {
      m_strFormulaOut = locale()->formatTime(valueTime(), false);
      setFormatType(Time);
    }
    else
    {
      m_strFormulaOut = util_timeFormat(locale(), valueTime(), formatType());
    }
  }
  else if ( context.value()->type() == KSValue::DateType)
  {
    clearAllErrors();
    m_dataType = DateData;
    m_Date = context.value()->dateValue();
    FormatType tmpFormat = formatType();
    if( tmpFormat != TextDate
        && !(tmpFormat>=200 &&tmpFormat<=216))
    {
        setFormatType(ShortDate);
        m_strFormulaOut = locale()->formatDate(valueDate(), true);
    }
    else
    {
        m_strFormulaOut = util_dateFormat( locale(), valueDate(), tmpFormat);
    }
  }
  else if ( context.value()->type() == KSValue::Empty )
  {
    m_dValue = 0.0;
    clearAllErrors();
    m_dataType = StringData;
    // Format the result appropriately
    setFormatType(Number);
    m_strFormulaOut = createFormat( valueDouble(), m_iColumn, m_iRow );
  }
  else
  {
    if ( m_pQML )
      delete m_pQML;
    m_pQML = 0;
    clearAllErrors();
    m_dataType = StringData;
    m_strFormulaOut = context.value()->toString( context );
    if ( !m_strFormulaOut.isEmpty() && m_strFormulaOut[0] == '!' )
    {
      m_pQML = new QSimpleRichText( m_strFormulaOut.mid(1),  QApplication::font() );//, m_pTable->widget() );
    }
    else if( !m_strFormulaOut.isEmpty() && m_strFormulaOut[0]=='\'')
    {
        m_strFormulaOut=m_strFormulaOut.right(m_strFormulaOut.length()-1);
    }
    else
      m_strFormulaOut=m_strFormulaOut;
    setFormatType(Text_format);
  }
  if ( m_style == ST_Select )
  {
      SelectPrivate *s = (SelectPrivate*)m_pPrivate;
      s->parse( m_strFormulaOut );
  }
  clearFlag(Flag_CalcDirty);
  setFlag(Flag_LayoutDirty);
  clearFlag(Flag_Progress);

  DO_UPDATE;

  return true;
}

QString KSpreadCell::valueString() const
{
  if ( m_style == ST_Select )
    return ((SelectPrivate*)m_pPrivate)->text();

  if ( isFormula() )
    return m_strFormulaOut;

  return m_strText;
}

void KSpreadCell::paintCell( const QRect& rect, QPainter &painter,
                             QPoint corner, QPoint cellRef, bool drawCursor )
{
  static int paintingObscured = 0;
  /* this flag indicates that we are working on drawing the cells that a cell
     is obscuring.  The value is the number of levels down we are currently
     working -- i.e. a cell obscured by a cell which is obscured by a cell.
  */

  /* if we're working on drawing an obscured cell, that means this cell
     should have a cell that obscured it. */
  Q_ASSERT(!(paintingObscured > 0 && m_ObscuringCells.isEmpty()));

  /* the cellref passed in should be this cell -- except if this is the default
     cell */

  Q_ASSERT(!(((cellRef.x() != m_iColumn) || (cellRef.y() != m_iRow)) &&
           !isDefault()));




  ColumnLayout* colLayout = m_pTable->columnLayout(cellRef.x());
  RowLayout* rowLayout = m_pTable->rowLayout(cellRef.y());
  int height = (m_iExtraYCells ? m_iExtraHeight : rowLayout->height());
  int width =  (m_iExtraXCells ? m_iExtraWidth : colLayout->width());
  QRect selection = m_pTable->selection();

  bool selected = selection.contains(cellRef);
  // Dont draw any selection when printing.
  if ( painter.device()->isExtDev() || !drawCursor)
    selected = false;

  calc();

  // Need to make a new layout ?
  if ( testFlag(Flag_LayoutDirty) )
    makeLayout( painter, cellRef.x(), cellRef.y() );

  QRect r2( corner.x(), corner.y(), width, height );
  if ( !r2.intersects( rect ) )
    return;

  if (!isObscuringForced())
  {
    paintBackground(painter, corner, cellRef, selected);
  }

  paintDefaultBorders(painter, corner, cellRef);
  paintCellBorders(painter, corner, cellRef);

  /* paint all the cells that this one obscures */
  paintingObscured++;
  paintObscuredCells(rect, painter, corner, cellRef);
  paintingObscured--;

  /* now print content, if this cell isn't obscured */
  if (!isObscured())
    /* don't paint content if this cell is obscured */
  {
    if ( !painter.device()->isExtDev() || m_pTable->getPrintCommentIndicator() )
      paintCommentIndicator(painter, corner, cellRef);
    if ( !painter.device()->isExtDev() || m_pTable->getPrintFormulaIndicator() )
      paintFormulaIndicator(painter, corner, cellRef);
    paintMoreTextIndicator(painter, corner, cellRef);

  /**
   * QML ?
   */
    if ( m_pQML && (!painter.device()->isExtDev() ||
                    !getDontprintText(cellRef.x(), cellRef.y()) ))
    {
      painter.save();
      m_pQML->draw( &painter, corner.x(), corner.y(),
                    QRegion( QRect( corner.x(), corner.y(), colLayout->width(),
                                    rowLayout->height() ) ),
                    QApplication::palette().active(), 0 );
      painter.restore();
    }
    /**
     * Usual Text
     */
    else if ( !m_strOutText.isEmpty() &&
              (!painter.device()->isExtDev() ||
               !getDontprintText(cellRef.x(),cellRef.y())))
    {
      paintText(painter, corner, cellRef);
    }
  } /* if (!isObscured()) */


  paintPageBorders( painter,corner, cellRef);


  if (isObscured() && paintingObscured == 0)
  {
    /* print the cells obscuring this one */

    /* if paintingObscured is > 0, that means drawing this cell was triggered
       while already drawing the obscuring cell -- don't want to cause an
       infinite loop
    */
    // Determine the dimension of the cell.
    KSpreadCell* obscuringCell = NULL;
    for (obscuringCell = m_ObscuringCells.first(); obscuringCell != NULL;
         obscuringCell = m_ObscuringCells.next())
    {
      QPoint obscuringCellRef(obscuringCell->column(), obscuringCell->row());
      QPoint obscuringCellLoc( m_pTable->columnPos(obscuringCell->column()),
                               m_pTable->rowPos(obscuringCell->row()));
      painter.save();

      obscuringCell->paintCell( rect, painter, obscuringCellLoc,
                                obscuringCellRef);
      painter.restore();
    }
  }
}
/* the following code was commented out in the above function.  I'll leave
   it here in case this functionality is ever re-implemented and someone
   wants some code to start from */

  /**
     * Modification for drawing the button
     */
/*
  if ( m_style == KSpreadCell::ST_Button )
  {

  QBrush fill( Qt::lightGray );
  QApplication::style().drawControl( QStyle::CE_PushButton, &_painter, this,
  QRect( _tx + 1, _ty + 1, w2 - 1, h2 - 1 ),
  defaultColorGroup ); //, selected, &fill );

    }
*/
    /**
     * Modification for drawing the combo box
     */
/*
  else if ( m_style == KSpreadCell::ST_Select )
    {
      QApplication::style().drawComboButton(  &_painter, _tx + 1, _ty + 1,
                                                w2 - 1, h2 - 1,
						defaultColorGroup, selected );
    }
*/



void KSpreadCell::paintObscuredCells(const QRect& rect, QPainter& painter,
                                     QPoint corner, QPoint cellRef)
{
  // This cell is obscuring other ones? Then we redraw their
  // background and borders before we paint our content there.
  if ( extraXCells() || extraYCells() )
  {
    int ypos = corner.y();
    for( int y = 0; y <= extraYCells(); ++y )
    {
      int xpos = corner.x();
      RowLayout* rl = m_pTable->rowLayout( cellRef.y() + y );

      for( int x = 0; x <= extraXCells(); ++ x )
      {
        ColumnLayout* cl = m_pTable->columnLayout( cellRef.x() + x );
        if ( y != 0 || x != 0 )
        {
          KSpreadCell* cell = m_pTable->cellAt( cellRef.x() + x,
                                                cellRef.y() + y );

          cell->paintCell( rect, painter, QPoint(xpos, ypos),
                           QPoint(cellRef.x() + x, cellRef.y() + y));
        }
        xpos += cl->width();
      }

      ypos += rl->height();
    }
  }
}


void KSpreadCell::paintBackground(QPainter& painter, QPoint corner,
                                  QPoint cellRef, bool selected)
{
  QColorGroup defaultColorGroup = QApplication::palette().active();
  QPoint marker = m_pTable->marker();
  ColumnLayout* colLayout = m_pTable->columnLayout(cellRef.x());
  RowLayout* rowLayout = m_pTable->rowLayout(cellRef.y());
  int width = (m_iExtraXCells ? m_iExtraWidth : colLayout->width());
  int height =  (m_iExtraYCells ? m_iExtraHeight : rowLayout->height());
  KSpreadCell* cell = m_pTable->cellAt(marker);
  QPoint bottomRight(marker.x() + cell->extraXCells(),
                     marker.y() + cell->extraYCells());
  QRect markerArea(marker, bottomRight);
  // Determine the correct background color
  if ( selected && !markerArea.contains(cellRef))
  {
    painter.setBackgroundColor( defaultColorGroup.highlight() );
  }
  else
  {
    QColor bg = bgColor( cellRef.x(), cellRef.y() );

    if (! painter.device()->isExtDev() )
    {
      if ( bg.isValid() )
      {
        painter.setBackgroundColor( bg );

      }
      else
        painter.setBackgroundColor( defaultColorGroup.base() );
    }
    else
    {
      //bad hack but there is a qt bug
      //so I can print backgroundcolor
      QBrush bb( bg );
      if( !bg.isValid())
        bb.setColor(Qt::white);
      /* I think this just gets erased below....I'll figure it out later. */
      painter.fillRect( corner.x(), corner.y(), width, height, bb );
    }
  }

  // Erase the background of the cell.
  if ( !painter.device()->isExtDev() )
    painter.eraseRect( corner.x(), corner.y(), width, height );

  // Draw a background brush
  QBrush bb = backGroundBrush( cellRef.x(), cellRef.y() );

  if( bb.style() != Qt::NoBrush )
  {
    painter.fillRect( corner.x(), corner.y(), width, height, bb );
  }

}

void KSpreadCell::paintDefaultBorders(QPainter& painter, QPoint corner,
                                      QPoint cellRef)
{
  QPen left_pen = leftBorderPen( cellRef.x(), cellRef.y() );
  QPen top_pen = topBorderPen( cellRef.x(), cellRef.y() );
  QPen right_pen = rightBorderPen( cellRef.x(), cellRef.y() );
  QPen bottom_pen = bottomBorderPen( cellRef.x(), cellRef.y() );
  ColumnLayout* colLayout = m_pTable->columnLayout(cellRef.x());
  RowLayout* rowLayout = m_pTable->rowLayout(cellRef.y());
  int height = rowLayout->height();
  int width =  colLayout->width();
  /* Each cell is responsible for drawing it's top and left portions of the
     "default" grid. --Or not drawing it if it shouldn't be there.
     It's even responsible to paint the right and bottom, if it is the last
     cell on a print out*/
  bool paintTop;
  bool paintLeft;
  bool paintBottom;
  bool paintRight;
  KSpreadCell *cell = NULL;

  paintLeft = ( left_pen.style() == Qt::NoPen && 
                table()->getShowGrid() );
  paintRight = ( painter.device()->isExtDev() && // Only on printout
                 right_pen.style() == Qt::NoPen && 
                 table()->getShowGrid() &&
                 table()->isOnNewPageX( cellRef.x() + 1 ) );  //Only when last cell on page
  paintTop = ( top_pen.style() == Qt::NoPen && 
               table()->getShowGrid() );
  paintBottom = ( painter.device()->isExtDev() &&  // Only on printout
                  bottom_pen.style() == Qt::NoPen && 
                  table()->getShowGrid() &&
                  table()->isOnNewPageY( cellRef.y() + 1 ) ); //Only when last cell on page

  for (cell = m_ObscuringCells.first(); cell != NULL;
       cell = m_ObscuringCells.next())
  {
    paintLeft = paintLeft && (cell->column() == cellRef.x());
    paintTop = paintTop && (cell->row() == cellRef.y());

  }

  /* should we do the left border? */
  if (paintLeft)
  {
    int dt = 0;
    int db = 0;

    if ( cellRef.x() > 1 )
    {
      QPen t = m_pTable->cellAt( cellRef.x() - 1, cellRef.y() )->topBorderPen( cellRef.x() - 1, cellRef.y() );
      QPen b = m_pTable->cellAt( cellRef.x() - 1, cellRef.y() )->bottomBorderPen( cellRef.x() - 1, cellRef.y() );

      if ( t.style() != Qt::NoPen )
        dt = (t.width() + 1 )/2;
      if ( b.style() != Qt::NoPen )
        db = (t.width() / 2);
    }

    painter.setPen( table()->doc()->defaultGridPen() );
    painter.drawLine( corner.x(), corner.y() + dt, 
                      corner.x(), corner.y() + height - db - dt );
  }

  /* should we do the right border? */
  if ( paintRight )
  {
    int dt = 0;
    int db = 0;

    if ( cellRef.x() < KS_colMax )
    {
      QPen t = m_pTable->cellAt( cellRef.x() + 1, cellRef.y() )->topBorderPen( cellRef.x() + 1, cellRef.y() );
      QPen b = m_pTable->cellAt( cellRef.x() + 1, cellRef.y() )->bottomBorderPen( cellRef.x() + 1, cellRef.y() );

      if ( t.style() != Qt::NoPen )
        dt = (t.width() + 1 )/2;
      if ( b.style() != Qt::NoPen )
        db = (t.width() / 2);
    }

    painter.setPen( table()->doc()->defaultGridPen() );
    painter.drawLine( corner.x() + width, corner.y() + dt, 
                      corner.x() + width, corner.y() + height - db - dt );
  }

  /* should we do the top border? */
  if ( paintTop )
  {
    int dl = 0;
    int dr = 0;
    if ( cellRef.y() > 1 )
    {
      QPen l = m_pTable->cellAt( cellRef.x(), cellRef.y() - 1 )->leftBorderPen( cellRef.x(), cellRef.y() - 1 );
      QPen r = m_pTable->cellAt( cellRef.x(), cellRef.y() - 1 )->rightBorderPen( cellRef.x(), cellRef.y() - 1 );

      if ( l.style() != Qt::NoPen )
        dl = ( l.width() - 1 ) / 2 + 1;
      if ( r.style() != Qt::NoPen )
        dr = r.width() / 2;
    }
    painter.setPen( table()->doc()->defaultGridPen() );
    painter.drawLine( corner.x() + dl,              corner.y(), 
                      corner.x() + width - dr - dl, corner.y() );
  }

  /* should we do the bottom border? */
  if ( paintBottom )
  {
    int dl = 0;
    int dr = 0;
    if ( cellRef.y() < KS_rowMax )
    {
      QPen l = m_pTable->cellAt( cellRef.x(), cellRef.y() + 1 )->leftBorderPen( cellRef.x(), cellRef.y() + 1 );
      QPen r = m_pTable->cellAt( cellRef.x(), cellRef.y() + 1 )->rightBorderPen( cellRef.x(), cellRef.y() + 1 );

      if ( l.style() != Qt::NoPen )
        dl = ( l.width() - 1 ) / 2 + 1;
      if ( r.style() != Qt::NoPen )
        dr = r.width() / 2;
    }
    painter.setPen( table()->doc()->defaultGridPen() );
    painter.drawLine( corner.x() + dl,              corner.y() + height, 
                      corner.x() + width - dr - dl, corner.y() + height );
  }
}

void KSpreadCell::paintCommentIndicator(QPainter& painter, QPoint corner,
                                        QPoint cellRef)
{
  // Point the little corner if there is a comment attached
  // to this cell.
  ColumnLayout* colLayout = m_pTable->columnLayout(cellRef.x());
  RowLayout* rowLayout = m_pTable->rowLayout(cellRef.y());
  int width =  (m_iExtraYCells ? m_iExtraHeight : colLayout->width());

  if( !comment(cellRef.x(),cellRef.y()).isEmpty() && 
      rowLayout->height() > 2 &&
      colLayout->width() > 10 &&
      ( table()->getPrintCommentIndicator() ||  
        ( !painter.device()->isExtDev() && table()->doc()->getShowCommentIndicator() ) ) )
  {
    QPointArray point( 3 );
    point.setPoint( 0, corner.x() + width - 5, corner.y() );
    point.setPoint( 1, corner.x() + width, corner.y() );
    point.setPoint( 2, corner.x() + width, corner.y() + 5 );
    painter.setBrush( QBrush(Qt::red ) );
    painter.setPen( Qt::NoPen );
    painter.drawPolygon( point );
  }
}

// small blue rectangle if this cell holds a formula
void KSpreadCell::paintFormulaIndicator(QPainter& painter, QPoint corner,
                                        QPoint /* cellRef */)
{
  if( isFormula() && m_pTable->getShowFormulaIndicator() )
  {
    QPointArray point( 3 );
    point.setPoint( 0, corner.x(), corner.y() + height() - 5 );
    point.setPoint( 1, corner.x(), corner.y() + height() );
    point.setPoint( 2, corner.x() + 5, corner.y() + height() );
    painter.setBrush( QBrush(Qt::blue ) );
    painter.setPen( Qt::NoPen );
    painter.drawPolygon( point );
  }
}
void KSpreadCell::paintMoreTextIndicator(QPainter& painter, QPoint corner,
                                         QPoint cellRef)
{
  ColumnLayout* colLayout = m_pTable->columnLayout(cellRef.x());
  RowLayout* rowLayout = m_pTable->rowLayout(cellRef.y());
  int height = (m_iExtraYCells ? m_iExtraHeight : rowLayout->height());
  int width =  (m_iExtraXCells ? m_iExtraWidth : colLayout->width());
  //show  a red triangle when it's not possible to write all text in cell
  //don't print the red triangle if we're printing

  if(testFlag(Flag_CellTooShort) && !painter.device()->isExtDev() &&
     rowLayout->height() > 2  && colLayout->width() > 4)
  {
    QPointArray point( 3 );
    point.setPoint( 0, corner.x() + width - 4 , (corner.y() + height/2) - 4 );
    point.setPoint( 1, corner.x() + width,(corner.y() + height/2) );
    point.setPoint( 2, corner.x() + width - 4,(corner.y() + height/2) + 4 );
    painter.setBrush( QBrush(Qt::red  ) );
    painter.setPen( Qt::NoPen );
    painter.drawPolygon( point );
  }
}

void KSpreadCell::paintText(QPainter& painter, QPoint corner, QPoint cellRef)
{
  ColumnLayout* colLayout = m_pTable->columnLayout(cellRef.x());
  RowLayout* rowLayout = m_pTable->rowLayout(cellRef.y());

  int width =  (m_iExtraYCells ? m_iExtraHeight : colLayout->width());
  QColorGroup defaultColorGroup = QApplication::palette().active();

  QColor textColorPrint = textColor( cellRef.x(), cellRef.y() );
  QPen tmpPen( textColorPrint );

  // Resolve the text color if invalid (=default)
  if(!textColorPrint.isValid())
  {
    if(painter.device()->isExtDev())
      textColorPrint = Qt::black;
    else
      textColorPrint = QApplication::palette().active().text();
  }

  KSpreadConditional condition;

  if(conditions.GetCurrentCondition(condition) &&
     !m_pTable->getShowFormula())
  {
    painter.setFont( condition.fontcond );
    tmpPen.setColor( condition.colorcond );
  }
  else
  {
    painter.setFont( textFont( cellRef.x(), cellRef.y() ) );
    if( isNumeric() && !m_pTable->getShowFormula() )
    {
      double v = valueDouble() * factor(column(),row());
      if ( floatColor( cellRef.x(), cellRef.y()) == KSpreadCell::NegRed && v < 0.0 )
        tmpPen.setColor( Qt::red );
    }
  }
/****

 For now I am commenting this out -- with the default color display you
 can read normal text through a highlighted background.  Maybe this isn't
 always the case, though, and we can put the highlighted text color back in.
 In that case, we need to somewhere in here figure out if the text overlaps
 another cell outside of the selection, otherwise that portion of the text
 will be printed white on white.  So just that portion would need to be
 painted again in the normal color.

 This should probably be done eventually, anyway, because I like using the
 reverse text color for highlighted cells.  I just don't like extending the
 cell 'highlight' background outside of the selection rectangle because it
 looks REALLY ugly.

  if ( selected && ( cellRef.x() != marker.x() || cellRef.y() != marker.y() )  )
  {
    QPen p( tmpPen );
    p.setColor( defaultColorGroup.highlightedText() );
    painter.setPen( p );
  }
  else
  {
    painter.setPen(tmpPen);
  }
*/
 painter.setPen(tmpPen);

  QString tmpText = m_strOutText;
  int tmpHeight = m_iOutTextHeight;
  int tmpWidth = m_iOutTextWidth;
  if( testFlag(Flag_CellTooShort) )
  {
    m_strOutText=textDisplaying( painter );
  }

  //hide zero
  if(m_pTable->getHideZero() && isNumeric() &&
     valueDouble() * factor(column(),row()) == 0 )
  {
    m_strOutText=QString::null;
  }

  if( colLayout->isHide()|| (rowLayout->height()<=2))
  {
    //clear extra cell if column or row is hidden
    freeAllObscuredCells();  /* TODO: This looks dangerous...must check when I
                                have time */
    m_strOutText="";
  }

  conditionAlign( painter, cellRef.x(), cellRef.y() );

  int indent = 0;
  int offsetCellTooShort = 0;
  int a = defineAlignX();
  //apply indent if text is align to left not when text is at right or middle
  if(  a == KSpreadCell::Left && !isEmpty())
  {
    indent=getIndent(column(),row());
  }

  //made an offset, otherwise ### is under red triangle
  if( a == KSpreadCell::Right && !isEmpty() && testFlag(Flag_CellTooShort) )
  {
    offsetCellTooShort=4;
  }

  QFontMetrics fm2 = painter.fontMetrics();
  int offsetFont=0;

  if((alignY(column(),row()) == KSpreadCell::Bottom)&&
     textFontUnderline(column(), row() ))
  {
    offsetFont=fm2.underlinePos()+1;
  }
  int tmpAngle=getAngle( cellRef.x(), cellRef.y() );
  if ( !multiRow( cellRef.x(), cellRef.y() ) &&
       !verticalText( cellRef.x(), cellRef.y()) && !tmpAngle)
  {
    painter.drawText( indent + corner.x() + m_iTextX - offsetCellTooShort,
                      corner.y() + m_iTextY - offsetFont, m_strOutText );
  }
  else if( tmpAngle != 0)
  {
    int angle = tmpAngle;
    QFontMetrics fm = painter.fontMetrics();
    painter.rotate(angle);
    int x;
    if(angle > 0)
      x = indent + corner.x() + m_iTextX;
    else
      x = indent + static_cast<int>(corner.x() + m_iTextX -
                                    (fm.descent() + fm.ascent()) *
                                    sin(angle*M_PI/180));
    int y;
    if(angle > 0)
      y = corner.y() + m_iTextY;
    else
      y = corner.y() + m_iTextY + m_iOutTextHeight;
    painter.drawText( qRound(x*cos(angle*M_PI/180) + y*sin(angle*M_PI/180)),
                      qRound(-x*sin(angle*M_PI/180) + y*cos(angle*M_PI/180)),
                      m_strOutText );
    painter.rotate(-angle);
  }
  else if( multiRow( cellRef.x(), cellRef.y()) && !verticalText(cellRef.x(), cellRef.y()))
  {
    QString t;
    int i;
    int pos = 0;
    int dy = 0;
    int dx = 0;
    QFontMetrics fm = painter.fontMetrics();
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
      if(m_pTable->getShowFormula())
        a = KSpreadCell::Left;

      // #### Torben: This looks duplicated for me
      switch( a )
      {
      case KSpreadCell::Left:
        m_iTextX = leftBorderWidth( cellRef.x(), cellRef.y() ) + BORDER_SPACE;
        break;
      case KSpreadCell::Right:
        m_iTextX = width - BORDER_SPACE - fm.width( t )
                   - rightBorderWidth( cellRef.x(), cellRef.y() );
        break;
      case KSpreadCell::Center:
        m_iTextX = ( width - fm.width( t ) ) / 2;
      }
      painter.drawText( indent + corner.x() + m_iTextX + dx,
                        corner.y() + m_iTextY + dy, t );
                dy += fm.descent() + fm.ascent();
    }
    while ( i != -1 );
  }
  else if(verticalText( cellRef.x(), cellRef.y()) && !m_strOutText.isEmpty())
  {
    QString t;
    int i=0;
    int dy = 0;
    int dx = 0;
    int j=0;
    QFontMetrics fm = painter.fontMetrics();
    do
    {
      i = m_strOutText.length();
      t = m_strOutText.at(j);
      painter.drawText( indent + corner.x() + m_iTextX + dx,
                        corner.y() + m_iTextY + dy, t );
      dy += fm.descent() + fm.ascent();
      j++;
    }
    while ( j != i );
  }

  if(testFlag(Flag_CellTooShort))
  {
    m_strOutText = tmpText;
    m_iOutTextHeight = tmpHeight;
    m_iOutTextWidth = tmpWidth;
  }

  if(m_pTable->getHideZero() && isNumeric() &&
     valueDouble() * factor(column(),row())==0)
  {
    m_strOutText=tmpText;
  }

  if( colLayout->isHide()|| (rowLayout->height()<=2))
    m_strOutText=tmpText;

}

void KSpreadCell::paintPageBorders(QPainter& painter, QPoint corner,
                                   QPoint cellRef)
{
  if ( painter.device()->isExtDev() )
    return;

  ColumnLayout* colLayout = m_pTable->columnLayout(cellRef.x());
  RowLayout* rowLayout = m_pTable->rowLayout(cellRef.y());
  int height = (m_iExtraYCells ? m_iExtraHeight : rowLayout->height());
  int width =  (m_iExtraXCells ? m_iExtraWidth : colLayout->width());

  // Draw page borders
  if ( m_pTable->isShowPageBorders() && 
       //Check for the print range
       cellRef.x() >= table()->printRange().left() &&
       cellRef.x() <= table()->printRange().right()+1 &&
       cellRef.y() >= table()->printRange().top() &&
       cellRef.y() <= table()->printRange().bottom()+1 )
  {
    if ( m_pTable->isOnNewPageY( cellRef.y() ) && ( cellRef.x() <= table()->printRange().right() ) )
    {
      painter.setPen( table()->doc()->pageBorderColor() );
      painter.drawLine( corner.x(), corner.y(), corner.x() + width,
                        corner.y() );
    }
    if ( m_pTable->isOnNewPageX( cellRef.x() ) && ( cellRef.y() <= table()->printRange().bottom() ) )
    {
      painter.setPen( table()->doc()->pageBorderColor() );
      painter.drawLine( corner.x(), corner.y(), corner.x(),
                        corner.y() + height );
    }
  }
}


void KSpreadCell::paintCellBorders(QPainter& painter, QPoint corner,
                                   QPoint cellRef)
{
  ColumnLayout* colLayout = m_pTable->columnLayout(cellRef.x());
  RowLayout* rowLayout = m_pTable->rowLayout(cellRef.y());
  int height = rowLayout->height();
  int width =  colLayout->width();

  /* we might not paint some borders if this cell is merged with another in
     that direction */
  bool paintLeft = true;
  bool paintRight = true;
  bool paintTop = true;
  bool paintBottom = true;

  KSpreadCell* cell = NULL;
  for (cell = m_ObscuringCells.first(); cell != NULL;
       cell = m_ObscuringCells.next())
  {
    int xDiff = cellRef.x() - cell->column();
    int yDiff = cellRef.y() - cell->row();
    paintLeft = paintLeft && xDiff == 0;
    paintTop = paintTop && yDiff == 0;

    paintRight = paintRight && cell->extraXCells() == xDiff;
    paintBottom = paintBottom && cell->extraYCells() == yDiff;
  }

  paintRight = paintRight && (extraXCells() == 0);
  paintBottom = paintBottom && (extraYCells() == 0);


  int top_offset = 0;
  int bottom_offset = 0;
  int left_offset = 0;
  int right_offset = 0;
  //
  // Determine the pens that should be used for drawing
  // the borders.
  //
  QPen left_pen = leftBorderPen( cellRef.x(), cellRef.y() );
  QPen right_pen = rightBorderPen( cellRef.x(), cellRef.y() );
  QPen top_pen = topBorderPen( cellRef.x(), cellRef.y() );
  QPen bottom_pen = bottomBorderPen( cellRef.x(), cellRef.y() );

  if ( left_pen.style() != Qt::NoPen && paintLeft)
  {
    int top = ( QMAX( 0, -1 + (int)top_pen.width() ) ) / 2 +
              ( ( QMAX( 0, -1 + (int)top_pen.width() ) ) % 2 );
    int bottom = ( QMAX( 0, -1 + (int)bottom_pen.width() ) ) / 2 + 1;

    painter.setPen( left_pen );
    painter.drawLine( corner.x(), corner.y() - top, corner.x(),
                      corner.y() + height + bottom );

    left_offset = left_pen.width() - ( left_pen.width() / 2 );
  }
  if ( right_pen.style() != Qt::NoPen && paintRight)
  {
    int top = ( QMAX( 0, -1 + (int)top_pen.width() ) ) / 2 +
              ( ( QMAX( 0, -1 + (int)top_pen.width() ) ) % 2 );
    int bottom = ( QMAX( 0, -1 + (int)bottom_pen.width() ) ) / 2 + 1;

    painter.setPen( right_pen );
    painter.drawLine( width + corner.x(), corner.y() - top,
                       width + corner.x(), corner.y() + height + bottom );
    right_offset = right_pen.width() / 2;
  }
  if ( top_pen.style() != Qt::NoPen && paintTop)
  {
    painter.setPen( top_pen );
    painter.drawLine( corner.x(), corner.y(), corner.x() + width, corner.y() );

    top_offset = top_pen.width() - ( top_pen.width() / 2 );
  }
  if ( bottom_pen.style() != Qt::NoPen && paintBottom )
  {
    painter.setPen( bottom_pen );
    painter.drawLine( corner.x(), height + corner.y(), corner.x() + width,
                      height + corner.y() );

    bottom_offset = bottom_pen.width() / 2;
  }



  //
  // Draw diagonal borders.
  //
  if (!isObscuringForced())
  {
    if ( fallDiagonalPen( cellRef.x(), cellRef.y() ).style() != Qt::NoPen )
    {
      painter.setPen( fallDiagonalPen(cellRef.x(), cellRef.y()) );
      painter.drawLine( corner.x(), corner.y(), corner.x() + width,
                        corner.y() + height );
    }
    if (goUpDiagonalPen( cellRef.x(), cellRef.y() ).style() != Qt::NoPen )
    {
      painter.setPen( goUpDiagonalPen(cellRef.x(), cellRef.y()) );
      painter.drawLine( corner.x(), corner.y() + height , corner.x() + width,
                        corner.y() );
    }
  }

  //
  // Look at the cells on our corners. It may happen that we
  // just erased parts of their borders corner, so we might need
  // to repaint these corners.
  //
  KSpreadCell* cell_t = m_pTable->cellAt( cellRef.x(), cellRef.y() - 1 );
  KSpreadCell* cell_l = m_pTable->cellAt( cellRef.x() - 1, cellRef.y() );
  KSpreadCell* cell_r = 0L;
  KSpreadCell* cell_b = 0L;
  if ( cellRef.x() < KS_colMax )
    cell_r = m_pTable->cellAt( cellRef.x() + 1, cellRef.y() );
  if ( cellRef.y() < KS_rowMax )
    cell_b = m_pTable->cellAt( cellRef.x(), cellRef.y() + 1 );

  QPen vert_pen, horz_pen;
  // Fix the borders which meet at the top left corner
  vert_pen = cell_t->leftBorderPen( cellRef.x(), cellRef.y() - 1 );
  if ( vert_pen.style() != Qt::NoPen )
  {
    horz_pen = cell_l->topBorderPen( cellRef.x() - 1, cellRef.y() );
    int bottom = ( QMAX( 0, -1 + (int)horz_pen.width() ) ) / 2 + 1;
    painter.setPen( vert_pen );
    painter.drawLine( corner.x(), corner.y(), corner.x(),
                      corner.y() + bottom );
  }

  // Fix the borders which meet at the top right corner
  vert_pen = cell_t->rightBorderPen( cellRef.x(), cellRef.y() - 1 );
  if ( ( vert_pen.style() != Qt::NoPen ) && ( cellRef.x() < KS_colMax ) )
  {
    horz_pen = cell_r->topBorderPen( cellRef.x() + 1, cellRef.y() );
    int bottom = ( QMAX( 0, -1 + (int)horz_pen.width() ) ) / 2 + 1;
    painter.setPen( vert_pen );
    painter.drawLine( corner.x() + width, corner.y(),
                      corner.x() + width, corner.y() + bottom );
  }

  // Bottom
  if ( cellRef.y() < KS_rowMax )
  {
    // Fix the borders which meet at the bottom left corner
    vert_pen = cell_b->leftBorderPen( cellRef.x(), cellRef.y() + 1 );
    if ( vert_pen.style() != Qt::NoPen )
    {
      horz_pen = cell_l->bottomBorderPen( cellRef.x() - 1, cellRef.y() );
      int bottom = ( QMAX( 0, -1 + (int)horz_pen.width() ) ) / 2;
      painter.setPen( vert_pen );
      painter.drawLine( corner.x(), corner.y() + height - bottom, corner.x(),
                        corner.y() + height );
    }

    // Fix the borders which meet at the bottom right corner
    vert_pen = cell_b->rightBorderPen( cellRef.x(), cellRef.y() + 1 );
    if ( ( vert_pen.style() != Qt::NoPen ) && ( cellRef.x() < KS_colMax ) )
    {
      horz_pen = cell_r->bottomBorderPen( cellRef.x() + 1, cellRef.y() );
      int bottom = ( QMAX( 0, -1 + (int)horz_pen.width() ) ) / 2;
      painter.setPen( vert_pen );
      painter.drawLine( corner.x() + width, corner.y() + height - bottom,
                        corner.x() + width, corner.y() + height );
    }
  }
}

int KSpreadCell::defineAlignX()
{
    int a = align(column(),row());
    if ( a == KSpreadCell::Undefined )
    {
        if ( isBool() || isNumeric() || isDate() || isTime() )
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
  if (( a == KSpreadCell::Left || a == KSpreadCell::Undefined) && !isNumeric()
    && !verticalText( column(),row() ))
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
	    if( getAngle(column(), row())!=0)
	      {
		QString tmp2;
		RowLayout *rl = m_pTable->rowLayout( row() );
		if(m_iOutTextHeight>rl->height())
		  {
		    for (int j=m_strOutText.length();j!=0;j--)
		      {
			tmp2=m_strOutText.left(j);
			if(fm.width(tmp2)<(rl->height()-1))
			  {
			    return m_strOutText.left(QMIN(tmp.length(),tmp2.length()));
			  }
		      }
		  }
		else
		  return tmp;

	      }
	    else
	      return tmp;
	  }
      }
    return QString("");
  }
 else if(verticalText( column(),row() ))
  {
     RowLayout *rl = m_pTable->rowLayout( row() );
     int tmpIndent=0;
     //not enough space but align to left
     int len=0;
     for (int i=column();i<=column()+m_iExtraXCells;i++)
       {
	 ColumnLayout *cl2 = m_pTable->columnLayout( i );
	 len+=cl2->width() - 1;
       }
     if(!isEmpty())
       tmpIndent=getIndent(column(),row());
     if( ((m_iOutTextWidth+tmpIndent)>len)||m_iOutTextWidth==0)
       return QString("");

     for (int i=m_strOutText.length();i!=0;i--)
       {
	if(((fm.ascent() + fm.descent())*i)<(rl->height()-1))
	  {
	    return m_strOutText.left(i);
	  }
      }
    return QString("");
   }

 ColumnLayout *cl = m_pTable->columnLayout( column() );
 int w = (  m_iExtraWidth == 0 ) ? cl->width() : m_iExtraWidth;

 if( isNumeric())
 {
   if( formatType()!=Scientific)
   {
     int p = (precision(column(),row())  == -1) ? 8 :
       precision(column(),row());
     double value =valueDouble() * factor(column(),row());
     int pos=0;
     QString localizedNumber= QString::number( (value), 'E', p);
     if((pos=localizedNumber.find('.'))!=-1)
     {
       localizedNumber=localizedNumber.replace(pos,1,decimal_point);
     }
     if( floatFormat( column(), row() ) ==
	 KSpreadCell::AlwaysSigned && value >= 0 )

     {
       if(locale()->positiveSign().isEmpty())
       {
	 localizedNumber='+'+localizedNumber;
       }
     }
     if ( precision(column(),row()) == -1 &&
	  localizedNumber.find(decimal_point) >= 0 )
     {
       //duplicate code it's not good I know I will fix it
       int start=0;
       if((start=localizedNumber.find('E'))!=-1)
       {
	 start=localizedNumber.length()-start;
       }
       int i = localizedNumber.length()-start;
       bool bFinished = FALSE;

       while ( !bFinished && i > 0 )
       {
	 QChar ch = localizedNumber[ i - 1 ];
	 if ( ch == '0' )
	 {
	   localizedNumber.remove(--i,1);
	 }
	 else
	 {
	   bFinished = TRUE;
	   if ( ch == decimal_point )
	   {
	     localizedNumber.remove(--i,1);
	   }
	 }
       }
     }
     if(fm.width(localizedNumber)<w)
     {
       return localizedNumber;
     }
   }
   /* What is this doing and is it broken with the new error handling? */
   QString str("####");
   int i;
   for(i=4;i!=0;i--)
   {
     if(fm.width(str.right(i))<(w-4-1))
     {
       break;
     }
   }
   return str.right(i);//QString("###");
 }
 else
 {
   QString tmp;
   for (int i=m_strOutText.length();i!=0;i--)
   {
     tmp=m_strOutText.left(i);
     if(fm.width(tmp)<(w-4-1)) //4 equals lenght of red triangle +1 pixel
     {
       return tmp;
     }
   }
 }
 return  QString::null;
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
      if(m_conditionIsTrue && !m_pTable->getShowFormula())
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
    if ( testFlag(Flag_ForceExtra) )
      return (int)( m_iExtraWidth );

    ColumnLayout *cl = m_pTable->columnLayout( _col );
    return cl->width( _canvas );
  }

  if ( testFlag(Flag_ForceExtra) )
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
    if ( testFlag(Flag_ForceExtra) )
      return (int)( m_iExtraHeight );

    RowLayout *rl = m_pTable->rowLayout( _row );
    return rl->height( _canvas );
  }

  if ( testFlag(Flag_ForceExtra) )
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

const QBrush& KSpreadCell::backGroundBrush( int _col, int _row ) const
{
  KSpreadCell* cell = m_ObscuringCells.getFirst();
  if ( cell != NULL )
  {
    return cell->backGroundBrush( cell->column(), cell->row() );
  }

  return KSpreadLayout::backGroundBrush( _col, _row );
}

const QColor& KSpreadCell::bgColor( int _col, int _row ) const
{
  KSpreadCell* cell = m_ObscuringCells.getFirst();
  if ( cell != NULL )
  {
    return cell->bgColor( cell->column(), cell->row() );
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
    KSpreadCell* cell = m_pTable->cellAt( column() - 1, row() ); //what happens on column=1
    if ( cell && cell->hasProperty( PRightBorder ) )
        cell->clearProperty( PRightBorder );

    KSpreadLayout::setLeftBorderPen( p );
}

void KSpreadCell::setTopBorderPen( const QPen& p )
{
    KSpreadCell* cell = m_pTable->cellAt( column(), row() - 1 ); //what happens on row=1
    if ( cell && cell->hasProperty( PBottomBorder ) )
        cell->clearProperty( PBottomBorder );

    KSpreadLayout::setTopBorderPen( p );
}

void KSpreadCell::setRightBorderPen( const QPen& p )
{
    KSpreadCell* cell = 0L;
    if ( column() < KS_colMax )
        cell = m_pTable->cellAt( column() + 1, row() );

    if ( cell && cell->hasProperty( PLeftBorder ) )
        cell->clearProperty( PLeftBorder );

    KSpreadLayout::setRightBorderPen( p );
}

void KSpreadCell::setBottomBorderPen( const QPen& p )
{
    KSpreadCell* cell = 0L;
    if ( row() < KS_rowMax )
        cell = m_pTable->cellAt( column(), row() + 1 );

    if ( cell && cell->hasProperty( PTopBorder ) )
        cell->clearProperty( PTopBorder );

    KSpreadLayout::setBottomBorderPen( p );
}

const QPen& KSpreadCell::rightBorderPen( int _col, int _row ) const
{

    if ( !hasProperty( PRightBorder ) && ( _col < KS_colMax ) )
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
    if ( !hasProperty( PBottomBorder ) && ( _row < KS_rowMax ) )
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
  if ( !isNumeric() )
    return;
  int tmpPreci=precision(column(),row());
  kdDebug(36001) << "incPrecision: tmpPreci = " << tmpPreci << endl;
  if ( tmpPreci == -1 )
  {
    int pos = m_strOutText.find(decimal_point);
    if ( pos == -1 )
        pos = m_strOutText.find('.');
    if ( pos == -1 )
      setPrecision(1);
    else
    {
      int start=0;
      if(m_strOutText.find('%')!=-1)
        start=2;
      else if(m_strOutText.find(locale()->currencySymbol())==((int)(m_strOutText.length()-locale()->currencySymbol().length())))
        start=locale()->currencySymbol().length()+1;
      else if((start=m_strOutText.find('E'))!=-1)
        start=m_strOutText.length()-start;

      //kdDebug(36001) << "start=" << start << " pos=" << pos << " length=" << m_strOutText.length() << endl;
      setPrecision( QMAX( 0, (int)m_strOutText.length() - start - pos ) );
    }
  }
  else if ( tmpPreci < 10 )
  {
    setPrecision(++tmpPreci);
  }
  setFlag(Flag_LayoutDirty);
}

void KSpreadCell::decPrecision()
{
  if ( !isNumeric() )
    return;
  int preciTmp=precision(column(),row());
  if ( precision(column(),row()) == -1 )
  {
    int pos = m_strOutText.find(decimal_point);
    int start=0;
    if(m_strOutText.find('%')!=-1)
        start=2;
    else if(m_strOutText.find(locale()->currencySymbol())==((int)(m_strOutText.length()-locale()->currencySymbol().length())))
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
    setFlag(Flag_LayoutDirty);
  }
  else if ( preciTmp > 0 )
  {
    setPrecision(--preciTmp);
    setFlag(Flag_LayoutDirty);
  }
}



void KSpreadCell::setCellText( const QString& _text, bool updateDepends )
{
    QString oldText=m_strText;
    setDisplayText( _text, updateDepends );
    if(!m_pTable->isLoading() && !testValidity() )
    {
      //reapply old value if action == stop
      setDisplayText( oldText, updateDepends );
    }
}



void KSpreadCell::setDisplayText( const QString& _text, bool updateDepends )
{
  clearAllErrors();
  m_strText = _text;

  // Free all content data
  if ( m_pQML )
  {
    delete m_pQML;
    m_pQML = NULL;
  }

  clearFormula();

  /**
   * A real formula "=A1+A2*3" was entered.
   */
  if ( !m_strText.isEmpty() && m_strText[0] == '=' )
  {
    setFlag(Flag_LayoutDirty);

    m_content = Formula;

    if ( !m_pTable->isLoading() )
    {
      if ( !makeFormula() )
      {
	kdError(36001) << "ERROR: Syntax ERROR" << endl;
      }
    }
  }
  /**
   * QML
   */
  else if ( !m_strText.isEmpty() && m_strText[0] == '!' )
  {
    m_pQML = new QSimpleRichText( m_strText.mid(1),  QApplication::font() );//, m_pTable->widget() );
    setFlag(Flag_LayoutDirty);
    m_content = RichText;
    m_dataType = OtherData;
  }
  /**
   * Some numeric value or a string.
   */
  else
  {
    m_content = Text;

    // Find out what data type it is
    checkTextInput();

    setFlag(Flag_LayoutDirty);
  }

  /**
   *  Special handling for selection boxes
   */
  if ( m_style == ST_Select && !m_pTable->isLoading() )
  {
      if ( testFlag(Flag_CalcDirty) )
          calc();

      SelectPrivate *s = (SelectPrivate*)m_pPrivate;
      if ( m_content == Formula )
          s->parse( m_strFormulaOut );
      else
          s->parse( m_strText );
      kdDebug(36001) << "SELECT " << s->text() << endl;
      checkTextInput(); // is this necessary?
      // setFlag(Flag_LayoutDirty);
  }
  setCalcDirtyFlag();

  /* those obscuring us need to redo their layout cause they can't obscure us
     now that we've got text.
     This includes cells obscuring cells that we are obscuring
  */
  for (int x = m_iColumn; x <= m_iColumn + extraXCells(); x++)
  {
    for (int y = m_iRow; y <= m_iRow + extraYCells(); y++)
    {
      KSpreadCell* cell = m_pTable->cellAt(x,y);
      QPtrList<KSpreadCell> lst = cell->obscuringCells();

      for (cell = lst.first(); cell != NULL; cell = lst.next())
      {
        cell->setFlag(Flag_LayoutDirty);
      }
    }
  }

  if ( updateDepends )
      update();

}

bool KSpreadCell::testValidity()
{
    bool valid = false;
    if( m_Validity != NULL )
    {
      if( isNumeric() &&
	  (m_Validity->m_allow == Allow_Number ||
	   (m_Validity->m_allow == Allow_Integer &&
	    valueDouble() == ceil(valueDouble()))))
      {
	switch( m_Validity->m_cond)
	{
	  case Equal:
	    valid = ( valueDouble() - m_Validity->valMin < DBL_EPSILON
		      && valueDouble() - m_Validity->valMin >
		      (0.0 - DBL_EPSILON));
	    break;
          case Superior:
	    valid = (valueDouble() > m_Validity->valMin);
	    break;
          case Inferior:
	    valid = (valueDouble()  <m_Validity->valMin);
	    break;
          case SuperiorEqual:
	    valid = (valueDouble() >= m_Validity->valMin);
            break;
          case InferiorEqual:
	    valid = (valueDouble() <= m_Validity->valMin);
	    break;
	  case Between:
	    valid = ( valueDouble() >= m_Validity->valMin &&
		      valueDouble() <= m_Validity->valMax);
	    break;
	  case Different:
	    valid = (valueDouble() < m_Validity->valMin ||
		     valueDouble() > m_Validity->valMax);
	    break;
	  default :
	    break;
        }
      }
      else if(m_Validity->m_allow==Allow_Text)
      {
	valid = ( m_dataType == StringData );
      }
      else if(m_Validity->m_allow==Allow_TextLength)
      {
	if( m_dataType == StringData )
        {
	  int len = m_strOutText.length();
	  switch( m_Validity->m_cond)
	  {
	    case Equal:
	      if (len == m_Validity->valMin)
		valid = true;
	      break;
	    case Superior:
	      if(len > m_Validity->valMin)
		valid = true;
	      break;
	    case Inferior:
	      if(len < m_Validity->valMin)
		valid = true;
	      break;
	    case SuperiorEqual:
	      if(len >= m_Validity->valMin)
		valid = true;
	      break;
	    case InferiorEqual:
	      if(len <= m_Validity->valMin)
		valid = true;
	      break;
	    case Between:
	      if(len >= m_Validity->valMin && len <= m_Validity->valMax)
		valid = true;
	      break;
	    case Different:
	      if(len <m_Validity->valMin || len >m_Validity->valMax)
		valid = true;
	      break;
	    default :
	      break;
	  }
	}
      }

      else if(m_Validity->m_allow == Allow_Time && isTime())
      {
	switch( m_Validity->m_cond)
	{
	  case Equal:
	    valid = (valueTime() == m_Validity->timeMin);
	    break;
	  case Superior:
	    valid = (valueTime() > m_Validity->timeMin);
	    break;
	  case Inferior:
	    valid = (valueTime() < m_Validity->timeMin);
	    break;
	  case SuperiorEqual:
	    valid = (valueTime() >= m_Validity->timeMin);
	    break;
	  case InferiorEqual:
	    valid = (valueTime() <= m_Validity->timeMin);
	    break;
	  case Between:
	    valid = (valueTime() >= m_Validity->timeMin &&
		     valueTime() <= m_Validity->timeMax);
	    break;
  	  case Different:
	    valid = (valueTime() < m_Validity->timeMin ||
		     valueTime() > m_Validity->timeMax);
	    break;
	  default :
	    break;

	}
      }
      else if(m_Validity->m_allow == Allow_Date && isDate())
      {
	switch( m_Validity->m_cond)
	{
	  case Equal:
	    valid = (valueDate() == m_Validity->dateMin);
	    break;
	  case Superior:
	    valid = (valueDate() > m_Validity->dateMin);
	    break;
	  case Inferior:
	    valid = (valueDate() < m_Validity->dateMin);
	    break;
	  case SuperiorEqual:
	    valid = (valueDate() >= m_Validity->dateMin);
	    break;
	  case InferiorEqual:
	    valid = (valueDate() <= m_Validity->dateMin);
	    break;
	  case Between:
	    valid = (valueDate() >= m_Validity->dateMin &&
		     valueDate() <= m_Validity->dateMax);
	    break;
	  case Different:
	    valid = (valueDate() < m_Validity->dateMin ||
		     valueDate() > m_Validity->dateMax);
	    break;
	  default :
	    break;

	}
      }
    }
    else
    {
      valid= true;
    }

    if(!valid &&m_Validity != NULL )
    {
      switch (m_Validity->m_action)
      {
        case Stop:
	  KMessageBox::error((QWidget*)0L, m_Validity->message,
			     m_Validity->title);
	  break;
        case Warning:
	  KMessageBox::warningYesNo((QWidget*)0L, m_Validity->message,
				    m_Validity->title);
	  break;
        case Information:
	  KMessageBox::information((QWidget*)0L, m_Validity->message,
				   m_Validity->title);
	  break;
      }
    }
    return (valid || m_Validity == NULL || m_Validity->m_action != Stop);
}

void KSpreadCell::setValue( double _d )
{
    clearAllErrors();
    m_strText = QString::number( _d );

    // Free all content data
    delete m_pQML;
    m_pQML = 0;

    clearFormula();

    clearAllErrors();
    m_dataType = NumericData;
    m_dValue = _d;
    setFlag(Flag_LayoutDirty);
    m_content = Text;

    // Do not update formulas and stuff here
    if ( !m_pTable->isLoading() )
        update();
}

void KSpreadCell::update()
{
  kdDebug(36001) << util_cellName( m_iColumn, m_iRow ) << " update" << endl;
  KSpreadCell* cell = NULL;
  for ( cell = m_ObscuringCells.first(); cell != NULL;
        cell = m_ObscuringCells.next())
  {
    cell->setLayoutDirtyFlag();
    cell->setDisplayDirtyFlag();
    m_pTable->updateCell( cell, cell->column(), cell->row() );
  }

  setFlag(Flag_DisplayDirty);

  updateDepending();

  if ( testFlag(Flag_DisplayDirty) )
    m_pTable->updateCell( this, m_iColumn, m_iRow );
}

void KSpreadCell::updateDepending()
{
  if ( testFlag(Flag_UpdatingDeps) || (!m_pTable->getAutoCalc()) )
  {
    return;
  }

  calc();

  kdDebug(36001) << util_cellName( m_iColumn, m_iRow ) << " updateDepending" << endl;

  KSpreadDependency* d = NULL;

  setFlag(Flag_UpdatingDeps);

  // Every cell that references us must calculate with this new value
  for (d = m_lstDependingOnMe.first(); d != NULL; d = m_lstDependingOnMe.next())
  {
    for (int c = d->Left(); c <= d->Right(); c++)
    {
      for (int r = d->Top(); r <= d->Bottom(); r++)
      {
	d->Table()->cellAt( c, r )->calc();
      }
    }
  }

  calc();

  kdDebug(36001) << util_cellName( m_iColumn, m_iRow ) << " updateDepending done" << endl;

  clearFlag(Flag_UpdatingDeps);
  updateChart();
}

void KSpreadCell::setCalcDirtyFlag()
{
  KSpreadDependency* d = NULL;

  if ( testFlag(Flag_CalcDirty) )
  {
    /* we need to avoid recursion */
    return;
  }

  setFlag(Flag_CalcDirty);

  /* if this cell is dirty, every cell that references this one is dirty */
  for (d = m_lstDependingOnMe.first(); d != NULL; d = m_lstDependingOnMe.next())
  {
    for (int c = d->Left(); c <= d->Right(); c++)
    {
      for (int r = d->Top(); r <= d->Bottom(); r++)
      {
	d->Table()->cellAt( c, r )->setCalcDirtyFlag();
      }
    }
  }

  if ( m_content != Formula )
  {
    /* we set it temporarily to true to handle recursion (although that shouldn't happen if it's not a
       formula - we might as well be safe).
    */
    clearFlag(Flag_CalcDirty);
  }

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
                if (!refresh)
                    return true;

                bind->cellChanged( this );
            }
        }
        return true;
    }
    return false;

}

void KSpreadCell::checkTextInput()
{
    // Goal of this method: determine m_dataType, and the value of the cell
    clearAllErrors();
    m_dValue = 0;

    Q_ASSERT( m_content == Text );
    if ( m_content != Text )
    {
        m_dataType = OtherData;
        return;
    }

    // Get the text from that cell (using result of formula if any)
    QString str = m_strText;
    if ( m_style == ST_Select )
        str = (static_cast<SelectPrivate*>(m_pPrivate))->text();
    else if ( isFormula() )
        str = m_strFormulaOut;

    // If the text is empty, we don't have a value (we use StringData for empty string)
    // If the user stated explicitely that he wanted text (using the format or using a quote),
    // then we don't parse as a value, but as string.
    if ( str.isEmpty() || formatType() == Text_format || str.at(0)=='\'' )
    {
        m_dataType = StringData;
        if(m_pTable->getFirstLetterUpper() && !m_strText.isEmpty())
            m_strText=m_strText[0].upper()+m_strText.right(m_strText.length()-1);
        //setFormatType(Text_format); // shouldn't be necessary. Won't apply with StringData anyway.
        return;
    }

    // Try parsing as various datatypes, to find the type of the cell
    // First as bool
    if ( tryParseBool( str ) )
        return;

    // Then as a number
    QString strStripped = str.stripWhiteSpace();
    if ( tryParseNumber( strStripped ) )
    {
        if ( strStripped.contains('E') || strStripped.contains('e') )
            setFormatType(Scientific);
        else
            checkNumberFormat();
        return;
    }

    // Test if text is a percent value, ending with a '%'
    // It's a bit dirty to do this here, but we have to because the % gets
    // saved into the XML file. It would be cleaner to save only the numerical value
    // and treat the trailing % as pure formatting.
    if( str.at(str.length()-1)=='%')
    {
        QString strTrimmed = str.left(str.length()-1);
        if ( tryParseNumber( strTrimmed ) )
        {
            m_dValue /= 100.0;
            if ( formatType() != Percentage )
            {
                setFormatType(Percentage);
                setPrecision(0); // Only set the precision if the format wasn't percentage.
            }
            setFactor(100.0);
            return;
        }
    }

    // Test for money number
    bool ok;
    double money = locale()->readMoney(str, &ok);
    if ( ok )
    {
        m_dValue = money;
        m_dataType = NumericData;
        setFormatType(Money);
        setFactor(1.0);
        setPrecision(2);
        return;
    }

    if ( tryParseDate( str ) )
    {
        FormatType tmpFormat = formatType();
        if(tmpFormat!=TextDate &&
           !(tmpFormat>=200 && tmpFormat<=216)) // ###
        {
            //test if it's a short date or text date.
            if((locale()->formatDate(valueDate(), false) == str))
                setFormatType(TextDate);
            else
                setFormatType(ShortDate);
        }

        m_strText = str;
        return;
    }

    if ( tryParseTime( str ) )
    {
        // Force default time format if format isn't time
        FormatType tmpFormat = formatType();
        if( tmpFormat!=SecondeTime && tmpFormat!=Time_format1
            && tmpFormat!=Time_format2 && tmpFormat!=Time_format3)
            setFormatType(Time);
        // Parsing as time acts like an autoformat: we even change m_strText
        m_strText=locale()->formatTime(valueTime(), true);
        return;
    }

    // Nothing particular found, then this is simply a string
    m_dataType = StringData;
    if(m_pTable->getFirstLetterUpper() && !m_strText.isEmpty())
        m_strText=m_strText[0].upper()+m_strText.right(m_strText.length()-1);
}

bool KSpreadCell::tryParseBool( const QString& str )
{
    if ( str.lower() == "true" || str.lower() == i18n("True").lower() )
    {
        m_dValue = 1.0;
        m_dataType = BoolData;
        return true;
    }
    if ( str.lower() == "false" || str.lower() == i18n("false").lower() )
    {
        m_dValue = 0.0;
        m_dataType = BoolData;
        return true;
    }
    return false;
}

bool KSpreadCell::tryParseNumber( const QString& str )
{
    // First try to understand the number using the locale
    bool ok;
    double value = locale()->readNumber(str, &ok);
    // If not, try with the '.' as decimal separator
    if ( !ok )
        value = str.toDouble(&ok);

    if ( ok )
    {
        kdDebug(36001) << "KSpreadCell::tryParseNumber '" << str << "' successfully parsed as number: " << value << endl;
        m_dValue = value;
        m_dataType = NumericData;
        return true;
    }

    return false;
}

bool KSpreadCell::tryParseDate( const QString& str )
{
    bool valid = false;
    QDate tmpDate = locale()->readDate(str, &valid);
    if (!valid)
    {
        // Try without the year
        // The tricky part is that we need to remove any separator around the year
        // For instance %Y-%m-%d becomes %m-%d and %d/%m/%Y becomes %d/%m
        // If the year is in the middle, say %m-%Y/%d, we'll remove the sep. before it (%m/%d).
        QString fmt = locale()->dateFormatShort();
        int yearPos = fmt.find( "%Y", 0, false );
        if ( yearPos > -1 )
        {
            if ( yearPos == 0 )
            {
                fmt.remove( 0, 2 );
                while ( fmt[0] != '%' )
                    fmt.remove( 0, 1 );
            } else
            {
                fmt.remove( yearPos, 2 );
                for ( ; yearPos > 0 && fmt[yearPos-1] != '%'; --yearPos )
                    fmt.remove( yearPos, 1 );
            }
            //kdDebug(36001) << "KSpreadCell::tryParseDate short format w/o date: " << fmt << endl;
            tmpDate = locale()->readDate( str, fmt, &valid );
        }
    }
    if (valid)
    {
        Q_ASSERT( tmpDate.isValid() );

        //KLocale::readDate( QString ) doesn't support long dates...
        // (David: it does now...)
        // _If_ the input is a long date, check if the first character isn't a number...
        // (David: why? this looks specific to some countries)

        // Deactivating for now. If you reactivate, please explain better (David).
        //if ( str.contains( ' ' ) == 0 )  //No spaces " " in short dates...
        {
            m_dataType = DateData;
            m_Date = tmpDate;
            return true;
        }
    }
    return false;
}

bool KSpreadCell::tryParseTime( const QString& str )
{
    bool valid = false;
    QTime tmpTime = locale()->readTime(str,&valid);
    if (!valid)
    {
        if(locale()->use12Clock())
        {
            QString stringPm=i18n("pm");
            QString stringAm=i18n("am");
            int pos=0;
            if((pos=str.find(stringPm))!=-1)
            {
                QString tmp=str.mid(0,str.length()-stringPm.length());
                tmp=tmp.simplifyWhiteSpace();
                tmpTime=locale()->readTime(tmp+" "+stringPm, &valid);
                if(!valid)
                    tmpTime=locale()->readTime(tmp+":00 "+stringPm, &valid);
            }
            else if((pos=str.find(stringAm))!=-1)
            {
                QString tmp=str.mid(0,str.length()-stringAm.length());
                tmp=tmp.simplifyWhiteSpace();
                tmpTime=locale()->readTime(tmp+" "+stringAm, &valid);
                if (!valid)
                    tmpTime=locale()->readTime(tmp+":00 "+stringAm, &valid);
            }
        }
    }
    if(valid)
    {
        m_dataType = TimeData;
        m_Time = tmpTime;
    }
    return valid;
}

void KSpreadCell::checkNumberFormat()
{
    if ( formatType() == Number && m_dataType == NumericData )
    {
        if ( valueDouble() > 1e+10 )
            setFormatType( Scientific );
    }
}

bool KSpreadCell::cellDependsOn(KSpreadTable *table, int col, int row)
{
  bool isdep = FALSE;

  KSpreadDependency *dep;
  for ( dep = m_lstDepends.first(); dep != 0L && !isdep; dep = m_lstDepends.next() )
  {
    if (dep->Table() == table &&
	dep->Left() <= col && dep->Right() >= col &&
	dep->Top() <= row && dep->Bottom() >= row)
    {
      isdep = TRUE;
    }
  }
  return isdep;
}

QDomElement KSpreadCell::save( QDomDocument& doc, int _x_offset, int _y_offset, bool force )
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
    QDomElement format = KSpreadLayout::save( doc, force );
    if ( format.hasChildNodes() || format.attributes().length() ) // don't save empty tags
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


    QDomElement conditionElement = conditions.SaveConditions(doc);

    if ( !conditionElement.isNull() )
    {
      cell.appendChild( conditionElement );
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

        QString tmp;
        if(  m_Validity->timeMin.isValid())
                {
                QDomElement timeMin = doc.createElement( "timemin" );
                tmp=m_Validity->timeMin.toString();
                timeMin.appendChild( doc.createTextNode( tmp ) );
                validity.appendChild( timeMin );
                }
        if(  m_Validity->timeMax.isValid())
                {
                QDomElement timeMax = doc.createElement( "timemax" );
                tmp=m_Validity->timeMax.toString();
                timeMax.appendChild( doc.createTextNode( tmp ) );
                validity.appendChild( timeMax );
                }

        if(m_Validity->dateMin.isValid())
                {
                QDomElement dateMin = doc.createElement( "datemin" );
                QString tmp("%1/%2/%3");
                tmp = tmp.arg(m_Validity->dateMin.year()).arg(m_Validity->dateMin.month()).arg(m_Validity->dateMin.day());
                dateMin.appendChild( doc.createTextNode( tmp ) );
                validity.appendChild( dateMin );
                }
        if( m_Validity->dateMax.isValid())
                {
                QDomElement dateMax = doc.createElement( "datemax" );
                QString tmp("%1/%2/%3");
                tmp = tmp.arg(m_Validity->dateMax.year()).arg(m_Validity->dateMax.month()).arg(m_Validity->dateMax.day());
                dateMax.appendChild( doc.createTextNode( tmp ) );
                validity.appendChild( dateMax );
                }

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
        // Formulas need to be encoded to ensure that they
        // are position independent.
        if ( isFormula() )
        {
            calc();
            QDomElement text = doc.createElement( "text" );
            text.appendChild( doc.createTextNode( encodeFormula() ) );
            cell.appendChild( text );

            /* we still want to save the results of the formula */
            QDomElement formulaResult = doc.createElement( "result" );
            QString str( m_strOutText );
            saveCellResult( doc, formulaResult, str );
            cell.appendChild( formulaResult );

        }
        // Have to be saved in some CDATA section because of too many
        // special charatcers.
        else if ( content() == RichText )//|| content() == VisualFormula )
        {
            QDomElement text = doc.createElement( "text" );
            text.appendChild( doc.createCDATASection( m_strText ) );
            cell.appendChild( text );
        }
        else
        {
            // Save the cell contents (in a locale-independent way)
            QString str( m_strText );
            QDomElement text = doc.createElement( "text" );
            saveCellResult( doc, text, str );
            cell.appendChild( text );
        }
    }
    if ( cell.hasChildNodes() || cell.attributes().length() > 2 ) // don't save empty tags
        // (the >2 is due to "row" and "column" attributes)
        return cell;
    else
        return QDomElement();
}

bool KSpreadCell::saveCellResult( QDomDocument& doc, QDomElement& result,
                                  QString defaultStr )
{
  QString str = defaultStr;
  result.setAttribute( "dataType", dataTypeToString( m_dataType ) );
  if( m_dataType == DateData )
  {
    str = "%1/%2/%3";
    str = str.arg(valueDate().year()).arg(valueDate().month()).
          arg(valueDate().day());
  }
  else if( m_dataType == TimeData )
  {
    str = valueTime().toString();
  }
  else if ( m_dataType == BoolData )
  {
    // See comment in KSpreadCell::loadCellData
    //str = m_dValue == 1.0 ? "true" : "false";
    str = m_strText;
  }
  else if ( m_dataType == NumericData )
  {
    str = QString::number(valueDouble(), 'g', DBL_DIG);
  }
  result.appendChild( doc.createTextNode( str ) );

  return true; /* really isn't much of a way for this function to fail */
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
    if ( m_iRow < 1 || m_iRow > KS_rowMax )
    {
        kdDebug(36001) << "KSpreadCell::load: Value out of Range Cell:row=" << m_iRow << endl;
        return false;
    }
    if ( m_iColumn < 1 || m_iColumn > KS_colMax )
    {
        kdDebug(36001) << "KSpreadCell::load: Value out of Range Cell:column=" << m_iColumn << endl;
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
            if ( i < 0 || i > KS_spanMax )
            {
                kdDebug(36001) << "Value out of range Cell::colspan=" << i << endl;
                return false;
            }
            m_iExtraXCells = i;
            if ( i > 0 )
            {
              setFlag(Flag_ForceExtra);
            }
        }

        if ( f.hasAttribute( "rowspan" ) )
        {
            int i = f.attribute("rowspan").toInt( &ok );
            if ( !ok ) return false;
            // Validation
            if ( i < 0 || i > KS_spanMax )
            {
                kdDebug(36001) << "Value out of range Cell::rowspan=" << i << endl;
                return false;
            }
            m_iExtraYCells = i;
            if ( i > 0 )
            {
              setFlag(Flag_ForceExtra);
            }
        }

        if(testFlag(Flag_ForceExtra))
        {
            forceExtraCells(m_iColumn,m_iRow,m_iExtraXCells,m_iExtraYCells);
        }

    }

    //
    // Load the condition section of a cell.
    //
    QDomElement conditionsElement = cell.namedItem( "condition" ).toElement();
    if ( !conditionsElement.isNull())
    {
      conditions.LoadConditions( conditionsElement );
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
        QDomElement timeMin = validity.namedItem( "timemin" ).toElement();
        if ( !timeMin.isNull()  )
        {
            m_Validity->timeMin=toTime(timeMin);
         }
        QDomElement timeMax = validity.namedItem( "timemax" ).toElement();
        if ( !timeMax.isNull()  )
        {
            m_Validity->timeMax=toTime(timeMax);
         }
        QDomElement dateMin = validity.namedItem( "datemin" ).toElement();
        if ( !dateMin.isNull()  )
        {
            m_Validity->dateMin=toDate(dateMin);
         }
        QDomElement dateMax = validity.namedItem( "datemax" ).toElement();
        if ( !dateMax.isNull()  )
        {
            m_Validity->dateMax=toDate(dateMax);
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

    if (!text.isNull() && (pm == ::Normal || pm == ::Text || pm == ::NoBorder ))
    {
      /* older versions mistakenly put the datatype attribute on the cell
         instead of the text.  Just move it over in case we're parsing
         an old document */
      if ( cell.hasAttribute( "dataType" ) ) // new docs
      {
        text.setAttribute( "dataType", cell.attribute( "dataType" ) );
      }

      loadCellData(text, op);
    }

    if ( !f.isNull() && f.hasAttribute( "style" ) )
        setStyle( (Style)f.attribute("style").toInt() );

    return true;
}

bool KSpreadCell::loadCellData(QDomElement text, Operation op )
{
  QString t = text.text();
  t = t.stripWhiteSpace();

  // A formula like =A1+A2 ?
  if( t[0] == '=' )
  {
    clearFormula();
    t = decodeFormula( t, m_iColumn, m_iRow );
    m_strText = pasteOperation( t, m_strText, op );

    setFlag(Flag_LayoutDirty);
    clearAllErrors();
    m_content = Formula;

    if ( !m_pTable->isLoading() ) // i.e. when pasting
      if ( !makeFormula() )
        kdError(36001) << "ERROR: Syntax ERROR" << endl;
  }
  else
  {
    bool newStyleLoading = true;
    if ( text.hasAttribute( "dataType" ) ) // new docs
    {
      m_dataType = stringToDataType( text.attribute( "dataType" ) );
    }
    else // old docs: do the ugly solution of calling checkTextInput to parse the text
    {
      // ...except for date/time
      FormatType cellFormatType = formatType();
      if ((cellFormatType==KSpreadCell::TextDate ||
           cellFormatType==KSpreadCell::ShortDate
           ||((int)(cellFormatType)>=200 && (int)(cellFormatType)<=217))
          && ( t.contains('/') == 2 ))
        m_dataType = DateData;
      else if ( (cellFormatType==KSpreadCell::Time
                 || cellFormatType==KSpreadCell::SecondeTime
                 ||cellFormatType==KSpreadCell::Time_format1
                 ||cellFormatType==KSpreadCell::Time_format2
                 ||cellFormatType==KSpreadCell::Time_format3)
                && ( t.contains(':') == 2 ) )
        m_dataType = TimeData;
      else
      {
        m_strText = pasteOperation( t, m_strText, op );
        checkTextInput();
        //kdDebug(36001) << "KSpreadCell::load called checkTextInput, got m_dataType=" << dataTypeToString( m_dataType ) << "  t=" << t << endl;
        newStyleLoading = false;
      }
    }

    if ( newStyleLoading )
    {
      m_dValue = 0.0;
      clearAllErrors();
      switch ( m_dataType ) {
      case BoolData:
      {
#if 0
// Problem: saving simply 'true' and 'false' means we don't know
// if we should restore it as true/false, True/False or i18n("True")/i18n("False") ....
// OTOH saving the original text means an environment, in another language, will not parse it.
// We should save both, the bool value and the text...
        if ( t == "false" )
          m_dValue = 0.0;
        else if ( t == "true" )
          m_dValue = 1.0;
        else
          kdWarning() << "Cell with BoolData, should be true or false: " << t << endl;
#endif
        m_strText = pasteOperation( t, m_strText, op );
        bool ok = tryParseBool( m_strText );
        if ( !ok )
          kdWarning(36001) << "Couldn't parse " << t << " as bool." << endl;
        break;
      }
      case NumericData:
      {
        bool ok = false;
        m_dValue = t.toDouble(&ok); // We save in non-localized format
        m_strText = pasteOperation( t, m_strText, op );
        if ( !ok )
          kdWarning(36001) << "Couldn't parse '" << t << "' as number." << endl;
        if ( formatType() == Percentage )
        {
          setFactor(100.0); // should have been already done by loadLayout
          m_strText += '%';
        }

        break;
      }
      case DateData:
      {
        int pos = t.find('/');
        int year = t.mid(0,pos).toInt();
        int pos1 = t.find('/',pos+1);
        int month = t.mid(pos+1,((pos1-1)-pos)).toInt();
        int day = t.right(t.length()-pos1-1).toInt();
        m_Date = QDate(year,month,day);
        if(valueDate().isValid() ) // Should always be the case for new docs
          m_strText = locale()->formatDate( valueDate(), true );
        else { // This happens with old docs, when format is set wrongly to date
          m_strText = pasteOperation( t, m_strText, op );
          checkTextInput();
        }
        break;
      }
      case TimeData:
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
        if(valueTime().isValid() ) // Should always be the case for new docs
          m_strText = locale()->formatTime( valueTime(), true );
        else { // This happens with old docs, when format is set wrongly to time
          m_strText = pasteOperation( t, m_strText, op );
                        checkTextInput();
        }
        break;
      }
      // A StringData, QML or a visual formula
      default:
        // Set the cell's text
        m_strText = pasteOperation( t, m_strText, op );
      }
      setFlag(Flag_LayoutDirty);
    }
  }

  if ( !m_pTable->isLoading() )
    setCellText(m_strText);

  return true;
}


QTime KSpreadCell::toTime(QDomElement &element)
{
    QString t = element.text();
    t = t.stripWhiteSpace();
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
    return valueTime();
}

QDate KSpreadCell::toDate(QDomElement &element)
{
    QString t = element.text();
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
    return valueDate();
}

const char* KSpreadCell::s_dataTypeToString[] = {
    "Str", "Bool", "Num", "Date", "Time", "Other", 0 };
QString KSpreadCell::dataTypeToString( DataType dt ) const
{
    Q_ASSERT( dt <= LastDataType );
    if ( dt <= LastDataType )
        return QString::fromLatin1( s_dataTypeToString[ dt ] );
    else
        return QString::null; // error
}

KSpreadCell::DataType KSpreadCell::stringToDataType( const QString& str ) const
{
    for ( int i = 0 ; s_dataTypeToString[i] ; ++i )
        if ( str == s_dataTypeToString[i] )
            return static_cast<DataType>(i);
    kdWarning(36001) << "Unknown datatype " << str << endl;
    return StringData;
}

QString KSpreadCell::pasteOperation( QString new_text, QString old_text, Operation op )
{
    if ( op == OverWrite )
        return new_text;

    QString tmp_op;
    QString tmp;
    QString old;

    if( !new_text.isEmpty() && new_text[0] == '=' )
    {
        tmp = new_text.right( new_text.length() - 1 );
    }
    else
    {
        tmp = new_text;
    }

    if ( old_text.isEmpty() && ( op == Add || op == Mul
                                 || op == Sub || op == Div ) )
    {
      old_text = "=0";
    }

    if( !old_text.isEmpty() && old_text[0] == '=' )
    {
        old = old_text.right( old_text.length() - 1 );
    }
    else
    {
        old = old_text;
    }

    bool b1, b2;
    tmp.toDouble( &b1 );
    old.toDouble( &b2 );
    if (b1 && !b2 && old.length() == 0)
    {
      old = "0";
      b2 = true;
    }

    if( b1 && b2 )
    {
        switch( op )
        {
        case  Add:
            tmp_op = "="+QString::number(old.toDouble()+tmp.toDouble());
            break;
        case Mul :
            tmp_op = "="+QString::number(old.toDouble()*tmp.toDouble());
            break;
        case Sub:
            tmp_op = "="+QString::number(old.toDouble()-tmp.toDouble());
            break;
        case Div:
            tmp_op = "="+QString::number(old.toDouble()/tmp.toDouble());
            break;
        default:
            Q_ASSERT( 0 );
        }

        tmp_op = decodeFormula( tmp_op, m_iColumn, m_iRow );
        setFlag(Flag_LayoutDirty);
        clearAllErrors();
        m_content = Formula;

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
            Q_ASSERT( 0 );
        }

        clearFormula();
        tmp_op = decodeFormula( tmp_op, m_iColumn, m_iRow );
        setFlag(Flag_LayoutDirty);
        clearAllErrors();
        m_content = Formula;

        return tmp_op;
    }

    new_text = decodeFormula( new_text, m_iColumn, m_iRow );
    setFlag(Flag_LayoutDirty);
    clearAllErrors();
    m_content = Formula;

    return new_text;
}

void KSpreadCell::setStyle( Style _s )
{
  if ( m_style == _s )
    return;

  m_style = _s;
  setFlag(Flag_LayoutDirty);

  if ( m_pPrivate )
    delete m_pPrivate;
  m_pPrivate = 0;

  if ( _s != ST_Select )
    return;

  m_pPrivate = new SelectPrivate( this );

  SelectPrivate *s = (SelectPrivate*)m_pPrivate;
  if ( isFormula() )
      s->parse( m_strFormulaOut );
  else
      s->parse( m_strText );
  checkTextInput(); // is this necessary?
  setFlag(Flag_LayoutDirty);

  if ( !m_pTable->isLoading() )
      update();
}

QString KSpreadCell::testAnchor( int _x, int _y )
{
  if ( !m_pQML )
    return QString::null;

  return m_pQML->anchorAt( QPoint( _x, _y ) );
}

void KSpreadCell::tableDies()
{
    // Avoid unobscuring the cells in the destructor.
    m_iExtraXCells = 0;
    m_iExtraYCells = 0;
    m_iMergedXCells = 0;
    m_iMergedYCells = 0;
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

    if(m_Validity!=0)
        delete m_Validity;

    // Unobscure cells.
    for( int x = 0; x <= m_iExtraXCells; ++x )
        for( int y = (x == 0) ? 1 : 0; // avoid looking at (+0,+0)
             y <= m_iExtraYCells; ++y )
    {
        KSpreadCell* cell = m_pTable->cellAt( m_iColumn + x, m_iRow + y );
        if ( cell )
            cell->unobscure(this);
    }

}

bool KSpreadCell::operator > ( const KSpreadCell & cell ) const
{
  if ( isNumeric() ) // ### what about bools ?
  {
    if ( cell.isNumeric() )
      return valueDouble() > cell.valueDouble();
    else
      return false; // numbers are always < than texts
  }
  else if(isDate())
  {
     if( cell.isDate() )
        return valueDate() > cell.valueDate();
     else if (cell.isNumeric())
        return true;
     else
        return false; //date are always < than texts and time
  }
  else if(isTime())
  {
     if( cell.isTime() )
        return valueTime() > cell.valueTime();
     else if( cell.isDate())
        return true; //time are always > than date
     else if( cell.isNumeric())
        return true;
     else
        return false; //time are always < than texts
  }
  else
    return valueString().compare(cell.valueString()) > 0;
}

bool KSpreadCell::operator < ( const KSpreadCell & cell ) const
{
  if ( isNumeric() )
  {
    if ( cell.isNumeric() )
      return valueDouble() < cell.valueDouble();
    else
      return true; // numbers are always < than texts
  }
  else if(isDate())
  {
     if( cell.isDate() )
        return valueDate() < cell.valueDate();
     else if( cell.isNumeric())
        return false;
     else
        return true; //date are always < than texts and time
  }
  else if(isTime())
  {
     if( cell.isTime() )
        return valueTime() < cell.valueTime();
     else if(cell.isDate())
        return false; //time are always > than date
     else if( cell.isNumeric())
        return false;
     else
        return true; //time are always < than texts
  }
  else
    return valueString().compare(cell.valueString()) < 0;
}

bool KSpreadCell::isDefault() const
{
    return ( m_iColumn == 0 );
}

void KSpreadCell::NotifyDepending( int col, int row, KSpreadTable* table, bool isDepending )
{
  if (isDefault())
  {
    return;
  }

  KSpreadDependency *d = NULL;
  bool alreadyInList = false;

  /* see if this cell is already in the list */
  for (d = m_lstDependingOnMe.first(); d != NULL && !alreadyInList; d = m_lstDependingOnMe.next() )
  {
    alreadyInList = (d->Left() <= row && d->Right() >= row &&
		     d->Top() <= col && d->Bottom() >= col &&
		     d->Table() == table);
  }

  if (isDepending && !alreadyInList)
  {
    /* if we're supposed to add it and it's not already in there, add it */
    d = new KSpreadDependency(col, row, table);
    m_lstDependingOnMe.prepend(d);
  }
  else if (!isDepending && alreadyInList)
  {
    /* if we're supposed to remove it and it actually was there, then remove it */
    m_lstDependingOnMe.remove();
  }

  return;
}

void KSpreadCell::NotifyDependancyList(QPtrList<KSpreadDependency> lst, bool isDepending)
{
  KSpreadDependency *d = NULL;

  for (d = lst.first(); d != NULL; d = lst.next())
  {
    for (int c = d->Left(); c <= d->Right(); c++)
    {
      for (int r = d->Top(); r <= d->Bottom(); r++)
      {
	d->Table()->nonDefaultCell( c, r )->NotifyDepending(m_iColumn, m_iRow, m_pTable, isDepending);
      }
    }
  }
}

QValueList<KSpreadConditional> KSpreadCell::GetConditionList()
{
  return conditions.GetConditionList();
}

void KSpreadCell::SetConditionList(QValueList<KSpreadConditional> newList)
{
  conditions.SetConditionList(newList);
}

bool KSpreadCell::hasError() const
{
  return ( testFlag(Flag_ParseError) ||
           testFlag(Flag_CircularCalculation) ||
           testFlag(Flag_DependancyError));

}

void KSpreadCell::clearAllErrors()
{
  clearFlag(Flag_ParseError);
  clearFlag(Flag_CircularCalculation);
  clearFlag(Flag_DependancyError);
}

bool KSpreadCell::calcDirtyFlag()
{
  return (m_content == Formula ? false : testFlag(Flag_CalcDirty));
}

bool KSpreadCell::layoutDirtyFlag() const
{
  return testFlag(Flag_LayoutDirty);
}

void KSpreadCell::clearDisplayDirtyFlag()
{
  clearFlag(Flag_DisplayDirty);
}

void KSpreadCell::setDisplayDirtyFlag()
{
  setFlag(Flag_DisplayDirty);
}

bool KSpreadCell::isForceExtraCells() const
{
  return testFlag(Flag_ForceExtra);
}

void KSpreadCell::clearFlag( CellFlags flag )
{
  m_flagsMask &= ~(Q_UINT32)flag;
}

void KSpreadCell::setFlag( CellFlags flag )
{
  m_flagsMask |= (Q_UINT32)flag;
}

bool KSpreadCell::testFlag( CellFlags flag ) const
{
  return ( m_flagsMask & (Q_UINT32)flag );
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
    m_pCell->checkTextInput(); // is this necessary ?
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

