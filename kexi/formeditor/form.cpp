/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
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

#include <qwidget.h>
#include <qtabwidget.h>

#include <kdebug.h>

#include "container.h"
#include "objecttree.h"
#include "objpropbuffer.h"
#include "formIO.h"
#include "formmanager.h"
#include "widgetlibrary.h"
#include "resizehandle.h"

#include "form.h"

namespace KFormDesigner {

Form::Form(FormManager *manager, const char *name)
  : QObject(manager, name)
{
	m_toplevel = 0;
	m_topTree = 0;
	m_manager = manager;
	m_resizeHandles.setAutoDelete(true);
	m_inter = true;
	m_design = true;
}

void
Form::createToplevel(QWidget *container, const QString &classname)
{
	kdDebug() << "Form::createToplevel()" << endl;

	m_toplevel = new Container(0, container, this, "form1");
	m_topTree = new ObjectTree(classname, container->name(), container, m_toplevel);
	m_toplevel->setObjectTree(m_topTree);
	m_toplevel->setForm(this);

	m_topTree->setWidget(container);
	m_topTree->addModProperty("caption", name());
	//m_topTree->addModProperty("icon");

	connect(container, SIGNAL(destroyed()), this, SLOT(formDeleted()));

	kdDebug() << "Form::createToplevel(): m_toplevel=" << m_toplevel << endl;
}

void
Form::setDesignMode(bool design)
{
	m_design = design;
	if(!design)
	{
		delete m_topTree;
		m_topTree = 0;
		delete m_toplevel;
		m_toplevel = 0;
	}
}

void
Form::setCurrentWidget(QWidget *w)
{
	if(!m_selected.find(w))
		w->raise();

	if(w)
	{
		m_selected.clear();
		m_selected.append(w);
		emit selectionChanged(w);
	}
	/*if(w->parentWidget()->inherits("QWidgetStack"))
	{
		w = w->parentWidget()->parentWidget();
	}*/

	m_resizeHandles.clear();
	if(w != m_toplevel->widget() && w)
		m_resizeHandles.insert(w->name(), new ResizeHandleSet(w));

}

void
Form::addSelectedWidget(QWidget *w)
{
	if(!w)
		return;

	m_selected.append(w);
	m_resizeHandles.insert(w->name(), new ResizeHandleSet(w));
	emit addedSelectedWidget(w);
}

void
Form::unSelectWidget(QWidget *w)
{
	m_selected.remove(w);
	m_resizeHandles.remove(w->name());
	if(m_selected.count() == 1)
		activeContainer()->setSelectedWidget(m_selected.first(), false);
}

void
Form::setSelWidget(QWidget *w)
{
	if(w->parentWidget()->inherits("QWidgetStack"))
	{
		w = w->parentWidget()->parentWidget();
	}

	Container *cont;
	ObjectTreeItem *item = m_topTree->lookup(w->name());
	if(item->container())
		cont = item->container();
	else
		cont = item->parent()->container();

	cont->setSelectedWidget(w, false);
}

void
Form::formDeleted()
{
	m_manager->deleteForm(this);
	delete this;
}

void
Form::changeName(const QString &oldname, const QString &newname)
{
	m_topTree->rename(oldname, newname);
}

void
Form::emitChildAdded(ObjectTreeItem *item)
{
	emit childAdded(item);
}

void
Form::emitChildRemoved(ObjectTreeItem *item)
{
	emit childRemoved(item);
}

Container*
Form::activeContainer()
{
	ObjectTreeItem *it = m_topTree->lookup(m_selected.last()->name());
	if (!it)
		return 0;
	if(it->container())
		return it->container();
	else
		return it->parent()->container();
}

Container*
Form::parentContainer(QWidget *w)
{
	ObjectTreeItem *it;
	if(!w)
		it = m_topTree->lookup(m_selected.last()->name());
	else
		it = m_topTree->lookup(w->name());

	if(it->parent()->container())
		return it->parent()->container();
	else
		return it->parent()->parent()->container();
}

void
Form::pasteWidget(QDomElement &widg, QPoint pos)
{
	if (!activeContainer())
		return;
	fixNames(widg);
	if(pos.isNull())
		widg = fixPos(widg);
	else
		widg = fixPos(widg, pos);
	m_inter = false;
	FormIO::loadWidget(activeContainer(), m_manager->lib(), widg);
	m_inter = true;
}

void
Form::fixNames(QDomElement el)
{
	QString wname;
	for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if((n.toElement().tagName() == "property") && (n.toElement().attribute("name") == "name"))
		{
			wname = n.toElement().text();
			if(m_topTree->lookup(wname))
			{
				bool ok;
				int num = wname.right(1).toInt(&ok, 10);
				if(ok)
					wname = wname.left(wname.length()-1) + QString::number(num+1);
				else
					wname += "2";

				n.removeChild(n.firstChild());
				QDomElement type = el.ownerDocument().createElement("string");
				QDomText valueE = el.ownerDocument().createTextNode(wname);
				type.appendChild(valueE);
				n.toElement().appendChild(type);
			}

		}
		if(n.toElement().tagName() == "widget")
		{
			fixNames(n.toElement());
		}
	}

}

QDomElement
Form::fixPos(QDomElement widg, QPoint newpos)
{
	QDomElement el = widg.cloneNode(true).toElement();
	QDomElement rect;
	for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if((n.toElement().tagName() == "property") && (n.toElement().attribute("name") == "geometry"))
		{
			rect = n.firstChild().toElement();
		}
	}

	QDomElement x = rect.namedItem("x").toElement();
	x.removeChild(x.firstChild());
	QDomText valueX = el.ownerDocument().createTextNode(QString::number(newpos.x()));
	x.appendChild(valueX);

	QDomElement y = rect.namedItem("y").toElement();
	y.removeChild(y.firstChild());
	QDomText valueY = el.ownerDocument().createTextNode(QString::number(newpos.y()));
	y.appendChild(valueY);

	return el;
}

QDomElement
Form::fixPos(QDomElement el)
{
	QDomElement rect;
	for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if((n.toElement().tagName() == "property") && (n.toElement().attribute("name") == "geometry"))
		{
			rect = n.firstChild().toElement();
		}
	}

	QDomElement x = rect.namedItem("x").toElement();
	QDomElement y = rect.namedItem("y").toElement();
	QDomElement w = rect.namedItem("width").toElement();
	QDomElement h = rect.namedItem("height").toElement();

	int rx = x.text().toInt();
	int ry = y.text().toInt();
	int rw = w.text().toInt();
	int rh = h.text().toInt();
	QRect r(rx, ry, rw, rh);

	QWidget *widg = m_toplevel->widget()->childAt(r.x()+6, r.y()+6, false);
	if(!widg)
		return el;

	while(widg->geometry() == r)
	{
		widg = m_toplevel->widget()->childAt(widg->x() + 16, widg->y() + 16, false);
		r.moveBy(10,10);
	}
	if(r == QRect(rx, ry, rw, rh))
		return el;
	else
		return fixPos(el, QPoint(r.x(), r.y()));

}

Form::~Form()
{
	if(m_topTree)
		delete m_topTree;
	m_resizeHandles.setAutoDelete(false); // otherwise, it tries to delete widgets which doesn't exist anymore
}

}

#include "form.moc"
