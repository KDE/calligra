/*
 *  kis_util.h - part of KImageShop
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

#ifndef __kis_util_h__
#define __kis_util_h__

#include <qrect.h>
#include <qpoint.h>
#include <qstring.h>
#include <sys/time.h>

#include <kiconloader.h>
#include "kis_factory.h"

#define KISBarIcon( x ) BarIcon( x, KisFactory::global() )

// size for graphic blocks - must be a power of 2
const int TILE_SIZE = 128;

// Anumber which can be added to any image coordinate to make it positive
// Used to make numbers round towards + or - infinity regardless of sign
const long BIGNUM = (TILE_SIZE*10000);



class KisUtil
{
 public:
  KisUtil();

  static void printRect( const QRect&, const QString& name = "Rect" );
  static void printPoint( const QPoint&, const QString& name = "Point" );

  static void enlargeRectToContainPoint( QRect& r, QPoint p );
  static QRect findTileExtents( QRect r );
  
  static void startTimer();
  static void stopTimer( const QString& text );

 private:
  static struct timeval tv1, tv2;
  static struct timezone tz;
};

template<class T> inline T min(T a, T b) { return (a<b)?a:b; }
template<class T> inline T max(T a, T b) { return (a>b)?a:b; }                          

#endif
