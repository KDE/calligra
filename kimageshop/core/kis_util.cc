/*
 *  kis_util.cc - part of KImageShop
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

#include "kis_util.h"

kisUtil::kisUtil() {}

void kisUtil::printRect( const QRect& r, const QString& name )
{
  qDebug("%s:: l:%d t:%d r:%d b:%d w:%d h:%d", name.latin1(), r.left(), r.top(), r.right(),
	 r.bottom(), r.width(), r.height());
}

void kisUtil::printPoint( const QPoint& p, const QString& name )
{
  qDebug("%s:: x:%d y:%d", name.latin1(), p.x(), p.y()); 
}

bool dbg;

void enlargeRectToContainPoint(QRect& r, QPoint p)
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
QRect findTileExtents(QRect r)
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

