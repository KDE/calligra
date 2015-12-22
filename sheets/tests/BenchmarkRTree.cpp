/* This file is part of the KDE project
   Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA  02110-1301  USA
*/
#include "BenchmarkRTree.h"

// #include "rtree.h"
#include "RTree.h"

#include <QTest>

using namespace std;
using namespace Calligra::Sheets;

void RTreeBenchmark::init()
{
    RTree<double> tree;
    m_tree = tree;
    // insert some data in the RTree
    const int max_x = 100;
    const int step_x = 1;
    const int max_y = 1000;
    const int step_y = 1;
    for (int y = 1; y <= max_y; y += step_y) { // equals row insertion into table
        for (int x = 1; x <= max_x; x += step_x) { // equals cell insertion into row
            m_tree.insert(QRect(x, y, step_x, step_y), 42);
        }
    }
}

void RTreeBenchmark::cleanup()
{
}

void RTreeBenchmark::testInsertionPerformance()
{
    // reset tree for this test
    RTree<double> tree;
    m_tree = tree;

    const int max_x = 100;
    const int step_x = 1;
    const int max_y = 1000;
    const int step_y = 1;

    QBENCHMARK {
        for (int y = 1; y <= max_y; y += step_y) { // equals row insertion into table
            for (int x = 1; x <= max_x; x += step_x) { // equals cell insertion into row
                m_tree.insert(QRect(x, y, step_x, step_y), 42);
            }
        }
    }
}

void RTreeBenchmark::testRowInsertionPerformance()
{
    QBENCHMARK {
        m_tree.insertRows(1, 5);
    }
}

void RTreeBenchmark::testColumnInsertionPerformance()
{
    QBENCHMARK {
        m_tree.insertColumns(1, 5);
    }
}

void RTreeBenchmark::testRowDeletionPerformance()
{
    QBENCHMARK {
        m_tree.removeRows(1, 5);
    }
}

void RTreeBenchmark::testColumnDeletionPerformance()
{
    QBENCHMARK {
        m_tree.removeColumns(1, 5);
    }
}

void RTreeBenchmark::testLookupPerformance()
{
    int counter = 0;
    const int max_x = 100;
    const int step_x = 1;
    const int max_y = 1000;
    const int step_y = 1;
    QBENCHMARK {
        for (int y = 1; y <= max_y; y += step_y) {
            for (int x = 1; x <= max_x; x += step_x) {
                if (!m_tree.contains(QPoint(x, y)).isEmpty()) counter++;
            }
        }
    }
}

QTEST_MAIN(RTreeBenchmark)
