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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include "damages.h"


#include "kspread_sheet.h"

namespace KSpread
{

class SheetDamage::Private
{
public:
  KSpreadSheet* sheet;
  int action;
};

}


using namespace KSpread;

SheetDamage::SheetDamage( KSpreadSheet* sheet, int action )
{
  d = new Private;
  d->sheet = sheet;
  d->action = action;
}

SheetDamage::~SheetDamage()
{
  delete d;
}

KSpreadSheet* SheetDamage::sheet() const
{
  return d->sheet;
}

int SheetDamage::action() const
{
  return d->action;
}
