/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2007 Jarosław Staniek <staniek@kde.org>

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

#include <qwidget.h>
#include <qlabel.h>
#include <qobject.h>
#include <QMap>
#include <Q3CString>

#include <kdebug.h>
#include <klocale.h>
#include <k3command.h>
#include <kaction.h>
#include <kmessagebox.h>
#include <kactioncollection.h>

#include "container.h"
#include "objecttree.h"
#include "widgetpropertyset.h"
#include "formIO.h"
#include "formmanager.h"
#include "widgetlibrary.h"
#include "spring.h"
#include "events.h"
#include "utils.h"
#include "form.h"
#include <koproperty/property.h>
#include <kexiutils/utils.h>
#include <kexi_global.h>

#ifdef __GNUC__
#warning pixmapcollection
#endif
#define KEXI_NO_PIXMAPCOLLECTION
#ifndef KEXI_NO_PIXMAPCOLLECTION
#include "pixmapcollection.h"
#endif

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
    formatVersion = KFormDesigner::version();
    originalFormatVersion = KFormDesigner::version();
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
        : m_form(0)
{
}

FormWidget::~FormWidget()
{
    if (m_form) {
        m_form->setFormWidget(0);
    }
}

//--------------------------------------

Form::Form(WidgetLibrary* library, bool designMode)
        : QObject(library)
        , m_lib(library)
{
    d = new FormPrivate();
// d->manager = manager;
    d->design = designMode;

    // Init actions
    d->collection = new KActionCollection(this);
    d->history = new K3CommandHistory(d->collection, true);
    connect(d->history, SIGNAL(commandExecuted()), this, SLOT(slotCommandExecuted()));
    connect(d->history, SIGNAL(documentRestored()), this, SLOT(slotFormRestored()));
}

Form::~Form()
{
    emit destroying();
    delete d;
    d = 0;
}

QWidget*
Form::widget() const
{
    if (d->topTree)
        return d->topTree->widget();
    else if (d->toplevel)
        return d->toplevel->widget();
    else // preview form
        return d->widget;
}

//////////////// Container -related functions ///////////////////////

void
Form::createToplevel(QWidget *container, FormWidget *formWidget, const Q3CString &)
{
    kDebug() << "Form::createToplevel() container= " << (container ? container->objectName() : "<NULL>")
    << " formWidget=" << formWidget;

    setFormWidget(formWidget);
    d->toplevel = new Container(0, container, this);
    d->toplevel->setObjectName(objectName());
    d->topTree = new ObjectTree(i18n("Form"), container->objectName(), container, d->toplevel);
    d->toplevel->setObjectTree(d->topTree);
    d->toplevel->setForm(this);
#ifdef __GNUC__
#warning pixmapcollection
#endif
#ifndef KEXI_NO_PIXMAPCOLLECTION
    d->pixcollection = new PixmapCollection(container->objectName(), this);
#endif

    d->topTree->setWidget(container);
//! todo: copy caption in Kexi from object's caption
// d->topTree->addModifiedProperty("caption", name());
    //m_topTree->addModifiedProperty("icon");

    connect(container, SIGNAL(destroyed()), this, SLOT(formDeleted()));

    kDebug() << "Form::createToplevel(): d->toplevel=" << d->toplevel;
}


Container*
Form::activeContainer()
{
    ObjectTreeItem *it;
    if (d->selected.count() == 0)
        return d->toplevel;

    if (d->selected.count() == 1)
        it = d->topTree->lookup(d->selected.last()->objectName());
    else
        it = commonParentContainer(&(d->selected));

    if (!it)
        return 0;
    if (it->container())
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
    for (QWidget *w = wlist->first(); w; w = wlist->next()) {
        if (list->findRef(w->parentWidget()) == -1)
            list->append(w->parentWidget());
    }

    removeChildrenFromList(*list);

    // one widget remains == the container we are looking for
    if (list->count() == 1)
        item = d->topTree->lookup(list->first()->objectName());
    else // we need to go one level up
        item =  commonParentContainer(list);

    delete list;
    return item;
}

Container*
Form::parentContainer(QWidget *w)
{
    ObjectTreeItem *it;
    if (!w)
        return 0;
    // it = d->topTree->lookup(d->selected.last()->name());
    //else
    it = d->topTree->lookup(w->objectName());

    if (it->parent()->container())
        return it->parent()->container();
    else
        return it->parent()->parent()->container();
}



void
Form::setDesignMode(bool design)
{
    d->design = design;
    if (!design) {
        ObjectTreeDict *dict = new ObjectTreeDict(*(d->topTree->dict()));
        ObjectTreeDictIterator it(*dict);
        for (; it.current(); ++it)
            m_lib->previewWidget(it.current()->widget()->metaObject()->className(), it.current()->widget(), d->toplevel);
        delete dict;

        d->widget = d->topTree->widget();
        delete(d->topTree);
        d->topTree = 0;
        delete(d->toplevel);
        d->toplevel = 0;
    }
}


///////////////////////////// Selection stuff ///////////////////////

void
Form::setSelectedWidget(QWidget *w, bool add, bool dontRaise, bool moreWillBeSelected)
{
    if ((d->selected.isEmpty()) || (w == widget()) || (d->selected.first() == widget()))
        add = false;

    if (!w) {
        setSelectedWidget(widget());
        return;
    }

    //raise selected widget and all possible parents
    QWidget *wtmp = w;
    while (!dontRaise && wtmp && wtmp->parentWidget() && (wtmp != widget())) {
        wtmp->raise();
        if (d->resizeHandles[ wtmp->objectName()])
            d->resizeHandles[ wtmp->objectName()]->raise();
        wtmp = wtmp->parentWidget();
    }

    if (wtmp)
        wtmp->setFocus();

    if (!add) {
        d->selected.clear();
        d->resizeHandles.clear();
    }
    d->selected.append(w);
    emit selectionChanged(w, add, moreWillBeSelected);
    emitActionSignals(false);

    // WidgetStack and TabWidget pages widgets shouldn't have resize handles, but their parent
    if (!FormManager::self()->isTopLevel(w) && w->parentWidget()
            && KexiUtils::objectIsA(w->parentWidget(), "QWidgetStack")) {
        w = w->parentWidget();
        if (w->parentWidget() && w->parentWidget()->inherits("QTabWidget"))
            w = w->parentWidget();
    }

    if (w && w != widget())
        d->resizeHandles.insert(w->objectName(), new ResizeHandleSet(w, this));
}

ResizeHandleSet*
Form::resizeHandlesForWidget(QWidget* w)
{
    return d->resizeHandles[w->objectName()];
}

void
Form::unSelectWidget(QWidget *w)
{
    d->selected.remove(w);
    d->resizeHandles.remove(w->objectName());
}

void
Form::selectFormWidget()
{
    setSelectedWidget(widget(), false);
}

void
Form::clearSelection()
{
    d->selected.clear();
    d->resizeHandles.clear();
    emit selectionChanged(0, false);
    emitActionSignals(false);
}

void
Form::emitActionSignals(bool withUndoAction)
{
    // Update menu and toolbar items
    if (d->selected.count() > 1)
        FormManager::self()->emitWidgetSelected(this, true);
    else if (d->selected.first() != widget())
        FormManager::self()->emitWidgetSelected(this, false);
    else
        FormManager::self()->emitFormWidgetSelected(this);

    if (!withUndoAction)
        return;

#ifdef __GNUC__
#warning pixmapcollection
#endif
#ifndef KEXI_NO_PIXMAPCOLLECTION
    KAction *undoAction = d->collection->action("edit_undo");
    if (undoAction)
        FormManager::self()->emitUndoEnabled(undoAction->isEnabled(), undoAction->text());

    KAction *redoAction = d->collection->action("edit_redo");
    if (redoAction)
        FormManager::self()->emitRedoEnabled(redoAction->isEnabled(), redoAction->text());
#endif
}

void
Form::emitSelectionSignals()
{
    emit selectionChanged(selectedWidgets()->first(), false);
// for(QWidget *w = selectedWidgets()->next(); w; w = selectedWidgets()->next())
//  emit selectionChanged(selectedWidgets()->first(), true);
    for (WidgetListIterator it(*selectedWidgets()); it.current(); ++it)
        emit selectionChanged(it.current(), true);
}

///////////////////////////  Various slots and signals /////////////////////
void
Form::formDeleted()
{
// clearSelection();
    d->selected.clear();
    d->resizeHandles.setAutoDelete(false);
    d->resizeHandles.clear();
    d->resizeHandles.setAutoDelete(true);
// emit selectionChanged(0, false);
// emitActionSignals(false);

    FormManager::self()->deleteForm(this);
    //delete this;
    deleteLater();
}

void
Form::changeName(const Q3CString &oldname, const Q3CString &newname)
{
    if (oldname == newname)
        return;
    if (!d->topTree->rename(oldname, newname)) { // rename failed
        KMessageBox::sorry(widget()->topLevelWidget(),
                           i18n("Renaming widget \"%1\" to \"%2\" failed.",
                                QString(oldname), QString(newname)));
//moved to WidgetPropertySet::slotChangeProperty()
//  KMessageBox::sorry(widget()->topLevelWidget(),
//  i18n("A widget with this name already exists. "
//   "Please choose another name or rename existing widget."));
        kDebug() << "Form::changeName() : ERROR : A widget named " << newname << " already exists";
        FormManager::self()->propertySet()->property("name") = QVariant(oldname);
    } else {
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
    if (d->connBuffer)
        d->connBuffer->removeAllConnectionsForWidget(item->name());
    emit childRemoved(item);
}

void
Form::addCommand(K3Command *command, bool execute)
{
    emit FormManager::self()->dirty(this, true);
    d->dirty = true;
    d->history->addCommand(command, execute);
    if (!execute) // simulate command to activate 'undo' menu
        slotCommandExecuted();
}

void
Form::clearCommandHistory()
{
    d->history->clear();
    FormManager::self()->emitUndoEnabled(false, QString());
    FormManager::self()->emitRedoEnabled(false, QString());
}

void
Form::slotCommandExecuted()
{
    emit FormManager::self()->dirty(this, true);
    d->dirty = true;
    // because actions text is changed after the commandExecuted() signal is emitted
    QTimer::singleShot(10, this, SLOT(emitUndoEnabled()));
    QTimer::singleShot(10, this, SLOT(emitRedoEnabled()));
}

void
Form::emitUndoEnabled()
{
#ifdef __GNUC__
#warning pixmapcollection
#endif
#ifndef KEXI_NO_PIXMAPCOLLECTION
    KAction *undoAction = d->collection->action("edit_undo");
    if (undoAction)
        FormManager::self()->emitUndoEnabled(undoAction->isEnabled(), undoAction->text());
#endif
}

void
Form::emitRedoEnabled()
{
#ifdef __GNUC__
#warning pixmapcollection
#endif
#ifndef KEXI_NO_PIXMAPCOLLECTION
    KAction *redoAction = d->collection->action("edit_redo");
    if (redoAction)
        FormManager::self()->emitRedoEnabled(redoAction->isEnabled(), redoAction->text());
#endif
}

void
Form::slotFormRestored()
{
    emit FormManager::self()->dirty(this, false);
    d->dirty = false;
}


///////////////////////////  Tab stops ////////////////////////

void
Form::addWidgetToTabStops(ObjectTreeItem *it)
{
    QWidget *w = it->widget();
    if (!w)
        return;
    if (!(w->focusPolicy() & Qt::TabFocus)) {
        // For composed widgets, we check if one of the child can have focus
        const QObjectList list(w->children());
        foreach(const QObject *obj, list) {
//   if(obj->isWidgetType() && (((QWidget*)obj)->focusPolicy() != QWidget::NoFocus)) {
//   if(obj->isWidgetType() && (((QWidget*)obj)->focusPolicy() & QWidget::TabFocus)) {
            if (obj->isWidgetType()) {//QWidget::TabFocus flag will be checked later!
                if (d->tabstops.findRef(it) == -1) {
                    d->tabstops.append(it);
                    return;
                }
            }
        }
    } else if (d->tabstops.findRef(it) == -1) // not yet in the list
        d->tabstops.append(it);
}

void
Form::updateTabStopsOrder()
{
    for (ObjectTreeListIterator it(d->tabstops);it.current();) {
        if (!(it.current()->widget()->focusPolicy() & Qt::TabFocus)) {
            kexidbg << "Form::updateTabStopsOrder(): widget removed because has no TabFocus: " << it.current()->widget()->objectName();
            d->tabstops.remove(it.current());
        } else
            ++it;
    }
}

//! Collects all the containers reculsively. Used by Form::autoAssignTabStops().
void collectContainers(ObjectTreeItem* item, QSet<Container*>& containers)
{
    if (!item->container())
        return;
    if (!containers.contains(item->container())) {
        kDebug() << "collectContainers() " << item->container()->objectTree()->className()
        << " " << item->container()->objectTree()->name();
        containers.insert(item->container());
    }
    for (ObjectTreeListIterator it(*item->children()); it.current(); ++it)
        collectContainers(it.current(), containers);
}

void
Form::autoAssignTabStops()
{
    VerWidgetList list(toplevelContainer()->widget());
    HorWidgetList hlist(toplevelContainer()->widget());

    // 1. Collect all the containers, as we'll be sorting widgets groupped by containers
    QSet<Container*> containers;

    collectContainers(toplevelContainer()->objectTree(), containers);

    foreach_list(ObjectTreeListIterator, it, d->tabstops) {
        if (it.current()->widget()) {
            kDebug() << "Form::autoAssignTabStops() widget to sort: " << it.current()->widget();
            list.append(it.current()->widget());
        }
    }

    list.sort();
    foreach_list(Q3PtrListIterator<QWidget>, iter, list)
    kDebug() << iter.current()->metaObject()->className()
    << " " << iter.current()->objectName();

    d->tabstops.clear();

    /// We automatically sort widget from the top-left to bottom-right corner
    //! \todo Handle RTL layout (ie from top-right to bottom-left)
    foreach_list(WidgetListIterator, it, list) {
        QWidget *w = it.current();
        hlist.append(w);

        ++it;
        QWidget *nextw = it.current();
        QObject *page_w = 0;
        KFormDesigner::TabWidget *tab_w = KFormDesigner::findParent<KFormDesigner::TabWidget>(w, "KFormDesigner::TabWidget", page_w);
        while (nextw) {
            if (KexiUtils::hasParent(w, nextw)) // do not group (sort) widgets where on is a child of another
                break;
            if (nextw->y() >= (w->y() + 20))
                break;
            if (tab_w) {
                QObject *page_nextw = 0;
                KFormDesigner::TabWidget *tab_nextw = KFormDesigner::findParent<KFormDesigner::TabWidget>(nextw, "KFormDesigner::TabWidget", page_nextw);
                if (tab_w == tab_nextw) {
                    if (page_w != page_nextw) // 'nextw' widget within different tab page
                        break;
                }
            }
            hlist.append(nextw);
            ++it;
            nextw = it.current();
        }
        hlist.sort();

        for (WidgetListIterator it2(hlist); it2.current() != 0; ++it2) {
            ObjectTreeItem *tree = d->topTree->lookup(it2.current()->objectName());
            if (tree) {
                kDebug() << "Form::autoAssignTabStops() adding " << tree->name();
                d->tabstops.append(tree);
            }
        }

        --it;
        hlist.clear();
    }
}

uint Form::formatVersion() const
{
    return d->formatVersion;
}

void Form::setFormatVersion(uint ver)
{
    d->formatVersion = ver;
}
uint Form::originalFormatVersion() const
{
    return d->originalFormatVersion;
}

void Form::setOriginalFormatVersion(uint ver)
{
    d->originalFormatVersion = ver;
}

void Form::setFormWidget(FormWidget* w)
{
    if (!d)
        return;
    d->formWidget = w;
    if (!d->formWidget)
        return;
    d->formWidget->m_form = this;
}

#include "form.moc"
