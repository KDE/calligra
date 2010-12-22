/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus stefan.nikolaus@kdemail.net

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_VALUE_STORAGE
#define KSPREAD_VALUE_STORAGE

namespace KSpread
{

/**
 * \class ValueStorage
 * \ingroup Storage
 * \ingroup Value
 * Stores cell values.
 */
class ValueStorage : public PointStorage<Value>
{
public:
    ValueStorage()
            : PointStorage<Value>() {
    }

    ValueStorage(const PointStorage<Value>& o)
            : PointStorage<Value>(o) {
    }

    ValueStorage& operator=(const PointStorage<Value>& o) {
        PointStorage<Value>::operator=(o);
        return *this;
    }
};

} // namespace KSpread

#endif // KSPREAD_VALUE_STORAGE
