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

#ifndef XmlReader_h_
#define XmlReader_h_

#include <iostream.h>

#include "XmlElement.h"
#include "XmlTokenizer.h"

/**
 * The XMLReader class supports reading elements from a XML stream.
 *
 * @short     A class for reading XML elements from a stream.
 * @author    Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
 * @version   $Id$
 */
class XmlReader {
public:
  /**
   * Construct a XmlReader instance for the given input stream.
   *
   * @param is   The open input stream.
   */
  XmlReader (istream& is);

  /**
   * Desctructor
   */
  ~XmlReader ();

  /**
   * Check the input stream for a valid XML header.
   * A header should look like
   * <pre>
   *  &lt;?xml version="1.0"?&gt;
   *  &lt;!doctype dtype system "dtd"&gt;
   * </pre>
   * where <tt>dtype</tt> and <tt>dtd</tt> are simple strings.
   *
   * @return  <tt>true</tt> if the header conforms to XML, otherwise
   *          <tt>false</tt>.
   */
  bool validHeader ();

  /**
   * Return the document type.
   *
   * @return   The name of the document type.
   */
  const string& doctype () const;

  /**
   * Return the name of the document type definition.
   *
   * @return   The name of the DTD.
   */
  const string& dtd () const;

  /**
   * Read a XML element from the stream. If the content is plain text
   * (no tag), an element with the pseudo ID <tt>#PADATA</tt> is returned 
   * and the text is available via method getText.
   *
   * @see #getText
   *
   * @param elem    The XML element which is initialized by the method.
   * @return        <tt>true</tt> for successful reading.
   */
  bool readElement (XmlElement& elem);

  /**
   * Read plain text from the stream.
   *
   * @return The text as a string.
   */
  const string& getText ();

protected:
  bool parseEndElement (XmlElement& elem);
  bool parseElement (const string& id, XmlElement& elem);
  bool readAttributes (list<XmlAttribute>& attrib_list);

private:
  XmlTokenizer tokenizer;
  string s_dtype, s_dtd;
  string text;
};

#endif

