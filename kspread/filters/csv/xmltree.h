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
#include <qobject.h>

class XMLTree : public QObject 
{

  Q_OBJECT

public:
  XMLTree();
  ~XMLTree();

  // Return the whole XML
  const QString part();

  // Create a new cell
  bool cell( const QString & contents ); 
  // Go to new line
  void newline();
  
private:

  int row, column;
  QDomDocument *root;
  QDomElement doc, paper, map;
};

#endif // XMLTREE_H
