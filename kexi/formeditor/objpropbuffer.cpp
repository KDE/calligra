/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kdebug.h>
#include <qstringlist.h>
#include <qstrlist.h>
#include <qmetaobject.h>
#include <qvariant.h>

#include <klocale.h>

#include "objecttree.h"
#include "form.h"
#include "container.h"
#include "formmanager.h"
#include "spacer.h"
#include "kexipropertyeditor.h"
#include "kexipropertyeditoritem.h"

#include "objpropbuffer.h"

namespace KFormDesigner {

ObjectPropertyBuffer::ObjectPropertyBuffer(FormManager *manager, QObject *parent, const char *name)
 : KexiPropertyBuffer(parent, name)
{
	m_object = 0;
	m_manager = manager;
}

void
ObjectPropertyBuffer::changeProperty(const QString &property, const QVariant &value)
{
	kdDebug() << "ObjPropBuffer::changeProperty(): changing: " << property << endl;
	KexiPropertyBuffer::changeProperty(property, value);
	
	if(property == "name")
		emit nameChanged(m_object->name(), value.toString());

	if((property == "hAlign") || (property == "vAlign") || (property == "wordbreak"))
	{
		saveAlignProperty();
	}
	else if(property == "layout")
	{
		saveLayoutProperty(value.toString());
	}
	else
	{
		if(!m_multiple)
		{
			m_object->setProperty(property.latin1(), value);
			emit propertyChanged(m_object, property, value);
		}
		else
		{
			QWidget *w;
			for(w = m_widgets.first(); w; w = m_widgets.next())
			{
				kdDebug() << "saving property for widget " << w->name() << endl;
				w->setProperty(property.latin1(), value);
				emit propertyChanged((QObject*)w, property, value);
			}
		}
	}
}

void
ObjectPropertyBuffer::setWidget(QWidget *widg)
{
	QObject *obj = (QObject*)widg; 
	if(obj==m_object && !m_multiple)
		return;

	m_widgets.clear();
	m_widgets.append(widg);
	m_multiple = false;
	checkModifiedProp();
	kdDebug() << "loading object = " << widg->name() << endl;
	
	if(m_object)
		m_object->removeEventFilter(this);

	m_manager->editor()->reset(false);
	clear();

	m_object = obj;
	QStrList pList = obj->metaObject()->propertyNames(true);

	int count = 0;
	QStrListIterator it(pList);
	for(; it.current() != 0; ++it)
	{
		count = obj->metaObject()->findProperty(*it, true);
		const QMetaProperty *meta = obj->metaObject()->property(count, true);
		if(meta->designable(obj))
		{
			if(!showProperty(obj, meta->name()))
				continue;

			QString desc = descFromName(meta->name());
			if(meta->isEnumType())
			{
				QStrList keys = meta->enumKeys();
				if(QString(meta->name()) == QString("alignment"))
				{
					createAlignProperty(meta, obj);
				}
				else
				{
					QStringList values = descList(QStringList::fromStrList(keys));
					
					add(new KexiProperty(meta->name(), meta->valueToKey(obj->property(meta->name()).toInt()),
						QStringList::fromStrList(keys), values, desc));
				}
			}
			else
				add(new KexiProperty(meta->name(), obj->property(meta->name()), desc));
		}
	}
	
	if(m_manager->activeForm())
	{
		ObjectTreeItem *objectIt = m_manager->activeForm()->objectTree()->lookup(widg->name());
		if(objectIt && objectIt->container())
			createLayoutProperty(objectIt->container());
	}

	m_manager->editor()->setBuffer(this);

	obj->installEventFilter(this);
}

void
ObjectPropertyBuffer::addWidget(QWidget *widg)
{
	m_multiple = true;
	if(m_widgets.find(widg) == -1)
		m_widgets.append(widg);

	QString classn;
	if(m_object->className() == widg->className())
		classn = m_object->className();

	m_manager->editor()->clear();

	QDictIterator<KexiProperty> it(*this);
	for(; it.current(); ++it)
	{
		if(!showMultipleProperty(it.currentKey(), classn))
			(*this)[it.currentKey()]->setVisible(false);
	}

	m_manager->editor()->setBuffer(this);
}

bool
ObjectPropertyBuffer::showProperty(QObject *obj, const QString &property)
{
	QWidget *w = (QWidget*)obj;
	if(!m_manager->isTopLevel(w))
	{
		QStringList list;
		list << "caption" << "icon" << "sizeIncrement" << "iconText";
		if(!(list.grep(property)).isEmpty())
			return false;
	}
	kdDebug() << "object is a " << obj->className() << endl;
	if(obj->isA("KFormDesigner::Spacer"))
		return Spacer::showProperty(property);
	return true;
}

bool
ObjectPropertyBuffer::showMultipleProperty(const QString &property, const QString &className)
{
	if(className.isNull())
	{
		QStringList list;
		list << "font" << "paletteBackgroundColor" << "enabled" << "paletteForegroundColor" << "cursor" << "paletteBackgroundPixmap";
		if(!(list.grep(property)).isEmpty())
			return true;
	}
	return false;
	// TODO : Filter properties following class name (ie : "alignment" for labels or line edits ...) (maybe using WidgetFactory ?)
}

bool
ObjectPropertyBuffer::eventFilter(QObject *o, QEvent *ev)
{
	if(o==m_object)
	{
		if((ev->type() == QEvent::Resize) || (ev->type() == QEvent::Move))
		{
			if((*this)["geometry"]->value() == o->property("geometry")) // to avoid infinite recursion
				return false;

			(*this)["geometry"]->setValue(((QWidget*)o)->geometry());
		}
	}
	return false;
}

void
ObjectPropertyBuffer::checkModifiedProp()
{
	if(m_object)
	{
		if(!m_manager->activeForm())
			return;
		ObjectTreeItem *treeIt = m_manager->activeForm()->objectTree()->lookup(m_object->name());
		if(treeIt)
		{
			QString name;
			QDictIterator<KexiProperty> it(*this);
			for(; it.current(); ++it)
			{
				name = it.current()->name();
				if((name == "hAlign") || (name == "vAlign") || (name == "wordbreak") || (name == "layout"))
						return;
				if(it.current()->changed())
					treeIt->addModProperty(name);
			}
		}
	}
}

QString
ObjectPropertyBuffer::descFromName(const QString &name)
{
	if(name == "name") return i18n("Name");
	if(name == "paletteBackgroundPixmap")  return i18n("Background Pixmap");
	if(name == "enabled") return i18n("Enabled");
	if(name == "geometry") return i18n("Geometry");
	if(name == "font") return i18n("Font");
	if(name == "cursor") return i18n("Cursor");

	return name;
}

QStringList
ObjectPropertyBuffer::descList(const QStringList &strlist)
{
	QStringList desc;
	QStringList list = strlist;

	for(QStringList::iterator it = list.begin(); it != list.end(); ++it)
	{
		desc += descFromValue(*it);
	}
	return desc;
}

QString
ObjectPropertyBuffer::descFromValue(const QString &name)
{
	if(name == "NoBackground") return i18n("No Background");
	if(name == "PaletteForeground") return i18n("Palette Foreground");
	if(name == "AutoText") return i18n("Auto");

	return name;
}

void
ObjectPropertyBuffer::createAlignProperty(const QMetaProperty *meta, QObject *obj)
{
	kdDebug() << "alignment property" << endl;
	QStringList list;
	QString value;
	QStringList keys = QStringList::fromStrList( meta->valueToKeys(obj->property("alignment").toInt()) );
	kdDebug() << "keys is " << keys.join("|") << endl;

	
	if(!keys.grep("AlignHCenter").empty())
		value = "AlignHCenter";
	else if(!keys.grep("AlignRight").empty())
		value = "AlignRight";
	else if(!keys.grep("AlignLeft").empty())
		value = "AlignLeft";
	else if(!keys.grep("AlignJustify").empty())
		value = "AlignJustify";
	else
		value = "AlignAuto";
	kdDebug() << "Hor value is " << value << endl;

	list << "AlignAuto" << "AlignLeft" << "AlignRight" << "AlignHCenter" << "AlignJustify";
	add(new KexiProperty("hAlign", value, list, descList(list), i18n("Horizontal alignment")));

	list.clear();

	if(!keys.grep("AlignTop").empty())
		value = "AlignTop";
	else if(!keys.grep("AlignBottom").empty())
		value = "AlignBottom";
	else
		value = "AlignVCenter";
	kdDebug() << "Vet value is " << value << endl;

	list << "AlignTop" << "AlignVCenter" << "AlignBottom";
	add(new KexiProperty("vAlign", value, list, descList(list), i18n("Vertical Alignment")));

	add(new KexiProperty("wordbreak", QVariant(false, 3), i18n("Word Break")));
}

void
ObjectPropertyBuffer::saveAlignProperty()
{
	QStrList list;
	list.append( (*this)["hAlign"]->value().toString().latin1() );
	list.append( (*this)["vAlign"]->value().toString().latin1() );
	if( (*this)["wordbreak"]->value().toBool() )
		list.append("WordBreak");
	kdDebug() << "alignemnt " << QStringList::fromStrList(list).join("|") << endl;
	int count = m_object->metaObject()->findProperty("alignment", true);
	const QMetaProperty *meta = m_object->metaObject()->property(count, true);
	m_object->setProperty("alignment", meta->keysToValue(list));
}

void
ObjectPropertyBuffer::createLayoutProperty(Container *container)
{
	QStringList list;
	QString value;

	switch(container->layoutType())
	{
		case Container::NoLayout:
		{
			value = "NoLayout";
			break;
		}
		case Container::HBox:
		{
			value = "HBox";
			break;
		}
		case Container::VBox:
		{
			value = "VBox";
			break;
		}
		case Container::Grid:
		{
			value = "Grid";
			break;
		}
	}

	list << "NoLayout" << "HBox" << "VBox" << "Grid";
	add(new KexiProperty("layout", value, list, descList(list), i18n("Container's layout")));
}

void
ObjectPropertyBuffer::saveLayoutProperty(const QString &value)
{
	Container *cont=0;
	if(m_manager->activeForm())
		cont = m_manager->activeForm()->objectTree()->lookup(m_object->name())->container();
	else
	{
		kdDebug() << "ERROR NO CONTAINER" << endl;
		return;
	}

	if(value == "NoLayout")    cont->setLayout(Container::NoLayout);
	if(value == "HBox")        cont->setLayout(Container::HBox);
	if(value == "VBox")        cont->setLayout(Container::VBox);
	if(value == "Grid")        cont->setLayout(Container::Grid);
}

ObjectPropertyBuffer::~ObjectPropertyBuffer()
{
}

}

#include "objpropbuffer.moc"
