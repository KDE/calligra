/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 2001-2002 Igor Janssen (rm@linux.ru.net)

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

#include <qpen.h>

class QDomDocument;
class QDomElement;

/* Class for drawing style */

class GStylePrivate;

class GStyle
{
public:
  static const int NoFill = 0;
  static const int ColorFill = 1;
  static const int GradientFill = 2;
  static const int PatternFill = 3;

  GStyle();
  GStyle(const QDomElement &style);
  GStyle(GStyle &obj);
  virtual ~GStyle();

  QDomElement writeToXml(QDomDocument &document);

  bool stroked() const;
  void stroked(bool stroked);

  const KoColor &outlineColor() const;
  void outlineColor(const KoColor &c);

  int outlineOpacity() const;
  void outlineOpacity(int o);

  unsigned int outlineWidth() const;
  void outlineWidth(unsigned int lwidth);

  Qt::PenJoinStyle joinStyle() const;
  void joinStyle(Qt::PenJoinStyle join);

  Qt::PenCapStyle capStyle() const;
  void capStyle(Qt::PenCapStyle cap);

  int filled() const;
  void filled(int filled);

  const KoColor &fillColor() const;
  void fillColor(const KoColor &c);

  Qt::BrushStyle brushStyle() const;
  void brushStyle(Qt::BrushStyle brushStyle);

  GStyle &operator=(const GStyle &s);

private:
  GStylePrivate *d;
};

#endif
