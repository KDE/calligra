/*
 *  XML storage for CSV imported files
 *
 *  Copyright (C) 1999 David Faure
 *
 */

#include <qtextstream.h>
#include <xmltree.h>
#include <xmltree.moc>

XMLTree::XMLTree()
  : QObject()
{
  QDomElement e;

  root = new QDomDocument("XML");

  QDomProcessingInstruction pro;
  pro = root->createProcessingInstruction("xml", "version=\"1.0\"");
  root->appendChild(pro);

  doc = root->createElement("DOC"); 
  //doc.setAttribute("author", "OLEFilter");
  doc.setAttribute("email", "unknown");
  doc.setAttribute("editor", "KSpread");
  doc.setAttribute("mime", "application/x-kspread");
  root->appendChild(doc);

  paper = root->createElement("PAPER");
  paper.setAttribute("format", "A4");
  paper.setAttribute("orientation", "Portrait");
  doc.appendChild(paper);

  e = root->createElement("PAPERBORDERS");
  //e.setAttribute("left", 20);
  //e.setAttribute("top", 20);
  //e.setAttribute("right", 20);
  //e.setAttribute("bottom", 20);
  paper.appendChild(e);

  map = root->createElement("MAP");
  doc.appendChild(map);

  row = 0;
  column = 0;
}

XMLTree::~XMLTree() 
{
  if(root) {
    delete root;
    root=0L;
  }
}

const QString XMLTree::part()
{
  QString s;
  QTextStream t(s, IO_WriteOnly);

  t << "<?xml version=\"1.0\"?>\n";
  doc.save(t);
  t << '\0';

  return s;
}

bool XMLTree::cell( const QString & contents )
{
  QDomElement e = root->createElement("CELL");
  //e.appendChild(getFormat(xf));
  //e.appendChild(getFont(xf));

  e.setAttribute("row", ++row);
  e.setAttribute("column", ++column);
  e.appendChild(root->createTextNode(contents));

  doc.appendChild(e);

  return true;
}


void XMLTree::newline()
{
  row ++;
  column = 0;
}

