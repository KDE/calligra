/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2008 Jarosław Staniek <staniek@kde.org>

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

#include <QLabel>

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
//Qt4    resizeHandles.setAutoDelete(true);
    dirty = false;
    interactive = true;
    design = true;
    autoTabstops = false;
//    tabstops.setAutoDelete(false);
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
//Qt4    resizeHandles.setAutoDelete(false);
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
Form::createToplevel(QWidget *container, FormWidget *formWidget, const QByteArray &)
{
    kDebug() << "container= " << (container ? container->objectName() : "<NULL>")
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

    kDebug() << "d->toplevel=" << d->toplevel;
}


Container*
Form::activeContainer()
{
    if (d->selected.isEmpty())
        return d->toplevel;

    ObjectTreeItem *it;
    if (d->selected.count() == 1)
        it = d->topTree->lookup(d->selected.last()->objectName());
    else
        it = commonParentContainer(d->selected);

    if (!it)
        return 0;
    if (it->container())
        return it->container();
    else
        return it->parent()->container();
}

ObjectTreeItem*
Form::commonParentContainer(const QWidgetList& wlist)
{
    // create a list of all widget parents
    QSet<QWidget*> parents;
    foreach (QWidget *w, wlist) {
        parents.insert(w->parentWidget());
    }

    QWidgetList parentsList(parents.toList());
    removeChildrenFromList(parentsList);

    // one widget remains == the container we are looking for
    ObjectTreeItem *item;
    if (parentsList.count() == 1) {
        item = d->topTree->lookup(parentsList.first()->objectName());
    }
    else {
        // we need to go one level up
        item =  commonParentContainer(parentsList);
    }
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
    if (design)
        return;

    ObjectTreeHash hash(*(d->topTree->hash()));
    foreach (ObjectTreeItem *item, hash) {
        m_lib->previewWidget(
            item->widget()->metaObject()->className(), 
            item->widget(), d->toplevel
        );
    }

    d->widget = d->topTree->widget();
    delete d->topTree;
    d->topTree = 0;
    delete d->toplevel;
    d->toplevel = 0;
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
        if (d->resizeHandles.value( wtmp->objectName() ))
            d->resizeHandles.value( wtmp->objectName() )->raise();
        wtmp = wtmp->parentWidget();
    }

    if (wtmp)
        wtmp->setFocus();

    if (!add) {
        d->selected.clear();
        qDeleteAll(d->resizeHandles);
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
    return d->resizeHandles.value(w->objectName());
}

void
Form::unSelectWidget(QWidget *w)
{
    d->selected.removeOne(w);
    ResizeHandleSet *set = d->resizeHandles.take(w->objectName());
    delete set;
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
    qDeleteAll(d->resizeHandles);
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
    if (selectedWidgets()->first()) {
        emit selectionChanged(selectedWidgets()->first(), false);
    }
// for(QWidget *w = selectedWidgets()->next(); w; w = selectedWidgets()->next())
//  emit selectionChanged(selectedWidgets()->first(), true);
    foreach (QWidget *w, *selectedWidgets()) {
        emit selectionChanged(w, true);
    }
}

///////////////////////////  Various slots and signals /////////////////////
void
Form::formDeleted()
{
// clearSelection();
    d->selected.clear();
//Qt4    d->resizeHandles.setAutoDelete(false);
    d->resizeHandles.clear();
//Qt4    d->resizeHandles.setAutoDelete(true);
// emit selectionChanged(0, false);
// emitActionSignals(false);

    FormManager::self()->deleteForm(this);
    //delete this;
    deleteLater();
}

void
Form::changeName(const QByteArray &oldname, const QByteArray &newname)
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
        kWarning() << "widget named " << newname << " already exists";
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
    d->tabstops.removeOne(item);
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
                if (!d->tabstops.contains(it)) {
                    d->tabstops.append(it);
                    return;
                }
            }
        }
    } else if (!d->tabstops.contains(it)) { // not yet in the list
        d->tabstops.append(it);
    }
}

void
Form::updateTabStopsOrder()
{
    ObjectTreeList newList(d->tabstops);
    foreach (ObjectTreeItem *item, d->tabstops) {
        if (!(item->widget()->focusPolicy() & Qt::TabFocus)) {
            kexidbg << "Widget removed because has no TabFocus: " 
                << item->widget()->objectName();
            newList.removeOne(item);
        }
    }
    d->tabstops = newList;
}

//! Collects all the containers reculsively. Used by Form::autoAssignTabStops().
static void collectContainers(ObjectTreeItem* item, QSet<Container*>& containers)
{
    if (!item->container())
        return;
    if (!containers.contains(item->container())) {
        kDebug() << item->container()->objectTree()->className()
        << " " << item->container()->objectTree()->name();
        containers.insert(item->container());
    }
    foreach (ObjectTreeItem *child, *item->children()) {
        collectContainers(child, containers);
    }
}

void
Form::autoAssignTabStops()
{
    VerticalWidgetList list(toplevelContainer()->widget());
    HorizontalWidgetList hlist(toplevelContainer()->widget());

    // 1. Collect all the containers, as we'll be sorting widgets groupped by containers
    QSet<Container*> containers;

    collectContainers(toplevelContainer()->objectTree(), containers);

    foreach (ObjectTreeItem *item, d->tabstops) {
        if (item->widget()) {
            kDebug() << "Widget to sort: " << item->widget();
            list.append(item->widget());
        }
    }

    list.sort();
    foreach (QWidget *w, list) {
        kDebug() << w->metaObject()->className()
            << " " << w->objectName();
    }
    d->tabstops.clear();

    /// We automatically sort widget from the top-left to bottom-right corner
    //! \todo Handle RTL layout (ie from top-right to bottom-left)
    for (QWidgetList::ConstIterator it(list.constBegin()); it!=list.constEnd(); ++it) {
        QWidget *w = *it;
        hlist.append(w);

        ++it;
        QWidget *nextw = *it;
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
            nextw = *it;
        }
        hlist.sort();

        foreach (QWidget *w, hlist) {
            ObjectTreeItem *tree = d->topTree->lookup(w->objectName());
            if (tree) {
                kDebug() << "adding " << tree->name();
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
