/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

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

#ifndef __GStyle_h__
#define __GStyle_h__

#include <koColor.h>

class QDomDocument;
class QDomElement;

/* Class for drawing style */

class GStyle
{
public:
  GStyle();
  GStyle(const QDomElement &style);
  GStyle(GStyle &obj);
     
  QDomElement writeToXml(QDomDocument &document);
  
  const KoColor &outlineColor() const {return ocolor; }
  void outlineColor(const KoColor &c);
  
  const KoColor &fillColor() const {return fcolor; }
  void fillColor(const KoColor &c);
  
  GStyle &operator=(const GStyle &s);
    
private:
  KoColor ocolor;      // outline color
  KoColor fcolor;      // fill color
};

#endif
