/* This file is part of the KDE project
  Copyright (c) 2002 Igor Janssen (rm@linux.ru.net)

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

#include "koPainter.h"

#include <qimage.h>

KoPainter::KoPainter(int w, int h)
{
  mBuffer = new QImage(w, h, 32);
}

KoPainter::~KoPainter()
{
  delete mBuffer;
}

void KoPainter::resize(int w, int h)
{
}

void KoPainter::fillAreaRGB(const QRect &r, const KoColor &c)
{
  QRgb a;
  for(int y = r.top(); y <= r.bottom(); y++)
  {
    QRgb *ptr = reinterpret_cast<QRgb *>(mBuffer->scanLine(y));
  }
}

void KoPainter::drawRectRGB(KoColor &c)
{
}
