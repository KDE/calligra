/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             2004 Ariya Hidayat <ariya@kde.org>

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
  Changes changes;
};

class CellDamage::Private
{
public:
  KSpread::Cell* cell;
  Changes changes;
};

}

using namespace KSpread;


CellDamage::CellDamage( KSpread::Cell* cell, Changes changes )
  : d( new Private )
{
  d->cell = cell;
  d->changes = changes;
}

CellDamage::~CellDamage()
{
  delete d;
}

KSpread::Cell* CellDamage::cell() const
{
  return d->cell;
}

CellDamage::Changes CellDamage::changes() const
{
  return d->changes;
}


SheetDamage::SheetDamage( KSpread::Sheet* sheet, Changes changes )
  : d( new Private )
{
  d->sheet = sheet;
  d->changes = changes;
}

SheetDamage::~SheetDamage()
{
  delete d;
}

KSpread::Sheet* SheetDamage::sheet() const
{
  return d->sheet;
}

SheetDamage::Changes SheetDamage::changes() const
{
  return d->changes;
}
