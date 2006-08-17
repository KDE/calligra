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
#include "Locale.h"
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
  d->cellView = new CellView( this );
  d->format = new Format(_sheet, _sheet->doc()->styleManager()->defaultStyle());
  d->format->setCell(this);
  clearAllErrors();
}


Cell::Cell( Sheet * _sheet, Style * _style,  int _column, int _row )
  : d(new Private)
{
  d->row = _row;
  d->column = _column;
  d->cellView = new CellView( this );
  d->format = new Format( _sheet, _style );
  d->format->setCell(this);

  clearAllErrors();
}

CellView* Cell::cellView() const
{
  return d->cellView;
}

Format* Cell::format() const
{
  return d->format;
}

// Return the sheet that this cell belongs to.
Sheet * Cell::sheet() const
{
  return d->format->sheet();
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
  return d->format->sheet()->doc()->locale();
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
  //(ie. no need to relayout, update dependant cells etc.),
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

  format()->sheet()->setRegionPaintDirty(cellRect());
}

void Cell::setCellValue (const Value &value, FormatType fmtType, const QString &txt)
{
  if (!txt.isEmpty())
    d->strText = txt;
  else
    d->strText = sheet()->doc()->converter()->asString (value).asString();
  if (fmtType != No_format)
    format()->setFormatType (fmtType);
  setValue (value);
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

Validity* Cell::validity( bool create )
{
    if ( (!create) && (!d->hasExtra()))
      //we don't have validity struct and we don't want one
      return 0;

    if( ( d->extra()->validity == 0 ) && create )
        d->extra()->validity = new Validity;
    return  d->extra()->validity;
}

void Cell::removeValidity()
{
    if (!d->hasExtra())
      return;

    delete d->extra()->validity;
    d->extra()->validity = 0;
}


void Cell::copyFormat( const Cell* cell )
{

    Q_ASSERT(cell);

    d->value.setFormat(cell->d->value.format());
    format()->copy(*(cell->format()));

    /*format()->setAlign( cell->format()->align( _column, _row ) );
    format()->setAlignY( cell->format()->alignY( _column, _row ) );
    format()->setTextFont( cell->format()->textFont( _column, _row ) );
    format()->setTextColor( cell->format()->textColor( _column, _row ) );
    format()->setBgColor( cell->bgColor( _column, _row) );
    setLeftBorderPen( cell->leftBorderPen( _column, _row ) );
    setTopBorderPen( cell->topBorderPen( _column, _row ) );
    setBottomBorderPen( cell->bottomBorderPen( _column, _row ) );
    setRightBorderPen( cell->rightBorderPen( _column, _row ) );
    format()->setFallDiagonalPen( cell->format()->fallDiagonalPen( _column, _row ) );
    format()->setGoUpDiagonalPen( cell->format()->goUpDiagonalPen( _column, _row ) );
    format()->setBackGroundBrush( cell->backGroundBrush( _column, _row) );
    format()->setPrecision( cell->format()->precision( _column, _row ) );
    format()->setPrefix( cell->format()->prefix( _column, _row ) );
    format()->setPostfix( cell->format()->postfix( _column, _row ) );
    format()->setFloatFormat( cell->format()->floatFormat( _column, _row ) );
    format()->setFloatColor( cell->format()->floatColor( _column, _row ) );
    format()->setMultiRow( cell->format()->multiRow( _column, _row ) );
    format()->setVerticalText( cell->format()->verticalText( _column, _row ) );
    format()->setDontPrintText( cell->format()->getDontprintText(_column, _row ) );
    format()->setNotProtected( cell->format()->notProtected(_column, _row ) );
    format()->setHideAll(cell->format()->isHideAll(_column, _row ) );
    format()->setHideFormula(cell->format()->isHideFormula(_column, _row ) );
    format()->setIndent( cell->format()->getIndent(_column, _row ) );
    format()->setAngle( cell->format()->getAngle(_column, _row) );
    format()->setFormatType( cell->format()->getFormatType(_column, _row) );
    Style::Currency currency;
    if ( cell->format()->currencyInfo( currency ) )
      format()->setCurrency( currency );*/

    QLinkedList<Conditional> conditionList = cell->conditionList();
    if (d->hasExtra())
      delete d->extra()->conditions;
    if ( cell->d->hasExtra() && cell->d->extra()->conditions )
      setConditionList( conditionList );
    else
      if (d->hasExtra())
        d->extra()->conditions = 0;

    /*format()->setComment( cell->format()->comment( _column, _row ) );*/
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
  format()->defaultStyleFormat();

  if (!d->hasExtra())
    return;

  if ( d->extra()->conditions )
  {
    delete d->extra()->conditions;
    d->extra()->conditions = 0;
  }

  delete d->extra()->validity;
  d->extra()->validity = 0;
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
        format()->sheet()->cellAt( x, y )->unobscure(this);
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
  format()->sheet()->nonDefaultCell( x, y )->obscure( this, true );
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
    format()->sheet()->doc()->addDamage( new CellDamage( this, CellDamage::Value ) );

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

  if ( !d->strText.trimmed().isEmpty() ) {
    return true;
  }

  // Cell borders?
  if ( format()->hasProperty( Style::STopBorder )
       || format()->hasProperty( Style::SLeftBorder )
       || format()->hasProperty( Style::SRightBorder )
       || format()->hasProperty( Style::SBottomBorder )
       || format()->hasProperty( Style::SFallDiagonal )
       || format()->hasProperty( Style::SGoUpDiagonal ) ) {
    return true;
  }

  // Background color or brush?
  if ( format()->hasProperty( Style::SBackgroundBrush ) )
  {
    const QBrush& brush=backGroundBrush(column(),row());

    // Only brushes that are visible (ie. they have a brush style
    // and are not white) need to be drawn
    if ( (brush.style() != Qt::NoBrush) &&
         (brush.color() != Qt::white || !brush.texture().isNull()) )
      return true;
  }

  if ( format()->hasProperty( Style::SBackgroundColor ) ) {
    kDebug() << "needsPrinting: Has background colour" << endl;
    QColor backgroundColor=bgColor(column(),row());

    //We don't need to print anything if the background is white
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
  format()->sheet()->setRegionPaintDirty( cellRect() );
}

void Cell::unobscure( Cell * cell )
{
  if (d->hasExtra())
    d->extra()->obscuringCells.removeAll( cell );
  setFlag( Flag_LayoutDirty );
  format()->sheet()->setRegionPaintDirty( cellRect() );
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
                kDebug(36001) << "Cell::decodeFormula: row or column out of range (col: " << col << " | row: " << row << ')' << endl;
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
        Cell *cell = format()->sheet()->cellAt( x, y );
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
      Cell* cell = format()->sheet()->cellAt(x,y);
      cell->setLayoutDirtyFlag();
    }
  }

  /* TODO - is this a good place for this? */
  updateChart( true );

  format()->sheet()->doc()->addDamage( new CellDamage( this, CellDamage::Value ) );
}


// Recalculate d->strOutText.
//

void Cell::setOutputText()
{
  if ( isDefault() ) {
    d->strOutText.clear();

    if ( d->hasExtra() && d->extra()->conditions )
      d->extra()->conditions->checkMatches();

    return;
  }

  // If nothing has changed, we don't need to remake the text layout.
  if ( !testFlag(Flag_TextFormatDirty) )
    return;

  // We don't want to remake the layout unnecessarily.
  clearFlag( Flag_TextFormatDirty );

  // Display a formula if warranted.  If not, display the value instead;
  // this is the most common case.
  if ( (!hasError()) && isFormula() && format()->sheet()->getShowFormula()
       && !( format()->sheet()->isProtected() && format()->isHideFormula( d->column, d->row ) )
       || isEmpty() )
    d->strOutText = d->strText;
  else {
    d->strOutText = sheet()->doc()->formatter()->formatText (this,
                   formatType());
  }

  // Check conditions if needed.
  if ( d->hasExtra() && d->extra()->conditions )
    d->extra()->conditions->checkMatches();
}




// ----------------------------------------------------------------
//                          Formula handling


bool Cell::makeFormula()
{
//   kDebug() << k_funcinfo << endl;

  d->formula = new KSpread::Formula (sheet(), this);
  d->formula->setExpression (d->strText);

  if (!d->formula->isValid ()) {
  // Did a syntax error occur ?
    clearFormula();

    if (format()->sheet()->doc()->showMessageError())
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
  format()->sheet()->doc()->addDamage( new CellDamage( this, CellDamage::Formula |
                                                             CellDamage::Value ) );

  return true;
}

void Cell::clearFormula()
{
  // Update the dependencies, if this was a formula.
  if (d->formula)
  {
    kDebug() << "This was a formula. Dependency update triggered." << endl;
    format()->sheet()->doc()->addDamage( new CellDamage( this, CellDamage::Formula ) );
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
    if ( format()->sheet()->doc()->delayCalculation() )
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
  int a = format()->align( column(), row() );
  if ( a == Style::HAlignUndefined )
  {
    //numbers should be right-aligned by default, as well as BiDi text
    if ((formatType() == Text_format) || value().isString())
      a = (d->strOutText.isRightToLeft()) ?
                               Style::Right : Style::Left;
    else {
      Value val = value();
      while (val.isArray()) val = val.element (0, 0);
      if (val.isBoolean() || val.isNumber())
        a = Style::Right;
      else
        a = Style::Left;
    }
  }
  return a;
}

int Cell::effAlignX()
{
  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( Style::SHAlign, true ) )
    return d->extra()->conditions->matchedStyle()->halign();

  return defineAlignX();
}


double Cell::dblWidth( int _col ) const
{
  if ( _col < 0 )
    _col = d->column;

  if ( testFlag(Flag_Merged) )
    return d->extra()->extraWidth;

  const ColumnFormat *cl = format()->sheet()->columnFormat( _col );
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

  const RowFormat *rl = format()->sheet()->rowFormat( _row );
  return rl->dblHeight();
}

int Cell::height( int _row ) const
{
  return int( dblHeight( _row ) );
}

///////////////////////////////////////////
//
// Misc Properties.
// Reimplementation of Format methods.
//
///////////////////////////////////////////

const QBrush& Cell::backGroundBrush( int _col, int _row ) const
{
  if ( d->hasExtra() && (!d->extra()->obscuringCells.isEmpty()) )
  {
    const Cell* cell = d->extra()->obscuringCells.first();
    return cell->backGroundBrush( cell->column(), cell->row() );
  }

  return format()->backGroundBrush( _col, _row );
}

const QColor Cell::bgColor( int _col, int _row ) const
{
  if ( d->hasExtra() && (!d->extra()->obscuringCells.isEmpty()) )
  {
    const Cell* cell = d->extra()->obscuringCells.first();
    return cell->bgColor( cell->column(), cell->row() );
  }

  return format()->bgColor( _col, _row );
}

///////////////////////////////////////////
//
// Borders.
// Reimplementation of Format methods.
//
///////////////////////////////////////////

void Cell::setLeftBorderPen( const QPen& p )
{
  if ( column() == 1 )
  {
    Cell* cell = format()->sheet()->cellAt( column() - 1, row() );
    if ( cell && cell->format()->hasProperty( Style::SRightBorder )
         && format()->sheet()->cellAt( column(), row() ) == this )
        cell->format()->clearProperty( Style::SRightBorder );
  }

  format()->setLeftBorderPen( p );
}

void Cell::setTopBorderPen( const QPen& p )
{
  if ( row() == 1 )
  {
    Cell* cell = format()->sheet()->cellAt( column(), row() - 1 );
    if ( cell && cell->format()->hasProperty( Style::SBottomBorder )
         && format()->sheet()->cellAt( column(), row() ) == this )
        cell->format()->clearProperty( Style::SBottomBorder );
  }
  format()->setTopBorderPen( p );
}

void Cell::setRightBorderPen( const QPen& p )
{
    Cell* cell = 0;
    if ( column() < KS_colMax )
        cell = format()->sheet()->cellAt( column() + 1, row() );

    if ( cell && cell->format()->hasProperty( Style::SLeftBorder )
         && format()->sheet()->cellAt( column(), row() ) == this )
        cell->format()->clearProperty( Style::SLeftBorder );

    format()->setRightBorderPen( p );
}

void Cell::setBottomBorderPen( const QPen& p )
{
    Cell* cell = 0;
    if ( row() < KS_rowMax )
        cell = format()->sheet()->cellAt( column(), row() + 1 );

    if ( cell && cell->format()->hasProperty( Style::STopBorder )
         && format()->sheet()->cellAt( column(), row() ) == this )
        cell->format()->clearProperty( Style::STopBorder );

    format()->setBottomBorderPen( p );
}

const QPen& Cell::rightBorderPen( int _col, int _row ) const
{
    if ( !format()->hasProperty( Style::SRightBorder ) && ( _col < KS_colMax ) )
    {
        Cell * cell = format()->sheet()->cellAt( _col + 1, _row );
        if ( cell && cell->format()->hasProperty( Style::SLeftBorder ) )
            return cell->leftBorderPen( _col + 1, _row );
    }

    return format()->rightBorderPen( _col, _row );
}

const QPen& Cell::leftBorderPen( int _col, int _row ) const
{
    if ( !format()->hasProperty( Style::SLeftBorder ) )
    {
        const Cell * cell = format()->sheet()->cellAt( _col - 1, _row );
        if ( cell && cell->format()->hasProperty( Style::SRightBorder ) )
            return cell->rightBorderPen( _col - 1, _row );
    }

    return format()->leftBorderPen( _col, _row );
}

const QPen& Cell::bottomBorderPen( int _col, int _row ) const
{
    if ( !format()->hasProperty( Style::SBottomBorder ) && ( _row < KS_rowMax ) )
    {
        const Cell * cell = format()->sheet()->cellAt( _col, _row + 1 );
        if ( cell && cell->format()->hasProperty( Style::STopBorder ) )
            return cell->topBorderPen( _col, _row + 1 );
    }

    return format()->bottomBorderPen( _col, _row );
}

const QPen& Cell::topBorderPen( int _col, int _row ) const
{
    if ( !format()->hasProperty( Style::STopBorder ) )
    {
        const Cell * cell = format()->sheet()->cellAt( _col, _row - 1 );
        if ( cell->format()->hasProperty( Style::SBottomBorder ) )
            return cell->bottomBorderPen( _col, _row - 1 );
    }

    return format()->topBorderPen( _col, _row );
}

const QColor Cell::effTextColor( int col, int row ) const
{
  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( Style::STextPen, true ) )
    return d->extra()->conditions->matchedStyle()->pen().color();

  return format()->textColor( col, row );
}

const QPen& Cell::effLeftBorderPen( int col, int row ) const
{
  if ( isPartOfMerged() )
  {
    Cell * cell = d->extra()->obscuringCells.first();
    return cell->effLeftBorderPen( cell->column(), cell->row() );
  }

  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( Style::SLeftBorder, true ) )
    return d->extra()->conditions->matchedStyle()->leftBorderPen();

  return leftBorderPen( col, row );
}

const QPen& Cell::effTopBorderPen( int col, int row ) const
{
  if ( isPartOfMerged() )
  {
    Cell * cell = d->extra()->obscuringCells.first();
    return cell->effTopBorderPen( cell->column(), cell->row() );
  }

  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( Style::STopBorder, true ) )
    return d->extra()->conditions->matchedStyle()->topBorderPen();

  return topBorderPen( col, row );
}

const QPen& Cell::effRightBorderPen( int col, int row ) const
{
  if ( isPartOfMerged() )
  {
    Cell * cell = d->extra()->obscuringCells.first();
    return cell->effRightBorderPen( cell->column(), cell->row() );
  }

  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( Style::SRightBorder, true ) )
    return d->extra()->conditions->matchedStyle()->rightBorderPen();

  return rightBorderPen( col, row );
}

const QPen& Cell::effBottomBorderPen( int col, int row ) const
{
  if ( isPartOfMerged() )
  {
    Cell * cell = d->extra()->obscuringCells.first();
    return cell->effBottomBorderPen( cell->column(), cell->row() );
  }

  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( Style::SBottomBorder, true ) )
    return d->extra()->conditions->matchedStyle()->bottomBorderPen();

  return bottomBorderPen( col, row );
}

const QPen & Cell::effGoUpDiagonalPen( int col, int row ) const
{
  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( Style::SGoUpDiagonal, true ) )
    return d->extra()->conditions->matchedStyle()->goUpDiagonalPen();

  return format()->goUpDiagonalPen( col, row );
}

const QPen & Cell::effFallDiagonalPen( int col, int row ) const
{
  if ( d->hasExtra() && d->extra()->conditions
       && d->extra()->conditions->matchedStyle()
       && d->extra()->conditions->matchedStyle()->hasFeature( Style::SFallDiagonal, true ) )
    return d->extra()->conditions->matchedStyle()->fallDiagonalPen();

  return format()->fallDiagonalPen( col, row );
}

uint Cell::effBottomBorderValue( int col, int row ) const
{
  if ( isPartOfMerged() )
  {
    Cell * cell = d->extra()->obscuringCells.first();
    return cell->effBottomBorderValue( cell->column(), cell->row() );
  }

  if ( d->hasExtra() && d->extra()->conditions
      && d->extra()->conditions->matchedStyle() )
    return d->extra()->conditions->matchedStyle()->bottomPenValue();

  return format()->bottomBorderValue( col, row );
}

uint Cell::effRightBorderValue( int col, int row ) const
{
  if ( isPartOfMerged() )
  {
    Cell * cell = d->extra()->obscuringCells.first();
    return cell->effRightBorderValue( cell->column(), cell->row() );
  }

  if ( d->hasExtra() && d->extra()->conditions
      && d->extra()->conditions->matchedStyle() )
    return d->extra()->conditions->matchedStyle()->rightPenValue();

  return format()->rightBorderValue( col, row );
}

uint Cell::effLeftBorderValue( int col, int row ) const
{
  if ( isPartOfMerged() )
  {
    Cell * cell = d->extra()->obscuringCells.first();
    return cell->effLeftBorderValue( cell->column(), cell->row() );
  }

  if ( d->hasExtra() && d->extra()->conditions
      && d->extra()->conditions->matchedStyle() )
    return d->extra()->conditions->matchedStyle()->leftPenValue();

  return format()->leftBorderValue( col, row );
}

uint Cell::effTopBorderValue( int col, int row ) const
{
  if ( isPartOfMerged() )
  {
    Cell * cell = d->extra()->obscuringCells.first();
    return cell->effTopBorderValue( cell->column(), cell->row() );
  }

  if ( d->hasExtra() && d->extra()->conditions
      && d->extra()->conditions->matchedStyle() )
    return d->extra()->conditions->matchedStyle()->topPenValue();

  return format()->topBorderValue( col, row );
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
  int tmpPreci = format()->precision( column(), row() );

  if ( tmpPreci == -1 )
  {
    int pos = d->strOutText.indexOf( locale()->decimalSymbol() );
    if ( pos == -1 )
        pos = d->strOutText.indexOf('.');
    if ( pos == -1 )
      format()->setPrecision(1);
    else
    {
      int start = 0;
      if ( d->strOutText.indexOf('%') != -1 )
        start = 2;
      else if ( d->strOutText.indexOf(locale()->currencySymbol()) == ((int)(d->strOutText.length()-locale()->currencySymbol().length())) )
        start = locale()->currencySymbol().length() + 1;
      else if ( (start=d->strOutText.indexOf('E')) != -1 )
        start = d->strOutText.length() - start;

      //kDebug(36001) << "start=" << start << " pos=" << pos << " length=" << d->strOutText.length() << endl;
      format()->setPrecision( qMax( 0, (int)d->strOutText.length() - start - pos ) );
    }
  }
  else if ( tmpPreci < 10 )
  {
    format()->setPrecision( ++tmpPreci );
  }
  setFlag(Flag_LayoutDirty);
}

void Cell::decPrecision()
{
  //TODO: This is ugly. Why not simply regenerate the text to display? Tomas

  if ( !value().isNumber() )
    return;
  int preciTmp = format()->precision( column(), row() );
//  kDebug(36001) << "decPrecision: tmpPreci = " << tmpPreci << endl;
  if ( format()->precision(column(),row()) == -1 )
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

    format()->setPrecision(d->strOutText.length() - pos - 2 - start);
    //   if ( preciTmp < 0 )
    //      format()->setPrecision( preciTmp );
  }
  else if ( preciTmp > 0 )
  {
    format()->setPrecision( --preciTmp );
  }
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
  if ( !format()->sheet()->isLoading() && !testValidity() )
  {
    //reapply old value if action == stop
    setDisplayText( oldText );
  }
}

void Cell::setDisplayText( const QString& _text )
{
//   kDebug() << k_funcinfo << endl;
  const bool isLoading = format()->sheet()->isLoading();

  if ( !isLoading )
    format()->sheet()->doc()->emitBeginOperation( false );

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
    format()->sheet()->doc()->emitEndOperation( QRect( d->column, d->row, 1, 1 ) );
}

void Cell::setLink( const QString& link )
{
  d->extra()->link = link;

  if( !link.isEmpty() && d->strText.isEmpty() )
    setCellText( link );
}

QString Cell::link() const
{
  return d->hasExtra() ? d->extra()->link : QString::null;
}

bool Cell::testValidity() const
{
    bool valid = false;
    if( d->hasExtra() && d->extra()->validity && d->extra()->validity->m_restriction != Restriction::None )
    {
        //fixme
        if ( d->extra()->validity->allowEmptyCell && d->strText.isEmpty() )
            return true;

        if( value().isNumber() &&
            (d->extra()->validity->m_restriction == Restriction::Number ||
             (d->extra()->validity->m_restriction == Restriction::Integer &&
              value().asFloat() == ceil(value().asFloat()))))
        {
            switch( d->extra()->validity->m_cond)
            {
              case Conditional::Equal:
                valid = ( value().asFloat() - d->extra()->validity->valMin < DBL_EPSILON
                          && value().asFloat() - d->extra()->validity->valMin >
                          (0.0 - DBL_EPSILON));
                break;
              case Conditional::DifferentTo:
                valid = !(  ( value().asFloat() - d->extra()->validity->valMin < DBL_EPSILON
                              && value().asFloat() - d->extra()->validity->valMin >
                              (0.0 - DBL_EPSILON)) );
                break;
              case Conditional::Superior:
                valid = ( value().asFloat() > d->extra()->validity->valMin);
                break;
              case Conditional::Inferior:
                valid = ( value().asFloat()  <d->extra()->validity->valMin);
                break;
              case Conditional::SuperiorEqual:
                valid = ( value().asFloat() >= d->extra()->validity->valMin);
                break;
              case Conditional::InferiorEqual:
                valid = (value().asFloat() <= d->extra()->validity->valMin);
                break;
              case Conditional::Between:
                valid = ( value().asFloat() >= d->extra()->validity->valMin &&
                          value().asFloat() <= d->extra()->validity->valMax);
                break;
              case Conditional::Different:
                valid = (value().asFloat() < d->extra()->validity->valMin ||
                         value().asFloat() > d->extra()->validity->valMax);
                break;
            default :
                break;
            }
        }
        else if(d->extra()->validity->m_restriction==Restriction::Text)
        {
            valid = value().isString();
        }
        else if ( d->extra()->validity->m_restriction == Restriction::List )
        {
            //test int value
            if ( value().isString() && d->extra()->validity->listValidity.contains( value().asString() ) )
                valid = true;
        }
        else if(d->extra()->validity->m_restriction==Restriction::TextLength)
        {
            if( value().isString() )
            {
                int len = d->strOutText.length();
                switch( d->extra()->validity->m_cond)
                {
                  case Conditional::Equal:
                    if (len == d->extra()->validity->valMin)
                        valid = true;
                    break;
                  case Conditional::DifferentTo:
                    if (len != d->extra()->validity->valMin)
                        valid = true;
                    break;
                  case Conditional::Superior:
                    if(len > d->extra()->validity->valMin)
                        valid = true;
                    break;
                  case Conditional::Inferior:
                    if(len < d->extra()->validity->valMin)
                        valid = true;
                    break;
                  case Conditional::SuperiorEqual:
                    if(len >= d->extra()->validity->valMin)
                        valid = true;
                    break;
                  case Conditional::InferiorEqual:
                    if(len <= d->extra()->validity->valMin)
                        valid = true;
                    break;
                  case Conditional::Between:
                    if(len >= d->extra()->validity->valMin && len <= d->extra()->validity->valMax)
                        valid = true;
                    break;
                  case Conditional::Different:
                    if(len <d->extra()->validity->valMin || len >d->extra()->validity->valMax)
                        valid = true;
                    break;
                default :
                    break;
                }
            }
        }
        else if(d->extra()->validity->m_restriction == Restriction::Time && isTime())
        {
            switch( d->extra()->validity->m_cond)
            {
              case Conditional::Equal:
                valid = (value().asTime() == d->extra()->validity->timeMin);
                break;
              case Conditional::DifferentTo:
                valid = (value().asTime() != d->extra()->validity->timeMin);
                break;
              case Conditional::Superior:
                valid = (value().asTime() > d->extra()->validity->timeMin);
                break;
              case Conditional::Inferior:
                valid = (value().asTime() < d->extra()->validity->timeMin);
                break;
              case Conditional::SuperiorEqual:
                valid = (value().asTime() >= d->extra()->validity->timeMin);
                break;
              case Conditional::InferiorEqual:
                valid = (value().asTime() <= d->extra()->validity->timeMin);
                break;
              case Conditional::Between:
                valid = (value().asTime() >= d->extra()->validity->timeMin &&
                         value().asTime() <= d->extra()->validity->timeMax);
                break;
              case Conditional::Different:
                valid = (value().asTime() < d->extra()->validity->timeMin ||
                         value().asTime() > d->extra()->validity->timeMax);
                break;
            default :
                break;

            }
        }
        else if(d->extra()->validity->m_restriction == Restriction::Date && isDate())
        {
            switch( d->extra()->validity->m_cond)
            {
              case Conditional::Equal:
                valid = (value().asDate() == d->extra()->validity->dateMin);
                break;
              case Conditional::DifferentTo:
                valid = (value().asDate() != d->extra()->validity->dateMin);
                break;
              case Conditional::Superior:
                valid = (value().asDate() > d->extra()->validity->dateMin);
                break;
              case Conditional::Inferior:
                valid = (value().asDate() < d->extra()->validity->dateMin);
                break;
              case Conditional::SuperiorEqual:
                valid = (value().asDate() >= d->extra()->validity->dateMin);
                break;
              case Conditional::InferiorEqual:
                valid = (value().asDate() <= d->extra()->validity->dateMin);
                break;
              case Conditional::Between:
                valid = (value().asDate() >= d->extra()->validity->dateMin &&
                         value().asDate() <= d->extra()->validity->dateMax);
                break;
              case Conditional::Different:
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

    if(!valid &&d->extra()->validity != 0 && d->extra()->validity->displayMessage)
    {
        switch (d->extra()->validity->m_action )
        {
          case Action::Stop:
            KMessageBox::error((QWidget*)0, d->extra()->validity->message,
                               d->extra()->validity->title);
            break;
          case Action::Warning:
            KMessageBox::warningYesNo((QWidget*)0, d->extra()->validity->message,
                                      d->extra()->validity->title);
            break;
          case Action::Information:
            KMessageBox::information((QWidget*)0, d->extra()->validity->message,
                                     d->extra()->validity->title);
            break;
        }
    }
    if (!d->hasExtra())
        return true;  //okay if there's no validity
    return (valid || d->extra()->validity == 0 || d->extra()->validity->m_action != Action::Stop);
}

FormatType Cell::formatType() const
{
    return format()->getFormatType( d->column, d->row );
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
        foreach ( CellBinding* binding, format()->sheet()->cellBindings() )
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

void Cell::convertToDouble ()
{
  if (isDefault())
    return;

  setValue (getDouble ());
}

void Cell::convertToPercent ()
{
  if (isDefault())
    return;

  setValue (getDouble ());
  d->value.setFormat (Value::fmt_Percent);
}

void Cell::convertToMoney ()
{
  if (isDefault())
    return;

  setValue (getDouble ());
  d->value.setFormat (Value::fmt_Money);
  format()->setPrecision (locale()->fracDigits());
}

void Cell::convertToTime ()
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

void Cell::convertToDate ()
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
    d->strText = locale()->formatTime( value().asDateTime().time(), true);

  // convert first letter to uppercase ?
  if (format()->sheet()->getFirstLetterUpper() && value().isString() &&
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
            format()->setFormatType( Scientific_format );
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
    QDomElement formatElement = format()->save( doc, column(), row(), force, copy );
    if ( formatElement.hasChildNodes() || formatElement.attributes().length() ) // don't save empty tags
        cell.appendChild( formatElement );

    if ( doesMergeCells() )
    {
        if ( extraXCells() )
            formatElement.setAttribute( "colspan", extraXCells() );
        if ( extraYCells() )
            formatElement.setAttribute( "rowspan", extraYCells() );
    }

    if ( d->hasExtra() && d->extra()->conditions )
    {
      QDomElement conditionElement = d->extra()->conditions->saveConditions( doc );

      if ( !conditionElement.isNull() )
        cell.appendChild( conditionElement );
    }

    if (validity())
    {
        Validity *validity = this->validity();
        QDomElement validityElement = doc.createElement("validity");

        QDomElement param=doc.createElement("param");
        param.setAttribute("cond",(int)validity->m_cond);
        param.setAttribute("action",(int)validity->m_action);
        param.setAttribute("allow",(int)validity->m_restriction);
        param.setAttribute("valmin",validity->valMin);
        param.setAttribute("valmax",validity->valMax);
        param.setAttribute("displaymessage",validity->displayMessage);
        param.setAttribute("displayvalidationinformation",validity->displayValidationInformation);
        param.setAttribute("allowemptycell",validity->allowEmptyCell);
        if ( !validity->listValidity.isEmpty() )
            param.setAttribute( "listvalidity", validity->listValidity.join( ";" ) );
        validityElement.appendChild(param);
        QDomElement title = doc.createElement( "title" );
        title.appendChild( doc.createTextNode( validity->title ) );
        validityElement.appendChild( title );
        QDomElement message = doc.createElement( "message" );
        message.appendChild( doc.createCDATASection( validity->message ) );
        validityElement.appendChild( message );

        QDomElement inputTitle = doc.createElement( "inputtitle" );
        inputTitle.appendChild( doc.createTextNode( validity->titleInfo ) );
        validityElement.appendChild( inputTitle );

        QDomElement inputMessage = doc.createElement( "inputmessage" );
        inputMessage.appendChild( doc.createTextNode( validity->messageInfo ) );
        validityElement.appendChild( inputMessage );



        QString tmp;
        if ( validity->timeMin.isValid() )
        {
                QDomElement timeMin = doc.createElement( "timemin" );
                tmp=validity->timeMin.toString();
                timeMin.appendChild( doc.createTextNode( tmp ) );
                validityElement.appendChild( timeMin );
        }
        if ( validity->timeMax.isValid() )
        {
                QDomElement timeMax = doc.createElement( "timemax" );
                tmp=validity->timeMax.toString();
                timeMax.appendChild( doc.createTextNode( tmp ) );
                validityElement.appendChild( timeMax );
        }

        if ( validity->dateMin.isValid() )
        {
                QDomElement dateMin = doc.createElement( "datemin" );
                QString tmp("%1/%2/%3");
                tmp = tmp.arg(validity->dateMin.year()).arg(validity->dateMin.month()).arg(validity->dateMin.day());
                dateMin.appendChild( doc.createTextNode( tmp ) );
                validityElement.appendChild( dateMin );
        }
        if ( validity->dateMax.isValid() )
        {
                QDomElement dateMax = doc.createElement( "datemax" );
                QString tmp("%1/%2/%3");
                tmp = tmp.arg(validity->dateMax.year()).arg(validity->dateMax.month()).arg(validity->dateMax.day());
                dateMax.appendChild( doc.createTextNode( tmp ) );
                validityElement.appendChild( dateMax );
        }

        cell.appendChild( validityElement );
    }

    if ( format()->comment() )
    {
        QDomElement comment = doc.createElement( "comment" );
        comment.appendChild( doc.createCDATASection( *format()->comment() ) );
        cell.appendChild( comment );
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

void Cell::saveOasisAnnotation( KoXmlWriter &xmlwriter )
{
    if ( format()->comment() )
    {
        //<office:annotation draw:style-name="gr1" draw:text-style-name="P1" svg:width="2.899cm" svg:height="2.691cm" svg:x="2.858cm" svg:y="0.001cm" draw:caption-point-x="-2.858cm" draw:caption-point-y="-0.001cm">
        xmlwriter.startElement( "office:annotation" );
        QStringList text = format()->comment()->split( "\n", QString::SkipEmptyParts );
        for ( QStringList::Iterator it = text.begin(); it != text.end(); ++it ) {
            xmlwriter.startElement( "text:p" );
            xmlwriter.addTextNode( *it );
            xmlwriter.endElement();
        }
        xmlwriter.endElement();
    }
}



QString Cell::saveOasisCellStyle( KoGenStyle &currentCellStyle, KoGenStyles &mainStyles )
{
    if ( d->hasExtra() && d->extra()->conditions )
    {
        // this has to be an automatic style
        currentCellStyle = KoGenStyle( Doc::STYLE_CELL_AUTO, "table-cell" );
        d->extra()->conditions->saveOasisConditions( currentCellStyle );
    }
    return format()->saveOasisCellStyle( currentCellStyle, mainStyles );
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
    saveOasisCellStyle( currentCellStyle,mainStyles );
    // skip 'table:style-name' attribute for the default style
    if ( !currentCellStyle.isDefaultStyle() )
      xmlwriter.addAttribute( "table:style-name", mainStyles.styles()[currentCellStyle] );

    // group empty cells with the same style
    if ( isEmpty() && !format()->hasProperty( Style::SComment ) && !isPartOfMerged() && !doesMergeCells() )
    {
      bool refCellIsDefault = isDefault();
      int j = column + 1;
      Cell *nextCell = format()->sheet()->getNextCellRight( column, row );
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
        nextCell->saveOasisCellStyle( nextCellStyle,mainStyles );

        if ( nextCell->isPartOfMerged() || nextCell->doesMergeCells() ||
             nextCell->format()->hasProperty( Style::SComment ) ||
             !(nextCellStyle == currentCellStyle) )
        {
          break;
        }
        ++repeated;
        // get the next cell and set the index to the adjacent cell
        nextCell = format()->sheet()->getNextCellRight( j++, row );
      }
      kDebug() << "Cell::saveOasis: empty cell in column " << column << " "
                << "repeated " << repeated << " time(s)" << endl;

      if ( repeated > 1 )
        xmlwriter.addAttribute( "table:number-columns-repeated", QString::number( repeated ) );
    }

    Validity *validity = this->validity();
    if (validity)
    {
        GenValidationStyle styleVal(validity);
        xmlwriter.addAttribute( "table:validation-name", valStyle.lookup( styleVal ) );
    }
    if ( isFormula() )
    {
      //kDebug() << "Formula found" << endl;
      QString formula = Oasis::encodeFormula( text(), locale() );
      xmlwriter.addAttribute( "table:formula", formula );
    }
    else if ( !link().isEmpty() )
    {
        //kDebug()<<"Link found \n";
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

    saveOasisAnnotation( xmlwriter );

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
      Style::Currency currency;
      format()->currencyInfo(currency);
      xmlWriter.addAttribute( "office:currency",
                              Currency::getCurrencyCode(currency.type) );
      xmlWriter.addAttribute( "office:value",
          QString::number( value().asFloat() ) );
      break;
    }
    case Value::fmt_DateTime: break;  //NOTHING HERE
    case Value::fmt_Date:
    {
      xmlWriter.addAttribute( "office:value-type", "date" );
      xmlWriter.addAttribute( "office:date-value",
          value().asDate().toString( Qt::ISODate ) );
      break;
    }
    case Value::fmt_Time:
    {
      xmlWriter.addAttribute( "office:value-type", "time" );
      xmlWriter.addAttribute( "office:time-value",
          value().asTime().toString( "PThhHmmMssS" ) );
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

void Cell::loadOasisConditional( KoXmlElement * style )
{
    //kDebug()<<" void Cell::loadOasisConditional( KoXmlElement * style  :"<<style<<endl;
    if ( style )//safe
    {
        //TODO fixme it doesn't work :(((
        KoXmlElement e;
        forEachElement( e, style->toElement() )
        {
//             kDebug()<<"e.localName() :"<<e.localName()<<endl;
            if ( e.localName() == "map" && e.namespaceURI() == KoXmlNS::style )
            {
                if (d->hasExtra())
                    delete d->extra()->conditions;
                d->extra()->conditions = new Conditions( this );
                d->extra()->conditions->loadOasisConditions( e );
                d->extra()->conditions->checkMatches();
                break;
            }
        }

    }
}

bool Cell::loadOasis( const KoXmlElement& element , KoOasisLoadingContext& oasisContext , Style* style )
{
    kDebug() << "*** Loading cell properties ***** at " << column() << ',' << row () << endl;

    if ( element.hasAttributeNS( KoXmlNS::table, "style-name" ) )
    {
        kDebug()<<" table:style-name: "<<element.attributeNS( KoXmlNS::table, "style-name", QString::null )<<endl;
        oasisContext.fillStyleStack( element, KoXmlNS::table, "styleName", "table-cell" );

        QString str = element.attributeNS( KoXmlNS::table, "style-name", QString::null );
        KoXmlElement* cellStyle = const_cast<KoXmlElement*>( oasisContext.oasisStyles().findStyle( str, "table-cell" ) );

        if ( cellStyle )
            loadOasisConditional( const_cast<KoXmlElement *>( cellStyle ) );
    }

    if (style)
    {
        format()->setStyle( style );
    }

    //Search and load each paragraph of text. Each paragraph is separated by a line break.
    loadOasisCellText( element );

    //
    // formula
    //
    bool isFormula = false;
    if ( element.hasAttributeNS( KoXmlNS::table, "formula" ) )
    {
        kDebug()<<" formula :"<<element.attributeNS( KoXmlNS::table, "formula", QString::null )<<endl;
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
        kDebug()<<" validation-name: "<<element.attributeNS( KoXmlNS::table, "validation-name", QString::null )<<endl;
        loadOasisValidation( element.attributeNS( KoXmlNS::table, "validation-name", QString::null ) );
    }

    //
    // value type
    //
    if( element.hasAttributeNS( KoXmlNS::office, "value-type" ) )
    {
        QString valuetype = element.attributeNS( KoXmlNS::office, "value-type", QString::null );
        kDebug()<<"  value-type: " << valuetype << endl;
        if( valuetype == "boolean" )
        {
          QString val = element.attributeNS( KoXmlNS::office, "boolean-value", QString::null ).toLower();
            if( ( val == "true" ) || ( val == "false" ) )
            {
                bool value = val == "true";
                setValue( value );
                if (!isFormula) setCellText( value ? i18n("True") : i18n("False" ) );
            }
        }

        // integer and floating-point value
        else if( valuetype == "float" )
        {
            bool ok = false;
            double value = element.attributeNS( KoXmlNS::office, "value", QString::null ).toDouble( &ok );
            if ( !isFormula )
                if( ok )
                    setCellValue( value );

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
                if ( !isFormula )
                    setCellValue( value );
                if (element.hasAttributeNS( KoXmlNS::office, "currency" ) )
                {
                  Currency currency(element.attributeNS( KoXmlNS::office, "currency", QString::null ) );
                  format()->setCurrency( currency.getIndex(), currency.getDisplayCode() );
                }
                format()->setFormatType (Money_format);
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
                setCellValue( value );

                if ( !isFormula && d->strText.isEmpty())
                {
                    QString str = locale()->formatNumber( percent, 15 );
                    setCellText( str );
                }

                format()->setFormatType (Percentage_format);
            }
        }
        else if ( valuetype == "date" )
        {
            QString value = element.attributeNS( KoXmlNS::office, "value", QString::null );
            if ( value.isEmpty() )
                value = element.attributeNS( KoXmlNS::office, "date-value", QString::null );
            kDebug() << "Type: date, value: " << value << endl;

            // "1980-10-15"
            int year = 0, month = 0, day = 0;
            bool ok = false;

            int p1 = value.indexOf( '-' );
            if ( p1 > 0 )
                year  = value.left( p1 ).toInt( &ok );

            kDebug() << "year: " << value.left( p1 ) << endl;

            int p2 = value.indexOf( '-', ++p1 );

            if ( ok )
                month = value.mid( p1, p2 - p1  ).toInt( &ok );

            kDebug() << "month: " << value.mid( p1, p2 - p1 ) << endl;

            if ( ok )
                day = value.right( value.length() - p2 - 1 ).toInt( &ok );

            kDebug() << "day: " << value.right( value.length() - p2 ) << endl;

            if ( ok )
            {
                setCellValue( QDate( year, month, day ) );
                format()->setFormatType (ShortDate_format);
                kDebug() << "Set QDate: " << year << " - " << month << " - " << day << endl;
            }

        }
        else if ( valuetype == "time" )
        {
            QString value = element.attributeNS( KoXmlNS::office, "value", QString::null );
            if ( value.isEmpty() )
                value = element.attributeNS( KoXmlNS::office, "time-value", QString::null );
            kDebug() << "Type: time: " << value << endl;
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

                kDebug() << "Num: " << num << endl;

                num = "";
                if ( !ok )
                    break;
            }
            kDebug() << "Hours: " << hours << ", " << minutes << ", " << seconds << endl;

            if ( ok )
            {
                // Value kval( timeToNum( hours, minutes, seconds ) );
                // cell->setValue( kval );
                setCellValue( QTime( hours % 24, minutes, seconds ) );
                format()->setFormatType (Time_format);
            }
        }
        else if( valuetype == "string" )
        {
            QString value = element.attributeNS( KoXmlNS::office, "value", QString::null );
            if ( value.isEmpty() && element.hasAttributeNS( KoXmlNS::office, "string-value" ))
            {
                //if there is not string-value entry don't overwrite value stored into <text:p>
                value = element.attributeNS( KoXmlNS::office, "string-value", QString::null );
                setCellValue( value );
            }
            format()->setFormatType (Text_format);
        }
        else
            kDebug()<<" type of value found : "<<valuetype<<endl;
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
            format()->setComment( comment );
    }

    KoXmlElement frame = KoDom::namedItemNS( element, KoXmlNS::draw, "frame" );
    if ( !frame.isNull() )
      loadOasisObjects( frame, oasisContext );

    if (isFormula)   // formulas must be recalculated
      format()->sheet()->doc()->addDamage( new CellDamage( this, CellDamage::Formula |
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
                    setValue( cellText );
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
        setValue( cellText );
    }

    //Enable word wrapping if multiple lines of text have been found.
    if ( multipleTextParagraphsFound )
    {
        format()->setMultiRow(true);
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
              kDebug() << "Object type wasn't loaded!" << endl;
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
    removeValidity ();
    Validity *validity = this->validity (true);
    if ( element.hasAttributeNS( KoXmlNS::table, "condition" ) )
    {
        QString valExpression = element.attributeNS( KoXmlNS::table, "condition", QString::null );
        kDebug()<<" element.attribute( table:condition ) "<<valExpression<<endl;
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
            kDebug()<<" valExpression = :"<<valExpression<<endl;
            validity->m_restriction = Restriction::TextLength;

            loadOasisValidationCondition( valExpression );
        }
        else if ( valExpression.contains( "cell-content-is-text()" ) )
        {
            validity->m_restriction = Restriction::Text;
        }
        //cell-content-text-length-is-between(Value, Value) | cell-content-text-length-is-not-between(Value, Value) | cell-content-is-in-list( StringList )
        else if ( valExpression.contains( "cell-content-text-length-is-between" ) )
        {
            validity->m_restriction = Restriction::TextLength;
            validity->m_cond = Conditional::Between;
            valExpression = valExpression.remove( "oooc:cell-content-text-length-is-between(" );
            kDebug()<<" valExpression :"<<valExpression<<endl;
            valExpression = valExpression.remove( ')' );
            QStringList listVal = valExpression.split( ',', QString::SkipEmptyParts );
            loadOasisValidationValue( listVal );
        }
        else if ( valExpression.contains( "cell-content-text-length-is-not-between" ) )
        {
            validity->m_restriction = Restriction::TextLength;
            validity->m_cond = Conditional::Different;
            valExpression = valExpression.remove( "oooc:cell-content-text-length-is-not-between(" );
            kDebug()<<" valExpression :"<<valExpression<<endl;
            valExpression = valExpression.remove( ')' );
            kDebug()<<" valExpression :"<<valExpression<<endl;
            QStringList listVal = valExpression.split( ',', QString::SkipEmptyParts );
            loadOasisValidationValue( listVal );
        }
        else if ( valExpression.contains( "cell-content-is-in-list(" ) )
        {
            validity->m_restriction = Restriction::List;
            valExpression = valExpression.remove( "oooc:cell-content-is-in-list(" );
            kDebug()<<" valExpression :"<<valExpression<<endl;
            valExpression = valExpression.remove( ')' );
            validity->listValidity = valExpression.split( ';',  QString::SkipEmptyParts );

        }
        //TrueFunction ::= cell-content-is-whole-number() | cell-content-is-decimal-number() | cell-content-is-date() | cell-content-is-time()
        else
        {
            if (valExpression.contains( "cell-content-is-whole-number()" ) )
            {
                validity->m_restriction =  Restriction::Number;
                valExpression = valExpression.remove( "oooc:cell-content-is-whole-number() and " );
            }
            else if (valExpression.contains( "cell-content-is-decimal-number()" ) )
            {
                validity->m_restriction = Restriction::Integer;
                valExpression = valExpression.remove( "oooc:cell-content-is-decimal-number() and " );
            }
            else if (valExpression.contains( "cell-content-is-date()" ) )
            {
                validity->m_restriction = Restriction::Date;
                valExpression = valExpression.remove( "oooc:cell-content-is-date() and " );
            }
            else if (valExpression.contains( "cell-content-is-time()" ) )
            {
                validity->m_restriction = Restriction::Time;
                valExpression = valExpression.remove( "oooc:cell-content-is-time() and " );
            }
            kDebug()<<"valExpression :"<<valExpression<<endl;

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
                valExpression = valExpression.remove( ')' );
                QStringList listVal = valExpression.split( ',', QString::SkipEmptyParts );
                loadOasisValidationValue( listVal );
                validity->m_cond = Conditional::Between;
            }
            if ( valExpression.contains( "cell-content-is-not-between(" ) )
            {
                valExpression = valExpression.remove( "cell-content-is-not-between(" );
                valExpression = valExpression.remove( ')' );
                QStringList listVal = valExpression.split( ',', QString::SkipEmptyParts );
                loadOasisValidationValue( listVal );
                validity->m_cond = Conditional::Different;
            }
        }
    }
    if ( element.hasAttributeNS( KoXmlNS::table, "allow-empty-cell" ) )
    {
        kDebug()<<" element.hasAttribute( table:allow-empty-cell ) :"<<element.hasAttributeNS( KoXmlNS::table, "allow-empty-cell" )<<endl;
        validity->allowEmptyCell = ( ( element.attributeNS( KoXmlNS::table, "allow-empty-cell", QString::null )=="true" ) ? true : false );
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
            kDebug()<<"help.attribute( table:title ) :"<<help.attributeNS( KoXmlNS::table, "title", QString::null )<<endl;
            validity->titleInfo = help.attributeNS( KoXmlNS::table, "title", QString::null );
        }
        if ( help.hasAttributeNS( KoXmlNS::table, "display" ) )
        {
            kDebug()<<"help.attribute( table:display ) :"<<help.attributeNS( KoXmlNS::table, "display", QString::null )<<endl;
            validity->displayValidationInformation = ( ( help.attributeNS( KoXmlNS::table, "display", QString::null )=="true" ) ? true : false );
        }
        KoXmlElement attrText = KoDom::namedItemNS( help, KoXmlNS::text, "p" );
        if ( !attrText.isNull() )
        {
            kDebug()<<"help text :"<<attrText.text()<<endl;
            validity->messageInfo = attrText.text();
        }
    }

    KoXmlElement error = KoDom::namedItemNS( element, KoXmlNS::table, "error-message" );
    if ( !error.isNull() )
    {
        if ( error.hasAttributeNS( KoXmlNS::table, "title" ) )
            validity->title = error.attributeNS( KoXmlNS::table, "title", QString::null );
        if ( error.hasAttributeNS( KoXmlNS::table, "message-type" ) )
        {
            QString str = error.attributeNS( KoXmlNS::table, "message-type", QString::null );
            if ( str == "warning" )
              validity->m_action = Action::Warning;
            else if ( str == "information" )
              validity->m_action = Action::Information;
            else if ( str == "stop" )
              validity->m_action = Action::Stop;
            else
                kDebug()<<"validation : message type unknown  :"<<str<<endl;
        }

        if ( error.hasAttributeNS( KoXmlNS::table, "display" ) )
        {
            kDebug()<<" display message :"<<error.attributeNS( KoXmlNS::table, "display", QString::null )<<endl;
            validity->displayMessage = (error.attributeNS( KoXmlNS::table, "display", QString::null )=="true");
        }
        KoXmlElement attrText = KoDom::namedItemNS( error, KoXmlNS::text, "p" );
        if ( !attrText.isNull() )
            validity->message = attrText.text();
    }
}


void Cell::loadOasisValidationValue( const QStringList &listVal )
{
    bool ok = false;
    kDebug()<<" listVal[0] :"<<listVal[0]<<" listVal[1] :"<<listVal[1]<<endl;
    
    Validity *validity = this->validity(true);
    
    if ( validity->m_restriction == Restriction::Date )
    {
        validity->dateMin = QDate::fromString( listVal[0] );
        validity->dateMax = QDate::fromString( listVal[1] );
    }
    else if ( validity->m_restriction == Restriction::Time )
    {
        validity->timeMin = QTime::fromString( listVal[0] );
        validity->timeMax = QTime::fromString( listVal[1] );
    }
    else
    {
        validity->valMin = listVal[0].toDouble(&ok);
        if ( !ok )
        {
            validity->valMin = listVal[0].toInt(&ok);
            if ( !ok )
                kDebug()<<" Try to parse this value :"<<listVal[0]<<endl;

#if 0
            if ( !ok )
                validity->valMin = listVal[0];
#endif
        }
        ok=false;
        validity->valMax = listVal[1].toDouble(&ok);
        if ( !ok )
        {
            validity->valMax = listVal[1].toInt(&ok);
            if ( !ok )
                kDebug()<<" Try to parse this value :"<<listVal[1]<<endl;

#if 0
            if ( !ok )
                validity->valMax = listVal[1];
#endif
        }
    }
}

void Cell::loadOasisValidationCondition( QString &valExpression )
{
    Validity *validity = this->validity (true);
    if (!validity) return;
    QString value;
    if (valExpression.indexOf( "<=" )==0 )
    {
        value = valExpression.remove( 0,2 );
        validity->m_cond = Conditional::InferiorEqual;
    }
    else if (valExpression.indexOf( ">=" )==0 )
    {
        value = valExpression.remove( 0,2 );
        validity->m_cond = Conditional::SuperiorEqual;
    }
    else if (valExpression.indexOf( "!=" )==0 )
    {
        //add Differentto attribute
        value = valExpression.remove( 0,2 );
        validity->m_cond = Conditional::DifferentTo;
    }
    else if ( valExpression.indexOf( '<' )==0 )
    {
        value = valExpression.remove( 0,1 );
        validity->m_cond = Conditional::Inferior;
    }
    else if(valExpression.indexOf( '>' )==0 )
    {
        value = valExpression.remove( 0,1 );
        validity->m_cond = Conditional::Superior;
    }
    else if (valExpression.indexOf( '=' )==0 )
    {
        value = valExpression.remove( 0,1 );
        validity->m_cond = Conditional::Equal;
    }
    else
        kDebug()<<" I don't know how to parse it :"<<valExpression<<endl;
    if ( validity->m_restriction == Restriction::Date )
    {
        validity->dateMin = QDate::fromString( value );
    }
    else if (validity->m_restriction == Restriction::Date )
    {
        validity->timeMin = QTime::fromString( value );
    }
    else
    {
        bool ok = false;
        validity->valMin = value.toDouble(&ok);
        if ( !ok )
        {
            validity->valMin = value.toInt(&ok);
            if ( !ok )
                kDebug()<<" Try to parse this value :"<<value<<endl;

#if 0
            if ( !ok )
                validity->valMin = value;
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

      if ( !format()->load( formatElement, pm, paste ) )
            return false;

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
      if (d->hasExtra())
        delete d->extra()->conditions;
      d->extra()->conditions = new Conditions( this );
      d->extra()->conditions->loadConditions( conditionsElement );
      d->extra()->conditions->checkMatches();
    }
    else if ((pm == Paste::Normal) || (pm == Paste::NoBorder))
    {
      //clear the conditional formatting
      if (d->hasExtra())
      {
        delete d->extra()->conditions;
        d->extra()->conditions = 0;
      }
    }

    KoXmlElement validityElement = cell.namedItem( "validity" ).toElement();
    if ( !validityElement.isNull())
    {
        KoXmlElement param = validityElement.namedItem( "param" ).toElement();
        Validity *validity = this->validity (true);
        if(!param.isNull())
        {
          if ( param.hasAttribute( "cond" ) )
          {
            validity->m_cond = (Conditional::Type) param.attribute("cond").toInt( &ok );
            if ( !ok )
              return false;
          }
          if ( param.hasAttribute( "action" ) )
          {
            validity->m_action = (Action::Type) param.attribute("action").toInt( &ok );
            if ( !ok )
              return false;
          }
          if ( param.hasAttribute( "allow" ) )
          {
            validity->m_restriction = (Restriction::Type) param.attribute("allow").toInt( &ok );
            if ( !ok )
              return false;
          }
          if ( param.hasAttribute( "valmin" ) )
          {
            validity->valMin = param.attribute("valmin").toDouble( &ok );
            if ( !ok )
              return false;
          }
          if ( param.hasAttribute( "valmax" ) )
          {
            validity->valMax = param.attribute("valmax").toDouble( &ok );
            if ( !ok )
              return false;
          }
          if ( param.hasAttribute( "displaymessage" ) )
          {
              validity->displayMessage = ( bool )param.attribute("displaymessage").toInt();
          }
          if ( param.hasAttribute( "displayvalidationinformation" ) )
          {
              validity->displayValidationInformation = ( bool )param.attribute("displayvalidationinformation").toInt();
          }
          if ( param.hasAttribute( "allowemptycell" ) )
          {
              validity->allowEmptyCell = ( bool )param.attribute("allowemptycell").toInt();
          }
          if ( param.hasAttribute("listvalidity") )
          {
            validity->listValidity = param.attribute("listvalidity").split(';', QString::SkipEmptyParts );
          }
        }
        KoXmlElement inputTitle = validityElement.namedItem( "inputtitle" ).toElement();
        if (!inputTitle.isNull())
        {
            validity->titleInfo = inputTitle.text();
        }
        KoXmlElement inputMessage = validityElement.namedItem( "inputmessage" ).toElement();
        if (!inputMessage.isNull())
        {
            validity->messageInfo = inputMessage.text();
        }

        KoXmlElement title = validityElement.namedItem( "title" ).toElement();
        if (!title.isNull())
        {
            validity->title = title.text();
        }
        KoXmlElement message = validityElement.namedItem( "message" ).toElement();
        if (!message.isNull())
        {
            validity->message = message.text();
        }
        KoXmlElement timeMin = validityElement.namedItem( "timemin" ).toElement();
        if ( !timeMin.isNull()  )
        {
            validity->timeMin = toTime(timeMin);
        }
        KoXmlElement timeMax = validityElement.namedItem( "timemax" ).toElement();
        if ( !timeMax.isNull()  )
        {
            validity->timeMax = toTime(timeMax);
        }
        KoXmlElement dateMin = validityElement.namedItem( "datemin" ).toElement();
        if ( !dateMin.isNull()  )
        {
            validity->dateMin = toDate(dateMin);
        }
        KoXmlElement dateMax = validityElement.namedItem( "datemax" ).toElement();
        if ( !dateMax.isNull()  )
        {
            validity->dateMax = toDate(dateMax);
        }
    }
    else if ((pm == Paste::Normal) || (pm == Paste::NoBorder))
    {
      // clear the validity
      removeValidity();
    }

    //
    // Load the comment
    //
    KoXmlElement comment = cell.namedItem( "comment" ).toElement();
    if ( !comment.isNull() && ( pm == Paste::Normal || pm == Paste::Comment || pm == Paste::NoBorder ))
    {
        QString t = comment.text();
        //t = t.trimmed();
        format()->setComment( t );
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
            setValue( false );
          else if ( t == "true" )
            setValue( true );
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
            int pos   = t.indexOf( '/' );
            int year  = t.mid( 0, pos ).toInt();
            int pos1  = t.indexOf( '/', pos + 1 );
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
            setCellValue( dd );
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
              setValue( time );
            else
              clear = false;
          }
        }
        else
        {
          setValue( t );
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
        setCellValue (val);
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
  KLocale* locale = format()->sheet()->doc()->locale();

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
        pos = t.indexOf(':');
        hours = t.mid(0,pos).toInt();
        pos1 = t.indexOf(':',pos+1);
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

  if ( !format()->sheet()->isLoading() )
    setCellText( d->strText );

  if ( d->hasExtra() && d->extra()->conditions )
    d->extra()->conditions->checkMatches();

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
    setValue( Value( QTime(hours,minutes,second)) );
    return value().asTime();
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
    setValue( Value( QDate(year,month,day) ) );
    return value().asDate();
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
        Cell* cell = format()->sheet()->cellAt( d->column + x, d->row + y );
        if ( cell )
            cell->unobscure(this);
    }

    d->value = Value::empty();

    if (!isDefault())
      valueChanged ();  //our value has been changed (is now null), but only if we aren't default

    delete d->format;
    delete d->cellView;
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
  if ( *d->format != *other.d->format )
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
    if ( d->extra()->conditions )
    {
      if ( !other.d->extra()->conditions )
        return false;
      if ( *d->extra()->conditions != *other.d->extra()->conditions )
        return false;
    }
    if ( d->extra()->validity )
    {
      if ( !other.d->extra()->validity )
        return false;
      if ( *d->extra()->validity != *other.d->extra()->validity )
        return false;
    }
  }
  return true;
}

QRect Cell::cellRect()
{
  Q_ASSERT(!isDefault());
  return QRect(QPoint(d->column, d->row), QPoint(d->column, d->row));
}

QLinkedList<Conditional> Cell::conditionList() const
{
  if ( !d->hasExtra() || !d->extra()->conditions )
  {
    QLinkedList<Conditional> emptyList;
    return emptyList;
  }

  return d->extra()->conditions->conditionList();
}

void Cell::setConditionList( const QLinkedList<Conditional> & newList )
{
  if (d->hasExtra())
    delete d->extra()->conditions;
  d->extra()->conditions = new Conditions( this );
  d->extra()->conditions->setConditionList( newList );
  d->extra()->conditions->checkMatches();
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
        formula = formula.replace( start, word.length(), '\'' + word + '\'' );
        l = formula.length();
        ++i;
        kDebug() << "Formula: " << formula << ", L: " << l << ", i: " << i + 1 <<endl;
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
      formula = formula.replace( start, word.length(), '\'' + word + '\'' );
      l = formula.length();
      ++i;
      kDebug() << "Formula: " << formula << ", L: " << l << ", i: " << i + 1 <<endl;
    }
  }
}
