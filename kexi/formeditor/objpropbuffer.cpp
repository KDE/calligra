/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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

#include <qstringlist.h>
#include <qstrlist.h>
#include <qmetaobject.h>
#include <qvariant.h>
#include <qevent.h>
#include <qlayout.h>

#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include "objecttree.h"
#include "form.h"
#include "container.h"
#include "formmanager.h"
#include "spring.h"
#include "widgetlibrary.h"
#include "kexipropertyeditor.h"
#include "kexipropertyeditoritem.h"
#include "commands.h"

#include "objpropbuffer.h"

#include <kexi_utils.h> //Cedric: we need to add this header to KFD sources

using namespace KFormDesigner;

ObjectPropertyBuffer::ObjectPropertyBuffer(FormManager *manager, QObject *parent, const char *name)
 : KexiPropertyBuffer(parent, name)
{
	m_manager = manager;
	m_lastcom = 0;
	m_lastgeocom = 0;
	m_undoing = false;
	m_origActiveColors = 0;
	m_slotChangePropertyDisabled = false;

	connect(this, SIGNAL(propertyChanged(KexiPropertyBuffer&, KexiProperty&)), 
		this, SLOT(slotChangeProperty(KexiPropertyBuffer&, KexiProperty&)));
	connect(this, SIGNAL(propertyChanged(KexiPropertyBuffer&, KexiProperty&)), 
		m_manager, SIGNAL(propertyChanged(KexiPropertyBuffer&, KexiProperty&)));
	connect(this, SIGNAL(propertyReset(KexiPropertyBuffer&, KexiProperty&)), 
		this, SLOT(slotResetProperty(KexiPropertyBuffer&, KexiProperty&)));
	connect(this, SIGNAL(collectionItemChoosed(KexiPropertyBuffer &, KexiProperty &)), 
		this, SLOT(storePixmapName(KexiPropertyBuffer &, KexiProperty &)));
	connect(this, SIGNAL(propertyExecuted(KexiPropertyBuffer &, KexiProperty &, const QString&)), 
		this,  // TMP
		SLOT(slotPropertyExecuted(KexiPropertyBuffer &, KexiProperty &, const QString&)));
}

void
ObjectPropertyBuffer::slotChangeProperty(KexiPropertyBuffer &, KexiProperty &prop)
{
	if(m_slotChangePropertyDisabled || !m_manager || !m_manager->activeForm() 
		|| ! m_manager->activeForm()->objectTree())
		return;
	QCString property = prop.name();
	QVariant value = prop.value();
	kdDebug() << "ObjPropBuffer::changeProperty(): changing: " << property << endl;

	if(property == "name" && m_widgets.first()) {
//! @todo add to undo buffer
		QWidget *w = m_widgets.first();
		//also update widget's name in QObject member
		if (!Kexi::isIdentifier( value.toString() )) {
			KMessageBox::sorry( m_manager->activeForm()->widget(),
				i18n("Could not rename widget. ")
				+i18n("\"%1\" is not a valid name for a widget.\n").arg(value.toString())
				+i18n("Its name will be reverted to \"%1\"").arg(w->name()));
			m_slotChangePropertyDisabled = true;
			prop.resetValue();
			m_slotChangePropertyDisabled = false;
			return;
		}
		if (0!=m_manager->activeForm()->objectTree()->lookup(value.toString())) {
			KMessageBox::sorry( m_manager->activeForm()->widget(),
				i18n("Could not rename widget. ")
				+i18n("A widget with name \"%1\" already exists.\n").arg(value.toString())
				+i18n("Its name will be reverted to \"%1\"").arg(w->name()));
			m_slotChangePropertyDisabled = true;
			prop.resetValue();
			m_slotChangePropertyDisabled = false;
			return;
		}

		if(m_lastcom && m_lastcom->property() == property && !m_undoing)
			m_lastcom->setValue(value);
		else if(!m_undoing) // we are not already undoing -> avoid recursion
		{
			m_lastcom = new PropertyCommand(this, QString(w->name()), 
				w->property(property), value, property);
			m_slotChangePropertyDisabled = true;
//! @todo this enables undo action, but in Kexi form dialog need to be focused for proper work..
//!         -- this will be fixed by fixing shared actions
			m_manager->activeForm()->addCommand(m_lastcom, false);
			m_slotChangePropertyDisabled = false;
		}

		emit nameChanged(w->name(), value.toString());
		w->setName( value.toCString() );
		m_manager->activeForm()->emitActionSignals(true);

//		emit propertyChanged(w, property, value);
//todo		w->setName( value.toCString().utf8() );
	}
	else if(property == "paletteBackgroundPixmap")
		(*this)["backgroundOrigin"] = "WidgetOrigin";
	else if(property == "signals")
		return;
	else if((property == "hAlign") || (property == "vAlign") || (property == "wordbreak"))
	{
		saveAlignProperty(property);
	}
	else if((property == "layout") || (property == "layoutMargin") || (property == "layoutSpacing"))
	{
		saveLayoutProperty(property, value);
	}
	else if(property == "enabled")
	{
		for(QWidget *w = m_widgets.first(); w; w = m_widgets.next()) {
			ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(w->name());
			if(tree->isEnabled() == value.toBool())
				continue;

			QPalette p = w->palette();
			if (!m_origActiveColors)
				m_origActiveColors = new QColorGroup( p.active() );
			if (value.toBool()) {
				if (m_origActiveColors)
					p.setActive( *m_origActiveColors ); //revert
			}
			else {
				QColorGroup cg = p.disabled();
				//also make base color a bit disabled-like
				cg.setColor(QColorGroup::Base, cg.color(QColorGroup::Background));
				p.setActive(cg);
			}
			w->setPalette(p);

			tree->setEnabled(value.toBool());
			emit propertyChanged(w, property, value);
			return;
		}
		return;
	}
	else
	{
		if(!m_multiple) // one widget selected
		{
			// If the last command is the same, we just change its value
			if(m_lastcom && m_lastcom->property() == property && !m_undoing)
				m_lastcom->setValue(value);
			else if(!m_undoing) // we are not already undoing -> avoid recursion
			{
//				if(m_widgets.first() && ((m_widgets.first() != m_manager->activeForm()->widget()) || (property != "geometry"))) {
					m_lastcom = new PropertyCommand(this, QString(m_widgets.first()->name()), 
						m_widgets.first()->property(property), value, property);
					m_manager->activeForm()->addCommand(m_lastcom, false);
//				}
			}

			if (m_widgets.first()) {
				// If the property is changed, we add it in ObjectTreeItem modifProp
				ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(m_widgets.first()->name());
				if (tree) {
					if((*this)[property].changed())
						tree->addModifiedProperty(property, m_widgets.first()->property(property));

					m_widgets.first()->setProperty(property, value);
					emit propertyChanged(m_widgets.first(), property, value);
				}
			}
		}
		else
		{
			QWidget *w;
			if(m_lastcom && m_lastcom->property() == property && !m_undoing)
				m_lastcom->setValue(value);
			else if(!m_undoing)
			{
				// We store old values for each widget
				QMap<QString, QVariant> list;
				for(w = m_widgets.first(); w; w = m_widgets.next())
					list.insert(w->name(), w->property(property));

				m_lastcom = new PropertyCommand(this, list, value, property);
				m_manager->activeForm()->addCommand(m_lastcom, false);
			}

			for(w = m_widgets.first(); w; w = m_widgets.next())
			{
				ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(w->name());
				if((*this)[property].changed())
					tree->addModifiedProperty(property, w->property(property));

				w->setProperty(property, value);
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
		if(tree->modifiedProperties()->contains(prop.name()))
			w->setProperty(prop.name(), tree->modifiedProperties()->find(prop.name()).data());
	}
}

void
ObjectPropertyBuffer::slotPropertyExecuted(KexiPropertyBuffer &buf, KexiProperty &prop, const QString &value)
{
	if( (&buf != this) || (prop.name() != "signals") )
		return;

	m_manager->emitCreateSlot(m_widgets.first()->name(), value);
}

void
ObjectPropertyBuffer::setSelectedWidget(QWidget *w, bool add)
{
	if(!w)
	{
		resetBuffer();
		return;
	}

	if(m_widgets.count() == 0)
		add = false;

	if(!add || !m_widgets.count())
		setWidget(w);
	else
		addWidget(w);
}

void
ObjectPropertyBuffer::setWidget(QWidget *w)
{
	kdDebug() << "ObjectPropertyBuffer::setWidget()" << endl;

	if (m_propDesc.isEmpty()) {
		//most common property names
//! @todo perhaps a few of them shouldn't be translated within KFD mode, 
//!       to be more Qt Designer friendly?
		m_propDesc["name"] = i18n("Name");
		m_propDesc["paletteBackgroundPixmap"] = i18n("Background Pixmap");
		m_propDesc["enabled"] = i18n("Enabled");
		m_propDesc["geometry"] = i18n("Geometry");
		m_propDesc["font"] = i18n("Font");
		m_propDesc["cursor"] = i18n("Cursor");

		m_propValDesc["NoBackground"] = i18n("No Background");
		m_propValDesc["PaletteForeground"] = i18n("Palette Foreground");
		m_propValDesc["AutoText"] = i18n("Auto");
	}

	// We select parent TabWidget or WidgetStack instead of pages
	/*if(!m_manager->isTopLevel(w) && w->parentWidget() && w->parentWidget()->isA("QWidgetStack"))
	{
		w = w->parentWidget();
		if(w->parentWidget() && w->parentWidget()->inherits("QTabWidget"))
			w = w->parentWidget();
	}*/

	if(w == m_widgets.first() && !m_multiple)
		return;

	resetBuffer();
	m_widgets.append(w);

	QStrList pList = w->metaObject()->propertyNames(true);
	bool isTopLevel = m_manager->isTopLevel(w);

	if (!m_manager->activeForm() || !m_manager->activeForm()->objectTree())
			return;
	ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(w->name());
	if(!tree)
		return;

	int count = 0;
	WidgetInfo *winfo = m_manager->lib()->widgetInfoForClassName(w->className());
//	WidgetFactory *factory = winfo ? winfo->factory() : 0; //m_manager->lib()->factoryForClassName(w->className());

	//pList.sort();
	QStrListIterator it(pList);
	// We go through the list of properties
	for(; it.current() != 0; ++it)
	{
//		kdDebug() << "ObjectPropertyBuffer::setWidget(): property: " << *it << endl;
		count = w->metaObject()->findProperty(*it, true);
		const QMetaProperty *meta = w->metaObject()->property(count, true);
		const char* propertyName = meta->name();
		if(meta->designable(w) && !hasProperty(propertyName))
		{
			KexiProperty *newProp = 0;

			QString desc( m_propDesc[meta->name()] );
			if (desc.isEmpty()) {
				//try to get property description from factory
				desc = m_manager->lib()->propertyDescForName(winfo, propertyName);
			}
			
			if(meta->isEnumType())
			{
				QStrList keys = meta->enumKeys();
				if(0==qstrcmp(propertyName, "alignment"))
				{
					createAlignProperty(meta, w);
					continue;
				}

				kdDebug() << meta->valueToKey(w->property(propertyName).toInt()) << endl;
				add(newProp = new KexiProperty(propertyName,
					meta->valueToKey(w->property(propertyName).toInt()),
//					w->property(propertyName).toInt(),
					new KexiProperty::ListData(QStringList::fromStrList(keys),
						descList(winfo, QStringList::fromStrList(keys))),
					desc)
				);
			}
			else
				add(newProp = new KexiProperty(propertyName, w->property(propertyName), desc));

			if(!isPropertyVisible(propertyName, isTopLevel)) {
				newProp->setVisible(false);
			}
		}

//		if(0==qstrcmp(propertyName, "name"))
//			(*this)["name"].setAutoSync(0); // name should be updated only when pressing Enter

		if (winfo) {
			tristate autoSync = winfo->autoSyncForProperty( propertyName );
			if (! ~autoSync)
				(*this)[propertyName].setAutoSync( autoSync ); 
		}

		updateOldValue(tree, propertyName); // update the KexiProperty.oldValue using the value in modifProp
	}//for

	if (winfo)
		m_manager->lib()->setPropertyOptions(*this, *winfo, w);

	(*this)["name"].setAutoSync(false); // name should be updated only when pressing Enter

	(*this)["enabled"].setValue( QVariant(tree->isEnabled(), 3));

	if (m_manager->lib()->advancedPropertiesVisible()) {
		// add the signals property
		QStrList strlist = w->metaObject()->signalNames(true);
		QStrListIterator strIt(strlist);
		QStringList list;
		for(; strIt.current() != 0; ++strIt)
			list.append(*strIt);
		add(new KexiProperty("signals", "",
			new KexiProperty::ListData(list, descList(winfo, list)),
			i18n("Events")));
	}

	if(m_manager->activeForm())
	{
		ObjectTreeItem *objectIt = m_manager->activeForm()->objectTree()->lookup(w->name());
		if(objectIt && objectIt->container()) // we are a container -> layout property
			createLayoutProperty(objectIt->container());
	}

	m_manager->showPropertyBuffer(this);
	w->installEventFilter(this);

	connect(w, SIGNAL(destroyed()), this, SLOT(widgetDestroyed()));
}

void
ObjectPropertyBuffer::widgetDestroyed()
{
	m_widgets.clear();
	kdDebug() << "ObjecPropBuffer :: object is being destroyed, reseting m_widget " << endl;
}

void
ObjectPropertyBuffer::addWidget(QWidget *widg)
{
	m_multiple = true;
	if(m_widgets.find(widg) == -1)
		m_widgets.append(widg);

	// Reset some stuff
	m_lastcom = 0;
	m_lastgeocom = 0;
	m_properties.clear();

	QCString classn;
	if(m_widgets.first()->className() == widg->className())
		classn = m_widgets.first()->className();

	bool isTopLevel = m_manager->isTopLevel(widg);
	QAsciiDictIterator<KexiProperty> it(*this);
	for(; it.current(); ++it)
	{
		if(!isPropertyVisible(it.currentKey(), isTopLevel, classn))
			(*this)[it.currentKey()].setVisible(false);
	}

	m_manager->showPropertyBuffer(this);
}

void
ObjectPropertyBuffer::resetBuffer()
{
	checkModifiedProp();

	m_manager->showPropertyBuffer(0);

	m_widgets.clear();
	m_multiple = false;
	m_lastcom = 0;
	m_lastgeocom = 0;
	m_properties.clear();

//	m_manager->showPropertyBuffer(0);
	clear();

	if(!m_widgets.isEmpty())
	{
		m_widgets.first()->removeEventFilter(this);
		disconnect(m_widgets.first(), 0, this, 0);
	}
}

//ObjectPropertyBuffer::showProperty(const QString &property, bool isTopLevel, const QCString &classname)
bool
ObjectPropertyBuffer::isPropertyVisible(const QCString &property, bool isTopLevel, const QCString &classname)
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

	return m_manager->lib()->isPropertyVisible(m_widgets.first()->className(), m_widgets.first(), property, m_multiple);
}

bool
ObjectPropertyBuffer::eventFilter(QObject *o, QEvent *ev)
{
	if(o == m_widgets.first() && !m_multiple)
	{
		if((ev->type() == QEvent::Resize) || (ev->type() == QEvent::Move))
		{
			/*if(!(this->find("geometry")));
			{
				return false;
			}*/
			if((*this)["geometry"].value() == o->property("geometry")) // to avoid infinite recursion
				return false;

			(*this)["geometry"] = static_cast<QWidget*>(o)->geometry();
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
	if(m_widgets.first() && m_multiple)
	{
		if(!m_manager->activeForm())
			return;
		ObjectTreeItem *treeIt = m_manager->activeForm()->objectTree()->lookup(m_widgets.first()->name());
		if(treeIt)
		{
			QCString name;
			QAsciiDictIterator<KexiProperty> it(*this);
			for(; it.current(); ++it)
			{
				name = it.current()->name();
				if(it.current()->changed())
					treeIt->addModifiedProperty(name, it.current()->oldValue());
			}
		}
	}
}

void
ObjectPropertyBuffer::storePixmapName(KexiPropertyBuffer &buf, KexiProperty &prop)
{
	if((&buf != this) || m_multiple)
		return;

	ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(m_widgets.first()->name());
	if(tree)
		tree->addPixmapName(prop.name(), prop.option("pixmapName").toString());
}

// i18n functions /////////////////////////////////

QStringList
ObjectPropertyBuffer::descList(WidgetInfo *winfo, const QStringList &list)
{
	QStringList desc;
	for(QStringList::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it)
	{
		QString n( m_propValDesc[*it] );
		if (n.isEmpty()) { //try within factory and (maybe) parent factory
			if (winfo)
				n = m_manager->lib()->propertyDescForValue( winfo, (*it).latin1() );
			if (n.isEmpty())
				desc += *it; //untranslated
			else
				desc += n;
		}
		else
			desc += n;
	}
	return desc;
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

	QStrList *enumKeys = new QStrList(meta->enumKeys());
	QStringList possibleValues = QStringList::fromStrList(*enumKeys);
	delete enumKeys;

	ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(obj->name());

	if(!possibleValues.grep("AlignHCenter").empty())
	{
		// Create the horizontal alignment property
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

		list << "AlignAuto" << "AlignLeft" << "AlignRight" << "AlignHCenter" << "AlignJustify";
		add(new KexiProperty("hAlign", value,
			new KexiProperty::ListData(list, descList(0, list)), i18n("Horizontal Alignment")));
		updateOldValue(tree, "hAlign");
		list.clear();
	}

	if(!possibleValues.grep("AlignTop").empty())
	{
		// Create the ver alignment property
		if(!keys.grep("AlignTop").empty())
			value = "AlignTop";
		else if(!keys.grep("AlignBottom").empty())
			value = "AlignBottom";
		else
			value = "AlignVCenter";

		list << "AlignTop" << "AlignVCenter" << "AlignBottom";
		add(new KexiProperty("vAlign", value, new KexiProperty::ListData(list, descList(0, list)),
			i18n("Vertical Alignment")));
		updateOldValue(tree, "vAlign");
	}

	if(!possibleValues.grep("WordBreak").empty())
	{
		// Create the wordbreak property
		add(new KexiProperty("wordbreak", QVariant(false, 3), i18n("Word Break")));
		updateOldValue(tree, "wordbreak");
	}
}

void
ObjectPropertyBuffer::saveAlignProperty(const QString &property)
{
	if (!m_manager->activeForm())
		return;

	QStrList list;
	if( (*this)["hAlign"] )
		list.append( (*this)["hAlign"].value().toString().latin1() );
	if( (*this)["vAlign"] )
		list.append( (*this)["vAlign"].value().toString().latin1() );
	if( (*this)["wordbreak"] && (*this)["wordbreak"].value().toBool() )
		list.append("WordBreak");

	int count = m_widgets.first()->metaObject()->findProperty("alignment", true);
	const QMetaProperty *meta = m_widgets.first()->metaObject()->property(count, true);
	m_widgets.first()->setProperty("alignment", meta->keysToValue(list));

	if(m_lastcom && m_lastcom->property() == "alignment" && !m_undoing)
		m_lastcom->setValue(meta->keysToValue(list));
	else if(!m_undoing)
	{
		m_lastcom = new PropertyCommand(this, QString(m_widgets.first()->name()), m_widgets.first()->property("alignment"), meta->keysToValue(list), "alignment");
		m_manager->activeForm()->addCommand(m_lastcom, false);
	}

	ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(m_widgets.first()->name());
	if(tree && (*this)[property.latin1()].changed())
		tree->addModifiedProperty(property.latin1(), (*this)[property.latin1()].oldValue());
}

// Layout-related functions  //////////////////////////

void
ObjectPropertyBuffer::createLayoutProperty(Container *container)
{
	if (!m_manager->activeForm() || !m_manager->activeForm()->objectTree() || !container->widget())
		return;
	// special containers have no 'layout' property, as it should not be changed
	QCString className = container->widget()->className();
	if((className == "HBox") || (className == "VBox") || (className == "Grid"))
		return;

	QStringList list;
	QString value;

	value = Container::layoutTypeToString(container->layoutType());

	list << "NoLayout" << "HBox" << "VBox" << "Grid";

	KexiProperty *lyrProperty = new KexiProperty("layout", value, new KexiProperty::ListData(list, descList(0, list)),
		i18n("Container's Layout"));
	lyrProperty->setVisible( m_manager->lib()->advancedPropertiesVisible() );
	add( lyrProperty );
	
	ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(container->widget()->name());
	updateOldValue(tree, "layout");

	add(new KexiProperty("layoutMargin", container->layoutMargin(), i18n("Layout Margin")));
	updateOldValue(tree, "layoutMargin");
	if(container->layoutType() == Container::NoLayout)
		(*this)["layoutMargin"].setVisible(false);

	add(new KexiProperty("layoutSpacing", container->layoutSpacing(), i18n("Layout Spacing")));
	updateOldValue(tree, "layoutSpacing");
	if(container->layoutType() == Container::NoLayout)
		(*this)["layoutSpacing"].setVisible(false);

}

void
ObjectPropertyBuffer::saveLayoutProperty(const QString &prop, const QVariant &value)
{
	Container *cont=0;
	if(m_manager->activeForm() && m_manager->activeForm()->objectTree()) {
		cont = m_manager->activeForm()->objectTree()->lookup(m_widgets.first()->name())->container();
	}
	else
	{
		kdWarning() << "ERROR NO CONTAINER" << endl;
		return;
	}

	if(prop == "layout")
	{
		Container::LayoutType type = Container::stringToLayoutType(value.toString());

		if(m_lastcom && m_lastcom->property() == "layout" && !m_undoing)
			m_lastcom->setValue(value);
		else if(!m_undoing)
		{
			m_lastcom = new LayoutPropertyCommand(this, m_widgets.first()->name(), (*this)["layout"].oldValue(), value);
			m_manager->activeForm()->addCommand(m_lastcom, false);
		}

		cont->setLayout(type);
		bool show = type != Container::NoLayout;
		if(show != (*this)["layoutMargin"].isVisible())
		{
			(*this)["layoutMargin"].setVisible(show);
			(*this)["layoutSpacing"].setVisible(show);
			m_manager->showPropertyBuffer(this);
		}
		return;
	}

	if(m_lastcom && (QString(m_lastcom->property()) == prop) && !m_undoing)
		m_lastcom->setValue(value);
	else if(!m_undoing)
	{
		m_lastcom = new PropertyCommand(this, m_widgets.first()->name(), (*this)[prop.latin1()].oldValue(), value, prop.latin1());
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

	ObjectTreeItem *tree = m_manager->activeForm()->objectTree()->lookup(m_widgets.first()->name());
	if(tree && (*this)[prop.latin1()].changed())
		tree->addModifiedProperty(prop.latin1(), (*this)[prop.latin1()].oldValue());
}

void
ObjectPropertyBuffer::updateOldValue(ObjectTreeItem *tree, const char *property)
{
	KexiProperty &p = (*this)[property];
	if(p.isNull())
		return;
	QVariantMap::ConstIterator it( tree->modifiedProperties()->find(property) );
	if (it!=tree->modifiedProperties()->constEnd())
	{
		blockSignals(true);
		QVariant v = p.value();
		QVariant objpropvalue = it.data();
//		kdDebug() << "ObjectPropertyBuffer::updateOldValue(): v1: " << objpropvalue << " v2: " << v 
//			<< " p.listData()->keys[ objpropvalue.toInt() ]: " << (p.isFixedList() ? p.listData()->keys[ objpropvalue.toInt() ] : QVariant()) << endl;
		if (p.isFixedList())
/*! @todo What about "set" type? */
			// sometimes enums are remembered as casted to ints, sometimes as string representations...
			if (objpropvalue.type() == QVariant::Int)
				p.setValue( p.listData()->keys[ objpropvalue.toInt() ], false );
			else
				p.setValue( objpropvalue.toCString(), false );
		else
			p.setValue( objpropvalue, false );
		p.setValue(v, true);
		blockSignals(false);
	}
}

ObjectPropertyBuffer::~ObjectPropertyBuffer()
{
	delete m_origActiveColors;
}

#include "objpropbuffer.moc"
