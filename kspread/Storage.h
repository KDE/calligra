/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_STORAGE
#define KSPREAD_STORAGE

#include <koffice_export.h>

#include "Region.h"
#include "RTree.h"

namespace KSpread
{

class Sheet;

/**
 * A custom spatial storage based on an R-Tree.
 */
template<typename T>
class KSPREAD_EXPORT Storage
{
public:
    Storage( Sheet* sheet );
    ~Storage() {};

    /**
     * \return the stored value at the position \p point .
     */
    T at(const QPoint& point) const;

    QList< QPair<QRectF, T> > undoData(const QRect& rect) const;

    /**
     * Assigns \p data to \p region .
     */
    void insert(const Region& region, const T& data);

protected:

private:
    Sheet*      m_sheet;
    RTree<T>    m_tree;
};

template<typename T>
Storage<T>::Storage( Sheet* sheet )
    : m_sheet( sheet )
{
}

template<typename T>
T Storage<T>::at(const QPoint& point) const
{
    QList<T> results = m_tree.contains(point);
    return results.isEmpty() ? T() : results.last();
}

template<typename T>
QList< QPair<QRectF,T> > Storage<T>::undoData(const QRect& rect) const
{
    QList< QPair<QRectF,T> > result = m_tree.intersectingPairs(rect);
    for ( int i = 0; i < result.count(); ++i )
    {
        // trim the rects
        result[i].first = result[i].first.intersected( rect );
    }
    return result;
}

template<typename T>
void Storage<T>::insert(const Region& region, const T& data)
{
    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it)
    {
        m_tree.insert((*it)->rect(), data);
    }
}

} // namespace KSpread

#endif // KSPREAD_STORAGE
