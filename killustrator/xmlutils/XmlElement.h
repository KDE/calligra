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

#ifndef XmlElement_h_
#define XmlElement_h_

#include <list>
#include <string>

#include <qcolor.h>
#include <qwmatrix.h>

/**
 * An instance of XmlAttribute represents an attribute of 
 * XML elements. It provides methods for accessing the attribute 
 * value.
 *
 * @short     A class for representing attributes of XML elements.
 * @author    Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
 * @version   $Id$
 */
class XmlAttribute {
public:
  /**
   * Construct an attribute from the given name and value.
   *
   * @param n   The name of the attribute.
   * @param v   The attribute value.
   */
  XmlAttribute (const string& n, const string& v);

  /**
   * Copy constructor.
   */
  XmlAttribute (const XmlAttribute& attr);

  /**
   * Destructor.
   */
  ~XmlAttribute ();

  /**
   * The assignment operator.
   */
  XmlAttribute& operator= (const XmlAttribute& attr);

  /**
   * Return the name of the attribute.
   *
   * @return The attribute name.
   */
  const string& name () const { return aname; }

  /**
   * Return a string representation of the attribute value.
   *
   * @return The attribute value as string.
   */
  const string& stringValue () const { return value; }

  /**
   * Return a float representation of the attribute value.
   *
   * @return The attribute value as float.
   */
  float floatValue () const;

  /**
   * Return a integer representation of the attribute value.
   *
   * @return The attribute value as integer.
   */
  int intValue () const;

  /**
   * Return a color representation of the attribute value.
   *
   * @return The attribute value as QColor instance.
   */
  QColor colorValue () const;

  /**
   * Return a matrix representation of the attribute value.
   *
   * @return The attribute value as QWMatrix instance.
   */
  QWMatrix matrixValue () const;

private:
  string aname, value;
};

/**
 * An instance of XmlElement represents an element (object) of a XML 
 * document. The elements consists of the tag (element ID) and a list
 * of attributes.
 *
 * @short     A class for representing XML elements.
 * @author    Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
 * @version   $Id$
 */
class XmlElement {
friend class XmlReader;
public:
  /**
   * Construct a new XML element.
   */
  XmlElement ();

  /**
   * Copy constructor.
   */
  XmlElement (const XmlElement& elem);

  /**
   * Destructor.
   */
  ~XmlElement ();

  void reset ();

  /**
   * The assignment operator.
   */
  XmlElement& operator= (const XmlElement& elem);

  /**
   * Return the ID of the element.
   *
   * @return The element ID.
   */
  const string& tag () const { return tagId; }

  /**
   * Return the attributes of the element.
   * @see XmlAttribute
   *
   * @return The list of attributes.
   */
  const list<XmlAttribute>& attributes () const { return attribs; }

  /**
   * Return <tt>true</tt> if the element is closed.
   *
   * @return <tt>true</tt> for a closed element.
   */
  bool isClosed () const { return closed; }

  /**
   * Return <tt>true</tt> if the element is an end tag.
   *
   * @return <tt>true</tt> for an end tag.
   */
  bool isEndTag () const { return endTag; }

private:
  string tagId;
  bool closed;
  bool endTag;
  list<XmlAttribute> attribs;
};

#endif

