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

#include <iostream>

#include <QCoreApplication>

#include <kdebug.h>

#include "BenchmarkHelper.h"

// #include "rtree.h"
#include "RTree.h"

using namespace std;
using namespace KSpread;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    RTree<double> tree;

    cout << "Insertion performance test..." << endl;

    const int max_x = 100;
    const int step_x = 1;
    const int max_y = 1000;
    const int step_y = 1;
    int counter = 0;
    Time::tval start = Time::stamp();
    for (int y = 1; y <= max_y; y += step_y) { // equals row insertion into table
        for (int x = 1; x <= max_x; x += step_x) { // equals cell insertion into row
            tree.insert(QRect(x, y, step_x, step_y), 42);
            ++counter;
        }
    }
    cout << "\t " << qPrintable(Time::printAverage(Time::elapsed(start), counter)) << endl;

    cout << "Row insertion performance test (5 rows at row 1)..." << endl;
    start = Time::stamp();
    tree.insertRows(1, 5);
    cout << "\t " << qPrintable(Time::printAverage(Time::elapsed(start), 1)) << endl;

    cout << "Column insertion performance test (5 columns at column 1)..." << endl;
    start = Time::stamp();
    tree.insertColumns(1, 5);
    cout << "\t " << qPrintable(Time::printAverage(Time::elapsed(start), 1)) << endl;

    cout << "Row deletion performance test (5 rows at row 1)..." << endl;
    start = Time::stamp();
    tree.removeRows(1, 5);
    cout << "\t " << qPrintable(Time::printAverage(Time::elapsed(start), 1)) << endl;

    cout << "Column deletion performance test (5 columns at column 1)..." << endl;
    start = Time::stamp();
    tree.removeColumns(1, 5);
    cout << "\t " << qPrintable(Time::printAverage(Time::elapsed(start), 1)) << endl;

    cout << "Lookup performance test..." << endl;
    start = Time::stamp();
    counter = 0;
    for (int y = 1; y <= max_y; y += step_y) {
        for (int x = 1; x <= max_x; x += step_x) {
            if (!tree.contains(QPoint(x, y)).isEmpty()) counter++;
        }
    }
    cout << "\t " << qPrintable(Time::printAverage(Time::elapsed(start), counter)) << endl;
    return 0;
}
