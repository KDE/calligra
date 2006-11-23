/* This file is part of the KDE project

   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   Copyright 2004-2005 Tomas Mecir <mecirt@gmail.com>
   Copyright 2004-2006 Inge Wallin <inge@lysator.liu.se>
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
   Copyright 1999 Michael Reiher <michael.reiher@gmx.de>
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

#include <QApplication>
#include <QPainter>
#include <QPolygon>
#include <QRegExp>

#include "Canvas.h"
#include "Condition.h"
#include "Currency.h"
#include "Damages.h"
#include "Doc.h"
#include "Format.h"
#include "Global.h"
#include "Map.h"
#include "Object.h"
#include "Sheet.h"
#include "SheetPrint.h"
#include "Style.h"
#include "StyleManager.h"
#include "Util.h"
#include "LoadingInfo.h"
#include "GenValidationStyle.h"
#include "Localization.h"
#include "Value.h"
#include "View.h"
#include "Value.h"
#include "Formula.h"
#include "Selection.h"
#include "Validity.h"
#include "ValueConverter.h"
#include "ValueFormatter.h"
#include "ValueParser.h"

#include <KoStyleStack.h>
#include <KoRect.h>
#include <KoXmlNS.h>
#include <KoDom.h>
#include <KoOasisStyles.h>
#include <KoXmlWriter.h>

#include <kmessagebox.h>

#include <kdebug.h>

#include "Cell.h"
#include "CellPrivate.h"
#include "CellView.h"

using namespace KSpread;

Cell::Cell( Sheet * _sheet, int _column, int _row )
    : d(new Private)
{
    d->row = _row;
    d->column = _column;
    d->sheet = _sheet;

    clearAllErrors();
}

Style Cell::style( int col, int row ) const
{
    Q_ASSERT( !isDefault() || (col!=0 && row!=0) );
    if ( col == 0 )
        col = this->column();
    if ( row == 0 )
        row = this->row();
    return sheet()->style( col, row );
}

void Cell::setStyle( const Style& style, int col, int row ) const
{
    if ( style.isEmpty() )
        return;
    Q_ASSERT( !isDefault() || (col!=0 && row!=0) );
    if ( col == 0 )
        col = this->column();
    if ( row == 0 )
        row = this->row();
    sheet()->setStyle( Region(QPoint(col, row)), style );
}

QString Cell::comment( int col, int row ) const
{
    Q_ASSERT( !isDefault() || (col!=0 && row!=0) );
    if ( col == 0 )
        col = this->column();
    if ( row == 0 )
        row = this->row();
    return sheet()->comment( col, row );
}

void Cell::setComment( const QString& comment, int col, int row ) const
{
    Q_ASSERT( !isDefault() || (col!=0 && row!=0) );
    if ( col == 0 )
        col = this->column();
    if ( row == 0 )
        row = this->row();
    sheet()->setComment( Region(QPoint(col, row)), comment );
}

QSharedDataPointer<Conditions> Cell::conditions( int col, int row ) const
{
    Q_ASSERT( !isDefault() || (col!=0 && row!=0) );
    if ( col == 0 )
        col = this->column();
    if ( row == 0 )
        row = this->row();
    return sheet()->conditions( col, row );
}

void Cell::setConditions( QSharedDataPointer<Conditions> conditions, int col, int row ) const
{
    Q_ASSERT( !isDefault() || (col!=0 && row!=0) );
    if ( col == 0 )
        col = this->column();
    if ( row == 0 )
        row = this->row();
    sheet()->setConditions( Region(QPoint(col, row)), conditions );
}

Validity Cell::validity( int col, int row ) const
{
    Q_ASSERT( !isDefault() || (col!=0 && row!=0) );
    if ( col == 0 )
        col = this->column();
    if ( row == 0 )
        row = this->row();
    return sheet()->validity( col, row );
}

void Cell::setValidity( Validity validity, int col, int row ) const
{
    Q_ASSERT( !isDefault() || (col!=0 && row!=0) );
    if ( col == 0 )
        col = this->column();
    if ( row == 0 )
        row = this->row();
    sheet()->setValidity( Region(QPoint(col, row)), validity );
}

// Return the sheet that this cell belongs to.
Sheet * Cell::sheet() const
{
    return d->sheet;
}

// Return true if this is the default cell.
bool Cell::isDefault() const
{
  return ( d->column == 0 );
}

// Return the row number of this cell.
int Cell::row() const
{
  // Make sure this isn't called for the default cell.  This assert
  // can save you (could have saved me!) the hassle of some very
  // obscure bugs.
  Q_ASSERT( !isDefault() );
  if ( isDefault() )
  {
    kWarning(36001) << "Error: Calling Cell::row() for default cell" << endl;
    return 0;
  }

  return d->row;
}


// Return the column number of this cell.
//
int Cell::column() const
{
  // Make sure this isn't called for the default cell.  This assert
  // can save you (could have saved me!) the hassle of some very
  // obscure bugs.
  Q_ASSERT( !isDefault() );
  if ( isDefault() )
  {
    kWarning(36001) << "Error: Calling Cell::column() for default cell" << endl;
    return 0;
  }
  return d->column;
}


// Return the name of this cell, i.e. the string that the user would
// use to reference it.  Example: A1, BZ16
//
QString Cell::name() const
{
    return name( d->column, d->row );
}


// Return the name of any cell given by (col, row).
//
QString Cell::name( int col, int row )
{
    return columnName( col ) + QString::number( row );
}


// Return the name of this cell, including the sheet name.
// Example: sheet1!A5
//
QString Cell::fullName() const
{
    return fullName( sheet(), d->column, d->row );
}


// Return the full name of any cell given a sheet and (col, row).
//
QString Cell::fullName( const Sheet* s, int col, int row )
{
  return s->sheetName() + '!' + name( col, row );
}


// Return the symbolic name of the column of this cell.  Examples: A, BB.
//
QString Cell::columnName() const
{
  return columnName( d->column );
}

KLocale* Cell::locale() const
{
  return sheet()->doc()->locale();
}

// Return the symbolic name of any column.
//
QString Cell::columnName( uint column )
{
    QString   str;
    unsigned  digits = 1;
    unsigned  offset = 0;

    column--;

    if( column > 4058115285U ) return  QString("@@@");

    for( unsigned limit = 26; column >= limit+offset; limit *= 26, digits++ )
        offset += limit;

    for( unsigned col = column - offset; digits; --digits, col/=26 )
        str.prepend( QChar( 'A' + (col%26) ) );

    return str;
}


// Return true if this cell is a formula.
//
bool Cell::isFormula() const
{
    return d->strText[0] == '=';
}


// Return the input text of this cell.  This could, for instance, be a
// formula.
//
// FIXME: These two functions are inconsistently named.  It should be
//        either text() and outText() or strText() and strOutText().
//
QString Cell::text() const
{
    return d->strText;
}


// Return the out text, i.e. the text that is visible in the cells
// square when shown.  This could, for instance, be the calculated
// result of a formula.
//
QString Cell::strOutText() const
{
    return d->strOutText;
}

const Formula* Cell::formula() const
{
  return d->formula;
}


// Return the value of this cell.
//
const Value Cell::value() const
{
  return d->value;
}


// Set the value of this cell.  It also clears all errors if the value
// itself is not an error.
//
// In addition to this, it calculates the outstring and sets the dirty
// flags so that a redraw is forced.
//
void Cell::setValue( const Value& value )
{
//   kDebug() << k_funcinfo << endl;
  if (value.type() != Value::Error)
    clearAllErrors();

  //If the value has not changed then we don't need to do anything
  //(ie. no need to relayout, update dependent cells etc.),
  //unless this cell contains a formula, in which case its dependancies might have changed
  //even though the value has not.  For example, if this cell was previously empty (and its value is
  //therefore empty) and a new dependency upon an empty cell has been added.  The new value would still
  //be empty, but the dependencies need to be updated (via the call to valueChanged() below).
  if ( ( d->value == value ) && ( !isFormula() ) )
    return;

  d->value = value;

  // Format and set the outText.
  setOutputText();

  // Set the displayed text, if we hold an error value.
  if (d->value.type() == Value::Error)
    d->strOutText = d->value.errorMessage ();

  // Value of the cell has changed - trigger necessary actions
  valueChanged ();

  sheet()->setRegionPaintDirty(Region(cellRect()));
}

void Cell::setCellValue (const Value &value, FormatType fmtType, const QString &txt)
{
    if ( !txt.isNull() )
    {
        d->strText = txt;
        if ( isFormula() )
            makeFormula();
    }
    else if ( !isFormula() )
        d->strText = sheet()->doc()->converter()->asString( value ).asString();
    if ( fmtType != No_format )
    {
        Style style;
        style.setFormatType( fmtType );
        setStyle( style );
    }
    setValue( value );
}

// FIXME: Continue commenting and cleaning here (ingwa)


Cell* Cell::previousCell() const
{
    return d->previousCell;
}

Cell* Cell::nextCell() const
{
    return d->nextCell;
}

void Cell::setPreviousCell( Cell* cell )
{
    d->previousCell = cell;
}

void Cell::setNextCell( Cell* cell )
{
    d->nextCell = cell;
}

void Cell::copyFormat( const Cell* cell )
{
    Q_ASSERT( !isDefault() ); // trouble ahead...
    Q_ASSERT( cell );

    d->value.setFormat(cell->d->value.format());
    setStyle( cell->style() );

    QSharedDataPointer<Conditions> conditions = cell->conditions();
    if ( conditions )
    {
        QLinkedList<Conditional> conditionList = conditions->conditionList();
        setConditionList( conditionList );
    }
}

void Cell::copyAll( Cell *cell )
{
    Q_ASSERT( !isDefault() ); // trouble ahead...
    copyFormat( cell );
    copyContent( cell );
}

void Cell::copyContent( const Cell* cell )
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

void Cell::defaultStyle()
{
    Style style;
    style.setDefault();
    setStyle( style );

    setConditions( QSharedDataPointer<Conditions>() );
    setValidity( Validity() );
}


// Merge a number of cells, i.e. make this cell obscure a number of
// other cells.  If _x and _y == 0, then the merging is removed.

void Cell::mergeCells( int _col, int _row, int _x, int _y )
{
  // Start by unobscuring the cells that we obscure right now
  int  extraXCells = d->hasExtra() ? d->extra()->extraXCells : 0;
  int  extraYCells = d->hasExtra() ? d->extra()->extraYCells : 0;
  for ( int x = _col; x <= _col + extraXCells; ++x ) {
    for ( int y = _row; y <= _row + extraYCells; ++y ) {
      if ( x != _col || y != _row )
        sheet()->cellAt( x, y )->unobscure(this);
    }
  }

  // If no merging, then remove all traces, and return.
  if ( _x == 0 && _y == 0 ) {
    clearFlag( Flag_Merged );
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

  // At this point, we know that we will merge some cells.
  setFlag(Flag_Merged);
  d->extra()->extraXCells  = _x;
  d->extra()->extraYCells  = _y;
  d->extra()->mergedXCells = _x;
  d->extra()->mergedYCells = _y;

  // Obscure the cells
  for ( int x = _col; x <= _col + _x; ++x ) {
    for ( int y = _row; y <= _row + _y; ++y ) {
      if ( x != _col || y != _row )
  sheet()->nonDefaultCell( x, y )->obscure( this, true );
    }
  }

  // Refresh the layout
  setFlag( Flag_LayoutDirty );
}

void Cell::move( int col, int row )
{
    // For the old position (the new is handled in valueChanged() at the end):
    setFlag( Flag_TextFormatDirty );
    setLayoutDirtyFlag( false );
    sheet()->doc()->addDamage( new CellDamage( this, CellDamage::Value ) );

    //int ex = extraXCells();
    //int ey = d->extra()->extraYCells();

    if (d->hasExtra())
    {
      d->extra()->obscuringCells.clear();
      d->extra()->mergedXCells = 0;
      d->extra()->mergedYCells = 0;
    }

    // Unobscure the objects we obscure right now
    freeAllObscuredCells();

    // move us
    d->column = col;
    d->row    = row;

    // Cell value has been changed (because we're another cell now).
    valueChanged ();
}

void Cell::setLayoutDirtyFlag( bool format )
{
    setFlag( Flag_LayoutDirty );
    if ( format )
        setFlag( Flag_TextFormatDirty );

    if (!d->hasExtra())
      return;

    QList<Cell*>::iterator it  = d->extra()->obscuringCells.begin();
    QList<Cell*>::iterator end = d->extra()->obscuringCells.end();
    for ( ; it != end; ++it ) {
      (*it)->setLayoutDirtyFlag( format );
    }
}

bool Cell::needsPrinting() const
{
    if ( isDefault() )
        return false;

    if ( !d->strText.trimmed().isEmpty() )
        return true;

    const Style style = this->style();

    // Cell borders?
    if ( style.hasAttribute( Style::TopPen ) ||
         style.hasAttribute( Style::LeftPen ) ||
         style.hasAttribute( Style::RightPen ) ||
         style.hasAttribute( Style::BottomPen ) ||
         style.hasAttribute( Style::FallDiagonalPen ) ||
         style.hasAttribute( Style::GoUpDiagonalPen ) )
        return true;

    // Background color or brush?
    if ( style.hasAttribute( Style::BackgroundBrush ) ) {
        QBrush brush = style.backgroundBrush();

        // Only brushes that are visible (ie. they have a brush style
        // and are not white) need to be drawn
        if ( (brush.style() != Qt::NoBrush) &&
             (brush.color() != Qt::white || !brush.texture().isNull()) )
            return true;
    }

    if ( style.hasAttribute( Style::BackgroundColor ) ) {
        kDebug(36004) << "needsPrinting: Has background color" << endl;
        QColor backgroundColor = style.backgroundColor();

        // We don't need to print anything if the background is white
        if (backgroundColor != Qt::white)
            return true;
    }

    return false;
}

bool Cell::isEmpty() const
{
    return isDefault() || d->strText.isEmpty();
}


// Return true if this cell is obscured by some other cell.

bool Cell::isObscured() const
{
  if (!d->hasExtra())
    return false;

  return !( d->extra()->obscuringCells.isEmpty() );
}


// Return true if this cell is part of a merged cell, but not the
// master cell.

bool Cell::isPartOfMerged() const
{
  if (!d->hasExtra())
    return false;

  QList<Cell*>::const_iterator it = d->extra()->obscuringCells.begin();
  QList<Cell*>::const_iterator end = d->extra()->obscuringCells.end();
  for ( ; it != end; ++it ) {
    Cell *cell = *it;

    if (cell->doesMergeCells()) {
      // The cell might merge extra cells, and then overlap even
      // beyond that so just knowing that the obscuring cell merges
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

Cell *Cell::ultimateObscuringCell() const
{
  if (!d->hasExtra())
    return (Cell *) this;

  else if (d->extra()->obscuringCells.isEmpty())
    return (Cell *) this;

  else
    return d->extra()->obscuringCells.first();

#if 0
  QList<Cell*>::const_iterator it = d->extra()->obscuringCells.begin();
  QList<Cell*>::const_iterator end = d->extra()->obscuringCells.end();
  for ( ; it != end; ++it ) {
    Cell *cell = *it;

    if (cell->doesMergeCells()) {
      // The cell might merge extra cells, and then overlap even
      // beyond that so just knowing that the obscuring cell merges
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


QList<Cell*> Cell::obscuringCells() const
{
  if (!d->hasExtra())
  {
    QList<Cell*> empty;
    return empty;
  }
  return d->extra()->obscuringCells;
}

void Cell::clearObscuringCells()
{
  if (!d->hasExtra())
    return;
  d->extra()->obscuringCells.clear();
}

void Cell::obscure( Cell *cell, bool isForcing )
{
  if (d->hasExtra())
  {
    d->extra()->obscuringCells.removeAll( cell ); // removes *all* occurrences
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
  sheet()->setRegionPaintDirty(Region(cellRect()));
}

void Cell::unobscure( Cell * cell )
{
  if (d->hasExtra())
    d->extra()->obscuringCells.removeAll( cell );
  setFlag( Flag_LayoutDirty );
  sheet()->setRegionPaintDirty(Region(cellRect()));
}

QString Cell::encodeFormula( bool _era, int _col, int _row ) const
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
                tmp = '$';
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
                    tmp += '$';
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

QString Cell::decodeFormula( const QString &_text, int _col, int _row) const
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
                kDebug(36003) << "Cell::decodeFormula: row or column out of range (col: " << col << " | row: " << row << ')' << endl;
                erg = "=\"#### " + i18n("REFERENCE TO COLUMN OR ROW IS OUT OF RANGE") + '"';
                return erg;
            }
            if ( abs1 )
                erg += '$';
            erg += Cell::columnName(col); //Get column text

            if ( abs2 )
                erg += '$';
            erg += QString::number( row );
        }
        else
            erg += _text[pos++];
    }

    return erg;
}


void Cell::freeAllObscuredCells()
{
    //
    // Free all obscured cells.
    //

  if (!d->hasExtra())
    return;

  const int extraXCells = d->extra()->extraXCells;
  const int extraYCells = d->extra()->extraYCells;
  for ( int x = d->column + d->extra()->mergedXCells; x <= d->column + extraXCells; ++x ) {
    for ( int y = d->row + d->extra()->mergedYCells; y <= d->row + extraYCells; ++y ) {
      if ( x != d->column || y != d->row ) {
        Cell *cell = sheet()->cellAt( x, y );
        cell->unobscure(this);
      }
    }
  }

  d->extra()->extraXCells = d->extra()->mergedXCells;
  d->extra()->extraYCells = d->extra()->mergedYCells;
}



void Cell::valueChanged ()
{
  // Those obscuring us need to redo their layout cause they can't obscure us
  // now that we've got text, but their text has not changed.
  setLayoutDirtyFlag( false );
  setFlag( Flag_TextFormatDirty );
  // Those we've obscured also need a relayout.
  for (int x = d->column; x <= d->column + extraXCells(); x++)
  {
    for (int y = d->row; y <= d->row + extraYCells(); y++)
    {
      Cell* cell = sheet()->cellAt(x,y);
      cell->setLayoutDirtyFlag();
    }
  }

  /* TODO - is this a good place for this? */
  updateChart( true );

  sheet()->doc()->addDamage( new CellDamage( this, CellDamage::Value ) );
}


// Recalculate d->strOutText.
//

void Cell::setOutputText()
{
  if ( isDefault() ) {
    d->strOutText.clear();
    return;
  }

  // If nothing has changed, we don't need to remake the text layout.
  if ( !testFlag(Flag_TextFormatDirty) )
    return;

  // We don't want to remake the layout unnecessarily.
  clearFlag( Flag_TextFormatDirty );

  // Display a formula if warranted.  If not, display the value instead;
  // this is the most common case.
  if ( (!hasError()) && isFormula() && sheet()->getShowFormula()
       && !( sheet()->isProtected() && style().hideFormula() ) || isEmpty() )
    d->strOutText = d->strText;
  else
    d->strOutText = sheet()->doc()->formatter()->formatText(this, formatType());

#if 0 // KSPREAD_NEW_STYLE_STORAGE // conditions
  // Check conditions if needed.
  if ( d->hasExtra() && d->extra()->conditions )
    d->extra()->conditions->checkMatches( this );
#endif
}




// ----------------------------------------------------------------
//                          Formula handling


bool Cell::makeFormula()
{
//   kDebug(36002) << k_funcinfo << endl;

  d->formula = new KSpread::Formula (sheet(), this);
  d->formula->setExpression (d->strText);

  if (!d->formula->isValid ()) {
  // Did a syntax error occur ?
    clearFormula();

    if (sheet()->doc()->showMessageError())
    {
      QString tmp(i18n("Error in cell %1\n\n"));
      tmp = tmp.arg( fullName() );
      KMessageBox::error( (QWidget*)0, tmp);
    }
    setFlag(Flag_ParseError);
    Value value;
    value.setError ( ValueFormatter::errorFormat(this) );
    setValue (value);
    return false;
  }

  // Update the dependencies and recalculate.
  sheet()->doc()->addDamage( new CellDamage( this, CellDamage::Formula |
                                                             CellDamage::Value ) );

  return true;
}

void Cell::clearFormula()
{
    // Update the dependencies, if this was a formula.
    if (d->formula)
    {
        if ( !sheet()->isLoading() )
        {
            kDebug(36002) << "This was a formula. Dependency update triggered." << endl;
            sheet()->doc()->addDamage( new CellDamage( this, CellDamage::Formula ) );
        }
        delete d->formula;
        d->formula = 0;
    }
}

bool Cell::calc(bool delay)
{
  if ( !testFlag( Flag_CalcDirty ) )
    return true;

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

  if ( delay )
  {
    if ( sheet()->doc()->delayCalculation() )
      return true;
  }

  Value result = d->formula->eval ();
  setValue (result);
  if (result.isNumber())
    checkNumberFormat(); // auto-chooses number or scientific

  // At last we can reset the calc dirty flag.
  clearFlag(Flag_CalcDirty);

  return true;
}




int Cell::defineAlignX()
{
    int align = style().halign();
    if ( align == Style::HAlignUndefined )
    {
        //numbers should be right-aligned by default, as well as BiDi text
        if ((formatType() == Text_format) || value().isString())
            align = (d->strOutText.isRightToLeft()) ? Style::Right : Style::Left;
        else
        {
            Value val = value();
            while (val.isArray()) val = val.element (0, 0);
            if (val.isBoolean() || val.isNumber())
                align = Style::Right;
            else
                align = Style::Left;
        }
    }
    return align;
}

double Cell::dblWidth( int _col ) const
{
  if ( _col < 0 )
    _col = d->column;

  if ( testFlag(Flag_Merged) )
    return d->extra()->extraWidth;

  const ColumnFormat *cl = sheet()->columnFormat( _col );
  return cl->dblWidth();
}

int Cell::width( int _col ) const
{
  return int( dblWidth( _col ) );
}

double Cell::dblHeight( int _row ) const
{
  if ( _row < 0 )
    _row = d->row;

  if ( testFlag(Flag_Merged) )
    return d->extra()->extraHeight;

  const RowFormat *rl = sheet()->rowFormat( _row );
  return rl->dblHeight();
}

int Cell::height( int _row ) const
{
  return int( dblHeight( _row ) );
}

///////////////////////////////////////////
//
// Precision
//
///////////////////////////////////////////

void Cell::incPrecision()
{
    //TODO: This is ugly. Why not simply regenerate the text to display? Tomas

    if ( !value().isNumber() )
        return;
    int tmpPreci = style().precision();

    Style style;
    if ( tmpPreci == -1 )
    {
        int pos = d->strOutText.indexOf( locale()->decimalSymbol() );
        if ( pos == -1 )
            pos = d->strOutText.indexOf('.');
        if ( pos == -1 )
            style.setPrecision( 1 );
        else
        {
            int start = 0;
            if ( d->strOutText.indexOf('%') != -1 )
                start = 2;
            else if ( d->strOutText.indexOf(locale()->currencySymbol()) == ((int)(d->strOutText.length()-locale()->currencySymbol().length())) )
                start = locale()->currencySymbol().length() + 1;
            else if ( (start=d->strOutText.indexOf('E')) != -1 )
                start = d->strOutText.length() - start;

            //kDebug() << "start=" << start << " pos=" << pos << " length=" << d->strOutText.length() << endl;
            style.setPrecision( qMax( 0, (int)d->strOutText.length() - start - pos ) );
        }
    }
    else if ( tmpPreci < 10 )
        style.setPrecision( ++tmpPreci );
    setStyle( style );
    setFlag(Flag_LayoutDirty);
}

void Cell::decPrecision()
{
    //TODO: This is ugly. Why not simply regenerate the text to display? Tomas

    if ( !value().isNumber() )
        return;
    int preciTmp = style().precision();

    //  kDebug() << "decPrecision: tmpPreci = " << tmpPreci << endl;
    Style style;
    if ( preciTmp == -1 )
    {
        int pos = d->strOutText.indexOf( locale()->decimalSymbol() );
        int start = 0;
        if ( d->strOutText.indexOf('%') != -1 )
            start = 2;
        else if ( d->strOutText.indexOf(locale()->currencySymbol()) == ((int)(d->strOutText.length()-locale()->currencySymbol().length())) )
            start = locale()->currencySymbol().length() + 1;
        else if ( (start = d->strOutText.indexOf('E')) != -1 )
            start = d->strOutText.length() - start;
        else
            start = 0;

        if ( pos == -1 )
            return;

        style.setPrecision(d->strOutText.length() - pos - 2 - start);
        //   if ( preciTmp < 0 )
        //      format()->setPrecision( preciTmp );
    }
    else if ( preciTmp > 0 )
        style.setPrecision( --preciTmp );
    setStyle( style );
    setFlag( Flag_LayoutDirty );
}

//set numerical value
//used in CSVFilter::convert (nowhere else yet)
void Cell::setNumber( double number )
{
  setValue( Value( number ) );

  d->strText.setNum( number );
  setDisplayText(d->strText);
  checkNumberFormat();
}

void Cell::setCellText( const QString& _text, bool asText )
{
//   kDebug() << k_funcinfo << endl;

  // Clears the formula and updates the dependencies, if a formula exist.
  clearFormula();

  // empty string?
  if (_text.isEmpty())
  {
    d->strText.clear();
    d->strOutText.clear();
    setValue( Value::empty() );
    return;
  }

  // as text?
  if (asText)
  {
    d->strOutText = _text;
    d->strText    = _text;
    setValue( Value( _text ) );
    return;
  }

  const QString oldText = d->strText;
  setDisplayText( _text );
  Validity validity = this->validity();
  if ( !sheet()->isLoading() && !validity.testValidity( this ) )
  {
    //reapply old value if action == stop
    setDisplayText( oldText );
  }
}

void Cell::setDisplayText( const QString& _text )
{
//   kDebug() << k_funcinfo << endl;
  const bool isLoading = sheet()->isLoading();

  if ( !isLoading )
    sheet()->doc()->emitBeginOperation( false );

  d->strText = _text;

  // A real formula "=A1+A2*3" was entered.
  if ( !d->strText.isEmpty() && d->strText[0] == '=' )
  {
    makeFormula();
  }
  // Some numeric value or a string.
  else
  {
    // Find out what data type it is
    checkTextInput();
  }

  if ( !isLoading )
    sheet()->doc()->emitEndOperation( Region( QRect( d->column, d->row, 1, 1 ) ) );
}

void Cell::setLink( const QString& link )
{
  d->extra()->link = link;

  if( !link.isEmpty() && d->strText.isEmpty() )
    setCellText( link );
}

QString Cell::link() const
{
  return d->hasExtra() ? d->extra()->link : QString();
}

FormatType Cell::formatType() const
{
    return style().formatType();
}

int Cell::mergedXCells() const
{
    return d->hasExtra() ? d->extra()->mergedXCells : 0;
}

int Cell::mergedYCells() const
{
    return d->hasExtra() ? d->extra()->mergedYCells : 0;
}

int Cell::extraXCells() const
{
    return d->hasExtra() ? d->extra()->extraXCells : 0;
}

int Cell::extraYCells() const
{
    return d->hasExtra() ? d->extra()->extraYCells : 0;
}

double Cell::extraWidth() const
{
    return d->hasExtra() ? d->extra()->extraWidth : 0;
}

double Cell::extraHeight() const
{
    return d->hasExtra() ? d->extra()->extraHeight : 0;
}


bool Cell::isDate() const
{
  FormatType ft = formatType();

  return (formatIsDate (ft) || ((ft == Generic_format) &&
      (value().format() == Value::fmt_Date)));
}

bool Cell::isTime() const
{
  FormatType ft = formatType();

  return (formatIsTime (ft) || ((ft == Generic_format) &&
      (value().format() == Value::fmt_Time)));
}


bool Cell::updateChart(bool refresh)
{
    // Update a chart for example if it depends on this cell.
    if ( d->row != 0 && d->column != 0 )
    {
        foreach ( CellBinding* binding, sheet()->cellBindings() )
        {
            if ( binding->contains( d->column, d->row ) )
            {
                if (!refresh)
                    return true;

                binding->cellChanged( this );
            }
        }
        return true;
    }
    return false;

}

double Cell::getDouble ()
{
  if (isDefault())
    return 0.0;
  //(Tomas) umm can't we simply call value().asFloat() ?
  if (isDate())
  {
    QDate date = value().asDate( sheet()->doc() );
    QDate dummy (1900, 1, 1);
    return (dummy.daysTo (date) + 1);
  }
  if (isTime())
  {
    QTime time  = value().asTime( sheet()->doc() );
    QTime dummy;
    return dummy.secsTo( time );
  }
  if (value().isNumber())
    return value().asFloat();

  return 0.0;
}

void Cell::convertToDouble ()
{
  if (isDefault())
    return;

  setValue (Value(getDouble ()));
}

void Cell::convertToPercent ()
{
  if (isDefault())
    return;

  setValue (Value(getDouble ()));
  d->value.setFormat (Value::fmt_Percent);
}

void Cell::convertToMoney ()
{
  if (isDefault())
    return;

  setValue (Value(getDouble ()));
  d->value.setFormat (Value::fmt_Money);
  Style style;
  style.setPrecision (locale()->fracDigits());
  setStyle( style );
}

void Cell::convertToTime ()
{
  //(Tomas) This is weird. And I mean *REALLY* weird. First, we
  //generate a time (QTime), then we convert it to text, then
  //we give the text to the cell and ask it to parse it. Weird...

  if (isDefault() || isEmpty())
    return;

  setValue (Value(getDouble ()));
  QTime time = value().asDateTime( sheet()->doc() ).time();
  int msec = (int) ( (value().asFloat() - (int) value().asFloat()) * 1000 );
  time = time.addMSecs( msec );
  setCellText( time.toString() );
}

void Cell::convertToDate ()
{
  //(Tomas) This is weird. And I mean *REALLY* weird. First, we
  //generate a date (QDate), then we convert it to text, then
  //we give the text to the cell and ask it to parse it. Weird...

  if (isDefault() || isEmpty())
    return;

  setValue (Value(getDouble ()));

  //TODO: why did we call setValue(), when we override it here?
  QDate date(1900, 1, 1);
  date = date.addDays( (int) value().asFloat() - 1 );
  date = value().asDateTime( sheet()->doc() ).date();
  setCellText (locale()->formatDate (date, true));
}

void Cell::checkTextInput()
{
//   kDebug() << k_funcinfo << endl;
  // Goal of this method: determine the value of the cell
  clearAllErrors();

  d->value = Value::empty();

  // Get the text from that cell
  QString str = d->strText;

  // Parses the text and sets its value appropriately (calls Cell::setValue).
  sheet()->doc()->parser()->parse (str, this);

  // Parsing as time acts like an autoformat: we even change d->strText
  // [h]:mm:ss -> might get set by ValueParser
  if (isTime() && (formatType() != Time_format7))
    d->strText = locale()->formatTime( value().asDateTime( sheet()->doc() ).time(), true);

  // convert first letter to uppercase ?
  if (sheet()->getFirstLetterUpper() && value().isString() &&
      (!d->strText.isEmpty()))
  {
    QString str = value().asString();
    setValue( Value( str[0].toUpper() + str.right( str.length()-1 ) ) );
  }
}

//used in calc, setNumber, ValueParser
void Cell::checkNumberFormat()
{
    if ( formatType() == Number_format && value().isNumber() )
    {
        if ( value().asFloat() > 1e+10 )
        {
            Style style;
            style.setFormatType( Scientific_format );
            setStyle( style );
        }
    }
}


// ================================================================
//                       Saving and loading


QDomElement Cell::save( QDomDocument& doc,
             int _x_offset, int _y_offset,
             bool force, bool copy, bool era )
{
    // Save the position of this cell
    QDomElement cell = doc.createElement( "cell" );
    cell.setAttribute( "row", row() - _y_offset );
    cell.setAttribute( "column", column() - _x_offset );

    //
    // Save the formatting information
    //
    QDomElement formatElement( doc.createElement( "format" ) );
    style().saveXML( doc, formatElement, force, copy );
    if ( formatElement.hasChildNodes() || formatElement.attributes().length() ) // don't save empty tags
        cell.appendChild( formatElement );

    if ( doesMergeCells() )
    {
        if ( extraXCells() )
            formatElement.setAttribute( "colspan", extraXCells() );
        if ( extraYCells() )
            formatElement.setAttribute( "rowspan", extraYCells() );
    }

    QSharedDataPointer<Conditions> conditions = this->conditions();
    if ( conditions )
    {
        QDomElement conditionElement = conditions->saveConditions( doc );
        if ( !conditionElement.isNull() )
            cell.appendChild( conditionElement );
    }

    Validity validity = this->validity();
    if ( !validity.isEmpty() )
    {
        QDomElement validityElement = validity.saveXML( doc );
        if ( !validityElement.isNull() )
            cell.appendChild( validityElement );
    }

    const QString comment = this->comment();
    if ( !comment.isEmpty() )
    {
        QDomElement commentElement = doc.createElement( "comment" );
        commentElement.appendChild( doc.createCDATASection( comment ) );
        cell.appendChild( commentElement );
    }

    //
    // Save the text
    //
    if ( !text().isEmpty() )
    {
        // Formulas need to be encoded to ensure that they
        // are position independent.
        if ( isFormula() )
        {
            QDomElement txt = doc.createElement( "text" );
            // if we are cutting to the clipboard, relative references need to be encoded absolutely
            txt.appendChild( doc.createTextNode( encodeFormula( era ) ) );
            cell.appendChild( txt );

            /* we still want to save the results of the formula */
            QDomElement formulaResult = doc.createElement( "result" );
            saveCellResult( doc, formulaResult, strOutText() );
            cell.appendChild( formulaResult );

        }
        else if ( !link().isEmpty() )
        {
            // KSpread pre 1.4 saves link as rich text, marked with first char '
            // Have to be saved in some CDATA section because of too many special charatcers.
            QDomElement txt = doc.createElement( "text" );
            QString qml = "!<a href=\"" + link() + "\">" + text() + "</a>";
            txt.appendChild( doc.createCDATASection( qml ) );
            cell.appendChild( txt );
        }
        else
        {
            // Save the cell contents (in a locale-independent way)
            QDomElement txt = doc.createElement( "text" );
            saveCellResult( doc, txt, text() );
            cell.appendChild( txt );
        }
    }
    if ( cell.hasChildNodes() || cell.attributes().length() > 2 ) // don't save empty tags
        // (the >2 is due to "row" and "column" attributes)
        return cell;
    else
        return QDomElement();
}

bool Cell::saveCellResult( QDomDocument& doc, QDomElement& result,
                                  QString str )
{
  QString dataType = "Other"; // fallback

  if ( value().isNumber() )
  {
      if ( isDate() )
      {
          // serial number of date
          QDate dd = value().asDateTime( sheet()->doc() ).date();
          dataType = "Date";
          str = "%1/%2/%3";
          str = str.arg(dd.year()).arg(dd.month()).arg(dd.day());
      }
      else if( isTime() )
      {
          // serial number of time
          dataType = "Time";
          str = value().asDateTime( sheet()->doc() ).time().toString();
      }
      else
      {
          // real number
          dataType = "Num";
          if (value().isInteger())
            str = QString::number(value().asInteger());
          else
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

void Cell::saveOasisAnnotation( KoXmlWriter &xmlwriter, int row, int column )
{
    const QString comment = this->comment( column, row );
    if ( !comment.isEmpty() )
    {
        //<office:annotation draw:style-name="gr1" draw:text-style-name="P1" svg:width="2.899cm" svg:height="2.691cm" svg:x="2.858cm" svg:y="0.001cm" draw:caption-point-x="-2.858cm" draw:caption-point-y="-0.001cm">
        xmlwriter.startElement( "office:annotation" );
        QStringList text = comment.split( "\n", QString::SkipEmptyParts );
        for ( QStringList::Iterator it = text.begin(); it != text.end(); ++it ) {
            xmlwriter.startElement( "text:p" );
            xmlwriter.addTextNode( *it );
            xmlwriter.endElement();
        }
        xmlwriter.endElement();
    }
}

QString Cell::saveOasisCellStyle( KoGenStyle &currentCellStyle, KoGenStyles &mainStyles, int col, int row )
{
    QSharedDataPointer<Conditions> conditions = this->conditions( col, row );
    if ( conditions )
    {
        // this has to be an automatic style
        currentCellStyle = KoGenStyle( Doc::STYLE_CELL_AUTO, "table-cell" );
        conditions->saveOasisConditions( currentCellStyle );
    }
    return style( col, row ).saveOasis( currentCellStyle, mainStyles );
}


bool Cell::saveOasis( KoXmlWriter& xmlwriter, KoGenStyles &mainStyles,
                      int row, int column, int &repeated,
                      GenValidationStyles &valStyle )
{
    if ( !isPartOfMerged() )
        xmlwriter.startElement( "table:table-cell" );
    else
        xmlwriter.startElement( "table:covered-table-cell" );
#if 0
    //add font style
    QFont font;
    Value const value( cell->value() );
    if ( !cell->isDefault() )
    {
      font = cell->format()->textFont( i, row );
      m_styles.addFont( font );

      if ( cell->format()->hasProperty( Style::SComment ) )
        hasComment = true;
    }
#endif
    // NOTE save the value before the style as long as the Formatter does not work correctly
    if ( link().isEmpty() )
      saveOasisValue (xmlwriter);

    KoGenStyle currentCellStyle; // the type determined in saveOasisCellStyle
    saveOasisCellStyle( currentCellStyle, mainStyles, column, row );
    // skip 'table:style-name' attribute for the default style
    if ( !currentCellStyle.isDefaultStyle() )
      xmlwriter.addAttribute( "table:style-name", mainStyles.styles()[currentCellStyle] );

    // group empty cells with the same style
    const QString comment = this->comment( column, row );
    if ( isEmpty() && comment.isEmpty() && !isPartOfMerged() && !doesMergeCells() )
    {
      bool refCellIsDefault = isDefault();
      int j = column + 1;
      Cell *nextCell = sheet()->getNextCellRight( column, row );
      while ( nextCell )
      {
        // if
        //   the next cell is not the adjacent one
        // or
        //   the next cell is not empty
        if ( nextCell->column() != j || !nextCell->isEmpty() )
        {
          if ( refCellIsDefault )
          {
            // if the origin cell was a default cell,
            // we count the default cells
            repeated = nextCell->column() - j + 1;
          }
          // otherwise we just stop here to process the adjacent
          // cell in the next iteration of the outer loop
          // (in Sheet::saveOasisCells)
          break;
        }

        KoGenStyle nextCellStyle; // the type is determined in saveOasisCellStyle
        nextCell->saveOasisCellStyle( nextCellStyle, mainStyles, nextCell->column(), nextCell->row() );

        if ( nextCell->isPartOfMerged() || nextCell->doesMergeCells() ||
             !nextCell->comment().isEmpty() ||
             !(nextCellStyle == currentCellStyle) )
        {
          break;
        }
        ++repeated;
        // get the next cell and set the index to the adjacent cell
        nextCell = sheet()->getNextCellRight( j++, row );
      }
      kDebug(36003) << "Cell::saveOasis: empty cell in column " << column << " "
                    << "repeated " << repeated << " time(s)" << endl;

      if ( repeated > 1 )
        xmlwriter.addAttribute( "table:number-columns-repeated", QString::number( repeated ) );
    }

    Validity validity = this->validity( column, row );
    if ( !validity.isEmpty() )
    {
        GenValidationStyle styleVal(&validity);
        xmlwriter.addAttribute( "table:validation-name", valStyle.lookup( styleVal ) );
    }
    if ( isFormula() )
    {
      //kDebug(36003) << "Formula found" << endl;
      QString formula = Oasis::encodeFormula( text(), locale() );
      xmlwriter.addAttribute( "table:formula", formula );
    }
    else if ( !link().isEmpty() )
    {
        //kDebug(36003)<<"Link found \n";
        xmlwriter.startElement( "text:p" );
        xmlwriter.startElement( "text:a" );
        //Reference cell is started by '#'
        if ( localReferenceAnchor( link() ) )
            xmlwriter.addAttribute( " xLinkDialog.href", ( '#'+link() ) );
        else
            xmlwriter.addAttribute( " xLinkDialog.href", link() );
        xmlwriter.addTextNode( text() );
        xmlwriter.endElement();
        xmlwriter.endElement();
    }

    if ( doesMergeCells() )
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
        xmlwriter.addTextNode( strOutText().toUtf8() );
        xmlwriter.endElement();
    }

    saveOasisAnnotation( xmlwriter, row, column );

    xmlwriter.endElement();
    return true;
}

void Cell::saveOasisValue (KoXmlWriter &xmlWriter)
{
  switch (value().format())
  {
    case Value::fmt_None: break;  //NOTHING HERE
    case Value::fmt_Boolean:
    {
      xmlWriter.addAttribute( "office:value-type", "boolean" );
      xmlWriter.addAttribute( "office:boolean-value", ( value().asBoolean() ?
          "true" : "false" ) );
      break;
    }
    case Value::fmt_Number:
    {
      xmlWriter.addAttribute( "office:value-type", "float" );
      if (value().isInteger())
        xmlWriter.addAttribute( "office:value", QString::number( value().asInteger() ) );
      else
        xmlWriter.addAttribute( "office:value", QString::number( value().asFloat(), 'g', DBL_DIG ) );
      break;
    }
    case Value::fmt_Percent:
    {
      xmlWriter.addAttribute( "office:value-type", "percentage" );
      xmlWriter.addAttribute( "office:value",
          QString::number( value().asFloat() ) );
      break;
    }
    case Value::fmt_Money:
    {
      xmlWriter.addAttribute( "office:value-type", "currency" );
      const Style style = this->style();
      if ( style.hasAttribute( Style::CurrencyFormat ) )
      {
        Style::Currency currency = style.currency();
        xmlWriter.addAttribute( "office:currency", Currency::getCurrencyCode(currency.type) );
      }
      xmlWriter.addAttribute( "office:value", QString::number( value().asFloat() ) );
      break;
    }
    case Value::fmt_DateTime: break;  //NOTHING HERE
    case Value::fmt_Date:
    {
      xmlWriter.addAttribute( "office:value-type", "date" );
      xmlWriter.addAttribute( "office:date-value",
          value().asDate( sheet()->doc() ).toString( Qt::ISODate ) );
      break;
    }
    case Value::fmt_Time:
    {
      xmlWriter.addAttribute( "office:value-type", "time" );
      xmlWriter.addAttribute( "office:time-value",
          value().asTime( sheet()->doc() ).toString( "PThhHmmMssS" ) );
      break;
    }
    case Value::fmt_String:
    {
      xmlWriter.addAttribute( "office:value-type", "string" );
      xmlWriter.addAttribute( "office:string-value", value().asString() );
      break;
    }
  };
}

void Cell::loadOasisConditional( const KoXmlElement* style )
{
    if ( style )//safe
    {
        // search for at least one condition
        KoXmlElement e;
        forEachElement( e, style->toElement() )
        {
            if ( e.localName() == "map" && e.namespaceURI() == KoXmlNS::style )
            {
                QSharedDataPointer<Conditions> conditions( new Conditions( d->sheet ) );
                conditions->loadOasisConditions( e );
                // conditions->checkMatches( this );
                setConditions( conditions );
                // break here
                // Conditions::loadOasisConditions finishes the iteration
                break;
            }
        }
    }
}

bool Cell::loadOasis( const KoXmlElement& element, KoOasisLoadingContext& oasisContext )
{
    kDebug(36003) << "*** Loading cell properties ***** at " << column() << ',' << row () << endl;

    if ( element.hasAttributeNS( KoXmlNS::table, "style-name" ) )
    {
        kDebug(36003)<<" table:style-name: "<<element.attributeNS( KoXmlNS::table, "style-name", QString::null )<<endl;
        oasisContext.fillStyleStack( element, KoXmlNS::table, "styleName", "table-cell" );

        QString str = element.attributeNS( KoXmlNS::table, "style-name", QString::null );
        const KoXmlElement* cellStyle = oasisContext.oasisStyles().findStyle( str, "table-cell" );

        if ( cellStyle )
            loadOasisConditional( cellStyle );
    }

    //Search and load each paragraph of text. Each paragraph is separated by a line break.
    loadOasisCellText( element );

    //
    // formula
    //
    bool isFormula = false;
    if ( element.hasAttributeNS( KoXmlNS::table, "formula" ) )
    {
        kDebug(36003)<<" formula :"<<element.attributeNS( KoXmlNS::table, "formula", QString::null )<<endl;
        isFormula = true;
        QString oasisFormula( element.attributeNS( KoXmlNS::table, "formula", QString::null ) );
        //necessary to remove it to load formula from oocalc2.0 (use namespace)
        if (oasisFormula.startsWith( "oooc:" ) )
            oasisFormula= oasisFormula.mid( 5 );
        else if (oasisFormula.startsWith( "kspr:" ) )
            oasisFormula= oasisFormula.mid( 5 );
        // TODO Stefan: merge this into Oasis::decodeFormula
        checkForNamedAreas( oasisFormula );
        oasisFormula = Oasis::decodeFormula( oasisFormula, locale() );
        setCellText( oasisFormula );
    }
    else if ( !d->strText.isEmpty() && d->strText.at(0) == '=' ) //prepend ' to the text to avoid = to be painted
        d->strText.prepend('\'');

    //
    // validation
    //
    if ( element.hasAttributeNS( KoXmlNS::table, "validation-name" ) )
    {
        kDebug(36003)<<" validation-name: "<<element.attributeNS( KoXmlNS::table, "validation-name", QString::null )<<endl;
        loadOasisValidation( element.attributeNS( KoXmlNS::table, "validation-name", QString::null ) );
    }

    //
    // value type
    //
    if( element.hasAttributeNS( KoXmlNS::office, "value-type" ) )
    {
        QString valuetype = element.attributeNS( KoXmlNS::office, "value-type", QString::null );
        kDebug(36003)<<"  value-type: " << valuetype << endl;
        if( valuetype == "boolean" )
        {
          QString val = element.attributeNS( KoXmlNS::office, "boolean-value", QString::null ).toLower();
            if( ( val == "true" ) || ( val == "false" ) )
            {
                bool value = val == "true";
                setCellValue( Value(value) );
            }
        }

        // integer and floating-point value
        else if( valuetype == "float" )
        {
            bool ok = false;
            double value = element.attributeNS( KoXmlNS::office, "value", QString::null ).toDouble( &ok );
            if( ok )
                setCellValue( Value(value) );

            if ( !isFormula && d->strText.isEmpty())
            {
                QString str = locale()->formatNumber( value, 15 );
                setCellText( str );
            }
        }

        // currency value
        else if( valuetype == "currency" )
        {
            bool ok = false;
            double value = element.attributeNS( KoXmlNS::office, "value", QString::null ).toDouble( &ok );
            if( ok )
            {
                setCellValue( Value(value), Money_format );

                if (element.hasAttributeNS( KoXmlNS::office, "currency" ) )
                {
                  Currency currency(element.attributeNS( KoXmlNS::office, "currency", QString::null ) );
                  Style style;
                  // FIXME: Use KSpread::Currency instead Style::Currency
                  Style::Currency sCurrency;
                  sCurrency.type = currency.getIndex();
                  sCurrency.symbol = currency.getDisplayCode();
                  style.setCurrency( sCurrency );
                  setStyle( style );
                }
            }
        }
        else if( valuetype == "percentage" )
        {
            bool ok = false;
            double percent = element.attributeNS( KoXmlNS::office, "value", QString::null ).toDouble( &ok );
            if( ok )
            {
                Value value;
                value.setValue(percent);
                value.setFormat (Value::fmt_Percent);
                setCellValue( Value(value) );

                if ( !isFormula && d->strText.isEmpty())
                {
                    QString str = locale()->formatNumber( percent, 15 );
                    setCellText( str );
                }
                Style style;
                style.setFormatType( Percentage_format );
                setStyle( style );
            }
        }
        else if ( valuetype == "date" )
        {
            QString value = element.attributeNS( KoXmlNS::office, "value", QString::null );
            if ( value.isEmpty() )
                value = element.attributeNS( KoXmlNS::office, "date-value", QString::null );
            kDebug(36003) << "Type: date, value: " << value << endl;

            // "1980-10-15"
            int year = 0, month = 0, day = 0;
            bool ok = false;

            int p1 = value.indexOf( '-' );
            if ( p1 > 0 )
                year  = value.left( p1 ).toInt( &ok );

            kDebug(36003) << "year: " << value.left( p1 ) << endl;

            int p2 = value.indexOf( '-', ++p1 );

            if ( ok )
                month = value.mid( p1, p2 - p1  ).toInt( &ok );

            kDebug(36003) << "month: " << value.mid( p1, p2 - p1 ) << endl;

            if ( ok )
                day = value.right( value.length() - p2 - 1 ).toInt( &ok );

            kDebug(36003) << "day: " << value.right( value.length() - p2 ) << endl;

            if ( ok )
            {
                setCellValue( Value( QDate( year, month, day ), sheet()->doc() ) );
                Style style;
                style.setFormatType(ShortDate_format);
                setStyle( style );
                kDebug(36003) << "Set QDate: " << year << " - " << month << " - " << day << endl;
            }

        }
        else if ( valuetype == "time" )
        {
            QString value = element.attributeNS( KoXmlNS::office, "value", QString::null );
            if ( value.isEmpty() )
                value = element.attributeNS( KoXmlNS::office, "time-value", QString::null );
            kDebug(36003) << "Type: time: " << value << endl;
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

                kDebug(36003) << "Num: " << num << endl;

                num = "";
                if ( !ok )
                    break;
            }
            kDebug(36003) << "Hours: " << hours << ", " << minutes << ", " << seconds << endl;

            if ( ok )
            {
                // Value kval( timeToNum( hours, minutes, seconds ) );
                // cell->setValue( kval );
                setCellValue( Value( QTime( hours % 24, minutes, seconds ), sheet()->doc() ) );
                Style style;
                style.setFormatType (Time_format);
                setStyle( style );
            }
        }
        else if( valuetype == "string" )
        {
            QString value = element.attributeNS( KoXmlNS::office, "value", QString::null );
            if ( value.isEmpty() && element.hasAttributeNS( KoXmlNS::office, "string-value" ))
            {
                //if there is not string-value entry don't overwrite value stored into <text:p>
                value = element.attributeNS( KoXmlNS::office, "string-value", QString::null );
                setCellValue( Value(value) );
            }
            Style style;
            style.setFormatType (Text_format);
            setStyle( style );
        }
        else
            kDebug(36003)<<" type of value found : "<<valuetype<<endl;
    }

    //
    // merged cells ?
    //
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
        mergeCells( d->column, d->row, colSpan - 1, rowSpan - 1 );

    //
    // cell comment/annotation
    //
    KoXmlElement annotationElement = KoDom::namedItemNS( element, KoXmlNS::office, "annotation" );
    if ( !annotationElement.isNull() )
    {
        QString comment;
        KoXmlNode node = annotationElement.firstChild();
        while( !node.isNull() )
        {
            KoXmlElement commentElement = node.toElement();
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

    KoXmlElement frame = KoDom::namedItemNS( element, KoXmlNS::draw, "frame" );
    if ( !frame.isNull() )
      loadOasisObjects( frame, oasisContext );

    if (isFormula)   // formulas must be recalculated
      sheet()->doc()->addDamage( new CellDamage( this, CellDamage::Formula |
                                                                 CellDamage::Value ) );

    return true;
}

void Cell::loadOasisCellText( const KoXmlElement& parent )
{
    //Search and load each paragraph of text. Each paragraph is separated by a line break
    KoXmlElement textParagraphElement;
    QString cellText;

    bool multipleTextParagraphsFound=false;

    forEachElement( textParagraphElement , parent )
    {
        if ( textParagraphElement.localName() == "p" &&
             textParagraphElement.namespaceURI()== KoXmlNS::text )
        {
            // our text, could contain formating for value or result of formul
            if (cellText.isEmpty())
                cellText = textParagraphElement.text();
            else
            {
                cellText += '\n' + textParagraphElement.text();
                multipleTextParagraphsFound=true;
            }

            KoXmlElement textA = KoDom::namedItemNS( textParagraphElement, KoXmlNS::text, "a" );
            if( !textA.isNull() )
            {
                if ( textA.hasAttributeNS( KoXmlNS::xlink, "href" ) )
                {
                    QString link = textA.attributeNS( KoXmlNS::xlink, "href", QString::null );
                    cellText = textA.text();
                    setCellText( cellText );
                    setValue( Value(cellText) );
                    if ( (!link.isEmpty()) && (link[0]=='#') )
                        link=link.remove( 0, 1 );
                    setLink( link );
                }
            }
        }
    }

    if (!cellText.isNull())
    {
        setCellText( cellText );
        setValue( Value(cellText) );
    }

    //Enable word wrapping if multiple lines of text have been found.
    if ( multipleTextParagraphsFound )
    {
        Style style;
        style.setWrapText( true );
        setStyle( style );
    }
}

void Cell::loadOasisObjects( const KoXmlElement &parent, KoOasisLoadingContext& oasisContext )
{
    for( KoXmlElement e = parent; !e.isNull(); e = e.nextSibling().toElement() )
    {
        if ( e.localName() == "frame" && e.namespaceURI() == KoXmlNS::draw )
        {
          EmbeddedObject *obj = 0;
          KoXmlNode object = KoDom::namedItemNS( e, KoXmlNS::draw, "object" );
          if ( !object.isNull() )
          {
            if ( !object.toElement().attributeNS( KoXmlNS::draw, "notify-on-update-of-ranges", QString::null).isNull() )
              obj = new EmbeddedChart( sheet()->doc(), sheet() );
            else
              obj = new EmbeddedKOfficeObject( sheet()->doc(), sheet() );
          }
          else
          {
            KoXmlNode image = KoDom::namedItemNS( e, KoXmlNS::draw, "image" );
            if ( !image.isNull() )
              obj = new EmbeddedPictureObject( sheet(), sheet()->doc()->pictureCollection() );
            else
              kDebug(36003) << "Object type wasn't loaded!" << endl;
          }

          if ( obj )
          {
            obj->loadOasis( e, oasisContext );
            sheet()->doc()->insertObject( obj );

            QString ref = e.attributeNS( KoXmlNS::table, "end-cell-address", QString::null );
            if ( ref.isNull() )
              continue;

            ref = Oasis::decodeFormula( ref );
            Point point( ref );
            if ( !point.isValid() )
              continue;

            KoRect geometry = obj->geometry();
            geometry.setLeft( geometry.left() + sheet()->columnPos( d->column ) );
            geometry.setTop( geometry.top() + sheet()->rowPos( d->row ) );

            QString str = e.attributeNS( KoXmlNS::table, "end-x", QString::null );
            if ( !str.isNull() )
            {
              uint end_x = (uint) KoUnit::parseValue( str );
              geometry.setRight( sheet()->columnPos( point.column() ) + end_x );
            }

            str = e.attributeNS( KoXmlNS::table, "end-y", QString::null );
            if ( !str.isNull() )
            {
              uint end_y = (uint) KoUnit::parseValue( str );
              geometry.setBottom( sheet()->rowPos( point.row() ) + end_y );
            }

            obj->setGeometry( geometry );
          }
        }
    }
}

void Cell::loadOasisValidation( const QString& validationName )
{
    KoXmlElement element = sheet()->doc()->loadingInfo()->validation( validationName);
    Validity validity;
    if ( element.hasAttributeNS( KoXmlNS::table, "condition" ) )
    {
        QString valExpression = element.attributeNS( KoXmlNS::table, "condition", QString::null );
        kDebug(36003)<<" element.attribute( table:condition ) "<<valExpression<<endl;
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
            kDebug(36003)<<" valExpression = :"<<valExpression<<endl;
            validity.setRestriction( Validity::TextLength );

            loadOasisValidationCondition( validity, valExpression );
        }
        else if ( valExpression.contains( "cell-content-is-text()" ) )
        {
            validity.setRestriction( Validity::Text );
        }
        //cell-content-text-length-is-between(Value, Value) | cell-content-text-length-is-not-between(Value, Value) | cell-content-is-in-list( StringList )
        else if ( valExpression.contains( "cell-content-text-length-is-between" ) )
        {
            validity.setRestriction( Validity::TextLength );
            validity.setCondition( Conditional::Between );
            valExpression = valExpression.remove( "oooc:cell-content-text-length-is-between(" );
            kDebug(36003)<<" valExpression :"<<valExpression<<endl;
            valExpression = valExpression.remove( ')' );
            QStringList listVal = valExpression.split( ',', QString::SkipEmptyParts );
            loadOasisValidationValue( validity, listVal );
        }
        else if ( valExpression.contains( "cell-content-text-length-is-not-between" ) )
        {
            validity.setRestriction( Validity::TextLength );
            validity.setCondition( Conditional::Different );
            valExpression = valExpression.remove( "oooc:cell-content-text-length-is-not-between(" );
            kDebug(36003)<<" valExpression :"<<valExpression<<endl;
            valExpression = valExpression.remove( ')' );
            kDebug(36003)<<" valExpression :"<<valExpression<<endl;
            QStringList listVal = valExpression.split( ',', QString::SkipEmptyParts );
            loadOasisValidationValue( validity, listVal );
        }
        else if ( valExpression.contains( "cell-content-is-in-list(" ) )
        {
            validity.setRestriction( Validity::List );
            valExpression = valExpression.remove( "oooc:cell-content-is-in-list(" );
            kDebug(36003)<<" valExpression :"<<valExpression<<endl;
            valExpression = valExpression.remove( ')' );
            validity.setValidityList( valExpression.split( ';',  QString::SkipEmptyParts ) );

        }
        //TrueFunction ::= cell-content-is-whole-number() | cell-content-is-decimal-number() | cell-content-is-date() | cell-content-is-time()
        else
        {
            if (valExpression.contains( "cell-content-is-whole-number()" ) )
            {
                validity.setRestriction(  Validity::Number );
                valExpression = valExpression.remove( "oooc:cell-content-is-whole-number() and " );
            }
            else if (valExpression.contains( "cell-content-is-decimal-number()" ) )
            {
                validity.setRestriction( Validity::Integer );
                valExpression = valExpression.remove( "oooc:cell-content-is-decimal-number() and " );
            }
            else if (valExpression.contains( "cell-content-is-date()" ) )
            {
                validity.setRestriction( Validity::Date );
                valExpression = valExpression.remove( "oooc:cell-content-is-date() and " );
            }
            else if (valExpression.contains( "cell-content-is-time()" ) )
            {
                validity.setRestriction( Validity::Time );
                valExpression = valExpression.remove( "oooc:cell-content-is-time() and " );
            }
            kDebug(36003)<<"valExpression :"<<valExpression<<endl;

            if ( valExpression.contains( "cell-content()" ) )
            {
                valExpression = valExpression.remove( "cell-content()" );
                loadOasisValidationCondition( validity, valExpression );
            }
            //GetFunction ::= cell-content-is-between(Value, Value) | cell-content-is-not-between(Value, Value)
            //for the moment we support just int/double value, not text/date/time :(
            if ( valExpression.contains( "cell-content-is-between(" ) )
            {
                valExpression = valExpression.remove( "cell-content-is-between(" );
                valExpression = valExpression.remove( ')' );
                QStringList listVal = valExpression.split( ',', QString::SkipEmptyParts );
                loadOasisValidationValue( validity, listVal );
                validity.setCondition( Conditional::Between );
            }
            if ( valExpression.contains( "cell-content-is-not-between(" ) )
            {
                valExpression = valExpression.remove( "cell-content-is-not-between(" );
                valExpression = valExpression.remove( ')' );
                QStringList listVal = valExpression.split( ',', QString::SkipEmptyParts );
                loadOasisValidationValue( validity, listVal );
                validity.setCondition( Conditional::Different );
            }
        }
    }
    if ( element.hasAttributeNS( KoXmlNS::table, "allow-empty-cell" ) )
    {
        kDebug(36003)<<" element.hasAttribute( table:allow-empty-cell ) :"<<element.hasAttributeNS( KoXmlNS::table, "allow-empty-cell" )<<endl;
        validity.setAllowEmptyCell( ( ( element.attributeNS( KoXmlNS::table, "allow-empty-cell", QString::null )=="true" ) ? true : false ) );
    }
    if ( element.hasAttributeNS( KoXmlNS::table, "base-cell-address" ) )
    {
        //todo what is it ?
    }

    KoXmlElement help = KoDom::namedItemNS( element, KoXmlNS::table, "help-message" );
    if ( !help.isNull() )
    {
        if ( help.hasAttributeNS( KoXmlNS::table, "title" ) )
        {
            kDebug(36003)<<"help.attribute( table:title ) :"<<help.attributeNS( KoXmlNS::table, "title", QString::null )<<endl;
            validity.setTitleInfo( help.attributeNS( KoXmlNS::table, "title", QString::null ) );
        }
        if ( help.hasAttributeNS( KoXmlNS::table, "display" ) )
        {
            kDebug(36003)<<"help.attribute( table:display ) :"<<help.attributeNS( KoXmlNS::table, "display", QString::null )<<endl;
            validity.setDisplayValidationInformation( ( ( help.attributeNS( KoXmlNS::table, "display", QString::null )=="true" ) ? true : false ) );
        }
        KoXmlElement attrText = KoDom::namedItemNS( help, KoXmlNS::text, "p" );
        if ( !attrText.isNull() )
        {
            kDebug(36003)<<"help text :"<<attrText.text()<<endl;
            validity.setMessageInfo( attrText.text() );
        }
    }

    KoXmlElement error = KoDom::namedItemNS( element, KoXmlNS::table, "error-message" );
    if ( !error.isNull() )
    {
        if ( error.hasAttributeNS( KoXmlNS::table, "title" ) )
            validity.setTitle( error.attributeNS( KoXmlNS::table, "title", QString::null ) );
        if ( error.hasAttributeNS( KoXmlNS::table, "message-type" ) )
        {
            QString str = error.attributeNS( KoXmlNS::table, "message-type", QString::null );
            if ( str == "warning" )
              validity.setAction( Validity::Warning );
            else if ( str == "information" )
              validity.setAction( Validity::Information );
            else if ( str == "stop" )
              validity.setAction( Validity::Stop );
            else
                kDebug(36003)<<"validation : message type unknown  :"<<str<<endl;
        }

        if ( error.hasAttributeNS( KoXmlNS::table, "display" ) )
        {
            kDebug(36003)<<" display message :"<<error.attributeNS( KoXmlNS::table, "display", QString::null )<<endl;
            validity.setDisplayMessage( (error.attributeNS( KoXmlNS::table, "display", QString::null )=="true") );
        }
        KoXmlElement attrText = KoDom::namedItemNS( error, KoXmlNS::text, "p" );
        if ( !attrText.isNull() )
            validity.setMessage( attrText.text() );
    }
    setValidity( validity );
}


void Cell::loadOasisValidationValue( Validity validity, const QStringList &listVal )
{
    bool ok = false;
    kDebug(36003)<<" listVal[0] :"<<listVal[0]<<" listVal[1] :"<<listVal[1]<<endl;

    if ( validity.restriction() == Validity::Date )
    {
        validity.setMinimumDate( QDate::fromString( listVal[0] ) );
        validity.setMaximumDate( QDate::fromString( listVal[1] ) );
    }
    else if ( validity.restriction() == Validity::Time )
    {
        validity.setMinimumTime( QTime::fromString( listVal[0] ) );
        validity.setMaximumTime( QTime::fromString( listVal[1] ) );
    }
    else
    {
        validity.setMinimumValue( listVal[0].toDouble(&ok) );
        if ( !ok )
        {
            validity.setMinimumValue( listVal[0].toInt(&ok) );
            if ( !ok )
                kDebug(36003)<<" Try to parse this value :"<<listVal[0]<<endl;

#if 0
            if ( !ok )
                validity.setMinimumValue( listVal[0] );
#endif
        }
        ok=false;
        validity.setMaximumValue( listVal[1].toDouble(&ok) );
        if ( !ok )
        {
            validity.setMaximumValue( listVal[1].toInt(&ok) );
            if ( !ok )
                kDebug(36003)<<" Try to parse this value :"<<listVal[1]<<endl;

#if 0
            if ( !ok )
                validity.setMaximumValue( listVal[1] );
#endif
        }
    }
    setValidity( validity );
}

void Cell::loadOasisValidationCondition( Validity validity, QString &valExpression )
{
    if (validity.isEmpty()) return;
    QString value;
    if (valExpression.indexOf( "<=" )==0 )
    {
        value = valExpression.remove( 0,2 );
        validity.setCondition( Conditional::InferiorEqual );
    }
    else if (valExpression.indexOf( ">=" )==0 )
    {
        value = valExpression.remove( 0,2 );
        validity.setCondition( Conditional::SuperiorEqual );
    }
    else if (valExpression.indexOf( "!=" )==0 )
    {
        //add Differentto attribute
        value = valExpression.remove( 0,2 );
        validity.setCondition( Conditional::DifferentTo );
    }
    else if ( valExpression.indexOf( '<' )==0 )
    {
        value = valExpression.remove( 0,1 );
        validity.setCondition( Conditional::Inferior );
    }
    else if(valExpression.indexOf( '>' )==0 )
    {
        value = valExpression.remove( 0,1 );
        validity.setCondition( Conditional::Superior );
    }
    else if (valExpression.indexOf( '=' )==0 )
    {
        value = valExpression.remove( 0,1 );
        validity.setCondition( Conditional::Equal );
    }
    else
        kDebug(36003)<<" I don't know how to parse it :"<<valExpression<<endl;
    if ( validity.restriction() == Validity::Date )
    {
        validity.setMinimumDate( QDate::fromString( value ) );
    }
    else if (validity.restriction() == Validity::Date )
    {
        validity.setMinimumTime( QTime::fromString( value ) );
    }
    else
    {
        bool ok = false;
        validity.setMinimumValue( value.toDouble(&ok) );
        if ( !ok )
        {
            validity.setMinimumValue( value.toInt(&ok) );
            if ( !ok )
                kDebug(36003)<<" Try to parse this value :"<<value<<endl;

#if 0
            if ( !ok )
                validity.setMinimumValue( value );
#endif
        }
    }
}


bool Cell::load( const KoXmlElement & cell, int _xshift, int _yshift,
                 Paste::Mode pm, Paste::Operation op, bool paste )
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
    if ( row() < 1 || row() > KS_rowMax )
    {
        kDebug(36001) << "Cell::load: Value out of Range Cell:row=" << d->row << endl;
        return false;
    }
    if ( column() < 1 || column() > KS_colMax )
    {
        kDebug(36001) << "Cell::load: Value out of Range Cell:column=" << d->column << endl;
        return false;
    }

    //
    // Load formatting information.
    //
    KoXmlElement formatElement = cell.namedItem( "format" ).toElement();
    if ( !formatElement.isNull()
          && ( (pm == Paste::Normal) || (pm == Paste::Format) || (pm == Paste::NoBorder) ) )
    {
        // send pm parameter. Didn't load Borders if pm==NoBorder

        Style style;
        if ( !style.loadXML( formatElement, pm, paste ) )
            return false;
        setStyle( style );

        if ( formatElement.hasAttribute( "colspan" ) )
        {
            int i = formatElement.attribute("colspan").toInt( &ok );
            if ( !ok ) return false;
            // Validation
            if ( i < 0 || i > KS_spanMax )
            {
                kDebug(36001) << "Value out of range Cell::colspan=" << i << endl;
                return false;
            }
            if (i || d->hasExtra())
              d->extra()->extraXCells = i;
            if ( i > 0 )
            {
              setFlag(Flag_Merged);
            }
        }

        if ( formatElement.hasAttribute( "rowspan" ) )
        {
            int i = formatElement.attribute("rowspan").toInt( &ok );
            if ( !ok ) return false;
            // Validation
            if ( i < 0 || i > KS_spanMax )
            {
                kDebug(36001) << "Value out of range Cell::rowspan=" << i << endl;
                return false;
            }
            if (i || d->hasExtra())
              d->extra()->extraYCells = i;
            if ( i > 0 )
            {
              setFlag(Flag_Merged);
            }
        }

        if ( testFlag( Flag_Merged ) )
        {
            if (d->hasExtra())
              mergeCells( d->column, d->row, d->extra()->extraXCells, d->extra()->extraYCells );
        }

    }

    //
    // Load the condition section of a cell.
    //
    KoXmlElement conditionsElement = cell.namedItem( "condition" ).toElement();
    if ( !conditionsElement.isNull())
    {
        QSharedDataPointer<Conditions> conditions( new Conditions( d->sheet ) );
        conditions->loadConditions( conditionsElement );
        // conditions->checkMatches( this );
        setConditions( conditions );
    }
    else if ((pm == Paste::Normal) || (pm == Paste::NoBorder))
    {
      //clear the conditional formatting
      setConditions( QSharedDataPointer<Conditions>() );
    }

    KoXmlElement validityElement = cell.namedItem( "validity" ).toElement();
    if ( !validityElement.isNull())
    {
        Validity validity;
        if ( validity.loadXML( this, validityElement ) )
            setValidity( validity );
    }
    else if ((pm == Paste::Normal) || (pm == Paste::NoBorder))
    {
      // clear the validity
      setValidity( Validity() );
    }

    //
    // Load the comment
    //
    KoXmlElement comment = cell.namedItem( "comment" ).toElement();
    if ( !comment.isNull() && ( pm == Paste::Normal || pm == Paste::Comment || pm == Paste::NoBorder ))
    {
        QString t = comment.text();
        //t = t.trimmed();
        setComment( t );
    }

    //
    // The real content of the cell is loaded here. It is stored in
    // the "text" tag, which contains either a text or a CDATA section.
    //
    // TODO: make this suck less. We set data twice, in loadCellData, and
    // also here. Not good.
    KoXmlElement text = cell.namedItem( "text" ).toElement();

    if ( !text.isNull() &&
          ( pm == Paste::Normal || pm == Paste::Text || pm == Paste::NoBorder || pm == Paste::Result ) )
    {
      /* older versions mistakenly put the datatype attribute on the cell
         instead of the text.  Just move it over in case we're parsing
         an old document */
#ifdef KOXML_USE_QDOM
      if ( cell.hasAttribute( "dataType" ) ) // new docs
        text.setAttribute( "dataType", cell.attribute( "dataType" ) );
#else
#warning Problem with KoXmlReader conversion!
      kWarning() << "Problem with KoXmlReader conversion!" << endl;
#endif

      KoXmlElement result = cell.namedItem( "result" ).toElement();
      QString txt = text.text();
      if ((pm == Paste::Result) && (txt[0] == '='))
        // paste text of the element, if we want to paste result
        // and the source cell contains a formula
        // note that we mustn't use setCellValue after this, or else we lose
        // all the formulas ...
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
            setValue( Value(false) );
          else if ( t == "true" )
            setValue( Value(true) );
          else
            clear = false;
        }
        else if( dataType == "Num" )
        {
          bool ok = false;
          double dd = t.toDouble( &ok );
          if ( ok )
            setValue ( Value(dd) );
          else
            clear = false;
        }
        else if( dataType == "Date" )
        {
          bool ok = false;
          double dd = t.toDouble( &ok );
          if ( ok )
            setValue ( Value(dd) );
          else
          {
            int pos   = t.indexOf( '/' );
            int year  = t.mid( 0, pos ).toInt();
            int pos1  = t.indexOf( '/', pos + 1 );
            int month = t.mid( pos + 1, ( ( pos1 - 1 ) - pos ) ).toInt();
            int day   = t.right( t.length() - pos1 - 1 ).toInt();
            QDate date( year, month, day );
            if ( date.isValid() )
              setValue( Value( date, sheet()->doc() ) );
            else
              clear = false;
          }
        }
        else if( dataType == "Time" )
        {
          bool ok = false;
          double dd = t.toDouble( &ok );
          if ( ok )
            setCellValue( Value(dd) );
          else
          {
            int hours   = -1;
            int minutes = -1;
            int second  = -1;
            int pos, pos1;
            pos   = t.indexOf( ':' );
            hours = t.mid( 0, pos ).toInt();
            pos1  = t.indexOf( ':', pos + 1 );
            minutes = t.mid( pos + 1, ( ( pos1 - 1 ) - pos ) ).toInt();
            second  = t.right( t.length() - pos1 - 1 ).toInt();
            QTime time( hours, minutes, second );
            if ( time.isValid() )
              setValue( Value( time, sheet()->doc() ) );
            else
              clear = false;
          }
        }
        else
        {
          setValue( Value(t) );
        }

        // if ( clear )
        //   clearFlag( Flag_CalcDirty );
      }
    }

    return true;
}

bool Cell::loadCellData(const KoXmlElement & text, Paste::Operation op )
{
  //TODO: use converter()->asString() to generate strText

  QString t = text.text();
  t = t.trimmed();

  setFlag(Flag_LayoutDirty);
  setFlag(Flag_TextFormatDirty);

  // A formula like =A1+A2 ?
  if( (!t.isEmpty()) && (t[0] == '=') )
  {
    t = decodeFormula( t, d->column, d->row );
    setCellText (pasteOperation( t, d->strText, op ));

    clearAllErrors();

    makeFormula();
  }
  // rich text ?
  else if ((!t.isEmpty()) && (t[0] == '!') )
  {
      // KSpread pre 1.4 stores hyperlink as rich text (first char is '!')
      // extract the link and the correspoding text
      // This is a rather dirty hack, but enough for KSpread generated XML
      bool inside_tag = false;
      QString qml_text;
      QString tag;
      QString qml_link;

      for( int i = 1; i < t.length(); i++ )
      {
        QChar ch = t[i];
        if( ch == '<' )
        {
          if( !inside_tag )
          {
            inside_tag = true;
            tag.clear();
          }
        }
        else if( ch == '>' )
        {
          if( inside_tag )
          {
            inside_tag = false;
            if( tag.startsWith( "a href=\"", Qt::CaseSensitive ) )
            if( tag.endsWith( '"' ) )
              qml_link = tag.mid( 8, tag.length()-9 );
            tag.clear();
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
      setValue( Value(d->strText) );
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
        //kDebug(36001) << "Cell::load called checkTextInput, got dataType=" << dataType << "  t=" << t << endl;
        newStyleLoading = false;
      }
    }

    if ( newStyleLoading )
    {
      d->value = Value::empty();
      clearAllErrors();

      // boolean ?
      if( dataType == "Bool" )
      {
        bool val = (t.toLower() == "true");
        setCellValue (Value(val));
      }

      // number ?
      else if( dataType == "Num" )
      {
        bool ok = false;
        if (t.contains('.'))
          setValue ( Value( t.toDouble(&ok) ) ); // We save in non-localized format
        else
          setValue ( Value( t.toLong(&ok) ) );
        if ( !ok )
        {
          kWarning(36001) << "Couldn't parse '" << t << "' as number." << endl;
        }
        /* We will need to localize the text version of the number */
        KLocale* locale = sheet()->doc()->locale();

        /* KLocale::formatNumber requires the precision we want to return.
        */
        int precision = t.length() - t.indexOf('.') - 1;

        if ( formatType() == Percentage_format )
        {
          if (value().isInteger())
            t = locale->formatNumber( value().asInteger() * 100 );
          else
            t = locale->formatNumber( value().asFloat() * 100.0, precision );
          d->strText = pasteOperation( t, d->strText, op );
          d->strText += '%';
        }
        else
        {
          if (value().isInteger())
            t = locale->formatLong(value().asInteger());
          else
            t = locale->formatNumber(value().asFloat(), precision);
          d->strText = pasteOperation( t, d->strText, op );
        }
      }

      // date ?
      else if( dataType == "Date" )
      {
        int pos = t.indexOf('/');
        int year = t.mid(0,pos).toInt();
        int pos1 = t.indexOf('/',pos+1);
        int month = t.mid(pos+1,((pos1-1)-pos)).toInt();
        int day = t.right(t.length()-pos1-1).toInt();
        setValue( Value( QDate(year,month,day), sheet()->doc() ) );
        if ( value().asDate( sheet()->doc() ).isValid() ) // Should always be the case for new docs
          d->strText = locale()->formatDate( value().asDate( sheet()->doc() ), true );
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
        pos = t.indexOf(':');
        hours = t.mid(0,pos).toInt();
        pos1 = t.indexOf(':',pos+1);
        minutes = t.mid(pos+1,((pos1-1)-pos)).toInt();
        second = t.right(t.length()-pos1-1).toInt();
        setValue( Value( QTime(hours,minutes,second), sheet()->doc() ) );
        if ( value().asTime( sheet()->doc() ).isValid() ) // Should always be the case for new docs
          d->strText = locale()->formatTime( value().asTime( sheet()->doc() ), true );
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
        setValue( Value(d->strText) );
      }
    }
  }

  if ( text.hasAttribute( "outStr" ) ) // very new docs
  {
    d->strOutText = text.attribute( "outStr" );
    if ( !d->strOutText.isEmpty() )
      clearFlag( Flag_TextFormatDirty );
  }

  if ( !sheet()->isLoading() )
    setCellText( d->strText );

#if 0 // KSPREAD_NEW_STYLE_STORAGE // conditions
  if ( d->hasExtra() && d->extra()->conditions )
    d->extra()->conditions->checkMatches( this );
#endif
  return true;
}

QTime Cell::toTime(const KoXmlElement &element)
{
    //TODO: can't we use tryParseTime (after modification) instead?
    QString t = element.text();
    t = t.trimmed();
    int hours = -1;
    int minutes = -1;
    int second = -1;
    int pos, pos1;
    pos = t.indexOf(':');
    hours = t.mid(0,pos).toInt();
    pos1 = t.indexOf(':',pos+1);
    minutes = t.mid(pos+1,((pos1-1)-pos)).toInt();
    second = t.right(t.length()-pos1-1).toInt();
    setValue( Value( QTime(hours,minutes,second), sheet()->doc() ) );
    return value().asTime( sheet()->doc() );
}

QDate Cell::toDate(const KoXmlElement &element)
{
    QString t = element.text();
    int pos;
    int pos1;
    int year = -1;
    int month = -1;
    int day = -1;
    pos = t.indexOf('/');
    year = t.mid(0,pos).toInt();
    pos1 = t.indexOf('/',pos+1);
    month = t.mid(pos+1,((pos1-1)-pos)).toInt();
    day = t.right(t.length()-pos1-1).toInt();
    setValue( Value( QDate(year,month,day), sheet()->doc() ) );
    return value().asDate( sheet()->doc() );
}

QString Cell::pasteOperation( const QString &new_text, const QString &old_text, Paste::Operation op )
{
  if ( op == Paste::OverWrite )
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

    if ( old_text.isEmpty() &&
         ( op == Paste::Add || op == Paste::Mul || op == Paste::Sub || op == Paste::Div ) )
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
      old = '0';
      b2 = true;
    }

    if( b1 && b2 )
    {
        switch( op )
        {
          case  Paste::Add:
            tmp_op = QString::number(old.toDouble()+tmp.toDouble());
            break;
          case Paste::Mul :
            tmp_op = QString::number(old.toDouble()*tmp.toDouble());
            break;
          case Paste::Sub:
            tmp_op = QString::number(old.toDouble()-tmp.toDouble());
            break;
          case Paste::Div:
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
          case Paste::Add :
            tmp_op="=("+old+")+"+'('+tmp+')';
            break;
          case Paste::Mul :
            tmp_op="=("+old+")*"+'('+tmp+')';
            break;
          case Paste::Sub:
            tmp_op="=("+old+")-"+'('+tmp+')';
            break;
          case Paste::Div:
            tmp_op="=("+old+")/"+'('+tmp+')';
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

void Cell::sheetDies()
{
    // Avoid unobscuring the cells in the destructor.
    if (d->hasExtra())
    {
      d->extra()->extraXCells = 0;
      d->extra()->extraYCells = 0;
      d->extra()->mergedXCells = 0;
      d->extra()->mergedYCells = 0;
    }

    //d->nextCell = 0;
    //d->previousCell = 0;
}

Cell::~Cell()
{
    if ( d->nextCell )
        d->nextCell->setPreviousCell( d->previousCell );
    if ( d->previousCell )
        d->previousCell->setNextCell( d->nextCell );

    if ( !isDefault() )
        setValidity( Validity() );
    // FIXME Stefan: Clear conditions?

    // Unobscure cells.
    int extraXCells = d->hasExtra() ? d->extra()->extraXCells : 0;
    int extraYCells = d->hasExtra() ? d->extra()->extraYCells : 0;
    for( int x = 0; x <= extraXCells; ++x )
        for( int y = (x == 0) ? 1 : 0; // avoid looking at (+0,+0)
             y <= extraYCells; ++y )
    {
        Cell* cell = sheet()->cellAt( d->column + x, d->row + y );
        if ( cell )
            cell->unobscure(this);
    }

    d->value = Value::empty();

    if (!isDefault())
      valueChanged ();  //our value has been changed (is now null), but only if we aren't default

    delete d;
}

bool Cell::operator > ( const Cell & cell ) const
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
        return value().asDate( sheet()->doc() ) > cell.value().asDate( sheet()->doc() );
     else if (cell.value().isNumber())
        return true;
     else
        return false; //date are always < than texts and time
  }
  else if(isTime())
  {
     if( cell.isTime() )
        return value().asTime( sheet()->doc() ) > cell.value().asTime( sheet()->doc() );
     else if( cell.isDate())
        return true; //time are always > than date
     else if( cell.value().isNumber())
        return true;
     else
        return false; //time are always < than texts
  }
  else
  {
      if ( Map::respectCase )
          return value().asString().compare(cell.value().asString()) > 0;
      else
          return ( value().asString() ).toLower().compare(cell.value().asString().toLower()) > 0;
  }
}

bool Cell::operator < ( const Cell & cell ) const
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
        return value().asDateTime( sheet()->doc() ).date() < cell.value().asDateTime( sheet()->doc() ).date();
     else if( cell.value().isNumber())
        return false;
     else
        return true; //date are always < than texts and time
  }
  else if(isTime())
  {
     if( cell.isTime() )
        return value().asDateTime( sheet()->doc() ).time() < cell.value().asDateTime( sheet()->doc() ).time();
     else if(cell.isDate())
        return false; //time are always > than date
     else if( cell.value().isNumber())
        return false;
     else
        return true; //time are always < than texts
  }
  else
  {
      if ( Map::respectCase )
          return value().asString().compare(cell.value().asString()) < 0;
      else
          return ( value().asString() ).toLower().compare(cell.value().asString().toLower()) < 0;
  }
}

bool Cell::operator==( const Cell& other ) const
{
  if ( d->strText != other.d->strText )
    return false;
  if ( d->value != other.d->value )
    return false;
  // FIXME KSPREAD_NEW_STYLE_STORAGE // comparison
  if ( !isDefault() && !other.isDefault() && style() != other.style() )
    return false;
  if ( !isDefault() && !other.isDefault() && conditions() != other.conditions() )
    return false;
  if ( !isDefault() && !other.isDefault() && validity() != other.validity() )
    return false;
  if ( d->hasExtra() )
  {
    if ( !other.d->hasExtra() )
      return false;
    if ( d->extra()->link != other.d->extra()->link )
      return false;
    if ( d->extra()->mergedXCells != other.d->extra()->mergedXCells )
      return false;
    if ( d->extra()->mergedYCells != other.d->extra()->mergedYCells )
      return false;
  }
  return true;
}

QRect Cell::cellRect()
{
  // this asserts if the invoicetemplate.ods file in http://kross.dipe.org/KSpreadInvoiceExample.tar.gz is opened.
  //Q_ASSERT(!isDefault());
  return QRect(QPoint(d->column, d->row), QPoint(d->column, d->row));
}

QLinkedList<Conditional> Cell::conditionList( int col, int row ) const
{
    QSharedDataPointer<Conditions> conditions = this->conditions( col, row );
    return conditions ? conditions->conditionList() : QLinkedList<Conditional>();
}

void Cell::setConditionList( const QLinkedList<Conditional> & newList )
{
    QSharedDataPointer<Conditions> conditions( new Conditions( d->sheet ) );
    conditions->setConditionList( newList );
    // conditions->checkMatches( this );
    setConditions( conditions );
}

bool Cell::hasError() const
{
  return ( testFlag(Flag_ParseError) ||
           testFlag(Flag_CircularCalculation) ||
           testFlag(Flag_DependencyError));
}

void Cell::clearAllErrors()
{
  clearFlag( Flag_ParseError );
  clearFlag( Flag_CircularCalculation );
  clearFlag( Flag_DependencyError );
}

bool Cell::doesMergeCells() const
{
  return testFlag( Flag_Merged );
}

void Cell::clearFlag( StatusFlag flag )
{
  d->flags &= ~flag;
}

void Cell::setFlag( StatusFlag flag )
{
  d->flags |= flag;
}

bool Cell::testFlag( StatusFlag flag ) const
{
  return ( d->flags & flag );
}


void Cell::checkForNamedAreas( QString & formula ) const
{

  LoadingInfo* loadinginfo = sheet()->doc()->loadingInfo();
  if(! loadinginfo) {
    kDebug(36003) << "Cell::checkForNamedAreas loadinginfo is NULL" << endl;
    return;
  }

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
      if ( loadinginfo->findWordInAreaList(word) )
      {
        formula = formula.replace( start, word.length(), '\'' + word + '\'' );
        l = formula.length();
        ++i;
        kDebug(36003) << "Formula: " << formula << ", L: " << l << ", i: " << i + 1 <<endl;
      }
    }

    ++i;
    word = "";
    start = i;
  }
  if ( !word.isEmpty() )
  {
    if ( loadinginfo->findWordInAreaList(word) )
    {
      formula = formula.replace( start, word.length(), '\'' + word + '\'' );
      l = formula.length();
      ++i;
      kDebug(36003) << "Formula: " << formula << ", L: " << l << ", i: " << i + 1 <<endl;
    }
  }
}
