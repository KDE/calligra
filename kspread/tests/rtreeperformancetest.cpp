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

#include <QList>
#include <QTime>
#include <QVector>

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kdebug.h>

// #include "rtree.h"
#include "RTree.h"

using namespace std;
using namespace KSpread;

int main( int argc, char** argv )
{
  // Initialize command line args
  KCmdLineArgs::init(argc, argv, "rtreetest", "rtreetest", "rtreee test", "0.1");
  KApplication app(false);

  RTree<double> tree;

  cout << "Insertion performance test..." << endl;

  QTime time;
  time.start();
  const int max_x = 100;
  const int step_x = 1;
  const int max_y = 1000;
  const int step_y = 1;
  int counter = 0;
  for ( int y = 1; y <= max_y; y += step_y ) // equals row insertion into table
  {
    for ( int x = 1; x <= max_x; x += step_x ) // equals cell insertion into row
    {
      tree.insert( QRect(x,y,step_x,step_y), 42 );
      ++counter;
    }
  }
  cout << "\t Inserted " << counter << " rectangles in " << time.elapsed() << " ms" << endl;

  cout << "Row insertion performance test..." << endl;
  time.restart();
  tree.insertRows(1,5);
  cout << "\t Inserted 5 rows at row 1 in " << time.elapsed() << " ms" << endl;

  cout << "Column insertion performance test..." << endl;
  time.restart();
  tree.insertColumns(1,5);
  cout << "\t Inserted 5 columns at column 1 in " << time.elapsed() << " ms" << endl;

  cout << "Row deletion performance test..." << endl;
  time.restart();
  tree.deleteRows(1,5);
  cout << "\t Deleted 5 rows at row 1 in " << time.elapsed() << " ms" << endl;

  cout << "Column deletion performance test..." << endl;
  time.restart();
  tree.deleteColumns(1,5);
  cout << "\t Deleted 5 columns at column 1 in " << time.elapsed() << " ms" << endl;

  cout << "Lookup performance test..." << endl;
  time.restart();
  counter = 0;
  for ( int y = 1; y <= max_y; y += step_y )
  {
    for ( int x = 1; x <= max_x; x += step_x )
    {
      if ( !tree.contains(QPoint(x,y)).isEmpty() ) counter++;
    }
  }
  cout << "\t Found " << counter << " positions in " << time.elapsed() << " ms" << endl;
}
