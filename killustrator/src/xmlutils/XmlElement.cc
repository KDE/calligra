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

#include <stdlib.h>
#include <strstream.h>
#include "XmlElement.h"

XmlAttribute::XmlAttribute (const string& n, const string& v) :
  aname (n), value (v) {
}

XmlAttribute::XmlAttribute (const XmlAttribute& attr) : 
  aname (attr.aname), value (attr.value) {
}

XmlAttribute::~XmlAttribute () {
}

XmlAttribute& XmlAttribute::operator= (const XmlAttribute& attr) {
  aname = attr.aname;
  value = attr.value;
  return *this;
}

float XmlAttribute::floatValue () const {
  return atof (value.c_str ());
}

int XmlAttribute::intValue () const {
  return atoi (value.c_str ());
}

QColor XmlAttribute::colorValue () const {
  float red = 0, green = 0, blue = 0;

  istrstream strm (value.c_str ());
  strm >> red >> green >> blue;

  return QColor (red, green, blue);
}

QWMatrix XmlAttribute::matrixValue () const {
  float m11 = 1, m12 = 0, m13 = 0;
  float m21 = 0, m22 = 1, m23 = 0;
  float m31 = 0, m32 = 0, m33 = 1;

  istrstream strm (value.c_str ());
  strm >> m11 >> m12 >> m13 
       >> m21 >> m22 >> m23 
       >> m31 >> m32 >> m33;
  return QWMatrix (m11, m12, m21, m22, m31, m32);
}

XmlElement::XmlElement () {
  closed = false;
  endTag = false;
}

XmlElement::XmlElement (const XmlElement& elem) :
  tagId (elem.tagId), closed (elem.closed), endTag (elem.endTag),
  attribs (elem.attribs) {
}

XmlElement::~XmlElement () {
}

void XmlElement::reset () {
  tagId = "";
  closed = false;
  endTag = false;
  attribs.erase (attribs.begin (), attribs.end ());
}

XmlElement& XmlElement::operator= (const XmlElement& elem) {
  tagId = elem.tagId;
  closed = elem.closed;
  endTag = elem.endTag;
  attribs =  elem.attribs;
  return *this;
}


