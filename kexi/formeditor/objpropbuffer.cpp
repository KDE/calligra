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
#include <qevent.h>

#include <klocale.h>

#include "objecttree.h"
#include "form.h"
#include "container.h"
#include "formmanager.h"
#include "spacer.h"
#include "widgetlibrary.h"
#include "kexipropertyeditor.h"
#include "kexipropertyeditoritem.h"
#include "commands.h"

#include "objpropbuffer.h"

namespace KFormDesigner {

ObjectPropertyBuffer::ObjectPropertyBuffer(FormManager *manager, QObject *parent, const char *name)
 : KexiPropertyBuffer(parent, name)
{
	m_object = 0;
	m_manager = manager;
	m_lastcom = 0;
	m_lastgeocom = 0;
	m_undoing = false;

	connect(this, SIGNAL(propertyChanged(KexiPropertyBuffer&, KexiProperty&)), this, SLOT(slotChangeProperty(KexiPropertyBuffer&, KexiProperty&)));
	connect(this, SIGNAL(propertyReset(KexiPropertyBuffer&, KexiProperty&)), this, SLOT(slotResetProperty(KexiPropertyBuffer&, KexiProperty&)));
}

void
ObjectPropertyBuffer::slotChangeProperty(KexiPropertyBuffer &buff, KexiProperty &prop)
{
	QString property = prop.name();
	QVariant value = prop.value();
	kdDebug() << "ObjPropBuffer::changeProperty(): changing: " << property << endl;

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
			if(m_lastcom && m_lastcom->property() == prop.name() && !m_undoing)
				m_lastcom->setValue(value);
			else if(!m_undoing)
			{
				m_lastcom = new PropertyCommand(this, QString(m_object->name()), m_object->property(property.latin1()), value, prop.name());
				m_manager->activeForm()->addCommand(m_lastcom, false);
			}

			ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(m_object->name());
			if((*this)[property.latin1()]->changed())
				tree->addModProperty(property, m_object->property(property.latin1()));

			m_object->setProperty(property.latin1(), value);
			emit propertyChanged(m_object, property, value);
		}
		else
		{
			QWidget *w;
			if(m_lastcom && m_lastcom->property() == prop.name() && !m_undoing)
				m_lastcom->setValue(value);
			else if(!m_undoing)
			{
				QMap<QString, QVariant> list;
				for(w = m_widgets.first(); w; w = m_widgets.next())
					list.insert(w->name(), w->property(property.latin1()));

				m_lastcom = new PropertyCommand(this, list, value, prop.name());
				m_manager->activeForm()->addCommand(m_lastcom, false);
			}

			for(w = m_widgets.first(); w; w = m_widgets.next())
			{
				ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(w->name());
				if((*this)[property.latin1()]->changed())
					tree->addModProperty(property, w->property(property.latin1()));

				w->setProperty(property.latin1(), value);
				emit propertyChanged((QObject*)w, property, value);
			}
		}
	}
}

void
ObjectPropertyBuffer::slotResetProperty(KexiPropertyBuffer &buff, KexiProperty &prop)
{
	if(!m_multiple)
		return;

	for(QWidget *w = m_widgets.first(); w; w = m_widgets.next())
	{
		ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(w->name());
		if(tree->modifProp()->contains(prop.name()))
			w->setProperty(prop.name(), tree->modifProp()->find(prop.name()).data());
	}
}

void
ObjectPropertyBuffer::setWidget(QWidget *widg)
{
	kdDebug() << "ObjectPropertyBuffer::setWidget()" << endl;
	QObject *obj = (QObject*)widg;

	if(obj==m_object && !m_multiple)
		return;

	m_widgets.clear();
	m_widgets.append(widg);
	m_multiple = false;
	m_lastcom = 0;
	m_lastgeocom = 0;
	m_properties.clear();
	checkModifiedProp();
	kdDebug() << "loading object = " << widg->name() << endl;

	if(m_object)
		m_object->removeEventFilter(this);

	//luci, TODO: m_manager->editor()->reset(false);
	m_manager->showPropertyBuffer(0);
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
			if(!showProperty(meta->name()))
				continue;

			QString desc = descFromName(meta->name());
			if(meta->isEnumType())
			{
				QStrList keys = meta->enumKeys();
				if(QString(meta->name()) == QString("alignment"))
				{
					createAlignProperty(meta, obj);
					break;
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

		if(QString(meta->name()) == "name")
			(*this)["name"]->setAutoSync(0);
		if (!m_manager->activeForm() || !m_manager->activeForm()->objectTree())
			return;
		ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(widg->name());
		if(!tree)  return;
		if(tree->modifProp()->contains(meta->name()))
		{
			blockSignals(true);
			QVariant v = (*this)[meta->name()]->value();
			(*this)[meta->name()]->setValue( tree->modifProp()->find(meta->name()).data() , false);
			(*this)[meta->name()]->setValue(v, true);
			blockSignals(false);
		}
	}

	if(m_manager->activeForm())
	{
		ObjectTreeItem *objectIt = m_manager->activeForm()->objectTree()->lookup(widg->name());
		if(objectIt && objectIt->container())
			createLayoutProperty(objectIt->container());
	}

	m_manager->showPropertyBuffer(this);

	obj->installEventFilter(this);
	connect(obj, SIGNAL(destroyed()), this, SLOT(widgetDestroyed()));
}

void
ObjectPropertyBuffer::widgetDestroyed()
{
	m_object = 0;
	kdDebug() << "ObjecPropBuffer :: object is being destroyed, reseting m_object " << endl;
}

void
ObjectPropertyBuffer::addWidget(QWidget *widg)
{
	m_multiple = true;
	if(m_widgets.find(widg) == -1)
		m_widgets.append(widg);

	m_lastcom = 0;
	m_lastgeocom = 0;
	m_properties.clear();
	QString classn;
	if(m_object->className() == widg->className())
		classn = m_object->className();

	//luci, TODO change back: m_manager->editor()->clear();

	QAsciiDictIterator<KexiProperty> it(*this);
	for(; it.current(); ++it)
	{
		if(!showProperty(it.currentKey(), classn))
			(*this)[it.currentKey()]->setVisible(false);
	}

	m_manager->showPropertyBuffer(this);
}

bool
ObjectPropertyBuffer::showProperty(const QString &property, const QString &classname)
{
	if(!m_multiple)
	{
		if(m_properties.isEmpty())
		{
			if(!m_manager->isTopLevel((QWidget*)m_object))
				m_properties << "caption" << "icon" << "sizeIncrement" << "iconText";
		}

		if(!(m_properties.grep(property)).isEmpty())
			return false;
	}
	else
	{
		if(m_properties.isEmpty())
		{
			m_properties << "font" << "paletteBackgroundColor" << "enabled" << "paletteForegroundColor"
			   << "cursor" << "paletteBackgroundPixmap";
		}
		if(!(m_properties.grep(property)).isEmpty())
			return true;

		if(classname.isEmpty())
			return false;
	}

	return m_manager->lib()->showProperty(m_object->className(), (QWidget*)m_object, property, m_multiple);
/*
	if(obj->isA("KFormDesigner::Spacer"))
		return Spacer::showProperty(property);
	return true;
*/
}
/*
bool
ObjectPropertyBuffer::showMultipleProperty(const QString &property, const QString &className)
{
//	if(className.isNull())
//	{
		QStringList list;
		list << "font" << "paletteBackgroundColor" << "enabled" << "paletteForegroundColor" << "cursor" << "paletteBackgroundPixmap";
		if(!(list.grep(property)).isEmpty())
			return true;
//	}
	return false;
	// TODO : Filter properties following class name (ie : "alignment" for labels or line edits ...) (maybe using WidgetFactory ?)
}*/

bool
ObjectPropertyBuffer::eventFilter(QObject *o, QEvent *ev)
{
	if(o==m_object && !m_multiple)
	{
		if((ev->type() == QEvent::Resize) || (ev->type() == QEvent::Move))
		{
//luci11			if((*this)["geometry"]->value() == o->property("geometry")) // to avoid infinite recursion
//				return false;

//luci11			(*this)["geometry"]->setValue(((QWidget*)o)->geometry());
		}
	}
	else if(m_multiple && ev->type() == QEvent::Move)
	{
		if(m_lastgeocom && !m_undoing)
			m_lastgeocom->setPos(static_cast<QMoveEvent*>(ev)->pos());
		else if(!m_undoing)
		{
			QStringList list;
			QWidget *w;
			for(w = m_widgets.first(); w; w = m_widgets.next())
				list.append(w->name());

			m_lastgeocom = new GeometryPropertyCommand(this, list, static_cast<QMoveEvent*>(ev)->oldPos());
			if (m_manager->activeForm())
				m_manager->activeForm()->addCommand(m_lastgeocom, false);
		}
	}
	return false;
}

void
ObjectPropertyBuffer::checkModifiedProp()
{
	if(m_object && m_multiple)
	{
		if(!m_manager->activeForm())
			return;
		ObjectTreeItem *treeIt = m_manager->activeForm()->objectTree()->lookup(m_object->name());
		if(treeIt)
		{
			QString name;
			QAsciiDictIterator<KexiProperty> it(*this);
			for(; it.current(); ++it)
			{
				name = it.current()->name();
				if(it.current()->changed())
					treeIt->addModProperty(name, it.current()->oldValue());
			}
		}
	}
}

// i18n functions /////////////////////////////////

QString
ObjectPropertyBuffer::descFromName(const QString &name)
{
	if(propDesc.isEmpty())
	{
		propDesc["name"] = i18n("Name");
		propDesc["paletteBackgroundPixmap"] = i18n("Background Pixmap");
		propDesc["enabled"] = i18n("Enabled");
		propDesc["geometry"] = i18n("Geometry");
		propDesc["font"] = i18n("Font");
		propDesc["cursor"] = i18n("Cursor");
	}

	if(propDesc.contains(name))
		return propDesc[name];
	else
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
	if(valueDesc.isEmpty())
	{
		valueDesc["NoBackground"] = i18n("No Background");
		valueDesc["PaletteForeground"] = i18n("Palette Foreground");
		valueDesc["AutoText"] = i18n("Auto");
	}

	if(valueDesc.contains(name))
		return valueDesc[name];
	else
		return name;
}

void
ObjectPropertyBuffer::addPropertyDescription(const char *property, const QString &desc)
{
	if(!propDesc.contains(property))
		propDesc[property] = desc;
}

void
ObjectPropertyBuffer::addValueDescription(const char *value, const QString &desc)
{
	if(!valueDesc.contains(value))
		valueDesc[value] = desc;
}


// Alignment-related functions /////////////////////////////

void
ObjectPropertyBuffer::createAlignProperty(const QMetaProperty *meta, QObject *obj)
{
	if (!m_manager->activeForm() || !m_manager->activeForm()->objectTree())
		return;
	QStringList list;
	QString value;
	QStringList keys = QStringList::fromStrList( meta->valueToKeys(obj->property("alignment").toInt()) );
	kdDebug() << "keys is " << keys.join("|") << endl;
	ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(obj->name());

	// Create the hor alignment property
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

	if(tree->modifProp()->contains("hAlign"))
	{
		blockSignals(true);
		QVariant v = (*this)["hAlign"]->value();
		(*this)["hAlign"]->setValue( tree->modifProp()->find("hAlign").data() , false);
		(*this)["hAlign"]->setValue(v, true);
		blockSignals(false);
	}
	list.clear();

	// Create the ver alignment property
	if(!keys.grep("AlignTop").empty())
		value = "AlignTop";
	else if(!keys.grep("AlignBottom").empty())
		value = "AlignBottom";
	else
		value = "AlignVCenter";
	kdDebug() << "Vet value is " << value << endl;

	list << "AlignTop" << "AlignVCenter" << "AlignBottom";
	add(new KexiProperty("vAlign", value, list, descList(list), i18n("Vertical Alignment")));
	if(tree->modifProp()->contains("vAlign"))
	{
		blockSignals(true);
		QVariant v = (*this)["vAlign"]->value();
		(*this)["vAlign"]->setValue( tree->modifProp()->find("vAlign").data() , false);
		(*this)["vAlign"]->setValue(v, true);
		blockSignals(false);
	}

	// Create the wordbreak property
	add(new KexiProperty("wordbreak", QVariant(false, 3), i18n("Word Break")));
	if(tree->modifProp()->contains("wordbreak"))
	{
		blockSignals(true);
		QVariant v = (*this)["wordbreak"]->value();
		(*this)["wordbreak"]->setValue( tree->modifProp()->find("wordbreak").data() , false);
		(*this)["wordbreak"]->setValue(v, true);
		blockSignals(false);
	}
}

void
ObjectPropertyBuffer::saveAlignProperty()
{
	if (!m_manager->activeForm())
		return;
	QStrList list;
	list.append( (*this)["hAlign"]->value().toString().latin1() );
	list.append( (*this)["vAlign"]->value().toString().latin1() );
	if( (*this)["wordbreak"]->value().toBool() )
		list.append("WordBreak");

	int count = m_object->metaObject()->findProperty("alignment", true);
	const QMetaProperty *meta = m_object->metaObject()->property(count, true);
	m_object->setProperty("alignment", meta->keysToValue(list));

	if(m_lastcom && m_lastcom->property() == "alignment" && !m_undoing)
		m_lastcom->setValue(meta->keysToValue(list));
	else if(!m_undoing)
	{
		m_lastcom = new PropertyCommand(this, QString(m_object->name()), m_object->property("alignment"), meta->keysToValue(list), "alignment");
		m_manager->activeForm()->addCommand(m_lastcom, false);
	}
}

// Layout-related functions  //////////////////////////

void
ObjectPropertyBuffer::createLayoutProperty(Container *container)
{
	if (!m_manager->activeForm() || !m_manager->activeForm()->objectTree())
		return;
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

	ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(container->widget()->name());
	if(tree->modifProp()->contains("layout"))
	{
		blockSignals(true);
		QVariant v = (*this)["layout"]->value();
		(*this)["layout"]->setValue( tree->modifProp()->find("layout").data() , false);
		(*this)["layout"]->setValue(v, true);
		blockSignals(false);
	}
}

void
ObjectPropertyBuffer::saveLayoutProperty(const QString &value)
{
	Container *cont=0;
	if(m_manager->activeForm() && m_manager->activeForm()->objectTree()) {
		cont = m_manager->activeForm()->objectTree()->lookup(m_object->name())->container();
	}
	else
	{
		kdDebug() << "ERROR NO CONTAINER" << endl;
		return;
	}

	Container::LayoutType type;
	if(value == "NoLayout")    type = Container::NoLayout;
	if(value == "HBox")        type = Container::HBox;
	if(value == "VBox")        type = Container::VBox;
	if(value == "Grid")        type = Container::Grid;

	if(m_lastcom && !m_undoing)
		m_lastcom->setValue(value);
	else if(!m_undoing)
	{
		m_lastcom = new LayoutPropertyCommand(this, m_object->name(), (*this)["layout"]->oldValue(), value);
		m_manager->activeForm()->addCommand(m_lastcom, false);
	}

	cont->setLayout(type);
}

ObjectPropertyBuffer::~ObjectPropertyBuffer()
{
}

}

#include "objpropbuffer.moc"
