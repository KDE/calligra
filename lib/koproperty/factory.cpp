/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "factory.h"
#include "property.h"
#include "customproperty.h"

#include "booledit.h"
#include "combobox.h"
#include "coloredit.h"
#include "cursoredit.h"
#include "dateedit.h"
#include "datetimeedit.h"
#include "dummywidget.h"
#include "fontedit.h"
#include "linestyleedit.h"
#include "pixmapedit.h"
#include "pointedit.h"
#include "rectedit.h"
#include "sizeedit.h"
#include "sizepolicyedit.h"
#include "spinbox.h"
#include "stringlistedit.h"
#include "stringedit.h"
#include "symbolcombo.h"
#include "timeedit.h"
#include "urledit.h"

#include <qvaluelist.h>

#ifdef QT_ONLY
#else
#include <kdebug.h>
#endif

namespace KoProperty {

Factory* Factory::m_factory = 0;

class FactoryPrivate
{
	public:
		FactoryPrivate() {}
		~FactoryPrivate() {}

		//registered widgets for property types
		QMap<int, CustomPropertyFactory* > registeredWidgets;
		QMap<int, CustomPropertyFactory* > registeredCustomProperties;
};
}

using namespace KoProperty;

Factory::Factory()
{
	d = new FactoryPrivate();
}

Factory::~Factory()
{
	delete d;
}

Factory*
Factory::getInstance()
{
	if(m_factory == 0)
		m_factory = new Factory();
	return m_factory;
}

///////////////////  Functions related to widgets /////////////////////////////////////

void
Factory::registerEditor(int type, CustomPropertyFactory *creator)
{
	if(d->registeredWidgets.contains(type))
		kopropertywarn << "Type (" << type << ") already registered. Overriding actual createWidget function." << endl;
	d->registeredWidgets.insert(type, creator);
}

void
Factory::registerEditor(const QValueList<int> &types, CustomPropertyFactory *creator)
{
	QValueList<int>::ConstIterator endIt = types.constEnd();
	for(QValueList<int>::ConstIterator it = types.constBegin(); it != endIt; ++it)
		registerEditor(*it, creator);
}

Widget*
Factory::widgetForProperty(Property *property)
{
	if(!property)
		return 0;

	const int type = property->type();

	if (d->registeredWidgets.contains(type))
		return d->registeredWidgets[type]->createCustomWidget(property);

	//handle combobox-based widgets:
	if (type==Cursor)
		return new CursorEdit(property);

	if (property->listData()) {
		return new ComboBox(property);
	}

	//handle other widget types:
	switch(type)
	{
		// Default QVariant types
		case String:
		case CString:
			return new StringEdit(property);
		case Rect_X:
		case Rect_Y:
		case Rect_Width:
		case Rect_Height:
		case Point_X:
		case Point_Y:
		case Size_Width:
		case Size_Height:
		case SizePolicy_HorStretch:
		case SizePolicy_VerStretch:
		case Integer:
			return new IntEdit(property);
		case Double:
			return new DoubleEdit(property);
		case Boolean:
			return new BoolEdit(property);
		case Date:
			return new DateEdit(property);
		case Time:
			return new TimeEdit(property);
		case DateTime:
			return new DateTimeEdit(property);
		case StringList:
			return new StringListEdit(property);
		case Color:
			return new ColorButton(property);
		case Font:
			return new FontEdit(property);
		case Pixmap:
			return new PixmapEdit(property);

		// Other default types
		case Symbol:
			return new SymbolCombo(property);
		//case FontName:
		//	return new FontCombo(property);
		case FileURL:
		case DirectoryURL:
			return new URLEdit(property);
		case LineStyle:
			return new LineStyleEdit(property);

		// Composed types
		case Size:
			return new SizeEdit(property);
		case Point:
			return new PointEdit(property);
		case Rect:
			return new RectEdit(property);
		case SizePolicy:
			return new SizePolicyEdit(property);

		case List:
		case Map:
		default:
			kopropertywarn << "No editor for property " << property->name() << " of type " << property->type() << endl;
			return new DummyWidget(property);
	}

}

///////////////////  Functions related to custom properties /////////////////////////////////////

void
Factory::registerCustomProperty(int type, CustomPropertyFactory *creator)
{
	if(!creator)
		return;
	if(d->registeredCustomProperties.contains(type))
		kopropertywarn << "Type (" << type << ") already registered. Overriding actual createCustomProperty function." << endl;
	d->registeredCustomProperties.insert(type, creator);
}

void
Factory::registerCustomProperty(const QValueList<int> &types, CustomPropertyFactory *creator)
{
	QValueList<int>::ConstIterator endIt = types.constEnd();
	for(QValueList<int>::ConstIterator it = types.constBegin(); it != endIt; ++it)
		registerCustomProperty(*it, creator);
}

CustomProperty*
Factory::customPropertyForProperty(Property *prop)
{
	int type = prop->type();
	if (d->registeredCustomProperties.contains(type))
		return d->registeredCustomProperties[type]->createCustomProperty(type);

	switch(type) {
		case Size: case Size_Width: case Size_Height:
			return new SizeCustomProperty(prop);
		case Point: case Point_X: case Point_Y:
			return new PointCustomProperty(prop);
		case Rect: case Rect_X: case Rect_Y: case Rect_Width: case Rect_Height:
			return new RectCustomProperty(prop);
		case SizePolicy: case SizePolicy_HorStretch: case SizePolicy_VerStretch:
		case SizePolicy_HorData: case SizePolicy_VerData:
			return new SizePolicyCustomProperty(prop);
		default:
			return 0;
	}
}

