/* This file is part of the KDE project

   Copyright 2004 Tomas Mecir <mecirt@gmail.com>
   Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
   Copyright 2002-2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2003 Reinhart Geiser <geiseri@kde.org>
   Copyright 2003-2004 Meni Livne <livne@kde.org>
   Copyright 2003 Peter Simonsson <psn@linux.se>
   Copyright 1999-2002 David Faure <faure@kde.org>
   Copyright 2000-2002 Werner Trobin <trobin@kde.org>
   Copyright 1999,2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1998-2000 Torben Weis <weis@kde.org>
   Copyright 2000 Bernd Wuebben <wuebben@kde.org>
   Copyright 2000 Simon Hausmann <hausmann@kde.org
   Copyright 1999 Stephan Kulow <coolo@kde.org>
   Copyright 1999 Michael Reiher <michael.reiher.gmx.de>
   Copyright 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   Copyright 1998-1999 Reginald Stadlbauer <reggie@kde.org>


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
#include <qregexp.h>
#include <qpopupmenu.h>
#include <koStyleStack.h>

#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_format.h"
#include "kspread_global.h"
#include "kspread_map.h"
#include "kspread_sheetprint.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"
#include "kspread_util.h"
#include "ksploadinginfo.h"
#include "kspread_genvalidationstyle.h"
#include "kspread_interpreter.h"
#include "kspread_locale.h"
#include "kspread_view.h"
#include "kspread_value.h"
#include "valueformatter.h"
#include "valueparser.h"

#include <koxmlwriter.h>

#include <kmessagebox.h>

#include <kdebug.h>

#define BORDER_SPACE 1


/**
 * A pointer to the decimal separator
 */

namespace KSpreadCell_LNS
{
  QChar decimal_point = '\0';
}

using namespace KSpreadCell_LNS;


// Some variables are placed in CellExtra because normally they're not required
// in simple case of cell(s). For example, most plain text cells don't need
// to store information about spanned columns and rows, as this is only
// the case with merged cells.
// When the cell is getting complex (e.g. merged with other cells, contains
// rich text, has validation criteria, etc), this CellExtra is allocated by
// CellPrivate and starts to be available. Otherwise, it won't exist at all.

class CellExtra
{
public:
    // Set when the cell contains rich text
    // At the moment, it's used to store hyperlink
    QSimpleRichText *QML;

    // amount of additional cells
    int extraXCells;
    int extraYCells;

    int mergedXCells;
    int mergedYCells;

    // If a cell overlapps other cells, then we have the cells width stored here.
    // This value does not mean anything unless extraXCells is different from 0.
    double extraWidth;

    // If a cell overlapps other cells, then we have the cells height stored here.
    // This value does not mean anything unless d->extra()->extraYCells is different from 0.
    double extraHeight;

    // A list of cells that obscure this one.
    // If this list is not empty, then this cell is obscured by another
    // enlarged object. This means that we have to call this object in order
    // of painting it for example instead of painting 'this'.
    QValueList<KSpreadCell*> obscuringCells;

    KSpreadConditions* conditions;
    KSpreadValidity * validity;

    // Store the number of line when you used multirow (default is 0)
    int nbLines;

private:
    CellExtra& operator=( const CellExtra& );
};


class CellPrivate
{
public:

    // This cell's row. If it is 0, this is the default cell and its row/column can
    // not be determined.
    int row;

    // This cell's column. If it is 0, this is the default cell and its row/column can
    // not be determined.
    int column;

    // Tells which kind of content the cell holds.
    KSpreadCell::Content content;

    // Value of the cell, either typed by user or as result of formula
    KSpreadValue value;

    // Holds the user's input
    QString strText;

    // This is the text we want to display. Not necessarily the same as strText,
    // e.g. strText="1" and strOutText="1.00"
    // Also holds value that we got from calculation, formerly known as strFormulaOut
    QString strOutText;

    // The parse tree of the real formula (e.g: "=A1*A2").
    KSParseNode* code;

    // position and dimension of displayed text
    double textX;
    double textY;
    double textWidth;
    double textHeight;

    // result of "fm.ascent()" in makeLayout. used in offsetAlign.
    int fmAscent;

    KSpreadCell* nextCell;
    KSpreadCell* previousCell;

    CellPrivate();
    ~CellPrivate();

    bool hasExtra() { return (cellExtra != 0); };
    CellExtra* extra();

private:
    // "extra stuff", see explanation for CellExtra
    CellExtra* cellExtra;
};

CellPrivate::CellPrivate()
{
  row = 0;
  column = 0;
  content= KSpreadCell::Text;
  value = KSpreadValue::empty();
  code = 0;

  textX = 0.0;
  textY = 0.0;
  textWidth = 0.0;
  textHeight = 0.0;
  fmAscent = 0;

  nextCell = 0;
  previousCell = 0;

  cellExtra = 0;
}

CellPrivate::~CellPrivate()
{
    delete cellExtra;
}

CellExtra* CellPrivate::extra()
{
    if( !cellExtra )
    {
        cellExtra = new CellExtra;
        cellExtra->QML = 0;
        cellExtra->conditions = 0;
        cellExtra->validity = 0;
        cellExtra->extraXCells = 0;
        cellExtra->extraYCells = 0;
        cellExtra->mergedXCells = 0;
        cellExtra->mergedYCells = 0;
        cellExtra->extraWidth = 0.0;
        cellExtra->extraHeight = 0.0;
        cellExtra->nbLines = 0;
    }

    return cellExtra;
}


/*****************************************************************************
 *
 * KSpreadCell
 *
 *****************************************************************************/

KSpreadCell::KSpreadCell( KSpreadSheet * _table, int _column, int _row )
  : KSpreadFormat( _table, _table->doc()->styleManager()->defaultStyle() )
{
  d = new CellPrivate;
  d->row = _row;
  d->column = _column;
  clearAllErrors();
}

KSpreadCell::KSpreadCell( KSpreadSheet * _table, KSpreadStyle * _style, int _column, int _row )
  : KSpreadFormat( _table, _style )
{
  d = new CellPrivate;
  d->row = _row;
  d->column = _column;
  clearAllErrors();
}

KSpreadSheet * KSpreadCell::sheet() const
{
  return m_pTable;
}

bool KSpreadCell::isDefault() const
{
    return ( d->column == 0 );
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
  return d->row;
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
  return d->column;
}

QString KSpreadCell::name() const
{
    return name( d->column, d->row );
}

QString KSpreadCell::fullName() const
{
    return fullName( table(), d->column, d->row );
}

QString KSpreadCell::name( int col, int row )
{
    return columnName( col ) + QString::number( row );
}

QString KSpreadCell::fullName( const KSpreadSheet* s, int col, int row )
{
    return s->tableName() + "!" + name( col, row );
}

QString KSpreadCell::columnName() const
{
    return columnName( d->column );
}

QString KSpreadCell::columnName( int column )
{
    QString str;
    unsigned digits = 1;
    unsigned offset = 0;

    column--;

    if( column > 4058115285 ) return  QString("@@@");

    for( unsigned limit = 26; column >= limit+offset; limit *= 26, digits++ )
        offset += limit;

    for( unsigned c = column - offset; digits; --digits, c/=26 )
        str.prepend( QChar( 'A' + (c%26) ) );

    return str;
}

KSpreadCell::Content KSpreadCell::content() const
{
    return d->content;
}

bool KSpreadCell::isFormula() const
{
    return d->content == Formula;
}

QString KSpreadCell::text() const
{
    return d->strText;
}

QString KSpreadCell::strOutText() const
{
    return d->strOutText;
}

const KSpreadValue KSpreadCell::value() const
{
  return d->value;
}

void KSpreadCell::setValue( const KSpreadValue& v )
{
  if (v.type() != KSpreadValue::Error)
    clearAllErrors();

  d->value = v;

  // Free all content data
  if (d->hasExtra())
  {
    delete d->extra()->QML;
    d->extra()->QML = 0;
  }

  //set the displayed text, if we hold an error value
  if (d->value.type() == KSpreadValue::Error)
    d->strOutText = d->value.errorMessage ();

  setFlag(Flag_LayoutDirty);
  setFlag(Flag_TextFormatDirty);

  //value of the cell has changed - trigger necessary actions
  valueChanged ();

  m_pTable->setRegionPaintDirty(cellRect());
}

KSpreadCell* KSpreadCell::previousCell() const
{
    return d->previousCell;
}

KSpreadCell* KSpreadCell::nextCell() const
{
    return d->nextCell;
}

void KSpreadCell::setPreviousCell( KSpreadCell* c )
{
    d->previousCell = c;
}

void KSpreadCell::setNextCell( KSpreadCell* c )
{
    d->nextCell = c;
}

KSpreadValidity* KSpreadCell::getValidity( int newStruct  )
{
    if ( (!newStruct) && (!d->hasExtra()))
      //we don't have validity struct and we don't want one
      return 0;

    if( ( d->extra()->validity == 0 ) && ( newStruct == -1 ) )
        d->extra()->validity = new KSpreadValidity;
    return  d->extra()->validity;
}

void KSpreadCell::removeValidity()
{
    if (!d->hasExtra())
      return;

    delete d->extra()->validity;
    d->extra()->validity = 0;
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
    if (d->hasExtra())
      delete d->extra()->conditions;
    if ( cell->d->hasExtra() && cell->d->extra()->conditions )
      setConditionList( conditionList );
    else
      if (d->hasExtra())
        d->extra()->conditions = 0;

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
      setCellText( cell->decodeFormula( d ) );
    }
    else
      setCellText( cell->text() );

}

void KSpreadCell::defaultStyle()
{
  defaultStyleFormat();

  if (!d->hasExtra())
    return;

  if ( d->extra()->conditions )
  {
    delete d->extra()->conditions;
    d->extra()->conditions = 0;
  }

  delete d->extra()->validity;
  d->extra()->validity = 0L;
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
  int extraXCells = d->hasExtra() ? d->extra()->extraXCells : 0;
  int extraYCells = d->hasExtra() ? d->extra()->extraYCells : 0;
  for( int x = _col; x <= _col + extraXCells; ++x )
    for( int y = _row; y <= _row + extraYCells; ++y )
      if ( x != _col || y != _row )
      {
        KSpreadCell * cell = m_pTable->nonDefaultCell( x, y );
        cell->unobscure(this);
      }

  // disable forcing ?
  if ( _x == 0 && _y == 0 )
  {
      clearFlag( Flag_ForceExtra );
      if (d->hasExtra())
      {
        d->extra()->extraXCells  = 0;
        d->extra()->extraYCells  = 0;
        d->extra()->extraWidth   = 0.0;
        d->extra()->extraHeight  = 0.0;
        d->extra()->mergedXCells = 0;
        d->extra()->mergedYCells = 0;
      }
      //refresh the layout
      setFlag( Flag_LayoutDirty );
      return;
  }

    setFlag(Flag_ForceExtra);
    d->extra()->extraXCells  = _x;
    d->extra()->extraYCells  = _y;
    d->extra()->mergedXCells = _x;
    d->extra()->mergedYCells = _y;

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
    //int ey = d->extra()->extraYCells();

    if (d->hasExtra())
      d->extra()->obscuringCells.clear();

    // Unobscure the objects we obscure right now
    int extraXCells = d->hasExtra() ? d->extra()->extraXCells : 0;
    int extraYCells = d->hasExtra() ? d->extra()->extraYCells : 0;
    for( int x = d->column; x <= d->column + extraXCells; ++x )
        for( int y = d->row; y <= d->row + extraYCells; ++y )
            if ( x != d->column || y != d->row )
            {
                KSpreadCell *cell = m_pTable->nonDefaultCell( x, y );
                cell->unobscure(this);
            }

    d->column = col;
    d->row    = row;

    if (d->hasExtra())
    {
      //    d->extra()->extraXCells = 0;
      //    d->extra()->extraYCells = 0;
      d->extra()->mergedXCells = 0;
      d->extra()->mergedYCells = 0;
    }

    //cell value has been changed (because we're another cell now)
    valueChanged ();

    // Reobscure cells if we are forced to do so.
    //if ( m_bForceExtraCells )
      //  forceExtraCells( col, row, ex, ey );
}

void KSpreadCell::setLayoutDirtyFlag( bool format )
{
    setFlag( Flag_LayoutDirty );
    if ( format )
        setFlag( Flag_TextFormatDirty );

    if (!d->hasExtra())
      return;

    QValueList<KSpreadCell*>::iterator it  = d->extra()->obscuringCells.begin();
    QValueList<KSpreadCell*>::iterator end = d->extra()->obscuringCells.end();
    for ( ; it != end; ++it )
    {
  (*it)->setLayoutDirtyFlag( format );
    }
}

bool KSpreadCell::needsPrinting() const
{
    if ( isDefault() )
        return FALSE;

    if ( !d->strText.isEmpty() )
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
    return isDefault() || d->strText.isEmpty();
}


bool KSpreadCell::isObscured() const
{
  if (!d->hasExtra())
    return false;
  return !( d->extra()->obscuringCells.isEmpty() );
}

bool KSpreadCell::isObscuringForced() const
{
  if (!d->hasExtra())
    return false;
  QValueList<KSpreadCell*>::const_iterator it = d->extra()->obscuringCells.begin();
  QValueList<KSpreadCell*>::const_iterator end = d->extra()->obscuringCells.end();
  for ( ; it != end; ++it )
  {
    KSpreadCell *cell = *it;
    if (cell->isForceExtraCells())
    {
      /* the cell might force extra cells, and then overlap even beyond that
         so just knowing that the obscuring cell forces extra isn't enough.
         We have to know that this cell is one of the ones it is forcing over.
      */
      if (column() <= cell->column() + cell->d->extra()->mergedXCells &&
          row() <= cell->row() + cell->mergedYCells() )
      {
        return true;
      }
    }
  }
  return false;
}

QValueList<KSpreadCell*> KSpreadCell::obscuringCells() const
{
  if (!d->hasExtra())
  {
    QValueList<KSpreadCell*> empty;
    return empty;
  }
  return d->extra()->obscuringCells;
}

void KSpreadCell::clearObscuringCells()
{
  if (!d->hasExtra())
    return;
  d->extra()->obscuringCells.clear();
}

void KSpreadCell::obscure( KSpreadCell *cell, bool isForcing )
{
  if (d->hasExtra())
  {
    d->extra()->obscuringCells.remove( cell ); // removes *all* occurrences
    cell->clearObscuringCells();
  }
  if ( isForcing )
  {
    d->extra()->obscuringCells.prepend( cell );
  }
  else
  {
    d->extra()->obscuringCells.append( cell );
  }
  setFlag(Flag_LayoutDirty);
  m_pTable->setRegionPaintDirty( cellRect() );
}

void KSpreadCell::unobscure( KSpreadCell * cell )
{
  if (d->hasExtra())
    d->extra()->obscuringCells.remove( cell );
  setFlag( Flag_LayoutDirty );
  m_pTable->setRegionPaintDirty( cellRect() );
}

void KSpreadCell::clicked( KSpreadCanvas* )
{
    return;
}

QString KSpreadCell::encodeFormula( bool _era, int _col, int _row )
{
    if ( _col == -1 )
        _col = d->column;
    if ( _row == -1 )
        _row = d->row;

    QString erg = "";

    if(d->strText.isEmpty())
        return d->strText;

    bool fix1 = FALSE;
    bool fix2 = FALSE;
    bool onNumber = false;
    unsigned int pos = 0;
    const unsigned int length = d->strText.length();

    // All this can surely be made 10 times faster, but I just "ported" it to QString
    // without any attempt to optimize things -- this is really brittle (Werner)
    while ( pos < length )
    {
        if ( d->strText[pos] == '"' )
        {
            erg += d->strText[pos++];
            while ( pos < length && d->strText[pos] != '"' )  // till the end of the world^H^H^H "string"
            {
                erg += d->strText[pos++];
                // Allow escaped double quotes (\")
                if ( pos < length && d->strText[pos] == '\\' && d->strText[pos+1] == '"' )
                {
                    erg += d->strText[pos++];
                    erg += d->strText[pos++];
                }
            }
            if ( pos < length )  // also copy the trailing double quote
                erg += d->strText[pos++];

            onNumber = false;
        }
        else if ( d->strText[pos].isDigit() )
        {
          erg += d->strText[pos++];
          fix1 = fix2 = FALSE;
          onNumber = true;
        }
        else if ( d->strText[pos] != '$' && !d->strText[pos].isLetter() )
        {
            erg += d->strText[pos++];
            fix1 = fix2 = FALSE;
            onNumber = false;
        }
        else
        {
            QString tmp = "";
            if ( d->strText[pos] == '$' )
            {
                tmp = "$";
                pos++;
                fix1 = TRUE;
            }
            if ( d->strText[pos].isLetter() )
            {
                QString buffer;
                unsigned int pos2 = 0;
                while ( pos < length && d->strText[pos].isLetter() )
                {
                    tmp += d->strText[pos];
                    buffer[pos2++] = d->strText[pos++];
                }
                if ( d->strText[pos] == '$' )
                {
                    tmp += "$";
                    pos++;
                    fix2 = TRUE;
                }
                if ( d->strText[pos].isDigit() )
                {
                    const unsigned int oldPos = pos;
                    while ( pos < length && d->strText[pos].isDigit() ) ++pos;
                    int row = 0;
                    if ( pos != oldPos )
                        row = d->strText.mid(oldPos, pos-oldPos).toInt();
                    // Is it a table name || is it a function name like DEC2HEX
                    /* or if we're parsing a number, this could just be the
                       exponential part of it  (1.23E4) */
                    if ( ( d->strText[pos] == '!' ) ||
                         d->strText[pos].isLetter() ||
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
        _col = d->column;
    if ( _row == -1 )
        _row = d->row;

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
            erg += KSpreadCell::columnName(col); //Get column text

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

  if (!d->hasExtra())
    return;

  for ( int x = d->column + d->extra()->mergedXCells; x <= d->column + d->extra()->extraXCells; ++x )
    for ( int y = d->row + d->extra()->mergedYCells; y <= d->row + d->extra()->extraYCells; ++y )
      if ( x != d->column || y != d->row )
      {
        KSpreadCell *cell = m_pTable->cellAt( x, y );
        cell->unobscure(this);
      }

  d->extra()->extraXCells = d->extra()->mergedXCells;
  d->extra()->extraYCells = d->extra()->mergedYCells;

}

// ##### Are _col and _row really needed ?
void KSpreadCell::makeLayout( QPainter &_painter, int _col, int _row )
{
    // Yes they are: they are useful if this is the default layout,
    // in which case d->row and d->column are 0 and 0, but col and row
    // are the real coordinates of the cell.

    // due to QSimpleRichText, always make layout for QML
    if ( !testFlag( Flag_LayoutDirty ) &&
        ( (!d->hasExtra()) || (!d->extra()->QML) ) )
      return;

    if (d->hasExtra())
      d->extra()->nbLines = 0;
    clearFlag( Flag_CellTooShortX );
    clearFlag( Flag_CellTooShortY );

    freeAllObscuredCells();
    /* but reobscure the ones that are forced obscuring */
    if (d->hasExtra())
      forceExtraCells( d->column, d->row, d->extra()->mergedXCells, d->extra()->mergedYCells );

    ColumnFormat *cl1 = m_pTable->columnFormat( _col );
    RowFormat *rl1 = m_pTable->rowFormat( _row );
    if ( cl1->isHide() || ( rl1->dblHeight() <= m_pTable->doc()->unzoomItY( 2 ) ) )
    {
        clearFlag( Flag_LayoutDirty );
        return;
    }
    setOutputText();

    // Empty text?
    if ( d->strOutText.isEmpty() )
    {
        d->strOutText = QString::null;
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
    if ( d->hasExtra() && d->extra()->QML  )
    {
        delete d->extra()->QML;

        // TODO: more formatting as QStyleSheet supports
        QString qml_text;
        qml_text += QString("<font face=\"%1\">").arg( _painter.font().family() );
        //if( _painter.font().bold() ) qml_text += "<b>";
        //if( _painter.font().italic() ) qml_text += "<i>";
        //if( _painter.font().underline() ) qml_text += "<u>";

        qml_text += d->strText.mid(1);
        d->extra()->QML = new QSimpleRichText( qml_text, _painter.font() );

        setFlag( Flag_LayoutDirty );

        // Calculate how many cells we could use in addition right hand
        // Never use more then 10 cells.
        int right = 0;
        double max_width = dblWidth( _col );
        bool ende = false;
        int c;
        d->extra()->QML->setWidth( &_painter, (int)max_width );
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
                int h = d->extra()->QML->height();
                d->extra()->QML->setWidth( &_painter, int( max_width ) );
                if ( d->extra()->QML->height() < h )
                    ++right;
                else
                {
                    max_width -= cl->dblWidth();
                    d->extra()->QML->setWidth( &_painter, int( max_width ) );
                    ende = true;
                }
            }
        }

        // How may space do we need now ?
        // d->extra()->QML->setWidth( &_painter, max_width );
        int h = d->extra()->QML->height();
        int w = d->extra()->QML->width();
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
        d->extra()->extraXCells = c - _col - 1;

        /* we may have used extra cells, but only cells that we were already
           merged to.
        */
        if( d->extra()->extraXCells < d->extra()->mergedXCells )
        {
            d->extra()->extraXCells = d->extra()->mergedXCells;
        }
        else
        {
            d->extra()->extraWidth = max_width;
        }
        // Occupy the needed extra cells in vertical direction
        double max_height = dblHeight( _row );
        int r = _row;
        ende = ( max_height >= h );
        for( r = _row + 1; !ende && r < _row + 500; ++r )
        {
            bool empty = true;
            for( c = _col; c <= _col + d->extra()->extraXCells; ++c )
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
                for( c = _col; c <= _col + d->extra()->extraXCells; ++c )
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
        d->extra()->extraYCells = r - _row - 1;
        /* we may have used extra cells, but only cells that we were already
           merged to.
        */
        if( d->extra()->extraYCells < d->extra()->mergedYCells )
        {
            d->extra()->extraYCells = d->extra()->mergedYCells;
        }
        else
        {
            d->extra()->extraHeight = max_height;
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
    RowFormat *rl = m_pTable->rowFormat( d->row );
    ColumnFormat *cl = m_pTable->columnFormat( d->column );

    double w = cl->dblWidth();
    double h = rl->dblHeight();

    // Calculate the extraWidth and extraHeight if we are forced to.
    /* Use d->extra()->extraWidth/height here? Isn't it already calculated?*/
    /* No, they are calculated here only (beside of QML part above) Philipp */
    if ( testFlag( Flag_ForceExtra ) )
    {
        int extraXCells = d->hasExtra() ? d->extra()->extraXCells : 0;
        int extraYCells = d->hasExtra() ? d->extra()->extraYCells : 0;

        for ( int x = _col + 1; x <= _col + extraXCells; x++ )
        {
            ColumnFormat *cl = m_pTable->columnFormat( x );
            w += cl->dblWidth() ;
        }
        for ( int y = _row + 1; y <= _row + extraYCells; y++ )
        {
            RowFormat *rl = m_pTable->rowFormat( y );
            h += rl->dblHeight() ;
        }
    }
    if (d->hasExtra())
    {
      d->extra()->extraWidth = w;
      d->extra()->extraHeight = h;
    }

    // Do we need to break the line into multiple lines and are we allowed to
    // do so?
    int lines = 1;
    if ( d->textWidth > w - 2 * BORDER_SPACE - leftBorderWidth( _col, _row ) -
         rightBorderWidth( _col, _row ) && multiRow( _col, _row ) )
    {
        // copy of d->strOutText
        QString o = d->strOutText;

        // No space ?
        if( o.find(' ') != -1 )
        {
            o += ' ';
            int start = 0;
            int pos = 0;
            int pos1 = 0;
            d->strOutText = "";
            do
            {
                pos = o.find( ' ', pos );
                double width = m_pTable->doc()->unzoomItX( fm.width( d->strOutText.mid( start, (pos1-start) )
                                                                     + o.mid( pos1, (pos-pos1) ) ) );

                if ( width <= w - 2 * BORDER_SPACE - leftBorderWidth( _col, _row ) -
                              rightBorderWidth( _col, _row ) )
                {
                    d->strOutText += o.mid( pos1, pos - pos1 );
                    pos1 = pos;
                }
                else
                {
                    if( o.at( pos1 ) == ' ' )
                        pos1 = pos1 + 1;
                    if( pos1 != 0 && pos != -1 )
                    {
                        d->strOutText += "\n" + o.mid( pos1, pos - pos1 );
                        lines++;
                    }
                    else
                        d->strOutText += o.mid( pos1, pos - pos1 );
                    start = pos1;
                    pos1 = pos;
                }
                pos++;
            }
            while( o.find( ' ', pos ) != -1 );
        }

        d->textHeight *= lines;

        if (lines > 1)
          d->extra()->nbLines = lines;
        d->textX = 0.0;

        // Calculate the maximum width
        QString t;
        int i;
        int pos = 0;
        d->textWidth = 0.0;
        do
        {
            i = d->strOutText.find( "\n", pos );
            if ( i == -1 )
                t = d->strOutText.mid( pos, d->strOutText.length() - pos );
            else
            {
                t = d->strOutText.mid( pos, i - pos );
                pos = i + 1;
            }
            double tw = m_pTable->doc()->unzoomItX( fm.width( t ) );
            if ( tw > d->textWidth )
                d->textWidth = tw;
        }
        while ( i != -1 );
    }
    d->fmAscent = fm.ascent();

    // Calculate d->textX and d->textY
    offsetAlign( _col, _row );

    double indent = 0.0;
    int a = effAlignX();
    //apply indent if text is align to left not when text is at right or middle
    if( a == KSpreadCell::Left && !isEmpty() )
        indent = getIndent( _col, _row );

    if( verticalText( _col, _row ) || getAngle( _col, _row ) != 0 )
    {
       RowFormat *rl = m_pTable->rowFormat( _row );

       if( d->textHeight >= rl->dblHeight() )
       {
         setFlag( Flag_CellTooShortX );
       }
    }

    // Do we have to occupy additional cells right hand ?
    if ( d->textWidth + indent > w - 2 * BORDER_SPACE -
         leftBorderWidth( _col, _row ) - rightBorderWidth( _col, _row ) )
    {
      int c = d->column;
      int end = 0;
      // Find free cells right hand to this one
      while ( !end )
      {
        ColumnFormat *cl2 = m_pTable->columnFormat( c + 1 );
        KSpreadCell *cell = m_pTable->visibleCellAt( c + 1, d->row );
        if ( cell->isEmpty() )
        {
          w += cl2->dblWidth() - 1;
          c++;

          // Enough space ?
          if ( d->textWidth + indent <= w - 2 * BORDER_SPACE -
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
        if( c - d->column > d->extra()->mergedXCells )
        {
          d->extra()->extraXCells = c - d->column;
          d->extra()->extraWidth = w;
          for( int i = d->column + 1; i <= c; ++i )
          {
            KSpreadCell *cell = m_pTable->nonDefaultCell( i, d->row );
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
    if ( ( (d->hasExtra() && d->extra()->QML) || multiRow( _col, _row ) ) &&
         d->textHeight > h - 2 * BORDER_SPACE -
         topBorderWidth( _col, _row ) - bottomBorderWidth( _col, _row ) )
    {
      int r = d->row;
      int end = 0;
      // Find free cells bottom to this one
      while ( !end )
      {
        RowFormat *rl2 = m_pTable->rowFormat( r + 1 );
        KSpreadCell *cell = m_pTable->visibleCellAt( d->column, r + 1 );
        if ( cell->isEmpty() )
        {
          h += rl2->dblHeight() - 1.0;
          r++;

          // Enough space ?
          if ( d->textHeight <= h - 2 * BORDER_SPACE -
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
      if( r - d->row > d->extra()->mergedYCells )
      {
        d->extra()->extraYCells = r - d->row;
        d->extra()->extraHeight = h;
        for( int i = d->row + 1; i <= r; ++i )
        {
          KSpreadCell *cell = m_pTable->nonDefaultCell( d->column, i );
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

void KSpreadCell::valueChanged ()
{
  update();

  m_pTable->valueChanged (this);
}

void KSpreadCell::setOutputText()
{
  if ( isDefault() )
  {
    d->strOutText = QString::null;
    if ( d->hasExtra() && d->extra()->conditions )
      d->extra()->conditions->checkMatches();
    return;
  }

  if ( !testFlag( Flag_TextFormatDirty ) )
    return;

  clearFlag( Flag_TextFormatDirty );



  //
  // Turn the stored value in a string
  //

  //should we display the formula?
  if ( (!hasError()) && isFormula() && m_pTable->getShowFormula()
       && !( m_pTable->isProtected() && isHideFormula( d->column, d->row ) ) )
    d->strOutText = d->strText;
  else
  {
    //we should display real value
    KSpread::ValueFormatter *formatter = KSpread::ValueFormatter::self();
    d->strOutText = formatter->formatText (this, formatType());
  }
  
  //check conditions if needed
  if ( d->hasExtra() && d->extra()->conditions )
    d->extra()->conditions->checkMatches();
}

//used in makeLayout() and calculateTextParameters()
void KSpreadCell::offsetAlign( int _col, int _row )
{
    int    a;
    AlignY ay;
    int    tmpAngle;
    bool   tmpVerticalText;
    bool   tmpMultiRow;
    int    tmpTopBorderWidth = effTopBorderPen( _col, _row ).width();

    if ( d->hasExtra() && d->extra()->conditions
        && d->extra()->conditions->matchedStyle() )
    {
      if ( d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SAlignX, true ) )
        a = d->extra()->conditions->matchedStyle()->alignX();
      else
        a = align( _col, _row );

      if ( d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SVerticalText, true ) )
        tmpVerticalText = d->extra()->conditions->matchedStyle()->hasProperty( KSpreadStyle::PVerticalText );
      else
        tmpVerticalText = verticalText( _col, _row );

      if ( d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SMultiRow, true ) )
        tmpMultiRow = d->extra()->conditions->matchedStyle()->hasProperty( KSpreadStyle::PMultiRow );
      else
        tmpMultiRow = multiRow( _col, _row );

      if ( d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SAlignY, true ) )
        ay = d->extra()->conditions->matchedStyle()->alignY();
      else
        ay = alignY( _col, _row );

      if ( d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SAngle, true ) )
        tmpAngle = d->extra()->conditions->matchedStyle()->rotateAngle();
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

    if ( d->hasExtra() && d->extra()->extraXCells )
        w = d->extra()->extraWidth;
    if ( d->hasExtra() && d->extra()->extraYCells )
        h = d->extra()->extraHeight;

    switch( ay )
    {
     case KSpreadCell::Top:
      if ( tmpAngle == 0 )
        d->textY = tmpTopBorderWidth + BORDER_SPACE
          + (double) d->fmAscent / m_pTable->doc()->zoomedResolutionY();
      else
      {
        if ( tmpAngle < 0 )
          d->textY = tmpTopBorderWidth + BORDER_SPACE;
        else
          d->textY = tmpTopBorderWidth + BORDER_SPACE +
            (double)d->fmAscent * cos( tmpAngle * M_PI / 180 ) /
            m_pTable->doc()->zoomedResolutionY();
      }
      break;

     case KSpreadCell::Bottom:
      if ( !tmpVerticalText && !tmpMultiRow && !tmpAngle )
      {
        d->textY = h - BORDER_SPACE - effBottomBorderPen( _col, _row ).width();
        if ( d->hasExtra() && d->extra()->QML )
          d->textY = d->textY - d->extra()->QML->height();
      }
      else if ( tmpAngle != 0 )
      {
        if ( h - BORDER_SPACE - d->textHeight - effBottomBorderPen( _col, _row ).width() > 0 )
        {
          if ( tmpAngle < 0 )
            d->textY = h - BORDER_SPACE - d->textHeight - effBottomBorderPen( _col, _row ).width();
          else
            d->textY = h - BORDER_SPACE - d->textHeight - effBottomBorderPen( _col, _row ).width()
              + (double) d->fmAscent * cos( tmpAngle * M_PI / 180 ) / m_pTable->doc()->zoomedResolutionY();
        }
        else
        {
          if ( tmpAngle < 0 )
            d->textY = tmpTopBorderWidth + BORDER_SPACE ;
          else
            d->textY = tmpTopBorderWidth + BORDER_SPACE
              + (double) d->fmAscent * cos( tmpAngle * M_PI / 180 ) / m_pTable->doc()->zoomedResolutionY();
        }
      }
      else if ( tmpMultiRow )
      {
        int tmpline = d->hasExtra() ? d->extra()->nbLines : 0;
        if ( tmpline > 1 )
          tmpline--;  //number of extra lines
        if( h - BORDER_SPACE - d->textHeight * d->extra()->nbLines - effBottomBorderPen( _col, _row ).width() > 0 )
          d->textY = h - BORDER_SPACE - d->textHeight * tmpline - effBottomBorderPen( _col, _row ).width();
        else
          d->textY = tmpTopBorderWidth + BORDER_SPACE
            + (double) d->fmAscent / m_pTable->doc()->zoomedResolutionY();
      }
      else
        if ( h - BORDER_SPACE - d->textHeight - effBottomBorderPen( _col, _row ).width() > 0 )
          d->textY = h - BORDER_SPACE - d->textHeight - effBottomBorderPen( _col, _row ).width()
            + (double)d->fmAscent / m_pTable->doc()->zoomedResolutionY();
        else
          d->textY = tmpTopBorderWidth + BORDER_SPACE
            + (double) d->fmAscent / m_pTable->doc()->zoomedResolutionY();
      break;

     case KSpreadCell::Middle:
     case KSpreadCell::UndefinedY:
      if ( !tmpVerticalText && !tmpMultiRow && !tmpAngle )
      {
        d->textY = ( h - d->textHeight ) / 2 + (double) d->fmAscent / m_pTable->doc()->zoomedResolutionY();
      }
      else if ( tmpAngle != 0 )
      {
        if ( h - d->textHeight > 0 )
        {
          if ( tmpAngle < 0 )
            d->textY = ( h - d->textHeight ) / 2 ;
          else
            d->textY = ( h - d->textHeight ) / 2 +
              (double) d->fmAscent * cos( tmpAngle * M_PI / 180 ) /
              m_pTable->doc()->zoomedResolutionY();
        }
        else
        {
          if ( tmpAngle < 0 )
            d->textY = tmpTopBorderWidth + BORDER_SPACE;
          else
            d->textY = tmpTopBorderWidth + BORDER_SPACE
              + (double)d->fmAscent * cos( tmpAngle * M_PI / 180 ) / m_pTable->doc()->zoomedResolutionY();
        }
      }
      else if ( tmpMultiRow )
      {
        int tmpline = d->hasExtra() ? d->extra()->nbLines : 0;
        if ( tmpline == 0 )
          tmpline = 1;
        if ( h - d->textHeight * tmpline > 0 )
          d->textY = ( h - d->textHeight * tmpline ) / 2
            + (double) d->fmAscent / m_pTable->doc()->zoomedResolutionY();
        else
          d->textY = tmpTopBorderWidth + BORDER_SPACE
            + (double) d->fmAscent / m_pTable->doc()->zoomedResolutionY();
      }
      else
        if ( h - d->textHeight > 0 )
          d->textY = ( h - d->textHeight ) / 2 + (double)d->fmAscent / m_pTable->doc()->zoomedResolutionY();
        else
          d->textY = tmpTopBorderWidth + BORDER_SPACE
            + (double)d->fmAscent / m_pTable->doc()->zoomedResolutionY();
      break;
    }

    a = effAlignX();
    if ( m_pTable->getShowFormula() && !( m_pTable->isProtected() && isHideFormula( _col, _row ) ) )
      a = KSpreadCell::Left;

    switch( a )
    {
     case KSpreadCell::Left:
      d->textX = effLeftBorderPen( _col, _row ).width() + BORDER_SPACE;
      break;
     case KSpreadCell::Right:
      d->textX = w - BORDER_SPACE - d->textWidth - effRightBorderPen( _col, _row ).width();
      break;
     case KSpreadCell::Center:
      d->textX = ( w - d->textWidth ) / 2;
      break;
    }
}

//used in makeLayout() and calculateTextParameters()
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

    if ( d->hasExtra() && d->extra()->conditions &&
        d->extra()->conditions->matchedStyle() )
    {
      if ( d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SAngle, true ) )
        tmpAngle = d->extra()->conditions->matchedStyle()->rotateAngle();
      else
        tmpAngle = getAngle( _col, _row );

      if ( d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SVerticalText, true ) )
        tmpVerticalText = d->extra()->conditions->matchedStyle()->hasProperty( KSpreadStyle::PVerticalText );
      else
        tmpVerticalText = verticalText( _col, _row );

      if ( d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SAlignY, true ) )
        ay = d->extra()->conditions->matchedStyle()->alignY();
      else
        ay = alignY( _col, _row );

      if ( d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SFontFlag, true ) )
        fontUnderlined = ( d->extra()->conditions->matchedStyle()->fontFlags() && (uint) KSpreadStyle::FUnderline );
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

    if( d->hasExtra() && d->extra()->QML )
    {
     d->textWidth = m_pTable->doc()->unzoomItX( d->extra()->QML->widthUsed() );
     d->textHeight = m_pTable->doc()->unzoomItY( d->extra()->QML->height() );
     return;
    }

    if ( !tmpVerticalText && !tmpAngle )
    {
        d->textWidth = m_pTable->doc()->unzoomItX( fm.width( d->strOutText ) );
        int offsetFont = 0;
        if ( ( ay == KSpreadCell::Bottom ) && fontUnderlined )
        {
            offsetFont = fm.underlinePos() + 1;
        }
        d->textHeight = m_pTable->doc()->unzoomItY( fm.ascent() + fm.descent() + offsetFont );
    }
    // Rotated text ?
    else if ( tmpAngle!= 0 )
    {
        d->textHeight = m_pTable->doc()->unzoomItY( int( cos( tmpAngle * M_PI / 180 ) *
                                                            ( fm.ascent() + fm.descent() ) +
                                                       abs( int( ( fm.width( d->strOutText ) *
                                                          sin( tmpAngle * M_PI / 180 ) ) ) ) ) );
        d->textWidth = m_pTable->doc()->unzoomItX( int( abs( int( ( sin( tmpAngle * M_PI / 180 ) *
                                                                     ( fm.ascent() + fm.descent() ) ) ) ) +
                                                           fm.width( d->strOutText ) *
                                                           cos ( tmpAngle * M_PI / 180 ) ) );
        //kdDebug(36001)<<"d->textWidth"<<d->textWidth<<"d->textHeight"<<d->textHeight<<endl;
    }
    // Vertical text ?
    else
    {
        int width = 0;
        for ( unsigned int i = 0; i < d->strOutText.length(); i++ )
          width = QMAX( width, fm.width( d->strOutText.at( i ) ) );
        d->textWidth = m_pTable->doc()->unzoomItX( width );
        d->textHeight = m_pTable->doc()->unzoomItY( ( fm.ascent() + fm.descent() ) *
                                                       d->strOutText.length() );
    }

}


//used in makeLayout() and calculateTextParameters()
void KSpreadCell::applyZoomedFont( QPainter &painter, int _col, int _row )
{
    QFont tmpFont( textFont( _col, _row ) );
    if ( d->hasExtra() && d->extra()->conditions &&
        d->extra()->conditions->matchedStyle() )
    {
      KSpreadStyle * s = d->extra()->conditions->matchedStyle();
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
      if ( d->extra()->conditions && d->extra()->conditions->currentCondition( condition )
        && !(m_pTable->getShowFormula()
              && !( m_pTable->isProtected() && isHideFormula( d->column, d->row ) ) ) )
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

//used in KSpreadSheet::adjustColumnHelper and KSpreadSheet::adjustRow
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
  /*QString sDelocalizedText ( d->strText );
    int pos=0;
    while ( ( pos = sDelocalizedText.find( decimal_point, pos ) ) >= 0 )
    sDelocalizedText.replace( pos++, 1, "." );
    // At least,  =2,5+3,2  is turned into =2.5+3.2, which can get parsed...
  */
  d->code = m_pTable->doc()->interpreter()->parse( context, m_pTable, /*sDelocalizedText*/d->strText );
  // Did a syntax error occur ?
  if ( context.exception() )
  {
    clearFormula();

    setFlag(Flag_ParseError);
    KSpreadValue v;
    v.setError ( "####" );
    setValue (v);
    if (m_pTable->doc()->getShowMessageError())
    {
      QString tmp(i18n("Error in cell %1\n\n"));
      tmp = tmp.arg( fullName() );
      tmp += context.exception()->toString( context );
      KMessageBox::error( (QWidget*)0L, tmp);
    }
    return false;
  }

  //our value has been changed
  //TODO: is this necessary?
  valueChanged ();

  return true;
}

void KSpreadCell::clearFormula()
{
  delete d->code;
  d->code = 0L;
}

bool KSpreadCell::calc(bool delay)
{
  if ( !isFormula() )
    return true;

  if ( d->code == 0 )
  {
    if ( testFlag( Flag_ParseError ) )  // there was a parse error
      return false;
    else
    {
      /* we were probably at a "isLoading() = true" state when we originally
       * parsed
       */
      makeFormula();

      if ( d->code == 0 ) // there was a parse error
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

  KSContext& context = m_pTable->doc()->context();
  if ( !m_pTable->doc()->interpreter()->evaluate( context, d->code, m_pTable, this ) )
  {
    // If we got an error during evaluation ...
    setFlag(Flag_ParseError);
    setFlag(Flag_LayoutDirty);
    KSpreadValue v;
    v.setError( "####" );
    setValue (v);
    // Print out exception if any
    if ( context.exception() && m_pTable->doc()->getShowMessageError())
    {
      QString tmp(i18n("Error in cell %1\n\n"));
      tmp = tmp.arg( fullName() );
      tmp += context.exception()->toString( context );
      KMessageBox::error( (QWidget*)0L, tmp);
    }

    // setFlag(Flag_LayoutDirty);
    clearFlag(Flag_CalcDirty);

    return false;
  }
  //d->strOutText will be set in setOutputText (called by makeLayout),
  //so we needn't worry about that here
  else if ( context.value()->type() == KSValue::DoubleType )
  {
    setValue ( KSpreadValue( context.value()->doubleValue() ) );
    checkNumberFormat(); // auto-chooses number or scientific
  }
  else if ( context.value()->type() == KSValue::IntType )
  {
    setValue ( KSpreadValue( (int)context.value()->intValue() ) );
    checkNumberFormat(); // auto-chooses number or scientific
  }
  else if ( context.value()->type() == KSValue::BoolType )
  {
    setValue ( KSpreadValue( context.value()->boolValue() ) );
    setFormatType(Number_format);
  }
  else if ( context.value()->type() == KSValue::TimeType )
  {
    setValue( KSpreadValue( context.value()->timeValue() ) );

    //change format
    FormatType tmpFormat = formatType();
    if (!isTime())
    {
      tmpFormat = Time_format;
      setFormatType (Time_format);
    }
  }
  else if ( context.value()->type() == KSValue::DateType)
  {
    setValue ( KSpreadValue( context.value()->dateValue() ) );
    FormatType tmpFormat = formatType();
    if (!isDate())
    {
      tmpFormat = ShortDate_format;
      setFormatType (ShortDate_format);
    }
  }
  else if ( context.value()->type() == KSValue::Empty )
  {
    setValue (KSpreadValue::empty());
    // Format the result appropriately
    setFormatType(Number_format);
  }
  else
  {
//FIXME    m_dataType = StringData;
    QString str = context.value()->toString( context );
    setValue (KSpreadValue (str));
    if (!str.isEmpty() && str[0] == '!' )
    {
      d->extra()->QML = new QSimpleRichText(str.mid(1), QApplication::font()); //, m_pTable->widget() );
    }

    setFormatType(Text_format);
  }

  clearFlag(Flag_CalcDirty);
  setFlag(Flag_LayoutDirty);

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
  Q_ASSERT(!(paintingObscured > 0 && d->extra()->obscuringCells.isEmpty()));

  /* the cellref passed in should be this cell -- except if this is the default
     cell */

  Q_ASSERT(!(((cellRef.x() != d->column) || (cellRef.y() != d->row)) && !isDefault()));

  double left = coordinate.x();

  ColumnFormat * colFormat = m_pTable->columnFormat( cellRef.x() );
  RowFormat    * rowFormat = m_pTable->rowFormat( cellRef.y() );

  double width, height;
  if (d->hasExtra())
  {
    width  = d->extra()->extraXCells ? d->extra()->extraWidth  :
      colFormat->dblWidth();
    height = d->extra()->extraYCells ? d->extra()->extraHeight :
      rowFormat->dblHeight();
  }
  else
  {
    width = colFormat->dblWidth();
    height = rowFormat->dblHeight();
  }

  if ( m_pTable->layoutDirection()==KSpreadSheet::RightToLeft && view && view->canvasWidget() )
  {
    double dwidth = view->doc()->unzoomItX(view->canvasWidget()->width());
    left = dwidth - coordinate.x() - width;
  }

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
  if ( d->hasExtra() && d->extra()->conditions
       &&d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SBackgroundColor, true ) )
    backgroundColor = d->extra()->conditions->matchedStyle()->bgColor();
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
    if ( d->hasExtra() && d->extra()->QML
         && ( !painter.device()->isExtDev() || !getDontprintText( cellRef.x(), cellRef.y() ) )
         && !( m_pTable->isProtected() && isHideAll( cellRef.x(), cellRef.y() ) ) )
    {
      paintText( painter, cellRect, cellRef );
    }
    /**
     * Usual Text
     */
    else
    if ( !d->strOutText.isEmpty()
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
    if (d->hasExtra())
    {
      QValueList<QPoint> listPoints;
      QValueList<KSpreadCell*>::iterator it = d->extra()->obscuringCells.begin();
      QValueList<KSpreadCell*>::iterator end = d->extra()->obscuringCells.end();
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
  if ( d->style == KSpreadCell::ST_Button )
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
  else if ( d->style == KSpreadCell::ST_Select )
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
  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SBackgroundBrush, true ) )
    bb = d->extra()->conditions->matchedStyle()->backGroundBrush();
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

  KSpreadSheet::LayoutDirection sheetDir =  m_pTable->layoutDirection();

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

  if (d->hasExtra())
  {
    QValueList<KSpreadCell*>::const_iterator it  = d->extra()->obscuringCells.begin();
    QValueList<KSpreadCell*>::const_iterator end = d->extra()->obscuringCells.end();
    for ( ; it != end; ++it )
    {
      KSpreadCell *cell = *it;
      paintLeft = paintLeft && ( cell->column() == cellRef.x() );
      paintTop  = paintTop && ( cell->row() == cellRef.y() );
      paintBottom = false;
      paintRight = false;
    }
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

    painter.setPen( QPen( table()->doc()->gridColor(), 1, Qt::SolidLine ) );

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
      if ( sheetDir == KSpreadSheet::RightToLeft )
        painter.drawLine( doc->zoomItX( cellRect.right() ),
                          doc->zoomItY( cellRect.y() + dt ),
                          doc->zoomItX( cellRect.right() ),
                          doc->zoomItY( cellRect.bottom() - db ) );
      else
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

    painter.setPen( QPen( table()->doc()->gridColor(), 1, Qt::SolidLine ) );

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
      if ( sheetDir == KSpreadSheet::RightToLeft )
        painter.drawLine( doc->zoomItX( cellRect.x() ),
                          doc->zoomItY( cellRect.y() + dt ),
                          doc->zoomItX( cellRect.x() ),
                          doc->zoomItY( cellRect.bottom() - db ) );
      else
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

    painter.setPen( QPen( table()->doc()->gridColor(), 1, Qt::SolidLine ) );

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

    painter.setPen( QPen( table()->doc()->gridColor(), 1, Qt::SolidLine ) );

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
    if ( m_pTable->layoutDirection()==KSpreadSheet::RightToLeft )
    {
      point.setPoint( 0, doc->zoomItX( cellRect.x() + 6.0 ),
                         doc->zoomItY( cellRect.y() ) );
      point.setPoint( 1, doc->zoomItX( cellRect.x() ),
                         doc->zoomItY( cellRect.y() ) );
      point.setPoint( 2, doc->zoomItX( cellRect.x() ),
                         doc->zoomItY( cellRect.y() + 6.0 ) );
    }
    else
    {
      point.setPoint( 0, doc->zoomItX( cellRect.right() - 5.0 ),
                         doc->zoomItY( cellRect.y() ) );
      point.setPoint( 1, doc->zoomItX( cellRect.right() ),
                         doc->zoomItY( cellRect.y() ) );
      point.setPoint( 2, doc->zoomItX( cellRect.right() ),
                         doc->zoomItY( cellRect.y() + 5.0 ) );
    }
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
    if ( m_pTable->layoutDirection()==KSpreadSheet::RightToLeft )
    {
      point.setPoint( 0, doc->zoomItX( cellRect.right() - 6.0 ),
                         doc->zoomItY( cellRect.bottom() ) );
      point.setPoint( 1, doc->zoomItX( cellRect.right() ),
                         doc->zoomItY( cellRect.bottom() ) );
      point.setPoint( 2, doc->zoomItX( cellRect.right() ),
                         doc->zoomItY( cellRect.bottom() - 6.0 ) );
    }
    else
    {
      point.setPoint( 0, doc->zoomItX( cellRect.x() ),
                         doc->zoomItY( cellRect.bottom() - 6.0 ) );
      point.setPoint( 1, doc->zoomItX( cellRect.x() ),
                         doc->zoomItY( cellRect.bottom() ) );
      point.setPoint( 2, doc->zoomItX( cellRect.x() + 6.0 ),
                         doc->zoomItY( cellRect.bottom() ) );
    }
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

    if ( d->strOutText.isRightToLeft() )
    {
      point.setPoint( 0, doc->zoomItX( cellRect.left() + 4.0 ),
                         doc->zoomItY( cellRect.y() + cellRect.height() / 2.0 -4.0 ) );
      point.setPoint( 1, doc->zoomItX( cellRect.left() ),
                         doc->zoomItY( cellRect.y() + cellRect.height() / 2.0 ));
      point.setPoint( 2, doc->zoomItX( cellRect.left() + 4.0 ),
                         doc->zoomItY( cellRect.y() + cellRect.height() / 2.0 +4.0 ) );
    }
    else
    {
      point.setPoint( 0, doc->zoomItX( cellRect.right() - 4.0 ),
                         doc->zoomItY( cellRect.y() + cellRect.height() / 2.0 - 4.0 ) );
      point.setPoint( 1, doc->zoomItX( cellRect.right() ),
                         doc->zoomItY( cellRect.y() + cellRect.height() / 2.0 ) );
      point.setPoint( 2, doc->zoomItX( cellRect.right() - 4.0 ),
                         doc->zoomItY( cellRect.y() + cellRect.height() / 2.0 + 4.0 ) );
    }

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
  if ( !d->hasExtra() || !d->extra()->conditions
      || !d->extra()->conditions->matchedStyle() )
  {
    if ( value().isNumber()
         && !( m_pTable->getShowFormula()
               && !( m_pTable->isProtected() && isHideFormula( d->column, d->row ) ) ) )
    {
      double v = value().asFloat() * factor( column(),row() );
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

  QString tmpText = d->strOutText;
  double tmpHeight = d->textHeight;
  double tmpWidth = d->textWidth;
  if( testFlag( Flag_CellTooShortX ) )
  {
    d->strOutText = textDisplaying( painter );
  }

  //hide zero
  if ( m_pTable->getHideZero() && value().isNumber() &&
       value().asFloat() * factor( column(), row() ) == 0 )
  {
    d->strOutText = QString::null;
  }

  if ( colFormat->isHide() || ( cellRect.height() <= 2 ) )
  {
    //clear extra cell if column or row is hidden
    freeAllObscuredCells();  /* TODO: This looks dangerous...must check when I
                                have time */
    d->strOutText = "";
  }

  double indent = 0.0;
  double offsetCellTooShort = 0.0;
  int a = effAlignX();
  //apply indent if text is align to left not when text is at right or middle
  if (  a == KSpreadCell::Left && !isEmpty() )
  {
    if ( d->hasExtra() && d->extra()->conditions
         && d->extra()->conditions->matchedStyle()
         && d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SIndent, true ) )
      indent = d->extra()->conditions->matchedStyle()->indent();
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

  if ( d->hasExtra() && d->extra()->conditions
      && d->extra()->conditions->matchedStyle() )
  {
    if ( d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SAngle, true ) )
      tmpAngle = d->extra()->conditions->matchedStyle()->rotateAngle();
    else
      tmpAngle = getAngle( cellRef.x(), cellRef.y() );

    if ( d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SVerticalText, true ) )
      tmpVerticalText = d->extra()->conditions->matchedStyle()->hasProperty( KSpreadStyle::PVerticalText );
    else
      tmpVerticalText = verticalText( cellRef.x(), cellRef.y() );

    if ( d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SMultiRow, true ) )
      tmpMultiRow = d->extra()->conditions->matchedStyle()->hasProperty( KSpreadStyle::PMultiRow );
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
    if( !d->hasExtra() || !d->extra()->QML )
       painter.drawText( doc->zoomItX( indent + cellRect.x() + d->textX - offsetCellTooShort ),
                      doc->zoomItY( cellRect.y() + d->textY - offsetFont ), d->strOutText );
    else
        d->extra()->QML->draw( &painter,
                    doc->zoomItX( indent + cellRect.x() + d->textX ),
                    doc->zoomItY( cellRect.y() + d->textY - offsetFont ),
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
      x = indent + cellRect.x() + d->textX;
    else
      x = indent + cellRect.x() + d->textX
        - ( fm.descent() + fm.ascent() ) * sin( angle * M_PI / 180 );
    double y;
    if ( angle > 0 )
      y = cellRect.y() + d->textY;
    else
      y = cellRect.y() + d->textY + d->textHeight;
    painter.drawText( doc->zoomItX( x * cos( angle * M_PI / 180 ) +
                                    y * sin( angle * M_PI / 180 ) ),
                      doc->zoomItY( -x * sin( angle * M_PI / 180 ) +
                                     y * cos( angle * M_PI / 180 ) ),
                      d->strOutText );
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
      i = d->strOutText.find( "\n", pos );
      if ( i == -1 )
        t = d->strOutText.mid( pos, d->strOutText.length() - pos );
      else
      {
        t = d->strOutText.mid( pos, i - pos );
        pos = i + 1;
      }

      int a = effAlignX();
      if ( m_pTable->getShowFormula() && !( m_pTable->isProtected() && isHideFormula( d->column, d->row ) ) )
        a = KSpreadCell::Left;

      // #### Torben: This looks duplicated for me
      switch( a )
      {
       case KSpreadCell::Left:
        d->textX = effLeftBorderPen( cellRef.x(), cellRef.y() ).width() + BORDER_SPACE;
        break;

       case KSpreadCell::Right:
        d->textX = cellRect.width() - BORDER_SPACE - doc->unzoomItX( fm.width( t ) )
          - effRightBorderPen( cellRef.x(), cellRef.y() ).width();
        break;

       case KSpreadCell::Center:
        d->textX = ( cellRect.width() - doc->unzoomItX( fm.width( t ) ) ) / 2;
      }

      painter.drawText( doc->zoomItX( indent + cellRect.x() + d->textX ),
                        doc->zoomItY( cellRect.y() + d->textY + dy ), t );
      dy += doc->unzoomItY( fm.descent() + fm.ascent() );
    }
    while ( i != -1 );
  }
  else if ( tmpVerticalText && !d->strOutText.isEmpty() )
  {
    QString t;
    int i = 0;
    int len = 0;
    double dy = 0.0;
    QFontMetrics fm = painter.fontMetrics();
    do
    {
      len = d->strOutText.length();
      t = d->strOutText.at( i );
      painter.drawText( doc->zoomItX( indent + cellRect.x() + d->textX ),
                        doc->zoomItY( cellRect.y() + d->textY + dy ), t );
      dy += doc->unzoomItY( fm.descent() + fm.ascent() );
      i++;
    }
    while ( i != len );
  }

  if ( testFlag( Flag_CellTooShortX ) )
  {
    d->strOutText = tmpText;
    d->textHeight = tmpHeight;
    d->textWidth = tmpWidth;
  }

  if ( m_pTable->getHideZero() && value().isNumber()
       && value().asFloat() * factor( column(), row() ) == 0 )
  {
    d->strOutText = tmpText;
  }

  if ( colFormat->isHide() || ( cellRect.height() <= 2 ) )
    d->strOutText = tmpText;
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

  KSpreadSheet::LayoutDirection sheetDir =  m_pTable->layoutDirection();

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

        if ( sheetDir == KSpreadSheet::RightToLeft )
          painter.drawLine( doc->zoomItX( cellRect.right() ),
                            doc->zoomItY( cellRect.y() ),
                            doc->zoomItX( cellRect.right() ),
                            doc->zoomItY( cellRect.bottom() ) );
        else
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

          if ( sheetDir == KSpreadSheet::RightToLeft )
            painter.drawLine( doc->zoomItX( cellRect.x() ),
                              doc->zoomItY( cellRect.y() ),
                              doc->zoomItX( cellRect.x() ),
                              doc->zoomItY( cellRect.bottom() ) );
          else
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

  KSpreadSheet::LayoutDirection sheetDir =  m_pTable->layoutDirection();
  /* we might not paint some borders if this cell is merged with another in
     that direction
  bool paintLeft   = paintBorderLeft;
  bool paintRight  = paintBorderRight;
  bool paintTop    = paintBorderTop;
  bool paintBottom = paintBorderBottom;
  */

  // paintRight  = paintRight  && ( extraXCells() == 0 );
  // paintBottom = paintBottom && ( d->extra()->extraYCells() == 0 );

  if (d->hasExtra())
  {
    QValueList<KSpreadCell*>::const_iterator it  = d->extra()->obscuringCells.begin();
    QValueList<KSpreadCell*>::const_iterator end = d->extra()->obscuringCells.end();
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
      if ( sheetDir == KSpreadSheet::RightToLeft )
        painter.drawLine( doc->zoomItX( cellRect.right() ),
                          doc->zoomItY( cellRect.y() ) - top,
                          doc->zoomItX( cellRect.right() ),
                          doc->zoomItY( cellRect.bottom() ) + bottom );
      else
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

      if ( sheetDir == KSpreadSheet::RightToLeft )
        painter.drawLine( doc->zoomItX( cellRect.x() ),
                          doc->zoomItY( cellRect.y() ) - top,
                          doc->zoomItX( cellRect.x() ),
                          doc->zoomItY( cellRect.bottom() ) + bottom );
      else
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
      if ( sheetDir == KSpreadSheet::RightToLeft )
        painter.drawLine( doc->zoomItX( cellRect.right() ),
                          doc->zoomItY( cellRect.y() ),
                          doc->zoomItX( cellRect.right() ),
                          doc->zoomItY( cellRect.y() ) + bottom );
      else
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
      if ( sheetDir == KSpreadSheet::RightToLeft )
        painter.drawLine( doc->zoomItX( cellRect.x() ),
                          doc->zoomItY( cellRect.y() ),
                          doc->zoomItX( cellRect.x() ),
                          doc->zoomItY( cellRect.y() ) + bottom );
      else
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
        if ( sheetDir == KSpreadSheet::RightToLeft )
          painter.drawLine( doc->zoomItX( cellRect.right() ),
                            doc->zoomItY( cellRect.bottom() ) - bottom,
                            doc->zoomItX( cellRect.right() ),
                            doc->zoomItY( cellRect.bottom() ) );
        else
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
        if ( sheetDir == KSpreadSheet::RightToLeft )
          painter.drawLine( doc->zoomItX( cellRect.x() ),
                            doc->zoomItY( cellRect.bottom() ) - bottom,
                            doc->zoomItX( cellRect.x() ),
                            doc->zoomItY( cellRect.bottom() ) );
        else
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
    if ( value().isBoolean() || value().isNumber() || (value().isString() && value().asString()[0].direction() == QChar::DirR ))
      a = KSpreadCell::Right;
    else
      a = KSpreadCell::Left;
  }
  return a;
}

int KSpreadCell::effAlignX()
{
  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SAlignX, true ) )
    return d->extra()->conditions->matchedStyle()->alignX();

  return defineAlignX();
}

//used in paintText
//cuts strOutText, so that it only holds the part that can be displayed
QString KSpreadCell::textDisplaying( QPainter &_painter )
{
  QFontMetrics fm = _painter.fontMetrics();
  int a = align( column(), row() );
  if (( a == KSpreadCell::Left || a == KSpreadCell::Undefined) && !value().isNumber()
    && !verticalText( column(),row() ))
  {
    //not enough space but align to left
    double len = 0.0;
    int extraXCells = d->hasExtra() ? d->extra()->extraXCells : 0;
    for ( int i = column(); i <= column() + extraXCells; i++ )
    {
      ColumnFormat *cl2 = m_pTable->columnFormat( i );
      len += cl2->dblWidth() - 1.0; //-1.0 because the pixel in between 2 cells is shared between both cells
    }

    QString tmp;
    double tmpIndent = 0.0;
    if( !isEmpty() )
      tmpIndent = getIndent( column(), row() );
    for( int i = d->strOutText.length(); i != 0; i-- )
    {
      tmp = d->strOutText.left(i);

        if( m_pTable->doc()->unzoomItX( fm.width( tmp ) ) + tmpIndent < len - 4.0 - 1.0 ) //4 equal lenght of red triangle +1 point
        {
            if( getAngle( column(), row() ) != 0 )
            {
                QString tmp2;
                RowFormat *rl = m_pTable->rowFormat( row() );
                if( d->textHeight > rl->dblHeight() )
                {
                    for ( int j = d->strOutText.length(); j != 0; j-- )
                    {
                        tmp2 = d->strOutText.left( j );
                        if( m_pTable->doc()->unzoomItY( fm.width( tmp2 ) ) < rl->dblHeight() - 1.0 )
                        {
                            return d->strOutText.left( QMIN( tmp.length(), tmp2.length() ) );
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
    int extraXCells = d->hasExtra() ? d->extra()->extraXCells : 0;
    for( int i = column(); i <= column() + extraXCells; i++ )
    {
        ColumnFormat *cl2 = m_pTable->columnFormat( i );
        len += cl2->dblWidth() - 1.0; //-1.0 because the pixel in between 2 cells is shared between both cells
    }
    if( !isEmpty() )
        tmpIndent = getIndent( column(), row() );
    if( ( d->textWidth + tmpIndent > len ) || d->textWidth == 0.0 )
        return QString( "" );

    for ( int i = d->strOutText.length(); i != 0; i-- )
    {
        if( m_pTable->doc()->unzoomItY( fm.ascent() + fm.descent() ) * i < rl->dblHeight() - 1.0 )
        {
            return d->strOutText.left( i );
        }
    }
    return QString( "" );
 }

 ColumnFormat *cl = m_pTable->columnFormat( column() );
 double w = cl->dblWidth();
 if ( d->hasExtra() && (d->extra()->extraWidth != 0.0) )
   w = d->extra()->extraWidth;

 if( value().isNumber())
 {
   if( formatType() != Scientific_format )
   {
     int p = (precision(column(),row())  == -1) ? 8 :
       precision(column(),row());
     double val =value().asFloat() * factor(column(),row());
     int pos=0;
     QString localizedNumber= QString::number( (val), 'E', p);
     if((pos=localizedNumber.find('.'))!=-1)
     {
       localizedNumber = localizedNumber.replace( pos, 1, decimal_point );
     }
     if( floatFormat( column(), row() ) ==
            KSpreadCell::AlwaysSigned && val >= 0 )
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
          && !( m_pTable->getShowFormula() && !( m_pTable->isProtected() && isHideFormula( d->column, d->row ) ) ) )
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
   for ( int i = d->strOutText.length(); i != 0; i-- )
   {
     tmp = d->strOutText.left( i );
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
    _col = d->column;

  if ( _canvas )
  {
    if ( testFlag(Flag_ForceExtra) )
      return d->extra()->extraWidth;

    const ColumnFormat *cl = m_pTable->columnFormat( _col );
    return cl->dblWidth( _canvas );
  }

  if ( testFlag(Flag_ForceExtra) )
    return d->extra()->extraWidth;

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
    _row = d->row;

  if ( _canvas )
  {
    if ( testFlag(Flag_ForceExtra) )
      return d->extra()->extraHeight;

    const RowFormat *rl = m_pTable->rowFormat( _row );
    return rl->dblHeight( _canvas );
  }

  if ( testFlag(Flag_ForceExtra) )
    return d->extra()->extraHeight;

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
  if ( d->hasExtra() && (!d->extra()->obscuringCells.isEmpty()) )
  {
    const KSpreadCell* cell = d->extra()->obscuringCells.first();
    return cell->backGroundBrush( cell->column(), cell->row() );
  }

  return KSpreadFormat::backGroundBrush( _col, _row );
}

const QColor& KSpreadCell::bgColor( int _col, int _row ) const
{
  if ( d->hasExtra() && (!d->extra()->obscuringCells.isEmpty()) )
  {
    const KSpreadCell* cell = d->extra()->obscuringCells.first();
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
  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::STextPen, true ) )
    return d->extra()->conditions->matchedStyle()->pen().color();

  return textColor( col, row );
}

const QPen& KSpreadCell::effLeftBorderPen( int col, int row ) const
{
  if ( isObscuringForced() )
  {
    KSpreadCell * cell = d->extra()->obscuringCells.first();
    return cell->effLeftBorderPen( cell->column(), cell->row() );
  }

  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SLeftBorder, true ) )
    return d->extra()->conditions->matchedStyle()->leftBorderPen();

  return KSpreadFormat::leftBorderPen( col, row );
}

const QPen& KSpreadCell::effTopBorderPen( int col, int row ) const
{
  if ( isObscuringForced() )
  {
    KSpreadCell * cell = d->extra()->obscuringCells.first();
    return cell->effTopBorderPen( cell->column(), cell->row() );
  }

  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::STopBorder, true ) )
    return d->extra()->conditions->matchedStyle()->topBorderPen();

  return KSpreadFormat::topBorderPen( col, row );
}

const QPen& KSpreadCell::effRightBorderPen( int col, int row ) const
{
  if ( isObscuringForced() )
  {
    KSpreadCell * cell = d->extra()->obscuringCells.first();
    return cell->effRightBorderPen( cell->column(), cell->row() );
  }

  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SRightBorder, true ) )
    return d->extra()->conditions->matchedStyle()->rightBorderPen();

  return KSpreadFormat::rightBorderPen( col, row );
}

const QPen& KSpreadCell::effBottomBorderPen( int col, int row ) const
{
  if ( isObscuringForced() )
  {
    KSpreadCell * cell = d->extra()->obscuringCells.first();
    return cell->effBottomBorderPen( cell->column(), cell->row() );
  }

  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SBottomBorder, true ) )
    return d->extra()->conditions->matchedStyle()->bottomBorderPen();

  return KSpreadFormat::bottomBorderPen( col, row );
}

const QPen & KSpreadCell::effGoUpDiagonalPen( int col, int row ) const
{
  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SGoUpDiagonal, true ) )
    return d->extra()->conditions->matchedStyle()->goUpDiagonalPen();

  return KSpreadFormat::goUpDiagonalPen( col, row );
}

const QPen & KSpreadCell::effFallDiagonalPen( int col, int row ) const
{
  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SFallDiagonal, true ) )
    return d->extra()->conditions->matchedStyle()->fallDiagonalPen();

  return KSpreadFormat::fallDiagonalPen( col, row );
}

uint KSpreadCell::effBottomBorderValue( int col, int row ) const
{
  if ( isObscuringForced() )
  {
    KSpreadCell * cell = d->extra()->obscuringCells.first();
    return cell->effBottomBorderValue( cell->column(), cell->row() );
  }

  if ( d->hasExtra() && d->extra()->conditions
      && d->extra()->conditions->matchedStyle() )
    return d->extra()->conditions->matchedStyle()->bottomPenValue();

  return KSpreadFormat::bottomBorderValue( col, row );
}

uint KSpreadCell::effRightBorderValue( int col, int row ) const
{
  if ( isObscuringForced() )
  {
    KSpreadCell * cell = d->extra()->obscuringCells.first();
    return cell->effRightBorderValue( cell->column(), cell->row() );
  }

  if ( d->hasExtra() && d->extra()->conditions
      && d->extra()->conditions->matchedStyle() )
    return d->extra()->conditions->matchedStyle()->rightPenValue();

  return KSpreadFormat::rightBorderValue( col, row );
}

uint KSpreadCell::effLeftBorderValue( int col, int row ) const
{
  if ( isObscuringForced() )
  {
    KSpreadCell * cell = d->extra()->obscuringCells.first();
    return cell->effLeftBorderValue( cell->column(), cell->row() );
  }

  if ( d->hasExtra() && d->extra()->conditions
      && d->extra()->conditions->matchedStyle() )
    return d->extra()->conditions->matchedStyle()->leftPenValue();

  return KSpreadFormat::leftBorderValue( col, row );
}

uint KSpreadCell::effTopBorderValue( int col, int row ) const
{
  if ( isObscuringForced() )
  {
    KSpreadCell * cell = d->extra()->obscuringCells.first();
    return cell->effTopBorderValue( cell->column(), cell->row() );
  }

  if ( d->hasExtra() && d->extra()->conditions
      && d->extra()->conditions->matchedStyle() )
    return d->extra()->conditions->matchedStyle()->topPenValue();

  return KSpreadFormat::topBorderValue( col, row );
}

///////////////////////////////////////////
//
// Precision
//
///////////////////////////////////////////

void KSpreadCell::incPrecision()
{
  //TODO: This is ugly. Why not simply regenerate the text to display? Tomas

  if ( !value().isNumber() )
    return;
  int tmpPreci = precision( column(), row() );
  kdDebug(36001) << "incPrecision: tmpPreci = " << tmpPreci << endl;
  if ( tmpPreci == -1 )
  {
    int pos = d->strOutText.find(decimal_point);
    if ( pos == -1 )
        pos = d->strOutText.find('.');
    if ( pos == -1 )
      setPrecision(1);
    else
    {
      int start = 0;
      if ( d->strOutText.find('%') != -1 )
        start = 2;
      else if ( d->strOutText.find(locale()->currencySymbol()) == ((int)(d->strOutText.length()-locale()->currencySymbol().length())) )
        start = locale()->currencySymbol().length() + 1;
      else if ( (start=d->strOutText.find('E')) != -1 )
        start = d->strOutText.length() - start;

      //kdDebug(36001) << "start=" << start << " pos=" << pos << " length=" << d->strOutText.length() << endl;
      setPrecision( QMAX( 0, (int)d->strOutText.length() - start - pos ) );
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
  //TODO: This is ugly. Why not simply regenerate the text to display? Tomas

  if ( !value().isNumber() )
    return;
  int preciTmp = precision( column(), row() );
//  kdDebug(36001) << "decPrecision: tmpPreci = " << tmpPreci << endl;
  if ( precision(column(),row()) == -1 )
  {
    int pos = d->strOutText.find( decimal_point );
    int start = 0;
    if ( d->strOutText.find('%') != -1 )
        start = 2;
    else if ( d->strOutText.find(locale()->currencySymbol()) == ((int)(d->strOutText.length()-locale()->currencySymbol().length())) )
        start = locale()->currencySymbol().length() + 1;
    else if ( (start = d->strOutText.find('E')) != -1 )
        start = d->strOutText.length() - start;
    else
        start = 0;

    if ( pos == -1 )
      return;

    setPrecision(d->strOutText.length() - pos - 2 - start);
    //   if ( preciTmp < 0 )
    //      setPrecision( preciTmp );
  }
  else if ( preciTmp > 0 )
  {
    setPrecision( --preciTmp );
  }
  setFlag( Flag_LayoutDirty );
}

//set numerical value
//used in KSpreadSheet::setSeries (nowhere else yet)
void KSpreadCell::setNumber( double number )
{
  setValue( KSpreadValue( number ) );

  d->content = Text;
  d->strText.setNum( number );
  checkNumberFormat();
}

void KSpreadCell::setCellText( const QString& _text, bool asText )
{
    QString ctext = _text;
    if( ctext.length() > 5000 )
      ctext = ctext.left( 5000 );

    if ( asText )
    {
      d->strOutText = ctext;
      d->strText    = ctext;
      d->content = Text;
      setValue( KSpreadValue( ctext ) );

      return;
    }

    QString oldText = d->strText;
    setDisplayText( ctext );
    if(!m_pTable->isLoading() && !testValidity() )
    {
      //reapply old value if action == stop
      setDisplayText( oldText );
    }
}



void KSpreadCell::setDisplayText( const QString& _text )
{
  m_pTable->doc()->emitBeginOperation( false );
  d->strText = _text;

  /**
   * A real formula "=A1+A2*3" was entered.
   */
  if ( !d->strText.isEmpty() && d->strText[0] == '=' )
  {
    setFlag(Flag_LayoutDirty);
    setFlag(Flag_TextFormatDirty);

    d->content = Formula;
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
  else if ( !d->strText.isEmpty() && d->strText[0] == '!' )
  {
    d->extra()->QML = new QSimpleRichText( d->strText.mid(1),  QApplication::font() );//, m_pTable->widget() );
    setFlag(Flag_LayoutDirty);
    setFlag(Flag_TextFormatDirty);
    d->content = RichText;
  }
  /**
   * Some numeric value or a string.
   */
  else
  {
    d->content = Text;

    // Find out what data type it is
    checkTextInput();

    setFlag(Flag_LayoutDirty);
    setFlag(Flag_TextFormatDirty);
  }

  m_pTable->doc()->emitEndOperation( QRect( d->column, d->row, 1, 1 ) );
}

void KSpreadCell::update()
{
  /* those obscuring us need to redo their layout cause they can't obscure us
     now that we've got text.
     This includes cells obscuring cells that we are obscuring
  */
  for (int x = d->column; x <= d->column + extraXCells(); x++)
  {
    for (int y = d->row; y <= d->row + extraYCells(); y++)
    {
      KSpreadCell* cell = m_pTable->cellAt(x,y);
      cell->setLayoutDirtyFlag();
    }
  }

  setCalcDirtyFlag();

  /* TODO - is this a good place for this? */
  updateChart(true);
}

bool KSpreadCell::testValidity() const
{
    bool valid = false;
    if( d->hasExtra() && d->extra()->validity && d->extra()->validity->m_allow != Allow_All )
    {
        //fixme
        if ( d->extra()->validity->allowEmptyCell && d->strText.isEmpty() )
            return true;

      if( value().isNumber() &&
    (d->extra()->validity->m_allow == Allow_Number ||
     (d->extra()->validity->m_allow == Allow_Integer &&
      value().asFloat() == ceil(value().asFloat()))))
      {
  switch( d->extra()->validity->m_cond)
  {
    case Equal:
      valid = ( value().asFloat() - d->extra()->validity->valMin < DBL_EPSILON
          && value().asFloat() - d->extra()->validity->valMin >
          (0.0 - DBL_EPSILON));
      break;
    case DifferentTo:
      valid = !(  ( value().asFloat() - d->extra()->validity->valMin < DBL_EPSILON
          && value().asFloat() - d->extra()->validity->valMin >
          (0.0 - DBL_EPSILON)) );
      break;
          case Superior:
      valid = ( value().asFloat() > d->extra()->validity->valMin);
      break;
          case Inferior:
      valid = ( value().asFloat()  <d->extra()->validity->valMin);
      break;
          case SuperiorEqual:
      valid = ( value().asFloat() >= d->extra()->validity->valMin);
            break;
          case InferiorEqual:
      valid = (value().asFloat() <= d->extra()->validity->valMin);
      break;
    case Between:
      valid = ( value().asFloat() >= d->extra()->validity->valMin &&
          value().asFloat() <= d->extra()->validity->valMax);
      break;
    case Different:
      valid = (value().asFloat() < d->extra()->validity->valMin ||
         value().asFloat() > d->extra()->validity->valMax);
      break;
    default :
      break;
        }
      }
      else if(d->extra()->validity->m_allow==Allow_Text)
      {
  valid = value().isString();
      }
      else if(d->extra()->validity->m_allow==Allow_TextLength)
      {
  if( value().isString() )
        {
    int len = d->strOutText.length();
    switch( d->extra()->validity->m_cond)
    {
      case Equal:
        if (len == d->extra()->validity->valMin)
    valid = true;
        break;
      case DifferentTo:
        if (len != d->extra()->validity->valMin)
    valid = true;
        break;
      case Superior:
        if(len > d->extra()->validity->valMin)
    valid = true;
        break;
      case Inferior:
        if(len < d->extra()->validity->valMin)
    valid = true;
        break;
      case SuperiorEqual:
        if(len >= d->extra()->validity->valMin)
    valid = true;
        break;
      case InferiorEqual:
        if(len <= d->extra()->validity->valMin)
    valid = true;
        break;
      case Between:
        if(len >= d->extra()->validity->valMin && len <= d->extra()->validity->valMax)
    valid = true;
        break;
      case Different:
        if(len <d->extra()->validity->valMin || len >d->extra()->validity->valMax)
    valid = true;
        break;
      default :
        break;
    }
  }
      }
      else if(d->extra()->validity->m_allow == Allow_Time && isTime())
      {
  switch( d->extra()->validity->m_cond)
  {
    case Equal:
      valid = (value().asTime() == d->extra()->validity->timeMin);
      break;
    case DifferentTo:
      valid = (value().asTime() != d->extra()->validity->timeMin);
      break;
    case Superior:
      valid = (value().asTime() > d->extra()->validity->timeMin);
      break;
    case Inferior:
      valid = (value().asTime() < d->extra()->validity->timeMin);
      break;
    case SuperiorEqual:
      valid = (value().asTime() >= d->extra()->validity->timeMin);
      break;
    case InferiorEqual:
      valid = (value().asTime() <= d->extra()->validity->timeMin);
      break;
    case Between:
      valid = (value().asTime() >= d->extra()->validity->timeMin &&
         value().asTime() <= d->extra()->validity->timeMax);
      break;
      case Different:
      valid = (value().asTime() < d->extra()->validity->timeMin ||
         value().asTime() > d->extra()->validity->timeMax);
      break;
    default :
      break;

  }
      }
      else if(d->extra()->validity->m_allow == Allow_Date && isDate())
      {
  switch( d->extra()->validity->m_cond)
  {
    case Equal:
      valid = (value().asDate() == d->extra()->validity->dateMin);
      break;
    case DifferentTo:
      valid = (value().asDate() != d->extra()->validity->dateMin);
      break;
    case Superior:
      valid = (value().asDate() > d->extra()->validity->dateMin);
      break;
    case Inferior:
      valid = (value().asDate() < d->extra()->validity->dateMin);
      break;
    case SuperiorEqual:
      valid = (value().asDate() >= d->extra()->validity->dateMin);
      break;
    case InferiorEqual:
      valid = (value().asDate() <= d->extra()->validity->dateMin);
      break;
    case Between:
      valid = (value().asDate() >= d->extra()->validity->dateMin &&
         value().asDate() <= d->extra()->validity->dateMax);
      break;
    case Different:
      valid = (value().asDate() < d->extra()->validity->dateMin ||
         value().asDate() > d->extra()->validity->dateMax);
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

    if(!valid &&d->extra()->validity != NULL && d->extra()->validity->displayMessage)
    {
      switch (d->extra()->validity->m_action )
      {
        case Stop:
            KMessageBox::error((QWidget*)0L, d->extra()->validity->message,
           d->extra()->validity->title);
    break;
        case Warning:
    KMessageBox::warningYesNo((QWidget*)0L, d->extra()->validity->message,
            d->extra()->validity->title);
    break;
        case Information:
    KMessageBox::information((QWidget*)0L, d->extra()->validity->message,
           d->extra()->validity->title);
    break;
      }
    }
    if (!d->hasExtra())
      return true;  //okay if there's no validity
    return (valid || d->extra()->validity == NULL || d->extra()->validity->m_action != Stop);
}

FormatType KSpreadCell::formatType() const
{
    return getFormatType( d->column, d->row );
}

double KSpreadCell::textWidth() const
{
    return d->textWidth;
}

double KSpreadCell::textHeight() const
{
    return d->textHeight;
}

int KSpreadCell::mergedXCells() const
{
    return d->hasExtra() ? d->extra()->mergedXCells : 0;
}

int KSpreadCell::mergedYCells() const
{
    return d->hasExtra() ? d->extra()->mergedYCells : 0;
}

int KSpreadCell::extraXCells() const
{
    return d->hasExtra() ? d->extra()->extraXCells : 0;
}

int KSpreadCell::extraYCells() const
{
    return d->hasExtra() ? d->extra()->extraYCells : 0;
}

double KSpreadCell::extraWidth() const
{
    return d->hasExtra() ? d->extra()->extraWidth : 0;
}

double KSpreadCell::extraHeight() const
{
    return d->hasExtra() ? d->extra()->extraHeight : 0;
}


bool KSpreadCell::isDate() const
{
  FormatType ft = formatType();
  // workaround, since date/time is stored as floating-point
  return value().isNumber()
    &&  ( ft == ShortDate_format || ft == TextDate_format || ( (ft >= date_format1) && (ft <= date_format26) ) );
}

bool KSpreadCell::isTime() const
{
  FormatType ft = formatType();

  // workaround, since date/time is stored as floating-point
  return value().isNumber()
    && ( ( (ft >= Time_format) && (ft <= Time_format8) ) );
}

void KSpreadCell::setCalcDirtyFlag()
{
  if ( d->content != Formula )
  {
    //don't set the flag if we don't hold a formula
    clearFlag(Flag_CalcDirty);
    return;
  }
  setFlag(Flag_CalcDirty);
  m_pTable->setRegionPaintDirty(cellRect());
}


bool KSpreadCell::updateChart(bool refresh)
{
    // Update a chart for example if it depends on this cell.
    if ( d->row != 0 && d->column != 0 )
    {
        CellBinding *bind;
        for ( bind = m_pTable->firstCellBinding(); bind != 0L; bind = m_pTable->nextCellBinding() )
        {
            if ( bind->contains( d->column, d->row ) )
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

double KSpreadCell::getDouble ()
{
  if (isDefault())
    return 0.0;
  //(Tomas) umm can't we simply call value().asFloat() ?
  if (isDate())
  {
    QDate date = value().asDate();
    QDate dummy (1900, 1, 1);
    return (dummy.daysTo (date) + 1);
  }
  if (isTime())
  {
    QTime time  = value().asTime();
    QTime dummy;
    return dummy.secsTo( time );
  }
  if (value().isNumber())
    return value().asFloat();

  return 0.0;
}

void KSpreadCell::convertToDouble ()
{
  if (isDefault())
    return;
  if (isTime() || isDate())
    setValue (getDouble ());
  setFactor (1.0);
  setFormatType (Number_format);
}

void KSpreadCell::convertToPercent ()
{
  if (isDefault())
    return;
  if (isTime() || isDate() )
    setValue (getDouble ());
  setFactor (100.0);
  setFormatType (Percentage_format);
}

void KSpreadCell::convertToMoney ()
{
  if (isDefault())
    return;
  if (isTime() || isDate() )
    setValue (getDouble ());
  setFormatType (Money_format);
  setFactor( 1.0 );
  setPrecision (locale()->fracDigits());
}

void KSpreadCell::convertToTime ()
{
  //(Tomas) This is weird. And I mean *REALLY* weird. First, we
  //generate a time (QTime), then we convert it to text, then
  //we give the text to the cell and ask it to parse it. Weird...
  
  if (isDefault() || isEmpty())
    return;
  if (isDate())
    setValue (getDouble ());
  setFormatType (SecondeTime_format);
  QTime time = value().asDateTime().time();
  int msec = (int) ( (value().asFloat() - (int) value().asFloat()) * 1000 );
  time = time.addMSecs( msec );
  setCellText( time.toString() );
}

void KSpreadCell::convertToDate ()
{
  //(Tomas) This is weird. And I mean *REALLY* weird. First, we
  //generate a date (QDate), then we convert it to text, then
  //we give the text to the cell and ask it to parse it. Weird...
  
  if (isDefault() || isEmpty())
    return;
  if (isTime())
    setValue (getDouble ());
  setFormatType (ShortDate_format);
  setFactor( 1.0 );

  //TODO: why did we call setValue(), when we override it here?
  QDate date(1900, 1, 1);
  date = date.addDays( (int) value().asFloat() - 1 );
  date = value().asDateTime().date();
  setCellText (locale()->formatDate (date, true));
}

void KSpreadCell::checkTextInput()
{
  // Goal of this method: determine the value of the cell
  clearAllErrors();

  d->value = KSpreadValue::empty();

  Q_ASSERT( d->content == Text );

  // Get the text from that cell
  QString str = d->strText;

  KSpread::ValueParser::self()->parse (str, this);
  
  // Parsing as time acts like an autoformat: we even change d->strText
  // [h]:mm:ss -> might get set by ValueParser
  if (isTime() && (formatType() != Time_format7))
    d->strText = locale()->formatTime( value().asDateTime().time(), true);
  
  // convert first letter to uppercase ?
  if (m_pTable->getFirstLetterUpper() && value().isString() && 
      (!d->strText.isEmpty()))
  {
    QString str = value().asString();
    setValue( KSpreadValue( str[0].upper() + str.right( str.length()-1 ) ) );
  }
}

//used in calc, setNumber, ValueParser
void KSpreadCell::checkNumberFormat()
{
    if ( formatType() == Number_format && value().isNumber() )
    {
        if ( value().asFloat() > 1e+10 )
            setFormatType( Scientific_format );
    }
}

QDomElement KSpreadCell::save( QDomDocument& doc, int _x_offset, int _y_offset, bool force, bool copy, bool era )
{
    // Save the position of this cell
    QDomElement cell = doc.createElement( "cell" );
    cell.setAttribute( "row", d->row - _y_offset );
    cell.setAttribute( "column", d->column - _x_offset );

    //
    // Save the formatting information
    //
    QDomElement format = KSpreadFormat::save( doc, d->column, d->row, force, copy );
    if ( format.hasChildNodes() || format.attributes().length() ) // don't save empty tags
        cell.appendChild( format );

    if ( isForceExtraCells() )
    {
        if ( extraXCells() )
            format.setAttribute( "colspan", extraXCells() );
        if ( extraYCells() )
            format.setAttribute( "rowspan", extraYCells() );
    }

    if ( d->hasExtra() && d->extra()->conditions )
    {
      QDomElement conditionElement = d->extra()->conditions->saveConditions( doc );

      if ( !conditionElement.isNull() )
        cell.appendChild( conditionElement );
    }

    if ( d->hasExtra() && (d->extra()->validity != 0) )
    {
        QDomElement validity = doc.createElement("validity");

        QDomElement param=doc.createElement("param");
        param.setAttribute("cond",(int)d->extra()->validity->m_cond);
        param.setAttribute("action",(int)d->extra()->validity->m_action);
        param.setAttribute("allow",(int)d->extra()->validity->m_allow);
        param.setAttribute("valmin",d->extra()->validity->valMin);
        param.setAttribute("valmax",d->extra()->validity->valMax);
        param.setAttribute("displaymessage",d->extra()->validity->displayMessage);
        validity.appendChild(param);
        QDomElement title = doc.createElement( "title" );
        title.appendChild( doc.createTextNode( d->extra()->validity->title ) );
        validity.appendChild( title );
        QDomElement message = doc.createElement( "message" );
        message.appendChild( doc.createCDATASection( d->extra()->validity->message ) );
        validity.appendChild( message );

        QString tmp;
        if ( d->extra()->validity->timeMin.isValid() )
        {
                QDomElement timeMin = doc.createElement( "timemin" );
                tmp=d->extra()->validity->timeMin.toString();
                timeMin.appendChild( doc.createTextNode( tmp ) );
                validity.appendChild( timeMin );
        }
        if ( d->extra()->validity->timeMax.isValid() )
        {
                QDomElement timeMax = doc.createElement( "timemax" );
                tmp=d->extra()->validity->timeMax.toString();
                timeMax.appendChild( doc.createTextNode( tmp ) );
                validity.appendChild( timeMax );
        }

        if ( d->extra()->validity->dateMin.isValid() )
        {
                QDomElement dateMin = doc.createElement( "datemin" );
                QString tmp("%1/%2/%3");
                tmp = tmp.arg(d->extra()->validity->dateMin.year()).arg(d->extra()->validity->dateMin.month()).arg(d->extra()->validity->dateMin.day());
                dateMin.appendChild( doc.createTextNode( tmp ) );
                validity.appendChild( dateMin );
        }
        if ( d->extra()->validity->dateMax.isValid() )
        {
                QDomElement dateMax = doc.createElement( "datemax" );
                QString tmp("%1/%2/%3");
                tmp = tmp.arg(d->extra()->validity->dateMax.year()).arg(d->extra()->validity->dateMax.month()).arg(d->extra()->validity->dateMax.day());
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
    if ( !d->strText.isEmpty() )
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
            saveCellResult( doc, formulaResult, d->strOutText );
            cell.appendChild( formulaResult );

        }
        // Have to be saved in some CDATA section because of too many
        // special charatcers.
        else if ( content() == RichText )//|| content() == VisualFormula )
        {
            QDomElement text = doc.createElement( "text" );
            text.appendChild( doc.createCDATASection( d->strText ) );
            cell.appendChild( text );
        }
        else
        {
            // Save the cell contents (in a locale-independent way)
            QDomElement text = doc.createElement( "text" );
            saveCellResult( doc, text, d->strText );
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

  if ( value().isNumber() )
  {
      if ( isDate() )
      {
          // serial number of date
          QDate dd = value().asDateTime().date();
          dataType = "Date";
          str = "%1/%2/%3";
          str = str.arg(dd.year()).arg(dd.month()).arg(dd.day());
      }
      else if( isTime() )
      {
          // serial number of time
          dataType = "Time";
          str = value().asDateTime().time().toString();
      }
      else
      {
          // real number
          dataType = "Num";
          str = QString::number(value().asFloat(), 'g', DBL_DIG);
      }
  }

  if ( value().isBoolean() )
  {
      dataType = "Bool";
      str = value().asBoolean() ? "true" : "false";
  }

  if ( value().isString() )
  {
      dataType = "Str";
      str = value().asString();
  }

  result.setAttribute( "dataType", dataType );
  if ( !d->strOutText.isEmpty() )
    result.setAttribute( "outStr", d->strOutText );
  result.appendChild( doc.createTextNode( str ) );

  return true; /* really isn't much of a way for this function to fail */
}

void KSpreadCell::saveOasisAnnotation( KoXmlWriter &xmlwriter )
{
    if ( m_strComment )
    {
        xmlwriter.startElement( "office:annotation" );
        QStringList text = QStringList::split( "\n", *m_strComment );
        for ( QStringList::Iterator it = text.begin(); it != text.end(); ++it ) {
            xmlwriter.startElement( "text:p" );
            xmlwriter.addTextNode( *it );
            xmlwriter.endElement();
        }
        xmlwriter.endElement();
    }
}



void KSpreadCell::saveOasisCellStyle( KoGenStyle &currentCellStyle )
{
    KSpreadFormat::saveOasisCellStyle( currentCellStyle, column(), row() );
    if ( d->hasExtra() && d->extra()->conditions )
        d->extra()->conditions->saveOasisConditions( currentCellStyle );
}


bool KSpreadCell::saveOasis( KoXmlWriter& xmlwriter, KoGenStyles &mainStyles, int row, int column, int maxCols, int &repeated, KSpreadGenValidationStyles &valStyle )
{
    if ( !isObscuringForced() )
        xmlwriter.startElement( "table:table-cell" );
    else
        xmlwriter.startElement( "table:covered-table-cell" );
#if 0
    //add font style
    QFont font;
    KSpreadValue const value( cell->value() );
    if ( !cell->isDefault() )
    {
      font = cell->textFont( i, row );
      m_styles.addFont( font );

      if ( cell->hasProperty( KSpreadFormat::PComment ) )
        hasComment = true;
    }
#endif
    KoGenStyle currentCellStyle( KSpreadDoc::STYLE_CELL /*Name?????*/ );
    saveOasisCellStyle( currentCellStyle );
    xmlwriter.addAttribute( "table:style-name", mainStyles.lookup( currentCellStyle, "ce" ) );
    // group empty cells with the same style
    if ( isEmpty() && !hasProperty( KSpreadFormat::PComment ) && !isObscuringForced() && !isForceExtraCells() )
    {
      int j = column + 1;
      while ( j <= maxCols )
      {
        KSpreadCell *nextCell = m_pTable->cellAt( j, row );
        KoGenStyle nextCellStyle( KSpreadDoc::STYLE_CELL /*Name?????*/ );
        nextCell->saveOasisCellStyle( nextCellStyle );

        if ( nextCell->isEmpty() && !nextCell->hasProperty( KSpreadFormat::PComment )
             && ( nextCellStyle==currentCellStyle ) && !isObscuringForced() && !isForceExtraCells() )
          ++repeated;
        else
          break;
        ++j;
      }
      if ( repeated > 1 )
        xmlwriter.addAttribute( "table:number-columns-repeated", QString::number( repeated ) );
    }

    saveOasisAnnotation( xmlwriter );
    if ( value().isBoolean() )
    {
        xmlwriter.addAttribute( "office:value-type", "boolean" );
        xmlwriter.addAttribute( "office:boolean-value", ( value().asBoolean() ? "true" : "false" ) );
    }
    else if ( value().isNumber() )
    {
      FormatType type = formatType();

      if ( type == Percentage_format )
        {
        xmlwriter.addAttribute( "office:value-type", "percentage" );
        xmlwriter.addAttribute( "office:value", QString::number( value().asFloat() ) );
        }
      else if ( type == Money_format )
        {
        xmlwriter.addAttribute( "office:value-type", "currency" );
        // TODO: add code of currency
        //xmlwriter.addAttribute( "tableoffice:currency", locale()->currencySymbol() );
        xmlwriter.addAttribute( "office:value", QString::number( value().asFloat() ) );
        }
      else if ( isDate() )
        {
          xmlwriter.addAttribute( "office:value-type", "date" );
          xmlwriter.addAttribute( "office:date-value", value().asDate().toString( Qt::ISODate ) );
        }
      else if ( isTime() )
        {
          xmlwriter.addAttribute( "office:value-type", "time" );
          xmlwriter.addAttribute( "office:time-value", value().asTime().toString( "PThhHmmMssS" ) );
        }
      else
        {
        xmlwriter.addAttribute( "office:value-type", "float" );
        xmlwriter.addAttribute( "office:value", QString::number( value().asFloat() ) );
        }
    }
    else if ( value().isString() )
      {
        xmlwriter.addAttribute( "office:value-type", "string" );
        xmlwriter.addAttribute( "office:string-value", value().asString() );
      }
    else
    {
      kdDebug() << "Type: " << value().type() << endl;
    }
    if (d->hasExtra() && d->extra()->validity)
    {
        KSpreadGenValidationStyle styleVal(d->extra()->validity);
        xmlwriter.addAttribute( "table:validation-name", valStyle.lookup( styleVal ) );
    }
    if ( isFormula() )
    {
      kdDebug() << "Formula found" << endl;
      QString formula( convertFormulaToOasisFormat( text() ) );
      xmlwriter.addAttribute( "table:formula", formula );
    }
    if ( isForceExtraCells() )
    {
      int colSpan = mergedXCells() + 1;
      int rowSpan = mergedYCells() + 1;

      if ( colSpan > 1 )
        xmlwriter.addAttribute( "table:number-columns-spanned", QString::number( colSpan ) );

      if ( rowSpan > 1 )
        xmlwriter.addAttribute( "table:number-rows-spanned", QString::number( rowSpan ) );
    }

    if ( !isEmpty() )
    {
        xmlwriter.startElement( "text:p" );
        xmlwriter.addTextNode(strOutText());
        xmlwriter.endElement();
    }
    xmlwriter.endElement();
    return true;
}


QString KSpreadCell::convertFormulaToOasisFormat( const QString & formula ) const
{
    QString s;
    QRegExp exp("(\\$?)([a-zA-Z]+)(\\$?)([0-9]+)");
    int n = exp.search( formula, 0 );
    kdDebug() << "Exp: " << formula << ", n: " << n << ", Length: " << formula.length()
              << ", Matched length: " << exp.matchedLength() << endl;

    bool inQuote1 = false;
    bool inQuote2 = false;
    int i = 0;
    int l = (int) formula.length();
    if ( l <= 0 )
        return formula;
    while ( i < l )
    {
        if ( ( n != -1 ) && ( n < i ) )
        {
            n = exp.search( formula, i );
            kdDebug() << "Exp: " << formula.right( l - i ) << ", n: " << n << endl;
        }
        if ( formula[i] == '"' )
        {
            inQuote1 = !inQuote1;
            s += formula[i];
            ++i;
            continue;
        }
        if ( formula[i] == '\'' )
        {
            // named area
            inQuote2 = !inQuote2;
            ++i;
            continue;
        }
        if ( inQuote1 || inQuote2 )
        {
            s += formula[i];
            ++i;
            continue;
        }
        if ( ( formula[i] == '=' ) && ( formula[i + 1] == '=' ) )
        {
            s += '=';
            ++i;++i;
            continue;
        }
        if ( formula[i] == '!' )
        {
            insertBracket( s );
            s += '.';
            ++i;
            continue;
        }
        if ( formula[i] == ',' )
        {
            s += '.';
            ++i;
            continue;
        }
        if ( n == i )
        {
            int ml = exp.matchedLength();
            if ( formula[ i + ml ] == '!' )
            {
                kdDebug() << "No cell ref but sheet name" << endl;
                s += formula[i];
                ++i;
                continue;
            }
            if ( ( i > 0 ) && ( formula[i - 1] != '!' ) )
                s += "[.";
            for ( int j = 0; j < ml; ++j )
            {
                s += formula[i];
                ++i;
            }
            s += ']';
            continue;
        }

        s += formula[i];
        ++i;
    }

    return s;
}

void KSpreadCell::loadOasisConditional( QDomElement * style )
{
    if ( style )//safe
    {
        QDomElement elementItem = style->firstChild().toElement();
        elementItem = elementItem.firstChild().toElement();
        if ( !elementItem.isNull() )
        {
            if (d->hasExtra())
              delete d->extra()->conditions;
            d->extra()->conditions = new KSpreadConditions( this );
            d->extra()->conditions->loadOasisConditions( elementItem );
            d->extra()->conditions->checkMatches();
        }
    }
}

bool KSpreadCell::loadOasis( const QDomElement &element, const KoOasisStyles& oasisStyles )
{
    QString text;
    kdDebug()<<" table:style-name :"<<element.attribute( "table:style-name" )<<endl;
    if ( element.hasAttribute( "table:style-name" ) )
    {
        QString str = element.attribute( "table:style-name" );
        QDomElement * style = oasisStyles.styles()[str];
        kdDebug()<<" style :"<<style<<endl;
        KoStyleStack styleStack;
        styleStack.push( *style );
        styleStack.setTypeProperties( "cell" );
        loadOasisStyleProperties( styleStack, oasisStyles );
        loadOasisConditional( style );
    }
    QDomElement textP = element.namedItem( "text:p" ).toElement();
    if ( !textP.isNull() )
    {
        QDomElement subText = textP.firstChild().toElement();
        if ( !subText.isNull() )
  {
            // something in <text:p>, e.g. links
            text = subText.text();

            if ( subText.hasAttribute( "xlink:href" ) )
      {
                QString link = subText.attribute( "xlink:href" );
                text = "!<a href=\"" + link + "\"><i>" + text + "</i></a>";
                d->extra()->QML = new QSimpleRichText( text.mid(1),  QApplication::font() );//, m_pTable->widget() );
                d->strText = text;
      }
  }
        else
  {
            text = textP.text(); // our text, could contain formating for value or result of formul
            setCellText( text );
            setValue( text );
  }
    }
    bool isFormula = false;
    if ( element.hasAttribute( "table:formula" ) )
    {
        kdDebug()<<" formula :"<<element.attribute( "table:formula" )<<endl;
        isFormula = true;
        QString formula;
        convertFormula( formula, element.attribute( "table:formula" ) );
        setCellText( formula );
    }
    if ( element.hasAttribute( "table:validation-name" ) )
    {
        kdDebug()<<" Cel has a validation :"<<element.attribute( "table:validation-name" )<<endl;
        loadOasisValidation( element.attribute( "table:validation-name" ) );
    }
    if( element.hasAttribute( "office:value-type" ) )
    {
        QString valuetype = element.attribute( "office:value-type" );
        if( valuetype == "boolean" )
        {
            QString val = element.attribute( "office:boolean-value" );
            if( ( val == "true" ) || ( val == "false" ) )
            {
                bool value = val == "true";
                setValue( value );
                setCellText( value ? i18n("True") : i18n("False" ) );
            }
        }

        // integer and floating-point value
        else if( valuetype == "float" )
        {
            bool ok = false;
            double value = element.attribute( "office:value" ).toDouble( &ok );
            if ( !isFormula )
                if( ok )
                    setValue( value );
        }

        // currency value
        else if( valuetype == "currency" )
        {
            bool ok = false;
            double value = element.attribute( "office:value" ).toDouble( &ok );
            if( ok )
            {
                if ( !isFormula )
                    setValue( value );
                setCurrency( 1, element.attribute( "office:currency" ) );
                setFormatType (Money_format);
            }
        }
        else if( valuetype == "percentage" )
        {
            bool ok = false;
            double value = element.attribute( "office:value" ).toDouble( &ok );
            if( ok )
            {
                if ( !isFormula )
                    setValue( value );
                setFormatType (Percentage_format);
            }
        }
        else if ( valuetype == "date" )
        {
            QString value = element.attribute( "office:value" );
            if ( value.isEmpty() )
                value = element.attribute( "office:date-value" );
            kdDebug() << "Type: date, value: " << value << endl;

            // "1980-10-15"
            int year = 0, month = 0, day = 0;
            bool ok = false;

            int p1 = value.find( '-' );
            if ( p1 > 0 )
                year  = value.left( p1 ).toInt( &ok );

            kdDebug() << "year: " << value.left( p1 ) << endl;

            int p2 = value.find( '-', ++p1 );

            if ( ok )
                month = value.mid( p1, p2 - p1  ).toInt( &ok );

            kdDebug() << "month: " << value.mid( p1, p2 - p1 ) << endl;

            if ( ok )
                day = value.right( value.length() - p2 - 1 ).toInt( &ok );

            kdDebug() << "day: " << value.right( value.length() - p2 ) << endl;

            if ( ok )
            {
                setValue( QDate( year, month, day ) );
                setFormatType (ShortDate_format);
                kdDebug() << "Set QDate: " << year << " - " << month << " - " << day << endl;
            }

        }
        else if ( valuetype == "time" )
        {
            QString value = element.attribute( "office:value" );
            if ( value.isEmpty() )
                value = element.attribute( "office:time-value" );
            kdDebug() << "Type: time: " << value << endl;
            // "PT15H10M12S"
            int hours = 0, minutes = 0, seconds = 0;
            int l = value.length();
            QString num;
            bool ok = false;
            for ( int i = 0; i < l; ++i )
            {
                if ( value[i].isNumber() )
                {
                    num += value[i];
                    continue;
                }
                else if ( value[i] == 'H' )
                    hours   = num.toInt( &ok );
                else if ( value[i] == 'M' )
                    minutes = num.toInt( &ok );
                else if ( value[i] == 'S' )
                    seconds = num.toInt( &ok );
                else
                    continue;

                kdDebug() << "Num: " << num << endl;

                num = "";
                if ( !ok )
                    break;
            }
            kdDebug() << "Hours: " << hours << ", " << minutes << ", " << seconds << endl;

            if ( ok )
            {
                // KSpreadValue kval( timeToNum( hours, minutes, seconds ) );
                // cell->setValue( kval );
                setValue( QTime( hours % 24, minutes, seconds ) );
                setFormatType (Time_format);
            }
        }
        else if( valuetype == "string" )
        {
          QString value = element.attribute( "office:value" );
            if ( value.isEmpty() )
                value = element.attribute( "office:string-value" );
          setValue( value );
          setFormatType (Text_format);
        }
        else
            kdDebug()<<" type of value found : "<<valuetype<<endl;
    }
    // merged cells ?
    int colSpan = 1;
    int rowSpan = 1;
    if ( element.hasAttribute( "table:number-columns-spanned" ) )
    {
        bool ok = false;
        int span = element.attribute( "table:number-columns-spanned" ).toInt( &ok );
        if( ok ) colSpan = span;
    }
    if ( element.hasAttribute( "table:number-rows-spanned" ) )
    {
        bool ok = false;
        int span = element.attribute( "table:number-rows-spanned" ).toInt( &ok );
        if( ok ) rowSpan = span;
    }
    if ( colSpan > 1 || rowSpan > 1 )
        forceExtraCells( d->column, d->row, colSpan - 1, rowSpan - 1 );

    // cell comment/annotation
    QDomElement annotationElement = element.namedItem( "office:annotation" ).toElement();
    if ( !annotationElement.isNull() )
    {
        QString comment;
        QDomNode node = annotationElement.firstChild();
        while( !node.isNull() )
        {
            QDomElement commentElement = node.toElement();
            if( !commentElement.isNull() )
                if( commentElement.tagName() == "text:p" )
                {
                    if( !comment.isEmpty() ) comment.append( '\n' );
                    comment.append( commentElement.text() );
                }

            node = node.nextSibling();
        }

        if( !comment.isEmpty() )
            setComment( comment );
    }
    return true;
}

void KSpreadCell::loadOasisValidation( const QString& validationName )
{
    kdDebug()<<"validationName:"<<validationName<<endl;
    QDomElement element = table()->doc()->loadingInfo()->validation( validationName);
    if (d->hasExtra())
      delete d->extra()->validity;
    d->extra()->validity = new KSpreadValidity;
    if ( element.hasAttribute( "table:condition" ) )
    {
        QString valExpression = element.attribute( "table:condition" );
        kdDebug()<<" element.attribute( table:condition ) "<<valExpression<<endl;
        //Condition ::= ExtendedTrueCondition | TrueFunction 'and' TrueCondition
        //TrueFunction ::= cell-content-is-whole-number() | cell-content-is-decimal-number() | cell-content-is-date() | cell-content-is-time()
        //ExtendedTrueCondition ::= ExtendedGetFunction | cell-content-text-length() Operator Value
        //TrueCondition ::= GetFunction | cell-content() Operator Value
        //GetFunction ::= cell-content-is-between(Value, Value) | cell-content-is-not-between(Value, Value)
        //ExtendedGetFunction ::= cell-content-text-length-is-between(Value, Value) | cell-content-text-length-is-not-between(Value, Value)
        //Operator ::= '<' | '>' | '<=' | '>=' | '=' | '!='
        //Value ::= NumberValue | String | Formula
        //A Formula is a formula without an equals (=) sign at the beginning. See section 8.1.3 for more information.
        //A String comprises one or more characters surrounded by quotation marks.
        //A NumberValue is a whole or decimal number. It must not contain comma separators for numbers of 1000 or greater.

        //ExtendedTrueCondition
        if ( valExpression.contains( "cell-content-text-length()" ) )
        {
            //"cell-content-text-length()>45"
            valExpression = valExpression.remove("cell-content-text-length()" );
            kdDebug()<<" valExpression = :"<<valExpression<<endl;
            d->extra()->validity->m_allow = Allow_TextLength;

            loadOasisValidationCondition( valExpression );
        }
        else if ( valExpression.contains( "cell-content-is-text()" ) )
        {
            d->extra()->validity->m_allow = Allow_Text;
        }
        //cell-content-text-length-is-between(Value, Value) | cell-content-text-length-is-not-between(Value, Value)
        else if ( valExpression.contains( "cell-content-text-length-is-between" ) )
        {
            d->extra()->validity->m_allow = Allow_TextLength;
            d->extra()->validity->m_cond = Between;
            valExpression = valExpression.remove( "cell-content-text-length-is-between(" );
            kdDebug()<<" valExpression :"<<valExpression<<endl;
            valExpression = valExpression.remove( ")" );
            QStringList listVal = QStringList::split( ",", valExpression );
            loadOasisValidationValue( listVal );
        }
        else if ( valExpression.contains( "cell-content-text-length-is-not-between" ) )
        {
            d->extra()->validity->m_allow = Allow_TextLength;
            d->extra()->validity->m_cond = Different;
            valExpression = valExpression.remove( "cell-content-text-length-is-not-between(" );
            kdDebug()<<" valExpression :"<<valExpression<<endl;
            valExpression = valExpression.remove( ")" );
            kdDebug()<<" valExpression :"<<valExpression<<endl;
            QStringList listVal = QStringList::split( ",", valExpression );
            loadOasisValidationValue( listVal );
        }
        //TrueFunction ::= cell-content-is-whole-number() | cell-content-is-decimal-number() | cell-content-is-date() | cell-content-is-time()
        else
        {
            if (valExpression.contains( "cell-content-is-whole-number()" ) )
            {
                d->extra()->validity->m_allow =  Allow_Number;
                valExpression = valExpression.remove( "cell-content-is-whole-number() and " );
            }
            else if (valExpression.contains( "cell-content-is-decimal-number()" ) )
            {
                d->extra()->validity->m_allow = Allow_Integer;
                valExpression = valExpression.remove( "cell-content-is-decimal-number() and " );
            }
            else if (valExpression.contains( "cell-content-is-date()" ) )
            {
                d->extra()->validity->m_allow = Allow_Date;
                valExpression = valExpression.remove( "cell-content-is-date() and " );
            }
            else if (valExpression.contains( "cell-content-is-time()" ) )
            {
                d->extra()->validity->m_allow = Allow_Time;
                valExpression = valExpression.remove( "cell-content-is-time() and " );
            }
            kdDebug()<<"valExpression :"<<valExpression<<endl;

            if ( valExpression.contains( "cell-content()" ) )
            {
                valExpression = valExpression.remove( "cell-content()" );
                loadOasisValidationCondition( valExpression );
            }
            //GetFunction ::= cell-content-is-between(Value, Value) | cell-content-is-not-between(Value, Value)
            //for the moment we support just int/double value, not text/date/time :(
            if ( valExpression.contains( "cell-content-is-between(" ) )
            {
                valExpression = valExpression.remove( "cell-content-is-between(" );
                valExpression = valExpression.remove( ")" );
                QStringList listVal = QStringList::split( "," , valExpression );
                loadOasisValidationValue( listVal );
                d->extra()->validity->m_cond = Between;
            }
            if ( valExpression.contains( "cell-content-is-not-between(" ) )
            {
                valExpression = valExpression.remove( "cell-content-is-not-between(" );
                valExpression = valExpression.remove( ")" );
                QStringList listVal = QStringList::split( ",", valExpression );
                loadOasisValidationValue( listVal );
                d->extra()->validity->m_cond = Different;
            }
        }
    }
    if ( element.hasAttribute( "table:allow-empty-cell" ) )
    {
        kdDebug()<<" element.hasAttribute( table:allow-empty-cell ) :"<<element.hasAttribute( "table:allow-empty-cell" )<<endl;
        d->extra()->validity->allowEmptyCell = ( ( element.attribute( "table:allow-empty-cell" )=="true" ) ? true : false );
    }
    if ( element.hasAttribute( "table:base-cell-address" ) )
    {
        //todo what is it ?
    }

    QDomElement help = element.namedItem( "table:help-message" ).toElement();
    if ( !help.isNull() )
    {
        if ( help.hasAttribute( "table:title" ) )
        {
            kdDebug()<<"help.attribute( table:title ) :"<<help.attribute( "table:title" )<<endl;
            d->extra()->validity->titleInfo = help.attribute( "table:title" );
        }
        if ( help.hasAttribute( "table:display" ) )
        {
            kdDebug()<<"help.attribute( table:display ) :"<<help.attribute( "table:display" )<<endl;
            d->extra()->validity->displayValidationInformation = ( ( help.attribute( "table:display" )=="true" ) ? true : false );
        }
        QDomElement attrText = help.namedItem( "text:p" ).toElement();
        if ( !attrText.isNull() )
        {
            kdDebug()<<"help text :"<<attrText.text()<<endl;
            d->extra()->validity->messageInfo = attrText.text();
        }
    }

    QDomElement error = element.namedItem( "table:error-message" ).toElement();
    if ( !error.isNull() )
    {
        if ( error.hasAttribute( "table:title" ) )
            d->extra()->validity->title = error.attribute( "table:title" );
        if ( error.hasAttribute( "table:message-type" ) )
        {
            QString str = error.attribute( "table:message-type" );
            if ( str == "warning" )
                d->extra()->validity->m_action = Warning;
            else if ( str == "information" )
                d->extra()->validity->m_action = Information;
            else if ( str == "stop" )
                d->extra()->validity->m_action = Stop;
            else
                kdDebug()<<"validation : message type unknown  :"<<str<<endl;
        }

        if ( error.hasAttribute( "table:display" ) )
        {
            kdDebug()<<" display message :"<<error.attribute( "table:display" )<<endl;
            d->extra()->validity->displayMessage = (error.attribute( "table:display" )=="true");
        }
        QDomElement attrText = error.namedItem( "text:p" ).toElement();
        if ( !attrText.isNull() )
            d->extra()->validity->message = attrText.text();
    }
}


void KSpreadCell::loadOasisValidationValue( const QStringList &listVal )
{
    bool ok = false;
    kdDebug()<<" listVal[0] :"<<listVal[0]<<" listVal[1] :"<<listVal[1]<<endl;

    if ( d->extra()->validity->m_allow == Allow_Date )
    {
        d->extra()->validity->dateMin = QDate::fromString( listVal[0] );
        d->extra()->validity->dateMax = QDate::fromString( listVal[1] );
    }
    else if ( d->extra()->validity->m_allow == Allow_Time )
    {
        d->extra()->validity->timeMin = QTime::fromString( listVal[0] );
        d->extra()->validity->timeMax = QTime::fromString( listVal[1] );
    }
    else
    {
        d->extra()->validity->valMin = listVal[0].toDouble(&ok);
        if ( !ok )
        {
            d->extra()->validity->valMin = listVal[0].toInt(&ok);
            if ( !ok )
                kdDebug()<<" Try to parse this value :"<<listVal[0]<<endl;

#if 0
            if ( !ok )
                d->extra()->validity->valMin = listVal[0];
#endif
        }
        ok=false;
        d->extra()->validity->valMax = listVal[1].toDouble(&ok);
        if ( !ok )
        {
            d->extra()->validity->valMax = listVal[1].toInt(&ok);
            if ( !ok )
                kdDebug()<<" Try to parse this value :"<<listVal[1]<<endl;

#if 0
            if ( !ok )
                d->extra()->validity->valMax = listVal[1];
#endif
        }
    }
}

void KSpreadCell::loadOasisValidationCondition( QString &valExpression )
{
    QString value;
    if (valExpression.find( "<=" )==0 )
    {
        value = valExpression.remove( 0,2 );
        d->extra()->validity->m_cond = InferiorEqual;
    }
    else if (valExpression.find( ">=" )==0 )
    {
        value = valExpression.remove( 0,2 );
        d->extra()->validity->m_cond = SuperiorEqual;
    }
    else if (valExpression.find( "!=" )==0 )
    {
        //add Differentto attribute
        value = valExpression.remove( 0,2 );
        d->extra()->validity->m_cond = DifferentTo;
    }
    else if ( valExpression.find( "<" )==0 )
    {
        value = valExpression.remove( 0,1 );
        d->extra()->validity->m_cond = Inferior;
    }
    else if(valExpression.find( ">" )==0 )
    {
        value = valExpression.remove( 0,1 );
        d->extra()->validity->m_cond = Superior;
    }
    else if (valExpression.find( "=" )==0 )
    {
        value = valExpression.remove( 0,1 );
        d->extra()->validity->m_cond = Equal;
    }
    else
        kdDebug()<<" I don't know how to parse it :"<<valExpression<<endl;
    kdDebug()<<" value :"<<value<<endl;
    if ( d->extra()->validity->m_allow == Allow_Date )
    {
        d->extra()->validity->dateMin = QDate::fromString( value );
    }
    else if (d->extra()->validity->m_allow == Allow_Date )
    {
        d->extra()->validity->timeMin = QTime::fromString( value );
    }
    else
    {
        bool ok = false;
        d->extra()->validity->valMin = value.toDouble(&ok);
        if ( !ok )
        {
            d->extra()->validity->valMin = value.toInt(&ok);
            if ( !ok )
                kdDebug()<<" Try to parse this value :"<<value<<endl;

#if 0
            if ( !ok )
                d->extra()->validity->valMin = value;
#endif
        }
    }
}


bool KSpreadCell::load( const QDomElement & cell, int _xshift, int _yshift,
                        PasteMode pm, Operation op, bool paste )
{
    bool ok;

    //
    // First of all determine in which row and column this
    // cell belongs.
    //
    d->row = cell.attribute( "row" ).toInt( &ok ) + _yshift;
    if ( !ok ) return false;
    d->column = cell.attribute( "column" ).toInt( &ok ) + _xshift;
    if ( !ok ) return false;

    // Validation
    if ( d->row < 1 || d->row > KS_rowMax )
    {
        kdDebug(36001) << "KSpreadCell::load: Value out of Range Cell:row=" << d->row << endl;
        return false;
    }
    if ( d->column < 1 || d->column > KS_colMax )
    {
        kdDebug(36001) << "KSpreadCell::load: Value out of Range Cell:column=" << d->column << endl;
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
            if (i || d->hasExtra())
              d->extra()->extraXCells = i;
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
            if (i || d->hasExtra())
              d->extra()->extraYCells = i;
            if ( i > 0 )
            {
              setFlag(Flag_ForceExtra);
            }
        }

        if ( testFlag( Flag_ForceExtra ) )
        {
            if (d->hasExtra())
              forceExtraCells( d->column, d->row, d->extra()->extraXCells, d->extra()->extraYCells );
        }

    }

    //
    // Load the condition section of a cell.
    //
    QDomElement conditionsElement = cell.namedItem( "condition" ).toElement();
    if ( !conditionsElement.isNull())
    {
      if (d->hasExtra())
        delete d->extra()->conditions;
      d->extra()->conditions = new KSpreadConditions( this );
      d->extra()->conditions->loadConditions( conditionsElement );
      d->extra()->conditions->checkMatches();
    }

    QDomElement validity = cell.namedItem( "validity" ).toElement();
    if ( !validity.isNull())
    {
        QDomElement param = validity.namedItem( "param" ).toElement();
        if(!param.isNull())
        {
          d->extra()->validity = new KSpreadValidity;
          if ( param.hasAttribute( "cond" ) )
          {
            d->extra()->validity->m_cond = (Conditional) param.attribute("cond").toInt( &ok );
            if ( !ok )
              return false;
          }
          if ( param.hasAttribute( "action" ) )
          {
            d->extra()->validity->m_action = (Action) param.attribute("action").toInt( &ok );
            if ( !ok )
              return false;
          }
          if ( param.hasAttribute( "allow" ) )
          {
            d->extra()->validity->m_allow = (Allow) param.attribute("allow").toInt( &ok );
            if ( !ok )
              return false;
          }
          if ( param.hasAttribute( "valmin" ) )
          {
            d->extra()->validity->valMin = param.attribute("valmin").toDouble( &ok );
            if ( !ok )
              return false;
          }
          if ( param.hasAttribute( "valmax" ) )
          {
            d->extra()->validity->valMax = param.attribute("valmax").toDouble( &ok );
            if ( !ok )
              return false;
          }
          if ( param.hasAttribute( "displaymessage" ) )
          {
              d->extra()->validity->displayMessage = ( bool )param.attribute("displaymessage").toInt();
          }
        }
        QDomElement title = validity.namedItem( "title" ).toElement();
        if (!title.isNull())
        {
            d->extra()->validity->title = title.text();
        }
        QDomElement message = validity.namedItem( "message" ).toElement();
        if (!message.isNull())
        {
            d->extra()->validity->message = message.text();
        }
        QDomElement timeMin = validity.namedItem( "timemin" ).toElement();
        if ( !timeMin.isNull()  )
        {
            d->extra()->validity->timeMin = toTime(timeMin);
        }
        QDomElement timeMax = validity.namedItem( "timemax" ).toElement();
        if ( !timeMax.isNull()  )
        {
            d->extra()->validity->timeMax = toTime(timeMax);
         }
        QDomElement dateMin = validity.namedItem( "datemin" ).toElement();
        if ( !dateMin.isNull()  )
        {
            d->extra()->validity->dateMin = toDate(dateMin);
         }
        QDomElement dateMax = validity.namedItem( "datemax" ).toElement();
        if ( !dateMax.isNull()  )
        {
            d->extra()->validity->dateMax = toDate(dateMax);
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
          d->strText = result.text();
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
          d->strOutText = result.attribute( "outStr" );
          if ( !d->strOutText.isEmpty() )
            clearFlag( Flag_TextFormatDirty );
        }

        bool clear = true;
        // boolean ?
        if( dataType == "Bool" )
        {
          if ( t == "false" )
            setValue( true );
          else if ( t == "true" )
            setValue( false );
          else
            clear = false;
        }
        else if( dataType == "Num" )
        {
          bool ok = false;
          double dd = t.toDouble( &ok );
          if ( ok )
            setValue ( dd );
          else
            clear = false;
        }
        else if( dataType == "Date" )
        {
          bool ok = false;
          double dd = t.toDouble( &ok );
          if ( ok )
            setValue ( dd );
          else
          {
            int pos   = t.find( '/' );
            int year  = t.mid( 0, pos ).toInt();
            int pos1  = t.find( '/', pos + 1 );
            int month = t.mid( pos + 1, ( ( pos1 - 1 ) - pos ) ).toInt();
            int day   = t.right( t.length() - pos1 - 1 ).toInt();
            QDate date( year, month, day );
            if ( date.isValid() )
              setValue( date );
            else
              clear = false;
          }
        }
        else if( dataType == "Time" )
        {
          bool ok = false;
          double dd = t.toDouble( &ok );
          if ( ok )
            setValue( dd );
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
              setValue( time );
            else
              clear = false;
          }
        }
        else
        {
          setValue( t );
        }

        if ( clear )
          clearFlag( Flag_CalcDirty );
      }
    }

    return true;
}

bool KSpreadCell::loadCellData(const QDomElement & text, Operation op )
{
  QString t = text.text();
  t = t.stripWhiteSpace();

  setFlag(Flag_LayoutDirty);
  setFlag(Flag_TextFormatDirty);

  // A formula like =A1+A2 ?
  if( t[0] == '=' )
  {
    t = decodeFormula( t, d->column, d->row );
    d->strText = pasteOperation( t, d->strText, op );

    setFlag(Flag_CalcDirty);
    clearAllErrors();
    d->content = Formula;

    if ( !m_pTable->isLoading() ) // i.e. when pasting
      if ( !makeFormula() )
        kdError(36001) << "ERROR: Syntax ERROR" << endl;
  }
  // rich text ?
  else if (t[0] == '!' )
  {
      d->extra()->QML = new QSimpleRichText( t.mid(1),  QApplication::font() );//, m_pTable->widget() );
      d->strText = t;
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
      if (isDate() && ( t.contains('/') == 2 ))
        dataType = "Date";
      else if (isTime() && ( t.contains(':') == 2 ) )
        dataType = "Time";
      else
      {
        d->strText = pasteOperation( t, d->strText, op );
        checkTextInput();
        //kdDebug(36001) << "KSpreadCell::load called checkTextInput, got dataType=" << dataType << "  t=" << t << endl;
        newStyleLoading = false;
      }
    }

    if ( newStyleLoading )
    {
      d->value = KSpreadValue::empty();
      clearAllErrors();

      // boolean ?
      if( dataType == "Bool" )
      {
        if ( t == "false" )
          setValue( true );
        else if ( t == "true" )
          setValue( false );
        else
          kdWarning() << "Cell with BoolData, should be true or false: " << t << endl;
      }

      // number ?
      else if( dataType == "Num" )
      {
        bool ok = false;
        setValue ( KSpreadValue( t.toDouble(&ok) ) ); // We save in non-localized format
        if ( !ok )
  {
          kdWarning(36001) << "Couldn't parse '" << t << "' as number." << endl;
  }
  /* We will need to localize the text version of the number */
  KLocale* locale = m_pTable->doc()->locale();

        /* KLocale::formatNumber requires the precision we want to return.
        */
        int precision = t.length() - t.find('.') - 1;

  if ( formatType() == Percentage_format )
        {
          setFactor( 100.0 ); // should have been already done by loadFormat
          t = locale->formatNumber( value().asFloat() * 100.0, precision );
    d->strText = pasteOperation( t, d->strText, op );
          d->strText += '%';
        }
        else
  {
          t = locale->formatNumber(value().asFloat(), precision);
    d->strText = pasteOperation( t, d->strText, op );
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
        setValue( QDate(year,month,day) );
        if ( value().asDate().isValid() ) // Should always be the case for new docs
          d->strText = locale()->formatDate( value().asDate(), true );
        else // This happens with old docs, when format is set wrongly to date
        {
          d->strText = pasteOperation( t, d->strText, op );
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
        setValue( QTime(hours,minutes,second) );
        if ( value().asTime().isValid() ) // Should always be the case for new docs
          d->strText = locale()->formatTime( value().asTime(), true );
        else  // This happens with old docs, when format is set wrongly to time
        {
          d->strText = pasteOperation( t, d->strText, op );
          checkTextInput();
        }
      }

      else
      {
        // Set the cell's text
        d->strText = pasteOperation( t, d->strText, op );
        setValue( d->strText );
      }
    }
  }

  if ( text.hasAttribute( "outStr" ) ) // very new docs
  {
    d->strOutText = text.attribute( "outStr" );
    if ( !d->strOutText.isEmpty() )
      clearFlag( Flag_TextFormatDirty );
  }

  if ( !m_pTable->isLoading() )
    setCellText( d->strText );

  if ( d->hasExtra() && d->extra()->conditions )
    d->extra()->conditions->checkMatches();

  return true;
}

QTime KSpreadCell::toTime(const QDomElement &element)
{
    //TODO: can't we use tryParseTime (after modification) instead?
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
    setValue( KSpreadValue( QTime(hours,minutes,second)) );
    return value().asTime();
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
    setValue( KSpreadValue( QDate(year,month,day) ) );
    return value().asDate();
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
        d->content = Text;

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

        tmp_op = decodeFormula( tmp_op, d->column, d->row );
        setFlag(Flag_LayoutDirty);
        clearAllErrors();
        d->content = Formula;

        return tmp_op;
    }

    tmp = decodeFormula( new_text, d->column, d->row );
    setFlag(Flag_LayoutDirty);
    clearAllErrors();
    d->content = Formula;

    return tmp;
}

QString KSpreadCell::testAnchor( int _x, int _y ) const
{
  if ( !d->hasExtra() || !d->extra()->QML )
    return QString::null;

  return d->extra()->QML->anchorAt( QPoint( _x, _y ) );
}

void KSpreadCell::tableDies()
{
    // Avoid unobscuring the cells in the destructor.
    if (d->hasExtra())
    {
      d->extra()->extraXCells = 0;
      d->extra()->extraYCells = 0;
      d->extra()->mergedXCells = 0;
      d->extra()->mergedYCells = 0;
    }
    d->nextCell = 0;
    d->previousCell = 0;
}

KSpreadCell::~KSpreadCell()
{
    if ( d->nextCell )
        d->nextCell->setPreviousCell( d->previousCell );
    if ( d->previousCell )
        d->previousCell->setNextCell( d->nextCell );

    if (d->hasExtra())
    {
      delete d->extra()->QML;
      delete d->extra()->validity;
    }
    delete d->code;

    // Unobscure cells.
    int extraXCells = d->hasExtra() ? d->extra()->extraXCells : 0;
    int extraYCells = d->hasExtra() ? d->extra()->extraYCells : 0;
    for( int x = 0; x <= extraXCells; ++x )
        for( int y = (x == 0) ? 1 : 0; // avoid looking at (+0,+0)
             y <= extraYCells; ++y )
    {
        KSpreadCell* cell = m_pTable->cellAt( d->column + x, d->row + y );
        if ( cell )
            cell->unobscure(this);
    }

    d->value = KSpreadValue::empty();
    
    if (!isDefault())
      valueChanged ();  //our value has been changed (is now null), but only if we aren't default

    delete d;
}

bool KSpreadCell::operator > ( const KSpreadCell & cell ) const
{
  if ( value().isNumber() ) // ### what about bools ?
  {
    if ( cell.value().isNumber() )
      return value().asFloat() > cell.value().asFloat();
    else
      return false; // numbers are always < than texts
  }
  else if(isDate())
  {
     if( cell.isDate() )
        return value().asDate() > cell.value().asDate();
     else if (cell.value().isNumber())
        return true;
     else
        return false; //date are always < than texts and time
  }
  else if(isTime())
  {
     if( cell.isTime() )
        return value().asTime() > cell.value().asTime();
     else if( cell.isDate())
        return true; //time are always > than date
     else if( cell.value().isNumber())
        return true;
     else
        return false; //time are always < than texts
  }
  else
  {
      if ( KSpreadMap::respectCase )
          return value().asString().compare(cell.value().asString()) > 0;
      else
          return ( value().asString() ).lower().compare(cell.value().asString().lower()) > 0;
  }
}

bool KSpreadCell::operator < ( const KSpreadCell & cell ) const
{
  if ( value().isNumber() )
  {
    if ( cell.value().isNumber() )
      return value().asFloat() < cell.value().asFloat();
    else
      return true; // numbers are always < than texts
  }
  else if(isDate())
  {
     if( cell.isDate() )
        return value().asDateTime().date() < cell.value().asDateTime().date();
     else if( cell.value().isNumber())
        return false;
     else
        return true; //date are always < than texts and time
  }
  else if(isTime())
  {
     if( cell.isTime() )
        return value().asDateTime().time() < cell.value().asDateTime().time();
     else if(cell.isDate())
        return false; //time are always > than date
     else if( cell.value().isNumber())
        return false;
     else
        return true; //time are always < than texts
  }
  else
  {
      if ( KSpreadMap::respectCase )
          return value().asString().compare(cell.value().asString()) < 0;
      else
          return ( value().asString() ).lower().compare(cell.value().asString().lower()) < 0;
  }
}

QRect KSpreadCell::cellRect()
{
  Q_ASSERT(!isDefault());
  return QRect(QPoint(d->column, d->row), QPoint(d->column, d->row));
}

QValueList<KSpreadConditional> KSpreadCell::conditionList() const
{
  if ( !d->hasExtra() || !d->extra()->conditions )
  {
    QValueList<KSpreadConditional> emptyList;
    return emptyList;
  }

  return d->extra()->conditions->conditionList();
}

void KSpreadCell::setConditionList( const QValueList<KSpreadConditional> & newList )
{
  if (d->hasExtra())
    delete d->extra()->conditions;
  d->extra()->conditions = new KSpreadConditions( this );
  d->extra()->conditions->setConditionList( newList );
  d->extra()->conditions->checkMatches();
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
  return ( (d->content == Formula) ? testFlag( Flag_CalcDirty ) : false );
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


void KSpreadCell::checkForNamedAreas( QString & formula ) const
{
  int l = formula.length();
  int i = 0;
  QString word;
  int start = 0;
  while ( i < l )
  {
    if ( formula[i].isLetterOrNumber() )
    {
      word += formula[i];
      ++i;
      continue;
    }
    if ( !word.isEmpty() )
    {
      if ( table()->doc()->loadingInfo()->findWordInAreaList(word) )
      {
        formula = formula.replace( start, word.length(), "'" + word + "'" );
        l = formula.length();
        ++i;
        kdDebug() << "Formula: " << formula << ", L: " << l << ", i: " << i + 1 <<endl;
      }
    }

    ++i;
    word = "";
    start = i;
  }
  if ( !word.isEmpty() )
  {
    if ( table()->doc()->loadingInfo()->findWordInAreaList(word) )
    {
      formula = formula.replace( start, word.length(), "'" + word + "'" );
      l = formula.length();
      ++i;
      kdDebug() << "Formula: " << formula << ", L: " << l << ", i: " << i + 1 <<endl;
    }
  }
}

void KSpreadCell::convertFormula( QString & text, const QString & f ) const
{
  kdDebug() << "Parsing formula: " << f << endl;

  QString formula;
  QString parameter;

  int l = f.length();
  int p = 0;

  while ( p < l )
  {
    if ( f[p] == '(' )
    {
      break;
    }
    else if ( f[p] == '[' )
      break;

    formula += f[p];
    ++p;
  }

  if ( parameter.isEmpty() )
  {
    checkForNamedAreas( formula );
  }

  kdDebug() << "Formula: " << formula << ", Parameter: " << parameter << ", P: " << p << endl;


  // replace formula names here
  if ( formula == "=MULTIPLE.OPERATIONS" )
    formula = "=MULTIPLEOPERATIONS";

  QString par;
  bool isPar   = false;
  bool inQuote = false;

  while ( p < l )
  {
    if ( f[p] == '"' )
    {
      inQuote = !inQuote;
      parameter += '"';
    }
    else if ( f[p] == '[' )
    {
      if ( !inQuote )
        isPar = true;
      else
        parameter += '[';
    }
    else if ( f[p] == ']' )
    {
      if ( inQuote )
      {
        parameter += ']';
        continue;
      }

      isPar = false;
      parameter +=  KSpreadSheet::translateOpenCalcPoint( par );
      par = "";
    }
    else if ( isPar )
    {
      par += f[p];
    }
    else if ( f[p] == '=' ) // TODO: check if StarCalc has a '==' sometimes
    {
      if ( inQuote )
        parameter += '=';
      else
        parameter += "==";
    }
    else if ( f[p] == ')' )
    {
      if ( !inQuote )
        parameter += ")";
    }
    else if ( f[p] == '.' && f[p+1].isNumber() ) // Convert '.' to ',' in floating point numbers
      parameter += ',';
    else
      parameter += f[p];

    ++p;
    if ( p == l )
      checkForNamedAreas( parameter );
  }

  text = formula + parameter;
  kdDebug() << "New formula: " << text << endl;
}
