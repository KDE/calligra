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
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_CELL_PRIVATE
#define KSPREAD_CELL_PRIVATE

// Qt
#include <QString>

// KSpread
#include "Formula.h"

namespace KSpread
{
class Cell;
class Conditions;
class Validity;
class Value;
}

using namespace KSpread;

// Some variables are placed in Cell::Extra because normally they're
// not required in simple case of cell(s). For example, most plain
// text cells don't need to store information about spanned columns
// and rows, as this is only the case with merged cells.
//
// When the cell is getting complex (e.g. merged with other cells,
// contains rich text, has validation criteria, etc), this Cell::Extra
// is allocated by Cell::Private and starts to be
// available. Otherwise, it won't exist at all.

class Cell::Extra
{
public:
  Extra() {}

  // Not empty when the cell holds a link
  QString link;

  // Number of cells explicitly merged by the user in X and Y directions.
  int mergedXCells;
  int mergedYCells;

  // FIXME Stefan: View related
  // Number of additional cells.
  int extraXCells;
  int extraYCells;

  // FIXME Stefan: View related
  // If this cell overlaps other cells, then we have the cells width and
  // height stored here.  These values do not mean anything unless
  // extraXCells and/or extraYCells are different from 0.
  double extraWidth;
  double extraHeight;

  // FIXME Stefan: View related
  // A list of cells that obscure this one.
  // If this list is not empty, then this cell is obscured by another
  // enlarged object. This means that we have to call this object in order
  // of painting it for example instead of painting 'this'.
  //
  // FIXME (comment): If the list consists of more than one obscuring
  //                  element, then is there an order between them that
  //                  is important?
  QList<Cell*> obscuringCells;

  // If non-0, contains a pointer to a condition or a validity test.
  Conditions  *conditions;
  Validity    *validity;

private:
  // Don't allow implicit copy.
  Extra& operator=( const Extra& );
};


class Cell::Private
{
public:

  Private()
  {
    // Some basic data.
    row     = 0;
    column  = 0;
    value   = Value::empty();
    formula = 0;

    nextCell     = 0;
    previousCell = 0;

    // Default is to not have the "extra" stuff in a cell.
    cellExtra = 0;
    format = 0;
    flags = 0;
  }

  ~Private()
  {
    delete cellExtra;
    delete formula;
  }

public:

  // This cell's row and column. If either of them is 0, this is the
  // default cell and its row/column can not be determined.  Note that
  // in the isDefault() method, only column is tested.
  int  row;
  int  column;

  // Value of the cell, either typed by user or as result of formula
  Value value;

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

  // Pointers to neighboring cells.
  // FIXME (comment): Which order?
  Cell  *nextCell;
  Cell  *previousCell;

  bool        hasExtra() const { return (cellExtra != 0); };
  Extra      *extra()
  {
    if ( !cellExtra ) {
      cellExtra = new Extra;
      cellExtra->conditions   = 0;
      cellExtra->validity     = 0;

      cellExtra->mergedXCells = 0;
      cellExtra->mergedYCells = 0;
      cellExtra->extraXCells  = 0;
      cellExtra->extraYCells  = 0;
      cellExtra->extraWidth   = 0.0;
      cellExtra->extraHeight  = 0.0;
//      cellExtra->highlight    = QColor(0,0,0);
    }

    return cellExtra;
  }

  CellView*   cellView;
  Format*     format;
  StatusFlags flags;

private:
  // "Extra stuff", see explanation for Cell::Extra.
  Extra  *cellExtra;
};

#endif
