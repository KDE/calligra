/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by  
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "XmlWriter.h"

using namespace std;

XmlWriter::XmlWriter (ostream& os) 
  : strm (os.rdbuf ()) {
  strm << "<?xml version=\"1.0\"?>" << endl;
}

XmlWriter::XmlWriter (ostream& os, bool standalone) 
  : strm (os.rdbuf ()) {
  strm << "<?xml version=\"1.0\" standalone=\"";
  if (standalone)
    strm << "yes";
  else
    strm << "no";
  strm << "\" ?>" << endl;
}

XmlWriter::~XmlWriter () {
  flush ();
}

void XmlWriter::docType (const char *str) {
  strm << "<!DOCTYPE " << str << ">" << endl;
}

void XmlWriter::startTag (const char* id, bool closeIt, bool empty) {
  strm << "<" << id;
  if (!empty) {
    lastTags.push (id);
  }

  if (closeIt) {
    if (empty) 
      strm << "/";
    strm << ">" << endl;
  }
  else
    strm << ' ';
}
 
void XmlWriter::endTag (const char* id) {
  strm << "</";
  if (id)
    strm << id;
  else {
    string tag = lastTags.top ();
    lastTags.pop ();
    strm << tag;
  }
  strm << ">" << endl;
}

void XmlWriter::closeTag (bool empty) {
  if (empty) {
    strm << "/";
    lastTags.pop ();
  }
  strm << ">" << endl;
}

void XmlWriter::addAttribute (const char* name, const char* value) {
  strm << name << "=\"";
  strm << value;
  strm << "\" ";
}

void XmlWriter::addAttribute (const char* name, int value) {
  strm << name << "=\"";
  strm << value;
  strm << "\" ";
}

void XmlWriter::addAttribute (const char* name, float value) {
  strm << name << "=\"";
  strm << value;
  strm << "\" ";
}

void XmlWriter::addAttribute (const char* name, double value) {
  strm << name << "=\"";
  strm << value;
  strm << "\" ";
}

void XmlWriter::addAttribute (const char* name, const QColor& color) {
  strm << name << "=\"";
  strm << color.red () << ' ' << color.green () << ' ' << color.blue ();
  strm << "\" ";
}

void XmlWriter::addAttribute (const char* name, const QWMatrix& m) {
  strm << name << "=\"";
  strm << m.m11 () << ' ' << m.m12 () << ' ' << 0 << ' ';
  strm << m.m21 () << ' ' << m.m22 () << ' ' << 0 << ' ';
  strm << m.dx () << ' ' << m.dy () << ' ' << 1;
  strm << "\" ";
}

void XmlWriter::writeText (const char* s) {
  const char* c = s;
  while (*c != '\0') {
    if (*c == '<') strm << "&lt;";
    else if (*c == '&') strm << "&amp;";
    else if (*c == '>') strm << "&gt;";
    else strm << *c;
    c++;
  }
}

void XmlWriter::writeTag (const char* s) {
  strm << "<" << s << ">";
}

void XmlWriter::flush () {
  strm.flush ();
}

