/* This file is part of the KDE project
  Copyright (c) 2002 Igor Janssen (rm@kde.org)

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

#include "koVectorPath.h"

#include <cmath>

#include <qwmatrix.h>

#include <kdebug.h>

KoVectorPath::KoVectorPath()
{
}

KoVectorPath::~KoVectorPath()
{
}

ArtVpath *KoVectorPath::data()
{
  return segments.data();
}

void KoVectorPath::moveTo(double x, double y)
{
  int n = segments.size();
  segments.resize(n + 1);
  segments[n].code = ART_MOVETO;
  segments[n].x = x;
  segments[n].y = y;
  xe = x;
  ye = y;
}

void KoVectorPath::moveToOpen(double x, double y)
{
  int n = segments.size();
  segments.resize(n + 1);
  segments[n].code = ART_MOVETO_OPEN;
  segments[n].x = x;
  segments[n].y = y;
  xe = x;
  ye = y;
}

void KoVectorPath::lineTo(double x, double y)
{
  int n = segments.size();
  segments.resize(n + 1);
  segments[n].code = ART_LINETO;
  segments[n].x = x;
  segments[n].y = y;
  xe = x;
  ye = y;
}

void KoVectorPath::bez(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3)
{
  const double flatness = 0.25;
  int n;
  double x3_0, y3_0;
  double z3_0_dot;
  double z1_dot, z2_dot;
  double z1_perp, z2_perp;
  double max_perp_sq;

  double x_m, y_m;
  double xa1, ya1;
  double xa2, ya2;
  double xb1, yb1;
  double xb2, yb2;

  /* It's possible to optimize this routine a fair amount.

     First, once the _dot conditions are met, they will also be met in
     all further subdivisions. So we might recurse to a different
     routine that only checks the _perp conditions.

     Second, the distance _should_ decrease according to fairly
     predictable rules (a factor of 4 with each subdivision). So it might
     be possible to note that the distance is within a factor of 4 of
     acceptable, and subdivide once. But proving this might be hard.

     Third, at the last subdivision, x_m and y_m can be computed more
     expeditiously (as in the routine above).

     Finally, if we were able to subdivide by, say 2 or 3, this would
     allow considerably finer-grain control, i.e. fewer points for the
     same flatness tolerance. This would speed things up downstream.

     In any case, this routine is unlikely to be the bottleneck. It's
     just that I have this undying quest for more speed...

  */

  x3_0 = x3 - x0;
  y3_0 = y3 - y0;

  /* z3_0_dot is dist z0-z3 squared */
  z3_0_dot = x3_0 * x3_0 + y3_0 * y3_0;

  /* todo: this test is far from satisfactory. */
  if (z3_0_dot < 0.001)
    goto nosubdivide;

  /* we can avoid subdivision if:

     z1 has distance no more than flatness from the z0-z3 line

     z1 is no more z0'ward than flatness past z0-z3

     z1 is more z0'ward than z3'ward on the line traversing z0-z3

     and correspondingly for z2 */

  /* perp is distance from line, multiplied by dist z0-z3 */
  max_perp_sq = flatness * flatness * z3_0_dot;

  z1_perp = (y1 - y0) * x3_0 - (x1 - x0) * y3_0;
  if (z1_perp * z1_perp > max_perp_sq)
    goto subdivide;

  z2_perp = (y3 - y2) * x3_0 - (x3 - x2) * y3_0;
  if (z2_perp * z2_perp > max_perp_sq)
    goto subdivide;

  z1_dot = (x1 - x0) * x3_0 + (y1 - y0) * y3_0;
  if (z1_dot < 0 && z1_dot * z1_dot > max_perp_sq)
    goto subdivide;

  z2_dot = (x3 - x2) * x3_0 + (y3 - y2) * y3_0;
  if (z2_dot < 0 && z2_dot * z2_dot > max_perp_sq)
    goto subdivide;

  if (z1_dot + z1_dot > z3_0_dot)
    goto subdivide;

  if (z2_dot + z2_dot > z3_0_dot)
    goto subdivide;

 nosubdivide:
  /* don't subdivide */
  n = segments.size();
  segments.resize(n + 1);
  segments[n].code = ART_LINETO;
  segments[n].x = x3;
  segments[n].y = y3;
  return;

 subdivide:

  xa1 = (x0 + x1) * 0.5;
  ya1 = (y0 + y1) * 0.5;
  xa2 = (x0 + 2 * x1 + x2) * 0.25;
  ya2 = (y0 + 2 * y1 + y2) * 0.25;
  xb1 = (x1 + 2 * x2 + x3) * 0.25;
  yb1 = (y1 + 2 * y2 + y3) * 0.25;
  xb2 = (x2 + x3) * 0.5;
  yb2 = (y2 + y3) * 0.5;
  x_m = (xa2 + xb1) * 0.5;
  y_m = (ya2 + yb1) * 0.5;
  bez(x0, y0, xa1, ya1, xa2, ya2, x_m, y_m);
  bez(x_m, y_m, xb1, yb1, xb2, yb2, x3, y3);
}

void KoVectorPath::bezierTo(double x, double y, double x1, double y1, double x2, double y2)
{
  bez(xe, ye, x1, y1, x2, y2, x, y);
  xe = x;
  ye = y;
}

void KoVectorPath::end()
{
  int n = segments.size();
  segments.resize(n + 1);
  segments[n].code = ART_END;
}

void KoVectorPath::transform(const QWMatrix &m)
{
  double x;
  double y;
  for(int i = 0; i < segments.size() - 1; i++)
  {
    m.map(segments[i].x, segments[i].y, &x, &y);
    segments[i].x = x;
    segments[i].y = y;
  }
}

KoVectorPath *KoVectorPath::rectangle(double x, double y, double w, double h, double rx, double ry)
{
  KoVectorPath *vec = new KoVectorPath;
  if(static_cast<int>(rx) != 0 && static_cast<int>(ry) != 0)
  {
    if(rx > w / 2)
      rx = w / 2;
    if(ry > h / 2)
      ry = h / 2;
    vec->moveTo(x + rx, y);
    vec->bezierTo(x, y + ry, x + rx * (1 - 0.552), y, x, y + ry * (1 - 0.552));
    if(ry < h / 2)
      vec->lineTo(x, y + h - ry);
    vec->bezierTo(x + rx, y + h, x, y + h - ry * (1 - 0.552), x + rx * (1 - 0.552), y + h);
    if(rx < w / 2)
      vec->lineTo(x + w - rx, y + h);
    vec->bezierTo(x + w, y + h - ry, x + w - rx * (1 - 0.552), y + h, x + w, y + h - ry * (1 - 0.552));
    if(ry < h / 2)
      vec->lineTo(x + w, y + ry);
    vec->bezierTo(x + w - rx, y, x + w, y + ry * (1 - 0.552), x + w - rx * (1 - 0.552), y);
    if(rx < w / 2)
      vec->lineTo(x + rx, y);
    vec->end();
  }
  else
  {
    vec->segments.resize(6);
    vec->segments[0].code = ART_MOVETO;
    vec->segments[0].x = x;
    vec->segments[0].y = y;

    vec->segments[1].code = ART_LINETO;
    vec->segments[1].x = x;
    vec->segments[1].y = y + h;

    vec->segments[2].code = ART_LINETO;
    vec->segments[2].x = x + w;
    vec->segments[2].y = y + h;

    vec->segments[3].code = ART_LINETO;
    vec->segments[3].x = x + w;
    vec->segments[3].y = y;

    vec->segments[4].code = ART_LINETO;
    vec->segments[4].x = x;
    vec->segments[4].y = y;

    vec->segments[5].code = ART_END;
  }
  return vec;
}

KoVectorPath *KoVectorPath::ellipse(double cx, double cy, double rx, double ry)
{
  KoVectorPath *vec = new KoVectorPath;
  double srx = rx * 0.5522847498;
  double sry = ry * 0.5522847498;
  vec->moveTo(cx, cy - ry);
  vec->bezierTo(cx + rx, cy, cx + srx, cy - ry, cx + rx, cy  - sry);
  vec->bezierTo(cx, cy + ry, cx + rx, cy  + sry, cx + srx, cy + ry);
  vec->bezierTo(cx - rx, cy, cx - srx, cy + ry, cx - rx, cy  + sry);
  vec->bezierTo(cx, cy - ry, cx - rx, cy - sry, cx - srx, cy - ry);
  vec->end();
  return vec;
}
