/*
 *  kis_util.h - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <me@kde.org>
 *                1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
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
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

class kisUtil
{
 public:
  kisUtil();

  static void printRect( const QRect&, const QString& name = "Rect" );
  static void printPoint( const QPoint&, const QString& name = "Point" );
};

#define MAX(a,b)        ((a) > (b) ? (a) : (b))
#define MIN(a,b)        ((a) < (b) ? (a) : (b))

#define showi(X) printf( #X " = %d\n", int(X));
#define showf(X) printf( #X " = %f\n", float(X));
#define showc(X) printf( #X " = %c\n", char(X));
#define shows(X) printf( #X " = %s\n", (char *)X);                              

// Time size for graphic blocks - must be a power of 2
#define TILE_SIZE 128
//#define TILE_SIZE 64

// Anumber which can be added to any image coordinate to make it positive
// Used to make numbers round towards + or - infinity regardless of sign
#define BIGNUM (TILE_SIZE*10000)

#define TIME_START \
{\
	struct timeval tv1,tv2;\
	struct timezone tz;\
	gettimeofday(&tv1,&tz);

#define TIME_END(STR)\
 	gettimeofday(&tv2,&tz);\
	float _t=float(tv2.tv_sec-tv1.tv_sec)+(tv2.tv_usec-tv1.tv_usec)/1000000.;\
	printf(STR " took %5.6f seconds\n",_t);\
}

void enlargeRectToContainPoint(QRect& r, QPoint p);
QRect findTileExtents(QRect r);

extern bool dbg;


#endif
