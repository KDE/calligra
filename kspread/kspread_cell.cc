/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 - 2003 The KSpread Team
                             www.koffice.org/kspread

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

#include "kspread_canvas.h"
#include "kspread_changes.h"
#include "kspread_doc.h"
#include "kspread_global.h"
#include "kspread_map.h"
#include "kspread_sheetprint.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"
#include "kspread_util.h"

#include <kspread_value.h>

#include <kmessagebox.h>

#include <kdebug.h>

/**
 * A pointer to the decimal separator
 */

namespace KSpreadCell_LNS
{
  QChar decimal_point = '\0';
}

using namespace KSpreadCell_LNS;

/*****************************************************************************
 *
 * KSpreadCell
 *
 *****************************************************************************/

KSpreadCell::KSpreadCell( KSpreadSheet * _table, int _column, int _row )
  : KSpreadFormat( _table, _table->doc()->styleManager()->defaultStyle() ),
    m_iRow( _row ),
    m_iColumn( _column ),
    m_dOutTextWidth( 0.0 ),
    m_dOutTextHeight( 0.0 ),
    m_dTextX( 0.0 ),
    m_dTextY( 0.0 ),
    m_iMergedXCells( 0 ),
    m_iMergedYCells( 0 ),
    m_iExtraXCells( 0 ),
    m_iExtraYCells( 0 ),
    m_dExtraWidth( 0.0 ),
    m_dExtraHeight( 0.0 ),
    m_style( ST_Normal ),
    m_pPrivate( 0 ),
    m_content( Text ),
    m_value( KSpreadValue::empty() ),
    m_pQML( 0 ),
    m_pCode( 0 ),
    m_conditions( 0 ),
    m_nbLines( 0 ),
    m_Validity( 0 ),
    m_nextCell( 0 ),
    m_previousCell( 0 )
{
  m_ObscuringCells.clear();

  m_lstDepends.setAutoDelete( true );
  m_lstDependingOnMe.setAutoDelete( true );

  clearAllErrors();
}

KSpreadCell::KSpreadCell( KSpreadSheet * _table, KSpreadStyle * _style, int _column, int _row )
  : KSpreadFormat( _table, _style ),
    m_iRow( _row ),
    m_iColumn( _column ),
    m_dOutTextWidth( 0.0 ),
    m_dOutTextHeight( 0.0 ),
    m_dTextX( 0.0 ),
    m_dTextY( 0.0 ),
    m_iMergedXCells( 0 ),
    m_iMergedYCells( 0 ),
    m_iExtraXCells( 0 ),
    m_iExtraYCells( 0 ),
    m_dExtraWidth( 0.0 ),
    m_dExtraHeight( 0.0 ),
    m_style( ST_Normal ),
    m_pPrivate( 0 ),
    m_content( Text ),
    m_value( KSpreadValue::empty() ),
    m_pQML( 0 ),
    m_pCode( 0 ),
    m_conditions( 0 ),
    m_nbLines( 0 ),
    m_Validity( 0 ),
    m_nextCell( 0 ),
    m_previousCell( 0 )
{
  m_ObscuringCells.clear();

  m_lstDepends.setAutoDelete( true );
  m_lstDependingOnMe.setAutoDelete( true );

  clearAllErrors();
}

KSpreadCell::KSpreadCell( KSpreadSheet *_table, QPtrList<KSpreadDependency> _deponme, int _column, int _row )
  : KSpreadFormat( _table, _table->doc()->styleManager()->defaultStyle() ),
    m_iRow( _row ),
    m_iColumn( _column ),
    m_dOutTextWidth( 0.0 ),
    m_dOutTextHeight( 0.0 ),
    m_dTextX( 0.0 ),
    m_dTextY( 0.0 ),
    m_iMergedXCells( 0 ),
    m_iMergedYCells( 0 ),
    m_iExtraXCells( 0 ),
    m_iExtraYCells( 0 ),
    m_dExtraWidth( 0.0 ),
    m_dExtraHeight( 0.0 ),
    m_style( ST_Normal ),
    m_pPrivate( 0 ),
    m_content( Text ),
    m_value( KSpreadValue::empty() ),
    m_pQML( 0 ),
    m_pCode( 0 ),
    m_conditions( 0 ),
    m_nbLines( 0 ),
    m_Validity( 0 ),
    m_nextCell( 0 ),
    m_previousCell( 0 )
{
  m_ObscuringCells.clear();

  m_lstDepends.setAutoDelete( true );
  m_lstDependingOnMe = _deponme ;
  m_lstDependingOnMe.setAutoDelete( true );

  clearAllErrors();
}

KSpreadSheet * KSpreadCell::sheet() const
{
  return m_pTable;
}

int KSpreadCell::row() const
{
  /* Make sure this isn't called for the default cell.  This assert can save you
     (could have saved me!) the hassle of some very obscure bugs.
  */
  if ( isDefault() )
  {
    kdWarning(36001) << "Error: Calling KSpreadCell::row() for default cell" << endl;
    return 0;
  }
  return m_iRow;
}

int KSpreadCell::column() const
{
  /* Make sure this isn't called for the default cell.  This assert can save you
     (could have saved me!) the hassle of some very obscure bugs.
  */
  if ( isDefault() )
  {
    kdWarning(36001) << "Error: Calling KSpreadCell::column() for default cell" << endl;
    return 0;
  }
  return m_iColumn;
}

void KSpreadCell::copyFormat( KSpreadCell * _cell )
{
    copyFormat( _cell->column(), _cell->row() );
}

void KSpreadCell::copyFormat( int _column, int _row )
{
    const KSpreadCell * cell = m_pTable->cellAt( _column, _row );

    setAlign( cell->align( _column, _row ) );
    setAlignY( cell->alignY( _column, _row ) );
    setTextFont( cell->textFont( _column, _row ) );
    setTextColor( cell->textColor( _column, _row ) );
    setBgColor( cell->bgColor( _column, _row) );
    setLeftBorderPen( cell->leftBorderPen( _column, _row ) );
    setTopBorderPen( cell->topBorderPen( _column, _row ) );
    setBottomBorderPen( cell->bottomBorderPen( _column, _row ) );
    setRightBorderPen( cell->rightBorderPen( _column, _row ) );
    setFallDiagonalPen( cell->fallDiagonalPen( _column, _row ) );
    setGoUpDiagonalPen( cell->goUpDiagonalPen( _column, _row ) );
    setBackGroundBrush( cell->backGroundBrush( _column, _row) );
    setPrecision( cell->precision( _column, _row ) );
    setPrefix( cell->prefix( _column, _row ) );
    setPostfix( cell->postfix( _column, _row ) );
    setFloatFormat( cell->floatFormat( _column, _row ) );
    setFloatColor( cell->floatColor( _column, _row ) );
    setFactor( cell->factor( _column, _row ) );
    setMultiRow( cell->multiRow( _column, _row ) );
    setVerticalText( cell->verticalText( _column, _row ) );
    setStyle( cell->style());
    setDontPrintText( cell->getDontprintText(_column, _row ) );
    setNotProtected( cell->notProtected(_column, _row ) );
    setHideAll(cell->isHideAll(_column, _row ) );
    setHideFormula(cell->isHideFormula(_column, _row ) );
    setIndent( cell->getIndent(_column, _row ) );
    setAngle( cell->getAngle(_column, _row) );
    setFormatType( cell->getFormatType(_column, _row) );
    Currency c;
    if ( cell->currencyInfo( c ) )
      setCurrency( c );

    QValueList<KSpreadConditional> conditionList = cell->conditionList();
    delete m_conditions;
    if ( cell->m_conditions )
      setConditionList( conditionList );
    else
      m_conditions = 0;

    setComment( cell->comment( _column, _row ) );
}

void KSpreadCell::copyAll( KSpreadCell *cell )
{
    Q_ASSERT( !isDefault() ); // trouble ahead...
    copyFormat( cell );
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

    setAction( cell->action() );

    delete m_pPrivate;
    m_pPrivate = 0;
    if ( cell->m_pPrivate )
        m_pPrivate = cell->m_pPrivate->copy( this );
}

void KSpreadCell::defaultStyle()
{
  defaultStyleFormat();

  if ( m_conditions )
  {
    delete m_conditions;
    m_conditions = 0;
  }

  delete m_Validity;
  m_Validity = 0L;
}

void KSpreadCell::formatChanged()
{
  setFlag( Flag_LayoutDirty );
  setFlag( Flag_TextFormatDirty );
}

KSpreadFormat * KSpreadCell::fallbackFormat( int, int row )
{
  return table()->rowFormat( row );
}

const KSpreadFormat * KSpreadCell::fallbackFormat( int, int row ) const
{
  return table()->rowFormat( row );
}

void KSpreadCell::forceExtraCells( int _col, int _row, int _x, int _y )
{
  // Unobscure the objects we obscure right now
  for( int x = _col; x <= _col + m_iExtraXCells; ++x )
    for( int y = _row; y <= _row + m_iExtraYCells; ++y )
      if ( x != _col || y != _row )
      {
        KSpreadCell * cell = m_pTable->nonDefaultCell( x, y );
        cell->unobscure(this);
      }

  // disable forcing ?
  if ( _x == 0 && _y == 0 )
  {
      clearFlag( Flag_ForceExtra );
      m_iExtraXCells  = 0;
      m_iExtraYCells  = 0;
      m_dExtraWidth   = 0.0;
      m_dExtraHeight  = 0.0;
      m_iMergedXCells = 0;
      m_iMergedYCells = 0;
      //refresh the layout
      setFlag( Flag_LayoutDirty );
      return;
  }

    setFlag(Flag_ForceExtra);
    m_iExtraXCells  = _x;
    m_iExtraYCells  = _y;
    m_iMergedXCells = _x;
    m_iMergedYCells = _y;

    // Obscure the cells
    for( int x = _col; x <= _col + _x; ++x )
        for( int y = _row; y <= _row + _y; ++y )
            if ( x != _col || y != _row )
            {
                KSpreadCell * cell = m_pTable->nonDefaultCell( x, y );
                cell->obscure( this, true );
            }

    // Refresh the layout
    // QPainter painter;
    // painter.begin( m_pTable->gui()->canvasWidget() );

    setFlag( Flag_LayoutDirty );
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
    for( int x = m_iColumn; x <= m_iColumn + m_iExtraXCells; ++x )
        for( int y = m_iRow; y <= m_iRow + m_iExtraYCells; ++y )
            if ( x != m_iColumn || y != m_iRow )
            {
                KSpreadCell *cell = m_pTable->nonDefaultCell( x, y );
                cell->unobscure(this);
            }

    m_iColumn = col;
    m_iRow    = row;

    //    m_iExtraXCells = 0;
    //    m_iExtraYCells = 0;
    m_iMergedXCells = 0;
    m_iMergedYCells = 0;

    // Reobscure cells if we are forced to do so.
    //if ( m_bForceExtraCells )
      //  forceExtraCells( col, row, ex, ey );
}

void KSpreadCell::setLayoutDirtyFlag( bool format )
{
    setFlag( Flag_LayoutDirty );
    if ( format )
        setFlag( Flag_TextFormatDirty );

    QValueList<KSpreadCell*>::iterator it  = m_ObscuringCells.begin();
    QValueList<KSpreadCell*>::iterator end = m_ObscuringCells.end();
    for ( ; it != end; ++it )
    {
	(*it)->setLayoutDirtyFlag( format );
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

bool KSpreadCell::isObscuringForced() const
{
  QValueList<KSpreadCell*>::const_iterator it = m_ObscuringCells.begin();
  QValueList<KSpreadCell*>::const_iterator end = m_ObscuringCells.end();
  for ( ; it != end; ++it )
  {
    KSpreadCell *cell = *it;
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

void KSpreadCell::clearObscuringCells()
{
    m_ObscuringCells.clear();
}

void KSpreadCell::obscure( KSpreadCell *cell, bool isForcing )
{
  m_ObscuringCells.remove( cell ); // removes *all* occurrences
  cell->clearObscuringCells();
  if ( isForcing )
  {
    m_ObscuringCells.prepend( cell );
  }
  else
  {
    m_ObscuringCells.append( cell );
  }
  setFlag(Flag_LayoutDirty);
  m_pTable->setRegionPaintDirty( cellRect() );
}

void KSpreadCell::unobscure( KSpreadCell * cell )
{
  m_ObscuringCells.remove( cell );
  setFlag( Flag_LayoutDirty );
  m_pTable->setRegionPaintDirty( cellRect() );
}

void KSpreadCell::clicked( KSpreadCanvas * _canvas )
{
  if ( m_style == KSpreadCell::ST_Normal )
    return;
  else if ( m_style == KSpreadCell::ST_Select )
  {
    // We do only show a menu if the user himself clicked
    // on the cell.
    if ( !_canvas )
      return;

    QPopupMenu *popup = new QPopupMenu( _canvas );
    SelectPrivate *s = (SelectPrivate*) m_pPrivate;

    int id = 0;
    QStringList::ConstIterator it = s->m_lstItems.begin();
    for( ; it != s->m_lstItems.end(); ++it )
        popup->insertItem( *it, id++ );
    QObject::connect( popup, SIGNAL( activated( int ) ),
                      s, SLOT( slotItemSelected( int ) ) );
    RowFormat *rl = m_pTable->rowFormat( row() );
    int tx = m_pTable->columnPos( column(), _canvas );
    double ty = m_pTable->dblRowPos( row(), _canvas );
    double h = rl->dblHeight( _canvas );
    if ( m_iExtraYCells )
      h = m_dExtraHeight;
    ty += h;

    QPoint p( tx, int( ty ) );
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
    kdDebug(36001) << "Failed action in cell " << name() << endl;
    if (m_pTable->doc()->getShowMessageError())
    {
      QString tmp(i18n("Error in cell %1\n\n"));
      tmp = tmp.arg( fullName() );
      tmp += context.exception()->toString( context );
      KMessageBox::error((QWidget*)0L , tmp);
    }
    return;
  }

  KSContext& context2 = m_pTable->doc()->context();
  if ( !m_pTable->doc()->interpreter()->evaluate( context2, code, m_pTable, this ) )
      // Print out exception if any
      if ( context2.exception() &&m_pTable->doc()->getShowMessageError())
      {
          QString tmp(i18n("Error in cell %1\n\n"));
          tmp = tmp.arg( fullName() );
          tmp += context2.exception()->toString( context2 );
          KMessageBox::error( (QWidget*)0L, tmp);
      }
}

QString KSpreadCell::encodeFormula( bool _era, int _col, int _row )
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
                            if (_era)
                                erg += QString( "\%%1" ).arg( col );
                            else
                                erg += QString( "#%1" ).arg( col - _col );

                        if ( fix2 )
                            erg += QString( "$%1#").arg( row );
                        else
                            if (_era)
                                erg += QString( "\%%1#" ).arg( row );
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
        else if ( _text[pos] == '#' || _text[pos] == '$' || _text[pos] == '%')
        {
            bool abs1 = FALSE;
            bool abs2 = FALSE;
            bool era1 = FALSE; // if 1st is relative but encoded absolutely
            bool era2 = FALSE;
            switch ( _text[pos++] ) {
                case '$': abs1 = TRUE; break ;
                case '%': era1 = TRUE; break ;
            }
            int col = 0;
            unsigned int oldPos = pos;
            while ( pos < length && ( _text[pos].isDigit() || _text[pos] == '-' ) ) ++pos;
            if ( pos != oldPos )
                col = _text.mid(oldPos, pos-oldPos).toInt();
            if ( !abs1 && !era1 )
                col += _col;
            // Skip '#' or '$'
            switch ( _text[pos++] ) {
                case '$': abs2 = TRUE; break ;
                case '%': era2 = TRUE; break ;
            }
            int row = 0;
            oldPos = pos;
            while ( pos < length && ( _text[pos].isDigit() || _text[pos] == '-' ) ) ++pos;
            if ( pos != oldPos )
                row = _text.mid(oldPos, pos-oldPos).toInt();
            if ( !abs2 && !era2)
                row += _row;
            // Skip '#' or '$'
            ++pos;
            if ( row < 1 || col < 1 || row > KS_rowMax || col > KS_colMax )
            {
                kdDebug(36001) << "KSpreadCell::decodeFormula: row or column out of range (col: " << col << " | row: " << row << ")" << endl;
                erg = "=\"#### " + i18n("REFERENCE TO COLUMN OR ROW IS OUT OF RANGE") + "\"";
                return erg;
            }
            if ( abs1 )
                erg += "$";
            erg += util_encodeColumnLabelText(col); //Get column text

            if ( abs2 )
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

  for ( int x = m_iColumn + m_iMergedXCells; x <= m_iColumn + m_iExtraXCells; ++x )
    for ( int y = m_iRow + m_iMergedYCells; y <= m_iRow + m_iExtraYCells; ++y )
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

    // due to QSimpleRichText, always make layout for QML
    if ( !testFlag( Flag_LayoutDirty ) && !m_pQML )
      return;

    m_nbLines = 0;
    clearFlag( Flag_CellTooShortX );
    clearFlag( Flag_CellTooShortY );

    freeAllObscuredCells();
    /* but reobscure the ones that are forced obscuring */
    forceExtraCells( m_iColumn, m_iRow, m_iMergedXCells, m_iMergedYCells );

    ColumnFormat *cl1 = m_pTable->columnFormat( _col );
    RowFormat *rl1 = m_pTable->rowFormat( _row );
    if ( cl1->isHide() || ( rl1->dblHeight() <= m_pTable->doc()->unzoomItY( 2 ) ) )
    {
        clearFlag( Flag_LayoutDirty );
        return;
    }

    setOutputText();

    // Empty text?
    if ( m_strOutText.isEmpty() )
    {
        m_strOutText = QString::null;
        if ( isDefault() )
        {
            clearFlag( Flag_LayoutDirty );
            return;
        }
    }

    //
    // Determine the correct font
    //
    applyZoomedFont( _painter, _col, _row );

    /**
     * RichText
     */
    if ( m_pQML  )
    {
        delete m_pQML;

        // TODO: more formatting as QStyleSheet supports
        QString qml_text;
        qml_text += QString("<font face=\"%1\">").arg( _painter.font().family() );
        //if( _painter.font().bold() ) qml_text += "<b>";
        //if( _painter.font().italic() ) qml_text += "<i>";
        //if( _painter.font().underline() ) qml_text += "<u>";

        qml_text += m_strText.mid(1);
        m_pQML = new QSimpleRichText( qml_text, _painter.font() );

        setFlag( Flag_LayoutDirty );

        // Calculate how many cells we could use in addition right hand
        // Never use more then 10 cells.
        int right = 0;
        double max_width = dblWidth( _col );
        bool ende = false;
        int c;
        m_pQML->setWidth( &_painter, (int)max_width );
        for( c = _col + 1; !ende && c <= _col + 10; ++c )
        {
            KSpreadCell *cell = m_pTable->cellAt( c, _row );
            if ( cell && !cell->isEmpty() )
                ende = true;
            else
            {
                ColumnFormat *cl = m_pTable->columnFormat( c );
                max_width += cl->dblWidth();

                // Can we make use of extra cells ?
                int h = m_pQML->height();
                m_pQML->setWidth( &_painter, int( max_width ) );
                if ( m_pQML->height() < h )
                    ++right;
                else
                {
                    max_width -= cl->dblWidth();
                    m_pQML->setWidth( &_painter, int( max_width ) );
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
        max_width = dblWidth( _col );
        ende = ( max_width >= w );
        for( c = _col + 1; !ende && c <= _col + right; ++c )
        {
            KSpreadCell *cell = m_pTable->nonDefaultCell( c, _row );
            cell->obscure( this );
            ColumnFormat *cl = m_pTable->columnFormat( c );
            max_width += cl->dblWidth();
            if ( max_width >= w )
                ende = true;
        }
        m_iExtraXCells = c - _col - 1;

        /* we may have used extra cells, but only cells that we were already
           merged to.
        */
        if( m_iExtraXCells < m_iMergedXCells )
        {
            m_iExtraXCells = m_iMergedXCells;
        }
        else
        {
            m_dExtraWidth = max_width;
        }
        // Occupy the needed extra cells in vertical direction
        double max_height = dblHeight( _row );
        int r = _row;
        ende = ( max_height >= h );
        for( r = _row + 1; !ende && r < _row + 500; ++r )
        {
            bool empty = true;
            for( c = _col; c <= _col + m_iExtraXCells; ++c )
            {
                KSpreadCell *cell = m_pTable->cellAt( c, r );
                if ( cell && !cell->isEmpty() )
                {
                    empty = false;
                    break;
                }
            }
            if ( !empty )
            {
                ende = true;
                break;
            }
            else
            {
                // Occupy this row
                for( c = _col; c <= _col + m_iExtraXCells; ++c )
                {
                    KSpreadCell *cell = m_pTable->nonDefaultCell( c, r );
                    cell->obscure( this );
                }
                RowFormat *rl = m_pTable->rowFormat( r );
                max_height += rl->dblHeight();
                if ( max_height >= h )
                    ende = true;
            }
        }
        m_iExtraYCells = r - _row - 1;
        /* we may have used extra cells, but only cells that we were already
           merged to.
        */
        if( m_iExtraYCells < m_iMergedYCells )
        {
            m_iExtraYCells = m_iMergedYCells;
        }
        else
        {
            m_dExtraHeight = max_height;
        }
        clearFlag( Flag_LayoutDirty );

        textSize( _painter );

        offsetAlign( _col, _row );

        return;
    }

    // Calculate text dimensions
    textSize( _painter );

    QFontMetrics fm = _painter.fontMetrics();

    //
    // Calculate the size of the cell
    //
    RowFormat *rl = m_pTable->rowFormat( m_iRow );
    ColumnFormat *cl = m_pTable->columnFormat( m_iColumn );

    double w = cl->dblWidth();
    double h = rl->dblHeight();

    // Calculate the extraWidth and extraHeight if we are forced to.
    /* Use m_dExtraWidth/height here? Isn't it already calculated?*/
    /* No, they are calculated here only (beside of QML part above) Philipp */
    if ( testFlag( Flag_ForceExtra ) )
    {
        for ( int x = _col + 1; x <= _col + m_iExtraXCells; x++ )
        {
            ColumnFormat *cl = m_pTable->columnFormat( x );
            w += cl->dblWidth() ;
        }
        for ( int y = _row + 1; y <= _row + m_iExtraYCells; y++ )
        {
            RowFormat *rl = m_pTable->rowFormat( y );
            h += rl->dblHeight() ;
        }
    }
    m_dExtraWidth = w;
    m_dExtraHeight = h;

    // Some space for the little button of the combo box
    if ( m_style == ST_Select )
        w -= 16.0;

    // Do we need to break the line into multiple lines and are we allowed to
    // do so?
    int lines = 1;
    if ( m_dOutTextWidth > w - 2 * BORDER_SPACE - leftBorderWidth( _col, _row ) -
         rightBorderWidth( _col, _row ) && multiRow( _col, _row ) )
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
                pos = o.find( ' ', pos );
                double width = m_pTable->doc()->unzoomItX( fm.width( m_strOutText.mid( start, (pos1-start) )
                                                                     + o.mid( pos1, (pos-pos1) ) ) );

                if ( width <= w - 2 * BORDER_SPACE - leftBorderWidth( _col, _row ) -
                              rightBorderWidth( _col, _row ) )
                {
                    m_strOutText += o.mid( pos1, pos - pos1 );
                    pos1 = pos;
                }
                else
                {
                    if( o.at( pos1 ) == ' ' )
                        pos1 = pos1 + 1;
                    if( pos1 != 0 && pos != -1 )
                    {
                        m_strOutText += "\n" + o.mid( pos1, pos - pos1 );
                        lines++;
                    }
                    else
                        m_strOutText += o.mid( pos1, pos - pos1 );
                    start = pos1;
                    pos1 = pos;
                }
                pos++;
            }
            while( o.find( ' ', pos ) != -1 );
        }

        m_dOutTextHeight *= lines;

        m_nbLines = lines;
        m_dTextX = 0.0;

        // Calculate the maximum width
        QString t;
        int i;
        int pos = 0;
        m_dOutTextWidth = 0.0;
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
            double tw = m_pTable->doc()->unzoomItX( fm.width( t ) );
            if ( tw > m_dOutTextWidth )
                m_dOutTextWidth = tw;
        }
        while ( i != -1 );
    }
    m_fmAscent = fm.ascent();

    // Calculate m_dTextX and m_dTextY
    offsetAlign( _col, _row );

    double indent = 0.0;
    int a = effAlignX();
    //apply indent if text is align to left not when text is at right or middle
    if( a == KSpreadCell::Left && !isEmpty() )
        indent = getIndent( _col, _row );

    if( verticalText( _col, _row ) || getAngle( _col, _row ) != 0 )
    {
       RowFormat *rl = m_pTable->rowFormat( _row );

       if( m_dOutTextHeight >= rl->dblHeight() )
       {
         setFlag( Flag_CellTooShortX );
       }
    }

    // Do we have to occupy additional cells right hand ?
    if ( m_dOutTextWidth + indent > w - 2 * BORDER_SPACE -
         leftBorderWidth( _col, _row ) - rightBorderWidth( _col, _row ) )
    {
      int c = m_iColumn;
      int end = 0;
      // Find free cells right hand to this one
      while ( !end )
      {
        ColumnFormat *cl2 = m_pTable->columnFormat( c + 1 );
        KSpreadCell *cell = m_pTable->visibleCellAt( c + 1, m_iRow );
        if ( cell->isEmpty() )
        {
          w += cl2->dblWidth() - 1;
          c++;

          // Enough space ?
          if ( m_dOutTextWidth + indent <= w - 2 * BORDER_SPACE -
               leftBorderWidth( _col, _row ) - rightBorderWidth( _col, _row ) )
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
      if( align( _col, _row ) == KSpreadCell::Left ||
          align( _col, _row ) == KSpreadCell::Undefined )
      {
        if( c - m_iColumn > m_iMergedXCells )
        {
          m_iExtraXCells = c - m_iColumn;
          m_dExtraWidth = w;
          for( int i = m_iColumn + 1; i <= c; ++i )
          {
            KSpreadCell *cell = m_pTable->nonDefaultCell( i, m_iRow );
            cell->obscure( this );
          }
          //Not enough space
          if( end == -1 )
          {
            setFlag( Flag_CellTooShortX );
          }
        }
        else
        {
          setFlag( Flag_CellTooShortX );
        }
      }
      else
      {
        setFlag( Flag_CellTooShortX );
      }
    }

    // Do we have to occupy additional cells at the bottom ?
    if ( ( m_pQML || multiRow( _col, _row ) ) &&
         m_dOutTextHeight > h - 2 * BORDER_SPACE -
         topBorderWidth( _col, _row ) - bottomBorderWidth( _col, _row ) )
    {
      int r = m_iRow;
      int end = 0;
      // Find free cells bottom to this one
      while ( !end )
      {
        RowFormat *rl2 = m_pTable->rowFormat( r + 1 );
        KSpreadCell *cell = m_pTable->visibleCellAt( m_iColumn, r + 1 );
        if ( cell->isEmpty() )
        {
          h += rl2->dblHeight() - 1.0;
          r++;

          // Enough space ?
          if ( m_dOutTextHeight <= h - 2 * BORDER_SPACE -
               topBorderWidth( _col, _row ) - bottomBorderWidth( _col, _row ) )
            end = 1;
        }
        // Not enough space, but the next cell is not empty
        else
          end = -1;
      }

      /* Check to make
         sure we haven't already force-merged enough cells
      */
      if( r - m_iRow > m_iMergedYCells )
      {
        m_iExtraYCells = r - m_iRow;
        m_dExtraHeight = h;
        for( int i = m_iRow + 1; i <= r; ++i )
        {
          KSpreadCell *cell = m_pTable->nonDefaultCell( m_iColumn, i );
          cell->obscure( this );
        }
        //Not enough space
        if( end == -1 )
        {
          setFlag( Flag_CellTooShortY );
        }
      }
      else
      {
        setFlag( Flag_CellTooShortY );
      }
    }

    clearFlag( Flag_LayoutDirty );

    return;
}

void KSpreadCell::setOutputText()
{
  if ( isDefault() )
  {
    m_strOutText = QString::null;
    if ( m_conditions )
      m_conditions->checkMatches();
    return;
  }

  if ( !testFlag( Flag_TextFormatDirty ) )
    return;

  clearFlag( Flag_TextFormatDirty );

  if ( hasError() )
  {
    if ( testFlag( Flag_ParseError ) )
    {
      m_strOutText = "#" + i18n("Parse") + "!";
    }
    else if ( testFlag( Flag_CircularCalculation ) )
    {
      m_strOutText = "#" + i18n("Circle") + "!";
    }
    else if ( testFlag( Flag_DependancyError ) )
    {
      m_strOutText = "#" + i18n("Depend") + "!";
    }
    else
    {
      m_strOutText = "####";
      kdDebug(36001) << "Unhandled error type." << endl;
    }
    if ( m_conditions )
      m_conditions->checkMatches();
    return;
  }


  /**
   * A usual numeric, boolean, date, time or string value.
   */

  //
  // Turn the stored value in a string
  //

  if ( isFormula() && m_pTable->getShowFormula()
       && !( m_pTable->isProtected() && isHideFormula( m_iColumn, m_iRow ) ) )
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
  else if ( m_value.isBoolean() )
  {
    m_strOutText = ( m_value.asBoolean()) ? i18n("True") : i18n("False");
  }
  else if( isDate() )
  {
    m_strOutText = util_dateFormat( locale(), valueDate(), formatType() );
  }
  else if( isTime() )
  {
    m_strOutText = util_timeFormat( locale(), m_value.asDateTime(), formatType() );
  }
  else if ( m_value.isNumber() )
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
    double v = m_value.asFloat() * factor(column(),row());

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
      else if (localizedNumber.find( locale()->currencySymbol()) == ((int)(localizedNumber.length() -
                                                                           locale()->currencySymbol().length())))
        start=locale()->currencySymbol().length() + 1;
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
  else if( m_value.isString() )
  {
    if (!m_value.asString().isEmpty() && m_value.asString()[0]=='\'' )
      m_strOutText = m_value.asString().mid(1);
    else
      m_strOutText = m_value.asString();
  }
  else // When does this happen ?
  {
//    kdDebug(36001) << "Please report: final case of makeLayout ...  m_strText=" << m_strText << endl;
    m_strOutText = m_value.asString();
  }
  if ( m_conditions )
    m_conditions->checkMatches();
}

QString KSpreadCell::createFormat( double value, int _col, int _row )
{
    // if precision is -1, ask for a huge number of decimals, we'll remove
    // the zeros later. Is 8 ok ?
    int p = (precision(_col,_row) == -1) ? 8 : precision(_col,_row) ;
    QString localizedNumber= locale()->formatNumber( value, p );
    int pos = 0;

    // this will avoid displaying negative zero, i.e "-0.0000"
    if( fabs( value ) < DBL_EPSILON ) value = 0.0;

    // round the number, based on desired precision if not scientific is chosen (scientific has relativ precision)
    if( formatType() != Scientific )
    {
        double m[] = { 1, 10, 100, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10 };
        double mm = (p > 10) ? pow(10.0,p) : m[p];
        bool neg = value < 0;
        value = floor( fabs(value)*mm + 0.5 ) / mm;
        if( neg ) value = -value;
    }

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
        localizedNumber = locale()->formatMoney(value, getCurrencySymbol(), p );
        if( floatFormat( _col, _row) == KSpreadCell::AlwaysSigned && value >= 0 )
        {
            if (locale()->positiveSign().isNull())
                localizedNumber = '+' + localizedNumber;
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


void KSpreadCell::offsetAlign( int _col, int _row )
{
    int    a;
    AlignY ay;
    int    tmpAngle;
    bool   tmpVerticalText;
    bool   tmpMultiRow;
    int    tmpTopBorderWidth = effTopBorderPen( _col, _row ).width();

    if ( m_conditions && m_conditions->matchedStyle() )
    {
      if ( m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SAlignX, true ) )
        a = m_conditions->matchedStyle()->alignX();
      else
        a = align( _col, _row );

      if ( m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SVerticalText, true ) )
        tmpVerticalText = m_conditions->matchedStyle()->hasProperty( KSpreadStyle::PVerticalText );
      else
        tmpVerticalText = verticalText( _col, _row );

      if ( m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SMultiRow, true ) )
        tmpMultiRow = m_conditions->matchedStyle()->hasProperty( KSpreadStyle::PMultiRow );
      else
        tmpMultiRow = multiRow( _col, _row );

      if ( m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SAlignY, true ) )
        ay = m_conditions->matchedStyle()->alignY();
      else
        ay = alignY( _col, _row );

      if ( m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SAngle, true ) )
        tmpAngle = m_conditions->matchedStyle()->rotateAngle();
      else
        tmpAngle = getAngle( _col, _row );
    }
    else
    {
      a = align( _col, _row );
      ay = alignY( _col, _row );
      tmpAngle = getAngle( _col, _row );
      tmpVerticalText = verticalText( _col, _row );
      tmpMultiRow = multiRow( _col, _row );
    }

    RowFormat    * rl = m_pTable->rowFormat( _row );
    ColumnFormat * cl = m_pTable->columnFormat( _col );

    double w = cl->dblWidth();
    double h = rl->dblHeight();

    if ( m_iExtraXCells )
        w = m_dExtraWidth;
    if ( m_iExtraYCells )
        h = m_dExtraHeight;

    switch( ay )
    {
     case KSpreadCell::Top:
      if ( tmpAngle == 0 )
        m_dTextY = tmpTopBorderWidth + BORDER_SPACE
          + (double) m_fmAscent / m_pTable->doc()->zoomedResolutionY();
      else
      {
        if ( tmpAngle < 0 )
          m_dTextY = tmpTopBorderWidth + BORDER_SPACE;
        else
          m_dTextY = tmpTopBorderWidth + BORDER_SPACE +
            (double)m_fmAscent * cos( tmpAngle * M_PI / 180 ) /
            m_pTable->doc()->zoomedResolutionY();
      }
      break;

     case KSpreadCell::Bottom:
      if ( !tmpVerticalText && !tmpMultiRow && !tmpAngle )
      {
        m_dTextY = h - BORDER_SPACE - effBottomBorderPen( _col, _row ).width();
        if( m_pQML ) m_dTextY = m_dTextY - m_pQML->height();
      }
      else if ( tmpAngle != 0 )
      {
        if ( h - BORDER_SPACE - m_dOutTextHeight - effBottomBorderPen( _col, _row ).width() > 0 )
        {
          if ( tmpAngle < 0 )
            m_dTextY = h - BORDER_SPACE - m_dOutTextHeight - effBottomBorderPen( _col, _row ).width();
          else
            m_dTextY = h - BORDER_SPACE - m_dOutTextHeight - effBottomBorderPen( _col, _row ).width()
              + (double) m_fmAscent * cos( tmpAngle * M_PI / 180 ) / m_pTable->doc()->zoomedResolutionY();
        }
        else
        {
          if ( tmpAngle < 0 )
            m_dTextY = tmpTopBorderWidth + BORDER_SPACE ;
          else
            m_dTextY = tmpTopBorderWidth + BORDER_SPACE
              + (double) m_fmAscent * cos( tmpAngle * M_PI / 180 ) / m_pTable->doc()->zoomedResolutionY();
        }
      }
      else if ( tmpMultiRow )
      {
        int tmpline = m_nbLines;
        if ( m_nbLines > 1 )
          tmpline = m_nbLines - 1;
        if( h - BORDER_SPACE - m_dOutTextHeight * m_nbLines - effBottomBorderPen( _col, _row ).width() > 0 )
          m_dTextY = h - BORDER_SPACE - m_dOutTextHeight * tmpline - effBottomBorderPen( _col, _row ).width();
        else
          m_dTextY = tmpTopBorderWidth + BORDER_SPACE
            + (double) m_fmAscent / m_pTable->doc()->zoomedResolutionY();
      }
      else
        if ( h - BORDER_SPACE - m_dOutTextHeight - effBottomBorderPen( _col, _row ).width() > 0 )
          m_dTextY = h - BORDER_SPACE - m_dOutTextHeight - effBottomBorderPen( _col, _row ).width()
            + (double)m_fmAscent / m_pTable->doc()->zoomedResolutionY();
        else
          m_dTextY = tmpTopBorderWidth + BORDER_SPACE
            + (double) m_fmAscent / m_pTable->doc()->zoomedResolutionY();
      break;

     case KSpreadCell::Middle:
     case KSpreadCell::UndefinedY:
      if ( !tmpVerticalText && !tmpMultiRow && !tmpAngle )
      {
        m_dTextY = ( h - m_dOutTextHeight ) / 2 + (double) m_fmAscent / m_pTable->doc()->zoomedResolutionY();
      }
      else if ( tmpAngle != 0 )
      {
        if ( h - m_dOutTextHeight > 0 )
        {
          if ( tmpAngle < 0 )
            m_dTextY = ( h - m_dOutTextHeight ) / 2 ;
          else
            m_dTextY = ( h - m_dOutTextHeight ) / 2 +
              (double) m_fmAscent * cos( tmpAngle * M_PI / 180 ) /
              m_pTable->doc()->zoomedResolutionY();
        }
        else
        {
          if ( tmpAngle < 0 )
            m_dTextY = tmpTopBorderWidth + BORDER_SPACE;
          else
            m_dTextY = tmpTopBorderWidth + BORDER_SPACE
              + (double)m_fmAscent * cos( tmpAngle * M_PI / 180 ) / m_pTable->doc()->zoomedResolutionY();
        }
      }
      else if ( tmpMultiRow )
      {
        int tmpline = m_nbLines;
        if ( m_nbLines == 0 )
          tmpline = 1;
        if ( h - m_dOutTextHeight * tmpline > 0 )
          m_dTextY = ( h - m_dOutTextHeight * tmpline ) / 2
            + (double) m_fmAscent / m_pTable->doc()->zoomedResolutionY();
        else
          m_dTextY = tmpTopBorderWidth + BORDER_SPACE
            + (double) m_fmAscent / m_pTable->doc()->zoomedResolutionY();
      }
      else
        if ( h - m_dOutTextHeight > 0 )
          m_dTextY = ( h - m_dOutTextHeight ) / 2 + (double)m_fmAscent / m_pTable->doc()->zoomedResolutionY();
        else
          m_dTextY = tmpTopBorderWidth + BORDER_SPACE
            + (double)m_fmAscent / m_pTable->doc()->zoomedResolutionY();
      break;
    }

    a = effAlignX();
    if ( m_pTable->getShowFormula() && !( m_pTable->isProtected() && isHideFormula( _col, _row ) ) )
      a = KSpreadCell::Left;

    switch( a )
    {
     case KSpreadCell::Left:
      m_dTextX = effLeftBorderPen( _col, _row ).width() + BORDER_SPACE;
      break;
     case KSpreadCell::Right:
      m_dTextX = w - BORDER_SPACE - m_dOutTextWidth - effRightBorderPen( _col, _row ).width();
      break;
     case KSpreadCell::Center:
      m_dTextX = ( w - m_dOutTextWidth ) / 2;
      break;
    }
}

void KSpreadCell::textSize( QPainter &_paint )
{
    QFontMetrics fm = _paint.fontMetrics();
    // Horizontal text ?

    int    tmpAngle;
    int    _row = row();
    int    _col = column();
    bool   tmpVerticalText;
    bool   fontUnderlined;
    AlignY ay;

    if ( m_conditions && m_conditions->matchedStyle() )
    {
      if ( m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SAngle, true ) )
        tmpAngle = m_conditions->matchedStyle()->rotateAngle();
      else
        tmpAngle = getAngle( _col, _row );

      if ( m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SVerticalText, true ) )
        tmpVerticalText = m_conditions->matchedStyle()->hasProperty( KSpreadStyle::PVerticalText );
      else
        tmpVerticalText = verticalText( _col, _row );

      if ( m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SAlignY, true ) )
        ay = m_conditions->matchedStyle()->alignY();
      else
        ay = alignY( _col, _row );

      if ( m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SFontFlag, true ) )
        fontUnderlined = ( m_conditions->matchedStyle()->fontFlags() && (uint) KSpreadStyle::FUnderline );
      else
        fontUnderlined = textFontUnderline( _col, _row );
    }
    else
    {
      tmpAngle = getAngle( _col, _row );
      tmpVerticalText = verticalText( _col, _row );
      ay = alignY( _col, _row );
      fontUnderlined = textFontUnderline( _col, _row );
    }

    if( m_pQML )
    {
     m_dOutTextWidth = m_pTable->doc()->unzoomItX( m_pQML->widthUsed() );
     m_dOutTextHeight = m_pTable->doc()->unzoomItY( m_pQML->height() );
     return;
    }

    if ( !tmpVerticalText && !tmpAngle )
    {
        m_dOutTextWidth = m_pTable->doc()->unzoomItX( fm.width( m_strOutText ) );
        int offsetFont = 0;
        if ( ( ay == KSpreadCell::Bottom ) && fontUnderlined )
        {
            offsetFont = fm.underlinePos() + 1;
        }
        m_dOutTextHeight = m_pTable->doc()->unzoomItY( fm.ascent() + fm.descent() + offsetFont );
    }
    // Rotated text ?
    else if ( tmpAngle!= 0 )
    {
        m_dOutTextHeight = m_pTable->doc()->unzoomItY( int( cos( tmpAngle * M_PI / 180 ) *
                                                            ( fm.ascent() + fm.descent() ) +
                                                       abs( int( ( fm.width( m_strOutText ) *
                                                          sin( tmpAngle * M_PI / 180 ) ) ) ) ) );
        m_dOutTextWidth = m_pTable->doc()->unzoomItX( int( abs( int( ( sin( tmpAngle * M_PI / 180 ) *
                                                                     ( fm.ascent() + fm.descent() ) ) ) ) +
                                                           fm.width( m_strOutText ) *
                                                           cos ( tmpAngle * M_PI / 180 ) ) );
        //kdDebug(36001)<<"m_dOutTextWidth"<<m_dOutTextWidth<<"m_dOutTextHeight"<<m_dOutTextHeight<<endl;
    }
    // Vertical text ?
    else
    {
        int width = 0;
        for ( unsigned int i = 0; i < m_strOutText.length(); i++ )
          width = QMAX( width, fm.width( m_strOutText.at( i ) ) );
        m_dOutTextWidth = m_pTable->doc()->unzoomItX( width );
        m_dOutTextHeight = m_pTable->doc()->unzoomItY( ( fm.ascent() + fm.descent() ) *
                                                       m_strOutText.length() );
    }

}


void KSpreadCell::applyZoomedFont( QPainter &painter, int _col, int _row )
{
    QFont tmpFont( textFont( _col, _row ) );
    if ( m_conditions && m_conditions->matchedStyle() )
    {
      KSpreadStyle * s = m_conditions->matchedStyle();
      if ( s->hasFeature( KSpreadStyle::SFontSize, true ) )
        tmpFont.setPointSizeFloat( s->fontSize() );
      if ( s->hasFeature( KSpreadStyle::SFontFlag, true ) )
      {
        uint flags = s->fontFlags();
        tmpFont.setBold( flags & (uint) KSpreadStyle::FBold );
        tmpFont.setUnderline( flags & (uint) KSpreadStyle::FUnderline );
        tmpFont.setItalic( flags & (uint) KSpreadStyle::FItalic );
        tmpFont.setStrikeOut( flags & (uint) KSpreadStyle::FStrike );
      }
      if ( s->hasFeature( KSpreadStyle::SFontFamily, true ) )
        tmpFont.setFamily( s->fontFamily() );
    }
    //    else
      /*
       * could somebody please explaint why we check for isProtected or isHideFormula here
       *
      if ( m_conditions && m_conditions->currentCondition( condition )
        && !(m_pTable->getShowFormula()
              && !( m_pTable->isProtected() && isHideFormula( m_iColumn, m_iRow ) ) ) )
      {
        if ( condition.fontcond )
            tmpFont = *(condition.fontcond);
        else
            tmpFont = condition.style->font();
      }
      */

    tmpFont.setPointSizeFloat( 0.01 * m_pTable->doc()->zoom() * tmpFont.pointSizeFloat() );
    painter.setFont( tmpFont );
}

void KSpreadCell::calculateTextParameters( QPainter &_paint, int _col, int _row )
{
    applyZoomedFont( _paint, _col, _row );

    textSize( _paint );

    offsetAlign( _col, _row );
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
    m_lstDepends.clear();
    clearFormula();

    setFlag(Flag_ParseError);
    m_strFormulaOut = "####";
    m_value.setError ( "####" );
    setFlag(Flag_LayoutDirty);
    setFlag(Flag_TextFormatDirty);
    if (m_pTable->doc()->getShowMessageError())
    {
      QString tmp(i18n("Error in cell %1\n\n"));
      tmp = tmp.arg( fullName() );
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
  delete m_pCode;
  m_pCode = 0L;
}

bool KSpreadCell::calc(bool delay)
{
  if ( !isFormula() )
    return true;

  if ( testFlag(Flag_Progress) )
  {
    kdError(36001) << "ERROR: Circle" << endl;
    setFlag(Flag_CircularCalculation);
    m_strFormulaOut = "####";
    m_value.setError ( "####" );

    setFlag(Flag_LayoutDirty);
    if ( m_style == ST_Select )
    {
        SelectPrivate *s = (SelectPrivate*)m_pPrivate;
        s->parse( m_strFormulaOut );
    }
    return false;
  }

  if ( m_pCode == 0 )
  {
    if ( testFlag( Flag_ParseError ) )  // there was a parse error
      return false;
    else
    {
      /* we were probably at a "isLoading() = true" state when we originally
       * parsed
       */
      makeFormula();

      if ( m_pCode == 0 ) // there was a parse error
        return false;
    }
  }

  if ( !testFlag( Flag_CalcDirty ) )
    return true;

  if ( delay )
  {
    if ( m_pTable->doc()->delayCalculation() )
      return true;
  }

  setFlag(Flag_LayoutDirty);
  setFlag(Flag_TextFormatDirty);
  clearFlag(Flag_CalcDirty);

  setFlag(Flag_Progress);

  /* calculate any dependancies */
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
	  m_value.setError( "####" );
          clearFlag(Flag_Progress);
	  if ( m_style == ST_Select )
          {
	    SelectPrivate *s = (SelectPrivate*)m_pPrivate;
	    s->parse( m_strFormulaOut );
	  }
	  setFlag(Flag_LayoutDirty);
          clearFlag(Flag_CalcDirty);
	  return false;
	}
      }
    }
  }

  KSContext& context = m_pTable->doc()->context();
  if ( !m_pTable->doc()->interpreter()->evaluate( context, m_pCode, m_pTable, this ) )
  {
    // If we got an error during evaluation ...
    setFlag(Flag_ParseError);
    m_strFormulaOut = "####";
    setFlag(Flag_LayoutDirty);
    m_value.setError( "####" );
    // Print out exception if any
    if ( context.exception() && m_pTable->doc()->getShowMessageError())
    {
      QString tmp(i18n("Error in cell %1\n\n"));
      tmp = tmp.arg( fullName() );
      tmp += context.exception()->toString( context );
      KMessageBox::error( (QWidget*)0L, tmp);
    }

    // setFlag(Flag_LayoutDirty);
    clearFlag(Flag_Progress);
    clearFlag(Flag_CalcDirty);

    if ( m_style == ST_Select )
    {
        SelectPrivate *s = (SelectPrivate*)m_pPrivate;
        s->parse( m_strFormulaOut );
    }
    return false;
  }
  else if ( context.value()->type() == KSValue::DoubleType )
  {
    m_value.setValue ( KSpreadValue( context.value()->doubleValue() ) );
    clearAllErrors();
    checkNumberFormat(); // auto-chooses number or scientific
    // Format the result appropriately
    m_strFormulaOut = createFormat( m_value.asFloat(), m_iColumn, m_iRow );
  }
  else if ( context.value()->type() == KSValue::IntType )
  {
    m_value.setValue ( KSpreadValue( (int)context.value()->intValue() ) );
    clearAllErrors();
    checkNumberFormat(); // auto-chooses number or scientific
    // Format the result appropriately
    m_strFormulaOut = createFormat( m_value.asFloat(), m_iColumn, m_iRow );
  }
  else if ( context.value()->type() == KSValue::BoolType )
  {
    m_value.setValue ( KSpreadValue( context.value()->boolValue() ) );
    clearAllErrors();
    m_strFormulaOut = context.value()->boolValue() ? i18n("True") : i18n("False");
    setFormatType(Number);
  }
  else if ( context.value()->type() == KSValue::TimeType )
  {
    clearAllErrors();
    m_value.setValue( KSpreadValue( context.value()->timeValue() ) );

    //change format
    FormatType tmpFormat = formatType();
    if( tmpFormat != SecondeTime &&  tmpFormat != Time_format1 &&  tmpFormat != Time_format2
        && tmpFormat != Time_format3)
    {
      m_strFormulaOut = locale()->formatTime( m_value.asDateTime().time(), false);
      setFormatType( Time );
    }
    else
    {
      m_strFormulaOut = util_timeFormat(locale(), m_value.asDateTime(), tmpFormat);
    }
  }
  else if ( context.value()->type() == KSValue::DateType)
  {
    clearAllErrors();
    m_value.setValue ( KSpreadValue( context.value()->dateValue() ) );
    FormatType tmpFormat = formatType();
    if( tmpFormat != TextDate
        && !(tmpFormat>=200 &&tmpFormat<=216))
    {
        setFormatType(ShortDate);
        m_strFormulaOut = locale()->formatDate( m_value.asDateTime().date(), true);
    }
    else
    {
        m_strFormulaOut = util_dateFormat( locale(), m_value.asDateTime().date(), tmpFormat);
    }
  }
  else if ( context.value()->type() == KSValue::Empty )
  {
    clearAllErrors();
    m_value = KSpreadValue::empty();
    // Format the result appropriately
    setFormatType(Number);
    m_strFormulaOut = createFormat( 0.0, m_iColumn, m_iRow );
  }
  else
  {
    delete m_pQML;

    m_pQML = 0;
    clearAllErrors();
//FIXME    m_dataType = StringData;
    m_value.setValue( KSpreadValue( context.value()->toString( context ) ) );
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

  return true;
}

void KSpreadCell::paintCell( const KoRect & rect, QPainter & painter,
                             KSpreadView * view, const KoPoint & coordinate,
                             const QPoint &cellRef, bool paintBorderRight,
                             bool paintBorderBottom, bool paintBorderLeft,
                             bool paintBorderTop, QPen & rightPen,
                             QPen & bottomPen, QPen & leftPen,
                             QPen & topPen, bool drawCursor )
{
  if ( testFlag( Flag_PaintingCell ) )
    return;

  setFlag( Flag_PaintingCell );

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

  Q_ASSERT(!(((cellRef.x() != m_iColumn) || (cellRef.y() != m_iRow)) && !isDefault()));

  double left = coordinate.x();

  ColumnFormat * colFormat = m_pTable->columnFormat( cellRef.x() );
  RowFormat    * rowFormat = m_pTable->rowFormat( cellRef.y() );
  double width  = m_iExtraXCells ? m_dExtraWidth  : colFormat->dblWidth();
  double height = m_iExtraYCells ? m_dExtraHeight : rowFormat->dblHeight();

  if ( m_pTable->isRightToLeft() && view && view->canvasWidget() )
    left = view->canvasWidget()->width() - coordinate.x() - width;

  const KoRect cellRect( left, coordinate.y(), width, height );
  bool selected = false;

  if ( view != NULL )
  {
    selected = view->selection().contains( cellRef );

    /* but the cell doesn't look selected if this is the marker cell */
    KSpreadCell * cell = m_pTable->cellAt( view->marker() );
    QPoint bottomRight( view->marker().x() + cell->extraXCells(),
                        view->marker().y() + cell->extraYCells() );
    QRect markerArea( view->marker(), bottomRight );
    selected = selected && !( markerArea.contains( cellRef ) );

    // Dont draw any selection when printing.
    if ( painter.device()->isExtDev() || !drawCursor )
      selected = false;
  }

  // Need to make a new layout ?

  /* TODO - this needs to be taken out eventually - it is done in canvas::paintUpdates */
  if ( testFlag( Flag_LayoutDirty ) )
    makeLayout( painter, cellRef.x(), cellRef.y() );

  if ( !cellRect.intersects( rect ) )
  {
    clearFlag( Flag_PaintingCell );
    return;
  }

  QColor backgroundColor;
  if ( m_conditions && m_conditions->matchedStyle()
       && m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SBackgroundColor, true ) )
    backgroundColor = m_conditions->matchedStyle()->bgColor();
  else
    backgroundColor = bgColor( cellRef.x(), cellRef.y() );

  if ( !isObscuringForced() )
    paintBackground( painter, cellRect, cellRef, selected, backgroundColor );
  if( painter.device()->devType() != QInternal::Printer )
    paintDefaultBorders( painter, rect, cellRect, cellRef, paintBorderRight, paintBorderBottom,
                         paintBorderLeft, paintBorderTop, rightPen, bottomPen, leftPen, topPen );

  /* paint all the cells that this one obscures */
  paintingObscured++;
  paintObscuredCells( rect, painter, view, cellRect, cellRef, paintBorderRight, paintBorderBottom,
                      paintBorderLeft, paintBorderTop, rightPen, bottomPen, leftPen, topPen );
  paintingObscured--;

  //If we print pages then we disable clipping otherwise borders are cut in the middle at the page borders
  if ( painter.device()->isExtDev() )
    painter.setClipping( false );

  if ( !isObscuringForced() )
    paintCellBorders( painter, rect, cellRect, cellRef, paintBorderRight, paintBorderBottom,
                      paintBorderLeft, paintBorderTop, rightPen, bottomPen, leftPen, topPen );

  if ( painter.device()->isExtDev() )
    painter.setClipping( true );

  paintCellDiagonalLines( painter, cellRect, cellRef );

  paintPageBorders( painter, cellRect, cellRef, paintBorderRight, paintBorderBottom );

  /* now print content, if this cell isn't obscured */
  if ( !isObscured() )
    /* don't paint content if this cell is obscured */
  {
    if ( !painter.device()->isExtDev() || m_pTable->print()->printCommentIndicator() )
      paintCommentIndicator( painter, cellRect, cellRef, backgroundColor );
    if ( !painter.device()->isExtDev() || m_pTable->print()->printFormulaIndicator() )
      paintFormulaIndicator( painter, cellRect, backgroundColor );

    paintMoreTextIndicator( painter, cellRect, backgroundColor );

  /**
   * QML ?
   */
    if ( m_pQML
         && ( !painter.device()->isExtDev() || !getDontprintText( cellRef.x(), cellRef.y() ) )
         && !( m_pTable->isProtected() && isHideAll( cellRef.x(), cellRef.y() ) ) )
    {
      paintText( painter, cellRect, cellRef );
    }
    /**
     * Usual Text
     */
    else
    if ( !m_strOutText.isEmpty()
              && ( !painter.device()->isExtDev() || !getDontprintText( cellRef.x(), cellRef.y() ) )
              && !( m_pTable->isProtected() && isHideAll( cellRef.x(), cellRef.y() ) ) )
    {
      paintText( painter, cellRect, cellRef );
    }
  }

  if ( isObscured() && paintingObscured == 0 )
  {
    /* print the cells obscuring this one */

    /* if paintingObscured is > 0, that means drawing this cell was triggered
       while already drawing the obscuring cell -- don't want to cause an
       infinite loop
    */

    /*
      Store the obscuringCells list in a list of QPoint(column, row)
      This avoids crashes during the iteration through obscuringCells,
      when the cells may get non valid or the list itself gets changed
      during a call of obscuringCell->paintCell (this happens e.g. when
      there is an updateDepend)
    */
    QValueList<QPoint> listPoints;
    QValueList<KSpreadCell*>::iterator it = m_ObscuringCells.begin();
    QValueList<KSpreadCell*>::iterator end = m_ObscuringCells.end();
    for ( ; it != end; ++it )
    {
      KSpreadCell *obscuringCell = *it;
      listPoints.append( QPoint( obscuringCell->column(), obscuringCell->row() ) );
    }

    QValueList<QPoint>::iterator it1 = listPoints.begin();
    QValueList<QPoint>::iterator end1 = listPoints.end();
    for ( ; it1 != end1; ++it1 )
    {
      QPoint obscuringCellRef = *it1;
      KSpreadCell *obscuringCell = m_pTable->cellAt( obscuringCellRef.x(), obscuringCellRef.y() );
      if( obscuringCell != 0 )
      {
        double x = m_pTable->dblColumnPos( obscuringCellRef.x() );
        double y = m_pTable->dblRowPos( obscuringCellRef.y() );
        if( view != 0 )
        {
          x -= view->canvasWidget()->xOffset();
          y -= view->canvasWidget()->yOffset();
        }

        KoPoint corner( x, y );
        painter.save();

        QPen rp( obscuringCell->effRightBorderPen( obscuringCellRef.x(), obscuringCellRef.y() ) );
        QPen bp( obscuringCell->effBottomBorderPen( obscuringCellRef.x(), obscuringCellRef.y() ) );
        QPen lp( obscuringCell->effLeftBorderPen( obscuringCellRef.x(), obscuringCellRef.y() ) );
        QPen tp( obscuringCell->effTopBorderPen( obscuringCellRef.x(), obscuringCellRef.y() ) );

        obscuringCell->paintCell( rect, painter, view,
                                  corner, obscuringCellRef, true, true, true, true, rp, bp, lp, tp );
        painter.restore();
      }
    }
  }

  clearFlag( Flag_PaintingCell );
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



void KSpreadCell::paintObscuredCells(const KoRect& rect, QPainter& painter,
                                     KSpreadView* view,
                                     const KoRect &cellRect,
                                     const QPoint &cellRef,
                                     bool paintBorderRight,
                                     bool paintBorderBottom,
                                     bool paintBorderLeft, bool paintBorderTop,
                                     QPen & rightPen, QPen & bottomPen,
                                     QPen & leftPen, QPen & topPen )
{
  // This cell is obscuring other ones? Then we redraw their
  // background and borders before we paint our content there.
  if ( extraXCells() || extraYCells() )
  {
    double ypos = cellRect.y();
    int maxY = extraYCells();
    int maxX = extraXCells();
    for( int y = 0; y <= maxY; ++y )
    {
      double xpos = cellRect.x();
      RowFormat* rl = m_pTable->rowFormat( cellRef.y() + y );

      for( int x = 0; x <= maxX; ++ x )
      {
        ColumnFormat * cl = m_pTable->columnFormat( cellRef.x() + x );
        if ( y != 0 || x != 0 )
        {
          KSpreadCell * cell = m_pTable->cellAt( cellRef.x() + x,
                                                 cellRef.y() + y );

          KoPoint corner( xpos, ypos );
          cell->paintCell( rect, painter, view,
                           corner,
                           QPoint( cellRef.x() + x, cellRef.y() + y ),
                           paintBorderRight, paintBorderBottom, paintBorderLeft, paintBorderTop,
                           rightPen, bottomPen, leftPen, topPen );
        }
        xpos += cl->dblWidth();
      }

      ypos += rl->dblHeight();
    }
  }
}


void KSpreadCell::paintBackground( QPainter& painter, const KoRect &cellRect,
                                   const QPoint &cellRef, bool selected,
                                   QColor &backgroundColor )
{
  QColorGroup defaultColorGroup = QApplication::palette().active();

  QRect zoomedCellRect = table()->doc()->zoomRect( cellRect );
  /*
     If this is not the KS_rowMax and/or KS_colMax, then we reduce width and/or height by one.
     This is due to the fact that the right/bottom most pixel is shared with the
     left/top most pixel of the following cell.
     Only in the case of KS_colMax/KS_rowMax we need to draw even this pixel,
     as there isn't a following cell to draw the background pixel.
   */
   if( cellRef.x() != KS_colMax )
   {
     zoomedCellRect.setWidth( zoomedCellRect.width() - 1 );
   }
   if( cellRef.y() != KS_rowMax )
   {
     zoomedCellRect.setHeight( zoomedCellRect.height() - 1 );
   }

  // Determine the correct background color
  if( selected )
  {
    painter.setBackgroundColor( defaultColorGroup.highlight() );
  }
  else
  {
    QColor bg( backgroundColor );

    if ( !painter.device()->isExtDev() )
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
      if( !bg.isValid() )
        bb.setColor( Qt::white );

      painter.fillRect( zoomedCellRect, bb );
      return;
    }
  }
  // Erase the background of the cell.
  if ( !painter.device()->isExtDev() )
    painter.eraseRect( zoomedCellRect );

  // Draw a background brush
  QBrush bb;
  if ( m_conditions && m_conditions->matchedStyle()
       && m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SBackgroundBrush, true ) )
    bb = m_conditions->matchedStyle()->backGroundBrush();
  else
    bb = backGroundBrush( cellRef.x(), cellRef.y() );

  if( bb.style() != Qt::NoBrush )
  {
    painter.fillRect( zoomedCellRect, bb );
  }

  backgroundColor = painter.backgroundColor();
}

void KSpreadCell::paintDefaultBorders( QPainter& painter, const KoRect &rect,
                                       const KoRect &cellRect,
                                       const QPoint &cellRef,
                                       bool paintBorderRight,
                                       bool paintBorderBottom,
                                       bool paintBorderLeft, bool paintBorderTop,
                                       QPen const & rightPen, QPen const & bottomPen,
                                       QPen const & leftPen, QPen const & topPen )
{
  KSpreadDoc* doc = table()->doc();

  /* Each cell is responsible for drawing it's top and left portions of the
     "default" grid. --Or not drawing it if it shouldn't be there.
     It's even responsible to paint the right and bottom, if it is the last
     cell on a print out*/

  bool paintTop;
  bool paintLeft;
  bool paintBottom;
  bool paintRight;

  paintLeft = ( paintBorderLeft && leftPen.style() == Qt::NoPen && table()->getShowGrid() );
  paintRight = ( paintBorderRight && rightPen.style() == Qt::NoPen  && table()->getShowGrid() );
  paintTop = ( paintBorderTop && topPen.style() == Qt::NoPen && table()->getShowGrid() );
  paintBottom = ( paintBorderBottom && table()->getShowGrid()
                  && bottomPen.style() == Qt::NoPen );

  QValueList<KSpreadCell*>::const_iterator it  = m_ObscuringCells.begin();
  QValueList<KSpreadCell*>::const_iterator end = m_ObscuringCells.end();
  for ( ; it != end; ++it )
  {
    KSpreadCell *cell = *it;
    paintLeft = paintLeft && ( cell->column() == cellRef.x() );
    paintTop  = paintTop && ( cell->row() == cellRef.y() );
    paintBottom = false;
    paintRight = false;
  }

  /* should we do the left border? */
  if ( paintLeft )
  {
    int dt = 0;
    int db = 0;

    if ( cellRef.x() > 1 )
    {
      QPen t = m_pTable->cellAt( cellRef.x() - 1, cellRef.y() )->effTopBorderPen( cellRef.x() - 1, cellRef.y() );
      QPen b = m_pTable->cellAt( cellRef.x() - 1, cellRef.y() )->effBottomBorderPen( cellRef.x() - 1, cellRef.y() );

      if ( t.style() != Qt::NoPen )
        dt = ( t.width() + 1 )/2;
      if ( b.style() != Qt::NoPen )
        db = ( t.width() / 2);
    }

    painter.setPen( table()->doc()->defaultGridPen() );

    //If we are on paper printout, we limit the length of the lines
    //On paper, we always have full cells, on screen not
    if ( painter.device()->isExtDev() )
    {
      painter.drawLine( doc->zoomItX( QMAX( rect.left(),   cellRect.x() ) ),
                        doc->zoomItY( QMAX( rect.top(),    cellRect.y() + dt ) ),
                        doc->zoomItX( QMIN( rect.right(),  cellRect.x() ) ),
                        doc->zoomItY( QMIN( rect.bottom(), cellRect.bottom() - db ) ) );
    }
    else
    {
      painter.drawLine( doc->zoomItX( cellRect.x() ),
                        doc->zoomItY( cellRect.y() + dt ),
                        doc->zoomItX( cellRect.x() ),
                        doc->zoomItY( cellRect.bottom() - db ) );
    }
  }

  /* should we do the right border? */
  if ( paintRight )
  {
    int dt = 0;
    int db = 0;

    if ( cellRef.x() < KS_colMax )
    {
      QPen t = m_pTable->cellAt( cellRef.x() + 1, cellRef.y() )->effTopBorderPen( cellRef.x() + 1, cellRef.y() );
      QPen b = m_pTable->cellAt( cellRef.x() + 1, cellRef.y() )->effBottomBorderPen( cellRef.x() + 1, cellRef.y() );

      if ( t.style() != Qt::NoPen )
        dt = ( t.width() + 1 )/2;
      if ( b.style() != Qt::NoPen )
        db = ( t.width() / 2);
    }

    painter.setPen( table()->doc()->defaultGridPen() );
    //If we are on paper printout, we limit the length of the lines
    //On paper, we always have full cells, on screen not
    if ( painter.device()->isExtDev() )
    {
      painter.drawLine( doc->zoomItX( QMAX( rect.left(),   cellRect.right() ) ),
                        doc->zoomItY( QMAX( rect.top(),    cellRect.y() + dt ) ),
                        doc->zoomItX( QMIN( rect.right(),  cellRect.right() ) ),
                        doc->zoomItY( QMIN( rect.bottom(), cellRect.bottom() - db ) ) );
    }
    else
    {
      painter.drawLine( doc->zoomItX( cellRect.right() ),
                        doc->zoomItY( cellRect.y() + dt ),
                        doc->zoomItX( cellRect.right() ),
                        doc->zoomItY( cellRect.bottom() - db ) );
    }
  }

  /* should we do the top border? */
  if ( paintTop )
  {
    int dl = 0;
    int dr = 0;
    if ( cellRef.y() > 1 )
    {
      QPen l = m_pTable->cellAt( cellRef.x(), cellRef.y() - 1 )->effLeftBorderPen( cellRef.x(), cellRef.y() - 1 );
      QPen r = m_pTable->cellAt( cellRef.x(), cellRef.y() - 1 )->effRightBorderPen( cellRef.x(), cellRef.y() - 1 );

      if ( l.style() != Qt::NoPen )
        dl = ( l.width() - 1 ) / 2 + 1;
      if ( r.style() != Qt::NoPen )
        dr = r.width() / 2;
    }

    painter.setPen( table()->doc()->defaultGridPen() );
    //If we are on paper printout, we limit the length of the lines
    //On paper, we always have full cells, on screen not
    if ( painter.device()->isExtDev() )
    {
      painter.drawLine( doc->zoomItX( QMAX( rect.left(),   cellRect.x() + dl ) ),
                        doc->zoomItY( QMAX( rect.top(),    cellRect.y() ) ),
                        doc->zoomItX( QMIN( rect.right(),  cellRect.right() - dr ) ),
                        doc->zoomItY( QMIN( rect.bottom(), cellRect.y() ) ) );
    }
    else
    {
      painter.drawLine( doc->zoomItX( cellRect.x() + dl ),
                        doc->zoomItY( cellRect.y() ),
                        doc->zoomItX( cellRect.right() - dr ),
                        doc->zoomItY( cellRect.y() ) );
    }
  }

  /* should we do the bottom border? */
  if ( paintBottom )
  {
    int dl = 0;
    int dr = 0;
    if ( cellRef.y() < KS_rowMax )
    {
      QPen l = m_pTable->cellAt( cellRef.x(), cellRef.y() + 1 )->effLeftBorderPen( cellRef.x(), cellRef.y() + 1 );
      QPen r = m_pTable->cellAt( cellRef.x(), cellRef.y() + 1 )->effRightBorderPen( cellRef.x(), cellRef.y() + 1 );

      if ( l.style() != Qt::NoPen )
        dl = ( l.width() - 1 ) / 2 + 1;
      if ( r.style() != Qt::NoPen )
        dr = r.width() / 2;
    }

    painter.setPen( table()->doc()->defaultGridPen() );
    //If we are on paper printout, we limit the length of the lines
    //On paper, we always have full cells, on screen not
    if ( painter.device()->isExtDev() )
    {
      painter.drawLine( doc->zoomItX( QMAX( rect.left(),   cellRect.x() + dl ) ),
                        doc->zoomItY( QMAX( rect.top(),    cellRect.bottom() ) ),
                        doc->zoomItX( QMIN( rect.right(),  cellRect.right() - dr ) ),
                        doc->zoomItY( QMIN( rect.bottom(), cellRect.bottom() ) ) );
    }
    else
    {
      painter.drawLine( doc->zoomItX( cellRect.x() + dl ),
                        doc->zoomItY( cellRect.bottom() ),
                        doc->zoomItX( cellRect.right() - dr ),
                        doc->zoomItY( cellRect.bottom() ) );
    }
  }
}


void KSpreadCell::paintCommentIndicator( QPainter& painter,
                                         const KoRect &cellRect,
                                         const QPoint &/*cellRef*/,
                                         QColor &backgroundColor )
{
  KSpreadDoc * doc = table()->doc();

  // Point the little corner if there is a comment attached
  // to this cell.
  if ( ( m_mask & (uint) PComment )
       && cellRect.width() > 10.0
       && cellRect.height() > 10.0
       && ( table()->print()->printCommentIndicator()
            || ( !painter.device()->isExtDev() && doc->getShowCommentIndicator() ) ) )
  {
    QColor penColor = Qt::red;
    //If background has high red part, switch to blue
    if ( qRed( backgroundColor.rgb() ) > 127 &&
         qGreen( backgroundColor.rgb() ) < 80 &&
         qBlue( backgroundColor.rgb() ) < 80 )
    {
        penColor = Qt::blue;
    }

    QPointArray point( 3 );
    point.setPoint( 0, doc->zoomItX( cellRect.right() - 5.0 ),
                       doc->zoomItY( cellRect.y() ) );
    point.setPoint( 1, doc->zoomItX( cellRect.right() ),
                       doc->zoomItY( cellRect.y() ) );
    point.setPoint( 2, doc->zoomItX( cellRect.right() ),
                       doc->zoomItY( cellRect.y() + 5.0 ) );
    painter.setBrush( QBrush( penColor ) );
    painter.setPen( Qt::NoPen );
    painter.drawPolygon( point );
  }
}


// small blue rectangle if this cell holds a formula
void KSpreadCell::paintFormulaIndicator( QPainter& painter,
                                         const KoRect &cellRect,
                                         QColor &backgroundColor )
{
  if( isFormula() &&
      m_pTable->getShowFormulaIndicator() &&
      cellRect.width() > 10.0 &&
      cellRect.height() > 10.0 )
  {
    KSpreadDoc* doc = table()->doc();

    QColor penColor = Qt::blue;
    //If background has high blue part, switch to red
    if( qRed( backgroundColor.rgb() ) < 80 &&
        qGreen( backgroundColor.rgb() ) < 80 &&
        qBlue( backgroundColor.rgb() ) > 127 )
    {
        penColor = Qt::red;
    }

    QPointArray point( 3 );
    point.setPoint( 0, doc->zoomItX( cellRect.x() ),
                       doc->zoomItY( cellRect.bottom() - 6.0 ) );
    point.setPoint( 1, doc->zoomItX( cellRect.x() ),
                       doc->zoomItY( cellRect.bottom() ) );
    point.setPoint( 2, doc->zoomItX( cellRect.x() + 6.0 ),
                       doc->zoomItY( cellRect.bottom() ) );
    painter.setBrush( QBrush( penColor ) );
    painter.setPen( Qt::NoPen );
    painter.drawPolygon( point );
  }
}


void KSpreadCell::paintMoreTextIndicator( QPainter& painter,
                                          const KoRect &cellRect,
                                          QColor &backgroundColor )
{
  //show  a red triangle when it's not possible to write all text in cell
  //don't print the red triangle if we're printing

  if( testFlag( Flag_CellTooShortX ) &&
      !painter.device()->isExtDev() &&
      cellRect.height() > 4.0  &&
      cellRect.width() > 4.0 )
  {
    KSpreadDoc* doc = table()->doc();

    QColor penColor = Qt::red;
    //If background has high red part, switch to blue
    if( qRed( backgroundColor.rgb() ) > 127 &&
        qGreen( backgroundColor.rgb() ) < 80 &&
        qBlue( backgroundColor.rgb() ) < 80 )
    {
        penColor = Qt::blue;
    }

    QPointArray point( 3 );
    point.setPoint( 0, doc->zoomItX( cellRect.right() - 4.0 ),
                       doc->zoomItY( cellRect.y() + cellRect.height() / 2.0 - 4.0 ) );
    point.setPoint( 1, doc->zoomItX( cellRect.right() ),
                       doc->zoomItY( cellRect.y() + cellRect.height() / 2.0 ) );
    point.setPoint( 2, doc->zoomItX( cellRect.right() - 4.0 ),
                       doc->zoomItY( cellRect.y() + cellRect.height() / 2.0 + 4.0 ) );
    painter.setBrush( QBrush( penColor ) );
    painter.setPen( Qt::NoPen );
    painter.drawPolygon( point );
  }
}

void KSpreadCell::paintText( QPainter& painter,
                             const KoRect &cellRect,
                             const QPoint &cellRef )
{
  KSpreadDoc* doc = table()->doc();

  ColumnFormat* colFormat = m_pTable->columnFormat( cellRef.x() );

  QColorGroup defaultColorGroup = QApplication::palette().active();
  QColor textColorPrint = effTextColor( cellRef.x(), cellRef.y() );

  // Resolve the text color if invalid (=default)
  if ( !textColorPrint.isValid() )
  {
    if ( painter.device()->isExtDev() )
      textColorPrint = Qt::black;
    else
      textColorPrint = QApplication::palette().active().text();
  }

  QPen tmpPen( textColorPrint );

  //Set the font according to condition
  applyZoomedFont( painter, cellRef.x(), cellRef.y() );

  //Check for red font color for negative values
  if ( !m_conditions || !m_conditions->matchedStyle() )
  {
    if ( m_value.isNumber()
         && !( m_pTable->getShowFormula()
               && !( m_pTable->isProtected() && isHideFormula( m_iColumn, m_iRow ) ) ) )
    {
      double v = m_value.asFloat() * factor( column(),row() );
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
  painter.setPen( tmpPen );

  QString tmpText = m_strOutText;
  double tmpHeight = m_dOutTextHeight;
  double tmpWidth = m_dOutTextWidth;
  if( testFlag( Flag_CellTooShortX ) )
  {
    m_strOutText = textDisplaying( painter );
  }

  //hide zero
  if ( m_pTable->getHideZero() && m_value.isNumber() &&
       m_value.asFloat() * factor( column(), row() ) == 0 )
  {
    m_strOutText = QString::null;
  }

  if ( colFormat->isHide() || ( cellRect.height() <= 2 ) )
  {
    //clear extra cell if column or row is hidden
    freeAllObscuredCells();  /* TODO: This looks dangerous...must check when I
                                have time */
    m_strOutText = "";
  }

  double indent = 0.0;
  double offsetCellTooShort = 0.0;
  int a = effAlignX();
  //apply indent if text is align to left not when text is at right or middle
  if (  a == KSpreadCell::Left && !isEmpty() )
  {
    if ( m_conditions && m_conditions->matchedStyle()
         && m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SIndent, true ) )
      indent = m_conditions->matchedStyle()->indent();
    else
      indent = getIndent( column(), row() );
  }

  //made an offset, otherwise ### is under red triangle
  if ( a == KSpreadCell::Right && !isEmpty() && testFlag( Flag_CellTooShortX ) )
  {
    offsetCellTooShort = m_pTable->doc()->unzoomItX( 4 );
  }

  QFontMetrics fm2 = painter.fontMetrics();
  double offsetFont = 0.0;

  if ( ( alignY( column(), row() ) == KSpreadCell::Bottom )
       && textFontUnderline( column(), row() ) )
  {
    offsetFont = m_pTable->doc()->unzoomItX( fm2.underlinePos() + 1 );
  }

  int  tmpAngle;
  bool tmpMultiRow;
  bool tmpVerticalText;

  if ( m_conditions && m_conditions->matchedStyle() )
  {
    if ( m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SAngle, true ) )
      tmpAngle = m_conditions->matchedStyle()->rotateAngle();
    else
      tmpAngle = getAngle( cellRef.x(), cellRef.y() );

    if ( m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SVerticalText, true ) )
      tmpVerticalText = m_conditions->matchedStyle()->hasProperty( KSpreadStyle::PVerticalText );
    else
      tmpVerticalText = verticalText( cellRef.x(), cellRef.y() );

    if ( m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SMultiRow, true ) )
      tmpMultiRow = m_conditions->matchedStyle()->hasProperty( KSpreadStyle::PMultiRow );
    else
      tmpMultiRow = multiRow( cellRef.x(), cellRef.y() );
  }
  else
  {
    tmpAngle        = getAngle( cellRef.x(), cellRef.y() );
    tmpVerticalText = verticalText( cellRef.x(), cellRef.y() );
    tmpMultiRow     = multiRow( cellRef.x(), cellRef.y() );
  }

  if ( !tmpMultiRow && !tmpVerticalText && !tmpAngle )
  {
    if( !m_pQML )
       painter.drawText( doc->zoomItX( indent + cellRect.x() + m_dTextX - offsetCellTooShort ),
                      doc->zoomItY( cellRect.y() + m_dTextY - offsetFont ), m_strOutText );
    else
        m_pQML->draw( &painter,
                    doc->zoomItX( indent + cellRect.x() + m_dTextX ),
                    doc->zoomItY( cellRect.y() + m_dTextY - offsetFont ),
                    QRegion( doc->zoomRect( KoRect( cellRect.x(),     cellRect.y(),
                                                    cellRect.width(), cellRect.height() ) ) ),
                    QApplication::palette().active(), 0 );
  }
  else if ( tmpAngle != 0 )
  {
    int angle = tmpAngle;
    QFontMetrics fm = painter.fontMetrics();

    painter.rotate( angle );
    double x;
    if ( angle > 0 )
      x = indent + cellRect.x() + m_dTextX;
    else
      x = indent + cellRect.x() + m_dTextX
        - ( fm.descent() + fm.ascent() ) * sin( angle * M_PI / 180 );
    double y;
    if ( angle > 0 )
      y = cellRect.y() + m_dTextY;
    else
      y = cellRect.y() + m_dTextY + m_dOutTextHeight;
    painter.drawText( doc->zoomItX( x * cos( angle * M_PI / 180 ) +
                                    y * sin( angle * M_PI / 180 ) ),
                      doc->zoomItY( -x * sin( angle * M_PI / 180 ) +
                                     y * cos( angle * M_PI / 180 ) ),
                      m_strOutText );
    painter.rotate( -angle );
  }
  else if ( tmpMultiRow && !tmpVerticalText )
  {
    QString t;
    int i;
    int pos = 0;
    double dy = 0.0;
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

      int a = effAlignX();
      if ( m_pTable->getShowFormula() && !( m_pTable->isProtected() && isHideFormula( m_iColumn, m_iRow ) ) )
        a = KSpreadCell::Left;

      // #### Torben: This looks duplicated for me
      switch( a )
      {
       case KSpreadCell::Left:
        m_dTextX = effLeftBorderPen( cellRef.x(), cellRef.y() ).width() + BORDER_SPACE;
        break;

       case KSpreadCell::Right:
        m_dTextX = cellRect.width() - BORDER_SPACE - doc->unzoomItX( fm.width( t ) )
          - effRightBorderPen( cellRef.x(), cellRef.y() ).width();
        break;

       case KSpreadCell::Center:
        m_dTextX = ( cellRect.width() - doc->unzoomItX( fm.width( t ) ) ) / 2;
      }

      painter.drawText( doc->zoomItX( indent + cellRect.x() + m_dTextX ),
                        doc->zoomItY( cellRect.y() + m_dTextY + dy ), t );
      dy += doc->unzoomItY( fm.descent() + fm.ascent() );
    }
    while ( i != -1 );
  }
  else if ( tmpVerticalText && !m_strOutText.isEmpty() )
  {
    QString t;
    int i = 0;
    int len = 0;
    double dy = 0.0;
    QFontMetrics fm = painter.fontMetrics();
    do
    {
      len = m_strOutText.length();
      t = m_strOutText.at( i );
      painter.drawText( doc->zoomItX( indent + cellRect.x() + m_dTextX ),
                        doc->zoomItY( cellRect.y() + m_dTextY + dy ), t );
      dy += doc->unzoomItY( fm.descent() + fm.ascent() );
      i++;
    }
    while ( i != len );
  }

  if ( testFlag( Flag_CellTooShortX ) )
  {
    m_strOutText = tmpText;
    m_dOutTextHeight = tmpHeight;
    m_dOutTextWidth = tmpWidth;
  }

  if ( m_pTable->getHideZero() && m_value.isNumber()
       && m_value.asFloat() * factor( column(), row() ) == 0 )
  {
    m_strOutText = tmpText;
  }

  if ( colFormat->isHide() || ( cellRect.height() <= 2 ) )
    m_strOutText = tmpText;
}

void KSpreadCell::paintPageBorders( QPainter& painter,
                                    const KoRect &cellRect,
                                    const QPoint &cellRef,
                                    bool paintBorderRight,
                                    bool paintBorderBottom )
{
  if ( painter.device()->isExtDev() )
    return;

  KSpreadSheetPrint* print = m_pTable->print();

  // Draw page borders
  if( m_pTable->isShowPageBorders() )
  {
    if( cellRef.x() >= print->printRange().left() &&
        cellRef.x() <= print->printRange().right() + 1 &&
        cellRef.y() >= print->printRange().top() &&
        cellRef.y() <= print->printRange().bottom() + 1 )
    {
      KSpreadDoc* doc = table()->doc();
      if ( print->isOnNewPageX( cellRef.x() ) &&
           ( cellRef.y() <= print->printRange().bottom() ) )
      {
        painter.setPen( table()->doc()->pageBorderColor() );
        painter.drawLine( doc->zoomItX( cellRect.x() ), doc->zoomItY( cellRect.y() ),
                          doc->zoomItX( cellRect.x() ), doc->zoomItY( cellRect.bottom() ) );
      }

      if ( print->isOnNewPageY( cellRef.y() ) &&
           ( cellRef.x() <= print->printRange().right() ) )
      {
        painter.setPen( table()->doc()->pageBorderColor() );
        painter.drawLine( doc->zoomItX( cellRect.x() ),     doc->zoomItY( cellRect.y() ),
                          doc->zoomItX( cellRect.right() ), doc->zoomItY( cellRect.y() ) );
      }

      if( paintBorderRight )
      {
        if ( print->isOnNewPageX( cellRef.x() + 1 ) &&
             ( cellRef.y() <= print->printRange().bottom() ) )
        {
          painter.setPen( table()->doc()->pageBorderColor() );
          painter.drawLine( doc->zoomItX( cellRect.right() ), doc->zoomItY( cellRect.y() ),
                            doc->zoomItX( cellRect.right() ), doc->zoomItY( cellRect.bottom() ) );
        }
      }

      if( paintBorderBottom )
      {
        if ( print->isOnNewPageY( cellRef.y() + 1 ) &&
             ( cellRef.x() <= print->printRange().right() ) )
        {
          painter.setPen( table()->doc()->pageBorderColor() );
          painter.drawLine( doc->zoomItX( cellRect.x() ),     doc->zoomItY( cellRect.bottom() ),
                            doc->zoomItX( cellRect.right() ), doc->zoomItY( cellRect.bottom() ) );
        }
      }
    }
  }
}


void KSpreadCell::paintCellBorders( QPainter& painter, const KoRect& rect,
                                    const KoRect &cellRect,
                                    const QPoint &cellRef,
                                    bool paintRight,
                                    bool paintBottom,
                                    bool paintLeft, bool paintTop,
                                    QPen & rightPen, QPen & bottomPen,
                                    QPen & leftPen, QPen & topPen )
{
  KSpreadDoc * doc = table()->doc();

  /* we might not paint some borders if this cell is merged with another in
     that direction
  bool paintLeft   = paintBorderLeft;
  bool paintRight  = paintBorderRight;
  bool paintTop    = paintBorderTop;
  bool paintBottom = paintBorderBottom;
  */

  // paintRight  = paintRight  && ( extraXCells() == 0 );
  // paintBottom = paintBottom && ( extraYCells() == 0 );

  QValueList<KSpreadCell*>::const_iterator it  = m_ObscuringCells.begin();
  QValueList<KSpreadCell*>::const_iterator end = m_ObscuringCells.end();
  for ( ; it != end; ++it )
  {
    KSpreadCell* cell = *it;
    int xDiff = cellRef.x() - cell->column();
    int yDiff = cellRef.y() - cell->row();
    paintLeft = paintLeft && xDiff == 0;
    paintTop  = paintTop  && yDiff == 0;

    paintRight  = paintRight  && cell->extraXCells() == xDiff;
    paintBottom = paintBottom && cell->extraYCells() == yDiff;
  }

  //
  // Determine the pens that should be used for drawing
  // the borders.
  //
  int left_penWidth   = QMAX( 1, doc->zoomItX( leftPen.width() ) );
  int right_penWidth  = QMAX( 1, doc->zoomItX( rightPen.width() ) );
  int top_penWidth    = QMAX( 1, doc->zoomItY( topPen.width() ) );
  int bottom_penWidth = QMAX( 1, doc->zoomItY( bottomPen.width() ) );

  leftPen.setWidth( left_penWidth );
  rightPen.setWidth( right_penWidth );
  topPen.setWidth( top_penWidth );
  bottomPen.setWidth( bottom_penWidth );

  if ( paintLeft && leftPen.style() != Qt::NoPen )
  {
    int top = ( QMAX( 0, -1 + top_penWidth ) ) / 2 +
              ( ( QMAX( 0, -1 + top_penWidth ) ) % 2 );
    int bottom = ( QMAX( 0, -1 + bottom_penWidth ) ) / 2 + 1;

    painter.setPen( leftPen );
    //If we are on paper printout, we limit the length of the lines
    //On paper, we always have full cells, on screen not
    if ( painter.device()->isExtDev() )
    {
      painter.drawLine( QMAX( doc->zoomItX( rect.left() ), doc->zoomItX( cellRect.x() ) ),
                        QMAX( doc->zoomItY( rect.top() ), doc->zoomItY( cellRect.y() ) - top ),
                        QMIN( doc->zoomItX( rect.right() ), doc->zoomItX( cellRect.x() ) ),
                        QMIN( doc->zoomItY( rect.bottom() ), doc->zoomItY( cellRect.bottom() ) + bottom ) );
    }
    else
    {
      painter.drawLine( doc->zoomItX( cellRect.x() ),
                        doc->zoomItY( cellRect.y() ) - top,
                        doc->zoomItX( cellRect.x() ),
                        doc->zoomItY( cellRect.bottom() ) + bottom );
    }
  }

  if ( paintRight && rightPen.style() != Qt::NoPen )
  {
    int top = ( QMAX( 0, -1 + top_penWidth ) ) / 2 +
              ( ( QMAX( 0, -1 + top_penWidth ) ) % 2 );
    int bottom = ( QMAX( 0, -1 + bottom_penWidth ) ) / 2 + 1;

    painter.setPen( rightPen );
    //If we are on paper printout, we limit the length of the lines
    //On paper, we always have full cells, on screen not
    if ( painter.device()->isExtDev() )
    {
      painter.drawLine( QMAX( doc->zoomItX( rect.left() ), doc->zoomItX( cellRect.right() ) ),
                        QMAX( doc->zoomItY( rect.top() ), doc->zoomItY( cellRect.y() ) - top ),
                        QMIN( doc->zoomItX( rect.right() ), doc->zoomItX( cellRect.right() ) ),
                        QMIN( doc->zoomItY( rect.bottom() ), doc->zoomItY( cellRect.bottom() ) + bottom ) );
    }
    else
    {
      double r = cellRect.right();

      painter.drawLine( doc->zoomItX( r ),
                        doc->zoomItY( cellRect.y() ) - top,
                        doc->zoomItX( r ),
                        doc->zoomItY( cellRect.bottom() ) + bottom );
    }
  }

  if ( paintTop && topPen.style() != Qt::NoPen )
  {
    painter.setPen( topPen );
    //If we are on paper printout, we limit the length of the lines
    //On paper, we always have full cells, on screen not
    if ( painter.device()->isExtDev() )
    {
      painter.drawLine( doc->zoomItX( QMAX( rect.left(),   cellRect.x() ) ),
                        doc->zoomItY( QMAX( rect.top(),    cellRect.y() ) ),
                        doc->zoomItX( QMIN( rect.right(),  cellRect.right() ) ),
                        doc->zoomItY( QMIN( rect.bottom(), cellRect.y() ) ) );
    }
    else
    {
      painter.drawLine( doc->zoomItX( cellRect.x() ),
                        doc->zoomItY( cellRect.y() ),
                        doc->zoomItX( cellRect.right() ),
                        doc->zoomItY( cellRect.y() ) );
    }
  }

  if ( paintBottom && bottomPen.style() != Qt::NoPen )
  {
    painter.setPen( bottomPen );
    //If we are on paper printout, we limit the length of the lines
    //On paper, we always have full cells, on screen not
    if ( painter.device()->isExtDev() )
    {
      painter.drawLine( doc->zoomItX( QMAX( rect.left(),   cellRect.x() ) ),
                        doc->zoomItY( QMAX( rect.top(),    cellRect.bottom() ) ),
                        doc->zoomItX( QMIN( rect.right(),  cellRect.right() ) ),
                        doc->zoomItY( QMIN( rect.bottom(), cellRect.bottom() ) ) );
    }
    else
    {
      painter.drawLine( doc->zoomItX( cellRect.x() ),
                        doc->zoomItY( cellRect.bottom() ),
                        doc->zoomItX( cellRect.right() ),
                        doc->zoomItY( cellRect.bottom() ) );
    }
  }

  //
  // Look at the cells on our corners. It may happen that we
  // just erased parts of their borders corner, so we might need
  // to repaint these corners.
  //
  QPen vert_pen, horz_pen;
  int vert_penWidth, horz_penWidth;

  // Fix the borders which meet at the top left corner
  if ( m_pTable->cellAt( cellRef.x(), cellRef.y() - 1 )->effLeftBorderValue( cellRef.x(), cellRef.y() - 1 )
       >= m_pTable->cellAt( cellRef.x() - 1, cellRef.y() - 1 )->effRightBorderValue( cellRef.x() - 1, cellRef.y() - 1 ) )
    vert_pen = m_pTable->cellAt( cellRef.x(), cellRef.y() - 1 )->effLeftBorderPen( cellRef.x(), cellRef.y() - 1 );
  else
    vert_pen = m_pTable->cellAt( cellRef.x() - 1, cellRef.y() - 1 )->effRightBorderPen( cellRef.x() - 1, cellRef.y() - 1 );

  vert_penWidth = QMAX( 1, doc->zoomItX( vert_pen.width() ) );
  vert_pen.setWidth( vert_penWidth );

  if ( vert_pen.style() != Qt::NoPen )
  {
    if ( m_pTable->cellAt( cellRef.x() - 1, cellRef.y() )->effTopBorderValue( cellRef.x() - 1, cellRef.y() )
         >= m_pTable->cellAt( cellRef.x() - 1, cellRef.y() - 1 )->effBottomBorderValue( cellRef.x() - 1, cellRef.y() - 1 ) )
      horz_pen = m_pTable->cellAt( cellRef.x() - 1, cellRef.y() )->effTopBorderPen( cellRef.x() - 1, cellRef.y() );
    else
      horz_pen = m_pTable->cellAt( cellRef.x() - 1, cellRef.y() - 1 )->effBottomBorderPen( cellRef.x() - 1, cellRef.y() - 1 );

    horz_penWidth = QMAX( 1, doc->zoomItY( horz_pen.width() ) );
    int bottom = ( QMAX( 0, -1 + horz_penWidth ) ) / 2 + 1;

    painter.setPen( vert_pen );
    //If we are on paper printout, we limit the length of the lines
    //On paper, we always have full cells, on screen not
    if ( painter.device()->isExtDev() )
    {
      painter.drawLine( QMAX( doc->zoomItX( rect.left() ), doc->zoomItX( cellRect.x() ) ),
                        QMAX( doc->zoomItY( rect.top() ), doc->zoomItY( cellRect.y() ) ),
                        QMIN( doc->zoomItX( rect.right() ), doc->zoomItX( cellRect.x() ) ),
                        QMIN( doc->zoomItY( rect.bottom() ), doc->zoomItY( cellRect.y() ) + bottom ) );
    }
    else
    {
      painter.drawLine( doc->zoomItX( cellRect.x() ),
                        doc->zoomItY( cellRect.y() ),
                        doc->zoomItX( cellRect.x() ),
                        doc->zoomItY( cellRect.y() ) + bottom );
    }
  }

  // Fix the borders which meet at the top right corner
  if ( m_pTable->cellAt( cellRef.x(), cellRef.y() - 1 )->effRightBorderValue( cellRef.x(), cellRef.y() - 1 )
       >= m_pTable->cellAt( cellRef.x() + 1, cellRef.y() - 1 )->effLeftBorderValue( cellRef.x() + 1, cellRef.y() - 1 ) )
    vert_pen = m_pTable->cellAt( cellRef.x(), cellRef.y() - 1 )->effRightBorderPen( cellRef.x(), cellRef.y() - 1 );
  else
    vert_pen = m_pTable->cellAt( cellRef.x() + 1, cellRef.y() - 1 )->effLeftBorderPen( cellRef.x() + 1, cellRef.y() - 1 );

  // vert_pen = effRightBorderPen( cellRef.x(), cellRef.y() - 1 );
  vert_penWidth = QMAX( 1, doc->zoomItX( vert_pen.width() ) );
  vert_pen.setWidth( vert_penWidth );
  if ( ( vert_pen.style() != Qt::NoPen ) && ( cellRef.x() < KS_colMax ) )
  {
    if ( m_pTable->cellAt( cellRef.x() + 1, cellRef.y() )->effTopBorderValue( cellRef.x() + 1, cellRef.y() )
         >= m_pTable->cellAt( cellRef.x() + 1, cellRef.y() - 1 )->effBottomBorderValue( cellRef.x() + 1, cellRef.y() - 1 ) )
      horz_pen = m_pTable->cellAt( cellRef.x() + 1, cellRef.y() )->effTopBorderPen( cellRef.x() + 1, cellRef.y() );
    else
      horz_pen = m_pTable->cellAt( cellRef.x() + 1, cellRef.y() - 1 )->effBottomBorderPen( cellRef.x() + 1, cellRef.y() - 1 );

    // horz_pen = effTopBorderPen( cellRef.x() + 1, cellRef.y() );
    horz_penWidth = QMAX( 1, doc->zoomItY( horz_pen.width() ) );
    int bottom = ( QMAX( 0, -1 + horz_penWidth ) ) / 2 + 1;

    painter.setPen( vert_pen );
    //If we are on paper printout, we limit the length of the lines
    //On paper, we always have full cells, on screen not
    if ( painter.device()->isExtDev() )
    {
      painter.drawLine( QMAX( doc->zoomItX( rect.left() ), doc->zoomItX( cellRect.right() ) ),
                        QMAX( doc->zoomItY( rect.top() ), doc->zoomItY( cellRect.y() ) ),
                        QMIN( doc->zoomItX( rect.right() ), doc->zoomItX( cellRect.right() ) ),
                        QMIN( doc->zoomItY( rect.bottom() ), doc->zoomItY( cellRect.y() ) + bottom ) );
    }
    else
    {
      painter.drawLine( doc->zoomItX( cellRect.right() ),
                        doc->zoomItY( cellRect.y() ),
                        doc->zoomItX( cellRect.right() ),
                        doc->zoomItY( cellRect.y() ) + bottom );
    }
  }

  // Bottom
  if ( cellRef.y() < KS_rowMax )
  {
    // Fix the borders which meet at the bottom left corner
    if ( m_pTable->cellAt( cellRef.x(), cellRef.y() + 1 )->effLeftBorderValue( cellRef.x(), cellRef.y() + 1 )
         >= m_pTable->cellAt( cellRef.x() - 1, cellRef.y() + 1 )->effRightBorderValue( cellRef.x() - 1, cellRef.y() + 1 ) )
      vert_pen = m_pTable->cellAt( cellRef.x(), cellRef.y() + 1 )->effLeftBorderPen( cellRef.x(), cellRef.y() + 1 );
    else
      vert_pen = m_pTable->cellAt( cellRef.x() - 1, cellRef.y() + 1 )->effRightBorderPen( cellRef.x() - 1, cellRef.y() + 1 );

    // vert_pen = effLeftBorderPen( cellRef.x(), cellRef.y() + 1 );
    vert_penWidth = QMAX( 1, doc->zoomItY( vert_pen.width() ) );
    vert_pen.setWidth( vert_penWidth );
    if ( vert_pen.style() != Qt::NoPen )
    {
      if ( m_pTable->cellAt( cellRef.x() - 1, cellRef.y() )->effBottomBorderValue( cellRef.x() - 1, cellRef.y() )
           >= m_pTable->cellAt( cellRef.x() - 1, cellRef.y() + 1 )->effTopBorderValue( cellRef.x() - 1, cellRef.y() + 1 ) )
        horz_pen = m_pTable->cellAt( cellRef.x() - 1, cellRef.y() )->effBottomBorderPen( cellRef.x() - 1, cellRef.y() );
      else
        horz_pen = m_pTable->cellAt( cellRef.x() - 1, cellRef.y() + 1 )->effTopBorderPen( cellRef.x() - 1, cellRef.y() + 1 );

      // horz_pen = effBottomBorderPen( cellRef.x() - 1, cellRef.y() );
      horz_penWidth = QMAX( 1, doc->zoomItX( horz_pen.width() ) );
      int bottom = ( QMAX( 0, -1 + horz_penWidth ) ) / 2;

      painter.setPen( vert_pen );
      //If we are on paper printout, we limit the length of the lines
      //On paper, we always have full cells, on screen not
      if ( painter.device()->isExtDev() )
      {
        painter.drawLine( QMAX( doc->zoomItX( rect.left() ), doc->zoomItX( cellRect.x() ) ),
                          QMAX( doc->zoomItY( rect.top() ), doc->zoomItY( cellRect.bottom() ) - bottom ),
                          QMIN( doc->zoomItX( rect.right() ), doc->zoomItX( cellRect.x() ) ),
                          QMIN( doc->zoomItY( rect.bottom() ), doc->zoomItY( cellRect.bottom() ) ) );
      }
      else
      {
        painter.drawLine( doc->zoomItX( cellRect.x() ),
                          doc->zoomItY( cellRect.bottom() ) - bottom,
                          doc->zoomItX( cellRect.x() ),
                          doc->zoomItY( cellRect.bottom() ) );
      }
    }

    // Fix the borders which meet at the bottom right corner
    if ( m_pTable->cellAt( cellRef.x(), cellRef.y() + 1 )->effRightBorderValue( cellRef.x(), cellRef.y() + 1 )
         >= m_pTable->cellAt( cellRef.x() + 1, cellRef.y() + 1 )->effLeftBorderValue( cellRef.x() + 1, cellRef.y() + 1 ) )
      vert_pen = m_pTable->cellAt( cellRef.x(), cellRef.y() + 1 )->effRightBorderPen( cellRef.x(), cellRef.y() + 1 );
    else
      vert_pen = m_pTable->cellAt( cellRef.x() + 1, cellRef.y() + 1 )->effLeftBorderPen( cellRef.x() + 1, cellRef.y() + 1 );

    // vert_pen = effRightBorderPen( cellRef.x(), cellRef.y() + 1 );
    vert_penWidth = QMAX( 1, doc->zoomItY( vert_pen.width() ) );
    vert_pen.setWidth( vert_penWidth );
    if ( ( vert_pen.style() != Qt::NoPen ) && ( cellRef.x() < KS_colMax ) )
    {
      if ( m_pTable->cellAt( cellRef.x() + 1, cellRef.y() )->effBottomBorderValue( cellRef.x() + 1, cellRef.y() )
           >= m_pTable->cellAt( cellRef.x() + 1, cellRef.y() + 1 )->effTopBorderValue( cellRef.x() + 1, cellRef.y() + 1 ) )
        horz_pen = m_pTable->cellAt( cellRef.x() + 1, cellRef.y() )->effBottomBorderPen( cellRef.x() + 1, cellRef.y() );
      else
        horz_pen = m_pTable->cellAt( cellRef.x() + 1, cellRef.y() + 1 )->effTopBorderPen( cellRef.x() + 1, cellRef.y() + 1 );

      // horz_pen = effBottomBorderPen( cellRef.x() + 1, cellRef.y() );
      horz_penWidth = QMAX( 1, doc->zoomItX( horz_pen.width() ) );
      int bottom = ( QMAX( 0, -1 + horz_penWidth ) ) / 2;

      painter.setPen( vert_pen );
      //If we are on paper printout, we limit the length of the lines
      //On paper, we always have full cells, on screen not
      if ( painter.device()->isExtDev() )
      {
        painter.drawLine( QMAX( doc->zoomItX( rect.left() ), doc->zoomItX( cellRect.right() ) ),
                          QMAX( doc->zoomItY( rect.top() ), doc->zoomItY( cellRect.bottom() ) - bottom ),
                          QMIN( doc->zoomItX( rect.right() ), doc->zoomItX( cellRect.right() ) ),
                          QMIN( doc->zoomItY( rect.bottom() ),doc->zoomItY( cellRect.bottom() ) ) );
      }
      else
      {
        painter.drawLine( doc->zoomItX( cellRect.right() ),
                          doc->zoomItY( cellRect.bottom() ) - bottom,
                          doc->zoomItX( cellRect.right() ),
                          doc->zoomItY( cellRect.bottom() ) );
      }
    }
  }
}

void KSpreadCell::paintCellDiagonalLines( QPainter& painter,
                                          const KoRect &cellRect,
                                          const QPoint &cellRef )
{
  if ( !isObscuringForced() )
  {
    if ( effFallDiagonalPen( cellRef.x(), cellRef.y() ).style() != Qt::NoPen )
    {
      KSpreadDoc* doc = table()->doc();
      painter.setPen( effFallDiagonalPen( cellRef.x(), cellRef.y() ) );
      painter.drawLine( doc->zoomItX( cellRect.x() ),
                        doc->zoomItY( cellRect.y() ),
                        doc->zoomItX( cellRect.right() ),
                        doc->zoomItY( cellRect.bottom() ) );
    }
    if ( effGoUpDiagonalPen( cellRef.x(), cellRef.y() ).style() != Qt::NoPen )
    {
      KSpreadDoc* doc = table()->doc();
      painter.setPen( effGoUpDiagonalPen( cellRef.x(), cellRef.y() ) );
      painter.drawLine( doc->zoomItX( cellRect.x() ),
                        doc->zoomItY( cellRect.bottom() ),
                        doc->zoomItX( cellRect.right() ),
                        doc->zoomItY( cellRect.y() ) );
    }
  }
}


int KSpreadCell::defineAlignX()
{
  int a = align( column(), row() );
  if ( a == KSpreadCell::Undefined )
  {
    if ( m_value.isBoolean() || m_value.isNumber() || (m_value.isString() && m_value.asString()[0].direction() == QChar::DirR ))
      a = KSpreadCell::Right;
    else
      a = KSpreadCell::Left;
  }
  return a;
}

int KSpreadCell::effAlignX()
{
  if ( m_conditions && m_conditions->matchedStyle()
       && m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SAlignX, true ) )
    return m_conditions->matchedStyle()->alignX();

  return defineAlignX();
}

QString KSpreadCell::textDisplaying( QPainter &_painter )
{
  QFontMetrics fm = _painter.fontMetrics();
  int a = align( column(), row() );
  if (( a == KSpreadCell::Left || a == KSpreadCell::Undefined) && !m_value.isNumber()
    && !verticalText( column(),row() ))
  {
    //not enough space but align to left
    double len = 0.0;
    for ( int i = column(); i <= column() + m_iExtraXCells; i++ )
    {
      ColumnFormat *cl2 = m_pTable->columnFormat( i );
      len += cl2->dblWidth() - 1.0; //-1.0 because the pixel in between 2 cells is shared between both cells
    }

    QString tmp;
    double tmpIndent = 0.0;
    if( !isEmpty() )
      tmpIndent = getIndent( column(), row() );
    for( int i = m_strOutText.length(); i != 0; i-- )
    {
      tmp = m_strOutText.left(i);

        if( m_pTable->doc()->unzoomItX( fm.width( tmp ) ) + tmpIndent < len - 4.0 - 1.0 ) //4 equal lenght of red triangle +1 point
        {
            if( getAngle( column(), row() ) != 0 )
            {
                QString tmp2;
                RowFormat *rl = m_pTable->rowFormat( row() );
                if( m_dOutTextHeight > rl->dblHeight() )
                {
                    for ( int j = m_strOutText.length(); j != 0; j-- )
                    {
                        tmp2 = m_strOutText.left( j );
                        if( m_pTable->doc()->unzoomItY( fm.width( tmp2 ) ) < rl->dblHeight() - 1.0 )
                        {
                            return m_strOutText.left( QMIN( tmp.length(), tmp2.length() ) );
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
    return QString( "" );
  }
  else if( verticalText( column(), row() ) )
  {
    RowFormat *rl = m_pTable->rowFormat( row() );
    double tmpIndent = 0.0;
    //not enough space but align to left
    double len = 0.0;
    for( int i = column(); i <= column() + m_iExtraXCells; i++ )
    {
        ColumnFormat *cl2 = m_pTable->columnFormat( i );
        len += cl2->dblWidth() - 1.0; //-1.0 because the pixel in between 2 cells is shared between both cells
    }
    if( !isEmpty() )
        tmpIndent = getIndent( column(), row() );
    if( ( m_dOutTextWidth + tmpIndent > len ) || m_dOutTextWidth == 0.0 )
        return QString( "" );

    for ( int i = m_strOutText.length(); i != 0; i-- )
    {
        if( m_pTable->doc()->unzoomItY( fm.ascent() + fm.descent() ) * i < rl->dblHeight() - 1.0 )
        {
            return m_strOutText.left( i );
        }
    }
    return QString( "" );
 }

 ColumnFormat *cl = m_pTable->columnFormat( column() );
 double w = ( m_dExtraWidth == 0.0 ) ? cl->dblWidth() : m_dExtraWidth;

 if( m_value.isNumber())
 {
   if( formatType() != Scientific )
   {
     int p = (precision(column(),row())  == -1) ? 8 :
       precision(column(),row());
     double value =m_value.asFloat() * factor(column(),row());
     int pos=0;
     QString localizedNumber= QString::number( (value), 'E', p);
     if((pos=localizedNumber.find('.'))!=-1)
     {
       localizedNumber = localizedNumber.replace( pos, 1, decimal_point );
     }
     if( floatFormat( column(), row() ) ==
            KSpreadCell::AlwaysSigned && value >= 0 )
     {
       if( locale()->positiveSign().isEmpty() )
       {
         localizedNumber = '+' + localizedNumber;
       }
     }
     if ( precision( column(), row() ) == -1 &&
          localizedNumber.find( decimal_point ) >= 0 )
     {
       //duplicate code it's not good I know I will fix it
       int start = 0;
       if( ( start = localizedNumber.find('E') ) != -1 )
       {
         start = localizedNumber.length() - start;
       }
       int i = localizedNumber.length() - start;
       bool bFinished = FALSE;

       while ( !bFinished && i > 0 )
       {
         QChar ch = localizedNumber[ i - 1 ];
         if ( ch == '0' )
         {
           localizedNumber.remove( --i, 1 );
         }
         else
         {
           bFinished = TRUE;
           if ( ch == decimal_point )
           {
             localizedNumber.remove( --i, 1 );
           }
         }
       }
     }
     if ( m_pTable->doc()->unzoomItX( fm.width( localizedNumber ) ) < w
          && !( m_pTable->getShowFormula() && !( m_pTable->isProtected() && isHideFormula( m_iColumn, m_iRow ) ) ) )
     {
       return localizedNumber;
     }
   }
   /* What is this doing and is it broken with the new error handling? */
   QString str( "####" );
   int i;
   for( i=4; i != 0; i-- )
   {
     if( m_pTable->doc()->unzoomItX( fm.width( str.right( i ) ) ) < w - 4.0 - 1.0 )
     {
       break;
     }
   }
   return str.right( i );//QString("###");
 }
 else
 {
   QString tmp;
   for ( int i = m_strOutText.length(); i != 0; i-- )
   {
     tmp = m_strOutText.left( i );
     if( m_pTable->doc()->unzoomItX( fm.width( tmp ) ) < w - 4.0 - 1.0 ) //4 equals lenght of red triangle +1 pixel
     {
       return tmp;
     }
   }
 }
 return  QString::null;
}


double KSpreadCell::dblWidth( int _col, const KSpreadCanvas *_canvas ) const
{
  if ( _col < 0 )
    _col = m_iColumn;

  if ( _canvas )
  {
    if ( testFlag(Flag_ForceExtra) )
      return m_dExtraWidth;

    const ColumnFormat *cl = m_pTable->columnFormat( _col );
    return cl->dblWidth( _canvas );
  }

  if ( testFlag(Flag_ForceExtra) )
    return m_dExtraWidth;

  const ColumnFormat *cl = m_pTable->columnFormat( _col );
  return cl->dblWidth();
}

int KSpreadCell::width( int _col, const KSpreadCanvas *_canvas ) const
{
  return int( dblWidth( _col, _canvas ) );
}

double KSpreadCell::dblHeight( int _row, const KSpreadCanvas *_canvas ) const
{
  if ( _row < 0 )
    _row = m_iRow;

  if ( _canvas )
  {
    if ( testFlag(Flag_ForceExtra) )
      return m_dExtraHeight;

    const RowFormat *rl = m_pTable->rowFormat( _row );
    return rl->dblHeight( _canvas );
  }

  if ( testFlag(Flag_ForceExtra) )
    return m_dExtraHeight;

  const RowFormat *rl = m_pTable->rowFormat( _row );
  return rl->dblHeight();
}

int KSpreadCell::height( int _row, const KSpreadCanvas *_canvas ) const
{
  return int( dblHeight( _row, _canvas ) );
}

///////////////////////////////////////////
//
// Misc Properties.
// Reimplementation of KSpreadFormat methods.
//
///////////////////////////////////////////

const QBrush& KSpreadCell::backGroundBrush( int _col, int _row ) const
{
  if ( !m_ObscuringCells.isEmpty() )
  {
    const KSpreadCell* cell = m_ObscuringCells.first();
    return cell->backGroundBrush( cell->column(), cell->row() );
  }

  return KSpreadFormat::backGroundBrush( _col, _row );
}

const QColor& KSpreadCell::bgColor( int _col, int _row ) const
{
  if ( !m_ObscuringCells.isEmpty() )
  {
    const KSpreadCell* cell = m_ObscuringCells.first();
    return cell->bgColor( cell->column(), cell->row() );
  }

  return KSpreadFormat::bgColor( _col, _row );
}

///////////////////////////////////////////
//
// Borders.
// Reimplementation of KSpreadFormat methods.
//
///////////////////////////////////////////

void KSpreadCell::setLeftBorderPen( const QPen& p )
{
  if ( column() == 1 )
  {
    KSpreadCell* cell = m_pTable->cellAt( column() - 1, row() );
    if ( cell && cell->hasProperty( PRightBorder )
         && m_pTable->cellAt( column(), row() ) == this )
        cell->clearProperty( PRightBorder );
  }

  KSpreadFormat::setLeftBorderPen( p );
}

void KSpreadCell::setTopBorderPen( const QPen& p )
{
  if ( row() == 1 )
  {
    KSpreadCell* cell = m_pTable->cellAt( column(), row() - 1 );
    if ( cell && cell->hasProperty( PBottomBorder )
         && m_pTable->cellAt( column(), row() ) == this )
        cell->clearProperty( PBottomBorder );
  }
  KSpreadFormat::setTopBorderPen( p );
}

void KSpreadCell::setRightBorderPen( const QPen& p )
{
    KSpreadCell* cell = 0L;
    if ( column() < KS_colMax )
        cell = m_pTable->cellAt( column() + 1, row() );

    if ( cell && cell->hasProperty( PLeftBorder )
         && m_pTable->cellAt( column(), row() ) == this )
        cell->clearProperty( PLeftBorder );

    KSpreadFormat::setRightBorderPen( p );
}

void KSpreadCell::setBottomBorderPen( const QPen& p )
{
    KSpreadCell* cell = 0L;
    if ( row() < KS_rowMax )
        cell = m_pTable->cellAt( column(), row() + 1 );

    if ( cell && cell->hasProperty( PTopBorder )
         && m_pTable->cellAt( column(), row() ) == this )
        cell->clearProperty( PTopBorder );

    KSpreadFormat::setBottomBorderPen( p );
}

const QPen& KSpreadCell::rightBorderPen( int _col, int _row ) const
{
    if ( !hasProperty( PRightBorder ) && ( _col < KS_colMax ) )
    {
        KSpreadCell * cell = m_pTable->cellAt( _col + 1, _row );
        if ( cell && cell->hasProperty( PLeftBorder ) )
            return cell->leftBorderPen( _col + 1, _row );
    }

    return KSpreadFormat::rightBorderPen( _col, _row );
}

const QPen& KSpreadCell::leftBorderPen( int _col, int _row ) const
{
    if ( !hasProperty( PLeftBorder ) )
    {
        const KSpreadCell * cell = m_pTable->cellAt( _col - 1, _row );
        if ( cell && cell->hasProperty( PRightBorder ) )
            return cell->rightBorderPen( _col - 1, _row );
    }

    return KSpreadFormat::leftBorderPen( _col, _row );
}

const QPen& KSpreadCell::bottomBorderPen( int _col, int _row ) const
{
    if ( !hasProperty( PBottomBorder ) && ( _row < KS_rowMax ) )
    {
        const KSpreadCell * cell = m_pTable->cellAt( _col, _row + 1 );
        if ( cell && cell->hasProperty( PTopBorder ) )
            return cell->topBorderPen( _col, _row + 1 );
    }

    return KSpreadFormat::bottomBorderPen( _col, _row );
}

const QPen& KSpreadCell::topBorderPen( int _col, int _row ) const
{
    if ( !hasProperty( PTopBorder ) )
    {
        const KSpreadCell * cell = m_pTable->cellAt( _col, _row - 1 );
        if ( cell->hasProperty( PBottomBorder ) )
            return cell->bottomBorderPen( _col, _row - 1 );
    }

    return KSpreadFormat::topBorderPen( _col, _row );
}

const QColor & KSpreadCell::effTextColor( int col, int row ) const
{
  if ( m_conditions && m_conditions->matchedStyle()
       && m_conditions->matchedStyle()->hasFeature( KSpreadStyle::STextPen, true ) )
    return m_conditions->matchedStyle()->pen().color();

  return textColor( col, row );
}

const QPen& KSpreadCell::effLeftBorderPen( int col, int row ) const
{
  if ( isObscuringForced() )
  {
    KSpreadCell * cell = m_ObscuringCells.first();
    return cell->effLeftBorderPen( cell->column(), cell->row() );
  }

  if ( m_conditions && m_conditions->matchedStyle()
       && m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SLeftBorder, true ) )
    return m_conditions->matchedStyle()->leftBorderPen();

  return KSpreadFormat::leftBorderPen( col, row );
}

const QPen& KSpreadCell::effTopBorderPen( int col, int row ) const
{
  if ( isObscuringForced() )
  {
    KSpreadCell * cell = m_ObscuringCells.first();
    return cell->effTopBorderPen( cell->column(), cell->row() );
  }

  if ( m_conditions && m_conditions->matchedStyle()
       && m_conditions->matchedStyle()->hasFeature( KSpreadStyle::STopBorder, true ) )
    return m_conditions->matchedStyle()->topBorderPen();

  return KSpreadFormat::topBorderPen( col, row );
}

const QPen& KSpreadCell::effRightBorderPen( int col, int row ) const
{
  if ( isObscuringForced() )
  {
    KSpreadCell * cell = m_ObscuringCells.first();
    return cell->effRightBorderPen( cell->column(), cell->row() );
  }

  if ( m_conditions && m_conditions->matchedStyle()
       && m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SRightBorder, true ) )
    return m_conditions->matchedStyle()->rightBorderPen();

  return KSpreadFormat::rightBorderPen( col, row );
}

const QPen& KSpreadCell::effBottomBorderPen( int col, int row ) const
{
  if ( isObscuringForced() )
  {
    KSpreadCell * cell = m_ObscuringCells.first();
    return cell->effBottomBorderPen( cell->column(), cell->row() );
  }

  if ( m_conditions && m_conditions->matchedStyle()
       && m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SBottomBorder, true ) )
    return m_conditions->matchedStyle()->bottomBorderPen();

  return KSpreadFormat::bottomBorderPen( col, row );
}

const QPen & KSpreadCell::effGoUpDiagonalPen( int col, int row ) const
{
  if ( m_conditions && m_conditions->matchedStyle()
       && m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SGoUpDiagonal, true ) )
    return m_conditions->matchedStyle()->goUpDiagonalPen();

  return KSpreadFormat::goUpDiagonalPen( col, row );
}

const QPen & KSpreadCell::effFallDiagonalPen( int col, int row ) const
{
  if ( m_conditions && m_conditions->matchedStyle()
       && m_conditions->matchedStyle()->hasFeature( KSpreadStyle::SFallDiagonal, true ) )
    return m_conditions->matchedStyle()->fallDiagonalPen();

  return KSpreadFormat::fallDiagonalPen( col, row );
}

uint KSpreadCell::effBottomBorderValue( int col, int row ) const
{
  if ( isObscuringForced() )
  {
    KSpreadCell * cell = m_ObscuringCells.first();
    return cell->effBottomBorderValue( cell->column(), cell->row() );
  }

  if ( m_conditions && m_conditions->matchedStyle() )
    return m_conditions->matchedStyle()->bottomPenValue();

  return KSpreadFormat::bottomBorderValue( col, row );
}

uint KSpreadCell::effRightBorderValue( int col, int row ) const
{
  if ( isObscuringForced() )
  {
    KSpreadCell * cell = m_ObscuringCells.first();
    return cell->effRightBorderValue( cell->column(), cell->row() );
  }

  if ( m_conditions && m_conditions->matchedStyle() )
    return m_conditions->matchedStyle()->rightPenValue();

  return KSpreadFormat::rightBorderValue( col, row );
}

uint KSpreadCell::effLeftBorderValue( int col, int row ) const
{
  if ( isObscuringForced() )
  {
    KSpreadCell * cell = m_ObscuringCells.first();
    return cell->effLeftBorderValue( cell->column(), cell->row() );
  }

  if ( m_conditions && m_conditions->matchedStyle() )
    return m_conditions->matchedStyle()->leftPenValue();

  return KSpreadFormat::leftBorderValue( col, row );
}

uint KSpreadCell::effTopBorderValue( int col, int row ) const
{
  if ( isObscuringForced() )
  {
    KSpreadCell * cell = m_ObscuringCells.first();
    return cell->effTopBorderValue( cell->column(), cell->row() );
  }

  if ( m_conditions && m_conditions->matchedStyle() )
    return m_conditions->matchedStyle()->topPenValue();

  return KSpreadFormat::topBorderValue( col, row );
}

///////////////////////////////////////////
//
// Precision
//
///////////////////////////////////////////

void KSpreadCell::incPrecision()
{
  if ( !m_value.isNumber() )
    return;
  int tmpPreci = precision( column(), row() );
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
      int start = 0;
      if ( m_strOutText.find('%') != -1 )
        start = 2;
      else if ( m_strOutText.find(locale()->currencySymbol()) == ((int)(m_strOutText.length()-locale()->currencySymbol().length())) )
        start = locale()->currencySymbol().length() + 1;
      else if ( (start=m_strOutText.find('E')) != -1 )
        start = m_strOutText.length() - start;

      //kdDebug(36001) << "start=" << start << " pos=" << pos << " length=" << m_strOutText.length() << endl;
      setPrecision( QMAX( 0, (int)m_strOutText.length() - start - pos ) );
    }
  }
  else if ( tmpPreci < 10 )
  {
    setPrecision( ++tmpPreci );
  }
  setFlag(Flag_LayoutDirty);
}

void KSpreadCell::decPrecision()
{
  if ( !m_value.isNumber() )
    return;
  int preciTmp = precision( column(), row() );
//  kdDebug(36001) << "decPrecision: tmpPreci = " << tmpPreci << endl;
  if ( precision(column(),row()) == -1 )
  {
    int pos = m_strOutText.find( decimal_point );
    int start = 0;
    if ( m_strOutText.find('%') != -1 )
        start = 2;
    else if ( m_strOutText.find(locale()->currencySymbol()) == ((int)(m_strOutText.length()-locale()->currencySymbol().length())) )
        start = locale()->currencySymbol().length() + 1;
    else if ( (start = m_strOutText.find('E')) != -1 )
        start = m_strOutText.length() - start;
    else
        start = 0;

    if ( pos == -1 )
      return;

    setPrecision(m_strOutText.length() - pos - 2 - start);
    //   if ( preciTmp < 0 )
    //      setPrecision( preciTmp );
  }
  else if ( preciTmp > 0 )
  {
    setPrecision( --preciTmp );
  }
  setFlag( Flag_LayoutDirty );
}

void KSpreadCell::setDate( QString const & dateString )
{
  clearAllErrors();
  clearFormula();

  delete m_pQML;
  m_pQML = 0L;
  m_content = Text;
  QString str( dateString );

  if ( tryParseDate( dateString ) )
  {
    FormatType tmpFormat = formatType();
    if ( tmpFormat != TextDate &&
         !(tmpFormat >= 200 && tmpFormat <= 216)) // ###
    {
      //test if it's a short date or text date.
      if ((locale()->formatDate( m_value.asDateTime().date(), false) == dateString))
        setFormatType(TextDate);
      else
        setFormatType(ShortDate);
    }
  }
  else
  {
    m_value.setValue( KSpreadValue( dateString ) );

    // convert first letter to uppercase ?
    if (m_pTable->getFirstLetterUpper() && !m_strText.isEmpty())
    {
        str = m_value.asString();
        m_value.setValue( KSpreadValue( str[0].upper()
                                        + str.right( str.length() - 1 ) ) );
    }
  }
  m_strText = str;

  setFlag( Flag_LayoutDirty );
  setFlag( Flag_TextFormatDirty );
  setCalcDirtyFlag();
}

void KSpreadCell::setDate( QDate const & date )
{
  clearAllErrors();
  clearFormula();

  delete m_pQML;
  m_pQML = 0L;
  m_content = Text;

  m_value.setValue( KSpreadValue( date ) );
  m_strText = locale()->formatDate( date, true );
  setFlag( Flag_LayoutDirty );
  setFlag( Flag_TextFormatDirty );
  checkNumberFormat();
  update();
}

void KSpreadCell::setNumber( double number )
{
  clearAllErrors();
  clearFormula();

  delete m_pQML;
  m_pQML = 0L;
  m_content = Text;

  m_value.setValue( KSpreadValue( number ) );
  m_strText.setNum( number );
  setFlag( Flag_LayoutDirty );
  setFlag( Flag_TextFormatDirty );
  checkNumberFormat();
  update();
}

void KSpreadCell::setCellText( const QString& _text, bool updateDepends, bool asText )
{
    QString ctext = _text;
    if( ctext.length() > 5000 )
      ctext = ctext.left( 5000 );

    if ( asText )
    {
      m_content = Text;

      clearAllErrors();
      clearFormula();

      delete m_pQML;
      m_pQML = 0L;

      m_strOutText = ctext;
      m_strText    = ctext;
      m_value.setValue( KSpreadValue( ctext ) );

      setFlag(Flag_LayoutDirty);
      setFlag(Flag_TextFormatDirty);
      if ( updateDepends )
        update();

      return;
    }

    QString oldText = m_strText;
    setDisplayText( ctext, updateDepends );
    if(!m_pTable->isLoading() && !testValidity() )
    {
      //reapply old value if action == stop
      setDisplayText( oldText, updateDepends );
    }
}



void KSpreadCell::setDisplayText( const QString& _text, bool /*updateDepends*/ )
{
  m_pTable->doc()->emitBeginOperation( false );
  clearAllErrors();
  m_strText = _text;

  // Free all content data
  delete m_pQML;
  m_pQML = 0L;
  clearFormula();
  /**
   * A real formula "=A1+A2*3" was entered.
   */
  if ( !m_strText.isEmpty() && m_strText[0] == '=' )
  {
    setFlag(Flag_LayoutDirty);
    setFlag(Flag_TextFormatDirty);

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
    setFlag(Flag_TextFormatDirty);
    m_content = RichText;
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
    setFlag(Flag_TextFormatDirty);
  }

  /**
   *  Special handling for selection boxes
   */
  if ( m_style == ST_Select && !m_pTable->isLoading() )
  {
      SelectPrivate *s = (SelectPrivate*)m_pPrivate;
      if ( m_content == Formula )
          s->parse( m_strFormulaOut );
      else
          s->parse( m_strText );
      kdDebug(36001) << "SELECT " << s->text() << endl;
      checkTextInput(); // is this necessary?
      // setFlag(Flag_LayoutDirty);
  }

  update();

  m_pTable->doc()->emitEndOperation( QRect( m_iColumn, m_iRow, 1, 1 ) );
}

void KSpreadCell::update()
{
  /* those obscuring us need to redo their layout cause they can't obscure us
     now that we've got text.
     This includes cells obscuring cells that we are obscuring
  */
  for (int x = m_iColumn; x <= m_iColumn + extraXCells(); x++)
  {
    for (int y = m_iRow; y <= m_iRow + extraYCells(); y++)
    {
      KSpreadCell* cell = m_pTable->cellAt(x,y);
      cell->setLayoutDirtyFlag();
    }
  }

  /* this call will recursively set cells that reference us as dirty,
     both calc dirty and paint dirty*/
  setCalcDirtyFlag();

  /* TODO - is this a good place for this? */
  updateChart(true);
}

bool KSpreadCell::testValidity() const
{
    bool valid = false;
    if( m_Validity != NULL )
    {
      if( m_value.isNumber() &&
	  (m_Validity->m_allow == Allow_Number ||
	   (m_Validity->m_allow == Allow_Integer &&
	    m_value.asFloat() == ceil(m_value.asFloat()))))
      {
	switch( m_Validity->m_cond)
	{
	  case Equal:
	    valid = ( m_value.asFloat() - m_Validity->valMin < DBL_EPSILON
		      && m_value.asFloat() - m_Validity->valMin >
		      (0.0 - DBL_EPSILON));
	    break;
          case Superior:
	    valid = ( m_value.asFloat() > m_Validity->valMin);
	    break;
          case Inferior:
	    valid = ( m_value.asFloat()  <m_Validity->valMin);
	    break;
          case SuperiorEqual:
	    valid = ( m_value.asFloat() >= m_Validity->valMin);
            break;
          case InferiorEqual:
	    valid = (m_value.asFloat() <= m_Validity->valMin);
	    break;
	  case Between:
	    valid = ( m_value.asFloat() >= m_Validity->valMin &&
		      m_value.asFloat() <= m_Validity->valMax);
	    break;
	  case Different:
	    valid = (m_value.asFloat() < m_Validity->valMin ||
		     m_value.asFloat() > m_Validity->valMax);
	    break;
	  default :
	    break;
        }
      }
      else if(m_Validity->m_allow==Allow_Text)
      {
	valid = m_value.isString();
      }
      else if(m_Validity->m_allow==Allow_TextLength)
      {
	if( m_value.isString() )
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

const KSpreadValue KSpreadCell::value() const
{
  return m_value;
}

void KSpreadCell::setValue( const KSpreadValue& v )
{
    clearFormula();
    clearAllErrors();
    m_value = v;

    if( m_value.isBoolean() )
        m_strOutText = m_strText  = ( m_value.asBoolean() ? i18n("True") : i18n("False") );

    // Free all content data
    delete m_pQML;
    m_pQML = 0;

    setFlag(Flag_LayoutDirty);
    setFlag(Flag_TextFormatDirty);
    m_content = Text;

    m_pTable->setRegionPaintDirty(cellRect());
}


bool KSpreadCell::isDate() const
{
  FormatType ft = formatType();
  // workaround, since date/time is stored as floating-point
  return m_value.isNumber()
    &&  ( ft == ShortDate || ft == TextDate || ( (ft >= date_format1) && (ft <= date_format26) ) );
}

bool KSpreadCell::isTime() const
{
  FormatType ft = formatType();

  // workaround, since date/time is stored as floating-point
  return m_value.isNumber()
    && ( ( (ft >= Time) && (ft <= Time_format8) ) );
}

QDate KSpreadCell::valueDate() const
{
  return m_value.asDateTime().date();
}

QTime KSpreadCell::valueTime() const
{
  return m_value.asDateTime().time();
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
  m_pTable->setRegionPaintDirty(cellRect());

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
    /* we set it temporarily to true to handle recursion (although that
       shouldn't happen if it's not a formula - we might as well be safe).
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
    // Goal of this method: determine the value of the cell
    clearAllErrors();

    m_value = KSpreadValue::empty();

    Q_ASSERT( m_content == Text );

    // Get the text from that cell (using result of formula if any)
    QString str = m_strText;
    if ( m_style == ST_Select )
        str = (static_cast<SelectPrivate*>(m_pPrivate))->text();
    else if ( isFormula() )
        str = m_strFormulaOut;

    // If the text is empty, we don't have a value
    // If the user stated explicitly that he wanted text (using the format or using a quote),
    // then we don't parse as a value, but as string.
    if ( str.isEmpty() || formatType() == Text_format || str.at(0)=='\'' )
    {
        if(m_pTable->getFirstLetterUpper() && !m_strText.isEmpty())
            m_strText=m_strText[0].upper()+m_strText.right(m_strText.length()-1);
        
        m_value.setValue(m_strText);
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
            m_value.setValue( KSpreadValue( m_value.asFloat()/ 100.0 ) );
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
        m_value.setValue( KSpreadValue( money ) );
        setFormatType(Money);
        setFactor(1.0);
        setPrecision(2);
        return;
    }

    if ( tryParseDate( str ) )
    {
        FormatType tmpFormat = formatType();
        if ( tmpFormat != TextDate &&
           !(tmpFormat >= 200 && tmpFormat <= 216)) // ###
        {
            //test if it's a short date or text date.
            if ((locale()->formatDate( m_value.asDateTime().date(), false) == str))
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
        if ( tmpFormat != SecondeTime && tmpFormat != Time_format1
             && tmpFormat != Time_format2 && tmpFormat != Time_format3
             && tmpFormat != Time_format4 && tmpFormat != Time_format6
             && tmpFormat != Time_format5 && tmpFormat != Time_format7
             && tmpFormat != Time_format8 )
          setFormatType(Time);

        // Parsing as time acts like an autoformat: we even change m_strText
        if ( tmpFormat != Time_format7 ) // [h]:mm:ss -> might get set by tryParseTime(str)
          m_strText = locale()->formatTime( m_value.asDateTime().time(), true);
        return;
    }

    // Nothing particular found, then this is simply a string
    m_value.setValue( KSpreadValue( m_strText ) );

    // convert first letter to uppercase ?
    if (m_pTable->getFirstLetterUpper() && !m_strText.isEmpty())
    {
        QString str = m_value.asString();
        m_value.setValue( KSpreadValue( str[0].upper() + str.right( str.length()-1 ) ) );
    }
}

bool KSpreadCell::tryParseBool( const QString& str )
{
    if ( str.lower() == "true" || str.lower() == i18n("True").lower() )
    {
        m_value.setValue( KSpreadValue( true ) );
        return true;
    }
    if ( str.lower() == "false" || str.lower() == i18n("false").lower() )
    {
        m_value.setValue( KSpreadValue( false ) );
        return true;
    }
    return false;
}

bool KSpreadCell::tryParseNumber( const QString& str )
{
    // First try to understand the number using the locale
    bool ok = false;
    double value = locale()->readNumber(str, &ok);
    // If not, try with the '.' as decimal separator
    if ( !ok )
        value = str.toDouble(&ok);

    if ( ok )
    {
        kdDebug(36001) << "KSpreadCell::tryParseNumber '" << str << "' successfully parsed as number: " << value << endl;
        m_value.setValue( KSpreadValue( value ) );
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
        // Note: if shortdate format only specifies 2 digits year, then 3/4/1955 will
	// be treated as in year 3055, while 3/4/55 as year 2055 (because 55 < 69,
	// see KLocale) and thus there's no way to enter for year 1995

	// The following fixes the problem, 3/4/1955 will always be 1955

	QString fmt = locale()->dateFormatShort();
	if( ( fmt.contains( "%y" ) == 1 ) && ( tmpDate.year() > 2999 ) )
             tmpDate = tmpDate.addYears( -1900 );

        // this is another HACK !
        // with two digit years, 0-69 is treated as year 2000-2069 (see KLocale)
        // however, in Excel only 0-29 is year 2000-2029, 30 or later is 1930 onwards

        // the following provides workaround for KLocale so we're compatible with Excel
        // (e.g 3/4/45 is Mar 4, 1945 not Mar 4, 2045)
        if( ( tmpDate.year() >= 2030 ) && ( tmpDate.year() <= 2069 ) )
        {
            QString yearFourDigits = QString::number( tmpDate.year() );
            QString yearTwoDigits = QString::number( tmpDate.year() % 100 );

            // if year is 2045, check to see if "2045" isn't there --> actual input is "45"
            if( ( str.contains( yearTwoDigits ) >= 1 ) && ( str.contains( yearFourDigits ) == 0 ) )
                tmpDate = tmpDate.addYears( -100 );
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
            m_value.setValue( KSpreadValue( tmpDate ) );
            return true;
        }
    }
    return false;
}

bool KSpreadCell::tryParseTime( const QString& str )
{
    bool valid    = false;
    bool duration = false;

    QDateTime tmpTime = util_readTime(str, locale(), true, &valid, duration);
    if (!tmpTime.isValid())
      tmpTime = util_readTime(str, locale(), false, &valid, duration);

    if (!valid)
    {
        QTime tm;
        if(locale()->use12Clock())
        {
            QString stringPm=i18n("pm");
            QString stringAm=i18n("am");
            int pos=0;
            if((pos=str.find(stringPm))!=-1)
            {
                QString tmp=str.mid(0,str.length()-stringPm.length());
                tmp=tmp.simplifyWhiteSpace();
                tm = locale()->readTime(tmp+" "+stringPm, &valid);
                if (!valid)
                    tm = locale()->readTime(tmp+":00 "+stringPm, &valid);
            }
            else if((pos=str.find(stringAm))!=-1)
            {
                QString tmp = str.mid(0,str.length()-stringAm.length());
                tmp = tmp.simplifyWhiteSpace();
                tm = locale()->readTime(tmp+" "+stringAm, &valid);
                if (!valid)
                    tm = locale()->readTime(tmp+":00 "+stringAm, &valid);
            }
        }
        if ( valid )
          m_value.setValue( KSpreadValue( tm ) );
        return valid;
    }
    if (valid)
    {
      if ( duration )
      {
        m_value.setValue( KSpreadValue( tmpTime ) );
        setFormatType( Time_format7 );
      }
      else
        m_value.setValue( KSpreadValue( tmpTime.time() ) );
    }
    return valid;
}

void KSpreadCell::checkNumberFormat()
{
    if ( formatType() == Number && m_value.isNumber() )
    {
        if ( m_value.asFloat() > 1e+10 )
            setFormatType( Scientific );
    }
}

bool KSpreadCell::cellDependsOn(KSpreadSheet *table, int col, int row)
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

QDomElement KSpreadCell::save( QDomDocument& doc, int _x_offset, int _y_offset, bool force, bool copy, bool era )
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
    QDomElement format = KSpreadFormat::save( doc, m_iColumn, m_iRow, force, copy );
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
        format.setAttribute( "style", (int) m_style );


    if ( m_conditions )
    {
      QDomElement conditionElement = m_conditions->saveConditions( doc );

      if ( !conditionElement.isNull() )
        cell.appendChild( conditionElement );
    }

    if ( m_Validity != 0 )
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
        if ( m_Validity->timeMin.isValid() )
        {
                QDomElement timeMin = doc.createElement( "timemin" );
                tmp=m_Validity->timeMin.toString();
                timeMin.appendChild( doc.createTextNode( tmp ) );
                validity.appendChild( timeMin );
        }
        if ( m_Validity->timeMax.isValid() )
        {
                QDomElement timeMax = doc.createElement( "timemax" );
                tmp=m_Validity->timeMax.toString();
                timeMax.appendChild( doc.createTextNode( tmp ) );
                validity.appendChild( timeMax );
        }

        if ( m_Validity->dateMin.isValid() )
        {
                QDomElement dateMin = doc.createElement( "datemin" );
                QString tmp("%1/%2/%3");
                tmp = tmp.arg(m_Validity->dateMin.year()).arg(m_Validity->dateMin.month()).arg(m_Validity->dateMin.day());
                dateMin.appendChild( doc.createTextNode( tmp ) );
                validity.appendChild( dateMin );
        }
        if ( m_Validity->dateMax.isValid() )
        {
                QDomElement dateMax = doc.createElement( "datemax" );
                QString tmp("%1/%2/%3");
                tmp = tmp.arg(m_Validity->dateMax.year()).arg(m_Validity->dateMax.month()).arg(m_Validity->dateMax.day());
                dateMax.appendChild( doc.createTextNode( tmp ) );
                validity.appendChild( dateMax );
        }

        cell.appendChild( validity );
    }

    if ( m_strComment )
    {
        QDomElement comment = doc.createElement( "comment" );
        comment.appendChild( doc.createCDATASection( *m_strComment ) );
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
            QDomElement text = doc.createElement( "text" );
            // if we are cutting to the clipboard, relative references need to be encoded absolutely
            text.appendChild( doc.createTextNode( encodeFormula( era ) ) );
            cell.appendChild( text );

            /* we still want to save the results of the formula */
            QDomElement formulaResult = doc.createElement( "result" );
            saveCellResult( doc, formulaResult, m_strOutText );
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
            QDomElement text = doc.createElement( "text" );
            saveCellResult( doc, text, m_strText );
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
                                  QString str )
{
  QString dataType = "Other"; // fallback

  if ( m_value.isNumber() )
  {
      if ( isDate() )
      {
          // serial number of date
          QDate d = m_value.asDateTime().date();
          dataType = "Date";
          str = "%1/%2/%3";
          str = str.arg(d.year()).arg(d.month()).arg(d.day());
      }
      else if( isTime() )
      {
          // serial number of time
          dataType = "Time";
          str = m_value.asDateTime().time().toString();
      }
      else
      {
          // real number
          dataType = "Num";
          str = QString::number(m_value.asFloat(), 'g', DBL_DIG);
      }
  }

  if ( m_value.isBoolean() )
  {
      dataType = "Bool";
      str = m_value.asBoolean() ? "true" : "false";
  }

  if ( m_value.isString() )
  {
      dataType = "Str";
      str = m_value.asString();
  }

  result.setAttribute( "dataType", dataType );
  if ( !m_strOutText.isEmpty() )
    result.setAttribute( "outStr", m_strOutText );
  result.appendChild( doc.createTextNode( str ) );

  return true; /* really isn't much of a way for this function to fail */
}

bool KSpreadCell::load( const QDomElement & cell, int _xshift, int _yshift,
                        PasteMode pm, Operation op, bool paste )
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
    if ( !f.isNull()
         && ( (pm == Normal) || (pm == Format) || (pm == NoBorder) ) )
    {
        // send pm parameter. Didn't load Borders if pm==NoBorder

        if ( !KSpreadFormat::load( f, pm, paste ) )
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

        if ( testFlag( Flag_ForceExtra ) )
        {
            forceExtraCells( m_iColumn, m_iRow, m_iExtraXCells, m_iExtraYCells );
        }

    }

    //
    // Load the condition section of a cell.
    //
    QDomElement conditionsElement = cell.namedItem( "condition" ).toElement();
    if ( !conditionsElement.isNull())
    {
      delete m_conditions;
      m_conditions = new KSpreadConditions( this );
      m_conditions->loadConditions( conditionsElement );
      m_conditions->checkMatches();
    }

    QDomElement validity = cell.namedItem( "validity" ).toElement();
    if ( !validity.isNull())
    {
        QDomElement param = validity.namedItem( "param" ).toElement();
        if(!param.isNull())
        {
          m_Validity = new KSpreadValidity;
          if ( param.hasAttribute( "cond" ) )
          {
            m_Validity->m_cond = (Conditional) param.attribute("cond").toInt( &ok );
            if ( !ok )
              return false;
          }
          if ( param.hasAttribute( "action" ) )
          {
            m_Validity->m_action = (Action) param.attribute("action").toInt( &ok );
            if ( !ok )
              return false;
          }
          if ( param.hasAttribute( "allow" ) )
          {
            m_Validity->m_allow = (Allow) param.attribute("allow").toInt( &ok );
            if ( !ok )
              return false;
          }
          if ( param.hasAttribute( "valmin" ) )
          {
            m_Validity->valMin = param.attribute("valmin").toDouble( &ok );
            if ( !ok )
              return false;
          }
          if ( param.hasAttribute( "valmax" ) )
          {
            m_Validity->valMax = param.attribute("valmax").toDouble( &ok );
            if ( !ok )
              return false;
          }
        }
        QDomElement title = validity.namedItem( "title" ).toElement();
        if (!title.isNull())
        {
            m_Validity->title = title.text();
        }
        QDomElement message = validity.namedItem( "message" ).toElement();
        if (!message.isNull())
        {
            m_Validity->message = message.text();
        }
        QDomElement timeMin = validity.namedItem( "timemin" ).toElement();
        if ( !timeMin.isNull()  )
        {
            m_Validity->timeMin = toTime(timeMin);
        }
        QDomElement timeMax = validity.namedItem( "timemax" ).toElement();
        if ( !timeMax.isNull()  )
        {
            m_Validity->timeMax = toTime(timeMax);
         }
        QDomElement dateMin = validity.namedItem( "datemin" ).toElement();
        if ( !dateMin.isNull()  )
        {
            m_Validity->dateMin = toDate(dateMin);
         }
        QDomElement dateMax = validity.namedItem( "datemax" ).toElement();
        if ( !dateMax.isNull()  )
        {
            m_Validity->dateMax = toDate(dateMax);
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

    if ( !text.isNull() && ( pm == ::Normal || pm == ::Text || pm == ::NoBorder || pm == ::Result ) )
    {
      /* older versions mistakenly put the datatype attribute on the cell
         instead of the text.  Just move it over in case we're parsing
         an old document */
      if ( cell.hasAttribute( "dataType" ) ) // new docs
      {
        text.setAttribute( "dataType", cell.attribute( "dataType" ) );
      }


      QDomElement result = cell.namedItem( "result" ).toElement();
      QString txt = text.text();
      if ((pm == ::Result) && (txt[0] == '='))
          // paste text of the element, if we want to paste result
          // and the source cell contains a formula
          m_strText = result.text();
      else
          //otherwise copy everything
          loadCellData(text, op);

      if ( !result.isNull() )
      {
        QString dataType;
        QString t = result.text();

        if ( result.hasAttribute( "dataType" ) )
          dataType = result.attribute( "dataType" );
        if ( result.hasAttribute( "outStr" ) )
        {
          m_strOutText = result.attribute( "outStr" );
          if ( !m_strOutText.isEmpty() )
            clearFlag( Flag_TextFormatDirty );
        }

        bool clear = true;
        // boolean ?
        if( dataType == "Bool" )
        {
          if ( t == "false" )
            m_value.setValue( true );
          else if ( t == "true" )
            m_value.setValue( false );
          else
            clear = false;
        }
        else if( dataType == "Num" )
        {
          bool ok = false;
          double d = t.toDouble( &ok );
          if ( ok )
            m_value.setValue ( d );
          else
            clear = false;
        }
        else if( dataType == "Date" )
        {
          bool ok = false;
          double d = t.toDouble( &ok );
          if ( ok )
            m_value.setValue ( d );
          else
          {
            int pos   = t.find( '/' );
            int year  = t.mid( 0, pos ).toInt();
            int pos1  = t.find( '/', pos + 1 );
            int month = t.mid( pos + 1, ( ( pos1 - 1 ) - pos ) ).toInt();
            int day   = t.right( t.length() - pos1 - 1 ).toInt();
            QDate date( year, month, day );
            if ( date.isValid() )
              m_value.setValue( date );
            else
              clear = false;
          }
        }
        else if( dataType == "Time" )
        {
          bool ok = false;
          double d = t.toDouble( &ok );
          if ( ok )
            m_value.setValue( d );
          else
          {
            int hours   = -1;
            int minutes = -1;
            int second  = -1;
            int pos, pos1;
            pos   = t.find( ':' );
            hours = t.mid( 0, pos ).toInt();
            pos1  = t.find( ':', pos + 1 );
            minutes = t.mid( pos + 1, ( ( pos1 - 1 ) - pos ) ).toInt();
            second  = t.right( t.length() - pos1 - 1 ).toInt();
            QTime time( hours, minutes, second );
            if ( time.isValid() )
              m_value.setValue( time );
            else
              clear = false;
          }
        }
        else
        {
          m_value.setValue( t );
        }

        if ( clear )
          clearFlag( Flag_CalcDirty );
      }
    }

    if ( !f.isNull() && f.hasAttribute( "style" ) )
        setStyle( (Style)f.attribute("style").toInt() );

    return true;
}

bool KSpreadCell::loadCellData(const QDomElement & text, Operation op )
{
  QString t = text.text();
  t = t.stripWhiteSpace();

  // feed the change recorder
  if ( !m_pTable->isLoading() && m_pTable->map() && m_pTable->map()->changes() )
  {
    m_pTable->map()->changes()->addChange( m_pTable, this, QPoint( m_iColumn, m_iRow ),
                                           getFormatString( m_iColumn, m_iRow ), this->text() );
  }


  setFlag(Flag_LayoutDirty);
  setFlag(Flag_TextFormatDirty);

  // A formula like =A1+A2 ?
  if( t[0] == '=' )
  {
    clearFormula();
    t = decodeFormula( t, m_iColumn, m_iRow );
    m_strText = pasteOperation( t, m_strText, op );

    setFlag(Flag_CalcDirty);
    clearAllErrors();
    m_content = Formula;

    if ( !m_pTable->isLoading() ) // i.e. when pasting
      if ( !makeFormula() )
        kdError(36001) << "ERROR: Syntax ERROR" << endl;
  }
  // rich text ?
  else if (t[0] == '!' )
  {
      m_pQML = new QSimpleRichText( t.mid(1),  QApplication::font() );//, m_pTable->widget() );
      m_strText = t;
  }
  else
  {
    bool newStyleLoading = true;
    QString dataType;

    if ( text.hasAttribute( "dataType" ) ) // new docs
    {
        dataType = text.attribute( "dataType" );
    }
    else // old docs: do the ugly solution of calling checkTextInput to parse the text
    {
      // ...except for date/time
      FormatType cellFormatType = formatType();
      if ((cellFormatType==KSpreadCell::TextDate ||
           cellFormatType==KSpreadCell::ShortDate
           ||((int)(cellFormatType)>=200 && (int)(cellFormatType)<=217))
          && ( t.contains('/') == 2 ))
        dataType = "Date";
      else if ( (cellFormatType==KSpreadCell::Time
                 || cellFormatType==KSpreadCell::SecondeTime
                 ||cellFormatType==KSpreadCell::Time_format1
                 ||cellFormatType==KSpreadCell::Time_format2
                 ||cellFormatType==KSpreadCell::Time_format3)
                && ( t.contains(':') == 2 ) )
        dataType = "Time";
      else
      {
        m_strText = pasteOperation( t, m_strText, op );
        checkTextInput();
        //kdDebug(36001) << "KSpreadCell::load called checkTextInput, got dataType=" << dataType << "  t=" << t << endl;
        newStyleLoading = false;
      }
    }

    if ( newStyleLoading )
    {
      m_value = KSpreadValue::empty();
      clearAllErrors();

      // boolean ?
      if( dataType == "Bool" )
      {
        if ( t == "false" )
          m_value.setValue( true );
        else if ( t == "true" )
          m_value.setValue( false );
        else
          kdWarning() << "Cell with BoolData, should be true or false: " << t << endl;
      }

      // number ?
      else if( dataType == "Num" )
      {
        bool ok = false;
        m_value.setValue ( KSpreadValue( t.toDouble(&ok) ) ); // We save in non-localized format
        if ( !ok )
	{
          kdWarning(36001) << "Couldn't parse '" << t << "' as number." << endl;
	}
	/* We will need to localize the text version of the number */
	KLocale* locale = m_pTable->doc()->locale();

        /* KLocale::formatNumber requires the precision we want to return.
        */
        int precision = t.length() - t.find('.') - 1;

	if ( formatType() == Percentage )
        {
          setFactor( 100.0 ); // should have been already done by loadFormat
          t = locale->formatNumber( m_value.asFloat() * 100.0, precision );
	  m_strText = pasteOperation( t, m_strText, op );
          m_strText += '%';
        }
        else
	{
          t = locale->formatNumber(m_value.asFloat(), precision);
	  m_strText = pasteOperation( t, m_strText, op );
	}
      }

      // date ?
      else if( dataType == "Date" )
      {
        int pos = t.find('/');
        int year = t.mid(0,pos).toInt();
        int pos1 = t.find('/',pos+1);
        int month = t.mid(pos+1,((pos1-1)-pos)).toInt();
        int day = t.right(t.length()-pos1-1).toInt();
        m_value.setValue( QDate(year,month,day) );
        if ( valueDate().isValid() ) // Should always be the case for new docs
          m_strText = locale()->formatDate( valueDate(), true );
        else // This happens with old docs, when format is set wrongly to date
        {
          m_strText = pasteOperation( t, m_strText, op );
          checkTextInput();
        }
      }

      // time ?
      else if( dataType == "Time" )
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
        m_value.setValue( QTime(hours,minutes,second) );
        if ( valueTime().isValid() ) // Should always be the case for new docs
          m_strText = locale()->formatTime( valueTime(), true );
        else  // This happens with old docs, when format is set wrongly to time
        {
          m_strText = pasteOperation( t, m_strText, op );
          checkTextInput();
        }
      }

      else
      {
        // Set the cell's text
        m_strText = pasteOperation( t, m_strText, op );
        m_value.setValue( m_strText );
      }
    }
  }

  if ( text.hasAttribute( "outStr" ) ) // very new docs
  {
    m_strOutText = text.attribute( "outStr" );
    if ( !m_strOutText.isEmpty() )
      clearFlag( Flag_TextFormatDirty );
  }

  if ( !m_pTable->isLoading() )
    setCellText( m_strText );

  if ( m_conditions )
    m_conditions->checkMatches();

  return true;
}

QTime KSpreadCell::toTime(const QDomElement &element)
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
    m_value.setValue( KSpreadValue( QTime(hours,minutes,second)) );
    return valueTime();
}

QDate KSpreadCell::toDate(const QDomElement &element)
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
    m_value.setValue( KSpreadValue( QDate(year,month,day) ) );
    return valueDate();
}

QString KSpreadCell::pasteOperation( const QString &new_text, const QString &old_text, Operation op )
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
      old = "=0";
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
            tmp_op = QString::number(old.toDouble()+tmp.toDouble());
            break;
        case Mul :
            tmp_op = QString::number(old.toDouble()*tmp.toDouble());
            break;
        case Sub:
            tmp_op = QString::number(old.toDouble()-tmp.toDouble());
            break;
        case Div:
            tmp_op = QString::number(old.toDouble()/tmp.toDouble());
            break;
        default:
            Q_ASSERT( 0 );
        }

        setFlag(Flag_LayoutDirty);
        clearAllErrors();
        m_content = Text;

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

    tmp = decodeFormula( new_text, m_iColumn, m_iRow );
    setFlag(Flag_LayoutDirty);
    clearAllErrors();
    m_content = Formula;

    return tmp;
}

void KSpreadCell::setStyle( Style _s )
{
  if ( m_style == _s )
    return;

  m_style = _s;
  setFlag(Flag_LayoutDirty);

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

  m_pTable->setRegionPaintDirty(cellRect());
}

QString KSpreadCell::testAnchor( int _x, int _y ) const
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

    delete m_pPrivate;
    delete m_pQML;
    delete m_Validity;
    delete m_pCode;

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
  if ( m_value.isNumber() ) // ### what about bools ?
  {
    if ( cell.value().isNumber() )
      return m_value.asFloat() > cell.m_value.asFloat();
    else
      return false; // numbers are always < than texts
  }
  else if(isDate())
  {
     if( cell.isDate() )
        return valueDate() > cell.valueDate();
     else if (cell.value().isNumber())
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
     else if( cell.value().isNumber())
        return true;
     else
        return false; //time are always < than texts
  }
  else
    return m_value.asString().compare(cell.value().asString()) > 0;
}

bool KSpreadCell::operator < ( const KSpreadCell & cell ) const
{
  if ( m_value.isNumber() )
  {
    if ( cell.value().isNumber() )
      return m_value.asFloat() < cell.value().asFloat();
    else
      return true; // numbers are always < than texts
  }
  else if(isDate())
  {
     if( cell.isDate() )
        return m_value.asDateTime().date() < cell.value().asDateTime().date();
     else if( cell.value().isNumber())
        return false;
     else
        return true; //date are always < than texts and time
  }
  else if(isTime())
  {
     if( cell.isTime() )
        return m_value.asDateTime().time() < cell.value().asDateTime().time();
     else if(cell.isDate())
        return false; //time are always > than date
     else if( cell.value().isNumber())
        return false;
     else
        return true; //time are always < than texts
  }
  else
    return m_value.asString().compare(cell.value().asString()) < 0;
}

QRect KSpreadCell::cellRect()
{
  Q_ASSERT(!isDefault());
  return QRect(QPoint(m_iColumn, m_iRow), QPoint(m_iColumn, m_iRow));
}

bool KSpreadCell::isDefault() const
{
    return ( m_iColumn == 0 );
}

void KSpreadCell::NotifyDepending( int col, int row, KSpreadSheet* table, bool isDepending )
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
    alreadyInList = (d->Left() <= col && d->Right() >= col &&
		     d->Top() <= row && d->Bottom() >= row &&
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

QPtrList<KSpreadDependency> KSpreadCell::getDepending ()
{
  QPtrList<KSpreadDependency> retval ;
  KSpreadDependency *d = NULL ;

  for (d = m_lstDependingOnMe.first() ; d != NULL ; d = m_lstDependingOnMe.next())
  {
    KSpreadDependency *d_copy = new KSpreadDependency (*d) ;
	retval.prepend (d_copy) ;
  }

  return retval ;
}

QValueList<KSpreadConditional> KSpreadCell::conditionList() const
{
  if ( !m_conditions )
  {
    QValueList<KSpreadConditional> emptyList;
    return emptyList;
  }

  return m_conditions->conditionList();
}

void KSpreadCell::setConditionList( const QValueList<KSpreadConditional> & newList )
{
  delete m_conditions;
  m_conditions = new KSpreadConditions( this );
  m_conditions->setConditionList( newList );
  m_conditions->checkMatches();
}

bool KSpreadCell::hasError() const
{
  return ( testFlag(Flag_ParseError) ||
           testFlag(Flag_CircularCalculation) ||
           testFlag(Flag_DependancyError));
}

void KSpreadCell::clearAllErrors()
{
  clearFlag( Flag_ParseError );
  clearFlag( Flag_CircularCalculation );
  clearFlag( Flag_DependancyError );
}

bool KSpreadCell::calcDirtyFlag()
{
  return ( m_content == Formula ? false : testFlag( Flag_CalcDirty ) );
}

bool KSpreadCell::layoutDirtyFlag() const
{
  return testFlag( Flag_LayoutDirty );
}

void KSpreadCell::clearDisplayDirtyFlag()
{
  clearFlag( Flag_DisplayDirty );
}

void KSpreadCell::setDisplayDirtyFlag()
{
  setFlag( Flag_DisplayDirty );
}

bool KSpreadCell::isForceExtraCells() const
{
  return testFlag( Flag_ForceExtra );
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

    m_pCell->setLayoutDirtyFlag( true );
    m_pCell->checkTextInput(); // is this necessary ?

    m_pCell->m_pTable->setRegionPaintDirty(m_pCell->cellRect());
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

QString KSpreadCell::name() const
{
    return name( m_iColumn, m_iRow );
}

QString KSpreadCell::fullName() const
{
    return fullName( table(), m_iColumn, m_iRow );
}

QString KSpreadCell::name( int col, int row )
{
    return util_encodeColumnLabelText( col ) + QString::number( row );
}

QString KSpreadCell::fullName( const KSpreadSheet* s, int col, int row )
{
    return s->tableName() + "!" + name( col, row );
}

#include "kspread_cell.moc"


