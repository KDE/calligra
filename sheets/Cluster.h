/* This file is part of the KDE project
   Copyright (C) 2000 Torben Weis <weis@kde.org>

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

#ifndef CLUSTER_H
#define CLUSTER_H

#include "Value.h"

#define CALLIGRA_SHEETS_CLUSTER_LEVEL1 256
#define CALLIGRA_SHEETS_CLUSTER_LEVEL2 256
#define CALLIGRA_SHEETS_CLUSTER_MAX (256*256)

class QPoint;

namespace Calligra
{
namespace Sheets
{
class Cell;
class ColumnFormat;
class RowFormat;

/**
 * \ingroup Storage
 * A pointer map to all column formats. Should probably be converted to something like RowFormatStorage.
 * \see Cluster
 */
class ColumnCluster
{
public:
    ColumnCluster();
    ~ColumnCluster();

    const ColumnFormat* lookup(int col) const;
    ColumnFormat* lookup(int col);

    void clear();

    void insertElement(ColumnFormat*, int col);
    void removeElement(int col);

    bool insertColumn(int col);
    bool removeColumn(int col);

    void setAutoDelete(bool);
    bool autoDelete() const;

    ColumnFormat* first() const {
        return m_first;
    }
    ColumnFormat* next(int col) const;

    void operator=(const ColumnCluster& other);

private:
    ColumnCluster(const ColumnCluster& other);

private:
    ColumnFormat*** m_cluster;
    ColumnFormat* m_first;
    bool m_autoDelete;
};

} // namespace Sheets
} // namespace Calligra

#endif
