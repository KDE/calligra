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

#ifndef XmlWriter_h_
#define XmlWriter_h_

#include <string>
#include <stack>
#include <vector>

#include <iostream.h>

#include <qcolor.h>
#include <qwmatrix.h>

/**
 * The XMLWriter class provides support for writing XML streams.
 * It contains methods for output XML elements with attributes.
 * 
 * Sample code:
 * <pre>
 *  ofstream os (fname);
 *  XmlWriter xml (os); // writes the XML header
 *
 *  xml.startTag ("head"); // writes &lt;head&gt;
 *
 *  // write &lt;layout format="a4" orientation="landscape"/&gt;
 *  xml.startTag ("layout", false);
 *  xml.addAttribute ("format", "a4");
 *  xml.addAttribute ("orientation", "landscape");
 *  xml.closeTag (true);
 * </pre>
 *
 * @short     A helper class for writing XML.
 * @author    Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
 * @version   $Id$
 */
class XmlWriter {
public:
  /**
   * Create a XmlWriter instance for the given output stream.
   * 
   * @param os       The open output stream for writing.
   */
  XmlWriter (ostream& os);

  /**
   * Desctructor.
   */
  ~XmlWriter ();

  /**
   * Write an element with the given ID to the XML stream. Attributes
   * can be added later.
   *
   * @param id       The element ID.
   * @param closeIt  If <tt>true</tt> the tag is closed by <tt>&gt;</tt>,
   *                 otherwise not. 
   * @param empty    If <tt>true</tt> an empty element is written, which
   *                 is closed by <tt>/&gt;</tt>.
   */
  void startTag (const char* id, bool closeIt = true, bool empty = false);

  /**
   * Write the end tag according to the given element ID or to the
   * last opened element.
   *
   * @param id       The element ID. If <tt>NULL</tt> the last opened
   *                 element is ended. (default). 
   */
  void endTag (const char* id = 0L);

  /**
   * Close the current open element.
   *
   * @param empty    If <tt>true</tt> an empty element is closed.
   */
  void closeTag (bool empty = false);

  /**
   * Add an attribute with the given value to the current element. 
   * This method doesn't check, if an element is open.
   *
   * @param name     The attribute name.
   * @param value    The string value of the attribute.
   */
  void addAttribute (const char* name, const char* value);

  /**
   * Add an attribute with the given value to the current element. 
   * This method doesn't check, if an element is open.
   *
   * @param name     The attribute name.
   * @param value    The integer value of the attribute.
   */
  void addAttribute (const char* name, int value);

  /**
   * Add an attribute with the given value to the current element. 
   * This method doesn't check, if an element is open.
   *
   * @param name     The attribute name.
   * @param value    The float value od the attribute.
   */
  void addAttribute (const char* name, float value);

  /**
   * Add an attribute with the given value to the current element. 
   * This method doesn't check, if an element is open.
   *
   * @param name     The attribute name.
   * @param value    The double value of the attribute.
   */
  void addAttribute (const char* name, double value);

  /**
   * Add an attribute with the given value to the current element. 
   * This method doesn't check, if an element is open.
   *
   * @param name     The attribute name.
   * @param color    The color value of the attribute.
   */
  void addAttribute (const char* name, const QColor& color);

  /**
   * Add an attribute with the given value to the current element. 
   * This method doesn't check, if an element is open.
   *
   * @param name     The attribute name.
   * @param m        The matrix value of the attribute.
   */
  void addAttribute (const char* name, const QWMatrix& m);

  /**
   * Write text to the XML stream. The method encodes the special
   * characters <tt>&lt;</tt>, <tt>&gt;</tt> and <tt>&amp;</tt>.
   *
   * @param s        The text.
   */
  void writeText (const char* s);

  /**
   * Write a tag to the XML stream.
   *
   * @param s        The tag without the brackets.
   */
  void writeTag (const char* s);

  /**
   * Flush the XML output stream.
   */
  void flush ();

  /**
   * Get the raw output stream.
   */
  ostream& stream () { return strm; }

private:
  stack<string, vector<string> > lastTags;
  ostream strm;
};

#endif

