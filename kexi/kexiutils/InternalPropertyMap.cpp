/* This file is part of the KDE project
   Copyright (C) 2007 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "InternalPropertyMap.h"
#include <QHash>

using namespace KexiUtils;

class InternalPropertyMap::Private
{
public:
    Private() {}

    QHash<QByteArray, QVariant> map;
};

//---------------------------------

InternalPropertyMap::InternalPropertyMap()
        : d(new Private)
{
}

InternalPropertyMap::~InternalPropertyMap()
{
    delete d;
}

QVariant InternalPropertyMap::internalPropertyValue(
    const QByteArray& name,
    const QVariant& defaultValue) const
{
    const QVariant result(d->map.value(name.toLower()));
    return result.isNull() ? defaultValue : result;
}

void InternalPropertyMap::setInternalPropertyValue(
    const QByteArray& name, const QVariant& value)
{
    if (value.isNull())
        d->map.remove(name.toLower());
    else
        d->map.insert(name.toLower(), value);
}
