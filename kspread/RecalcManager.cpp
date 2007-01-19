/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             2004 Tomas Mecir <mecirt@gmail.com>

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

#include <QHash>
#include <QMap>

#include "Cell.h"
#include "DependencyManager.h"
#include "Formula.h"
#include "Map.h"
#include "Sheet.h"
#include "Region.h"
#include "Value.h"
#include "ValueFormatter.h"

#include "RecalcManager.h"

using namespace KSpread;

class RecalcManager::Private
{
public:
  /**
   * Stores cells with its reference depth.
   * Depth means the maximum depth of all cells this cell depends on plus one,
   * while a cell which has a formula without cell references has a depth
   * of zero.
   *
   * Examples:
   * \li A1: '=1.0'
   * \li A2: '=A1+A1'
   * \li A3: '=A1+A1+A2'
   *
   * \li depth(A1) = 0
   * \li depth(A2) = 1
   * \li depth(A3) = 2
   */
  QMap<int, Cell*> depths;
  const Map* map;
  bool busy;
};

RecalcManager::RecalcManager( const Map* map )
  : d(new Private)
{
  d->map  = map;
  d->busy = false;
}

RecalcManager::~RecalcManager()
{
  delete d;
}

void RecalcManager::regionChanged(const Region& region)
{
    if (d->busy || region.isEmpty())
        return;
    d->busy = true;
    kDebug(36002) << "RecalcManager::regionChanged " << region.name() << endl;
    ElapsedTime et( "Overall region recalculation", ElapsedTime::PrintOnlyTime );
    d->depths = d->map->dependencyManager()->cellsToCalculate( region );
    recalc();
    d->busy = false;
}

void RecalcManager::recalcSheet(Sheet* const sheet)
{
    if (d->busy)
        return;
    d->busy = true;
    ElapsedTime et( "Overall sheet recalculation", ElapsedTime::PrintOnlyTime );
    d->depths = d->map->dependencyManager()->cellsToCalculate( sheet );
    recalc();
    d->busy = false;
}

void RecalcManager::recalcMap()
{
    if (d->busy)
        return;
    d->busy = true;
    ElapsedTime et( "Overall map recalculation", ElapsedTime::PrintOnlyTime );
    d->depths = d->map->dependencyManager()->cellsToCalculate();
    recalc();
    d->busy = false;
}

void RecalcManager::recalc()
{
    kDebug(36002) << "Recalculating " << d->depths.count() << " cell(s).." << endl;
    ElapsedTime et( "Recalculating cells", ElapsedTime::PrintOnlyTime );
    foreach (Cell* cell, d->depths)
    {
        if ( !cell->isFormula() )
            continue;
        // only recalculate, if no circular dependency occurred
        if ( cell->value() == Value::errorCIRCLE() )
            continue;
        // Formula object not yet created?
        if ( cell->formula() == 0 )
        {
            // because of a parse error?
            if ( cell->value() == Value::errorPARSE() )
                continue;
            // We were probably at a "isLoading() = true" state,
            // when we originally parsed. Try again.
            if ( !cell->makeFormula() )
                continue; // there was a parse error
        }

        // evaluate the formula and set the result
        Value result = cell->formula()->eval();
        cell->setValue( result, false );
    }
//     dump();
    d->depths.clear();
}

void RecalcManager::dump() const
{
    QMap<int, Cell*>::ConstIterator end(d->depths.constEnd());
    for ( QMap<int, Cell*>::ConstIterator it(d->depths.constBegin()); it != end; ++it )
    {
        Cell* cell = it.value();
        QString cellName = cell->name();
        while ( cellName.count() < 4 ) cellName.prepend( ' ' );
        kDebug(36002) << "depth( " << cellName << " ) = " << it.key() << endl;
    }
}
