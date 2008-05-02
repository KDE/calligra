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
#include "krscriptsection.h"
#include "krscriptlabel.h"
#include "krscriptfield.h"
#include "krscripttext.h"
#include "krscriptbarcode.h"
#include "krscriptimage.h"
#include "krscriptline.h"
#include "krscriptchart.h"
#include <kdebug.h>

namespace Scripting
{
	Section::Section ( KRSectionData* sec )
	{
		_section = sec;
	}


	Section::~Section()
	{
	}

	QColor Section::backgroundColor()
	{
		return _section->_bgColor->value().value<QColor>();
	}

	void   Section::setBackgroundColor ( const QColor &c)
	{
		kDebug() << c.name();
		_section->_bgColor->setValue(c);
	}

	qreal Section::height()
	{
		return _section->_height->value().toDouble();
	}

	void Section::setHeight ( qreal h)
	{
		_section->_height->setValue(h);
	}

	QString Section::name()
	{
		return _section->_name;
	}

	QObject* Section::objectByNumber(int i)
	{
		switch (_section->_objects[i]->type())
		{
			case KRObjectData::EntityLabel:
				return new Scripting::Label(_section->_objects[i]->toLabel());
				break;
			case KRObjectData::EntityField:
				return new Scripting::Field(_section->_objects[i]->toField());
				break;
			case KRObjectData::EntityText:
				return new Scripting::Field(_section->_objects[i]->toField());
				break;
			case KRObjectData::EntityBarcode:
				return new Scripting::Barcode(_section->_objects[i]->toBarcode());
				break;
			case KRObjectData::EntityLine:
				return new Scripting::Line(_section->_objects[i]->toLine());
				break;
			case KRObjectData::EntityChart:
				return new Scripting::Chart(_section->_objects[i]->toChart());
				break;
			case KRObjectData::EntityImage:
				return new Scripting::Image(_section->_objects[i]->toImage());
				break;
			default:
				return new QObject();
		}
	
	}

	QObject* Section::objectByName(const QString& n)
	{
		for (int i = 0; i < _section->objects().count(); ++i)
		{
			if (_section->_objects[i]->entityName() == n)
			{
				return objectByNumber(i);
			}
		}
		return 0;
	}

}
