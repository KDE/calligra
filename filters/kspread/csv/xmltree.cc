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

#include <xmltree.h>

#include <QDateTime>
#include <kdebug.h>


XMLTree::XMLTree(QDomDocument &qdoc) : root(qdoc)
{
  root=QDomDocument("spreadsheet");
  root.appendChild( root.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
  doc = root.createElement( "spreadsheet" );

  doc.setAttribute("editor", "KSpread CSV Filter");
  doc.setAttribute("mime", "application/x-kspread");
  root.appendChild(doc);

  QDomElement paper = root.createElement("paper");
  paper.setAttribute("format", "A4");
  paper.setAttribute("orientation", "Portrait");
  QDomElement borders = root.createElement( "borders" );
  borders.setAttribute( "left", 20 );
  borders.setAttribute( "top", 20 );
  borders.setAttribute( "right", 20 );
  borders.setAttribute( "bottom", 20 );
  paper.appendChild( borders );
  doc.appendChild(paper);

  map = root.createElement("map");
  doc.appendChild(map);

  sheet = root.createElement("table");

  sheet.setAttribute("name", "foobar");
  map.appendChild(sheet);

  row = 1;
  column = 1;
}

XMLTree::~XMLTree()
{
    //if(root)      // We're using fancy references, now! (Werner)
    //delete root;
}

// Not needed anymore (Werner)
//const QString XMLTree::part()
//{
//  QString s;
//  QTextStream t(s, QIODevice::WriteOnly);

//  QTime tmr;
//  tmr.start();
//  kDebug(30501) << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;

//  root.save(t);  // Why does this take sooooo long (approx. 8s on my Athlon 500 with a
                 // quite small file :( )

// David: gdb says that QString::replace calls itself recursively an enormous amount of time
// This is called by QStringBuffer::write (), called by QTextStream::writeBlock ()
// called by QTextStream::operator<< () in QDOM_AttrPrivate::save ().
//
// And this looks related to the UTF 8 encoding ...

//  kDebug(30501) << (const char*)QString::number((int)tmr.elapsed()) << endl;
//  kDebug(30501) << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;

//  t << '\0';

//  return s;
//}

bool XMLTree::cell( const QString & contents )
{
  QDomElement e = root.createElement("cell");
  //e.appendChild(getFormat(xf));
  //e.appendChild(getFont(xf));

  e.setAttribute("row", row);
  e.setAttribute("column", column++);

  QDomElement format=root.createElement("format");
  format.setAttribute("float", "3");
  format.setAttribute("alignY", "2");
  format.setAttribute("floatcolor", "2");
  format.setAttribute("faktor", "1");
  format.setAttribute("precision", "-1");
  format.setAttribute("align", "4");

  QDomElement pen=root.createElement("pen");
  pen.setAttribute("width", "1");
  pen.setAttribute("style", "0");
  pen.setAttribute("color", "#000000");

  QDomElement lborder=root.createElement("left-border");
  lborder.appendChild(pen);
  format.appendChild(lborder);

  pen=root.createElement("pen");
  pen.setAttribute("width", "1");
  pen.setAttribute("style", "0");
  pen.setAttribute("color", "#000000");

  QDomElement tborder=root.createElement("top-border");
  tborder.appendChild(pen);
  format.appendChild(tborder);

  pen=root.createElement("pen");
  pen.setAttribute("width", "1");
  pen.setAttribute("style", "0");
  pen.setAttribute("color", "#000000");

  QDomElement fdia=root.createElement("fall-diagonal");
  fdia.appendChild(pen);
  format.appendChild(fdia);

  pen=root.createElement("pen");
  pen.setAttribute("width", "1");
  pen.setAttribute("style", "0");
  pen.setAttribute("color", "#000000");

  QDomElement udia=root.createElement("up-diagonal");
  udia.appendChild(pen);
  format.appendChild(udia);

  e.appendChild(format);

  QDomElement text=root.createElement("text");
  text.appendChild(root.createTextNode(contents));
  e.appendChild(text);

  sheet.appendChild(e);

  return true;
}
