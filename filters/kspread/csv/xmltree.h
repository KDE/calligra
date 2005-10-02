/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <faure@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef XMLTREE_H
#define XMLTREE_H

#include <qdom.h>
#include <qmemarray.h>
#include <qtextstream.h>

class XMLTree
{

public:
  // Create a tree.
  // inputFileName is only used in order to name the sheet
  XMLTree(QDomDocument &qdoc);
  ~XMLTree();

  // Return the whole XML as QString
  //const QString part();

  // Create a new cell
  bool cell( const QString & contents );

  // Tell about an empty cell (doesn't create it, just skips)
  void emptycell() { column ++; } // easy heh ? ;)

  // Go to new line
  void newline() { row ++; column = 1; }

private:
  int row, column;
  QDomDocument &root;
  QDomElement doc, map, sheet;
};
#endif // XMLTREE_H
