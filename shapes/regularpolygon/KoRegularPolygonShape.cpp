/* This file is part of the KDE project
 * Copyright (C) 2006 Isaac Clerencia <isaac@warp.es>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KoRegularPolygonShape.h"

#include <QPainter>
#include <QPainterPath>

#include <math.h>

KoRegularPolygonShape::KoRegularPolygonShape() : KoShape()
{
	m_points = 5;
}

void KoRegularPolygonShape::paint(QPainter &painter, const KoViewConverter &converter) {
	QPolygonF polygon;

	double angle = 2 *M_PI / m_points;
	double diameter = static_cast<double>(
		qMin( size().width(), size().height() ) );

	double radius = diameter * 0.5;
	double a = 0;

	for ( int i = 0; i < m_points; ++i ) {
		double xp = radius * sin(a);
		double yp = -radius * cos(a);
		a += angle;
		polygon << QPointF(xp, yp);
	}

	QRectF rect = polygon.boundingRect();
	double fx = size().width() / rect.width();
	double fy = size().height() / rect.height();

	double halfWidth = size().width() / 2;
	double halfHeight = size().height() / 2;

	for (int i = 0; i < polygon.size(); ++i) {
		polygon[i] = converter.documentToView(QPointF(halfWidth + (polygon[i].x() * fx), halfHeight + (polygon[i].y() * fy)));
	}
	polygon.translate(0,-polygon[0].y());
	
	painter.setBrush(background());
	painter.drawPolygon(polygon);
}

const QPainterPath KoRegularPolygonShape::outline() const {
	QPolygonF polygon;

	double angle = 2 *M_PI / m_points;
	double diameter = static_cast<double>(
		qMin( size().width(), size().height() ) );

	double radius = diameter * 0.5;
	double a = 0;

	for ( int i = 0; i < m_points; ++i ) {
		double xp = radius * sin(a);
		double yp = -radius * cos(a);
		a += angle;
		polygon << QPointF(xp, yp);
	}

	QRectF rect = polygon.boundingRect();
	double fx = size().width() / rect.width();
	double fy = size().height() / rect.height();

	double halfWidth = size().width() / 2;
	double halfHeight = size().height() / 2;

	for (int i = 0; i < polygon.size(); ++i) {
		polygon[i] = QPointF(halfWidth + (polygon[i].x() * fx), halfHeight + (polygon[i].y() * fy));
	}
	polygon.translate(0,-polygon[0].y());
	QPainterPath path;
	path.addPolygon(polygon);
	path.closeSubpath();
	return path;
}

void KoRegularPolygonShape::setPoints(int points) {
	m_points = points;
}
