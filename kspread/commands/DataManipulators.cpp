/* This file is part of the KDE project
   Copyright (C) 2006 Tomas Mecir <mecirt@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
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


#include "DataManipulators.h"

#include <klocale.h>

#include "Cell.h"
#include "CellStorage.h"
#include "Doc.h"
#include "Sheet.h"
#include "ValueCalc.h"

#include <float.h>
#include <math.h>

using namespace KSpread;

AbstractDataManipulator::AbstractDataManipulator ()
{
}

AbstractDataManipulator::~AbstractDataManipulator ()
{
}

bool AbstractDataManipulator::process (Element* element)
{
  QRect range = element->rect();
  for (int col = range.left(); col <= range.right(); ++col)
    for (int row = range.top(); row <= range.bottom(); ++row) {
      Value val;
      QString text;
//       int colidx = col - range.left();
//       int rowidx = row - range.top();
      bool parse = false;
      Format::Type fmtType = Format::None;

      // do nothing if we don't want a change here
      if (!wantChange (element, col, row))
        continue;

      if (m_reverse) {
        // reverse - use the stored value
        m_sheet->cellStorage()->undo( m_undoData );
      } else {
        val = newValue (element, col, row, &parse, &fmtType);
        if (parse)
          text = val.asString();
      }

      // we have the data - set it !
      if (parse) {
        Cell cell = Cell( m_sheet, col, row);
        if (fmtType != Format::None)
        {
            Style style;
            style.setFormatType (fmtType);
            cell.setStyle( style );
        }
        cell.setCellText (text);
      } else {
        if ( !val.isEmpty() )
        {
          Cell( m_sheet, col, row ).setCellValue( val, fmtType );
        }
      }
    }
  return true;
}

bool AbstractDataManipulator::preProcessing()
{
    // not the first run - data already stored ...
    if ( !m_firstrun )
        return true;
    m_sheet->cellStorage()->startUndoRecording();
    return true;
}

bool AbstractDataManipulator::postProcessing()
{
    // not the first run - data already stored ...
    if ( !m_firstrun )
        return true;
    m_undoData = m_sheet->cellStorage()->stopUndoRecording();
    return true;
}

AbstractDFManipulator::AbstractDFManipulator ()
{
  m_changeformat = true;
}

AbstractDFManipulator::~AbstractDFManipulator ()
{
}

bool AbstractDFManipulator::process (Element* element)
{
  // let parent class process it first
  AbstractDataManipulator::process (element);

  // don't continue if we don't have to change formatting
  if (!m_changeformat) return true;
  if (m_reverse) return true; // undo done by AbstractDataManipulator

  QRect range = element->rect();
  for (int col = range.left(); col <= range.right(); ++col)
  {
    for (int row = range.top(); row <= range.bottom(); ++row) {
      Cell cell( m_sheet, col, row );
//       int colidx = col - range.left();
//       int rowidx = row - range.top();
        Style style = newFormat (element, col, row);
        cell.setStyle( style );
    }
  }
  return true;
}


DataManipulator::DataManipulator()
    : m_format( Format::None )
    , m_parsing( false )
    , m_expandMatrix( false )
{
  // default name for DataManipulator, can be changed using setName
  m_name = i18n ("Change Value");
}

DataManipulator::~DataManipulator ()
{
}

bool DataManipulator::preProcessing()
{
    // extend a singular region to the matrix size, if applicable
    if ( m_firstrun && m_parsing && m_expandMatrix && Region::isSingular() )
    {
        const QString expression = m_data.asString();
        if ( !expression.isEmpty() && expression[0] == '=' )
        {
            Formula formula( m_sheet );
            formula.setExpression( expression );
            if ( formula.isValid() )
            {
                const Value result = formula.eval();
                if ( result.columns() > 1 || result.rows() > 1 )
                {
                    const QPoint point = cells()[0]->rect().topLeft();
                    Region::add( QRect( point.x(), point.y(), result.columns(), result.rows() ), m_sheet );
                }
            }
        }
        else if ( !m_data.isArray() )
        {
            // not a formula; not a matrix: unset m_expandMatrix
            m_expandMatrix = false;
        }
    }
    return AbstractDataManipulator::preProcessing();
}

bool DataManipulator::process( Element* element )
{
    bool success = AbstractDataManipulator::process( element );
    if ( !success )
        return false;
    if ( m_expandMatrix )
    {
        if ( !m_reverse )
            m_sheet->cellStorage()->lockCells( element->rect() );
//         else
//             m_sheet->cellStorage()->unlockCells( element->rect().left(), element->rect().top() );
    }
    return true;
}

Value DataManipulator::newValue( Element *element, int col, int row,
                                 bool *parsing, Format::Type *formatType )
{
    *parsing = m_parsing;
    if (m_format != Format::None)
        *formatType = m_format;
    QRect range = element->rect();
    int colidx = col - range.left();
    int rowidx = row - range.top();
    if ( m_parsing && m_expandMatrix )
    {
        if ( !colidx && !rowidx )
        {
            const QString expression = m_data.asString();
            if ( !expression.isEmpty() && expression[0] == '=' )
            {
                Formula formula( m_sheet, Cell( m_sheet, col, row ) );
                formula.setExpression( expression );
                if ( formula.isValid() )
                {
                    const Value result = formula.eval();
                    if ( result.columns() > 1 || result.rows() > 1 )
                    {
                        *parsing = false;
                        m_sheet->cellStorage()->setFormula( col, row, formula );
                        return result;
                    }
                }
            }
        }
        else
        {
            *parsing = false;
            return Cell( m_sheet, range.topLeft() ).value().element( colidx, rowidx );
        }
    }
    return m_data.element( colidx, rowidx );
}


SeriesManipulator::SeriesManipulator ()
{
  setName (i18n ("Insert Series"));

  m_type = Linear;
  m_last = -2;
}

SeriesManipulator::~SeriesManipulator ()
{
}

void SeriesManipulator::setupSeries (const QPoint &_marker, double start,
    double end, double step, Series mode, Series type)
{
  m_type = type;
  m_start = Value (start);
  m_step = Value (step);
  // compute cell count
  int numberOfCells = 1;
  if (type == Linear)
    numberOfCells = (int) ((end - start) / step + 1);
  if (type == Geometric)
    /* basically, A(n) = start * step ^ n
    * so when is end >= start * step ^ n ??
    * when n = ln(end/start) / ln(step)
    */
    // DBL_EPSILON is added to prevent rounding errors
    numberOfCells = (int) (log (end / start) / log (step) + DBL_EPSILON) + 1;

  // with this, generate range information
  Region range (_marker.x(), _marker.y(), (mode == Column) ? 1 : numberOfCells,
      (mode == Row) ? 1 : numberOfCells);

  // and add the range to the manipulator
  add (range);
}

Value SeriesManipulator::newValue (Element *element, int col, int row,
    bool *parse, Format::Type *)
{
  *parse = false;
  ValueCalc *calc = m_sheet->doc()->calc();

  // either colidx or rowidx is always zero
  QRect range = element->rect();
  int colidx = col - range.left();
  int rowidx = row - range.top();
  int which = (colidx > 0) ? colidx : rowidx;
  Value val;
  if (which == m_last + 1) {
    // if we are requesting next item in the series, which should almost always
    // be the case, we can use the pre-computed value to speed up the process
    if (m_type == Linear)
      val = calc->add (m_prev, m_step);
    if (m_type == Geometric)
      val = calc->mul (m_prev, m_step);
  }
  else {
    // otherwise compute from scratch
    val = m_start;
    for (int i = 0; i < which; ++i)
    {
      if (m_type == Linear)
        val = calc->add (val, m_step);
      if (m_type == Geometric)
        val = calc->mul (val, m_step);
    } 
  }
  // store last value
  m_prev = val;
  m_last = which;

  // return the computed value
  return val;
}


FillManipulator::FillManipulator ()
{
  m_dir = Down;
  m_changeformat = true;
  m_name = i18n ("Fill Selection");
}

FillManipulator::~FillManipulator ()
{
}

Value FillManipulator::newValue (Element *element, int col, int row,
    bool *parse, Format::Type *fmtType)
{
    Q_UNUSED(parse);
    Q_UNUSED(fmtType);
    switch ( m_dir )
    {
        case Up:    row = element->rect().bottom(); break;
        case Down:  row = element->rect().top();    break;
        case Left:  col = element->rect().right();  break;
        case Right: col = element->rect().left();   break;
    };
    return Cell( m_sheet, col, row ).value();
}

Style FillManipulator::newFormat (Element *element, int col, int row)
{
    switch ( m_dir )
    {
        case Up:    row = element->rect().bottom(); break;
        case Down:  row = element->rect().top();    break;
        case Left:  col = element->rect().right();  break;
        case Right: col = element->rect().left();   break;
    };
    return Cell( m_sheet, col, row ).style();
}

CaseManipulator::CaseManipulator ()
{
  m_mode = Upper;
  m_name = i18n ("Change Case");
}

CaseManipulator::~CaseManipulator ()
{
}

Value CaseManipulator::newValue (Element *element, int col, int row,
    bool *parse, Format::Type *)
{
  Q_UNUSED(element)
  // if we are here, we know that we want the change
  *parse = false;
  QString str = Cell( m_sheet, col, row).value().asString();
  switch (m_mode) {
    case Upper: str = str.toUpper();
    break;
    case Lower: str = str.toLower();
    break;
    case FirstUpper:
      if (str.length() > 0)
        str = str.at(0).toUpper() + str.right(str.length()-1);
    break;
  };
  return Value (str);
}

bool CaseManipulator::wantChange (Element *element, int col, int row)
{
  Q_UNUSED(element)
  Cell cell( m_sheet, col, row );
  // don't change cells with a formula
  if (cell.isFormula())
    return false;
  // don't change cells containing other things than strings
  if (!cell.value().isString())
    return false;
  // original version was dismissing text starting with '!' and '*', is this
  // necessary ?
  return true;
}



ShiftManipulator::ShiftManipulator()
    : Manipulator()
    , m_mode( Insert )
{
}

void ShiftManipulator::setReverse( bool reverse )
{
    m_reverse = reverse;
    m_mode = reverse ? Delete : Insert;
}

bool ShiftManipulator::process(Element* element)
{
    // create undo for styles, comments, conditions, validity
    if ( m_firstrun )
        m_sheet->cellStorage()->startUndoRecording();

    const QRect range = element->rect();
    if ( !m_reverse ) // insertion
    {
        if ( m_direction == ShiftBottom )
        {
            m_sheet->insertShiftDown( range );
            m_sheet->cellStorage()->insertShiftDown( range );
        }
        else if ( m_direction == ShiftRight )
        {
            m_sheet->insertShiftRight( range );
            m_sheet->cellStorage()->insertShiftRight( range );
        }

        // undo deletion
        if ( m_mode == Delete )
        {
            m_sheet->cellStorage()->undo( m_undoData );
        }
    }
    else // deletion
    {
        if ( m_direction == ShiftBottom )
        {
            m_sheet->removeShiftUp( range );
            m_sheet->cellStorage()->removeShiftUp( range );
        }
        else if ( m_direction == ShiftRight )
        {
            m_sheet->removeShiftLeft( range );
            m_sheet->cellStorage()->removeShiftLeft( range );
        }

        // undo insertion
        if ( m_mode == Insert )
        {
            m_sheet->cellStorage()->undo( m_undoData );
        }
    }

    if ( m_firstrun )
        m_undoData = m_sheet->cellStorage()->stopUndoRecording();
    return true;
}

bool ShiftManipulator::postProcessing()
{
    m_sheet->recalc();
    return true;
}

QString ShiftManipulator::name() const
{
    if ( !m_reverse )
        return i18n( "Insert Cells" );
    else
        return i18n( "Remove Cells" );
}
