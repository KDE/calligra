/*
 *  XML storage for CSV imported files
 *
 *  Copyright (C) 1999 David Faure
 *
 */

#include <qtextstream.h>
#include <xmltree.h>
#include <xmltree.moc>

XMLTree::XMLTree(const QString & inputFileName)
  : QObject()
{
  root = new QDomDocument("XML");

  QDomProcessingInstruction pro;
  pro = root->createProcessingInstruction("xml", "version=\"1.0\"");
  root->appendChild(pro);

  doc = root->createElement("DOC"); 
  doc.setAttribute("editor", "KSpread CSV Filter");
  doc.setAttribute("mime", "application/x-kspread");
  root->appendChild(doc);

  QDomElement paper = root->createElement("PAPER");
  paper.setAttribute("format", "A4");
  paper.setAttribute("orientation", "Portrait");
  doc.appendChild(paper);

  map = root->createElement("MAP");
  doc.appendChild(map);

  table = root->createElement("TABLE");
  table.setAttribute("name", inputFileName);
  map.appendChild(table);

  row = 1;
  column = 1;
}

XMLTree::~XMLTree() 
{
  if(root)
    delete root;
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

  e.setAttribute("row", row);
  e.setAttribute("column", column++);
  e.appendChild(root->createTextNode(contents));

  table.appendChild(e);

  return true;
}

