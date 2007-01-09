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
#include "Sheet.h"
#include "Region.h"
#include "Util.h"
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
  DependencyManager* depManager;
  bool busy;
};

RecalcManager::RecalcManager(DependencyManager* depManager)
  : d(new Private)
{
  d->depManager = depManager;
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
    d->depths = d->depManager->cellsToCalculate( region );
    recalc();
    d->busy = false;
}

void RecalcManager::recalcSheet(Sheet* const sheet)
{
    if (d->busy)
        return;
    d->busy = true;
    ElapsedTime et( "Overall sheet recalculation", ElapsedTime::PrintOnlyTime );
    d->depths = d->depManager->cellsToCalculate( sheet );
    recalc();
    d->busy = false;
}

void RecalcManager::recalcMap()
{
    if (d->busy)
        return;
    d->busy = true;
    ElapsedTime et( "Overall map recalculation", ElapsedTime::PrintOnlyTime );
    d->depths = d->depManager->cellsToCalculate();
    recalc();
    d->busy = false;
}

void RecalcManager::recalc()
{
    ElapsedTime et( "Recalculating cells", ElapsedTime::PrintOnlyTime );
    foreach (Cell* cell, d->depths)
    {
        // only recalculate, if no circular dependency occurred
        if ( cell->value() != Value::errorCIRCLE() )
            cell->calc( false );
    }
    kDebug(36002) << "Recalculating " << d->depths.count() << " cell(s).." << endl;
    dump();
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
