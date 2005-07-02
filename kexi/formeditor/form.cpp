/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
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
#include "widgetpropertyset.h"
#include "formIO.h"
#include "formmanager.h"
#include "widgetlibrary.h"
#include "spring.h"
#include "pixmapcollection.h"
#include "events.h"
#include "utils.h"
#include <koproperty/property.h>

#include "form.h"

using namespace KFormDesigner;

FormPrivate::FormPrivate()
{
	toplevel = 0;
	topTree = 0;
	widget = 0;
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

//--------------------------------------

FormWidget::FormWidget()
{
}

//--------------------------------------

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

Form::~Form()
{
	delete d;
}

QWidget*
Form::widget() const
{
	if(d->topTree)
		return d->topTree->widget();
	else if(d->toplevel)
		return d->toplevel->widget();
	else // preview form
		return d->widget;
}


//////////////// Container -related functions ///////////////////////

void
Form::createToplevel(QWidget *container, FormWidget *formWidget, const QCString &)
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

	removeChildrenFromList(*list);

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

		d->widget = d->topTree->widget();
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

ResizeHandleSet*
Form::resizeHandlesForWidget(QWidget* w)
{
	return d->resizeHandles[w->name()];
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
		d->manager->emitWidgetSelected(this, true);
	else if(d->selected.first() != widget())
		d->manager->emitWidgetSelected(this, false);
	else
		d->manager->emitFormWidgetSelected(this);

	if(!withUndoAction)
		return;

	KAction *undoAction = d->collection->action("edit_undo");
	if(undoAction)
		d->manager->emitUndoEnabled(undoAction->isEnabled(), undoAction->text());

	KAction *redoAction = d->collection->action("edit_redo");
	if(redoAction)
		d->manager->emitRedoEnabled(redoAction->isEnabled(), redoAction->text());
}

void
Form::emitSelectionSignals()
{
	emit selectionChanged(selectedWidgets()->first(), false);
	for(QWidget *w = selectedWidgets()->next(); w; w = selectedWidgets()->next())
		emit selectionChanged(selectedWidgets()->first(), true);
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
Form::changeName(const QCString &oldname, const QCString &newname)
{
	if(oldname == newname)
		return;
	if(!d->topTree->rename(oldname, newname)) // rename failed
	{
		KMessageBox::sorry(widget()->topLevelWidget(),
			i18n("Renaming widget \"%1\" to \"%2\" failed.").arg(oldname).arg(newname));
//moved to WidgetPropertySet::slotChangeProperty()
//		KMessageBox::sorry(widget()->topLevelWidget(),
//		i18n("A widget with this name already exists. "
//			"Please choose another name or rename existing widget."));
		kdDebug() << "Form::changeName() : ERROR : A widget named " << newname << " already exists" << endl;
		d->manager->propertySet()->property("name") = QVariant(oldname);
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
		d->manager->emitUndoEnabled(undoAction->isEnabled(), undoAction->text());
}

void
Form::emitRedoEnabled()
{
	KAction *redoAction = d->collection->action("edit_redo");
	if(redoAction)
		d->manager->emitRedoEnabled(redoAction->isEnabled(), redoAction->text());
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
	if(!(w->focusPolicy() & QWidget::TabFocus))
	{
		if(!w->children())
		return;

		// For composed widgets, we check if one of the child can have focus
		QObjectList list = *(w->children());
		for(QObject *obj = list.first(); obj; obj = list.next())
		{
//			if(obj->isWidgetType() && (((QWidget*)obj)->focusPolicy() != QWidget::NoFocus)) {
//			if(obj->isWidgetType() && (((QWidget*)obj)->focusPolicy() & QWidget::TabFocus)) {
			if(obj->isWidgetType()) {//QWidget::TabFocus flag will be checked later!
				if(d->tabstops.findRef(c) == -1) {
					d->tabstops.append(c);
					return;
				}
			}
		}
	}
	else if(d->tabstops.findRef(c) == -1) // not yet in the list
		d->tabstops.append(c);
}

void
Form::updateTabStopsOrder()
{
	for (ObjectTreeListIterator it(d->tabstops);it.current();) {
		if(!(it.current()->widget()->focusPolicy() & QWidget::TabFocus)) {
			kexidbg << "Form::updateTabStopsOrder(): widget removed because has no TabFocus: " << it.current()->widget()->name() << endl;
			d->tabstops.remove( it.current() );
		}
		else
			++it;
	}
}

void
Form::autoAssignTabStops()
{
	VerWidgetList list;
	HorWidgetList hlist;

	foreach_list(ObjectTreeListIterator, it, d->tabstops) {
		if(it.current()->widget())
			list.append(it.current()->widget());
	}

	list.sort();
	d->tabstops.clear();

	/// We automatically sort widget from the top-left to bottom-right corner
	//! \todo Handle RTL layout (ie form top-right to bottom-left)
	foreach_list(WidgetListIterator, it, list) {
//	for(WidgetListIterator it(list); it.current() != 0; ++it)
		QWidget *w = it.current();
		hlist.append(w);

		++it;
		QWidget *nextw = it.current();
		while(nextw && (nextw->y() < (w->y() + 20))) {
			hlist.append(nextw);
			++it; nextw = it.current();
		}
		hlist.sort();

		for(WidgetListIterator it2(hlist); it2.current() != 0; ++it2) {
			ObjectTreeItem *tree = d->topTree->lookup(it2.current()->name());
			if(tree) {
				kdDebug() << "Form::autoAssignTabStops() adding " << tree->name() << endl;
				d->tabstops.append(tree);
			}
		}

		--it;
		hlist.clear();
	}
}

/*void
Form::resizeHandleDraggingStarted(QWidget *draggedWidget)
{
	WidgetFactory *wfactory = d->manager->lib()->factoryForClassName(draggedWidget->className());
	wfactory->resetEditor();
	draggedWidget
}*/

#include "form.moc"

