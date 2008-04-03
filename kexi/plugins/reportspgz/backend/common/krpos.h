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
#ifndef KRPOS_H
#define KRPOS_H
#include <KoGlobal.h>
#include <KoUnit.h>
#include <QPointF>
#include <koproperty/property.h>

/**
	@author
*/
class KRPos
{
	public:
		KRPos(const KoUnit& unit = KoUnit(KoUnit::Centimeter));

		~KRPos();
		QPointF toUnit();
		QPointF toPoint();
		QPointF toScene();
		void setScenePos(const QPointF&, bool = true);
		void setUnitPos(const QPointF&, bool = true);
		void setPointPos(const QPointF&, bool = true);
		void setUnit(KoUnit);
		void setName(const QString&);
		KoProperty::Property* property(){return _property;}
	private:
		QPointF _pointPos;
		KoUnit _unit;
		KoProperty::Property* _property;
};

#endif
