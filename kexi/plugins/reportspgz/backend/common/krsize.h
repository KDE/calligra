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
#ifndef KRSIZE_H
#define KRSIZE_H
#include <KoUnit.h>
#include <QSizeF>
#include <KoGlobal.h>
#include <koproperty/property.h>

/**
	@author 
*/
class KRSize
{
	public:
		KRSize(const KoUnit& unit = KoUnit(KoUnit::Centimeter));

		~KRSize();
		QSizeF toUnit();
		QSizeF toPoint();
		QSizeF toScene();
		void setSceneSize(const QSizeF&);
		void setUnitSize(const QSizeF&);
		void setPointSize(const QSizeF&);
		void setUnit(KoUnit);
		
		KoProperty::Property* property(){return _property;}
	private:
		QSizeF _pointSize;
		KoUnit _unit;
		KoProperty::Property* _property;
};

#endif
