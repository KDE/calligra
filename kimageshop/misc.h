//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#ifndef MISC_H
#define MISC_H

#include <qrect.h>
#include <qpoint.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>


#define MAX(a,b)        ((a) > (b) ? (a) : (b))
#define MIN(a,b)        ((a) < (b) ? (a) : (b))

#define SHOW_RECT(A) printf("Rect(p,s) " #A " = (%d,%d,%d,%d)\n",\
														A.left(),A.top(),A.width(),A.height());

#define SHOW_RECT_COORDS(A) printf("Rect(coords) " #A " = (%d,%d,%d,%d)\n",\
														A.left(),A.top(),A.right(),A.bottom());

#define SHOW_POINT(A) printf("Point " #A " = (%d,%d)\n", A.x(),A.y());
#define showi(X) printf( #X " = %d\n", int(X));
#define showf(X) printf( #X " = %f\n", float(X));
#define showc(X) printf( #X " = %c\n", char(X));
#define shows(X) printf( #X " = %s\n", (char *)X);                              

// Time size for graphic blocks - must be a power of 2
//#define TILE_SIZE 128
#define TILE_SIZE 128

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
	float t=float(tv2.tv_sec-tv1.tv_sec)+(tv2.tv_usec-tv1.tv_usec)/1000000.;\
	printf(STR " took %5.6f seconds\n",t);\
}

void enlargeRectToContainPoint(QRect& r, QPoint p);
QRect findTileExtents(QRect r);

extern bool dbg;


#endif
