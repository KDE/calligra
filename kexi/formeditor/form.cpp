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

namespace KFormDesigner {

FormPrivate::FormPrivate()
{
	toplevel = 0;
	topTree = 0;
	cursors = 0;
	resizeHandles.setAutoDelete(true);
	dirty = false;
	interactive = true;
	design = true;
	autoTabstops = false;
	tabstops.setAutoDelete(false);
	connBuffer = new ConnectionBuffer();
}

FormPrivate::~FormPrivate()
{
	delete history;
	delete topTree;
	delete connBuffer;
	connBuffer = 0;
	resizeHandles.setAutoDelete(false);
	// otherwise, it tries to delete widgets which doesn't exist anymore
}

Form::Form(FormManager *manager, const char *name)
  : QObject(manager, name)
{
	d = new FormPrivate();
	d->manager = manager;

	// Init actions
	d->collection = new KActionCollection(0, this);
	d->history = new KCommandHistory(d->collection, true);
	connect(d->history, SIGNAL(commandExecuted()), this, SLOT(slotCommandExecuted()));
	connect(d->history, SIGNAL(documentRestored()), this, SLOT(slotFormRestored()));
}

QWidget*
Form::widget() const
{
	if(d->topTree)
		return d->topTree->widget();
	else if(d->toplevel)
		return d->toplevel->widget();
	else
		return 0;
}


//////////////// Container -related functions ///////////////////////

void
Form::createToplevel(QWidget *container, FormWidget *formWidget, const QString &)
{
	kdDebug() << "Form::createToplevel() container= "<< (container ? container->name() : "<NULL>")
		<< " formWidget=" << formWidget << "className=" << name() << endl;

	d->formWidget = formWidget;
	d->toplevel = new Container(0, container, this, name());
	d->topTree = new ObjectTree(i18n("Form"), container->name(), container, d->toplevel);
	d->toplevel->setObjectTree(d->topTree);
	d->toplevel->setForm(this);
	d->pixcollection = new PixmapCollection(container->name(), this);

	d->topTree->setWidget(container);
	d->topTree->addModifiedProperty("caption", name());
	//m_topTree->addModifiedProperty("icon");

	connect(container, SIGNAL(destroyed()), this, SLOT(formDeleted()));

	kdDebug() << "Form::createToplevel(): d->toplevel=" << d->toplevel << endl;
}


Container*
Form::activeContainer()
{
	ObjectTreeItem *it;
	if(d->selected.count() == 0)
		return d->toplevel;

	if(d->selected.count() == 1)
		it = d->topTree->lookup(d->selected.last()->name());
	else
		it = commonParentContainer( &(d->selected) );

	if (!it)
		return 0;
	if(it->container())
		return it->container();
	else
		return it->parent()->container();
}

ObjectTreeItem*
Form::commonParentContainer(WidgetList *wlist)
{
	ObjectTreeItem *item = 0;
	WidgetList *list = new WidgetList();

	// Creates a list of all widget parents
	for(QWidget *w = wlist->first(); w; w = wlist->next())
	{
		if(list->findRef(w->parentWidget()) == -1)
			list->append(w->parentWidget());
	}

	// For each widget in the list, we check if it is parent of one of the other widget, which we can remove then
	for(QWidget *w = list->first(); w; w = list->next())
	{
		QWidget *widg;
		for(widg = list->first(); widg; widg = list->next())
		{
			if((w != widg) && (w->child(widg->name()))) // == widg is a child of w
			{
				kdDebug() << "Removing the widget " << widg->name() << "which is a child of " << w->name() << endl;
				list->remove(widg);
			}
		}

		widg = list->first();
		while(widg != w)
			widg = list->next();
	}

	// one widget remains == the container we are looking for
	if(list->count() == 1)
		item = d->topTree->lookup(list->first()->name());
	else // we need to go one level up
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
	//	it = d->topTree->lookup(d->selected.last()->name());
	//else
	it = d->topTree->lookup(w->name());

	if(it->parent()->container())
		return it->parent()->container();
	else
		return it->parent()->parent()->container();
}



void
Form::setDesignMode(bool design)
{
	d->design = design;
	if(!design)
	{
		ObjectTreeDict *dict = new ObjectTreeDict( *(d->topTree->dict()) );
		ObjectTreeDictIterator it(*dict);
		for(; it.current(); ++it)
			d->manager->lib()->previewWidget(it.current()->widget()->className(), it.current()->widget(), d->toplevel);
		delete dict;

		delete (d->topTree);
		d->topTree = 0;
		delete (d->toplevel);
		d->toplevel = 0;
	}
}


///////////////////////////// Selection stuff ///////////////////////

void
Form::setSelectedWidget(QWidget *w, bool add, bool dontRaise)
{
	if((d->selected.isEmpty()) || (w == widget()) || (d->selected.first() == widget()))
		add = false;

	if(!w)
	{
		setSelectedWidget(widget());
		return;
	}

	//raise selected widget and all possible parents
	QWidget *wtmp = w;
	while(!dontRaise && wtmp && wtmp->parentWidget() && (wtmp != widget()))
	{
		wtmp->raise();
		if(d->resizeHandles[ wtmp->name() ])
			d->resizeHandles[ wtmp->name() ]->raise();
		wtmp = wtmp->parentWidget();
	}

	if (wtmp)
		wtmp->setFocus();

	if(!add)
	{
		d->selected.clear();
		d->resizeHandles.clear();
	}
	d->selected.append(w);
	emit selectionChanged(w, add);
	emitActionSignals(false);

	// WidgetStack and TabWidget pages widgets shouldn't have resize handles, but their parent
	if(!d->manager->isTopLevel(w) && w->parentWidget() && w->parentWidget()->isA("QWidgetStack"))
	{
		w = w->parentWidget();
		if(w->parentWidget() && w->parentWidget()->inherits("QTabWidget"))
			w = w->parentWidget();
	}

	if(w && w != widget())
		d->resizeHandles.insert(w->name(), new ResizeHandleSet(w, this));
}

void
Form::unSelectWidget(QWidget *w)
{
	d->selected.remove(w);
	d->resizeHandles.remove(w->name());
}

void
Form::resetSelection()
{
	setSelectedWidget(widget(), false);
}

void
Form::emitActionSignals(bool withUndoAction)
{
	// Update menu and toolbar items
	if(d->selected.count() > 1)
		emit d->manager->widgetSelected(this, true);
	else if(d->selected.first() != widget())
		emit d->manager->widgetSelected(this, false);
	else
		emit d->manager->formWidgetSelected(this);

	if(!withUndoAction)
		return;

	KAction *undoAction = d->collection->action("edit_undo");
	if(undoAction)
		emit d->manager->undoEnabled(undoAction->isEnabled(), undoAction->text());

	KAction *redoAction = d->collection->action("edit_redo");
	if(redoAction)
		emit d->manager->redoEnabled(redoAction->isEnabled(), redoAction->text());
}

///////////////////////////  Various slots and signals /////////////////////
void
Form::formDeleted()
{
	d->manager->deleteForm(this);
	//delete this;
	deleteLater();
}

void
Form::changeName(const QString &oldname, const QString &newname)
{
	if(oldname == newname)
		return;
	if(!d->topTree->rename(oldname, newname)) // rename failed
	{
		KMessageBox::sorry(widget()->topLevelWidget(),
		i18n("A widget with this name already exists. "
			"Please choose another name or rename existing widget."));
		kdDebug() << "Form::changeName() : ERROR : A widget named " << newname << " already exists" << endl;
		(*(d->manager->buffer()))["name"] = oldname;
	}
	else
	{
		d->connBuffer->fixName(oldname, newname);
		ResizeHandleSet *temp = d->resizeHandles.take(oldname);
		d->resizeHandles.insert(newname, temp);
	}

}

void
Form::emitChildAdded(ObjectTreeItem *item)
{
	addWidgetToTabStops(item);
	emit childAdded(item);
}

void
Form::emitChildRemoved(ObjectTreeItem *item)
{
	d->tabstops.remove(item);
	if(d->connBuffer)
		d->connBuffer->removeAllConnectionsForWidget(item->name());
	emit childRemoved(item);
}

void
Form::addCommand(KCommand *command, bool execute)
{
	emit d->manager->dirty(this, true);
	d->dirty = true;
	d->history->addCommand(command, execute);
	if(!execute) // simulate command to activate 'undo' menu
		slotCommandExecuted();
}

void
Form::slotCommandExecuted()
{
	emit d->manager->dirty(this, true);
	d->dirty = true;
	// because actions text is changed after the commandExecuted() signal is emitted
	QTimer::singleShot(10, this, SLOT(emitUndoEnabled()));
	QTimer::singleShot(10, this, SLOT(emitRedoEnabled()));
}

void
Form::emitUndoEnabled()
{
	KAction *undoAction = d->collection->action("edit_undo");
	if(undoAction)
		emit d->manager->undoEnabled(undoAction->isEnabled(), undoAction->text());
}

void
Form::emitRedoEnabled()
{
	KAction *redoAction = d->collection->action("edit_redo");
	if(redoAction)
		emit d->manager->redoEnabled(redoAction->isEnabled(), redoAction->text());
}

void
Form::slotFormRestored()
{
	emit d->manager->dirty(this, false);
	d->dirty = false;
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
				if(d->tabstops.findRef(c) == -1)
				{
					d->tabstops.append(c);
					return;
				}
			}
		}
	}

	if(d->tabstops.findRef(c) == -1) // not yet in the list
		d->tabstops.append(c);
}

void
Form::autoAssignTabStops()
{
	VerWidgetList list;
	HorWidgetList hlist;

	for(ObjectTreeItem *tree = d->tabstops.first(); tree; tree = d->tabstops.next())
	{
		if(tree->widget())
			list.append(tree->widget());
	}

	list.sort();
	d->tabstops.clear();

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
			ObjectTreeItem *tree = d->topTree->lookup(widg->name());
			if(tree)
				d->tabstops.append(tree);
		}

		nextw = list.prev();
		hlist.clear();
	}
}

Form::~Form()
{
	delete d;
}

}

#include "form.moc"

