//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#include "misc.h"

bool dbg;


void
enlargeRectToContainPoint(QRect& r, QPoint p)
{
	if (r.contains(p)) {
		puts("enlargeRectToContainPoint: point already contained\n");	
		return;
	}
	if (p.x()<r.left())   r.setLeft(p.x());
	if (p.x()>r.right())  r.setRight(p.x());
	if (p.y()<r.top())    r.setTop(p.y());
	if (p.y()>r.bottom()) r.setBottom(p.y());
}

// Find a rectangle which encloses r whose coordinates are divisible
// by TILE_SIZE (ie no remainder)
QRect
findTileExtents(QRect r)
{
// 	puts("findTileExtents");
// 	SHOW_RECT_COORDS(r);
	r.setLeft(((r.left()+BIGNUM)/TILE_SIZE)*TILE_SIZE-BIGNUM);
	r.setTop(((r.top()+BIGNUM)  /TILE_SIZE)*TILE_SIZE-BIGNUM);
	r.setBottom(((r.bottom()+TILE_SIZE+BIGNUM)/TILE_SIZE)*TILE_SIZE-BIGNUM-1);
	r.setRight(((r.right()+TILE_SIZE+BIGNUM)  /TILE_SIZE)*TILE_SIZE-BIGNUM-1);
// 	SHOW_RECT_COORDS(r);
	return(r);
}

