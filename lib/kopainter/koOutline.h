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

#ifndef __ko_outline_h__
#define __ko_outline_h__

#include "koColor.h"

class KoOutline
{
public:
  enum Join {JoinMiter, JoinRound, JoinBevel};
  enum Cap {CapButt, CapRound, CapSquare};

  KoOutline();

  const KoColor &color() const {return mColor; }
  void color(const KoColor &c);

  int opacity() const {return mOpacity; }
  void opacity(int o);

  double width() const {return mWidth; }
  void width(double w);

  double dashOffset() const {return mDashOffset; }
  void dashOffset(double d);

  const QMemArray<double> &dashes() const {return mDashes; }
  void dashResize(int s);
  void setDash(int n, double l);

  Join join() const {return mJoin; }
  void join(Join j);

  Cap cap() const {return mCap; }
  void cap(Cap c);

  KoOutline &operator=(const KoOutline &o);

private:
  KoColor mColor;
  int mOpacity;
  double mWidth;
  double mDashOffset;
  QMemArray<double> mDashes;
  Join mJoin;
  Cap mCap;
};

#endif
