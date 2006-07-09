/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/


#include "Damages.h"

#include "Cell.h"
#include "Sheet.h"

namespace KSpread
{

class SheetDamage::Private
{
public:
  KSpread::Sheet* sheet;
  int action;
};

class CellDamage::Private
{
public:
  KSpread::Cell* cell;
};

}

using namespace KSpread;


CellDamage::CellDamage( KSpread::Cell* cell )
{
  d = new Private;
  d->cell = cell;
}

CellDamage::~CellDamage()
{
  delete d;
}

KSpread::Cell* CellDamage::cell()
{
  return d->cell;
}

SheetDamage::SheetDamage( KSpread::Sheet* sheet, int action )
{
  d = new Private;
  d->sheet = sheet;
  d->action = action;
}

SheetDamage::~SheetDamage()
{
  delete d;
}

KSpread::Sheet* SheetDamage::sheet() const
{
  return d->sheet;
}

int SheetDamage::action() const
{
  return d->action;
}
