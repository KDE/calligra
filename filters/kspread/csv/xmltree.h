/*
 *  XML storage for CSV imported files
 *
 *  Copyright (C) 1999 David Faure
 *
 */

#ifndef XMLTREE_H
#define XMLTREE_H

#include <qdom.h>
#include <qarray.h>
#include <qtextstream.h>

class XMLTree
{

public:
  // Create a tree.
  // inputFileName is only used in order to name the table
  XMLTree(const QString & inputFileName);
  ~XMLTree();

  // Return the whole XML
  const QString part();


  // Create a new cell
  bool cell( const QString & contents ); 

  // Tell about an empty cell (doesn't create it, just skips)
  void emptycell() { column ++; } // easy heh ? ;)

  // Go to new line
  void newline() { row ++; column = 1; }
  
private:
  int row, column;
  QDomDocument *root;
  QDomElement doc, map, table;
};

#endif // XMLTREE_H
