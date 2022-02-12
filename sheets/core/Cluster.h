/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CLUSTER_H
#define CLUSTER_H

#include "sheets_core_export.h"

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
class CALLIGRA_SHEETS_CORE_EXPORT ColumnCluster
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
