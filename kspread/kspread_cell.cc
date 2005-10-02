/* This file is part of the KDE project

   Copyright 2004-2005 Tomas Mecir <mecirt@gmail.com>
   Copyright 1999-2002,2004,2005 Laurent Montel <montel@kde.org>
   Copyright 2002-2005 Ariya Hidayat <ariya@kde.org>
   Copyright 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2003 Reinhart Geiser <geiseri@kde.org>
   Copyright 2003-2005 Meni Livne <livne@kde.org>
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <math.h>

#include <qapplication.h>
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
#include "kspread_locale.h"
#include "kspread_view.h"
#include "kspread_value.h"
#include "formula.h"
#include "valueformatter.h"
#include "valueparser.h"

#include <koxmlns.h>
#include <kodom.h>
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
//
// When the cell is getting complex (e.g. merged with other cells, contains
// rich text, has validation criteria, etc), this CellExtra is allocated by
// CellPrivate and starts to be available. Otherwise, it won't exist at all.

class CellExtra
{
public:

  // Not empty when the cell holds a link
  QString link;

  // Number of cells explicitly merged by the user in X and Y directions.
  int mergedXCells;
  int mergedYCells;

  // Number of additional cells.
  int extraXCells;
  int extraYCells;

  // If this cell overlaps other cells, then we have the cells width and
  // height stored here.  These values do not mean anything unless
  // extraXCells and/or extraYCells are different from 0.
  double extraWidth;
  double extraHeight;

  // A list of cells that obscure this one.
  // If this list is not empty, then this cell is obscured by another
  // enlarged object. This means that we have to call this object in order
  // of painting it for example instead of painting 'this'.
  //
  // FIXME (comment): If the list consists of more than one obscuring
  //                  element, then is there an order between them that
  //                  is important?
  QValueList<KSpreadCell*> obscuringCells;

  // If non-NULL, contains a pointer to a condition or a validity test.
  KSpreadConditions  *conditions;
  KSpreadValidity    *validity;

  // Store the number of line when you multirow is used (default is 0)
  int nbLines;


private:
  // Don't allow implicit copy.
  CellExtra& operator=( const CellExtra& );
};


class CellPrivate
{
public:

  CellPrivate();
  ~CellPrivate();

public:

  // This cell's row and column. If either of them is 0, this is the
  // default cell and its row/column can not be determined.  Note that
  // in the isDefault() method, only column is tested.
  int  row;
  int  column;

  // Value of the cell, either typed by user or as result of formula
  KSpreadValue value;

  // Holds the user's input.
  //
  // FIXME:
  // Eventually, we'll want to get rid of strText and generate
  // user's input on-the-fly. Then, for normal cells, we'll generate
  // this string using converter()->asString
  // (value()).
  //
  // Here the problem is, that strText also holds the formula -
  // we'll need to provide some method to generate it from the
  // parsed version, created in KSpread::Formula. Hence, we won't be
  // able to get rid of strText until we switch to the new formula
  // parser and until we write some method that re-generates the
  // input formula...
  //
  // Alternately, we can keep using strText for formulas and
  // generate it dynamically for static cells...
  //
  //  /Tomas
  //
  QString  strText;

  // This is the text we want to display. Not necessarily the same
  // as strText, e.g. strText="1" and strOutText="1.00" Also holds
  // value that we got from calculation, formerly known as
  // strFormulaOut
  QString  strOutText;

  // the Formula object for the cell
  KSpread::Formula *formula;

  // Position and dimension of displayed text.
  // FIXME (comment): Which coordinate system?  pixels?  mm/cm?  zoom?
  double  textX;
  double  textY;
  double  textWidth;
  double  textHeight;

  // result of "fm.ascent()" in makeLayout. used in offsetAlign.
  int  fmAscent;

  // Pointers to neighboring cells.
  // FIXME (comment): Which order?
  KSpreadCell  *nextCell;
  KSpreadCell  *previousCell;

  bool        hasExtra() const { return (cellExtra != 0); };
  CellExtra  *extra();

private:
  // "Extra stuff", see explanation for CellExtra.
  CellExtra  *cellExtra;
};


CellPrivate::CellPrivate()
{
  // Some basic data.
  row     = 0;
  column  = 0;
  value   = KSpreadValue::empty();
  formula = 0;

  // Formatting
  textX      = 0.0;
  textY      = 0.0;
  textWidth  = 0.0;
  textHeight = 0.0;
  fmAscent   = 0;

  nextCell     = 0;
  previousCell = 0;

  // Default is to not have the "extra" stuff in a cell.
  cellExtra = 0;
}


CellPrivate::~CellPrivate()
{
  delete cellExtra;
  delete formula;
}


CellExtra* CellPrivate::extra()
{
    if ( !cellExtra ) {
      cellExtra = new CellExtra;
      cellExtra->conditions   = 0;
      cellExtra->validity     = 0;

      cellExtra->mergedXCells = 0;
      cellExtra->mergedYCells = 0;
      cellExtra->extraXCells  = 0;
      cellExtra->extraYCells  = 0;
      cellExtra->extraWidth   = 0.0;
      cellExtra->extraHeight  = 0.0;
      cellExtra->nbLines      = 0;
//      cellExtra->highlight    = QColor(0,0,0);
    }

    return cellExtra;
}


/*****************************************************************************
 *
 *                                 KSpreadCell
 *
 *****************************************************************************/


KSpreadCell::KSpreadCell( KSpreadSheet * _sheet, int _column, int _row )
  : KSpreadFormat (_sheet, _sheet->doc()->styleManager()->defaultStyle())
{
  d = new CellPrivate;
  d->row = _row;
  d->column = _column;
  clearAllErrors();
}


KSpreadCell::KSpreadCell( KSpreadSheet * _sheet,
    KSpreadStyle * _style,  int _column, int _row )
  : KSpreadFormat( _sheet, _style )
{
  d = new CellPrivate;
  d->row = _row;
  d->column = _column;
  clearAllErrors();
}

// Return the sheet that this cell belongs to.
KSpreadSheet * KSpreadCell::sheet() const
{
  return m_pSheet;
}

// Return true if this is the default cell.
bool KSpreadCell::isDefault() const
{
  return ( d->column == 0 );
}

// Return the row number of this cell.
int KSpreadCell::row() const
{
  // Make sure this isn't called for the default cell.  This assert
  // can save you (could have saved me!) the hassle of some very
  // obscure bugs.

  if ( isDefault() )
  {
    kdWarning(36001) << "Error: Calling KSpreadCell::row() for default cell" << endl;
    return 0;
  }

  return d->row;
}


// Return the column number of this cell.
//
int KSpreadCell::column() const
{
  // Make sure this isn't called for the default cell.  This assert
  // can save you (could have saved me!) the hassle of some very
  // obscure bugs.
  if ( isDefault() )
  {
    kdWarning(36001) << "Error: Calling KSpreadCell::column() for default cell" << endl;
    return 0;
  }
  return d->column;
}


// Return the name of this cell, i.e. the string that the user would
// use to reference it.  Example: A1, BZ16
//
QString KSpreadCell::name() const
{
    return name( d->column, d->row );
}


// Return the name of any cell given by (col, row).
//
QString KSpreadCell::name( int col, int row )
{
    return columnName( col ) + QString::number( row );
}


// Return the name of this cell, including the sheet name.
// Example: sheet1!A5
//
QString KSpreadCell::fullName() const
{
    return fullName( sheet(), d->column, d->row );
}


// Return the full name of any cell given a sheet and (col, row).
//
QString KSpreadCell::fullName( const KSpreadSheet* s, int col, int row )
{
  return s->sheetName() + "!" + name( col, row );
}


// Return the symbolic name of the column of this cell.  Examples: A, BB.
//
QString KSpreadCell::columnName() const
{
  return columnName( d->column );
}

KLocale* KSpreadCell::locale()
{
  return m_pSheet->doc()->locale();
}

// Return the symbolic name of any column.
//
QString KSpreadCell::columnName( uint column )
{
    QString   str;
    unsigned  digits = 1;
    unsigned  offset = 0;

    column--;

    if( column > 4058115285U ) return  QString("@@@");

    for( unsigned limit = 26; column >= limit+offset; limit *= 26, digits++ )
        offset += limit;

    for( unsigned c = column - offset; digits; --digits, c/=26 )
        str.prepend( QChar( 'A' + (c%26) ) );

    return str;
}


// Return true if this cell is a formula.
//
bool KSpreadCell::isFormula() const
{
    return d->strText[0] == '=';
}


// Return the input text of this cell.  This could, for instance, be a
// formula.
//
// FIXME: These two functions are inconsistently named.  It should be
//        either text() and outText() or strText() and strOutText().
//
QString KSpreadCell::text() const
{
    return d->strText;
}


// Return the out text, i.e. the text that is visible in the cells
// square when shown.  This could, for instance, be the calculated
// result of a formula.
//
QString KSpreadCell::strOutText() const
{
    return d->strOutText;
}


// Return the value of this cell.
//
const KSpreadValue KSpreadCell::value() const
{
  return d->value;
}


// Set the value of this cell.  It also clears all errors if the value
// itself is not an error.
//
// In addition to this, it calculates the outstring and sets the dirty
// flags so that a redraw is forced.
//
void KSpreadCell::setValue( const KSpreadValue& v )
{
  if (v.type() != KSpreadValue::Error)
    clearAllErrors();

  d->value = v;

  setFlag(Flag_LayoutDirty);
  setFlag(Flag_TextFormatDirty);

  // Format and set the outText.
  setOutputText();

  // Set the displayed text, if we hold an error value.
  if (d->value.type() == KSpreadValue::Error)
    d->strOutText = d->value.errorMessage ();

  // Value of the cell has changed - trigger necessary actions
  valueChanged ();

  m_pSheet->setRegionPaintDirty(cellRect());
}

// FIXME: Continue commenting and cleaning here (ingwa)


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
    const KSpreadCell * cell = m_pSheet->cellAt( _column, _row );

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
      KSpreadFormat::setCurrency( c );

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
  return sheet()->rowFormat( row );
}

const KSpreadFormat * KSpreadCell::fallbackFormat( int, int row ) const
{
  return sheet()->rowFormat( row );
}


// Make this cell obscure a number of other cells.

void KSpreadCell::forceExtraCells( int _col, int _row, int _x, int _y )
{
  // Start by unobscuring the cells that we obscure right now
  int  extraXCells = d->hasExtra() ? d->extra()->extraXCells : 0;
  int  extraYCells = d->hasExtra() ? d->extra()->extraYCells : 0;
  for ( int x = _col; x <= _col + extraXCells; ++x )
    for ( int y = _row; y <= _row + extraYCells; ++y ) {
      if ( x != _col || y != _row )
        m_pSheet->nonDefaultCell( x, y )->unobscure(this);
    }

  // If no forcing, then remove all traces, and return.
  if ( _x == 0 && _y == 0 ) {
    clearFlag( Flag_ForceExtra );
    if (d->hasExtra()) {
      d->extra()->extraXCells  = 0;
      d->extra()->extraYCells  = 0;
      d->extra()->extraWidth   = 0.0;
      d->extra()->extraHeight  = 0.0;
      d->extra()->mergedXCells = 0;
      d->extra()->mergedYCells = 0;
    }

    // Refresh the layout
    setFlag( Flag_LayoutDirty );
    return;
  }

  // At this point, we know that we will force some extra cells.
  setFlag(Flag_ForceExtra);
  d->extra()->extraXCells  = _x;
  d->extra()->extraYCells  = _y;
  d->extra()->mergedXCells = _x;
  d->extra()->mergedYCells = _y;

  // Obscure the cells
  for ( int x = _col; x <= _col + _x; ++x )
    for ( int y = _row; y <= _row + _y; ++y ) {
      if ( x != _col || y != _row )
	m_pSheet->nonDefaultCell( x, y )->obscure( this, true );
    }

  // Refresh the layout
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
                KSpreadCell *cell = m_pSheet->nonDefaultCell( x, y );
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
        return false;

    if ( !d->strText.isEmpty() ) {
	//kdWarning(36001) << name() << ": not empty - needs printing" << endl;
        return true;
    }

    // Cell borders?
    if ( hasProperty( PTopBorder ) || hasProperty( PLeftBorder ) ||
         hasProperty( PRightBorder ) || hasProperty( PBottomBorder ) ||
         hasProperty( PFallDiagonal ) || hasProperty( PGoUpDiagonal ) ) {
	//kdDebug(36001) << name()
	//		 << ": has border property - needs printing" << endl;
	return true;
    }

    // Background color or brush?
    if ( hasProperty( PBackgroundBrush ) ) {
	//kdDebug(36001) << name()
	//		 << ": has brush property - needs printing" << endl;
        return true;
    }

    if ( hasProperty( PBackgroundColor ) ) {
	//kdDebug(36001) << name()
	//		 << ": has backgroundColor property - needs printing"
	//		 << endl;
	return true;
    }

    return false;
}

bool KSpreadCell::isEmpty() const
{
    return isDefault() || d->strText.isEmpty();
}


// Return true if this cell is obscured by some other cell.

bool KSpreadCell::isObscured() const
{
  if (!d->hasExtra())
    return false;

  return !( d->extra()->obscuringCells.isEmpty() );
}


// Return true if this cell is part of a merged cell ("forced
// obscuring"), but not the master cell.
//
// FIXME: Better name!

bool KSpreadCell::isObscuringForced() const
{
  if (!d->hasExtra())
    return false;

  QValueList<KSpreadCell*>::const_iterator it = d->extra()->obscuringCells.begin();
  QValueList<KSpreadCell*>::const_iterator end = d->extra()->obscuringCells.end();
  for ( ; it != end; ++it ) {
    KSpreadCell *cell = *it;

    if (cell->isForceExtraCells()) {
      // The cell might force extra cells, and then overlap even
      // beyond that so just knowing that the obscuring cell forces
      // extra isn't enough.  We have to know that this cell is one of
      // the ones it is forcing over.
      if (column() <= cell->column() + cell->d->extra()->mergedXCells
	  && row() <= cell->row() + cell->mergedYCells() )
	return true;
    }
  }

  return false;
}


// Return the cell that obscures this one.  If no cell is obscuring,
// then return this.  This method is slightly complicated because we
// can have several layers of obscuring.
//
// Update: it seems that if we do an actual merge, then the obscuring
// cell is prepended and if just expanding, then it is appended.  This
// means that we should be able to just look at the first one.

KSpreadCell *KSpreadCell::ultimateObscuringCell() const
{
  if (!d->hasExtra())
    return (KSpreadCell *) this;

  else if (d->extra()->obscuringCells.isEmpty())
    return (KSpreadCell *) this;

  else
    return d->extra()->obscuringCells.first();

#if 0
  QValueList<KSpreadCell*>::const_iterator it = d->extra()->obscuringCells.begin();
  QValueList<KSpreadCell*>::const_iterator end = d->extra()->obscuringCells.end();
  for ( ; it != end; ++it ) {
    KSpreadCell *cell = *it;

    if (cell->isForceExtraCells()) {
      // The cell might force extra cells, and then overlap even
      // beyond that so just knowing that the obscuring cell forces
      // extra isn't enough.  We have to know that this cell is one of
      // the ones it is forcing over.
      if (column() <= cell->column() + cell->d->extra()->mergedXCells
	  && row() <= cell->row() + cell->mergedYCells() )
	return true;
    }
  }

  return false;
#endif
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
  m_pSheet->setRegionPaintDirty( cellRect() );
}

void KSpreadCell::unobscure( KSpreadCell * cell )
{
  if (d->hasExtra())
    d->extra()->obscuringCells.remove( cell );
  setFlag( Flag_LayoutDirty );
  m_pSheet->setRegionPaintDirty( cellRect() );
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

    bool fix1 = false;
    bool fix2 = false;
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
          fix1 = fix2 = false;
          onNumber = true;
        }
        else if ( d->strText[pos] != '$' && !d->strText[pos].isLetter() )
        {
            erg += d->strText[pos++];
            fix1 = fix2 = false;
            onNumber = false;
        }
        else
        {
            QString tmp = "";
            if ( d->strText[pos] == '$' )
            {
                tmp = "$";
                pos++;
                fix1 = true;
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
                    fix2 = true;
                }
                if ( d->strText[pos].isDigit() )
                {
                    const unsigned int oldPos = pos;
                    while ( pos < length && d->strText[pos].isDigit() ) ++pos;
                    int row = 0;
                    if ( pos != oldPos )
                        row = d->strText.mid(oldPos, pos-oldPos).toInt();
                    // Is it a sheet name || is it a function name like DEC2HEX
                    /* or if we're parsing a number, this could just be the
                       exponential part of it  (1.23E4) */
                    if ( ( d->strText[pos] == '!' ) ||
                         d->strText[pos].isLetter() ||
                         onNumber )
                    {
                        erg += tmp;
                        fix1 = fix2 = false;
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
                                erg += QChar(0xA7) + QString( "%1" ).arg( col );
                            else
                                erg += QString( "#%1" ).arg( col - _col );

                        if ( fix2 )
                            erg += QString( "$%1#").arg( row );
                        else
                            if (_era)
                                erg += QChar(0xA7) + QString( "%1#" ).arg( row );
                            else
                                erg += QString( "#%1#" ).arg( row - _row );
                    }
                }
                else
                {
                    erg += tmp;
                    fix1 = fix2 = false;
                }
            }
            else
            {
                erg += tmp;
                fix1 = false;
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
        else if ( _text[pos] == '#' || _text[pos] == '$' || _text[pos] == QChar(0xA7))
        {
            bool abs1 = false;
            bool abs2 = false;
            bool era1 = false; // if 1st is relative but encoded absolutely
            bool era2 = false;

            QChar _t = _text[pos++];
            if ( _t == '$' )
                abs1 = true;
            else if ( _t == QChar(0xA7) )
                era1 = true;

            int col = 0;
            unsigned int oldPos = pos;
            while ( pos < length && ( _text[pos].isDigit() || _text[pos] == '-' ) ) ++pos;
            if ( pos != oldPos )
                col = _text.mid(oldPos, pos-oldPos).toInt();
            if ( !abs1 && !era1 )
                col += _col;
            // Skip '#' or '$'

            _t = _text[pos++];
            if ( _t == '$' )
                 abs2 = true;
            else if ( _t == QChar(0xA7) )
                 era2 = true;

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

  for ( int x = d->column + d->extra()->mergedXCells;
	x <= d->column + d->extra()->extraXCells; ++x ) {
    for ( int y = d->row + d->extra()->mergedYCells;
	  y <= d->row + d->extra()->extraYCells; ++y ) {
      if ( x != d->column || y != d->row ) {
        KSpreadCell *cell = m_pSheet->cellAt( x, y );
        cell->unobscure(this);
      }
    }
  }

  d->extra()->extraXCells = d->extra()->mergedXCells;
  d->extra()->extraYCells = d->extra()->mergedYCells;

}


// ----------------------------------------------------------------
//                              Layout


// Recalculate the entire layout.  This includes the following members:
//
//   d->textX,     d->textY
//   d->textWidth, d->textHeight
//   d->fmAscent
//   d->extra()->extraXCells, d->extra()->extraYCells
//   d->extra()->extraWidth,  d->extra()->extraHeight
//   d->extra()->nbLines (if multirow)
//
// and, of course,
//
//   d->strOutText
//

void KSpreadCell::makeLayout( QPainter &_painter, int _col, int _row )
{
  // Are _col and _row really needed ?
  //
  // Yes they are: they are useful if this is the default layout, in
  // which case d->row and d->column are 0 and 0, but _col and _row
  // are the real coordinates of the cell.

  // There is no need to remake the layout if it hasn't changed.
  if ( !testFlag( Flag_LayoutDirty ) )
    return;

  // Some initializations.
  if (d->hasExtra())
    d->extra()->nbLines = 0;
  clearFlag( Flag_CellTooShortX );
  clearFlag( Flag_CellTooShortY );

  // Initiate the cells that this one is obscuring to the ones that
  // are actually merged.
  freeAllObscuredCells();
  if (d->hasExtra())
    forceExtraCells( d->column, d->row,
		     d->extra()->mergedXCells, d->extra()->mergedYCells );

  // If the column for this cell is hidden or the row is too low,
  // there is no use in remaking the layout.
  ColumnFormat  *cl1 = m_pSheet->columnFormat( _col );
  RowFormat     *rl1 = m_pSheet->rowFormat( _row );
  if ( cl1->isHide()
       || ( rl1->dblHeight() <= m_pSheet->doc()->unzoomItY( 2 ) ) ) {
      clearFlag( Flag_LayoutDirty );
      return;
  }

  // Recalculate the output text, d->strOutText.
  setOutputText();

  // Empty text?  Reset the outstring and, if this is the default
  // cell, return.
  if ( d->strOutText.isEmpty() ) {
    d->strOutText = QString::null;

    if ( isDefault() ) {
      clearFlag( Flag_LayoutDirty );
      return;
    }
  }

  // Up to here, we have just cared about the contents, not the
  // painting of it.  Now it is time to see if the contents fits into
  // the cell and, if not, maybe rearrange the outtext a bit.
  //
  // First, Determine the correct font with zoom taken into account,
  // and apply it to _painter.  Then calculate text dimensions, i.e.
  // d->textWidth and d->textHeight.
  applyZoomedFont( _painter, _col, _row );
  textSize( _painter );

  //
  // Calculate the size of the cell
  //
  RowFormat     *rl = m_pSheet->rowFormat( d->row );
  ColumnFormat  *cl = m_pSheet->columnFormat( d->column );

  double         width  = cl->dblWidth();
  double         height = rl->dblHeight();

  // Calculate extraWidth and extraHeight if we have a merged cell.
  if ( testFlag( Flag_ForceExtra ) ) {
    int  extraXCells = d->hasExtra() ? d->extra()->extraXCells : 0;
    int  extraYCells = d->hasExtra() ? d->extra()->extraYCells : 0;

    // FIXME: Introduce double extraWidth/Height here and use them
    //        instead (see FIXME about this in paintCell()).

    for ( int x = _col + 1; x <= _col + extraXCells; x++ )
      width += m_pSheet->columnFormat( x )->dblWidth();

    for ( int y = _row + 1; y <= _row + extraYCells; y++ )
      height += m_pSheet->rowFormat( y )->dblHeight();
  }

  // Cache the newly calculated extraWidth and extraHeight if we have
  // already allocated a struct for it.  Otherwise it will be zero, so
  // don't bother.
  if (d->hasExtra()) {
    d->extra()->extraWidth  = width;
    d->extra()->extraHeight = height;
  }

  QFontMetrics  fm = _painter.fontMetrics();
  d->fmAscent = fm.ascent();

  // Check if we need to break the line into multiple lines and are
  // allowed to do so.  If so, set `lines' to the number of lines that
  // are needed to fit into the total width of the combined cell.
  //
  // Also recalculate d->textHeight, d->textWidth, d->extra->nbLines
  // and d->strOutText.
  //
  int  lines = 1;
  if ( d->textWidth > (width - 2 * BORDER_SPACE
		       - leftBorderWidth( _col, _row )
		       - rightBorderWidth( _col, _row ) )
       && multiRow( _col, _row ) )
  {
    // Copy of d->strOutText but without the newlines.
    QString  o = d->strOutText.replace( QChar('\n'), " " );

    // Break the line at appropriate places, i.e. spaces, if
    // necessary.  This means to change the spaces where breaks occur
    // into newlines.
    if ( o.find(' ') != -1 ) {
      d->strOutText = "";

      // Make sure that we have a space at the end.
      o += ' ';

      int start = 0;		// Start of the line we are handling now
      int breakpos = 0;		// The next candidate pos to break the string
      int pos1 = 0;
      int availableWidth = (int) ( width - 2 * BORDER_SPACE
			     - leftBorderWidth( _col, _row )
			     - rightBorderWidth( _col, _row ) );

      do {
	breakpos = o.find( ' ', breakpos );
	double lineWidth = m_pSheet->doc()
	  ->unzoomItX( fm.width( d->strOutText.mid( start, (pos1 - start) )
				 + o.mid( pos1, breakpos - pos1 ) ) );

	if ( lineWidth <= availableWidth ) {
	  // We have room for the rest of the line.  End it here.
	  d->strOutText += o.mid( pos1, breakpos - pos1 );
	  pos1 = breakpos;
	}
	else {
	  // Still not enough room.  Try to split further.
	  if ( o.at( pos1 ) == ' ' )
	    pos1++;

	  if ( pos1 != 0 && breakpos != -1 ) {
	    d->strOutText += "\n" + o.mid( pos1, breakpos - pos1 );
	    lines++;
	  }
	  else
	    d->strOutText += o.mid( pos1, breakpos - pos1 );

	  start = pos1;
	  pos1 = breakpos;
	}

	breakpos++;
      } while( o.find( ' ', breakpos ) != -1 );
    }

    d->textHeight *= lines;
    if (lines > 1)
      d->extra()->nbLines = lines;

    d->textX = 0.0;

    // Calculate the maximum width, taking into account linebreaks,
    // and put it in d->textWidth.
    QString  t;
    int      i;
    int      pos = 0;
    d->textWidth = 0.0;
    do {
      i = d->strOutText.find( "\n", pos );

      if ( i == -1 )
	t = d->strOutText.mid( pos, d->strOutText.length() - pos );
      else {
	t = d->strOutText.mid( pos, i - pos );
	pos = i + 1;
      }

      double  tw = m_pSheet->doc()->unzoomItX( fm.width( t ) );
      if ( tw > d->textWidth )
	d->textWidth = tw;
    } while ( i != -1 );
  }

  // Calculate d->textX and d->textY
  offsetAlign( _col, _row );

  int a = effAlignX();

  // Get indentation.  This is only used for left aligned text.
  double indent = 0.0;
  if ( a == KSpreadCell::Left && !isEmpty() )
    indent = getIndent( _col, _row );

  // Set Flag_CellTooShortX if the text is vertical or angled, and too
  // high for the cell.
  if ( verticalText( _col, _row ) || getAngle( _col, _row ) != 0 ) {
    RowFormat  *rl = m_pSheet->rowFormat( _row );

    if ( d->textHeight >= rl->dblHeight() )
      setFlag( Flag_CellTooShortX );
  }

  // Do we have to occupy additional cells to the right?  This is only
  // done for cells that have no merged cells in the Y direction.
  //
  // FIXME: Check if all cells along the merged edge to the right are
  //        empty and use the extra space?  No, probably not.
  //
  if ( d->textWidth + indent > ( width - 2 * BORDER_SPACE
				 - leftBorderWidth( _col, _row )
				 - rightBorderWidth( _col, _row ) )
       && ( !d->hasExtra() || d->extra()->mergedYCells == 0 ) )
  {
    int c = d->column;

    // Find free cells to the right of this one.
    int end = 0;
    while ( !end ) {
      ColumnFormat  *cl2  = m_pSheet->columnFormat( c + 1 );
      KSpreadCell   *cell = m_pSheet->visibleCellAt( c + 1, d->row );

      if ( cell->isEmpty() ) {
	width += cl2->dblWidth() - 1;
	c++;

	// Enough space?
	if ( d->textWidth + indent <= ( width - 2 * BORDER_SPACE
					- leftBorderWidth( _col, _row )
					- rightBorderWidth( _col, _row ) ) )
	  end = 1;
      }
      else
	// Not enough space, but the next cell is not empty
	end = -1;
    }

    // Try to use additional space from the neighboring cells that
    // were calculated in the last step.  This is the place that we
    // set d->extra()->extraXCells and d->extra()->extraWidth.
    //
    // Currently this is only done for left aligned cells. We have to
    // check to make sure we haven't already force-merged enough cells
    //
    // FIXME: Why not right/center aligned text?
    //
    // FIXME: Shouldn't we check to see if end == -1 here before
    //        setting Flag_CellTooShortX?
    //
    if ( align( _col, _row ) == KSpreadCell::Left
	 || ( align( _col, _row ) == KSpreadCell::Undefined
	      && !value().isNumber() ) )
    {
      if ( c - d->column > d->extra()->mergedXCells ) {
	d->extra()->extraXCells = c - d->column;
	d->extra()->extraWidth  = width;
	for ( int i = d->column + 1; i <= c; ++i ) {
	  KSpreadCell *cell = m_pSheet->nonDefaultCell( i, d->row );
	  cell->obscure( this );
	}

	// Not enough space
	if ( end == -1 )
	  setFlag( Flag_CellTooShortX );
      }
      else
	setFlag( Flag_CellTooShortX );
    }
    else
      setFlag( Flag_CellTooShortX );
  }

  // Do we have to occupy additional cells at the bottom ?
  //
  // FIXME: Setting to make the current cell grow.
  //
  if ( multiRow( _col, _row )
       && d->textHeight > ( height - 2 * BORDER_SPACE
			    - topBorderWidth( _col, _row )
			    - bottomBorderWidth( _col, _row ) ) )
  {
    int  r   = d->row;
    int  end = 0;

    // Find free cells bottom to this one
    while ( !end ) {
      RowFormat    *rl2  = m_pSheet->rowFormat( r + 1 );
      KSpreadCell  *cell = m_pSheet->visibleCellAt( d->column, r + 1 );

      if ( cell->isEmpty() ) {
	height += rl2->dblHeight() - 1.0;
	r++;

	// Enough space ?
	if ( d->textHeight <= ( height - 2 * BORDER_SPACE
				- topBorderWidth( _col, _row )
				- bottomBorderWidth( _col, _row ) ) )
	  end = 1;
      }
      else
	// Not enough space, but the next cell is not empty.
	end = -1;
    }

    // Check to make sure we haven't already force-merged enough cells.
    if ( r - d->row > d->extra()->mergedYCells ) {
      d->extra()->extraYCells = r - d->row;
      d->extra()->extraHeight = height;

      for ( int i = d->row + 1; i <= r; ++i ) {
	KSpreadCell  *cell = m_pSheet->nonDefaultCell( d->column, i );
	cell->obscure( this );
      }

      // Not enough space?
      if ( end == -1 )
	setFlag( Flag_CellTooShortY );
    }
    else
      setFlag( Flag_CellTooShortY );
  }

  clearFlag( Flag_LayoutDirty );

  return;
}


void KSpreadCell::valueChanged ()
{
  update();

  m_pSheet->valueChanged (this);
}


// Recalculate d->strOutText.
//

void KSpreadCell::setOutputText()
{
  if ( isDefault() ) {
    d->strOutText = QString::null;

    if ( d->hasExtra() && d->extra()->conditions )
      d->extra()->conditions->checkMatches();

    return;
  }

  // If nothing has changed, we don't need to remake the text layout.
  if ( !testFlag( Flag_TextFormatDirty ) )
    return;

  // We don't want to remake the layout unnecessarily.
  clearFlag( Flag_TextFormatDirty );

  // Display a formula if warranted.  If not, display the value instead;
  // this is the most common case.
  if ( (!hasError()) && isFormula() && m_pSheet->getShowFormula()
       && !( m_pSheet->isProtected() && isHideFormula( d->column, d->row ) ) )
    d->strOutText = d->strText;
  else {
    d->strOutText = sheet()->doc()->formatter()->formatText (this,
							     formatType());
  }

  // Check conditions if needed.
  if ( d->hasExtra() && d->extra()->conditions )
    d->extra()->conditions->checkMatches();
}


// Recalculate d->textX and d->textY.
//
// Used in makeLayout() and calculateTextParameters().
//

void KSpreadCell::offsetAlign( int _col, int _row )
{
  int     a;
  AlignY  ay;
  int     tmpAngle;
  bool    tmpVerticalText;
  bool    tmpMultiRow;
  int     tmpTopBorderWidth = effTopBorderPen( _col, _row ).width();

  if ( d->hasExtra()
       && d->extra()->conditions
       && d->extra()->conditions->matchedStyle() )
  {
    KSpreadStyle  *style = d->extra()->conditions->matchedStyle();

    if ( style->hasFeature( KSpreadStyle::SAlignX, true ) )
      a = style->alignX();
    else
      a = align( _col, _row );

    if ( style->hasFeature( KSpreadStyle::SVerticalText, true ) )
      tmpVerticalText = style->hasProperty( KSpreadStyle::PVerticalText );
    else
      tmpVerticalText = verticalText( _col, _row );

    if ( style->hasFeature( KSpreadStyle::SMultiRow, true ) )
      tmpMultiRow = style->hasProperty( KSpreadStyle::PMultiRow );
    else
      tmpMultiRow = multiRow( _col, _row );

    if ( style->hasFeature( KSpreadStyle::SAlignY, true ) )
      ay = style->alignY();
    else
      ay = alignY( _col, _row );

    if ( style->hasFeature( KSpreadStyle::SAngle, true ) )
      tmpAngle = style->rotateAngle();
    else
      tmpAngle = getAngle( _col, _row );
  }
  else {
    a               = align( _col, _row );
    ay              = alignY( _col, _row );
    tmpAngle        = getAngle( _col, _row );
    tmpVerticalText = verticalText( _col, _row );
    tmpMultiRow     = multiRow( _col, _row );
  }

  RowFormat     *rl = m_pSheet->rowFormat( _row );
  ColumnFormat  *cl = m_pSheet->columnFormat( _col );

  double  w = cl->dblWidth();
  double  h = rl->dblHeight();

  if ( d->hasExtra() ) {
    if ( d->extra()->extraXCells )  w = d->extra()->extraWidth;
    if ( d->extra()->extraYCells )  h = d->extra()->extraHeight;
  }

  // Calculate d->textY based on the vertical alignment and a few
  // other inputs.
  switch( ay ) {
  case KSpreadCell::Top:
    if ( tmpAngle == 0 )
      d->textY = tmpTopBorderWidth + BORDER_SPACE
	+ (double) d->fmAscent / m_pSheet->doc()->zoomedResolutionY();
    else if ( tmpAngle < 0 )
      d->textY = tmpTopBorderWidth + BORDER_SPACE;
    else
      d->textY = tmpTopBorderWidth + BORDER_SPACE
	+ ( (double)d->fmAscent * cos( tmpAngle * M_PI / 180 )
	    / m_pSheet->doc()->zoomedResolutionY() );
    break;

  case KSpreadCell::Bottom:
    if ( !tmpVerticalText && !tmpMultiRow && !tmpAngle ) {
      d->textY = h - BORDER_SPACE - effBottomBorderPen( _col, _row ).width();
    }
    else if ( tmpAngle != 0 ) {
      if ( h - BORDER_SPACE - d->textHeight
	   - effBottomBorderPen( _col, _row ).width() > 0 )
      {
	if ( tmpAngle < 0 )
	  d->textY = h - BORDER_SPACE - d->textHeight
	    - effBottomBorderPen( _col, _row ).width();
	else
	  d->textY = h - BORDER_SPACE - d->textHeight
	    - effBottomBorderPen( _col, _row ).width()
	    + ( (double) d->fmAscent * cos( tmpAngle * M_PI / 180 )
		/ m_pSheet->doc()->zoomedResolutionY() );
      }
      else if ( tmpAngle < 0 )
	d->textY = tmpTopBorderWidth + BORDER_SPACE ;
      else
	d->textY = tmpTopBorderWidth + BORDER_SPACE
	  + ( (double) d->fmAscent * cos( tmpAngle * M_PI / 180 )
	      / m_pSheet->doc()->zoomedResolutionY() );
    }
    else if ( tmpMultiRow ) {
      int tmpline = d->hasExtra() ? d->extra()->nbLines : 0;
      if ( tmpline > 1 )
	tmpline--;  //number of extra lines

      if ( h - BORDER_SPACE - d->textHeight * d->extra()->nbLines
	   - effBottomBorderPen( _col, _row ).width() > 0 )
	d->textY = h - BORDER_SPACE - d->textHeight * tmpline
	  - effBottomBorderPen( _col, _row ).width();
      else
	d->textY = tmpTopBorderWidth + BORDER_SPACE
	  + (double) d->fmAscent / m_pSheet->doc()->zoomedResolutionY();
    }
    else {
      if ( h - BORDER_SPACE - d->textHeight
	   - effBottomBorderPen( _col, _row ).width() > 0 )
	d->textY = h - BORDER_SPACE - d->textHeight
	  - effBottomBorderPen( _col, _row ).width()
	  + (double)d->fmAscent / m_pSheet->doc()->zoomedResolutionY();
      else
	d->textY = tmpTopBorderWidth + BORDER_SPACE
	  + (double) d->fmAscent / m_pSheet->doc()->zoomedResolutionY();
    }
    break;

  case KSpreadCell::Middle:
  case KSpreadCell::UndefinedY:
    if ( !tmpVerticalText && !tmpMultiRow && !tmpAngle ) {
      d->textY = ( h - d->textHeight ) / 2
	+ (double) d->fmAscent / m_pSheet->doc()->zoomedResolutionY();
    }
    else if ( tmpAngle != 0 ) {
      if ( h - d->textHeight > 0 ) {
	if ( tmpAngle < 0 )
	  d->textY = ( h - d->textHeight ) / 2 ;
	else
	  d->textY = ( h - d->textHeight ) / 2 +
	    (double) d->fmAscent * cos( tmpAngle * M_PI / 180 ) /
	    m_pSheet->doc()->zoomedResolutionY();
      }
      else {
	if ( tmpAngle < 0 )
	  d->textY = tmpTopBorderWidth + BORDER_SPACE;
	else
	  d->textY = tmpTopBorderWidth + BORDER_SPACE
	    + ( (double)d->fmAscent * cos( tmpAngle * M_PI / 180 )
		/ m_pSheet->doc()->zoomedResolutionY() );
      }
    }
    else if ( tmpMultiRow ) {
      int tmpline = d->hasExtra() ? d->extra()->nbLines : 0;
      if ( tmpline == 0 )
	tmpline = 1;

      if ( h - d->textHeight * tmpline > 0 )
	d->textY = ( h - d->textHeight * tmpline ) / 2
	  + (double) d->fmAscent / m_pSheet->doc()->zoomedResolutionY();
      else
	d->textY = tmpTopBorderWidth + BORDER_SPACE
	  + (double) d->fmAscent / m_pSheet->doc()->zoomedResolutionY();
    }
    else {
      if ( h - d->textHeight > 0 )
	d->textY = ( h - d->textHeight ) / 2
	  + (double)d->fmAscent / m_pSheet->doc()->zoomedResolutionY();
      else
	d->textY = tmpTopBorderWidth + BORDER_SPACE
	  + (double)d->fmAscent / m_pSheet->doc()->zoomedResolutionY();
    }
    break;
  }

  a = effAlignX();
  if ( m_pSheet->getShowFormula()
       && !( m_pSheet->isProtected() && isHideFormula( _col, _row ) ) )
    a = KSpreadCell::Left;

  // Calculate d->textX based on alignment and textwidth.
  switch ( a ) {
  case KSpreadCell::Left:
    d->textX = effLeftBorderPen( _col, _row ).width() + BORDER_SPACE;
    break;
  case KSpreadCell::Right:
    d->textX = ( w - BORDER_SPACE - d->textWidth
		 - effRightBorderPen( _col, _row ).width() );
    break;
  case KSpreadCell::Center:
    d->textX = ( w - d->textWidth ) / 2;
    break;
  }
}


// Recalculate the current text dimensions, i.e. d->textWidth and
// d->textHeight.
//
// Used in makeLayout() and calculateTextParameters().
//
void KSpreadCell::textSize( QPainter &_paint )
{
  QFontMetrics  fm = _paint.fontMetrics();
  // Horizontal text ?

  int    tmpAngle;
  int    _row = row();
  int    _col = column();
  bool   tmpVerticalText;
  bool   fontUnderlined;
  AlignY ay;

  // Set tmpAngle, tmpeVerticalText, ay and fontUnderlined according
  // to if there is a matching condition or not.
  if ( d->hasExtra()
       && d->extra()->conditions
       && d->extra()->conditions->matchedStyle() )
  {
    KSpreadStyle  *style = d->extra()->conditions->matchedStyle();

    if ( style->hasFeature( KSpreadStyle::SAngle, true ) )
      tmpAngle = style->rotateAngle();
    else
      tmpAngle = getAngle( _col, _row );

    if ( style->hasFeature( KSpreadStyle::SVerticalText, true ) )
      tmpVerticalText = style->hasProperty( KSpreadStyle::PVerticalText );
    else
      tmpVerticalText = verticalText( _col, _row );

    if ( style->hasFeature( KSpreadStyle::SAlignY, true ) )
      ay = style->alignY();
    else
      ay = alignY( _col, _row );

    if ( style->hasFeature( KSpreadStyle::SFontFlag, true ) )
      fontUnderlined = ( style->fontFlags()
			 // FIXME: Should be & (uint)...?
			 && (uint) KSpreadStyle::FUnderline );
    else
      fontUnderlined = textFontUnderline( _col, _row );
  }
  else {
    // The cell has no condition with a maxed style.
    tmpAngle        = getAngle( _col, _row );
    tmpVerticalText = verticalText( _col, _row );
    ay              = alignY( _col, _row );
    fontUnderlined  = textFontUnderline( _col, _row );
  }

  // Set d->textWidth and d->textHeight to correct values according to
  // if the text is horizontal, vertical or rotated.
  if ( !tmpVerticalText && !tmpAngle ) {
    // Horizontal text.

    d->textWidth = m_pSheet->doc()->unzoomItX( fm.width( d->strOutText ) );
    int offsetFont = 0;
    if ( ( ay == KSpreadCell::Bottom ) && fontUnderlined ) {
      offsetFont = fm.underlinePos() + 1;
    }

    d->textHeight = m_pSheet->doc()->unzoomItY( fm.ascent() + fm.descent()
						+ offsetFont );
  }
  else if ( tmpAngle!= 0 ) {
    // Rotated text.

    d->textHeight = m_pSheet->doc()
      ->unzoomItY( int( cos( tmpAngle * M_PI / 180 )
			* ( fm.ascent() + fm.descent() )
			+ abs( int( ( fm.width( d->strOutText )
				      * sin( tmpAngle * M_PI / 180 ) ) ) ) ) );

    d->textWidth = m_pSheet->doc()
      ->unzoomItX( int( abs( int( ( sin( tmpAngle * M_PI / 180 )
				    * ( fm.ascent() + fm.descent() ) ) ) )
			+ fm.width( d->strOutText )
			  * cos ( tmpAngle * M_PI / 180 ) ) );
  }
  else {
    // Vertical text.
    int width = 0;
    for ( unsigned int i = 0; i < d->strOutText.length(); i++ )
      width = QMAX( width, fm.width( d->strOutText.at( i ) ) );

    d->textWidth  = m_pSheet->doc()->unzoomItX( width );
    d->textHeight = m_pSheet->doc()->unzoomItY( ( fm.ascent() + fm.descent() )
						* d->strOutText.length() );
  }
}


// Get the effective font to use after the zooming and apply it to `painter'.
//
// Used in makeLayout() and calculateTextParameters().
//

void KSpreadCell::applyZoomedFont( QPainter &painter, int _col, int _row )
{
  QFont  tmpFont( textFont( _col, _row ) );

  // If there is a matching condition on this cell then set the
  // according style parameters.
  if ( d->hasExtra()
       && d->extra()->conditions
       && d->extra()->conditions->matchedStyle() ) {

    KSpreadStyle * s = d->extra()->conditions->matchedStyle();

    // Other size?
    if ( s->hasFeature( KSpreadStyle::SFontSize, true ) )
      tmpFont.setPointSizeFloat( s->fontSize() );

    // Other attributes?
    if ( s->hasFeature( KSpreadStyle::SFontFlag, true ) ) {
      uint flags = s->fontFlags();

      tmpFont.setBold(      flags & (uint) KSpreadStyle::FBold );
      tmpFont.setUnderline( flags & (uint) KSpreadStyle::FUnderline );
      tmpFont.setItalic(    flags & (uint) KSpreadStyle::FItalic );
      tmpFont.setStrikeOut( flags & (uint) KSpreadStyle::FStrike );
    }

    // Other family?
    if ( s->hasFeature( KSpreadStyle::SFontFamily, true ) )
      tmpFont.setFamily( s->fontFamily() );
  }
#if 0
  else
  /*
   * could somebody please explaint why we check for isProtected or isHideFormula here
   */
   if ( d->extra()->conditions
	&& d->extra()->conditions->currentCondition( condition )
	&& !(m_pSheet->getShowFormula()
	     && !( m_pSheet->isProtected()
		   && isHideFormula( d->column, d->row ) ) ) )
   {
     if ( condition.fontcond )
       tmpFont = *(condition.fontcond);
     else
       tmpFont = condition.style->font();
   }
#endif

  // Scale the font size according to the current zoom.
  tmpFont.setPointSizeFloat( 0.01 * m_pSheet->doc()->zoom()
			     * tmpFont.pointSizeFloat() );

  painter.setFont( tmpFont );
}


//used in KSpreadSheet::adjustColumnHelper and KSpreadSheet::adjustRow
void KSpreadCell::calculateTextParameters( QPainter &_painter,
					   int _col, int _row )
{
  // Apply the correct font to _painter.
  applyZoomedFont( _painter, _col, _row );

  // Recalculate d->textWidth and d->textHeight
  textSize( _painter );

  // Recalculate d->textX and d->textY.
  offsetAlign( _col, _row );
}


// ----------------------------------------------------------------
//                          Formula handling


bool KSpreadCell::makeFormula()
{
  clearFormula ();

  d->formula = new KSpread::Formula (sheet(), this);
  d->formula->setExpression (d->strText);

  if (!d->formula->isValid ()) {
  // Did a syntax error occur ?
    clearFormula();

    if (m_pSheet->doc()->getShowMessageError())
    {
      QString tmp(i18n("Error in cell %1\n\n"));
      tmp = tmp.arg( fullName() );
      KMessageBox::error( (QWidget*)0L, tmp);
    }
    setFlag(Flag_ParseError);
    KSpreadValue v;
    v.setError ( "####" );
    setValue (v);
    return false;
  }

  //our value has been changed
  //TODO: is this necessary?
  valueChanged ();

  return true;
}

void KSpreadCell::clearFormula()
{
  delete d->formula;
  d->formula = 0L;
}

bool KSpreadCell::calc(bool delay)
{
  if ( !isFormula() )
    return true;

  if (d->formula == 0)
  {
    if ( testFlag( Flag_ParseError ) )  // there was a parse error
      return false;
    else
    {
      /* we were probably at a "isLoading() = true" state when we originally
       * parsed
       */
      makeFormula ();

      if ( d->formula == 0 ) // there was a parse error
        return false;
    }
  }

  if ( !testFlag( Flag_CalcDirty ) )
    return true;

  if ( delay )
  {
    if ( m_pSheet->doc()->delayCalculation() )
      return true;
  }

  setFlag(Flag_LayoutDirty);
  setFlag(Flag_TextFormatDirty);
  clearFlag(Flag_CalcDirty);

  KSpreadValue result = d->formula->eval ();
  setValue (result);
  if (result.isNumber())
    checkNumberFormat(); // auto-chooses number or scientific

  clearFlag(Flag_CalcDirty);
  setFlag(Flag_LayoutDirty);

  return true;
}


// ================================================================
//                            Painting


// Paint the cell.  This is the main function that calls a lot of
// other helper functions.
//
// `rect'       is the rectangle that we should paint on.  If the cell
//              does not overlap this rectangle, we can return immediately.
// `coordinate' is the origin (the upper left) of the cell in document
//              coordinates.
//

void KSpreadCell::paintCell( const KoRect   &rect, QPainter & painter,
                             KSpreadView    *view,
			     const KoPoint  &coordinate,
                             const QPoint   &cellRef,
			    /* bool paintBorderRight, bool paintBorderBottom,
			     bool paintBorderLeft,  bool paintBorderTop,*/
			     int paintBorder,
			     QPen & rightPen, QPen & bottomPen,
			     QPen & leftPen,  QPen & topPen,

			     bool drawCursor )
{

	bool paintBorderRight = paintBorder & Border_Right;
	bool paintBorderBottom = paintBorder & Border_Bottom;
	bool paintBorderLeft = paintBorder & Border_Left;
	bool paintBorderTop = paintBorder & Border_Top;

  // If we are already painting this cell, then return immediately.
  // This avoids infinite recursion.
  if ( testFlag( Flag_PaintingCell ) )
    return;

  // Indicate that we are painting this cell now.
  setFlag( Flag_PaintingCell );

  // This flag indicates that we are working on drawing the cells that
  // another cell is obscuring.  The value is the number of levels down we
  // are currently working -- i.e. a cell obscured by a cell which is
  // obscured by a cell.
  static int  paintingObscured = 0;

#if 0
  if (paintingObscured == 0)
    kdDebug(36001) << "painting cell " << name() << endl;
  else
    kdDebug(36001) << "  painting obscured cell " << name() << endl;
#endif

  // Sanity check: If we're working on drawing an obscured cell, that
  // means this cell should have a cell that obscures it.
  Q_ASSERT(!(paintingObscured > 0 && d->extra()->obscuringCells.isEmpty()));

  // The parameter cellref should be *this, unless this is the default cell.
  Q_ASSERT(isDefault()
	   || (((cellRef.x() == d->column) && (cellRef.y() == d->row))));

  KSpreadSheet::LayoutDirection sheetDir =  m_pSheet->layoutDirection();

  double left = coordinate.x();

  ColumnFormat * colFormat = m_pSheet->columnFormat( cellRef.x() );
  RowFormat    * rowFormat = m_pSheet->rowFormat( cellRef.y() );

  // Set width, height to the total width and height that this cell
  // covers, including obscured cells, and width0, height0 to the
  // width and height of this cell, maybe merged but never implicitly
  // extended.
  double  width0  = colFormat->dblWidth();
  double  height0 = rowFormat->dblHeight();
  double  width   = width0;
  double  height  = height0;

  // Handle right-to-left layout.
  // In an RTL sheet the cells have to be painted at their opposite horizontal
  // location on the canvas, meaning that column A will be the rightmost column
  // on screen, column B will be to the left of it and so on. Here we change
  // the horizontal coordinate at which we start painting the cell in case the
  // sheet's direction is RTL. We do this only if paintingObscured is 0,
  // otherwise the cell's painting location will flip back and forth in
  // consecutive calls to paintCell when painting obscured cells.
  if ( sheetDir == KSpreadSheet::RightToLeft && paintingObscured == 0
       && view && view->canvasWidget() )
  {
    double  dwidth = view->doc()->unzoomItX(view->canvasWidget()->width());
    left = dwidth - coordinate.x() - width;
  }

  // See if this cell is merged or has overflown into neighbor cells.
  // In that case, the width/height is greater than just the cell
  // itself.
  if (d->hasExtra()) {
    if (d->extra()->mergedXCells > 0 || d->extra()->mergedYCells > 0) {
      // merged cell extends to the left if sheet is RTL
      if ( sheetDir == KSpreadSheet::RightToLeft )
      {
        left -= d->extra()->extraWidth - width;
      }
      width0  = d->extra()->extraWidth;
      height0 = d->extra()->extraHeight;
      width   = d->extra()->extraWidth;
      height  = d->extra()->extraHeight;
    }
    else {
#if 0
      width  += d->extra()->extraXCells ? d->extra()->extraWidth  : 0;
      height += d->extra()->extraYCells ? d->extra()->extraHeight : 0;
#else
      // FIXME: Make extraWidth/Height really contain the *extra* width/height.
      if ( d->extra()->extraXCells )
	width  = d->extra()->extraWidth;
      if ( d->extra()->extraYCells )
	height = d->extra()->extraHeight;
#endif
    }
  }

  // Check if the cell is "selected", i.e. it should be drawn with the
  // color that indicates selection (dark blue).  If more than one
  // square is selected, the last one uses the ordinary colors.  In
  // that case, "selected" will be set to false even though the cell
  // itself really is selected.
  bool  selected = false;
  if ( view != NULL ) {
    selected = view->selection().contains( cellRef );

    // But the cell doesn't look selected if this is the marker cell.
    KSpreadCell  *cell = m_pSheet->cellAt( view->marker() );
    QPoint        bottomRight( view->marker().x() + cell->extraXCells(),
			       view->marker().y() + cell->extraYCells() );
    QRect         markerArea( view->marker(), bottomRight );
    selected = selected && !( markerArea.contains( cellRef ) );

    // Don't draw any selection at all when printing.
    if ( painter.device()->isExtDev() || !drawCursor )
      selected = false;
  }

  // Need to make a new layout ?
  //
  // FIXME: We have already used (at least) extraWidth/Height above,
  //        and now we are recalculating the layout.  This has to be
  //        moved up above all uses.
  //
  // FIXME: This needs to be taken out eventually - it is done in
  //        canvas::paintUpdates().
  if ( testFlag( Flag_LayoutDirty ) )
    makeLayout( painter, cellRef.x(), cellRef.y() );

  // ----------------  Start the actual painting.  ----------------

  // If the rect of this cell doesn't intersect the rect that should
  // be painted, we can skip the rest and return. (Note that we need
  // to calculate `left' first before we can do this.)
  const KoRect  cellRect( left, coordinate.y(), width, height );
  const KoRect  cellRect0( left, coordinate.y(), width0, height0 );
  if ( !cellRect.intersects( rect ) ) {
    clearFlag( Flag_PaintingCell );
    return;
  }

  // Get the background color.
  //
  // If there is a condition giving the background color for this cell
  // (and it matches), use that one, otherwise get the standard
  // background.
  QColor backgroundColor;
  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SBackgroundColor, true ) )
    backgroundColor = d->extra()->conditions->matchedStyle()->bgColor();
  else
    backgroundColor = bgColor( cellRef.x(), cellRef.y() );

  // 1. Paint the background.
  if ( !isObscuringForced() )
    paintBackground( painter, cellRect0, cellRef, selected, backgroundColor );

  // 2. Paint the default borders if we are on screen or if we are printing
  //    and the checkbox to do this is checked.
  if ( painter.device()->devType() != QInternal::Printer
       || m_pSheet->print()->printGrid())
    paintDefaultBorders( painter, rect, cellRect, cellRef,
			 paintBorderRight, paintBorderBottom,
                         paintBorderLeft,  paintBorderTop,
			 rightPen, bottomPen, leftPen, topPen );

  // 3. Paint all the cells that this one obscures.  They may only be
  //    partially obscured.
  //
  // The `paintingObscured' variable is used to avoid infinite
  // recursion since cells sometimes paint their obscuring cell as
  // well.
  paintingObscured++;

  if (d->hasExtra() && (d->extra()->extraXCells > 0
			|| d->extra()->extraYCells > 0))
    //kdDebug(36001) << "painting obscured cells for " << name() << endl;

  paintObscuredCells( rect, painter, view, cellRect, cellRef,
		      paintBorderRight, paintBorderBottom,
		      paintBorderLeft,  paintBorderTop,
		      rightPen, bottomPen, leftPen, topPen );
  paintingObscured--;

  // If we print pages, then we disable clipping, otherwise borders are
  // cut in the middle at the page borders.
  if ( painter.device()->isExtDev() )
    painter.setClipping( false );

  // 4. Paint the borders of the cell if no other cell is forcing this
  // one, i.e. this cell is not part of a merged cell.
  //
  //  FIXME: I don't like the term "force" here. Find a better one.
  if ( !isObscuringForced() )
	 // if (!testFlag(Flag_Highlight))
    paintCellBorders( painter, rect, cellRect0,
		      cellRef,
		      paintBorderRight, paintBorderBottom,
                      paintBorderLeft,  paintBorderTop,
		      rightPen, bottomPen, leftPen, topPen );

  if ( painter.device()->isExtDev() )
    painter.setClipping( true );

  // 5. Paint diagonal lines and page borders..
  paintCellDiagonalLines( painter, cellRect0, cellRef );
  paintPageBorders( painter, cellRect0, cellRef,
		    paintBorderRight, paintBorderBottom );



  // 6. Now paint the content, if this cell isn't obscured.
  if ( !isObscured() ) {

    // 6a. Paint possible comment indicator.
    if ( !painter.device()->isExtDev()
	 || m_pSheet->print()->printCommentIndicator() )
      paintCommentIndicator( painter, cellRect, cellRef, backgroundColor );

    // 6b. Paint possible formula indicator.
    if ( !painter.device()->isExtDev()
	 || m_pSheet->print()->printFormulaIndicator() )
      paintFormulaIndicator( painter, cellRect, backgroundColor );

    // 6c. Paint possible indicator for clipped text.
    paintMoreTextIndicator( painter, cellRect, backgroundColor );

     //6c. Paint cell highlight
   /* if (highlightBorder != Border_None)
	paintCellHighlight ( painter, cellRect, cellRef, highlightBorder,
    rightHighlightPen, bottomHighlightPen, leftHighlightPen, topHighlightPen );*/

    // 6d. Paint the text in the cell unless:
    //  a) it is empty
    //  b) something indicates that the text should not be painted
    //  c) the sheet is protected and the cell is hidden.
    if ( !d->strOutText.isEmpty()
	 && ( !painter.device()->isExtDev() || !getDontprintText( cellRef.x(),
								  cellRef.y() ) )
	 && !( m_pSheet->isProtected() && isHideAll( cellRef.x(), cellRef.y() ) ) )
    {
      paintText( painter, cellRect, cellRef );
    }
  }

  // 7. If this cell is obscured and we are not already painting obscured
  //    cells, then paint the obscuring cell(s).  Otherwise don't do
  //    anything so that we don't cause an infinite loop.
  if ( isObscured() && paintingObscured == 0 &&
       !( sheetDir == KSpreadSheet::RightToLeft && painter.device()->isExtDev() ) )
  {

    //kdDebug(36001) << "painting cells that obscure " << name() << endl;

    // Store the obscuringCells list in a list of QPoint(column, row)
    // This avoids crashes during the iteration through
    // obscuringCells, when the cells may get non valid or the list
    // itself gets changed during a call of obscuringCell->paintCell
    // (this happens e.g. when there is an updateDepend)
    if (d->hasExtra()) {
      QValueList<QPoint>                  listPoints;
      QValueList<KSpreadCell*>::iterator  it = d->extra()->obscuringCells.begin();
      QValueList<KSpreadCell*>::iterator  end = d->extra()->obscuringCells.end();
      for ( ; it != end; ++it ) {
        KSpreadCell *obscuringCell = *it;

        listPoints.append( QPoint( obscuringCell->column(),
				   obscuringCell->row() ) );
      }

      QValueList<QPoint>::iterator  it1  = listPoints.begin();
      QValueList<QPoint>::iterator  end1 = listPoints.end();
      for ( ; it1 != end1; ++it1 ) {
        QPoint obscuringCellRef = *it1;
        KSpreadCell *obscuringCell = m_pSheet->cellAt( obscuringCellRef.x(),
						       obscuringCellRef.y() );

        if ( obscuringCell != 0 ) {
          double x = m_pSheet->dblColumnPos( obscuringCellRef.x() );
          double y = m_pSheet->dblRowPos( obscuringCellRef.y() );
          if ( view != 0 ) {
            x -= view->canvasWidget()->xOffset();
            y -= view->canvasWidget()->yOffset();
          }

          KoPoint corner( x, y );
          painter.save();

	  // Get the effective pens for the borders.  These are
	  // determined by possible conditions on the cell with
	  // associated styles.
          QPen rp( obscuringCell->effRightBorderPen( obscuringCellRef.x(),
						     obscuringCellRef.y() ) );
          QPen bp( obscuringCell->effBottomBorderPen( obscuringCellRef.x(),
						      obscuringCellRef.y() ) );
          QPen lp( obscuringCell->effLeftBorderPen( obscuringCellRef.x(),
						    obscuringCellRef.y() ) );
          QPen tp( obscuringCell->effTopBorderPen( obscuringCellRef.x(),
						   obscuringCellRef.y() ) );


	  //kdDebug(36001) << "  painting obscuring cell "
	  //		 << obscuringCell->name() << endl;
	 // QPen highlightPen;

	  //Note: Painting of highlight isn't quite right.  If several cells are merged, then the
	  //whole merged cell will be painted with the colour of the last cell referenced which is inside the merged range
          obscuringCell->paintCell( rect, painter, view,
                                    corner, obscuringCellRef,
				    Border_Left|Border_Top|Border_Right|Border_Bottom,
						    rp, bp, lp, tp); // new pens
          painter.restore();
        }
      }
    }
  }

  // We are done with the painting, so remove the flag on the cell.
  clearFlag( Flag_PaintingCell );
}



// The following code was commented out in the above function.  I'll
// leave it here in case this functionality is ever re-implemented and
// someone wants some code to start from.
//
#if 0

  /**
     * Modification for drawing the button
     */
  if ( d->style == KSpreadCell::ST_Button )
  {

  QBrush fill( Qt::lightGray );
  QApplication::style().drawControl( QStyle::CE_PushButton, &_painter, this,
  QRect( _tx + 1, _ty + 1, w2 - 1, h2 - 1 ),
  defaultColorGroup ); //, selected, &fill );

    }

    /**
     * Modification for drawing the combo box
     */
  else if ( d->style == KSpreadCell::ST_Select )
    {
      QApplication::style().drawComboButton(  &_painter, _tx + 1, _ty + 1,
                                                w2 - 1, h2 - 1,
            defaultColorGroup, selected );
    }
#endif


/*
 void KSpreadCell::paintCellHighlight(QPainter& painter,
			 const KoRect& cellRect,
			 const QPoint& cellRef,
			 const int highlightBorder,
			 const QPen& rightPen,
			 const QPen& bottomPen,
			 const QPen& leftPen,
			 const QPen& topPen
			)
{
	//painter.drawLine(cellRect.left(),cellRect.top(),cellRect.right(),cellRect.bottom());
	//QPen pen(d->extra()->highlight);
	//painter.setPen(highlightPen);

	QBrush nullBrush;
	painter.setBrush(nullBrush);

	QRect zoomedCellRect = sheet()->doc()->zoomRect( cellRect );

	//The highlight rect is just inside the main cell rect
	//This saves the hassle of repainting nearby cells when the highlight is changed as the highlight areas
	//do not overlap
	zoomedCellRect.setLeft(zoomedCellRect.left()+1);
	//zoomedCellRect.setRight(zoomedCellRect.right()-1);
	zoomedCellRect.setTop(zoomedCellRect.top()+1);
	//zoomedCellRect.setBottom(zoomedCellRect.bottom()-1);

	if ( cellRef.x() != KS_colMax )
		zoomedCellRect.setWidth( zoomedCellRect.width() - 1 );
	if ( cellRef.y() != KS_rowMax )
	zoomedCellRect.setHeight( zoomedCellRect.height() - 1 );

	if (highlightBorder & Border_Top)
	{
		painter.setPen(topPen);
		painter.drawLine(zoomedCellRect.left(),zoomedCellRect.top(),zoomedCellRect.right(),zoomedCellRect.top());
	}
	if (highlightBorder & Border_Left)
	{
		painter.setPen(leftPen);
		painter.drawLine(zoomedCellRect.left(),zoomedCellRect.top(),zoomedCellRect.left(),zoomedCellRect.bottom());
	}
	if (highlightBorder & Border_Right)
	{
		painter.setPen(rightPen);
		painter.drawLine(zoomedCellRect.right(),zoomedCellRect.top(),zoomedCellRect.right(),zoomedCellRect.bottom());
	}
	if (highlightBorder & Border_Bottom)
	{
		painter.setPen(bottomPen);
		painter.drawLine(zoomedCellRect.left(),zoomedCellRect.bottom(),zoomedCellRect.right(),zoomedCellRect.bottom());
	}

	if (highlightBorder & Border_SizeGrip)
	{
		QBrush brush(rightPen.color());
		painter.setBrush(brush);
		painter.setPen(rightPen);
		painter.drawRect(zoomedCellRect.right()-3,zoomedCellRect.bottom()-3,4,4);
	}

	//painter.drawRect(zoomedCellRect.left(),zoomedCellRect.top(),zoomedCellRect.width(),zoomedCellRect.height());
}  */

// Paint all the cells that this cell obscures (helper function to paintCell).
//
void KSpreadCell::paintObscuredCells(const KoRect& rect, QPainter& painter,
                                     KSpreadView* view,
                                     const KoRect &cellRect,
                                     const QPoint &cellRef,
                                     bool paintBorderRight,
                                     bool _paintBorderBottom,
                                     bool paintBorderLeft,
				     bool _paintBorderTop,
                                     QPen & rightPen, QPen & _bottomPen,
                                     QPen & leftPen,  QPen & _topPen )
{
  // If there are no obscured cells, return.
  if ( !extraXCells() && !extraYCells() )
    return;

  double  ypos = cellRect.y();
  int     maxY = extraYCells();
  int     maxX = extraXCells();

  // Loop through the rectangle of squares that we obscure and paint them.
  for ( int y = 0; y <= maxY; ++y ) {
    double xpos = cellRect.x();
    RowFormat* rl = m_pSheet->rowFormat( cellRef.y() + y );

    for( int x = 0; x <= maxX; ++ x ) {
      ColumnFormat * cl = m_pSheet->columnFormat( cellRef.x() + x );
      if ( y != 0 || x != 0 ) {
	uint  column = cellRef.x() + x;
	uint  row    = cellRef.y() + y;

	QPen  topPen;
	QPen  bottomPen;
	bool  paintBorderTop;
	bool  paintBorderBottom;

	KSpreadCell  *cell = m_pSheet->cellAt( column, row );
	KoPoint       corner( xpos, ypos );

	// Check if the upper and lower borders should be painted, and
	// if so which pens we should use.  There used to be a nasty
	// bug here (#61452).
	// Check top pen.  Only check if this is not on the top row.
	topPen         = _topPen;
	paintBorderTop = _paintBorderTop;
	if ( row > 1 && !cell->isObscuringForced() ) {
	  KSpreadCell  *cellUp = m_pSheet->cellAt( column, row - 1 );

	  if ( cellUp->isDefault() )
	    paintBorderTop = false;
	  else {
	  // If the cell towards the top is part of a merged cell, get
	  // the pointer to the master cell.
	    cellUp = cellUp->ultimateObscuringCell();

	    topPen = cellUp->effBottomBorderPen( cellUp->column(),
						 cellUp->row() );

#if 0
	    int  penWidth = QMAX(1, sheet()->doc()->zoomItY( topPen.width() ));
	    topPen.setWidth( penWidth );
#endif
	  }
	}

	// FIXME: I thought we had to check bottom pen as well.
	//        However, it looks as if we don't need to.  It works anyway.
	bottomPen         = _bottomPen;
	paintBorderBottom = _paintBorderBottom;

	int  paintBorder = Border_None;
	if (paintBorderLeft) 	paintBorder |= KSpreadCell::Border_Left;
	if (paintBorderRight) 	paintBorder |= KSpreadCell::Border_Right;
	if (paintBorderTop)	paintBorder |= KSpreadCell::Border_Top;
	if (paintBorderBottom)	paintBorder |= KSpreadCell::Border_Bottom;

	/*KSpreadCell::BorderSides highlightBorder = Border_None;
	QPen highlightPen;*/

	cell->paintCell( rect, painter, view,
			 corner,
			 QPoint( cellRef.x() + x, cellRef.y() + y ),
			 paintBorder,
			 rightPen, bottomPen, leftPen, topPen);
      }
      xpos += cl->dblWidth();
    }

    ypos += rl->dblHeight();
  }
}


// Paint the background of this cell.
//
void KSpreadCell::paintBackground( QPainter& painter, const KoRect &cellRect,
                                   const QPoint &cellRef, bool selected,
                                   QColor &backgroundColor )
{
  QColorGroup  defaultColorGroup = QApplication::palette().active();
  QRect        zoomedCellRect    = sheet()->doc()->zoomRect( cellRect );

  // If this is not the KS_rowMax and/or KS_colMax, then we reduce
  // width and/or height by one.  This is due to the fact that the
  // right/bottom most pixel is shared with the left/top most pixel of
  // the following cell.  Only in the case of KS_colMax/KS_rowMax we
  // need to draw even this pixel, as there isn't a following cell to
  // draw the background pixel.
   if ( cellRef.x() != KS_colMax )
     zoomedCellRect.setWidth( zoomedCellRect.width() - 1 );
   if ( cellRef.y() != KS_rowMax )
     zoomedCellRect.setHeight( zoomedCellRect.height() - 1 );

  // Determine the correct background color
  if ( selected )
  {
    QColor c = defaultColorGroup.highlight().light();
    painter.setBackgroundColor( c );
  }
  else {
    QColor bg( backgroundColor );

    // Handle printers separately.
    if ( !painter.device()->isExtDev() ) {
      if ( bg.isValid() )
        painter.setBackgroundColor( bg );
      else
        painter.setBackgroundColor( defaultColorGroup.base() );
    }
    else {
      //bad hack but there is a qt bug
      //so I can print backgroundcolor
      QBrush bb( bg );
      if ( !bg.isValid() )
        bb.setColor( Qt::white );

      painter.fillRect( zoomedCellRect, bb );
      return;
    }
  }

  // Erase the background of the cell.
  if ( !painter.device()->isExtDev() )
    painter.eraseRect( zoomedCellRect );

  // Get a background brush
  QBrush bb;
  if ( d->hasExtra()
       && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SBackgroundBrush, true ) )
    bb = d->extra()->conditions->matchedStyle()->backGroundBrush();
  else
    bb = backGroundBrush( cellRef.x(), cellRef.y() );

  // Draw background pattern if necessary.
  if ( bb.style() != Qt::NoBrush )
    painter.fillRect( zoomedCellRect, bb );

  backgroundColor = painter.backgroundColor();
}


// Paint the standard light grey borders that are always visible.
//
void KSpreadCell::paintDefaultBorders( QPainter& painter, const KoRect &rect,
                                       const KoRect &cellRect,
                                       const QPoint &cellRef,
                                       bool paintBorderRight, bool paintBorderBottom,
                                       bool paintBorderLeft,  bool paintBorderTop,
                                       QPen const & rightPen, QPen const & bottomPen,
                                       QPen const & leftPen, QPen const & topPen )
{
  KSpreadDoc* doc = sheet()->doc();

  KSpreadSheet::LayoutDirection sheetDir =  m_pSheet->layoutDirection();

  // Each cell is responsible for drawing it's top and left portions
  // of the "default" grid. --Or not drawing it if it shouldn't be
  // there.  It's also responsible to paint the right and bottom, if
  // it is the last cell on a print out.

  bool paintTop;
  bool paintLeft;
  bool paintBottom;
  bool paintRight;

  paintLeft   = ( paintBorderLeft && leftPen.style() == Qt::NoPen
		  && sheet()->getShowGrid() );
  paintRight  = ( paintBorderRight && rightPen.style() == Qt::NoPen
		  && sheet()->getShowGrid() );
  paintTop    = ( paintBorderTop && topPen.style() == Qt::NoPen
		  && sheet()->getShowGrid() );
  paintBottom = ( paintBorderBottom && sheet()->getShowGrid()
                  && bottomPen.style() == Qt::NoPen );

  // If there are extra cells, there might be more conditions.
  if (d->hasExtra()) {
    QValueList<KSpreadCell*>::const_iterator it  = d->extra()->obscuringCells.begin();
    QValueList<KSpreadCell*>::const_iterator end = d->extra()->obscuringCells.end();
    for ( ; it != end; ++it ) {
      KSpreadCell *cell = *it;

      paintTop  = paintTop && ( cell->row() == cellRef.y() );
      paintBottom = false;

      if ( sheetDir == KSpreadSheet::RightToLeft )
      {
        paintRight = paintRight && ( cell->column() == cellRef.x() );
        paintLeft = false;
      }
      else
      {
        paintLeft = paintLeft && ( cell->column() == cellRef.x() );
        paintRight = false;
      }
    }
  }

  // The left border.
  if ( paintLeft ) {
    int dt = 0;
    int db = 0;

    if ( cellRef.x() > 1 ) {
      KSpreadCell  *cell_west = m_pSheet->cellAt( cellRef.x() - 1,
						  cellRef.y() );
      QPen t = cell_west->effTopBorderPen( cellRef.x() - 1, cellRef.y() );
      QPen b = cell_west->effBottomBorderPen( cellRef.x() - 1, cellRef.y() );

      if ( t.style() != Qt::NoPen )
        dt = ( t.width() + 1 )/2;
      if ( b.style() != Qt::NoPen )
        db = ( t.width() / 2);
    }

    painter.setPen( QPen( sheet()->doc()->gridColor(), 1, Qt::SolidLine ) );

    // If we are on paper printout, we limit the length of the lines.
    // On paper, we always have full cells, on screen not.
    if ( painter.device()->isExtDev() ) {
      if ( sheetDir == KSpreadSheet::RightToLeft )
        painter.drawLine( doc->zoomItX( QMAX( rect.left(),   cellRect.right() ) ),
                          doc->zoomItY( QMAX( rect.top(),    cellRect.y() + dt ) ),
                          doc->zoomItX( QMIN( rect.right(),  cellRect.right() ) ),
                          doc->zoomItY( QMIN( rect.bottom(), cellRect.bottom() - db ) ) );
      else
        painter.drawLine( doc->zoomItX( QMAX( rect.left(),   cellRect.x() ) ),
                          doc->zoomItY( QMAX( rect.top(),    cellRect.y() + dt ) ),
                          doc->zoomItX( QMIN( rect.right(),  cellRect.x() ) ),
                          doc->zoomItY( QMIN( rect.bottom(), cellRect.bottom() - db ) ) );
    }
    else {
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

  // The right border.
  if ( paintRight ) {
    int dt = 0;
    int db = 0;

    if ( cellRef.x() < KS_colMax ) {
      KSpreadCell  *cell_east = m_pSheet->cellAt( cellRef.x() + 1,
						  cellRef.y() );

      QPen t = cell_east->effTopBorderPen(    cellRef.x() + 1, cellRef.y() );
      QPen b = cell_east->effBottomBorderPen( cellRef.x() + 1, cellRef.y() );

      if ( t.style() != Qt::NoPen )
        dt = ( t.width() + 1 ) / 2;
      if ( b.style() != Qt::NoPen )
        db = ( t.width() / 2);
    }

    painter.setPen( QPen( sheet()->doc()->gridColor(), 1, Qt::SolidLine ) );

    // If we are on paper printout, we limit the length of the lines.
    // On paper, we always have full cells, on screen not.
    if ( painter.device()->isExtDev() )     {
      if ( sheetDir == KSpreadSheet::RightToLeft )
        painter.drawLine( doc->zoomItX( QMAX( rect.left(),   cellRect.x() ) ),
                          doc->zoomItY( QMAX( rect.top(),    cellRect.y() + dt ) ),
                          doc->zoomItX( QMIN( rect.right(),  cellRect.x() ) ),
                          doc->zoomItY( QMIN( rect.bottom(), cellRect.bottom() - db ) ) );
      else
        painter.drawLine( doc->zoomItX( QMAX( rect.left(),   cellRect.right() ) ),
                          doc->zoomItY( QMAX( rect.top(),    cellRect.y() + dt ) ),
                          doc->zoomItX( QMIN( rect.right(),  cellRect.right() ) ),
                          doc->zoomItY( QMIN( rect.bottom(), cellRect.bottom() - db ) ) );
    }
    else {
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

  // The top border.
  if ( paintTop ) {
    int dl = 0;
    int dr = 0;
    if ( cellRef.y() > 1 ) {
      KSpreadCell  *cell_north = m_pSheet->cellAt( cellRef.x(),
						   cellRef.y() - 1 );

      QPen l = cell_north->effLeftBorderPen(  cellRef.x(), cellRef.y() - 1 );
      QPen r = cell_north->effRightBorderPen( cellRef.x(), cellRef.y() - 1 );

      if ( l.style() != Qt::NoPen )
        dl = ( l.width() - 1 ) / 2 + 1;
      if ( r.style() != Qt::NoPen )
        dr = r.width() / 2;
    }

    painter.setPen( QPen( sheet()->doc()->gridColor(), 1, Qt::SolidLine ) );

    // If we are on paper printout, we limit the length of the lines.
    // On paper, we always have full cells, on screen not.
    if ( painter.device()->isExtDev() ) {
      painter.drawLine( doc->zoomItX( QMAX( rect.left(),   cellRect.x() + dl ) ),
                        doc->zoomItY( QMAX( rect.top(),    cellRect.y() ) ),
                        doc->zoomItX( QMIN( rect.right(),  cellRect.right() - dr ) ),
                        doc->zoomItY( QMIN( rect.bottom(), cellRect.y() ) ) );
    }
    else {
      painter.drawLine( doc->zoomItX( cellRect.x() + dl ),
                        doc->zoomItY( cellRect.y() ),
                        doc->zoomItX( cellRect.right() - dr ),
                        doc->zoomItY( cellRect.y() ) );
    }
  }

  // The bottom border.
  if ( paintBottom ) {
    int dl = 0;
    int dr = 0;
    if ( cellRef.y() < KS_rowMax ) {
      KSpreadCell  *cell_south = m_pSheet->cellAt( cellRef.x(),
						   cellRef.y() + 1 );

      QPen l = cell_south->effLeftBorderPen(  cellRef.x(), cellRef.y() + 1 );
      QPen r = cell_south->effRightBorderPen( cellRef.x(), cellRef.y() + 1 );

      if ( l.style() != Qt::NoPen )
        dl = ( l.width() - 1 ) / 2 + 1;
      if ( r.style() != Qt::NoPen )
        dr = r.width() / 2;
    }

    painter.setPen( QPen( sheet()->doc()->gridColor(), 1, Qt::SolidLine ) );

    // If we are on paper printout, we limit the length of the lines.
    // On paper, we always have full cells, on screen not.
    if ( painter.device()->isExtDev() ) {
      painter.drawLine( doc->zoomItX( QMAX( rect.left(),   cellRect.x() + dl ) ),
                        doc->zoomItY( QMAX( rect.top(),    cellRect.bottom() ) ),
                        doc->zoomItX( QMIN( rect.right(),  cellRect.right() - dr ) ),
                        doc->zoomItY( QMIN( rect.bottom(), cellRect.bottom() ) ) );
    }
    else {
      painter.drawLine( doc->zoomItX( cellRect.x() + dl ),
                        doc->zoomItY( cellRect.bottom() ),
                        doc->zoomItX( cellRect.right() - dr ),
                        doc->zoomItY( cellRect.bottom() ) );
    }
  }
}


// Paint a comment indicator if the cell has a comment.
//
void KSpreadCell::paintCommentIndicator( QPainter& painter,
                                         const KoRect &cellRect,
                                         const QPoint &/*cellRef*/,
                                         QColor &backgroundColor )
{
  KSpreadDoc * doc = sheet()->doc();

  // Point the little corner if there is a comment attached
  // to this cell.
  if ( ( m_mask & (uint) PComment )
       && cellRect.width() > 10.0
       && cellRect.height() > 10.0
       && ( sheet()->print()->printCommentIndicator()
            || ( !painter.device()->isExtDev() && doc->getShowCommentIndicator() ) ) ) {
    QColor penColor = Qt::red;

    // If background has high red part, switch to blue.
    if ( qRed( backgroundColor.rgb() ) > 127 &&
         qGreen( backgroundColor.rgb() ) < 80 &&
         qBlue( backgroundColor.rgb() ) < 80 )
    {
        penColor = Qt::blue;
    }

    // Get the triangle.
    QPointArray  point( 3 );
    if ( m_pSheet->layoutDirection()==KSpreadSheet::RightToLeft ) {
      point.setPoint( 0, doc->zoomItX( cellRect.x() + 6.0 ),
                         doc->zoomItY( cellRect.y() ) );
      point.setPoint( 1, doc->zoomItX( cellRect.x() ),
                         doc->zoomItY( cellRect.y() ) );
      point.setPoint( 2, doc->zoomItX( cellRect.x() ),
                         doc->zoomItY( cellRect.y() + 6.0 ) );
    }
    else {
      point.setPoint( 0, doc->zoomItX( cellRect.right() - 5.0 ),
                         doc->zoomItY( cellRect.y() ) );
      point.setPoint( 1, doc->zoomItX( cellRect.right() ),
                         doc->zoomItY( cellRect.y() ) );
      point.setPoint( 2, doc->zoomItX( cellRect.right() ),
                         doc->zoomItY( cellRect.y() + 5.0 ) );
    }

    // And draw it.
    painter.setBrush( QBrush( penColor ) );
    painter.setPen( Qt::NoPen );
    painter.drawPolygon( point );
  }
}



// Paint a small rectangle if this cell holds a formula.
//
void KSpreadCell::paintFormulaIndicator( QPainter& painter,
                                         const KoRect &cellRect,
                                         QColor &backgroundColor )
{
  if ( isFormula() &&
      m_pSheet->getShowFormulaIndicator() &&
      cellRect.width()  > 10.0 &&
      cellRect.height() > 10.0 )
  {
    KSpreadDoc* doc = sheet()->doc();

    QColor penColor = Qt::blue;
    // If background has high blue part, switch to red.
    if ( qRed( backgroundColor.rgb() ) < 80 &&
        qGreen( backgroundColor.rgb() ) < 80 &&
        qBlue( backgroundColor.rgb() ) > 127 )
    {
        penColor = Qt::red;
    }

    // Get the triangle...
    QPointArray point( 3 );
    if ( m_pSheet->layoutDirection()==KSpreadSheet::RightToLeft ) {
      point.setPoint( 0, doc->zoomItX( cellRect.right() - 6.0 ),
                         doc->zoomItY( cellRect.bottom() ) );
      point.setPoint( 1, doc->zoomItX( cellRect.right() ),
                         doc->zoomItY( cellRect.bottom() ) );
      point.setPoint( 2, doc->zoomItX( cellRect.right() ),
                         doc->zoomItY( cellRect.bottom() - 6.0 ) );
    }
    else {
      point.setPoint( 0, doc->zoomItX( cellRect.x() ),
                         doc->zoomItY( cellRect.bottom() - 6.0 ) );
      point.setPoint( 1, doc->zoomItX( cellRect.x() ),
                         doc->zoomItY( cellRect.bottom() ) );
      point.setPoint( 2, doc->zoomItX( cellRect.x() + 6.0 ),
                         doc->zoomItY( cellRect.bottom() ) );
    }

    // ...and draw it.
    painter.setBrush( QBrush( penColor ) );
    painter.setPen( Qt::NoPen );
    painter.drawPolygon( point );
  }
}


// Paint an indicator that the text in the cell is cut.
//
void KSpreadCell::paintMoreTextIndicator( QPainter& painter,
                                          const KoRect &cellRect,
                                          QColor &backgroundColor )
{
  // Show a red triangle when it's not possible to write all text in cell.
  // Don't print the red triangle if we're printing.
  if( testFlag( Flag_CellTooShortX ) &&
      !painter.device()->isExtDev() &&
      cellRect.height() > 4.0  &&
      cellRect.width()  > 4.0 )
  {
    KSpreadDoc* doc = sheet()->doc();

    QColor penColor = Qt::red;
    // If background has high red part, switch to blue.
    if ( qRed( backgroundColor.rgb() ) > 127 &&
	 qGreen( backgroundColor.rgb() ) < 80 &&
	 qBlue( backgroundColor.rgb() ) < 80 )
    {
        penColor = Qt::blue;
    }

    // Get the triangle...
    QPointArray point( 3 );
    if ( d->strOutText.isRightToLeft() ) {
      point.setPoint( 0, doc->zoomItX( cellRect.left() + 4.0 ),
                         doc->zoomItY( cellRect.y() + cellRect.height() / 2.0 -4.0 ) );
      point.setPoint( 1, doc->zoomItX( cellRect.left() ),
                         doc->zoomItY( cellRect.y() + cellRect.height() / 2.0 ));
      point.setPoint( 2, doc->zoomItX( cellRect.left() + 4.0 ),
                         doc->zoomItY( cellRect.y() + cellRect.height() / 2.0 +4.0 ) );
    }
    else {
      point.setPoint( 0, doc->zoomItX( cellRect.right() - 4.0 ),
                         doc->zoomItY( cellRect.y() + cellRect.height() / 2.0 - 4.0 ) );
      point.setPoint( 1, doc->zoomItX( cellRect.right() ),
                         doc->zoomItY( cellRect.y() + cellRect.height() / 2.0 ) );
      point.setPoint( 2, doc->zoomItX( cellRect.right() - 4.0 ),
                         doc->zoomItY( cellRect.y() + cellRect.height() / 2.0 + 4.0 ) );
    }

    // ...and paint it.
    painter.setBrush( QBrush( penColor ) );
    painter.setPen( Qt::NoPen );
    painter.drawPolygon( point );
  }
}


// Paint the real contents of a cell - the text.
//
void KSpreadCell::paintText( QPainter& painter,
                             const KoRect &cellRect,
                             const QPoint &cellRef )
{
  KSpreadDoc    *doc = sheet()->doc();

  ColumnFormat  *colFormat         = m_pSheet->columnFormat( cellRef.x() );

  QColorGroup    defaultColorGroup = QApplication::palette().active();
  QColor         textColorPrint    = effTextColor( cellRef.x(), cellRef.y() );

  // Resolve the text color if invalid (=default).
  if ( !textColorPrint.isValid() ) {
    if ( painter.device()->isExtDev() )
      textColorPrint = Qt::black;
    else
      textColorPrint = QApplication::palette().active().text();
  }

  QPen tmpPen( textColorPrint );

  // Set the font according to the current zoom.
  applyZoomedFont( painter, cellRef.x(), cellRef.y() );

  // Check for red font color for negative values.
  if ( !d->hasExtra()
       || !d->extra()->conditions
       || !d->extra()->conditions->matchedStyle() ) {
    if ( value().isNumber()
         && !( m_pSheet->getShowFormula()
               && !( m_pSheet->isProtected()
		     && isHideFormula( d->column, d->row ) ) ) )
    {
      double v = value().asFloat();
      if ( floatColor( cellRef.x(), cellRef.y()) == KSpreadCell::NegRed
	   && v < 0.0 )
        tmpPen.setColor( Qt::red );
    }
  }

  // Check for blue color, for hyperlink.
  if ( !link().isEmpty() ) {
    tmpPen.setColor( QApplication::palette().active().link() );
    QFont f = painter.font();
    f.setUnderline( true );
    painter.setFont( f );
  }

#if 0
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
*/

  if ( selected && ( cellRef.x() != marker.x() || cellRef.y() != marker.y() ) )
  {
    QPen p( tmpPen );
    p.setColor( defaultColorGroup.highlightedText() );
    painter.setPen( p );
  }
  else {
    painter.setPen(tmpPen);
  }
#endif
  painter.setPen( tmpPen );

  QString  tmpText   = d->strOutText;
  double   tmpHeight = d->textHeight;
  double   tmpWidth  = d->textWidth;

  // If the cell is to narrow to paint the whole contents, then pick
  // out a part of the content that we paint.  The result of this is
  // dependent on the data type of the content.
  //
  // FIXME: Make this dependent on the height as well.
  //
  if ( testFlag( Flag_CellTooShortX ) ) {
    d->strOutText = textDisplaying( painter );

    // Recalculate the text width and the offset.
    textSize( painter );
    offsetAlign( column(), row() );
  }

  // Hide zero.
  if ( m_pSheet->getHideZero()
       && value().isNumber()
       && value().asFloat() == 0 ) {
    d->strOutText = QString::null;
  }

  // Clear extra cell if column or row is hidden
  //
  // FIXME: I think this should be done before the call to
  // textDisplaying() above.
  //
  if ( colFormat->isHide() || ( cellRect.height() <= 2 ) ) {
    freeAllObscuredCells();  /* TODO: This looks dangerous...must check when I
                                have time */
    d->strOutText = "";
  }

  double indent = 0.0;
  double offsetCellTooShort = 0.0;
  int a = effAlignX();

  // Apply indent if text is align to left not when text is at right or middle.
  if (  a == KSpreadCell::Left && !isEmpty() ) {
    // FIXME: The following condition should be remade into a call to
    //        a new convenience function:
    //   if ( hasConditionStyleFeature( KSpreadStyle::SIndent, true )...
    //        This should be done throughout the entire file.
    //
    if ( d->hasExtra()
	 && d->extra()->conditions
         && d->extra()->conditions->matchedStyle()
         && d->extra()->conditions->matchedStyle()->hasFeature( KSpreadStyle::SIndent, true ) )
      indent = d->extra()->conditions->matchedStyle()->indent();
    else
      indent = getIndent( column(), row() );
  }

  // Made an offset, otherwise ### is under red triangle.
  if ( a == KSpreadCell::Right && !isEmpty() && testFlag( Flag_CellTooShortX ) )
    offsetCellTooShort = m_pSheet->doc()->unzoomItX( 4 );

  QFontMetrics fm2 = painter.fontMetrics();
  double offsetFont = 0.0;

  if ( alignY( column(), row() ) == KSpreadCell::Bottom
       && textFontUnderline( column(), row() ) )
    offsetFont = m_pSheet->doc()->unzoomItX( fm2.underlinePos() + 1 );

  int  tmpAngle;
  bool tmpMultiRow;
  bool tmpVerticalText;

  // Check for angled or vertical text.
  if ( d->hasExtra()
       && d->extra()->conditions
       && d->extra()->conditions->matchedStyle() )
  {
    KSpreadStyle  *matchedStyle = d->extra()->conditions->matchedStyle();

    if ( matchedStyle->hasFeature( KSpreadStyle::SAngle, true ) )
      tmpAngle = d->extra()->conditions->matchedStyle()->rotateAngle();
    else
      tmpAngle = getAngle( cellRef.x(), cellRef.y() );

    if ( matchedStyle->hasFeature( KSpreadStyle::SVerticalText, true ) )
      tmpVerticalText = matchedStyle->hasProperty( KSpreadStyle::PVerticalText );
    else
      tmpVerticalText = verticalText( cellRef.x(), cellRef.y() );

    if ( matchedStyle->hasFeature( KSpreadStyle::SMultiRow, true ) )
      tmpMultiRow = matchedStyle->hasProperty( KSpreadStyle::PMultiRow );
    else
      tmpMultiRow = multiRow( cellRef.x(), cellRef.y() );
  }
  else {
    tmpAngle        = getAngle( cellRef.x(), cellRef.y() );
    tmpVerticalText = verticalText( cellRef.x(), cellRef.y() );
    tmpMultiRow     = multiRow( cellRef.x(), cellRef.y() );
  }

  // Actually paint the text.  There are a number of cases to consider.
  //
  if ( !tmpMultiRow && !tmpVerticalText && !tmpAngle ) {
    // Case 1: The simple case, one line, no angle.

    painter.drawText( doc->zoomItX( indent + cellRect.x() + d->textX - offsetCellTooShort ),
                      doc->zoomItY( cellRect.y() + d->textY - offsetFont ), d->strOutText );
  }
  else if ( tmpAngle != 0 ) {
    // Case 2: an angle.

    int angle = tmpAngle;
    QFontMetrics fm = painter.fontMetrics();

    painter.rotate( angle );
    double x;
    if ( angle > 0 )
      x = indent + cellRect.x() + d->textX;
    else
      x = indent + cellRect.x() + d->textX
        - doc->unzoomItX((int) (( fm.descent() + fm.ascent() ) * sin( angle * M_PI / 180 )));
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
  else if ( tmpMultiRow && !tmpVerticalText ) {
    // Case 3: Multiple rows, but horizontal.

    QString t;
    int i;
    int pos = 0;
    double dy = 0.0;
    QFontMetrics fm = painter.fontMetrics();
    do {
      i = d->strOutText.find( "\n", pos );
      if ( i == -1 )
        t = d->strOutText.mid( pos, d->strOutText.length() - pos );
      else {
        t = d->strOutText.mid( pos, i - pos );
        pos = i + 1;
      }

      int align = effAlignX();
      if ( m_pSheet->getShowFormula()
	   && !( m_pSheet->isProtected()
		 && isHideFormula( d->column, d->row ) ) )
        align = KSpreadCell::Left;

      // #### Torben: This looks duplicated for me
      switch ( align ) {
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
    } while ( i != -1 );
  }
  else if ( tmpVerticalText && !d->strOutText.isEmpty() ) {
    // Case 4: Vertical text.

    QString t;
    int i = 0;
    int len = 0;
    double dy = 0.0;
    QFontMetrics fm = painter.fontMetrics();
    do {
      len = d->strOutText.length();
      t = d->strOutText.at( i );
      painter.drawText( doc->zoomItX( indent + cellRect.x() + d->textX ),
                        doc->zoomItY( cellRect.y() + d->textY + dy ), t );
      dy += doc->unzoomItY( fm.descent() + fm.ascent() );
      i++;
    } while ( i != len );
  }

  // Check for too short cell and set the outText for future reference.
  if ( testFlag( Flag_CellTooShortX ) ) {
    d->strOutText = tmpText;
    d->textHeight = tmpHeight;
    d->textWidth  = tmpWidth;
  }

  if ( m_pSheet->getHideZero() && value().isNumber()
       && value().asFloat() == 0 )
    d->strOutText = tmpText;

  if ( colFormat->isHide() || ( cellRect.height() <= 2 ) )
    d->strOutText = tmpText;
}


// Paint page borders on the page.  Only do this on the screen.
//
void KSpreadCell::paintPageBorders( QPainter& painter,
                                    const KoRect &cellRect,
                                    const QPoint &cellRef,
                                    bool paintBorderRight,
                                    bool paintBorderBottom )
{
  // Not screen?  Return immediately.
  if ( painter.device()->isExtDev() )
    return;

  if ( ! m_pSheet->isShowPageBorders() )
    return;

  KSpreadSheetPrint* print = m_pSheet->print();

  KSpreadSheet::LayoutDirection sheetDir =  m_pSheet->layoutDirection();

  KSpreadDoc* doc = sheet()->doc();
  int zcellRect_left = doc->zoomItX (cellRect.left());
  int zcellRect_right = doc->zoomItX (cellRect.right());
  int zcellRect_top = doc->zoomItY (cellRect.top());
  int zcellRect_bottom = doc->zoomItY (cellRect.bottom());

  // Draw page borders

  if ( cellRef.x() >= print->printRange().left() &&
    cellRef.x() <= print->printRange().right() + 1 &&
    cellRef.y() >= print->printRange().top() &&
    cellRef.y() <= print->printRange().bottom() + 1 )
  {
    if ( print->isOnNewPageX( cellRef.x() ) &&
          ( cellRef.y() <= print->printRange().bottom() ) )
    {
      painter.setPen( sheet()->doc()->pageBorderColor() );

      if ( sheetDir == KSpreadSheet::RightToLeft )
        painter.drawLine( zcellRect_right, zcellRect_top,
                          zcellRect_right, zcellRect_bottom );
      else
        painter.drawLine( zcellRect_left, zcellRect_top,
                          zcellRect_left, zcellRect_bottom );
    }

    if ( print->isOnNewPageY( cellRef.y() ) &&
          ( cellRef.x() <= print->printRange().right() ) )
    {
      painter.setPen( sheet()->doc()->pageBorderColor() );
      painter.drawLine( zcellRect_left,  zcellRect_top,
                        zcellRect_right, zcellRect_top );
    }

    if ( paintBorderRight ) {
      if ( print->isOnNewPageX( cellRef.x() + 1 )
            && ( cellRef.y() <= print->printRange().bottom() ) ) {
        painter.setPen( sheet()->doc()->pageBorderColor() );

        if ( sheetDir == KSpreadSheet::RightToLeft )
          painter.drawLine( zcellRect_left, zcellRect_top,
                            zcellRect_left, zcellRect_bottom );
        else
          painter.drawLine( zcellRect_right, zcellRect_top,
                            zcellRect_right, zcellRect_bottom );
      }
    }

    if ( paintBorderBottom ) {
      if ( print->isOnNewPageY( cellRef.y() + 1 )
          && ( cellRef.x() <= print->printRange().right() ) ) {
        painter.setPen( sheet()->doc()->pageBorderColor() );
        painter.drawLine( zcellRect_left,  zcellRect_bottom,
                          zcellRect_right, zcellRect_bottom );
      }
    }
  }
}


// Paint the cell borders.
//
void KSpreadCell::paintCellBorders( QPainter& painter, const KoRect& rect,
                                    const KoRect &cellRect,
                                    const QPoint &cellRef,
                                    bool paintRight, bool paintBottom,
                                    bool paintLeft,  bool paintTop,
                                    QPen & _rightPen, QPen & _bottomPen,
                                    QPen & _leftPen,  QPen & _topPen )
{
  KSpreadDoc * doc = sheet()->doc();

  KSpreadSheet::LayoutDirection sheetDir =  m_pSheet->layoutDirection();

  // compute zoomed rectangles
  // I don't use KoRect, because that ends up producing lots of warnings
  // about double->int conversions in calls to painter.drawLine
  int zrect_left (doc->zoomItX (rect.left()));
  int zrect_right (doc->zoomItX (rect.right()));
  int zrect_top (doc->zoomItY (rect.top()));
  int zrect_bottom (doc->zoomItY (rect.bottom()));
  int zcellRect_left (doc->zoomItX (cellRect.left()));
  int zcellRect_right (doc->zoomItX (cellRect.right()));
  int zcellRect_top (doc->zoomItY (cellRect.top()));
  int zcellRect_bottom (doc->zoomItY (cellRect.bottom()));

  /* we might not paint some borders if this cell is merged with another in
     that direction
  bool paintLeft   = paintBorderLeft;
  bool paintRight  = paintBorderRight;
  bool paintTop    = paintBorderTop;
  bool paintBottom = paintBorderBottom;
  */

  // paintRight  = paintRight  && ( extraXCells() == 0 );
  // paintBottom = paintBottom && ( d->extra()->extraYCells() == 0 );

  if (d->hasExtra()) {
    QValueList<KSpreadCell*>::const_iterator it  = d->extra()->obscuringCells.begin();
    QValueList<KSpreadCell*>::const_iterator end = d->extra()->obscuringCells.end();
    for ( ; it != end; ++it ) {
      KSpreadCell* cell = *it;

      int xDiff = cellRef.x() - cell->column();
      int yDiff = cellRef.y() - cell->row();
      paintLeft = paintLeft && xDiff == 0;
      paintTop  = paintTop  && yDiff == 0;

      // Paint the border(s) if either this one should or if we have a
      // merged cell with this cell as its border.
      paintRight  = paintRight  && cell->mergedXCells() == xDiff;
      paintBottom = paintBottom && cell->mergedYCells() == yDiff;
    }
  }

  // Must create copies of these since otherwise the zoomIt()
  // operation will be performed on them repeatedly.
  QPen  leftPen( _leftPen );
  QPen  rightPen( _rightPen );
  QPen  topPen( _topPen );
  QPen  bottomPen( _bottomPen );

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

  if ( paintLeft && leftPen.style() != Qt::NoPen ) {
    int top = ( QMAX( 0, -1 + top_penWidth ) ) / 2 +
              ( ( QMAX( 0, -1 + top_penWidth ) ) % 2 );
    int bottom = ( QMAX( 0, -1 + bottom_penWidth ) ) / 2 + 1;

    painter.setPen( leftPen );

    //kdDebug(36001) << "    painting left border of cell " << name() << endl;

    // If we are on paper printout, we limit the length of the lines.
    // On paper, we always have full cells, on screen not.
    if ( painter.device()->isExtDev() ) {
	// FIXME: There is probably Cut&Paste bugs here as well as below.
	//        The QMIN/QMAX and left/right pairs don't really make sense.
	//
	//    UPDATE: In fact, most of these QMIN/QMAX combinations
	//            are TOTALLY BOGUS.  For one thing, the idea
	//            that we always have full cells on paper is wrong
	//            since we can have embedded sheets in e.g. kword,
	//            and those can be arbitrarily clipped.  WE HAVE TO
	//            REVISE THIS WHOLE BORDER PAINTING SECTION!
	//
      if ( sheetDir == KSpreadSheet::RightToLeft )
        painter.drawLine( QMIN( zrect_right,  zcellRect_right ),
                          QMAX( zrect_top,    zcellRect_top - top ),
                          QMIN( zrect_right,  zcellRect_right ),
                          QMIN( zrect_bottom, zcellRect_bottom + bottom ) );
      else
        painter.drawLine( QMAX( zrect_left,   zcellRect_left ),
                          QMAX( zrect_top,    zcellRect_top - top ),
                          QMAX( zrect_left,   zcellRect_left ),
                          QMIN( zrect_bottom, zcellRect_bottom + bottom ) );
    }
    else {
      if ( sheetDir == KSpreadSheet::RightToLeft )
        painter.drawLine( zcellRect_right,
                          zcellRect_top - top,
                          zcellRect_right,
                          zcellRect_bottom + bottom );
      else
        painter.drawLine( zcellRect_left,
                          zcellRect_top - top,
                          zcellRect_left,
                          zcellRect_bottom + bottom );
    }
  }

  if ( paintRight && rightPen.style() != Qt::NoPen ) {
    int top = ( QMAX( 0, -1 + top_penWidth ) ) / 2 +
              ( ( QMAX( 0, -1 + top_penWidth ) ) % 2 );
    int bottom = ( QMAX( 0, -1 + bottom_penWidth ) ) / 2 + 1;

    painter.setPen( rightPen );

    //kdDebug(36001) << "    painting right border of cell " << name() << endl;

    // If we are on paper printout, we limit the length of the lines.
    // On paper, we always have full cells, on screen not.
    if ( painter.device()->isExtDev() ) {
      if ( sheetDir == KSpreadSheet::RightToLeft )
        painter.drawLine( QMAX( zrect_left, zcellRect_left ),
                          QMAX( zrect_top, zcellRect_top - top ),
                          QMAX( zrect_left, zcellRect_left ),
                          QMIN( zrect_bottom, zcellRect_bottom + bottom ) );
      else {
	// FIXME: This is the way all these things should look.
	//        Make it so.
	//
	// Only print the right border if it is visible.
	if ( zcellRect_right <= zrect_right + right_penWidth / 2)
	  painter.drawLine( zcellRect_right,
			    QMAX( zrect_top, zcellRect_top - top ),
			    zcellRect_right,
			    QMIN( zrect_bottom, zcellRect_bottom + bottom ) );
      }
    }
    else {
      if ( sheetDir == KSpreadSheet::RightToLeft )
        painter.drawLine( zcellRect_left,
                          zcellRect_top - top,
                          zcellRect_left,
                          zcellRect_bottom + bottom );
      else
        painter.drawLine( zcellRect_right,
                          zcellRect_top - top,
                          zcellRect_right,
                          zcellRect_bottom + bottom );
    }
  }

  if ( paintTop && topPen.style() != Qt::NoPen ) {
    painter.setPen( topPen );

    //kdDebug(36001) << "    painting top border of cell " << name()
    //		   << " [" << zcellRect_left << "," << zcellRect_right
    //		   << ": " << zcellRect_right - zcellRect_left << "]" << endl;

    // If we are on paper printout, we limit the length of the lines.
    // On paper, we always have full cells, on screen not.
    if ( painter.device()->isExtDev() ) {
      if ( zcellRect_top >= zrect_top + top_penWidth / 2)
	painter.drawLine( QMAX( zrect_left,   zcellRect_left ),
			  zcellRect_top,
			  QMIN( zrect_right,  zcellRect_right ),
			  zcellRect_top );
    }
    else {
      painter.drawLine( zcellRect_left, zcellRect_top,
                        zcellRect_right, zcellRect_top );
    }
  }

  if ( paintBottom && bottomPen.style() != Qt::NoPen ) {
    painter.setPen( bottomPen );

    //kdDebug(36001) << "    painting bottom border of cell " << name()
    //		   << " [" << zcellRect_left << "," << zcellRect_right
    //		   << ": " << zcellRect_right - zcellRect_left << "]" << endl;

    // If we are on paper printout, we limit the length of the lines.
    // On paper, we always have full cells, on screen not.
    if ( painter.device()->isExtDev() ) {
      if ( zcellRect_bottom <= zrect_bottom + bottom_penWidth / 2)
	painter.drawLine( QMAX( zrect_left,   zcellRect_left ),
			  zcellRect_bottom,
			  QMIN( zrect_right,  zcellRect_right ),
			  zcellRect_bottom );
    }
    else {
      painter.drawLine( zcellRect_left, zcellRect_bottom,
                        zcellRect_right, zcellRect_bottom );
    }
  }

  // FIXME: Look very closely at when the following code is really needed.
  //        I can't really see any case, but I might be wrong.
  //        Since the code below is buggy, and incredibly complex,
  //        I am currently disabling it.  If somebody wants to enable
  //        it again, then please also solve bug 68977: "Embedded KSpread
  //        document printing problem" at the same time.
  return;

  // Look at the cells on our corners. It may happen that we
  // just erased parts of their borders corner, so we might need
  // to repaint these corners.
  //
  QPen  vert_pen, horz_pen;
  int   vert_penWidth, horz_penWidth;

  // Some useful referenses.
  KSpreadCell  *cell_north     = m_pSheet->cellAt( cellRef.x(),
						   cellRef.y() - 1 );
  KSpreadCell  *cell_northwest = m_pSheet->cellAt( cellRef.x() - 1,
						   cellRef.y() - 1 );
  KSpreadCell  *cell_west      = m_pSheet->cellAt( cellRef.x() - 1,
						   cellRef.y() );
  KSpreadCell  *cell_northeast = m_pSheet->cellAt( cellRef.x() + 1,
						   cellRef.y() - 1 );
  KSpreadCell  *cell_east      = m_pSheet->cellAt( cellRef.x() + 1,
						   cellRef.y() );
  KSpreadCell  *cell_south     = m_pSheet->cellAt( cellRef.x(),
						   cellRef.y() + 1 );
  KSpreadCell  *cell_southwest = m_pSheet->cellAt( cellRef.x() - 1,
						   cellRef.y() + 1 );
  KSpreadCell  *cell_southeast = m_pSheet->cellAt( cellRef.x() + 1,
						   cellRef.y() + 1 );

  // Fix the borders which meet at the top left corner
  if ( cell_north->effLeftBorderValue( cellRef.x(), cellRef.y() - 1 )
       >= cell_northwest->effRightBorderValue( cellRef.x() - 1, cellRef.y() - 1 ) )
    vert_pen = cell_north->effLeftBorderPen( cellRef.x(), cellRef.y() - 1 );
  else
    vert_pen = cell_northwest->effRightBorderPen( cellRef.x() - 1,
						  cellRef.y() - 1 );

  vert_penWidth = QMAX( 1, doc->zoomItX( vert_pen.width() ) );
  vert_pen.setWidth( vert_penWidth );

  if ( vert_pen.style() != Qt::NoPen ) {
    if ( cell_west->effTopBorderValue( cellRef.x() - 1, cellRef.y() )
         >= cell_northwest->effBottomBorderValue( cellRef.x() - 1, cellRef.y() - 1 ) )
      horz_pen = cell_west->effTopBorderPen( cellRef.x() - 1, cellRef.y() );
    else
      horz_pen = cell_northwest->effBottomBorderPen( cellRef.x() - 1,
						     cellRef.y() - 1 );

    horz_penWidth = QMAX( 1, doc->zoomItY( horz_pen.width() ) );
    int bottom = ( QMAX( 0, -1 + horz_penWidth ) ) / 2 + 1;

    painter.setPen( vert_pen );
    // If we are on paper printout, we limit the length of the lines.
    // On paper, we always have full cells, on screen not.
    if ( painter.device()->isExtDev() ) {
      if ( sheetDir == KSpreadSheet::RightToLeft )
        painter.drawLine( QMAX( zrect_left, zcellRect_right ),
                          QMAX( zrect_top, zcellRect_top ),
                          QMIN( zrect_right, zcellRect_right ),
                          QMIN( zrect_bottom, zcellRect_top + bottom ) );
      else
        painter.drawLine( QMAX( zrect_left, zcellRect_left ),
                          QMAX( zrect_top, zcellRect_top ),
                          QMIN( zrect_right, zcellRect_left ),
                          QMIN( zrect_bottom, zcellRect_top + bottom ) );
    }
    else {
      if ( sheetDir == KSpreadSheet::RightToLeft )
        painter.drawLine( zcellRect_right, zcellRect_top,
                          zcellRect_right, zcellRect_top + bottom );
      else
        painter.drawLine( zcellRect_left, zcellRect_top,
                          zcellRect_left, zcellRect_top + bottom );
    }
  }

  // Fix the borders which meet at the top right corner
  if ( cell_north->effRightBorderValue( cellRef.x(), cellRef.y() - 1 )
       >= cell_northeast->effLeftBorderValue( cellRef.x() + 1,
					      cellRef.y() - 1 ) )
    vert_pen = cell_north->effRightBorderPen( cellRef.x(), cellRef.y() - 1 );
  else
    vert_pen = cell_northeast->effLeftBorderPen( cellRef.x() + 1,
						 cellRef.y() - 1 );

  // vert_pen = effRightBorderPen( cellRef.x(), cellRef.y() - 1 );
  vert_penWidth = QMAX( 1, doc->zoomItX( vert_pen.width() ) );
  vert_pen.setWidth( vert_penWidth );
  if ( ( vert_pen.style() != Qt::NoPen ) && ( cellRef.x() < KS_colMax ) ) {
    if ( cell_east->effTopBorderValue( cellRef.x() + 1, cellRef.y() )
         >= cell_northeast->effBottomBorderValue( cellRef.x() + 1,
						  cellRef.y() - 1 ) )
      horz_pen = cell_east->effTopBorderPen( cellRef.x() + 1, cellRef.y() );
    else
      horz_pen = cell_northeast->effBottomBorderPen( cellRef.x() + 1,
						     cellRef.y() - 1 );

    // horz_pen = effTopBorderPen( cellRef.x() + 1, cellRef.y() );
    horz_penWidth = QMAX( 1, doc->zoomItY( horz_pen.width() ) );
    int bottom = ( QMAX( 0, -1 + horz_penWidth ) ) / 2 + 1;

    painter.setPen( vert_pen );
    //If we are on paper printout, we limit the length of the lines
    //On paper, we always have full cells, on screen not
    if ( painter.device()->isExtDev() ) {
      if ( sheetDir == KSpreadSheet::RightToLeft )
        painter.drawLine( QMAX( zrect_left, zcellRect_left ),
                          QMAX( zrect_top, zcellRect_top ),
                          QMIN( zrect_right, zcellRect_left ),
                          QMIN( zrect_bottom, zcellRect_top + bottom ) );
      else
        painter.drawLine( QMAX( zrect_left, zcellRect_right ),
                          QMAX( zrect_top, zcellRect_top ),
                          QMIN( zrect_right, zcellRect_right ),
                          QMIN( zrect_bottom, zcellRect_top + bottom ) );
    }
    else {
      if ( sheetDir == KSpreadSheet::RightToLeft )
        painter.drawLine( zcellRect_left, zcellRect_top,
                          zcellRect_left, zcellRect_top + bottom );
      else
        painter.drawLine( zcellRect_right, zcellRect_top,
                          zcellRect_right, zcellRect_top + bottom );
    }
  }

  // Bottom
  if ( cellRef.y() < KS_rowMax ) {
    // Fix the borders which meet at the bottom left corner
    if ( cell_south->effLeftBorderValue( cellRef.x(), cellRef.y() + 1 )
         >= cell_southwest->effRightBorderValue( cellRef.x() - 1,
						 cellRef.y() + 1 ) )
      vert_pen = cell_south->effLeftBorderPen( cellRef.x(), cellRef.y() + 1 );
    else
      vert_pen = cell_southwest->effRightBorderPen( cellRef.x() - 1,
						    cellRef.y() + 1 );

    // vert_pen = effLeftBorderPen( cellRef.x(), cellRef.y() + 1 );
    vert_penWidth = QMAX( 1, doc->zoomItY( vert_pen.width() ) );
    vert_pen.setWidth( vert_penWidth );
    if ( vert_pen.style() != Qt::NoPen ) {
      if ( cell_west->effBottomBorderValue( cellRef.x() - 1, cellRef.y() )
           >= cell_southwest->effTopBorderValue( cellRef.x() - 1,
						 cellRef.y() + 1 ) )
        horz_pen = cell_west->effBottomBorderPen( cellRef.x() - 1,
						  cellRef.y() );
      else
        horz_pen = cell_southwest->effTopBorderPen( cellRef.x() - 1,
						    cellRef.y() + 1 );

      // horz_pen = effBottomBorderPen( cellRef.x() - 1, cellRef.y() );
      horz_penWidth = QMAX( 1, doc->zoomItX( horz_pen.width() ) );
      int bottom = ( QMAX( 0, -1 + horz_penWidth ) ) / 2;

      painter.setPen( vert_pen );
      // If we are on paper printout, we limit the length of the lines.
      // On paper, we always have full cells, on screen not.
      if ( painter.device()->isExtDev() ) {
        if ( sheetDir == KSpreadSheet::RightToLeft )
          painter.drawLine( QMAX( zrect_left, zcellRect_right ),
                            QMAX( zrect_top, zcellRect_bottom - bottom ),
                            QMIN( zrect_right, zcellRect_right ),
                            QMIN( zrect_bottom, zcellRect_bottom ) );
        else
          painter.drawLine( QMAX( zrect_left, zcellRect_left ),
                            QMAX( zrect_top, zcellRect_bottom - bottom ),
                            QMIN( zrect_right, zcellRect_left ),
                            QMIN( zrect_bottom, zcellRect_bottom ) );
      }
      else {
        if ( sheetDir == KSpreadSheet::RightToLeft )
          painter.drawLine( zcellRect_right, zcellRect_bottom - bottom,
                            zcellRect_right, zcellRect_bottom );
        else
          painter.drawLine( zcellRect_left, zcellRect_bottom - bottom,
                            zcellRect_left, zcellRect_bottom );
      }
    }

    // Fix the borders which meet at the bottom right corner
    if ( cell_south->effRightBorderValue( cellRef.x(), cellRef.y() + 1 )
         >= cell_southeast->effLeftBorderValue( cellRef.x() + 1,
						cellRef.y() + 1 ) )
      vert_pen = cell_south->effRightBorderPen( cellRef.x(), cellRef.y() + 1 );
    else
      vert_pen = cell_southeast->effLeftBorderPen( cellRef.x() + 1,
						   cellRef.y() + 1 );

    // vert_pen = effRightBorderPen( cellRef.x(), cellRef.y() + 1 );
    vert_penWidth = QMAX( 1, doc->zoomItY( vert_pen.width() ) );
    vert_pen.setWidth( vert_penWidth );
    if ( ( vert_pen.style() != Qt::NoPen ) && ( cellRef.x() < KS_colMax ) ) {
      if ( cell_east ->effBottomBorderValue( cellRef.x() + 1, cellRef.y() )
           >= cell_southeast->effTopBorderValue( cellRef.x() + 1,
						 cellRef.y() + 1 ) )

        horz_pen = m_pSheet->cellAt( cellRef.x() + 1, cellRef.y() )
	  ->effBottomBorderPen( cellRef.x() + 1, cellRef.y() );
      else
        horz_pen = m_pSheet->cellAt( cellRef.x() + 1, cellRef.y() + 1 )
	  ->effTopBorderPen( cellRef.x() + 1, cellRef.y() + 1 );

      // horz_pen = effBottomBorderPen( cellRef.x() + 1, cellRef.y() );
      horz_penWidth = QMAX( 1, doc->zoomItX( horz_pen.width() ) );
      int bottom = ( QMAX( 0, -1 + horz_penWidth ) ) / 2;

      painter.setPen( vert_pen );
      // If we are on paper printout, we limit the length of the lines.
      // On paper, we always have full cells, on screen not.
      if ( painter.device()->isExtDev() )      {
        if ( sheetDir == KSpreadSheet::RightToLeft )
          painter.drawLine( QMAX( zrect_left, zcellRect_left ),
                            QMAX( zrect_top, zcellRect_bottom - bottom ),
                            QMIN( zrect_right, zcellRect_left ),
                            QMIN( zrect_bottom, zcellRect_bottom ) );
        else
          painter.drawLine( QMAX( zrect_left, zcellRect_right ),
                            QMAX( zrect_top, zcellRect_bottom - bottom ),
                            QMIN( zrect_right, zcellRect_right ),
                            QMIN( zrect_bottom, zcellRect_bottom ) );
      }
      else {
        if ( sheetDir == KSpreadSheet::RightToLeft )
          painter.drawLine( zcellRect_left, zcellRect_bottom - bottom,
                            zcellRect_left, zcellRect_bottom );
        else
          painter.drawLine( zcellRect_right, zcellRect_bottom - bottom,
                            zcellRect_right, zcellRect_bottom );
      }
    }
  }
}


// Paint diagonal lines through the cell.
//
void KSpreadCell::paintCellDiagonalLines( QPainter& painter,
                                          const KoRect &cellRect,
                                          const QPoint &cellRef )
{
  if ( isObscuringForced() )
    return;

  KSpreadDoc* doc = sheet()->doc();

  if ( effFallDiagonalPen( cellRef.x(), cellRef.y() ).style() != Qt::NoPen ) {
    painter.setPen( effFallDiagonalPen( cellRef.x(), cellRef.y() ) );
    painter.drawLine( doc->zoomItX( cellRect.x() ),
		      doc->zoomItY( cellRect.y() ),
		      doc->zoomItX( cellRect.right() ),
		      doc->zoomItY( cellRect.bottom() ) );
  }

  if ( effGoUpDiagonalPen( cellRef.x(), cellRef.y() ).style() != Qt::NoPen ) {
    painter.setPen( effGoUpDiagonalPen( cellRef.x(), cellRef.y() ) );
    painter.drawLine( doc->zoomItX( cellRect.x() ),
		      doc->zoomItY( cellRect.bottom() ),
		      doc->zoomItX( cellRect.right() ),
		      doc->zoomItY( cellRect.y() ) );
  }
}


//                        End of Painting
// ================================================================


int KSpreadCell::defineAlignX()
{
  int a = align( column(), row() );
  if ( a == KSpreadCell::Undefined )
  {
    //numbers should be right-aligned by default, as well as BiDi text
    if ((formatType() == Text_format) || value().isString())
      a = (d->strOutText.isRightToLeft()) ?
                               KSpreadCell::Right : KSpreadCell::Left;
    else {
      KSpreadValue val = value();
      while (val.isArray()) val = val.element (0, 0);
      if (val.isBoolean() || val.isNumber())
        a = KSpreadCell::Right;
      else
        a = KSpreadCell::Left;
    }
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

// Cut strOutText, so that it only holds the part that can be displayed.
//
// Used in paintText().
//

QString KSpreadCell::textDisplaying( QPainter &_painter )
{
  QFontMetrics  fm = _painter.fontMetrics();
  int           a  = align( column(), row() );

  // If the content of the cell is a number, then we shouldn't cut
  // anything, but just display an error indication.
  //
  // FIXME: Check if this should be in class ValueFormatter() instead.
  if ( value().isNumber() )
    d->strOutText = "#####################################################";

  if ( !verticalText( column(),row() ) ) {
    // Non-vertical text: the ordinary case.

    // Not enough space but align to left
    double  len = 0.0;
    int     extraXCells = d->hasExtra() ? d->extra()->extraXCells : 0;

    for ( int i = column(); i <= column() + extraXCells; i++ ) {
      ColumnFormat *cl2 = m_pSheet->columnFormat( i );
      len += cl2->dblWidth() - 1.0; //-1.0 because the pixel in between 2 cells is shared between both cells
    }

    QString  tmp;
    double   tmpIndent = 0.0;
    if ( !isEmpty() )
      tmpIndent = getIndent( column(), row() );

    // Start out with the whole text, cut one character at a time, and
    // when the text finally fits, return it.
    for ( int i = d->strOutText.length(); i != 0; i-- ) {
      if ( a == KSpreadCell::Left || a == KSpreadCell::Undefined )
	tmp = d->strOutText.left(i);
      else if ( a == KSpreadCell::Right)
	tmp = d->strOutText.right(i);
      else
	tmp = d->strOutText.mid( ( d->strOutText.length() - i ) / 2, i);

      // 4 equal lenght of red triangle +1 point.
      if ( m_pSheet->doc()->unzoomItX( fm.width( tmp ) ) + tmpIndent
	   < len - 4.0 - 1.0 )
      {
	if ( getAngle( column(), row() ) != 0 ) {
	  QString tmp2;
	  RowFormat *rl = m_pSheet->rowFormat( row() );
	  if ( d->textHeight > rl->dblHeight() ) {
	    for ( int j = d->strOutText.length(); j != 0; j-- ) {
	      tmp2 = d->strOutText.left( j );
	      if ( m_pSheet->doc()->unzoomItY( fm.width( tmp2 ) )
		   < rl->dblHeight() - 1.0 )
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
  else if ( verticalText( column(), row() ) ) {
    // Vertical text.

    RowFormat  *rl = m_pSheet->rowFormat( row() );
    double      tmpIndent = 0.0;

    // Not enough space but align to left.
    double  len = 0.0;
    int     extraXCells = d->hasExtra() ? d->extra()->extraXCells : 0;

    for ( int i = column(); i <= column() + extraXCells; i++ ) {
      ColumnFormat  *cl2 = m_pSheet->columnFormat( i );

      // -1.0 because the pixel in between 2 cells is shared between both cells
      len += cl2->dblWidth() - 1.0;
    }

    if ( !isEmpty() )
      tmpIndent = getIndent( column(), row() );

    if ( ( d->textWidth + tmpIndent > len ) || d->textWidth == 0.0 )
      return QString( "" );

    for ( int i = d->strOutText.length(); i != 0; i-- ) {
      if ( m_pSheet->doc()->unzoomItY( fm.ascent() + fm.descent() ) * i
	   < rl->dblHeight() - 1.0 )
	return d->strOutText.left( i );
    }

    return QString( "" );
  }

  ColumnFormat  *cl = m_pSheet->columnFormat( column() );
  double         w = cl->dblWidth();

  if ( d->hasExtra() && (d->extra()->extraWidth != 0.0) )
    w = d->extra()->extraWidth;

  QString tmp;
  for ( int i = d->strOutText.length(); i != 0; i-- ) {
    tmp = d->strOutText.left( i );

    // 4 equals lenght of red triangle +1 pixel
    if ( m_pSheet->doc()->unzoomItX( fm.width( tmp ) ) < w - 4.0 - 1.0 )
      return tmp;
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

    const ColumnFormat *cl = m_pSheet->columnFormat( _col );
    return cl->dblWidth( _canvas );
  }

  if ( testFlag(Flag_ForceExtra) )
    return d->extra()->extraWidth;

  const ColumnFormat *cl = m_pSheet->columnFormat( _col );
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

    const RowFormat *rl = m_pSheet->rowFormat( _row );
    return rl->dblHeight( _canvas );
  }

  if ( testFlag(Flag_ForceExtra) )
    return d->extra()->extraHeight;

  const RowFormat *rl = m_pSheet->rowFormat( _row );
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
    KSpreadCell* cell = m_pSheet->cellAt( column() - 1, row() );
    if ( cell && cell->hasProperty( PRightBorder )
         && m_pSheet->cellAt( column(), row() ) == this )
        cell->clearProperty( PRightBorder );
  }

  KSpreadFormat::setLeftBorderPen( p );
}

void KSpreadCell::setTopBorderPen( const QPen& p )
{
  if ( row() == 1 )
  {
    KSpreadCell* cell = m_pSheet->cellAt( column(), row() - 1 );
    if ( cell && cell->hasProperty( PBottomBorder )
         && m_pSheet->cellAt( column(), row() ) == this )
        cell->clearProperty( PBottomBorder );
  }
  KSpreadFormat::setTopBorderPen( p );
}

void KSpreadCell::setRightBorderPen( const QPen& p )
{
    KSpreadCell* cell = 0L;
    if ( column() < KS_colMax )
        cell = m_pSheet->cellAt( column() + 1, row() );

    if ( cell && cell->hasProperty( PLeftBorder )
         && m_pSheet->cellAt( column(), row() ) == this )
        cell->clearProperty( PLeftBorder );

    KSpreadFormat::setRightBorderPen( p );
}

void KSpreadCell::setBottomBorderPen( const QPen& p )
{
    KSpreadCell* cell = 0L;
    if ( row() < KS_rowMax )
        cell = m_pSheet->cellAt( column(), row() + 1 );

    if ( cell && cell->hasProperty( PTopBorder )
         && m_pSheet->cellAt( column(), row() ) == this )
        cell->clearProperty( PTopBorder );

    KSpreadFormat::setBottomBorderPen( p );
}

const QPen& KSpreadCell::rightBorderPen( int _col, int _row ) const
{
    if ( !hasProperty( PRightBorder ) && ( _col < KS_colMax ) )
    {
        KSpreadCell * cell = m_pSheet->cellAt( _col + 1, _row );
        if ( cell && cell->hasProperty( PLeftBorder ) )
            return cell->leftBorderPen( _col + 1, _row );
    }

    return KSpreadFormat::rightBorderPen( _col, _row );
}

const QPen& KSpreadCell::leftBorderPen( int _col, int _row ) const
{
    if ( !hasProperty( PLeftBorder ) )
    {
        const KSpreadCell * cell = m_pSheet->cellAt( _col - 1, _row );
        if ( cell && cell->hasProperty( PRightBorder ) )
            return cell->rightBorderPen( _col - 1, _row );
    }

    return KSpreadFormat::leftBorderPen( _col, _row );
}

const QPen& KSpreadCell::bottomBorderPen( int _col, int _row ) const
{
    if ( !hasProperty( PBottomBorder ) && ( _row < KS_rowMax ) )
    {
        const KSpreadCell * cell = m_pSheet->cellAt( _col, _row + 1 );
        if ( cell && cell->hasProperty( PTopBorder ) )
            return cell->topBorderPen( _col, _row + 1 );
    }

    return KSpreadFormat::bottomBorderPen( _col, _row );
}

const QPen& KSpreadCell::topBorderPen( int _col, int _row ) const
{
    if ( !hasProperty( PTopBorder ) )
    {
        const KSpreadCell * cell = m_pSheet->cellAt( _col, _row - 1 );
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

// setCurrency - reimplemented from KSpreadFormat, adding locale support
void KSpreadCell::setCurrency( int type, QString const & symbol )
{
  Currency c;

  c.symbol = symbol.simplifyWhiteSpace();
  c.type   = type;

  if (c.symbol.length() == 0)
  {
    c.type = 0;
    c.symbol = locale()->currencySymbol();
  }
  m_pStyle = m_pStyle->setCurrency( c );

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

  d->strText.setNum( number );
  checkNumberFormat();
}

void KSpreadCell::setCellText( const QString& _text, bool asText )
{
  QString ctext = _text;
  if( ctext.length() > 5000 )
    ctext = ctext.left( 5000 );

  // empty string ?
  if (ctext.length() == 0) {
    d->strOutText = d->strText = "";
    setValue (KSpreadValue::empty());
    return;
  }

  // as text ?
  if (asText) {
    d->strOutText = ctext;
    d->strText    = ctext;
    setValue (KSpreadValue (ctext));

    return;
  }

  QString oldText = d->strText;
  setDisplayText( ctext );
  if(!m_pSheet->isLoading() && !testValidity() )
  {
    //reapply old value if action == stop
    setDisplayText( oldText );
  }
}

void KSpreadCell::setDisplayText( const QString& _text )
{
  m_pSheet->doc()->emitBeginOperation( false );
  d->strText = _text;

  /**
   * A real formula "=A1+A2*3" was entered.
   */
  if ( !d->strText.isEmpty() && d->strText[0] == '=' )
  {
    setFlag(Flag_LayoutDirty);
    setFlag(Flag_TextFormatDirty);

    if ( !m_pSheet->isLoading() )
    {
      if ( !makeFormula() )
      {
  kdError(36001) << "ERROR: Syntax ERROR" << endl;
      }
    }
  }

  /**
   * Some numeric value or a string.
   */
  else
  {
    // Find out what data type it is
    checkTextInput();

    setFlag(Flag_LayoutDirty);
    setFlag(Flag_TextFormatDirty);
  }

  m_pSheet->doc()->emitEndOperation( QRect( d->column, d->row, 1, 1 ) );
}

void KSpreadCell::setLink( const QString& link )
{
  d->extra()->link = link;

  if( !link.isEmpty() && d->strText.isEmpty() )
    setCellText( link );
}

QString KSpreadCell::link() const
{
  return d->hasExtra() ? d->extra()->link : QString::null;
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
      KSpreadCell* cell = m_pSheet->cellAt(x,y);
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
        else if ( d->extra()->validity->m_allow == Allow_List )
        {
            //test int value
            if ( value().isString() && d->extra()->validity->listValidity.contains( value().asString() ) )
                valid = true;
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

  return (formatIsTime (ft) || ((ft == Generic_format) &&
      (value().format() == KSpreadValue::fmt_Date)));
}

bool KSpreadCell::isTime() const
{
  FormatType ft = formatType();

  return (formatIsTime (ft) || ((ft == Generic_format) &&
      (value().format() == KSpreadValue::fmt_Time)));
}

void KSpreadCell::setCalcDirtyFlag()
{
  if ( !isFormula() )
  {
    //don't set the flag if we don't hold a formula
    clearFlag(Flag_CalcDirty);
    return;
  }
  setFlag(Flag_CalcDirty);
  m_pSheet->setRegionPaintDirty(cellRect());
}


bool KSpreadCell::updateChart(bool refresh)
{
    // Update a chart for example if it depends on this cell.
    if ( d->row != 0 && d->column != 0 )
    {
        CellBinding *bind;
        for ( bind = m_pSheet->firstCellBinding(); bind != 0L; bind = m_pSheet->nextCellBinding() )
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

  setValue (getDouble ());
}

void KSpreadCell::convertToPercent ()
{
  if (isDefault())
    return;

  setValue (getDouble ());
  d->value.setFormat (KSpreadValue::fmt_Percent);
}

void KSpreadCell::convertToMoney ()
{
  if (isDefault())
    return;

  setValue (getDouble ());
  d->value.setFormat (KSpreadValue::fmt_Money);
  setPrecision (locale()->fracDigits());
}

void KSpreadCell::convertToTime ()
{
  //(Tomas) This is weird. And I mean *REALLY* weird. First, we
  //generate a time (QTime), then we convert it to text, then
  //we give the text to the cell and ask it to parse it. Weird...

  if (isDefault() || isEmpty())
    return;

  setValue (getDouble ());
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

  setValue (getDouble ());

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

  // Get the text from that cell
  QString str = d->strText;

  sheet()->doc()->parser()->parse (str, this);

  // Parsing as time acts like an autoformat: we even change d->strText
  // [h]:mm:ss -> might get set by ValueParser
  if (isTime() && (formatType() != Time_format7))
    d->strText = locale()->formatTime( value().asDateTime().time(), true);

  // convert first letter to uppercase ?
  if (m_pSheet->getFirstLetterUpper() && value().isString() &&
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


// ================================================================
//                       Saving and loading


QDomElement KSpreadCell::save( QDomDocument& doc,
			       int _x_offset, int _y_offset,
			       bool force, bool copy, bool era )
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
        param.setAttribute("displayvalidationinformation",d->extra()->validity->displayValidationInformation);
        param.setAttribute("allowemptycell",d->extra()->validity->allowEmptyCell);
        if ( !d->extra()->validity->listValidity.isEmpty() )
            param.setAttribute( "listvalidity", d->extra()->validity->listValidity.join( ";" ) );
        validity.appendChild(param);
        QDomElement title = doc.createElement( "title" );
        title.appendChild( doc.createTextNode( d->extra()->validity->title ) );
        validity.appendChild( title );
        QDomElement message = doc.createElement( "message" );
        message.appendChild( doc.createCDATASection( d->extra()->validity->message ) );
        validity.appendChild( message );

        QDomElement inputTitle = doc.createElement( "inputtitle" );
        inputTitle.appendChild( doc.createTextNode( d->extra()->validity->titleInfo ) );
        validity.appendChild( inputTitle );

        QDomElement inputMessage = doc.createElement( "inputmessage" );
        inputMessage.appendChild( doc.createTextNode( d->extra()->validity->messageInfo ) );
        validity.appendChild( inputMessage );



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
        else if ( !link().isEmpty() )
        {
            // KSpread pre 1.4 saves link as rich text, marked with first char '
            // Have to be saved in some CDATA section because of too many special charatcers.
            QDomElement text = doc.createElement( "text" );
            QString qml = "!<a href=\"" + link() + "\">" + d->strText + "</a>";
            text.appendChild( doc.createCDATASection( qml ) );
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
        //<office:annotation draw:style-name="gr1" draw:text-style-name="P1" svg:width="2.899cm" svg:height="2.691cm" svg:x="2.858cm" svg:y="0.001cm" draw:caption-point-x="-2.858cm" draw:caption-point-y="-0.001cm">
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



QString KSpreadCell::saveOasisCellStyle( KoGenStyle &currentCellStyle, KoGenStyles &mainStyles, bool force, bool copy)
{
    QString formatCellStyle = KSpreadFormat::saveOasisCellStyle( currentCellStyle, mainStyles, column(), row(), force, copy );
    if ( d->hasExtra() && d->extra()->conditions )
        d->extra()->conditions->saveOasisConditions( currentCellStyle );
    return formatCellStyle;
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
    KoGenStyle currentCellStyle( KSpreadDoc::STYLE_CELL,"table-cell" );
    QString cellNumericStyle = saveOasisCellStyle( currentCellStyle,mainStyles );
    xmlwriter.addAttribute( "table:style-name", mainStyles.lookup( currentCellStyle, "ce" ) );
    if ( !cellNumericStyle.isEmpty() )
        xmlwriter.addAttribute( "style:data-style-name", cellNumericStyle );

    // group empty cells with the same style
    if ( isEmpty() && !hasProperty( KSpreadFormat::PComment ) && !isObscuringForced() && !isForceExtraCells() )
    {
      int j = column + 1;
      while ( j <= maxCols )
      {
        KSpreadCell *nextCell = m_pSheet->cellAt( j, row );
        KoGenStyle nextCellStyle( KSpreadDoc::STYLE_CELL,"table-cell" );
        nextCell->saveOasisCellStyle( nextCellStyle,mainStyles );

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


    if ( link().isEmpty() )
      saveOasisValue (xmlwriter);

    if (d->hasExtra() && d->extra()->validity)
    {
        KSpreadGenValidationStyle styleVal(d->extra()->validity);
        xmlwriter.addAttribute( "table:validation-name", valStyle.lookup( styleVal ) );
    }
    if ( isFormula() )
    {
        //kdDebug() << "Formula found" << endl;
      QString formula( convertFormulaToOasisFormat( text() ) );
      xmlwriter.addAttribute( "table:formula", formula );
    }
    else if ( !link().isEmpty() )
    {
        //kdDebug()<<"Link found \n";
        xmlwriter.startElement( "text:p" );
        xmlwriter.startElement( "text:a" );
        //Reference cell is started by "#"
        if ( localReferenceAnchor( link() ) )
            xmlwriter.addAttribute( " xlink:href", ( "#"+link() ) );
        else
            xmlwriter.addAttribute( " xlink:href", link() );
        xmlwriter.addTextNode( text() );
        xmlwriter.endElement();
        xmlwriter.endElement();
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

    if ( !isEmpty() && link().isEmpty() )
    {
        xmlwriter.startElement( "text:p" );
        xmlwriter.addTextNode(strOutText());
        xmlwriter.endElement();
    }

    saveOasisAnnotation( xmlwriter );

    xmlwriter.endElement();
    return true;
}

void KSpreadCell::saveOasisValue (KoXmlWriter &xmlWriter)
{
  switch (value().format())
  {
    case KSpreadValue::fmt_None: break;  //NOTHING HERE
    case KSpreadValue::fmt_Boolean:
    {
      xmlWriter.addAttribute( "office:value-type", "boolean" );
      xmlWriter.addAttribute( "office:boolean-value", ( value().asBoolean() ?
          "true" : "false" ) );
      break;
    }
    case KSpreadValue::fmt_Number:
    {
      xmlWriter.addAttribute( "office:value-type", "float" );
      xmlWriter.addAttribute( "office:value", QString::number( value().asFloat() ) );
      break;
    }
    case KSpreadValue::fmt_Percent:
    {
      xmlWriter.addAttribute( "office:value-type", "percentage" );
      xmlWriter.addAttribute( "office:value",
          QString::number( value().asFloat() ) );
      break;
    }
    case KSpreadValue::fmt_Money:
    {
      xmlWriter.addAttribute( "office:value-type", "currency" );
      // TODO: add code of currency
      //xmlWriter.addAttribute( "tableoffice:currency",
      // locale()->currencySymbol() );
      xmlWriter.addAttribute( "office:value",
          QString::number( value().asFloat() ) );
      break;
    }
    case KSpreadValue::fmt_DateTime: break;  //NOTHING HERE
    case KSpreadValue::fmt_Date:
    {
      xmlWriter.addAttribute( "office:value-type", "date" );
      xmlWriter.addAttribute( "office:date-value",
          value().asDate().toString( Qt::ISODate ) );
      break;
    }
    case KSpreadValue::fmt_Time:
    {
      xmlWriter.addAttribute( "office:value-type", "time" );
      xmlWriter.addAttribute( "office:time-value",
          value().asTime().toString( "PThhHmmMssS" ) );
      break;
    }
    case KSpreadValue::fmt_String:
    {
      xmlWriter.addAttribute( "office:value-type", "string" );
      xmlWriter.addAttribute( "office:string-value", value().asString() );
      break;
    }
  };
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
    //kdDebug()<<" void KSpreadCell::loadOasisConditional( QDomElement * style  :"<<style<<endl;
    if ( style )//safe
    {
        //TODO fixme it doesn't work :(((
        QDomElement e;
        forEachElement( e, style->toElement() )
        {
            kdDebug()<<"e.localName() :"<<e.localName()<<endl;
            if ( e.localName() == "map" && e.namespaceURI() == KoXmlNS::style )
            {
                if (d->hasExtra())
                    delete d->extra()->conditions;
                d->extra()->conditions = new KSpreadConditions( this );
                d->extra()->conditions->loadOasisConditions( e );
                d->extra()->conditions->checkMatches();
                break;
            }
        }

    }
}

bool KSpreadCell::loadOasis( const QDomElement &element, const KoOasisStyles& oasisStyles )
{
    QString text;
    kdDebug()<<" table:style-name :"<<element.attributeNS( KoXmlNS::table, "style-name", QString::null )<<endl;
    if ( element.hasAttributeNS( KoXmlNS::table, "style-name" ) )
    {
        QString str = element.attributeNS( KoXmlNS::table, "style-name", QString::null );
        kdDebug()<<" bool KSpreadCell::loadOasis( const QDomElement &element, const KoOasisStyles& oasisStyles ) str :"<<str<<endl;
        QDomElement * style = oasisStyles.styles()[str];
        //kdDebug()<<" style :"<<style<<endl;
        KoStyleStack styleStack;
        styleStack.push( *style );
        styleStack.setTypeProperties( "table-cell" );
        loadOasisStyleProperties( styleStack, oasisStyles );
        loadOasisConditional( style );
    }
    QDomElement textP = KoDom::namedItemNS( element, KoXmlNS::text, "p" );
    if ( !textP.isNull() )
    {
        text = textP.text(); // our text, could contain formating for value or result of formul
        setCellText( text );
        setValue( text );

        QDomElement textA = KoDom::namedItemNS( textP, KoXmlNS::text, "a" );
        if( !textA.isNull() )
        {
            if ( textA.hasAttributeNS( KoXmlNS::xlink, "href" ) )
            {
                QString link = textA.attributeNS( KoXmlNS::xlink, "href", QString::null );
                text = textA.text();
                setCellText( text );
                setValue( text );
                if ( link[0]=='#' )
                    link=link.remove( 0, 1 );
                setLink( link );
            }
        }
    }

    bool isFormula = false;
    if ( element.hasAttributeNS( KoXmlNS::table, "formula" ) )
    {
        kdDebug()<<" formula :"<<element.attributeNS( KoXmlNS::table, "formula", QString::null )<<endl;
        isFormula = true;
        QString formula;
        convertFormula( formula, element.attributeNS( KoXmlNS::table, "formula", QString::null ) );
        setCellText( formula );
    }
    if ( element.hasAttributeNS( KoXmlNS::table, "validation-name" ) )
    {
        kdDebug()<<" Cel has a validation :"<<element.attributeNS( KoXmlNS::table, "validation-name", QString::null )<<endl;
        loadOasisValidation( element.attributeNS( KoXmlNS::table, "validation-name", QString::null ) );
    }
    if( element.hasAttributeNS( KoXmlNS::office, "value-type" ) )
    {
        QString valuetype = element.attributeNS( KoXmlNS::office, "value-type", QString::null );
        if( valuetype == "boolean" )
        {
            QString val = element.attributeNS( KoXmlNS::office, "boolean-value", QString::null );
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
            double value = element.attributeNS( KoXmlNS::office, "value", QString::null ).toDouble( &ok );
            if ( !isFormula )
                if( ok )
                    setValue( value );
        }

        // currency value
        else if( valuetype == "currency" )
        {
            bool ok = false;
            double value = element.attributeNS( KoXmlNS::office, "value", QString::null ).toDouble( &ok );
            if( ok )
            {
                if ( !isFormula )
                    setValue( value );
                setCurrency( 1, element.attributeNS( KoXmlNS::office, "currency", QString::null ) );
                setFormatType (Money_format);
            }
        }
        else if( valuetype == "percentage" )
        {
            bool ok = false;
            double value = element.attributeNS( KoXmlNS::office, "value", QString::null ).toDouble( &ok );
            if( ok )
            {
                if ( !isFormula )
                    setValue( value );
                setFormatType (Percentage_format);
            }
        }
        else if ( valuetype == "date" )
        {
            QString value = element.attributeNS( KoXmlNS::office, "value", QString::null );
            if ( value.isEmpty() )
                value = element.attributeNS( KoXmlNS::office, "date-value", QString::null );
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
            QString value = element.attributeNS( KoXmlNS::office, "value", QString::null );
            if ( value.isEmpty() )
                value = element.attributeNS( KoXmlNS::office, "time-value", QString::null );
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
            QString value = element.attributeNS( KoXmlNS::office, "value", QString::null );
            if ( value.isEmpty() )
                value = element.attributeNS( KoXmlNS::office, "string-value", QString::null );
            setValue( value );
            setFormatType (Text_format);
        }
        else
            kdDebug()<<" type of value found : "<<valuetype<<endl;
    }
    // merged cells ?
    int colSpan = 1;
    int rowSpan = 1;
    if ( element.hasAttributeNS( KoXmlNS::table, "number-columns-spanned" ) )
    {
        bool ok = false;
        int span = element.attributeNS( KoXmlNS::table, "number-columns-spanned", QString::null ).toInt( &ok );
        if( ok ) colSpan = span;
    }
    if ( element.hasAttributeNS( KoXmlNS::table, "number-rows-spanned" ) )
    {
        bool ok = false;
        int span = element.attributeNS( KoXmlNS::table, "number-rows-spanned", QString::null ).toInt( &ok );
        if( ok ) rowSpan = span;
    }
    if ( colSpan > 1 || rowSpan > 1 )
        forceExtraCells( d->column, d->row, colSpan - 1, rowSpan - 1 );
    // cell comment/annotation
    QDomElement annotationElement = KoDom::namedItemNS( element, KoXmlNS::office, "annotation" );
    if ( !annotationElement.isNull() )
    {
        QString comment;
        QDomNode node = annotationElement.firstChild();
        while( !node.isNull() )
        {
            QDomElement commentElement = node.toElement();
            if( !commentElement.isNull() )
                if( commentElement.localName() == "p" && commentElement.namespaceURI() == KoXmlNS::text )
                {
                    if( !comment.isEmpty() ) comment.append( '\n' );
                    comment.append( commentElement.text() );
                }

            node = node.nextSibling();
        }

        if( !comment.isEmpty() )
            setComment( comment );
    }

    if ( element.hasAttributeNS( KoXmlNS::style, "data-style-name" ) )
    {
        QString str = element.attributeNS( KoXmlNS::style, "data-style-name", QString::null );
        //kdDebug()<<" data-style-name !"<<str<<endl;
        //kdDebug()<< " oasisStyles.dataFormats()[...] :"<< oasisStyles.dataFormats()[str].formatStr<<endl;
        //kdDebug()<< " oasisStyles.dataFormats()[...] prefix :"<< oasisStyles.dataFormats()[str].prefix<<endl;
        //kdDebug()<< " oasisStyles.dataFormats()[...] suffix :"<< oasisStyles.dataFormats()[str].suffix<<endl;
        setPrefix( oasisStyles.dataFormats()[str].prefix );
        setPostfix( oasisStyles.dataFormats()[str].suffix );
        setFormatType( KSpreadStyle::formatType( oasisStyles.dataFormats()[str].formatStr ) );
    }
    return true;
}

void KSpreadCell::loadOasisValidation( const QString& validationName )
{
    QDomElement element = sheet()->doc()->loadingInfo()->validation( validationName);
    if (d->hasExtra())
      delete d->extra()->validity;
    d->extra()->validity = new KSpreadValidity;
    if ( element.hasAttributeNS( KoXmlNS::table, "condition" ) )
    {
        QString valExpression = element.attributeNS( KoXmlNS::table, "condition", QString::null );
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
            valExpression = valExpression.remove("oooc:cell-content-text-length()" );
            kdDebug()<<" valExpression = :"<<valExpression<<endl;
            d->extra()->validity->m_allow = Allow_TextLength;

            loadOasisValidationCondition( valExpression );
        }
        else if ( valExpression.contains( "cell-content-is-text()" ) )
        {
            d->extra()->validity->m_allow = Allow_Text;
        }
        //cell-content-text-length-is-between(Value, Value) | cell-content-text-length-is-not-between(Value, Value) | cell-content-is-in-list( StringList )
        else if ( valExpression.contains( "cell-content-text-length-is-between" ) )
        {
            d->extra()->validity->m_allow = Allow_TextLength;
            d->extra()->validity->m_cond = Between;
            valExpression = valExpression.remove( "oooc:cell-content-text-length-is-between(" );
            kdDebug()<<" valExpression :"<<valExpression<<endl;
            valExpression = valExpression.remove( ")" );
            QStringList listVal = QStringList::split( ",", valExpression );
            loadOasisValidationValue( listVal );
        }
        else if ( valExpression.contains( "cell-content-text-length-is-not-between" ) )
        {
            d->extra()->validity->m_allow = Allow_TextLength;
            d->extra()->validity->m_cond = Different;
            valExpression = valExpression.remove( "oooc:cell-content-text-length-is-not-between(" );
            kdDebug()<<" valExpression :"<<valExpression<<endl;
            valExpression = valExpression.remove( ")" );
            kdDebug()<<" valExpression :"<<valExpression<<endl;
            QStringList listVal = QStringList::split( ",", valExpression );
            loadOasisValidationValue( listVal );
        }
        else if ( valExpression.contains( "cell-content-is-in-list(" ) )
        {
            d->extra()->validity->m_allow = Allow_List;
            valExpression = valExpression.remove( "oooc:cell-content-is-in-list(" );
            kdDebug()<<" valExpression :"<<valExpression<<endl;
            valExpression = valExpression.remove( ")" );
            d->extra()->validity->listValidity = QStringList::split( ";", valExpression );

        }
        //TrueFunction ::= cell-content-is-whole-number() | cell-content-is-decimal-number() | cell-content-is-date() | cell-content-is-time()
        else
        {
            if (valExpression.contains( "cell-content-is-whole-number()" ) )
            {
                d->extra()->validity->m_allow =  Allow_Number;
                valExpression = valExpression.remove( "oooc:cell-content-is-whole-number() and " );
            }
            else if (valExpression.contains( "cell-content-is-decimal-number()" ) )
            {
                d->extra()->validity->m_allow = Allow_Integer;
                valExpression = valExpression.remove( "oooc:cell-content-is-decimal-number() and " );
            }
            else if (valExpression.contains( "cell-content-is-date()" ) )
            {
                d->extra()->validity->m_allow = Allow_Date;
                valExpression = valExpression.remove( "oooc:cell-content-is-date() and " );
            }
            else if (valExpression.contains( "cell-content-is-time()" ) )
            {
                d->extra()->validity->m_allow = Allow_Time;
                valExpression = valExpression.remove( "oooc:cell-content-is-time() and " );
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
    if ( element.hasAttributeNS( KoXmlNS::table, "allow-empty-cell" ) )
    {
        kdDebug()<<" element.hasAttribute( table:allow-empty-cell ) :"<<element.hasAttributeNS( KoXmlNS::table, "allow-empty-cell" )<<endl;
        d->extra()->validity->allowEmptyCell = ( ( element.attributeNS( KoXmlNS::table, "allow-empty-cell", QString::null )=="true" ) ? true : false );
    }
    if ( element.hasAttributeNS( KoXmlNS::table, "base-cell-address" ) )
    {
        //todo what is it ?
    }

    QDomElement help = KoDom::namedItemNS( element, KoXmlNS::table, "help-message" );
    if ( !help.isNull() )
    {
        if ( help.hasAttributeNS( KoXmlNS::table, "title" ) )
        {
            kdDebug()<<"help.attribute( table:title ) :"<<help.attributeNS( KoXmlNS::table, "title", QString::null )<<endl;
            d->extra()->validity->titleInfo = help.attributeNS( KoXmlNS::table, "title", QString::null );
        }
        if ( help.hasAttributeNS( KoXmlNS::table, "display" ) )
        {
            kdDebug()<<"help.attribute( table:display ) :"<<help.attributeNS( KoXmlNS::table, "display", QString::null )<<endl;
            d->extra()->validity->displayValidationInformation = ( ( help.attributeNS( KoXmlNS::table, "display", QString::null )=="true" ) ? true : false );
        }
        QDomElement attrText = KoDom::namedItemNS( help, KoXmlNS::text, "p" );
        if ( !attrText.isNull() )
        {
            kdDebug()<<"help text :"<<attrText.text()<<endl;
            d->extra()->validity->messageInfo = attrText.text();
        }
    }

    QDomElement error = KoDom::namedItemNS( element, KoXmlNS::table, "error-message" );
    if ( !error.isNull() )
    {
        if ( error.hasAttributeNS( KoXmlNS::table, "title" ) )
            d->extra()->validity->title = error.attributeNS( KoXmlNS::table, "title", QString::null );
        if ( error.hasAttributeNS( KoXmlNS::table, "message-type" ) )
        {
            QString str = error.attributeNS( KoXmlNS::table, "message-type", QString::null );
            if ( str == "warning" )
                d->extra()->validity->m_action = Warning;
            else if ( str == "information" )
                d->extra()->validity->m_action = Information;
            else if ( str == "stop" )
                d->extra()->validity->m_action = Stop;
            else
                kdDebug()<<"validation : message type unknown  :"<<str<<endl;
        }

        if ( error.hasAttributeNS( KoXmlNS::table, "display" ) )
        {
            kdDebug()<<" display message :"<<error.attributeNS( KoXmlNS::table, "display", QString::null )<<endl;
            d->extra()->validity->displayMessage = (error.attributeNS( KoXmlNS::table, "display", QString::null )=="true");
        }
        QDomElement attrText = KoDom::namedItemNS( error, KoXmlNS::text, "p" );
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
          if ( param.hasAttribute( "displayvalidationinformation" ) )
          {
              d->extra()->validity->displayValidationInformation = ( bool )param.attribute("displayvalidationinformation").toInt();
          }
          if ( param.hasAttribute( "allowemptycell" ) )
          {
              d->extra()->validity->allowEmptyCell = ( bool )param.attribute("allowemptycell").toInt();
          }
          if ( param.hasAttribute("listvalidity") )
          {
              d->extra()->validity->listValidity=QStringList::split(";", param.attribute("listvalidity") );
          }
        }
        QDomElement inputTitle = validity.namedItem( "inputtitle" ).toElement();
        if (!inputTitle.isNull())
        {
            d->extra()->validity->titleInfo = inputTitle.text();
        }
        QDomElement inputMessage = validity.namedItem( "inputmessage" ).toElement();
        if (!inputMessage.isNull())
        {
            d->extra()->validity->messageInfo = inputMessage.text();
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
        text.setAttribute( "dataType", cell.attribute( "dataType" ) );

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
  //TODO: use converter()->asString() to generate strText

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

    if ( !m_pSheet->isLoading() ) // i.e. when pasting
      if ( !makeFormula() )
        kdError(36001) << "ERROR: Syntax ERROR" << endl;
  }
  // rich text ?
  else if (t[0] == '!' )
  {
      // KSpread pre 1.4 stores hyperlink as rich text (first char is '!')
      // extract the link and the correspoding text
      // This is a rather dirty hack, but enough for KSpread generated XML
      bool inside_tag = false;
      QString qml_text;
      QString tag;
      QString qml_link;

      for( unsigned i = 1; i < t.length(); i++ )
      {
        QChar ch = t[i];
        if( ch == '<' )
        {
          if( !inside_tag )
          {
            inside_tag = true;
            tag = QString::null;
          }
        }
        else if( ch == '>' )
        {
          if( inside_tag )
          {
            inside_tag = false;
            if( tag.startsWith( "a href=\"", true ) )
            if( tag.endsWith( "\"" ) )
              qml_link = tag.mid( 8, tag.length()-9 );
            tag = QString::null;
          }
        }
        else
        {
          if( !inside_tag )
            qml_text += ch;
          else
            tag += ch;
        }
      }

      if( !qml_link.isEmpty() )
        d->extra()->link = qml_link;
      d->strText = qml_text;
      setValue( d->strText );
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
  KLocale* locale = m_pSheet->doc()->locale();

        /* KLocale::formatNumber requires the precision we want to return.
        */
        int precision = t.length() - t.find('.') - 1;

  if ( formatType() == Percentage_format )
        {
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

  if ( !m_pSheet->isLoading() )
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

        return tmp_op;
    }

    tmp = decodeFormula( new_text, d->column, d->row );
    setFlag(Flag_LayoutDirty);
    clearAllErrors();

    return tmp;
}

QString KSpreadCell::testAnchor( int x, int y ) const
{
  if( link().isEmpty() )
    return QString::null;

  KSpreadDoc* doc = m_pSheet->doc();
  int x1 = doc->zoomItX( d->textX );
  int y1 = doc->zoomItX( d->textY - d->textHeight );
  int x2 = doc->zoomItX( d->textX + d->textWidth );
  int y2 = doc->zoomItX( d->textY );

  if( x > x1 ) if( x < x2 )
  if( y > y1 ) if( y < y2 )
    return link();

  return QString::null;
}

void KSpreadCell::sheetDies()
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
      delete d->extra()->validity;
    }

    // Unobscure cells.
    int extraXCells = d->hasExtra() ? d->extra()->extraXCells : 0;
    int extraYCells = d->hasExtra() ? d->extra()->extraYCells : 0;
    for( int x = 0; x <= extraXCells; ++x )
        for( int y = (x == 0) ? 1 : 0; // avoid looking at (+0,+0)
             y <= extraYCells; ++y )
    {
        KSpreadCell* cell = m_pSheet->cellAt( d->column + x, d->row + y );
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
  return isFormula() ? testFlag( Flag_CalcDirty ) : false;
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
      if ( sheet()->doc()->loadingInfo()->findWordInAreaList(word) )
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
    if ( sheet()->doc()->loadingInfo()->findWordInAreaList(word) )
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

#if 0 //TODO replace formula name from oocalc if it's necessary (code from oo/import)
  // replace formula names here
  if ( formula == "=MULTIPLE.OPERATIONS" )
    formula = "=MULTIPLEOPERATIONS";
#endif
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
