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

#ifndef __ko_dash_h__
#define __ko_dash_h__

#include <qmemarray.h>

class QImage;

class KoDash
{
public:
  KoDash();

  double offset() const {return mOffset; }
  void offset(double d);

  const QMemArray<double> &dashes() const {return mDashes; }

  void resize(int s);
  void setDash(int n, double l);

  KoDash &operator=(const KoDash &d);

  void draw(QImage *img);

private:
  double mOffset;
  QMemArray<double> mDashes;
};

#endif
