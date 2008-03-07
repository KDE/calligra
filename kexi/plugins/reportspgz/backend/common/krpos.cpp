/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)                  
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#include "krpos.h"
#include <kdebug.h>

KRPos::KRPos(const KoUnit& unit)
{
	_unit = unit;
	//TODO When KoProperty can handle QPointF
	_property = new KoProperty::Property ( "Position", toScene().toPoint(), "Position", "Position" );
}

void KRPos::setName(const QString& n)
{
	_property->setName(n.toLatin1());
	_property->setCaption(n);
}

KRPos::~KRPos()
{
}

void KRPos::setScenePos(const QPointF& pos, bool update)
{
	qreal x, y;
	
	x = INCH_TO_POINT(pos.x() / KoGlobal::dpiX());
	y = INCH_TO_POINT(pos.y() / KoGlobal::dpiY());

	_pointPos.setX(x);
	_pointPos.setY(y);
	
	if (update)
	_property->setValue(toUnit());
}

void KRPos::setUnitPos(const QPointF& pos, bool update)
{
	kDebug() << pos << endl;
	qreal x, y;
	x = _unit.fromUserValue(pos.x());
	y = _unit.fromUserValue(pos.y());

	_pointPos.setX(x);
	_pointPos.setY(y);
	
	if (update)
	_property->setValue(toUnit().toPoint());
}

void KRPos::setPointPos(const QPointF& pos, bool update)
{
	_pointPos.setX(pos.x());
	_pointPos.setY(pos.y());
	
	if (update)
	_property->setValue(toUnit().toPoint());
	
}

void KRPos::setUnit(KoUnit u)
{
	_unit = u;
	_property->setValue(toUnit().toPoint());
}

QPointF KRPos::toPoint()
{
	return _pointPos;
}

QPointF KRPos::toScene()
{
	qreal x, y;
	x = POINT_TO_INCH(_pointPos.x()) * KoGlobal::dpiX();
	y = POINT_TO_INCH(_pointPos.y()) * KoGlobal::dpiY();

	return QPointF(x, y);
}

QPointF KRPos::toUnit()
{
	qreal x, y;
	x = _unit.toUserValue(_pointPos.x());
	y = _unit.toUserValue(_pointPos.y());
	
	return QPointF(x, y);
}

