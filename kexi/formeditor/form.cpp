/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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
#include <qlabel.h>
#include <qobjectlist.h>

#include <kdebug.h>
#include <klocale.h>
#include <kcommand.h>
#include <kaction.h>
#include <kmessagebox.h>

#include "container.h"
#include "objecttree.h"
#include "objpropbuffer.h"
#include "formIO.h"
#include "formmanager.h"
#include "widgetlibrary.h"
#include "spring.h"
#include "pixmapcollection.h"
#include "events.h"

#include "form.h"

using namespace KFormDesigner;

Form::Form(FormManager *manager, const char *name)
  : QObject(manager, name)
{
	m_toplevel = 0;
	m_topTree = 0;
	m_cursors = 0;
	m_manager = manager;
	m_resizeHandles.setAutoDelete(true);
	m_dirty = false;
	m_inter = true;
	m_design = true;
	m_autoTabstops = false;
	m_tabstops.setAutoDelete(false);
	m_connBuffer = new ConnectionBuffer();

	// Init actions
	m_collection = new KActionCollection(0, this);
	m_history = new KCommandHistory(m_collection, true);
	connect(m_history, SIGNAL(commandExecuted()), this, SLOT(slotCommandExecuted()));
	connect(m_history, SIGNAL(documentRestored()), this, SLOT(slotFormRestored()));
}


//////////////// Container -related functions ///////////////////////

void
Form::createToplevel(QWidget *container, FormWidget *formWidget, const QString &)
{
	kdDebug() << "Form::createToplevel() container= "<< (container ? container->name() : "<NULL>")
		<< " formWidget=" << formWidget << "className=" << name() << endl;

	m_formWidget = formWidget;
	m_toplevel = new Container(0, container, this, name());
	m_topTree = new ObjectTree(i18n("Form"), container->name(), container, m_toplevel);
	m_toplevel->setObjectTree(m_topTree);
	m_toplevel->setForm(this);
	m_pixcollection = new PixmapCollection(container->name(), this);

	m_topTree->setWidget(container);
	m_topTree->addModProperty("caption", name());
	//m_topTree->addModProperty("icon");

	connect(container, SIGNAL(destroyed()), this, SLOT(formDeleted()));

	kdDebug() << "Form::createToplevel(): m_toplevel=" << m_toplevel << endl;
}


Container*
Form::activeContainer()
{
	ObjectTreeItem *it;
	if(m_selected.count() == 0)
		return m_toplevel;

	if(m_selected.count() == 1)
		it = m_topTree->lookup(m_selected.last()->name());
	else
		it = commonParentContainer(&m_selected);

	if (!it)
		return 0;
	if(it->container())
		return it->container();
	else
		return it->parent()->container();
}

ObjectTreeItem*
Form::commonParentContainer(QtWidgetList *wlist)
{
	ObjectTreeItem *item = 0;
	QtWidgetList *list = new QtWidgetList();

	for(QWidget *w = wlist->first(); w; w = wlist->next())
	{
		if(list->findRef(w->parentWidget()) == -1)
			list->append(w->parentWidget());
	}

	for(QWidget *w = list->first(); w; w = list->next())
	{
		QWidget *widg;
		for(widg = list->first(); widg; widg = list->next())
		{
			if((w != widg) && (w->child(widg->name())))
			{
				kdDebug() << "Removing the widget " << widg->name() << "which is a child of " << w->name() << endl;
				list->remove(widg);
			}
		}

		widg = list->first();
		while(widg != w)
			widg = list->next();
	}

	if(list->count() == 1)
		item = m_topTree->lookup(list->first()->name());
	else
		item =  commonParentContainer(list);

	delete list;
	return item;
}

Container*
Form::parentContainer(QWidget *w)
{
	ObjectTreeItem *it;
	if(!w)
		return 0;
	//	it = m_topTree->lookup(m_selected.last()->name());
	//else
	it = m_topTree->lookup(w->name());

	if(it->parent()->container())
		return it->parent()->container();
	else
		return it->parent()->parent()->container();
}



void
Form::setDesignMode(bool design)
{
	m_design = design;
	if(!design)
	{
		TreeDict dict = *(m_topTree->dict());
		TreeDictIterator it(dict);
		for(; it.current(); ++it)
			m_manager->lib()->previewWidget(it.current()->widget()->className(), it.current()->widget(), m_toplevel);

		delete m_topTree;
		m_topTree = 0;
		delete m_toplevel;
		m_toplevel = 0;
	}
}


///////////////////////////// Selection stuff ///////////////////////

void
Form::setSelectedWidget(QWidget *w, bool add, bool dontRaise)
{
	if((m_selected.isEmpty()) || (w == m_topTree->widget()) || (m_selected.first() == m_topTree->widget()))
		add = false;

	if(!w)
	{
		setSelectedWidget(m_topTree->widget());
		return;
	}

	//raise selected widget and all possible parents
	QWidget *wtmp = w;
	while(!dontRaise && wtmp && wtmp->parentWidget() && (wtmp != m_topTree->widget()))
	{
		wtmp->raise();
		if(m_resizeHandles[ wtmp->name() ])
			m_resizeHandles[ wtmp->name() ]->raise();
		wtmp = wtmp->parentWidget();
	}

	if (wtmp)
		wtmp->setFocus();

	if(!add)
	{
		m_selected.clear();
		m_resizeHandles.clear();
	}
	m_selected.append(w);
	emit selectionChanged(w, add);
	emitActionSignals(false);

	// WidgetStack and TabWidget pages widgets shouldn't have resize handles, but their parent
	if(!m_manager->isTopLevel(w) && w->parentWidget() && w->parentWidget()->isA("QWidgetStack"))
	{
		w = w->parentWidget();
		if(w->parentWidget() && w->parentWidget()->inherits("QTabWidget"))
			w = w->parentWidget();
	}

	if(m_toplevel && w != m_toplevel->widget() && w)
		m_resizeHandles.insert(w->name(), new ResizeHandleSet(w, this));
}

void
Form::unSelectWidget(QWidget *w)
{
	m_selected.remove(w);
	m_resizeHandles.remove(w->name());
}

void
Form::resetSelection()
{
	setSelectedWidget(m_topTree->widget(), false);
}

void
Form::emitActionSignals(bool withUndoAction)
{
	// Update menu and toolbar items
	if(m_selected.count() > 1)
		emit m_manager->widgetSelected(this, true);
	else if(m_selected.first() != m_topTree->widget())
		emit m_manager->widgetSelected(this, false);
	else
		emit m_manager->formWidgetSelected(this);

	if(!withUndoAction)
		return;

	KAction *undoAction = m_collection->action("edit_undo");
	if(undoAction)
		emit m_manager->undoEnabled(undoAction->isEnabled(), undoAction->text());

	KAction *redoAction = m_collection->action("edit_redo");
	if(redoAction)
		emit m_manager->redoEnabled(redoAction->isEnabled(), redoAction->text());
}

///////////////////////////  Various slots and signals /////////////////////
void
Form::formDeleted()
{
	m_manager->deleteForm(this);
	//delete this;
	deleteLater();
}

void
Form::changeName(const QString &oldname, const QString &newname)
{
	if(oldname == newname)
		return;
	if(!m_topTree->rename(oldname, newname))
	{
		KMessageBox::sorry(m_toplevel->widget()->topLevelWidget(),
		i18n("A widget with this name already exists. "
			"Please choose another name or rename existing widget."));
		kdDebug() << "Form::changeName() : ERROR : A widget named " << newname << " already exists" << endl;
		(*(m_manager->buffer()))["name"]->setValue(oldname);
	}
	else
	{
		m_connBuffer->fixName(oldname, newname);
		ResizeHandleSet *temp = m_resizeHandles.take(oldname);
		m_resizeHandles.insert(newname, temp);
	}

}

void
Form::emitChildAdded(ObjectTreeItem *item)
{
	emit childAdded(item);
}

void
Form::emitChildRemoved(ObjectTreeItem *item)
{
	m_tabstops.remove(item);
	if(m_connBuffer)
		m_connBuffer->removeAllConnectionsForWidget(item->name());
	emit childRemoved(item);
}

void
Form::addCommand(KCommand *command, bool execute)
{
	emit m_manager->dirty(this, true);
	m_dirty = true;
	m_history->addCommand(command, execute);
	if(!execute) // simulate command to activate 'undo' menu
		slotCommandExecuted();
}

void
Form::slotCommandExecuted()
{
	emit m_manager->dirty(this, true);
	m_dirty = true;
	QTimer::singleShot(10, this, SLOT(emitUndoEnabled()));
	QTimer::singleShot(10, this, SLOT(emitRedoEnabled()));
}

void
Form::emitUndoEnabled()
{
	KAction *undoAction = m_collection->action("edit_undo");
	if(undoAction)
		emit m_manager->undoEnabled(undoAction->isEnabled(), undoAction->text());
}

void
Form::emitRedoEnabled()
{
	KAction *redoAction = m_collection->action("edit_redo");
	if(redoAction)
		emit m_manager->redoEnabled(redoAction->isEnabled(), redoAction->text());
}

void
Form::slotFormRestored()
{
	emit m_manager->dirty(this, false);
	m_dirty = false;
}


///////////////////////////  Tab stops ////////////////////////

void
Form::addWidgetToTabStops(ObjectTreeItem *c)
{
	QWidget *w = c->widget();
	if(!w)
		return;
	if(w->focusPolicy() == QWidget::NoFocus)
	{
		if(!w->children())
		return;

		// For composed widgets, we check if one of the child can have focus
		QObjectList list = *(w->children());
		for(QObject *obj = list.first(); obj; obj = list.next())
		{
			if(obj->isWidgetType() && (((QWidget*)obj)->focusPolicy() != QWidget::NoFocus))
			{
				if(m_tabstops.findRef(c) == -1)
				{
					m_tabstops.append(c);
					return;
				}
			}
		}
	}

	if(m_tabstops.findRef(c) == -1) // not yet in the list
		m_tabstops.append(c);
}

void
Form::autoAssignTabStops()
{
	VerWidgetList list;
	HorWidgetList hlist;

	for(ObjectTreeItem *tree = m_tabstops.first(); tree; tree = m_tabstops.next())
	{
		if(tree->widget())
			list.append(tree->widget());
	}

	list.sort();
	m_tabstops.clear();

	/// We automatically sort widget from the top-left to bottom-right corner
	//! \todo Handle RTL layout (ie form top-right to bottom-left)
	for(QWidget *w = list.first(); w; w = list.next())
	{
		hlist.append(w);

		QWidget *nextw = list.next();
		while(nextw && (nextw->y() < (w->y() + 20)))
		{
			hlist.append(nextw);
			nextw = list.next();
		}
		hlist.sort();

		for(QWidget *widg = hlist.first(); widg; widg = hlist.next())
		{
			ObjectTreeItem *tree = m_topTree->lookup(widg->name());
			if(tree)
				m_tabstops.append(tree);
		}

		nextw = list.prev();
		hlist.clear();
	}
}


///////////// Functions to paste widgets ///////////////////

void
Form::pasteWidget(QDomElement &widg, Container *cont, QPoint pos)
{
	Container *container = cont ? cont : activeContainer();
	if (!container)
		return;

	fixNames(widg); // to avoid name clash
	if(pos.isNull())
		widg = fixPos(widg, cont); // to avoid widget being at the same location
	else
		widg = fixPos(widg, pos);
	m_inter = false;
	FormIO::loadWidget(container, m_manager->lib(), widg);
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
			while(m_topTree->lookup(wname)) // name already exists
			{
				bool ok;
				int num = wname.right(1).toInt(&ok, 10);
				if(ok)
					wname = wname.left(wname.length()-1) + QString::number(num+1);
				else
					wname += "2";
			}
			if(wname != n.toElement().text()) // we change the name, so we recreate the element
			{
				n.removeChild(n.firstChild());
				QDomElement type = el.ownerDocument().createElement("string");
				QDomText valueE = el.ownerDocument().createTextNode(wname);
				type.appendChild(valueE);
				n.toElement().appendChild(type);
			}

		}
		if(n.toElement().tagName() == "widget") // fix child widgets names
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
	// Find the widget geometry if there is one
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
Form::fixPos(QDomElement el, Container *container)
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

	while((widg->geometry() == r) && (widg != 0))// there is already a widget there, with the same size
	{
		widg = m_toplevel->widget()->childAt(widg->x() + 16, widg->y() + 16, false);
		r.moveBy(10,10);
	}

	if(r.x() < 0)
		r.setX(0);
	else if(r.right() > container->widget()->width())
		r.setX(container->widget()->width() - r.width());

	if(r.y() < 0)
		r.setY(0);
	else if(r.bottom() > container->widget()->height())
		r.setY(container->widget()->height() - r.height());

	if(r == QRect(rx, ry, rw, rh))
		return el;
	else
		return fixPos(el, QPoint(r.x(), r.y()));

}

Form::~Form()
{
	delete m_history;
	delete m_topTree;
	delete m_connBuffer;
	m_connBuffer = 0;
	m_resizeHandles.setAutoDelete(false); // otherwise, it tries to delete widgets which doesn't exist anymore
}

#include "form.moc"

