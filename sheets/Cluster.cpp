/* This file is part of the KDE project
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>
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

// Local
#include "Cluster.h"

#include <stdlib.h>

#include "SheetsDebug.h"
#include "Cell.h"
#include "RowColumnFormat.h"

using namespace Calligra::Sheets;

/****************************************************
 *
 * ColumnCluster
 *
 ****************************************************/

ColumnCluster::ColumnCluster()
        : m_first(0), m_autoDelete(false)
{
    m_cluster = (ColumnFormat***)malloc(CALLIGRA_SHEETS_CLUSTER_LEVEL1 * sizeof(ColumnFormat**));

    for (int x = 0; x < CALLIGRA_SHEETS_CLUSTER_LEVEL1; ++x)
        m_cluster[ x ] = 0;
}

ColumnCluster::~ColumnCluster()
{
    for (int x = 0; x < CALLIGRA_SHEETS_CLUSTER_LEVEL1; ++x) {
        ColumnFormat** cl = m_cluster[ x ];
        if (cl) {
            free(cl);
            m_cluster[ x ] = 0;
        }
    }

    if (m_autoDelete) {
        ColumnFormat* cell = m_first;
        while (cell) {
            ColumnFormat* n = cell->next();
            delete cell;
            cell = n;
        }
    }


    free(m_cluster);
}

ColumnFormat* ColumnCluster::lookup(int col)
{
    if (col >= CALLIGRA_SHEETS_CLUSTER_MAX || col < 0) {
        debugSheets << "ColumnCluster::lookup: invalid column value (col:"
        << col << ")" << endl;
        return 0;
    }

    int cx = col / CALLIGRA_SHEETS_CLUSTER_LEVEL2;
    int dx = col % CALLIGRA_SHEETS_CLUSTER_LEVEL2;

    ColumnFormat** cl = m_cluster[ cx ];
    if (!cl)
        return 0;

    return cl[ dx ];
}

const ColumnFormat* ColumnCluster::lookup(int col) const
{
    if (col >= CALLIGRA_SHEETS_CLUSTER_MAX || col < 0) {
        debugSheets << "ColumnCluster::lookup: invalid column value (col:"
        << col << ")" << endl;
        return 0;
    }

    int cx = col / CALLIGRA_SHEETS_CLUSTER_LEVEL2;
    int dx = col % CALLIGRA_SHEETS_CLUSTER_LEVEL2;

    ColumnFormat** cl = m_cluster[ cx ];
    if (!cl)
        return 0;

    return cl[ dx ];
}

void ColumnCluster::clear()
{
    for (int x = 0; x < CALLIGRA_SHEETS_CLUSTER_LEVEL1; ++x) {
        ColumnFormat** cl = m_cluster[ x ];
        if (cl) {
            free(cl);
            m_cluster[ x ] = 0;
        }
    }

    if (m_autoDelete) {
        ColumnFormat* cell = m_first;
        while (cell) {
            ColumnFormat* n = cell->next();
            delete cell;
            cell = n;
        }
    }

    m_first = 0;
}

void ColumnCluster::insertElement(ColumnFormat* lay, int col)
{
    if (col >= CALLIGRA_SHEETS_CLUSTER_MAX || col < 0) {
        debugSheets << "ColumnCluster::insertElement: invalid column value (col:"
        << col << ")" << endl;
        return;
    }

    int cx = col / CALLIGRA_SHEETS_CLUSTER_LEVEL2;
    int dx = col % CALLIGRA_SHEETS_CLUSTER_LEVEL2;

    ColumnFormat** cl = m_cluster[ cx ];
    if (!cl) {
        cl = (ColumnFormat**)malloc(CALLIGRA_SHEETS_CLUSTER_LEVEL2 * sizeof(ColumnFormat*));
        m_cluster[ cx ] = cl;

        for (int a = 0; a < CALLIGRA_SHEETS_CLUSTER_LEVEL2; ++a)
            cl[ a ] = 0;
    }

    if (cl[ dx ])
        removeElement(col);

    cl[ dx ] = lay;

    if (m_first) {
        lay->setNext(m_first);
        m_first->setPrevious(lay);
    }
    m_first = lay;
}

void ColumnCluster::removeElement(int col)
{
    if (col >= CALLIGRA_SHEETS_CLUSTER_MAX || col < 0) {
        debugSheets << "ColumnCluster::removeElement: invalid column value (col:"
        << col << ")" << endl;
        return;
    }

    int cx = col / CALLIGRA_SHEETS_CLUSTER_LEVEL2;
    int dx = col % CALLIGRA_SHEETS_CLUSTER_LEVEL2;

    ColumnFormat** cl = m_cluster[ cx ];
    if (!cl)
        return;

    ColumnFormat* c = cl[ dx ];
    if (!c)
        return;

    cl[ dx ] = 0;

    if (m_autoDelete) {
        if (m_first == c)
            m_first = c->next();
        delete c;
    } else {
        if (m_first == c)
            m_first = c->next();
        if (c->previous())
            c->previous()->setNext(c->next());
        if (c->next())
            c->next()->setPrevious(c->previous());
        c->setNext(0);
        c->setPrevious(0);
    }
}

bool ColumnCluster::insertColumn(int col)
{
    if (col >= CALLIGRA_SHEETS_CLUSTER_MAX || col < 0) {
        debugSheets << "ColumnCluster::insertColumn: invalid column value (col:"
        << col << ")" << endl;
        return false;
    }

    int cx = col / CALLIGRA_SHEETS_CLUSTER_LEVEL2;
    int dx = col % CALLIGRA_SHEETS_CLUSTER_LEVEL2;

    // Is there a column layout at the right most position ?
    // In this case the shift is impossible.
    ColumnFormat** cl = m_cluster[ CALLIGRA_SHEETS_CLUSTER_LEVEL1 - 1 ];
    if (cl && cl[ CALLIGRA_SHEETS_CLUSTER_LEVEL2 - 1 ])
        return false;

    bool a = autoDelete();
    setAutoDelete(false);

    for (int i = CALLIGRA_SHEETS_CLUSTER_LEVEL1 - 1; i >= cx ; --i) {
        ColumnFormat** cl = m_cluster[ i ];
        if (cl) {
            int left = 0;
            if (i == cx)
                left = dx;
            int right = CALLIGRA_SHEETS_CLUSTER_LEVEL2 - 1;
            if (i == CALLIGRA_SHEETS_CLUSTER_LEVEL1 - 1)
                right = CALLIGRA_SHEETS_CLUSTER_LEVEL2 - 2;
            for (int k = right; k >= left; --k) {
                ColumnFormat* c = cl[ k ];
                if (c) {
                    removeElement(c->column());
                    c->setColumn(c->column() + 1);
                    insertElement(c, c->column());
                }
            }
        }
    }

    setAutoDelete(a);

    return true;
}

bool ColumnCluster::removeColumn(int column)
{
    if (column >= CALLIGRA_SHEETS_CLUSTER_MAX || column < 0) {
        debugSheets << "ColumnCluster::removeColumn: invalid column value (col:"
        << column << ")" << endl;
        return false;
    }

    int cx = column / CALLIGRA_SHEETS_CLUSTER_LEVEL2;
    int dx = column % CALLIGRA_SHEETS_CLUSTER_LEVEL2;

    removeElement(column);

    bool a = autoDelete();
    setAutoDelete(false);

    for (int i = cx; i < CALLIGRA_SHEETS_CLUSTER_LEVEL1; ++i) {
        ColumnFormat** cl = m_cluster[ i ];
        if (cl) {
            int left = 0;
            if (i == cx)
                left = dx + 1;
            int right = CALLIGRA_SHEETS_CLUSTER_LEVEL2 - 1;
            for (int k = left; k <= right; ++k) {
                ColumnFormat* c = cl[ k ];
                if (c) {
                    removeElement(c->column());
                    c->setColumn(c->column() - 1);
                    insertElement(c, c->column());
                }
            }
        }
    }

    setAutoDelete(a);

    return true;
}

void ColumnCluster::setAutoDelete(bool a)
{
    m_autoDelete = a;
}

bool ColumnCluster::autoDelete() const
{
    return m_autoDelete;
}

ColumnFormat* ColumnCluster::next(int col) const
{
    if (col >= CALLIGRA_SHEETS_CLUSTER_MAX || col < 0) {
        debugSheets << "ColumnCluster::next: invalid column value (col:"
        << col << ")" << endl;
        return 0;
    }

    int cx = (col + 1) / CALLIGRA_SHEETS_CLUSTER_LEVEL2;
    int dx = (col + 1) % CALLIGRA_SHEETS_CLUSTER_LEVEL2;

    while (cx < CALLIGRA_SHEETS_CLUSTER_LEVEL1) {
        if (m_cluster[ cx ]) {
            while (dx < CALLIGRA_SHEETS_CLUSTER_LEVEL2) {

                if (m_cluster[ cx ][  dx ]) {
                    return m_cluster[ cx ][ dx ];
                }
                ++dx;
            }
        }
        ++cx;
        dx = 0;
    }
    return 0;
}

void ColumnCluster::operator=(const ColumnCluster & other)
{
    m_first = 0;
    m_autoDelete = other.m_autoDelete;
    // TODO Stefan: Optimize!
    m_cluster = (ColumnFormat***)malloc(CALLIGRA_SHEETS_CLUSTER_LEVEL1 * sizeof(ColumnFormat**));
    for (int i = 0; i < CALLIGRA_SHEETS_CLUSTER_LEVEL1; ++i) {
        if (other.m_cluster[i]) {
            m_cluster[i] = (ColumnFormat**)malloc(CALLIGRA_SHEETS_CLUSTER_LEVEL2 * sizeof(ColumnFormat*));
            for (int j = 0; j < CALLIGRA_SHEETS_CLUSTER_LEVEL2; ++j) {
                m_cluster[i][j] = 0;
                if (other.m_cluster[i][j]) {
                    ColumnFormat* columnFormat = new ColumnFormat(*other.m_cluster[i][j]);
                    columnFormat->setNext(0);
                    columnFormat->setPrevious(0);
                    insertElement(columnFormat, columnFormat->column());
                }
            }
        } else
            m_cluster[i] = 0;
    }
}


