/* This file is part of the KDE libraries
    Copyright (C) 1999 Ilya Baran (ibaran@mit.edu)

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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef MATRIXBOX_H_INCLUDED
#define MATRIXBOX_H_INCLUDED

#include "box.h"

/**
 * Matrix formula layout helper.
 * @internal
 */
class matrixbox : public box
{
protected:
  int width;
  int height; // number of elements in each direction

  QArray<box *> elems;
  QPointArray elem_pos; // the relative positions of all the elements

public:
  matrixbox();
  matrixbox(int w, int h);

  ~matrixbox();

  void addElem(box *e);

  void calculate(QPainter &p, int setFontsize = -1, QFont *f = 0, QColor *bc = 0, QColor *fc = 0);
  void draw(QPainter &p, int x, int y, QFont *f = 0, QColor *bc = 0, QColor *fc = 0);

  QRect getCursorPos(charinfo i, int x, int y);
};

#endif // MATRIXBOX_H_INCLUDED
