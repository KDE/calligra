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

#include <qwmatrix.h>

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
}

void KoVectorPath::moveToOpen(double x, double y)
{
  int n = segments.size();
  segments.resize(n + 1);
  segments[n].code = ART_MOVETO_OPEN;
  segments[n].x = x;
  segments[n].y = y;
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

void KoVectorPath::bezierTo(double x, double y, double x1, double y1, double x2, double y2)
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

  x3_0 = x - xe;
  y3_0 = y - ye;

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

  z1_perp = (y1 - ye) * x3_0 - (x1 - xe) * y3_0;
  if (z1_perp * z1_perp > max_perp_sq)
    goto subdivide;

  z2_perp = (y - y2) * x3_0 - (x - x2) * y3_0;
  if (z2_perp * z2_perp > max_perp_sq)
    goto subdivide;

  z1_dot = (x1 - xe) * x3_0 + (y1 - ye) * y3_0;
  if (z1_dot < 0 && z1_dot * z1_dot > max_perp_sq)
    goto subdivide;

  z2_dot = (x - x2) * x3_0 + (y - y2) * y3_0;
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
  segments[n].x = x;
  segments[n].y = y;
  return;

 subdivide:

  xa1 = (xe + x1) * 0.5;
  ya1 = (ye + y1) * 0.5;
  xa2 = (xe + 2 * x1 + x2) * 0.25;
  ya2 = (ye + 2 * y1 + y2) * 0.25;
  xb1 = (x1 + 2 * x2 + x) * 0.25;
  yb1 = (y1 + 2 * y2 + y) * 0.25;
  xb2 = (x2 + x) * 0.5;
  yb2 = (y2 + y) * 0.5;
  x_m = (xa2 + xb1) * 0.5;
  y_m = (ya2 + yb1) * 0.5;

  bezierTo(x_m, y_m, xa1, ya1, xa2, ya2);
  xe = x_m;
  ye = y_m;
  bezierTo(x, y, xb1, yb1, xb2, yb2);
}

void KoVectorPath::end()
{
  int n = segments.size();
  segments.resize(n + 1);
  segments[n].code = ART_END;
}

void KoVectorPath::transform(const QWMatrix &m)
{
  for(int i = 0; i < segments.size() - 1; i++)
  {
    double x;
    double y;
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
/*		ArtVpath *res;
		ArtBpath *vec = art_new(ArtBpath, 10);

		int i = 0;

		if(rx > w / 2)
			rx = w / 2;

		if(ry > h / 2)
			ry = h / 2;

		vec[i].code = ART_MOVETO_OPEN;
		vec[i].x3 = x + rx;
		vec[i].y3 = y;

		i++;

		vec[i].code = ART_CURVETO;
		vec[i].x1 = x + rx * (1 - 0.552);
		vec[i].y1 = y;
		vec[i].x2 = x;
		vec[i].y2 = y + ry * (1 - 0.552);
		vec[i].x3 = x;
		vec[i].y3 = y + ry;

		i++;

		if(ry < h / 2)
		{
			vec[i].code = ART_LINETO;
			vec[i].x3 = x;
			vec[i].y3 = y + h - ry;

			i++;
		}

		vec[i].code = ART_CURVETO;
		vec[i].x1 = x;
		vec[i].y1 = y + h - ry * (1 - 0.552);
		vec[i].x2 = x + rx * (1 - 0.552);
		vec[i].y2 = y + h;
		vec[i].x3 = x + rx;
		vec[i].y3 = y + h;

		i++;

		if(rx < w / 2)
		{
			vec[i].code = ART_LINETO;
			vec[i].x3 = x + w - rx;
			vec[i].y3 = y + h;

			i++;
		}

		vec[i].code = ART_CURVETO;
		vec[i].x1 = x + w - rx * (1 - 0.552);
		vec[i].y1 = y + h;
		vec[i].x2 = x + w;
		vec[i].y2 = y + h - ry * (1 - 0.552);
		vec[i].x3 = x + w;

		vec[i].y3 = y + h - ry;

		i++;

		if(ry < h / 2)
		{
			vec[i].code = ART_LINETO;
			vec[i].x3 = x + w;
			vec[i].y3 = y + ry;

			i++;
		}

		vec[i].code = ART_CURVETO;
		vec[i].x1 = x + w;
		vec[i].y1 = y + ry * (1 - 0.552);
		vec[i].x2 = x + w - rx * (1 - 0.552);
		vec[i].y2 = y;
		vec[i].x3 = x + w - rx;
		vec[i].y3 = y;

		i++;

		if(rx < w / 2)
		{
			vec[i].code = ART_LINETO;
			vec[i].x3 = x + rx;
			vec[i].y3 = y;

			i++;
		}

		vec[i].code = ART_END;

		res = art_bez_path_to_vec(vec, 0.25);
*/
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
