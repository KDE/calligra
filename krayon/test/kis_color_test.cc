/*
 *  kis_color_test.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <me@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <kis_color.h>

void dumpColor(const KisColor& k)
{
  qDebug(" rgb : %3d %3d %3d", k.R(), k.G(), k.B());
  qDebug(" hsv : %3d %3d %3d", k.h(), k.s(), k.v());
  qDebug(" lab : %3d %3d %3d", k.l(), k.a(), k.b());
  qDebug(" cmyk: %3d %3d %3d %3d", k.c(), k.m(), k.y(), k.k());
  
  switch (k.native())
    {
    case RGB:
      qDebug(" RGB is native color space.");
      break;
    case HSV:
      qDebug(" HSV is native color space.");
      break;
    case LAB:
      qDebug(" Lab is native color space.");
      break;
    case CMYK:
      qDebug(" CMYK is native color space.");
      break;
    default:
      qDebug(" Warning: No native color space.");
      break;
    }
}
int main( int argc, char **argv )
{
  qDebug("\n--> KColor a;");
  KisColor a;
  qDebug("Dump a:");
  dumpColor(a);

  qDebug("\n--> KisColor b(0, 0, 255);");
  KisColor b(0, 0, 255);
  qDebug("Dump b:");
  dumpColor(b);

  qDebug("\n--> KisColor c(0, 255, 255, 0);");
  KisColor c(0, 255, 255, 0);
  qDebug("Dump c:");
  dumpColor(c);

  qDebug("\n--> a = c;");
  a = c;
  qDebug("Dump a:");
  dumpColor(a);

  qDebug("\n--> a = KisColor::red();");
  a = KisColor::red();
  qDebug("Dump a:");
  dumpColor(a);

  qDebug("\n--> KisColor d = KisColor::darkBlue();");
  KisColor d = KisColor::darkBlue();
  qDebug("Dump d:");
  dumpColor(d);

  qDebug("\n--> KisColor e = KisColor(QColor(0, 255, 0));");
  KisColor e = KisColor(QColor(0, 255, 0));
  qDebug("Dump e:");
  dumpColor(e);
  qDebug("Dump e again:");
  dumpColor(e);

  qDebug("\n--> e.setCMYK(255, 0, 0, 0);");
  e.setCMYK(255, 0, 0, 0);
  qDebug("Dump e:");
  dumpColor(e);

  return 0;
}


