/*
 *  Copyright (c) 2004 Adrian Page <adrian@pagenet.plus.com>
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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#if !defined KIS_POINT_H_
#define KIS_POINT_H_

#include <koPoint.h>

class KisPoint : public KoPoint {
	typedef KoPoint super;
public:
	KisPoint() {}
	KisPoint(double x, double y) : super(x, y) {}
	KisPoint(const QPoint& pt) : super(pt) {}
	KisPoint(const KoPoint& pt) : super(pt) {}

	int floorX() const { return static_cast<int>(x()); }
	int floorY() const { return static_cast<int>(y()); }
	int roundX() const { return qRound(x()); }
	int roundY() const { return qRound(y()); }

	QPoint floorQPoint() const { return QPoint(static_cast<int>(x()), static_cast<int>(y())); }
	QPoint roundQPoint() const { return QPoint(qRound(x()), qRound(y())); }
};

#endif // KIS_POINT_H_

