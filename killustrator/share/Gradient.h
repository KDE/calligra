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

#ifndef Gradient_h_
#define Gradient_h_

#include <qpixmap.h>
#include <qcolor.h>

class Gradient {
public:
  enum Style { 
    Horizontal, Vertical, Radial, Rectangular, Diagonal1, Diagonal2 
  };

  Gradient (const QColor& c1, const QColor& c2, Style s);
  Gradient () : style (Horizontal) {}

  void setColor1 (const QColor& c);
  void setColor2 (const QColor& c);
  void setStyle (Style s);

  const QColor& getColor1 () const;
  const QColor& getColor2 () const;
  Style getStyle () const;

  QPixmap createPixmap (unsigned int width, unsigned int height);

private:
  void createLinearGradient (QPainter& p, unsigned int width, 
			     unsigned int height);

  QColor color1, color2;
  Style style;
};

#endif
