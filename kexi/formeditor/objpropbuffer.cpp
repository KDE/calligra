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
#include <qlayout.h>

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
	m_widget = 0;
	m_manager = manager;
	m_lastcom = 0;
	m_lastgeocom = 0;
	m_undoing = false;

	connect(this, SIGNAL(propertyChanged(KexiPropertyBuffer&, KexiProperty&)), this, SLOT(slotChangeProperty(KexiPropertyBuffer&, KexiProperty&)));
	connect(this, SIGNAL(propertyReset(KexiPropertyBuffer&, KexiProperty&)), this, SLOT(slotResetProperty(KexiPropertyBuffer&, KexiProperty&)));
	connect(this, SIGNAL(collectionItemChoosed(KexiPropertyBuffer &, KexiProperty &)), this,
	    SLOT(storePixmapName(KexiPropertyBuffer &, KexiProperty &)));
	connect(this, SIGNAL(propertyExecuted(KexiPropertyBuffer &, KexiProperty &, const QString&)), this,  // TMP
	    SLOT(slotPropertyExecuted(KexiPropertyBuffer &, KexiProperty &, const QString&)));
}

void
ObjectPropertyBuffer::slotChangeProperty(KexiPropertyBuffer &, KexiProperty &prop)
{
	if(!m_manager || !m_manager->activeForm() || ! m_manager->activeForm()->objectTree())
		return;
	QString property = prop.name();
	QVariant value = prop.value();
	kdDebug() << "ObjPropBuffer::changeProperty(): changing: " << property << endl;

	if(property == "name")
		emit nameChanged(m_widget->name(), value.toString());
	else if(property == "paletteBackgroundPixmap")
		(*this)["backgroundOrigin"]->setValue("WidgetOrigin");

	if(property == "signals")
		return;
	if((property == "hAlign") || (property == "vAlign") || (property == "wordbreak"))
	{
		saveAlignProperty(property);
	}
	else if((property == "layout") || (property == "layoutMargin") || (property == "layoutSpacing"))
	{
		saveLayoutProperty(property, value);
	}
	else
	{
		if(!m_multiple) // one widget selected
		{
			// If the last command is the same, we just change its value
			if(m_lastcom && m_lastcom->property() == prop.name() && !m_undoing)
				m_lastcom->setValue(value);
			else if(!m_undoing) // we are not already undoing -> avoid recursion
			{
				m_lastcom = new PropertyCommand(this, QString(m_widget->name()), m_widget->property(property.latin1()), value, prop.name());
				m_manager->activeForm()->addCommand(m_lastcom, false);
			}

			// If the property is changed, we add it in ObjectTreeItem modifProp
			ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(m_widget->name());
			if((*this)[property.latin1()]->changed())
				tree->addModProperty(property, m_widget->property(property.latin1()));

			m_widget->setProperty(property.latin1(), value);
			emit propertyChanged(m_widget, property, value);
		}
		else
		{
			QWidget *w;
			if(m_lastcom && m_lastcom->property() == prop.name() && !m_undoing)
				m_lastcom->setValue(value);
			else if(!m_undoing)
			{
				// We store lod values for each widget
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
				emit propertyChanged(w, property, value);
			}
		}
	}
}

void
ObjectPropertyBuffer::slotResetProperty(KexiPropertyBuffer &, KexiProperty &prop)
{
	if(!m_multiple)
		return;

	// We use the old value in modifProp for each widget
	for(QWidget *w = m_widgets.first(); w; w = m_widgets.next())
	{
		ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(w->name());
		if(tree->modifProp()->contains(prop.name()))
			w->setProperty(prop.name(), tree->modifProp()->find(prop.name()).data());
	}
}

void
ObjectPropertyBuffer::slotPropertyExecuted(KexiPropertyBuffer &buf, KexiProperty &prop, const QString &value)
{
	if( (&buf != this) || (prop.name() != "signals") )
		return;

	m_manager->emitCreateSlot(m_widget->name(), value);
}

void
ObjectPropertyBuffer::setWidget(QWidget *widg)
{
	kdDebug() << "ObjectPropertyBuffer::setWidget()" << endl;

	QWidget *w = widg;
	// We select parent TabWidget or WidgetStack instead of pages
	if(!m_manager->isTopLevel(w) && w->parentWidget() && w->parentWidget()->isA("QWidgetStack"))
	{
		w = w->parentWidget();
		if(w->parentWidget() && w->parentWidget()->inherits("QTabWidget"))
			w = w->parentWidget();
	}

	if(w == m_widget && !m_multiple)
		return;

	m_widgets.clear();
	m_widgets.append(w);
	m_multiple = false;
	m_lastcom = 0;
	m_lastgeocom = 0;
	m_properties.clear();
	checkModifiedProp();
	kdDebug() << "loading object = " << widg->name() << endl;

	if(m_widget)
		m_widget->removeEventFilter(this);

	//luci, TODO: m_manager->editor()->reset(false);
	m_manager->showPropertyBuffer(0);
	clear();

	m_widget = w;
	QStrList pList = m_widget->metaObject()->propertyNames(true);
	bool isTopLevel = m_manager->isTopLevel(m_widget);

	int count = 0;
	QStrListIterator it(pList);
	// We go through the list of properties
	for(; it.current() != 0; ++it)
	{
		count = m_widget->metaObject()->findProperty(*it, true);
		const QMetaProperty *meta = m_widget->metaObject()->property(count, true);
		if(meta->designable(m_widget))
		{
			if(!showProperty(meta->name(), isTopLevel))
				continue;

			QString desc = descFromName(meta->name());
			if(meta->isEnumType())
			{
				QStrList keys = meta->enumKeys();
				if(QString(meta->name()) == QString("alignment"))
				{
					createAlignProperty(meta, m_widget);
					break;
				}
				else
				{
					QStringList values = descList(QStringList::fromStrList(keys));

					add(new KexiProperty(meta->name(), meta->valueToKey(m_widget->property(meta->name()).toInt()),
						QStringList::fromStrList(keys), values, desc));
				}
			}
			else
				add(new KexiProperty(meta->name(), m_widget->property(meta->name()), desc));
		}

		if(QString(meta->name()) == "name")
			(*this)["name"]->setAutoSync(0); // name should be updated only when pressing Enter
		if (!m_manager->activeForm() || !m_manager->activeForm()->objectTree())
			return;
		ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(widg->name());
		if(!tree)  return;
		updateOldValue(tree, meta->name()); // update the KexiProperty.oldValue using the value in modifProp
	}

	// add the signals property
	QStrList strlist = m_widget->metaObject()->signalNames(true);
	QStrListIterator strIt(strlist);
	QStringList list;
	for(; strIt.current() != 0; ++strIt)
		list.append(*strIt);
	add(new KexiProperty("signals", "", list, descList(list), i18n("Events")));

	if(m_manager->activeForm())
	{
		ObjectTreeItem *objectIt = m_manager->activeForm()->objectTree()->lookup(widg->name());
		if(objectIt && objectIt->container()) // we are a container -> layout property
			createLayoutProperty(objectIt->container());
	}

	m_manager->showPropertyBuffer(this);

	m_widget->installEventFilter(this);
	connect(m_widget, SIGNAL(destroyed()), this, SLOT(widgetDestroyed()));
}

void
ObjectPropertyBuffer::widgetDestroyed()
{
	m_widget = 0;
	kdDebug() << "ObjecPropBuffer :: object is being destroyed, reseting m_widget " << endl;
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
	if(m_widget->className() == widg->className())
		classn = m_widget->className();

	//luci, TODO change back: m_manager->editor()->clear();

	bool isTopLevel = m_manager->isTopLevel(widg);
	QAsciiDictIterator<KexiProperty> it(*this);
	for(; it.current(); ++it)
	{
		if(!showProperty(it.currentKey(), isTopLevel, classn))
			(*this)[it.currentKey()]->setVisible(false);
	}

	m_manager->showPropertyBuffer(this);
}

bool
ObjectPropertyBuffer::showProperty(const QString &property, bool isTopLevel, const QString &classname)
{
	if(!m_multiple)
	{
		if(m_properties.isEmpty())
		{
			if(!isTopLevel)
				m_properties << "caption" << "icon" << "sizeIncrement" << "iconText";
		} // we don't show these properties for a non-toplevel widget

		if(!(m_properties.grep(property)).isEmpty())
			return false;
	}
	else
	{
		if(m_properties.isEmpty())
		{
			m_properties << "font" << "paletteBackgroundColor" << "enabled" << "paletteForegroundColor"
			   << "cursor" << "paletteBackgroundPixmap";
		} // properties to show in multiple mode
		if(!(m_properties.grep(property)).isEmpty())
			return true;

		if(classname.isEmpty())
			return false;
	}

	return m_manager->lib()->showProperty(m_widget->className(), m_widget, property, m_multiple);
}

bool
ObjectPropertyBuffer::eventFilter(QObject *o, QEvent *ev)
{
	if(o==m_widget && !m_multiple)
	{
		if((ev->type() == QEvent::Resize) || (ev->type() == QEvent::Move))
		{
			/*if(!(this->find("geometry")));
			{
				return false;
			}*/
			if((*this)["geometry"]->value() == o->property("geometry")) // to avoid infinite recursion
				return false;

			(*this)["geometry"]->setValue(((QWidget*)o)->geometry());
		}
	}
	else if(m_multiple && ev->type() == QEvent::Move) // the widget is being moved, we update the property
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
	if(m_widget && m_multiple)
	{
		if(!m_manager->activeForm())
			return;
		ObjectTreeItem *treeIt = m_manager->activeForm()->objectTree()->lookup(m_widget->name());
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

void
ObjectPropertyBuffer::storePixmapName(KexiPropertyBuffer &buf, KexiProperty &prop)
{
	if((&buf != this) || m_multiple)
		return;

	ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(m_widget->name());
	if(tree)
		tree->addPixmapName(prop.name(), prop.pixmapName());
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
ObjectPropertyBuffer::createAlignProperty(const QMetaProperty *meta, QWidget *obj)
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
	updateOldValue(tree, "hAlign");
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
	updateOldValue(tree, "vAlign");

	// Create the wordbreak property
	add(new KexiProperty("wordbreak", QVariant(false, 3), i18n("Word Break")));
	updateOldValue(tree, "wordbreak");
}

void
ObjectPropertyBuffer::saveAlignProperty(const QString &property)
{
	if (!m_manager->activeForm())
		return;
	QStrList list;
	list.append( (*this)["hAlign"]->value().toString().latin1() );
	list.append( (*this)["vAlign"]->value().toString().latin1() );
	if( (*this)["wordbreak"]->value().toBool() )
		list.append("WordBreak");

	int count = m_widget->metaObject()->findProperty("alignment", true);
	const QMetaProperty *meta = m_widget->metaObject()->property(count, true);
	m_widget->setProperty("alignment", meta->keysToValue(list));

	if(m_lastcom && m_lastcom->property() == "alignment" && !m_undoing)
		m_lastcom->setValue(meta->keysToValue(list));
	else if(!m_undoing)
	{
		m_lastcom = new PropertyCommand(this, QString(m_widget->name()), m_widget->property("alignment"), meta->keysToValue(list), "alignment");
		m_manager->activeForm()->addCommand(m_lastcom, false);
	}

	ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(m_widget->name());
	if(tree && (*this)[property.latin1()]->changed())
		tree->addModProperty(property, (*this)[property.latin1()]->oldValue());
}

// Layout-related functions  //////////////////////////

void
ObjectPropertyBuffer::createLayoutProperty(Container *container)
{
	if (!m_manager->activeForm() || !m_manager->activeForm()->objectTree())
		return;
	QStringList list;
	QString value;

	value = Container::layoutTypeToString(container->layoutType());

	list << "NoLayout" << "HBox" << "VBox" << "Grid";

	add(new KexiProperty("layout", value, list, descList(list), i18n("Container's layout")));

	ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(container->widget()->name());
	updateOldValue(tree, "layout");

	add(new KexiProperty("layoutMargin", container->layoutMargin(), i18n("Layout margin")));
	updateOldValue(tree, "layoutMargin");
	if(container->layoutType() == Container::NoLayout)
		(*this)["layoutMargin"]->setVisible(false);

	add(new KexiProperty("layoutSpacing", container->layoutSpacing(), i18n("Layout spacing")));
	updateOldValue(tree, "layoutSpacing");
	if(container->layoutType() == Container::NoLayout)
		(*this)["layoutSpacing"]->setVisible(false);

}

void
ObjectPropertyBuffer::saveLayoutProperty(const QString &prop, const QVariant &value)
{
	Container *cont=0;
	if(m_manager->activeForm() && m_manager->activeForm()->objectTree()) {
		cont = m_manager->activeForm()->objectTree()->lookup(m_widget->name())->container();
	}
	else
	{
		kdDebug() << "ERROR NO CONTAINER" << endl;
		return;
	}

	if(prop == "layout")
	{
		Container::LayoutType type = Container::stringToLayoutType(value.toString());

		if(m_lastcom && m_lastcom->property() == "layout" && !m_undoing)
			m_lastcom->setValue(value);
		else if(!m_undoing)
		{
			m_lastcom = new LayoutPropertyCommand(this, m_widget->name(), (*this)["layout"]->oldValue(), value);
			m_manager->activeForm()->addCommand(m_lastcom, false);
		}

		cont->setLayout(type);
		bool show = !(type == Container::NoLayout);
		if(show != (*this)["layoutMargin"]->isVisible())
		{
			(*this)["layoutMargin"]->setVisible(show);
			(*this)["layoutSpacing"]->setVisible(show);
			m_manager->showPropertyBuffer(this);
		}
		return;
	}

	if(m_lastcom && (QString(m_lastcom->property()) == prop) && !m_undoing)
		m_lastcom->setValue(value);
	else if(!m_undoing)
	{
		m_lastcom = new PropertyCommand(this, m_widget->name(), (*this)[prop.latin1()]->oldValue(), value, prop.latin1());
		m_manager->activeForm()->addCommand(m_lastcom, false);
	}

	if(prop == "layoutMargin" && cont->layout())
	{
		cont->setLayoutMargin(value.toInt());
		cont->layout()->setMargin(value.toInt());
	}
	else if(prop == "layoutSpacing" && cont->layout())
	{
		cont->setLayoutSpacing(value.toInt());
		cont->layout()->setSpacing(value.toInt());
	}

	ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(m_widget->name());
	if(tree && (*this)[prop.latin1()]->changed())
		tree->addModProperty(prop, (*this)[prop.latin1()]->oldValue());
}

void
ObjectPropertyBuffer::updateOldValue(ObjectTreeItem *tree, const char *property)
{
	if(tree->modifProp()->contains(property))
	{
		if(!(*this)[property])
			return;
		blockSignals(true);
		QVariant v = (*this)[property]->value();
		(*this)[property]->setValue( tree->modifProp()->find(property).data() , false);
		(*this)[property]->setValue(v, true);
		blockSignals(false);
	}
}

ObjectPropertyBuffer::~ObjectPropertyBuffer()
{
}

}

#include "objpropbuffer.moc"
