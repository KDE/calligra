/***************************************************************************
 * list.cpp
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#include "list.h"
#include "exception.h"

#include <klocale.h>
#include <kdebug.h>

using namespace Kross::Api;

List::List(const QValueList<Object*>& value, const QString& name)
    : Value< List, QValueList<Object*> >(value, name)
{
}

List::~List()
{
}

const QString List::getClassName() const
{
    return "Kross::Api::List";
}

const QString List::getDescription() const
{
    return i18n("Object to handle QValueList collections.");
}

Object* List::item(uint idx)
{
    QValueList<Object*> list = getValue();
    if(idx >= list.count()) {
        kdDebug() << "List::item index=" << idx << " is out of bounds. Raising TypeException." << endl;
        throw TypeException(i18n("List-index %1 out of bounds.").arg(idx));
    }
    Object* obj = list[idx];
    return obj;
}

uint List::count()
{
    QValueList<Object*> list = getValue();
    return list.count();
}
