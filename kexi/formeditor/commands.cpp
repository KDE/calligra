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
#include <qdom.h>
#include <qwidget.h>

#include <kdebug.h>
#include <klocale.h>

#include "formIO.h"
#include "container.h"
#include "objecttree.h"
#include "formmanager.h"
#include "form.h"
#include "objpropbuffer.h"
#include "kexiproperty.h"
#include "widgetlibrary.h"

#include "commands.h"

namespace KFormDesigner {

// PropertyCommand

PropertyCommand::PropertyCommand(ObjectPropertyBuffer *buf, const QString &wname, const QVariant &oldValue, const QVariant &value, const QString &property)
  : KCommand(), m_buffer(buf), m_name(wname), m_value(value), m_oldvalue(oldValue), m_property(property)
{}

void
PropertyCommand::setValue(const QVariant &value)
{
	m_value = value;
}

void
PropertyCommand::execute()
{
	m_buffer->m_undoing = true;
	QWidget *w = m_buffer->m_manager->activeForm()->objectTree()->lookup(m_name)->widget();
	m_buffer->m_manager->activeForm()->setSelWidget(w);
	m_buffer->setWidget(w);
	(*m_buffer)[m_property]->setValue(m_value);
	m_buffer->m_undoing = false;
}

void
PropertyCommand::unexecute()
{
	m_buffer->m_undoing = true;
	QWidget *w = m_buffer->m_manager->activeForm()->objectTree()->lookup(m_name)->widget();
	m_buffer->m_manager->activeForm()->setSelWidget(w);
	m_buffer->setWidget(w);
	(*m_buffer)[m_property]->setValue(m_oldvalue);
	m_buffer->m_undoing = false;
}

QString
PropertyCommand::name() const
{
	return i18n("Change %1 of widget %2" ).arg(m_property).arg(m_name);
}

// LayoutPropertyCommand

LayoutPropertyCommand::LayoutPropertyCommand(ObjectPropertyBuffer *buf, const QString &name, const QVariant &oldValue, const QVariant &value)
 : PropertyCommand(buf, name, oldValue, value, "layout")
{
	m_form = buf->m_manager->activeForm();
	Container *m_container = m_form->objectTree()->lookup(name)->container();
	for(ObjectTreeItem *it = m_container->tree()->children()->first(); it; it = m_container->tree()->children()->next())
		m_geometries.insert(it->name(), it->widget()->geometry());
}

void
LayoutPropertyCommand::execute()
{
	PropertyCommand::execute();
}

void
LayoutPropertyCommand::unexecute()
{
	Container *m_container = m_form->objectTree()->lookup(m_name)->container();
	m_container->setLayout(Container::NoLayout);
	for(QMap<QString,QRect>::Iterator it = m_geometries.begin(); it != m_geometries.end(); ++it)
	{
		ObjectTreeItem *tree = m_container->form()->objectTree()->lookup(it.key());
		if(tree)
			tree->widget()->setGeometry(it.data());
	}

	PropertyCommand::unexecute();
}

QString
LayoutPropertyCommand::name() const
{
	return i18n("Change layout of %1").arg(m_name);
}


// InsertWidgetCommand

InsertWidgetCommand::InsertWidgetCommand(Container *container, QPoint p)
  : KCommand(), m_point(p)
{
	m_containername = container->widget()->name();
	m_form = container->form();
	m_class = container->form()->manager()->insertClass();
	m_insertRect = container->m_insertRect;
}

void
InsertWidgetCommand::execute()
{
	Container *m_container = m_form->objectTree()->lookup(m_containername)->container();
	if(m_name.isEmpty())
		m_name = m_container->form()->objectTree()->genName(m_class);

	QWidget *w = m_container->form()->manager()->lib()->createWidget(m_class, m_container->m_container, m_name.latin1(), m_container);

	if(!w)
		return;
	m_widgetname = w->name();

	m_container->m_insertRect = m_insertRect;
	if(!m_container->m_insertRect.isValid())
		m_container->m_insertRect = QRect(m_point.x(), m_point.y(), w->sizeHint().width(), w->sizeHint().height());
	w->move(m_container->m_insertRect.x(), m_container->m_insertRect.y());
	w->resize(m_container->m_insertRect.width()-1, m_container->m_insertRect.height()-1);
	w->show();
	m_container->m_container->repaint();

	m_container->m_insertRect = QRect();
	m_container->form()->manager()->stopInsert();

	if (!m_container->form()->objectTree()->lookup(m_name))
		m_container->form()->objectTree()->addChild(m_container->m_tree, new ObjectTreeItem(m_class, m_name, w));

	Container::LayoutType type = m_container->layoutType();
	m_container->setLayout(Container::NoLayout);
	m_container->setLayout(type);
	kdDebug() << "Container::eventFilter(): widget added " << this << endl;
}

void
InsertWidgetCommand::unexecute()
{
	QWidget *m_widget = m_form->objectTree()->lookup(m_widgetname)->widget();
	Container *m_container = m_form->objectTree()->lookup(m_containername)->container();
	m_container->m_selected.clear();
	m_container->m_selected.append(m_widget);
	m_container->deleteItem();
}

QString
InsertWidgetCommand::name() const
{
	if(!m_name.isEmpty())
		return i18n("Insert %1").arg(m_name);
	else
		return i18n("Insert widget");
}

// PasteWidgetCommand

PasteWidgetCommand::PasteWidgetCommand(QDomDocument &domDoc, Container *container, QPoint p)
  : m_point(p)
{
	m_data = domDoc.toCString();
	m_containername = container->widget()->name();
	m_form = container->form();
}

void
PasteWidgetCommand::execute()
{
	Container *m_container = m_form->objectTree()->lookup(m_containername)->container();
	QString errMsg;
	int errLine;
	int errCol;
	QDomDocument domDoc("UI");
	bool parsed = domDoc.setContent(m_data, false, &errMsg, &errLine, &errCol);

	if(!parsed)
	{
		kdDebug() << "WidgetWatcher::load(): " << errMsg << endl;
		kdDebug() << "WidgetWatcher::load(): line: " << errLine << " col: " << errCol << endl;
		return;
	}

	if(!domDoc.namedItem("UI").hasChildNodes())
		return;
	if(domDoc.namedItem("UI").firstChild().nextSibling().isNull())
	{
		QDomElement widg = domDoc.namedItem("UI").firstChild().toElement();
		if(m_point.isNull())
			m_container->form()->pasteWidget(widg, m_container);
		else
			m_container->form()->pasteWidget(widg, m_container,  m_point);
	}
	else for(QDomNode n = domDoc.namedItem("UI").firstChild(); !n.isNull(); n = n.nextSibling())
	{
		QDomElement widg = n.toElement();
		m_container->form()->pasteWidget(widg, m_container);
	}

	m_names.clear();
	for(QDomNode n = domDoc.namedItem("UI").firstChild(); !n.isNull(); n = n.nextSibling())
	{
		for(QDomNode m = n.firstChild(); !m.isNull(); n = m.nextSibling())
		{
			if((m.toElement().tagName() == "property") && (m.toElement().attribute("name") == "name"))
			{
				m_names.append(m.toElement().text());
				break;
			}
		}
	}
}

void
PasteWidgetCommand::unexecute()
{
	Container *m_container = m_form->objectTree()->lookup(m_containername)->container();
	m_container->m_selected.clear();
	for(QStringList::Iterator it = m_names.begin(); it != m_names.end(); ++it)
	{
		QWidget *w = m_container->form()->objectTree()->lookup(*it)->widget();
		m_container->m_selected.append(w);
	}

	m_container->deleteItem();
}

QString
PasteWidgetCommand::name() const
{
	return i18n("Paste");
}

// DeleteWidgetCommand

DeleteWidgetCommand::DeleteWidgetCommand(WidgetList &list, Form *form)
 : KCommand(), m_form(form)
{
	for(QWidget *w = list.first(); w; w = list.next())
		m_names += w->name();

	m_domDoc = QDomDocument("UI");
	m_domDoc.appendChild(m_domDoc.createElement("UI"));

	QDomElement parent = m_domDoc.namedItem("UI").toElement();
	for(QStringList::Iterator it = m_names.begin(); it != m_names.end(); ++it)
	{
		ObjectTreeItem *item = m_form->objectTree()->lookup(*it);
		if (!item)
			return;
		m_containers.insert(item->name(), m_form->parentContainer(item->widget())->widget()->name());
		FormIO::saveWidget(item, parent, m_domDoc);
	}
}

void
DeleteWidgetCommand::execute()
{
	for(QStringList::Iterator it = m_names.begin(); it != m_names.end(); ++it)
	{
		ObjectTreeItem *item = m_form->objectTree()->lookup(*it);
		if (!item)
			continue;

		Container *cont = m_form->parentContainer(item->widget());
		cont->m_selected.clear();
		cont->m_selected.append(item->widget());
		cont->deleteItem();
	}
}

void
DeleteWidgetCommand::unexecute()
{
	QString wname;
	m_form->setInteractiveMode(false);
	for(QDomNode n = m_domDoc.namedItem("UI").firstChild(); !n.isNull(); n = n.nextSibling())
	{
		for(QDomNode m = n.firstChild(); !m.isNull(); n = m.nextSibling())
		{
			if((m.toElement().tagName() == "property") && (m.toElement().attribute("name") == "name"))
			{
				wname = m.toElement().text();
				break;
			}
		}

		Container *cont = m_form->objectTree()->lookup(m_containers[wname])->container();
		QDomElement widg = n.toElement();
		FormIO::loadWidget(cont, m_form->manager()->lib(), widg);
	}
	m_form->setInteractiveMode(true);
}

QString
DeleteWidgetCommand::name() const
{
	return i18n("Delete widget");
}

// CutWidgetCommand

CutWidgetCommand::CutWidgetCommand(WidgetList &list, Form *form)
 : DeleteWidgetCommand(list, form)
{}

void
CutWidgetCommand::execute()
{
	DeleteWidgetCommand::execute();
	m_data = m_form->manager()->m_domDoc.toCString();
	m_form->manager()->m_domDoc.setContent(m_domDoc.toCString());
}

void
CutWidgetCommand::unexecute()
{
	DeleteWidgetCommand::unexecute();
	m_form->manager()->m_domDoc.setContent(m_data);
}

QString
CutWidgetCommand::name() const
{
	return i18n("Cut");
}

}

