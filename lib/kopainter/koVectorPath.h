/* This file is part of the KDE project
  Copyright (c) 2002 Igor Jansen (rm@kde.org)

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

#ifndef __ko_vectorpath_h__
#define __ko_vectorpath_h__

#include <qmemarray.h>
#include <libart_lgpl/art_vpath.h>

class QWMatrix;

class KoVectorPath
{
public:
  KoVectorPath();
  KoVectorPath(const KoVectorPath &vp, const QWMatrix &m);
  ~KoVectorPath();

  ArtVpath *data();

  void moveTo(double x, double y);
  void moveToOpen(double x, double y);
  void lineTo(double x, double y);
  void bezierTo(double x, double y, double x1, double y1, double x2, double y2);
  void arcTo(double rx, double ry, double sa, double a);

  void transform(const QWMatrix &m);

  static KoVectorPath *rectangle(double x, double y, double w, double h, double rx, double ry);
  static KoVectorPath *ellipse(double cx, double cy, double rx, double ry);
  static KoVectorPath *arc(double cx, double cy, double rx, double ry, double sa, double a);

private:
  void bez(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3);

private:
  QMemArray<ArtVpath> segments;
  double xe;
  double ye;
};

#endif
