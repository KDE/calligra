/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 2001-2002 Igor Janssen (rm@kde.org)

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

#include <koOutline.h>

class QDomDocument;
class QDomElement;
class KoFill;
namespace Kontour
{

/* Class for drawing style */

class GStyle
{
public:
  static const int NoFill;
  static const int ColorFill;
  static const int GradientFill;
  static const int PatternFill;

  GStyle();
  GStyle(const QDomElement &style);
  GStyle(GStyle &obj);
  virtual ~GStyle();

  KoOutline *outline() const;
  KoFill *fill() const;

  QDomElement writeToXml(QDomDocument &document);

  bool stroked() const;
  void stroked(bool aStroked);

  const KoColor &outlineColor() const;
  void outlineColor(const KoColor &c);

  int outlineOpacity() const;
  void outlineOpacity(int o);

  double outlineWidth() const;
  void outlineWidth(double w);

  KoOutline::Join joinStyle() const;
  void joinStyle(KoOutline::Join join);

  KoOutline::Cap capStyle() const;
  void capStyle(KoOutline::Cap cap);

  int filled() const;
  void filled(int aFilled);

  int fillOpacity() const;
  void fillOpacity(int o);

  const KoColor &fillColor() const;
  void fillColor(const KoColor &c);

  GStyle &operator=(const GStyle &s);

private:
  bool         mStroked;
  KoOutline   *mOutline;
  int          mOutlineOpacity;
  int          mFilled;
  KoFill      *mFill;
  int          mFillOpacity;
};
};
using namespace Kontour;

#endif
