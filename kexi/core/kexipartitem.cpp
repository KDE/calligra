/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2005-2007 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexipartitem.h"

namespace KexiPart {
class Item::Private
{
  public:
    Private()
    {
    }
    int dummy;
  // unused for now
};
}

//-----------------------

using namespace KexiPart;

Item::Item()
 : m_id(0) //- null
 , m_neverSaved(false)
 , d( new Private() )
{
}

Item::~Item()
{
  delete d;
}

//-----------------------

ItemDict::ItemDict()
: QHash<int, KexiPart::Item*>()
{
}

ItemDict::~ItemDict()
{
}

//-----------------------

ItemList::ItemList()
 : QList<KexiPart::Item*>()
{
}

bool lessThan( KexiPart::Item* item1, KexiPart::Item* item2 )
{
  return item1->name() < item2->name();
}

void ItemList::sort()
{
  qSort(begin(), end(), lessThan);
}
