/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2014 Jarosław Staniek <staniek@kde.org>

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

#include "form_p.h"

#include <QApplication>
#include <QClipboard>
#include <QLabel>
#include <QDomDocument>
#include <QLineEdit>
#include <QMenu>
#include <QAction>
#include <QDebug>

#include <kundo2stack.h>

#include <kmessagebox.h>
#include <kactioncollection.h>
#include <kfontdialog.h>
#include <ktextedit.h>
#include <KLocalizedString>

#include <KexiIcon.h>

#include "WidgetInfo.h"
#include "FormWidget.h"
#include "container.h"
#include "objecttree.h"
#include "formIO.h"
#include "FormWidgetInterface.h"
#include "widgetlibrary.h"
#include "events.h"
#include "utils.h"
#include "widgetwithsubpropertiesinterface.h"
#include "tabstopdialog.h"
#include <kexiutils/utils.h>
#include <kexiutils/identifier.h>

#include <db/utils.h>

using namespace KFormDesigner;

Form::Form(WidgetLibrary* library, Mode mode, KActionCollection &col, ActionGroup& group)
        : QObject(library)
        , d( new FormPrivate(this, library) )
{
    init(mode, col, group);
}

Form::Form(Form *parent)
        : QObject(parent->library())
        , d( new FormPrivate(this, parent->library()) )
{
    init(parent->mode(), *parent->actionCollection(), *parent->widgetActionGroup());
}

Form::~Form()
{
    emit destroying();
    delete d;
}

void Form::init(Mode mode, KActionCollection &col, KFormDesigner::ActionGroup &group)
{
    d->mode = mode;
    d->features = 0;
    d->widgetActionGroup = &group;

    connect(&d->propertySet, SIGNAL(propertyChanged(KPropertySet&,KProperty&)),
            this, SLOT(slotPropertyChanged(KPropertySet&,KProperty&)));
    connect(&d->propertySet, SIGNAL(propertyReset(KPropertySet&,KProperty&)),
            this, SLOT(slotPropertyReset(KPropertySet&,KProperty&)));

    d->collection = &col;
}

WidgetLibrary* Form::library() const
{
    return d->library;
}

KActionCollection  *Form::actionCollection() const
{
    return d->collection;
}

KFormDesigner::ActionGroup* Form::widgetActionGroup() const
{
    return d->widgetActionGroup;
}

void Form::setFeatures(Features features)
{
    d->features = features;
}

Form::Features Form::features() const
{
    return d->features;
}

QWidget* Form::widget() const
{
    if (d->topTree)
        return d->topTree->widget();
    else if (d->toplevel)
        return d->toplevel->widget();
    else // preview form
        return d->widget;
}

FormWidget* Form::formWidget() const
{
    return d->formWidget;
}

ObjectTree* Form::objectTree() const
{
    return d->topTree;
}

QWidgetList* Form::selectedWidgets() const
{
    return &(d->selected);
}

QWidget* Form::selectedWidget() const
{
    return d->selected.count() == 1 ? d->selected.first() : 0;
}

void Form::setInteractiveMode(bool interactive)
{
    d->interactive = interactive;
}

bool Form::interactiveMode() const
{
    return d->interactive;
}

Form::Mode Form::mode() const
{
    return d->mode;
}

bool Form::isModified() const
{
    return d->modified;
}

void Form::setModified(bool set)
{
    d->modified = set;
    emit modified(set);
}

int Form::gridSize() const
{
    return d->gridSize;
}

void Form::setGridSize(int gridSize)
{
    d->gridSize = gridSize;
}

int Form::defaultMargin() const
{
    return 11;
}

int Form::defaultSpacing() const
{
    return 6;
}

QString Form::filename() const
{
    return d->filename;
}

void Form::setFilename(const QString &file)
{
    d->filename = file;
}

void Form::clearUndoStack()
{
    d->undoStack.clear();
}

void Form::setUndoStackClean()
{
    d->undoStack.setClean();
}

#ifdef KFD_SIGSLOTS
ConnectionBuffer* Form::connectionBuffer() const
{
    return d->connBuffer;
}

void Form::setConnectionBuffer(ConnectionBuffer *b)
{
    if (b != d->connBuffer) {
        delete d->connBuffer;
    }
    d->connBuffer = b;
}
#endif

#ifndef KEXI_NO_PIXMAPCOLLECTION
PixmapCollection* Form::pixmapCollection() const
{
    return d->pixcollection;
}
#endif

void Form::setPixmapsStoredInline(bool set)
{
    d->pixmapsStoredInline = set;
}

bool Form::pixmapsStoredInline() const
{
    return d->pixmapsStoredInline;
}

ObjectTreeList* Form::tabStops()
{
    return &(d->tabstops);
}

bool Form::autoTabStops() const {
    return d->autoTabstops;
}

void Form::setAutoTabStops(bool autoTab)
{
    d->autoTabstops = autoTab;
}

QHash<QByteArray, QString>* Form::headerProperties()
{
    return &d->headerProperties;
}

//////////////// Container -related functions ///////////////////////

Container* Form::toplevelContainer() const
{
    return d->toplevel;
}

void Form::createToplevel(QWidget *container, FormWidget *formWidget, const QByteArray &)
{
    //qDebug() << "container= " << (container ? container->objectName() : "<NULL>")
    //         << " formWidget=" << formWidget;

    setFormWidget(formWidget);
    d->toplevel = new Container(0, container, this);
    d->toplevel->setObjectName(objectName());
    d->topTree = new ObjectTree(xi18n("Form"), container->objectName(), container, d->toplevel);
    d->toplevel->setObjectTree(d->topTree);
    d->toplevel->setForm(this);

//! @todo pixmapcollection
#ifndef KEXI_NO_PIXMAPCOLLECTION
    d->pixcollection = new PixmapCollection(container->objectName(), this);
#endif

    d->topTree->setWidget(container);
//! @todo copy caption in Kexi from object's caption
// d->topTree->addModifiedProperty("caption", name());
//d->topTree->addModifiedProperty("icon");

    connect(container, SIGNAL(destroyed()), this, SLOT(formDeleted()));
    //qDebug() << "d->toplevel=" << d->toplevel;

    // alter the style
    delete d->designModeStyle;
    d->designModeStyle = 0;
    if (d->mode == DesignMode) {
        d->designModeStyle = new DesignModeStyle(d->topTree->widget()->style(), this);
        d->topTree->widget()->setStyle(d->designModeStyle);
    }
}

Container* Form::activeContainer()
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

ObjectTreeItem* Form::commonParentContainer(const QWidgetList& wlist)
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

Container* Form::parentContainer(QWidget *w) const
{
    if (!w)
        return 0;
    ObjectTreeItem *it = d->topTree->lookup(w->objectName());
    if (!it || !it->parent())
        return 0;

    if (it->parent()->container())
        return it->parent()->container();
    else
        return it->parent()->parent()->container();
}

void Form::setMode(Mode mode)
{
    d->mode = mode;
    if (d->mode == DesignMode) {
        d->designModeStyle = new DesignModeStyle(d->widget->style());
        d->widget->setStyle(d->designModeStyle);
        return;
    }

    ObjectTreeHash hash(*(d->topTree->hash()));
    foreach (ObjectTreeItem *item, hash) {
        library()->previewWidget(
            item->widget()->metaObject()->className(),
            item->widget(), d->toplevel
        );
    }

    d->widget = d->topTree->widget();
    delete d->topTree;
    d->topTree = 0;
    delete d->toplevel;
    d->toplevel = 0;

    // alter the style
    delete d->designModeStyle;
    d->designModeStyle = 0;
}


///////////////////////////// Selection stuff ///////////////////////

void Form::selectWidget(QWidget *w, WidgetSelectionFlags flags)
{
    if (!d->selectWidgetEnabled)
        return;
    d->selectWidgetEnabled = false;
    selectWidgetInternal(w, flags);
    d->selectWidgetEnabled = true;
}

void Form::selectWidgetInternal(QWidget *w, WidgetSelectionFlags flags)
{
    if (!w) {
        selectWidget(widget());
        return;
    }
    //qDebug() << "selected count=" << d->selected.count();
    if (!d->selected.isEmpty()) {
        //qDebug() << "first=" << d->selected.first();
    }
    //qDebug() << w;

    if (d->selected.count() == 1 && d->selected.first() == w) {
        return;
    }

    if (d->selected.isEmpty() || w == widget() || (d->selected.first() == widget())) {
        flags |= ReplacePreviousSelection;
    }

    //raise selected widget and all possible parents
    QWidget *wtmp = w;
    while (!(flags & DontRaise) && wtmp && wtmp->parentWidget() && (wtmp != widget())) {
        wtmp->raise();
        if (d->resizeHandles.value( wtmp->objectName() ))
            d->resizeHandles.value( wtmp->objectName() )->raise();
        wtmp = wtmp->parentWidget();
    }

    if (wtmp)
        wtmp->setFocus();

    if (flags & ReplacePreviousSelection) {
        d->selected.clear();
        qDeleteAll(d->resizeHandles);
        d->resizeHandles.clear();
    }
    d->selected.append(w);
    emitSelectionChanged(w, flags);
    emitActionSignals();

    // WidgetStack and TabWidget pages widgets shouldn't have resize handles, but their parent
//! @todo move special case to a factory?
#if 0
    if (!isTopLevelWidget(w) && w->parentWidget()
            && KexiUtils::objectIsA(w->parentWidget(), "QWidgetStack")) {
        w = w->parentWidget();
        if (w->parentWidget() && w->parentWidget()->inherits("QTabWidget"))
            w = w->parentWidget();
    }
#endif

    if (w && w != widget()) {
        ResizeHandleSet *handles = new ResizeHandleSet(w, this);
        d->resizeHandles.insert(w->objectName(), handles);
        connect(handles, SIGNAL(geometryChangeStarted()),
                parentContainer(w), SLOT(startChangingGeometryPropertyForSelectedWidget()));
        connect(handles, SIGNAL(geometryChanged(QRect)),
                parentContainer(w), SLOT(setGeometryPropertyForSelectedWidget(QRect)));
    }
}

void Form::selectWidgets(const QList<QWidget*>& widgets, WidgetSelectionFlags flags)
{
    int i = 0;
    const int count = widgets.count();
    foreach (QWidget* widget, widgets) {
        if (i == 1) {
            flags |= AddToPreviousSelection;
        }
        if (i == (count - 1)) {
            flags = LastSelection;
        }
        selectWidget(widget, flags);
    }
}

QList<QWidget*> Form::widgetsForNames(const QList<QByteArray>& names) const
{
    QList<QWidget*> widgets;
    foreach (const QByteArray& name, names) {
        ObjectTreeItem* item = objectTree()->lookup(name);
        if (item) { //we're checking for item!=0 because the name could be of a form widget
            widgets.append(item->widget());
        }
    }
    return widgets;
}

void Form::selectWidgets(const QList<QByteArray>& names, WidgetSelectionFlags flags)
{
    selectWidgets(widgetsForNames(names), flags);
}

bool Form::isTopLevelWidget(QWidget *w) const
{
    /* should not be used, just check w==formWidget() instead? */
    ObjectTreeItem *item = objectTree()->lookup(w->objectName());
    if (!item)
        return true;

    return !item->parent();
}

ResizeHandleSet* Form::resizeHandlesForWidget(QWidget* w)
{
    return d->resizeHandles.value(w->objectName());
}

void Form::deselectWidget(QWidget *w)
{
    d->selected.removeOne(w);
    ResizeHandleSet *set = d->resizeHandles.take(w->objectName());
    delete set;
}

void Form::selectFormWidget()
{
    selectWidget(widget());
}

void Form::clearSelection()
{
    d->selected.clear();
    qDeleteAll(d->resizeHandles);
    d->resizeHandles.clear();
    emitSelectionChanged(0, DefaultWidgetSelectionFlags);
    emitActionSignals();
}

void Form::setInsertionPoint(const QPoint &p)
{
    d->insertionPoint = p;
}

QAction* Form::action(const QString& name)
{
    if (name == KStandardAction::name(KStandardAction::Undo)) {
        QAction *a = d->internalCollection.action( name );
        if (!a) {
            a = d->undoStack.createUndoAction(&d->internalCollection);
            // connect this action to the form instead of stack
            disconnect(a, SIGNAL(triggered()), &d->undoStack, SLOT(undo()));
            connect(a, SIGNAL(triggered()), this, SLOT(undo()));
        }
        return a;
    }
    else if (name == KStandardAction::name(KStandardAction::Redo)) {
        QAction *a = d->internalCollection.action( name );
        if (!a) {
            a = d->undoStack.createRedoAction(&d->internalCollection);
            // connect this action to the form instead of stack
            disconnect(a, SIGNAL(triggered()), &d->undoStack, SLOT(redo()));
            connect(a, SIGNAL(triggered()), this, SLOT(redo()));
        }
        return a;
    }
    return d->collection->action(name);
}

void Form::emitActionSignals()
{
    // Update menu and toolbar items
    if (selectedWidget()) {
      if (widget() == selectedWidget())
        emitFormWidgetSelected();
      else
        emitWidgetSelected( false );
    }
    else if (selectedWidgets()) {
      emitWidgetSelected( true );
    }
}

void Form::emitUndoActionSignals()
{
//! @todo pixmapcollection
#ifndef KEXI_NO_PIXMAPCOLLECTION
    QAction *undoAction = d->collection->action(QLatin1String("edit_undo"));
    if (undoAction)
        emitUndoEnabled(undoAction->isEnabled(), undoAction->text());

    QAction *redoAction = d->collection->action(QLatin1String("edit_redo"));
    if (redoAction)
        emitRedoEnabled(redoAction->isEnabled(), redoAction->text());
#endif
}

void
Form::emitSelectionSignals()
{
    if (!selectedWidgets()->isEmpty()) {
        emitSelectionChanged(selectedWidgets()->first(), DefaultWidgetSelectionFlags);
    }
    foreach (QWidget *w, *selectedWidgets()) {
        emitSelectionChanged(w, LastSelection);
    }
}

void Form::emitWidgetSelected(bool multiple)
{
    enableFormActions();
    // Enable edit actions
    d->enableAction("edit_copy", true);
    d->enableAction("edit_cut", true);
    d->enableAction("edit_delete", true);
    d->enableAction("clear_contents", true);

    // 'Align Widgets' menu
    d->enableAction("align_menu", multiple);
    d->enableAction("align_to_left", multiple);
    d->enableAction("align_to_right", multiple);
    d->enableAction("align_to_top", multiple);
    d->enableAction("align_to_bottom", multiple);

    d->enableAction("adjust_size_menu", true);
    d->enableAction("adjust_width_small", multiple);
    d->enableAction("adjust_width_big", multiple);
    d->enableAction("adjust_height_small", multiple);
    d->enableAction("adjust_height_big", multiple);

    d->enableAction("format_raise", true);
    d->enableAction("format_lower", true);

    QWidgetList *wlist = selectedWidgets();
    bool fontEnabled = false;
    foreach (QWidget* w, *wlist) {
        if (-1 != w->metaObject()->indexOfProperty("font")) {
            fontEnabled = true;
            break;
        }
    }
    d->enableAction("format_font", fontEnabled);

    // If the widgets selected is a container, we enable layout actions
    if (!multiple) {
        if (!wlist->isEmpty()) {
            objectTree()->lookup(wlist->first()->objectName());
        }
    }
    emit widgetSelected(true);
}

void Form::emitFormWidgetSelected()
{
    d->enableAction("edit_copy", false);
    d->enableAction("edit_cut", false);
    d->enableAction("edit_delete", false);
    d->enableAction("clear_contents", false);

    // Disable format functions
    d->enableAction("align_menu", false);
    d->enableAction("align_to_left", false);
    d->enableAction("align_to_right", false);
    d->enableAction("align_to_top", false);
    d->enableAction("align_to_bottom", false);
    d->enableAction("adjust_size_menu", false);
    d->enableAction("format_raise", false);
    d->enableAction("format_lower", false);

    d->enableAction("format_font", false);

    enableFormActions();
    emit formWidgetSelected();
}

void Form::emitNoFormSelected()
{
    disableWidgetActions();

    // Disable 'Tools' actions
    d->enableAction("pixmap_collection", false);
#ifdef KFD_SIGSLOTS
    if (d->features & EnableConnections) {
        d->enableAction("form_connections", false);
    }
#endif
    d->enableAction("taborder", false);
    d->enableAction("change_style", true);

    // Disable items in 'File'
    if (d->features & EnableFileActions) {
        d->enableAction("file_save", false);
        d->enableAction("file_save_as", false);
        d->enableAction("preview_form", false);
    }

    emit noFormSelected();
}

void Form::enableFormActions()
{
    // Enable 'Tools' actions
    d->enableAction("pixmap_collection", true);
#ifdef KFD_SIGSLOTS
    if (d->features & EnableConnections) {
        d->enableAction("form_connections", true);
    }
#endif
    d->enableAction("taborder", true);
    d->enableAction("change_style", true);

    // Enable items in 'File'
    if (d->features & EnableFileActions) {
        d->enableAction("file_save", true);
        d->enableAction("file_save_as", true);
        d->enableAction("preview_form", true);
    }

    d->enableAction("edit_paste", true); //?? isPasteEnabled());
    d->enableAction("edit_select_all", true);
}

void Form::disableWidgetActions()
{
    // Disable edit actions
    d->enableAction("edit_copy", false);
    d->enableAction("edit_cut", false);
    d->enableAction("edit_delete", false);
    d->enableAction("clear_contents", false);

    // Disable format functions
    d->enableAction("align_menu", false);
    d->enableAction("align_to_left", false);
    d->enableAction("align_to_right", false);
    d->enableAction("align_to_top", false);
    d->enableAction("align_to_bottom", false);
    d->enableAction("adjust_size_menu", false);
    d->enableAction("format_raise", false);
    d->enableAction("format_lower", false);
}

///////////////////////////  Various slots and signals /////////////////////
void Form::formDeleted()
{
    d->selected.clear();
    d->resizeHandles.clear();
    deleteLater();
}

void Form::changeName(const QByteArray &oldname, const QByteArray &newname)
{
    if (oldname == newname)
        return;

    if (d->topTree->rename(oldname, newname)) {
#ifdef KFD_SIGSLOTS
        d->connBuffer->fixName(oldname, newname);
#endif
        ResizeHandleSet *temp = d->resizeHandles.take(oldname);
        d->resizeHandles.insert(newname, temp);
    }
    else { // rename failed
        KMessageBox::sorry(widget()->topLevelWidget(),
                           xi18n("Renaming widget \"%1\" to \"%2\" failed.",
                                QString(oldname), QString(newname)));
        qWarning() << "widget" << newname << "already exists, reverting rename";
        d->propertySet.changeProperty("objectName", oldname);
    }
}

void Form::emitChildAdded(ObjectTreeItem *item)
{
    addWidgetToTabStops(item);
    emit childAdded(item);
}

void Form::emitChildRemoved(ObjectTreeItem *item)
{
    d->tabstops.removeOne(item);
#ifdef KFD_SIGSLOTS
    if (d->connBuffer)
        d->connBuffer->removeAllConnectionsForWidget(item->name());
#endif
    emit childRemoved(item);
}

const Command* Form::executingCommand() const
{
    return d->executingCommand;
}

bool Form::addCommand(Command *command, AddCommandOption option)
{
    setModified(true);
    if (option == DontExecuteCommand) {
        command->blockRedoOnce();
    }
    const bool saveExecutingCommand = !d->executingCommand;
    if (saveExecutingCommand)
        d->executingCommand = command;

    d->undoStack.push(command);

    if (saveExecutingCommand)
        d->executingCommand = 0;
    //qDebug() << "ADDED:" << *command;
    return true;
}

void Form::emitUndoEnabled()
{
//! @todo pixmapcollection
#ifndef KEXI_NO_PIXMAPCOLLECTION
    QAction *undoAction = d->collection->action(QLatin1String("edit_undo"));
    if (undoAction)
        emitUndoEnabled(undoAction->isEnabled(), undoAction->text());
#endif
}

void Form::emitRedoEnabled()
{
//! @todo pixmapcollection
#ifndef KEXI_NO_PIXMAPCOLLECTION
    QAction *redoAction = d->collection->action(QLatin1String("edit_redo"));
    if (redoAction)
        emitRedoEnabled(redoAction->isEnabled(), redoAction->text());
#endif
}

void Form::slotFormRestored()
{
    setModified(false);
}


///////////////////////////  Tab stops ////////////////////////

void Form::addWidgetToTabStops(ObjectTreeItem *it)
{
    QWidget *w = it->widget();
    if (!w)
        return;
    if (!(w->focusPolicy() & Qt::TabFocus)) {
        // For composed widgets, we check if one of the child can have focus
        const QObjectList list(w->children());
        foreach(const QObject *obj, list) {
            if (obj->isWidgetType()) {//QWidget::TabFocus flag will be checked later!
                if (!d->tabstops.contains(it)) {
                    //qDebug() << "adding child of" << w << ":" << obj;
                    d->tabstops.append(it);
                    return;
                }
            }
        }
    }
    else if (!d->tabstops.contains(it)) { // not yet in the list
        //qDebug() << "adding" << w;
        d->tabstops.append(it);
    }
}

void Form::updateTabStopsOrder()
{
    ObjectTreeList newList(d->tabstops);
    foreach (ObjectTreeItem *item, d->tabstops) {
        if (!(item->widget()->focusPolicy() & Qt::TabFocus)) {
            //qDebug() << "Widget removed because has no TabFocus:"
            //.        << item->widget()->objectName();
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
        //qDebug() << item->container()->objectTree()->className()
        //         << " " << item->container()->objectTree()->name();
        containers.insert(item->container());
    }
    foreach (ObjectTreeItem *child, *item->children()) {
        collectContainers(child, containers);
    }
}

void Form::autoAssignTabStops()
{
    VerticalWidgetList list(toplevelContainer()->widget());
    HorizontalWidgetList hlist(toplevelContainer()->widget());

    // 1. Collect all the containers, as we'll be sorting widgets groupped by containers
    QSet<Container*> containers;

    collectContainers(toplevelContainer()->objectTree(), containers);

    foreach (ObjectTreeItem *item, d->tabstops) {
        if (item->widget()) {
            //qDebug() << "Widget to sort: " << item->widget();
            list.append(item->widget());
        }
    }

    list.sort();
    //foreach (QWidget *w, list) {
    //    qDebug() << w->metaObject()->className() << w->objectName();
    //}
    d->tabstops.clear();

    /// We automatically sort widget from the top-left to bottom-right corner
    //! \todo Handle RTL layout (ie from top-right to bottom-left)
    for (QWidgetList::ConstIterator it(list.constBegin()); it!=list.constEnd(); ++it) {
        QWidget *w = *it;
        hlist.append(w);

        ++it;
        QWidget *nextw = it==list.constEnd() ? 0 : *it;
        Q_UNUSED(nextw);
        QObject *page_w = 0;
        KFormDesigner::TabWidget *tab_w
                = KFormDesigner::findParent<KFormDesigner::TabWidget>(
                    w, "KFormDesigner::TabWidget", page_w);
        
        for (; it!=list.constEnd(); ++it) {
            QWidget *nextw = *it;
            if (KexiUtils::hasParent(w, nextw)) // do not group (sort) widgets where one is a child of another
                break;
            if (nextw->y() >= (w->y() + 20))
                break;
            if (tab_w) {
                QObject *page_nextw = 0;
                KFormDesigner::TabWidget *tab_nextw
                        = KFormDesigner::findParent<KFormDesigner::TabWidget>(
                            nextw, "KFormDesigner::TabWidget", page_nextw);
                if (tab_w == tab_nextw) {
                    if (page_w != page_nextw) // 'nextw' widget within different tab page
                        break;
                }
            }
            hlist.append(nextw);
        }
        hlist.sort();

        foreach (QWidget *w, hlist) {
            ObjectTreeItem *tree = d->topTree->lookup(w->objectName());
            if (tree) {
                //qDebug() << "adding " << tree->name();
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
    d->formWidget = w;
    if (!d->formWidget)
        return;
    d->formWidget->setForm(this);
}

void Form::enterWidgetInsertingState(const QByteArray &classname)
{
    if (d->state != WidgetInserting) {
        enterWidgetSelectingState();
    }
    d->state = WidgetInserting;

    if (toplevelContainer()) {
        widget()->setCursor(QCursor(Qt::CrossCursor));
    }
    const QList<QWidget*> list(widget()->findChildren<QWidget*>());
    foreach (QWidget *w, list) {
        d->cursors.insert(w, w->cursor());
        w->setCursor(QCursor(Qt::CrossCursor));
    }

    d->selectedClass = classname;
    QAction *pointer_action = d->collection->action(QLatin1String("edit_pointer"));
    if (pointer_action) {
        pointer_action->setChecked(false);
    }
}

QByteArray Form::selectedClass() const
{
    return d->selectedClass;
}

void Form::abortWidgetInserting()
{
    if (d->state != WidgetInserting)
        return;

    widget()->unsetCursor();
    const QList<QWidget*> list(widget()->findChildren<QWidget*>());
    foreach (QWidget *w, list) {
        w->unsetCursor();
    }
    d->state = WidgetSelecting;
    QAction *pointer_action = d->widgetActionGroup->action(QLatin1String("edit_pointer"));
    if (pointer_action) {
        pointer_action->setChecked(true);
    }
}

void Form::enterWidgetSelectingState()
{
    switch (d->state) {
    case WidgetInserting:
        abortWidgetInserting();
        break;
#ifdef KFD_SIGSLOTS
    case Connecting:
        abortCreatingConnection();
        break;
#endif
    default:
        break;
    }
}

#ifdef KFD_SIGSLOTS
void Form::enterConnectingState()
{
    if (!(d->features & EnableConnections))
        return;
    enterWidgetSelectingState();

    // We set a Pointing hand cursor while drawing the connection
    d->mouseTrackers = new QStringList();
    if (toplevelContainer()) {
        widget()->setCursor(QCursor(Qt::PointingHandCursor));
        widget()->setMouseTracking(true);
    }
    const QList<QWidget*> list(widget()->findChildren<QWidget*>());
    foreach(QWidget *w, list) {
        d->cursors.insert(w, w->cursor());
        w->setCursor(QCursor(Qt::PointingHandCursor));
        if (w->hasMouseTracking())
            d->mouseTrackers->append(w->objectName());
        w->setMouseTracking(true);
    }
    delete m_connection;
    m_connection = new Connection();
    m_drawingSlot = true;
    if (m_dragConnection)
        m_dragConnection->setChecked(true);
}

void Form::resetSelectedConnection()
{
//! @todo
    if (!(d->features & EnableConnections))
        return;

    delete m_connection;
    m_connection = new Connection();

    if (formWidget()) {
        formWidget()->clearForm();
    }
    if (widget()) {
        widget()->repaint();
    }
}

void Form::abortCreatingConnection()
{
//! @todo
    if (!(d->features & EnableConnections))
        return;
    if (d->state != Connecting)
        return;

    if (formWidget()) {
        formWidget()->clearForm();
    }

    widget()->unsetCursor();
    widget()->setMouseTracking(false);
    const QList<QWidget*> list(widget()->findChildren<QWidget*>());
    foreach (QWidget *w, list) {
        QHash<QObject*, QCursor>::ConstIterator curIt(d->cursors.find(w));
        if (curIt != d->cursors.constEnd())
            w->setCursor(*curIt);
        w->setMouseTracking(d->mouseTrackers->contains(w->objectName()));
    }
    delete d->mouseTrackers;
    d->mouseTrackers = 0;

    if (m_connection->slot().isNull())
        emit connectionAborted(this);
    delete m_connection;
    m_connection = 0;
    m_drawingSlot = false;
    QAction *pointer_action = d->widgetActionGroup->action(QLatin1String("edit_pointer"));
    if (pointer_action) {
        pointer_action->setChecked(true);
    }
}
#endif

Form::State Form::state() const
{
    return d->state;
}

void Form::addPropertyCommand(const QByteArray &wname, const QVariant &oldValue,
                              const QVariant &value, const QByteArray &propertyName,
                              AddCommandOption addOption, uint idOfPropertyCommand)
{
    QHash<QByteArray, QVariant> oldValues;
    oldValues.insert(wname, oldValue);
    addPropertyCommand(oldValues, value, propertyName, addOption, idOfPropertyCommand);
}

void Form::addPropertyCommand(const QHash<QByteArray, QVariant> &oldValues,
                              const QVariant &value, const QByteArray &propertyName,
                              AddCommandOption addOption, uint idOfPropertyCommand)
{
//! @todo add to merge in PropertyCommand...
#if 0
    qDebug() << d->propertySet[propertyName];
    qDebug() << "oldValue:" << oldValues << "value:" << value;
    qDebug() << "idOfPropertyCommand:" << idOfPropertyCommand;
    d->insideAddPropertyCommand = true;
    PropertyCommand *presentCommand = dynamic_cast<PropertyCommand*>( d->commandHistory->presentCommand() );
    if (   presentCommand
        && d->lastCommand == presentCommand
        && idOfPropertyCommand > 0
        && d->idOfPropertyCommand == idOfPropertyCommand)
    {
        d->lastCommand->setValue(value); // just change the value, 
                                         // to avoid multiple PropertyCommands that only differ by value
    }
    else {
        d->lastCommand = new PropertyCommand(*this, oldValues, value, propertyName);
        if (!addCommand(d->lastCommand, execute)) {
            d->lastCommand = 0;
        }
        d->idOfPropertyCommand = idOfPropertyCommand;
    }
    d->insideAddPropertyCommand = false;
#endif
    d->insideAddPropertyCommand = true;
    d->lastCommand = new PropertyCommand(*this, oldValues, value, propertyName);
    d->lastCommand->setUniqueId(idOfPropertyCommand);
    //qDebug() << "ADD:" << *d->lastCommand;
    if (!addCommand(d->lastCommand, addOption)) {
        d->lastCommand = 0;
    }
    d->insideAddPropertyCommand = false;
}

void Form::addPropertyCommandGroup(PropertyCommandGroup *commandGroup,
                                   AddCommandOption addOption, uint idOfPropertyCommand)
{
//! @todo add to merge in PropertyCommand...?
#if 0
    if (!commandGroup || commandGroup->commands().isEmpty())
        return;
    qDebug() << "count:" << commandGroup->commands().count();
    qDebug() << "idOfPropertyCommand:" << idOfPropertyCommand;
    d->insideAddPropertyCommand = true;
    PropertyCommandGroup *presentCommand = dynamic_cast<PropertyCommandGroup*>( d->commandHistory->presentCommand() );
    if (   presentCommand
        && d->lastCommandGroup == presentCommand
        && idOfPropertyCommand > 0
        && d->idOfPropertyCommand == idOfPropertyCommand)
    {
        presentCommand->copyPropertyValuesFrom(*commandGroup); // just change the values, 
                                                       // to avoid multiple CommandsGroups
                                                       // that only differ by values
        delete commandGroup;
    }
    else {
        d->lastCommandGroup = commandGroup;
        addCommand(d->lastCommandGroup, execute);
        d->idOfPropertyCommand = idOfPropertyCommand;
    }
#endif
    d->insideAddPropertyCommand = true;
    d->lastCommandGroup = commandGroup;
    if (!addCommand(d->lastCommandGroup, addOption)) {
        d->lastCommandGroup = 0;
    }
    d->idOfPropertyCommand = idOfPropertyCommand;
    d->insideAddPropertyCommand = false;
}

void Form::slotPropertyChanged(KPropertySet& set, KProperty& p)
{
    Q_UNUSED(set);

    if (!d->slotPropertyChangedEnabled || !objectTree())
        return;

    const QByteArray property( p.name() );
    if (property.startsWith("this:"))
        return; //starts with magical prefix: it's a "meta" prop.

    const QVariant value( p.value() );

    // check if the name is valid (ie is correct identifier) and there is no name conflict
    if (property == "objectName") {
        if (d->selected.count() != 1) {
            qWarning() << "changing objectName property only allowed for single selection";
            return;
        }
        if (!isNameValid(value.toString()))
            return;
    }
    else if (property == "paletteBackgroundPixmap") {
        // a widget with a background pixmap should have its own origin
        // special types of properties handled separately
    }
    else if (property == "paletteBackgroundColor") {
        d->setColorProperty(p, &QWidget::backgroundRole, p.value());
        return;
    }
    else if (property == "paletteForegroundColor") {
        d->setColorProperty(p, &QWidget::foregroundRole, p.value());
        return;
    }
    else if (property == "autoFillBackground") {
        if (!p.value().toBool()) { // make background inherited
            d->setColorProperty(p, &QWidget::backgroundRole, QVariant());
        }
    }
    else if (property == "hAlign" || property == "vAlign" || property == "wordbreak") {
        saveAlignProperty(property);
        return;
    }

    // make sure we are not already undoing -> avoid recursion
    if (d->isUndoing && !d->isRedoing) {
        return;
    }

    if (d->selected.count() == 1) { // one widget selected
        // If the last command is the same, we just change its value
//! @todo add to merge in PropertyCommand if needed
        if (d->slotPropertyChanged_addCommandEnabled && !d->isRedoing) {
            addPropertyCommand(d->selected.first()->objectName().toLatin1(),
                               p.oldValue(), value, property, DontExecuteCommand);
        }

        // If the property is changed, we add it in ObjectTreeItem modifProp
        ObjectTreeItem *tree = objectTree()->lookup(d->selected.first()->objectName());
        if (tree && p.isModified()) {
            tree->addModifiedProperty(property, d->selected.first()->property(property));
        }

        if (property == "objectName") {
            changeName(d->selected.first()->objectName().toLatin1(), p.value().toByteArray());
            emit widgetNameChanged(d->selected.first()->objectName().toLatin1(), p.value().toByteArray());
        }
        d->selected.first()->setProperty(property, value);
        handleWidgetPropertyChanged(d->selected.first(), property, value);
    }
    else {
//! @todo add to merge in PropertyCommand if needed
        if (d->slotPropertyChanged_addCommandEnabled && !d->isRedoing) {
            // We store old values for each widget
            QHash<QByteArray, QVariant> oldValues;
            foreach(QWidget* widget, d->selected) {
                oldValues.insert(widget->objectName().toLatin1(), widget->property(property));
            }
            addPropertyCommand(oldValues, value, property, DontExecuteCommand);
        }
        foreach(QWidget* widget, d->selected) {
            ObjectTreeItem *titem = objectTree()->lookup(widget->objectName());
            if (titem && p.isModified())
                titem->addModifiedProperty(property, widget->property(property));
            widget->setProperty(property, value);
            handleWidgetPropertyChanged(widget, property, value);
        }
    }
}

void Form::slotPropertyReset(KPropertySet& set, KProperty& property)
{
    Q_UNUSED(set);

    if (d->selected.count() < 2)
        return;

    // We use the old value in modifProp for each widget
    foreach(QWidget* widget, d->selected) {
        ObjectTreeItem *titem = objectTree()->lookup(widget->objectName());
        if (titem && titem->modifiedProperties()->contains(property.name()))
            widget->setProperty(
                property.name(), titem->modifiedProperties()->find(property.name()).value());
    }
}

bool Form::isNameValid(const QString &name) const
{
    if (d->selected.isEmpty())
        return false;
//! @todo add to the undo buffer
    QWidget *w = d->selected.first();
    //also update widget's name in QObject member
    if (!KexiDB::isIdentifier(name)) {
        KMessageBox::sorry(widget(),
                           xi18n("Could not rename widget \"%1\" to \"%2\" because "
                                "\"%3\" is not a valid name (identifier) for a widget.",
                                w->objectName(), name, name));
        d->slotPropertyChangedEnabled = false;
        d->propertySet["objectName"].resetValue();
        d->slotPropertyChangedEnabled = true;
        return false;
    }

    if (objectTree()->lookup(name)) {
        KMessageBox::sorry(widget(),
                           xi18n("Could not rename widget \"%1\" to \"%2\" "
                                "because a widget with the name \"%3\" already exists.",
                                w->objectName(), name, name));
        d->slotPropertyChangedEnabled = false;
        d->propertySet["objectName"].resetValue();
        d->slotPropertyChangedEnabled = true;
        return false;
    }

    return true;
}

void Form::undo()
{
    if (!objectTree())
        return;
    if (!d->undoStack.canUndo()) {
        qWarning() << "cannot redo";
        return;
    }

    const bool saveExecutingCommand = !d->executingCommand;
    //qDebug() << "saveExecutingCommand:" << saveExecutingCommand;
    if (saveExecutingCommand)
        d->executingCommand = dynamic_cast<const Command*>(d->undoStack.command(0));
    //qDebug() << d->undoStack.index();
    //qDebug() << d->executingCommand;

    d->undoStack.undo();

    if (saveExecutingCommand)
        d->executingCommand = 0;
}

void Form::redo()
{
    if (!objectTree())
        return;
    if (!d->undoStack.canRedo()) {
        qWarning() << "cannot redo";
        return;
    }
    d->isRedoing = true;
    const bool saveExecutingCommand = !d->executingCommand;
    //qDebug() << "saveExecutingCommand:" << saveExecutingCommand;
    if (saveExecutingCommand)
        d->executingCommand = dynamic_cast<const Command*>(d->undoStack.command(d->undoStack.index()));
    //qDebug() << d->undoStack.index();
    //qDebug() << *d->executingCommand;

    d->undoStack.redo();

    if (saveExecutingCommand)
        d->executingCommand = 0;
    d->isRedoing = false;
}

bool Form::isRedoing() const
{
    return d->isRedoing;
}

void Form::setUndoing(bool undoing)
{
    d->isUndoing = undoing;
}

bool Form::isUndoing() const
{
    return d->isUndoing;
}

bool Form::isPropertyVisible(const QByteArray &property, bool isTopLevel,
                             const QByteArray &classname) const
{
    const bool multiple = d->selected.count() >= 2;
    if (multiple && classname.isEmpty())
        return false;

    QWidget *w = d->selected.first();
    WidgetWithSubpropertiesInterface* subpropIface
        = dynamic_cast<WidgetWithSubpropertiesInterface*>(w);
    QWidget *subwidget;
    if (subpropIface && subpropIface->findMetaSubproperty(property).isValid()) // special case - subproperty
        subwidget = subpropIface->subwidget();
    else
        subwidget = w;

    return library()->isPropertyVisible(
               subwidget->metaObject()->className(), subwidget, property, multiple, isTopLevel);
}

void Form::addWidget(QWidget *w)
{
    d->selected.append(w);

    // Reset some stuff
    d->lastCommand = 0;
    d->lastCommandGroup = 0;

    QByteArray classname;
    if (d->selected.first()->metaObject()->className() == w->metaObject()->className()) {
        classname = d->selected.first()->metaObject()->className();
    }

    // show only properties shared by widget (properties chosen by factory)
    bool isTopLevel = isTopLevelWidget(w);

    for (KPropertySet::Iterator it(d->propertySet); it.current(); ++it) {
        //qDebug() << it.current();
        if (!isPropertyVisible(it.current()->name(), isTopLevel, classname)) {
            it.current()->setVisible(false);
        }
    }

    if (d->selected.count() >= 2) {
        //second widget, update metainfo
        d->propertySet["this:className"].setValue("special:multiple");
        d->propertySet["this:classString"].setValue(
            xi18n("Multiple Widgets (%1)", d->selected.count()));
        d->propertySet["this:iconName"].setValue("multiple_obj");
        //name doesn't make sense for now
        d->propertySet["objectName"].setValue("");
    }
}

void Form::createPropertiesForWidget(QWidget *w)
{
    d->propertySet.clear();

    if (!objectTree()) {
        qWarning() << "no object tree!";
        return;
    }
    ObjectTreeItem *tree = objectTree()->lookup(w->objectName());
    if (!tree)
        return;

    const QHash<QString, QVariant>* modifiedProperties = tree->modifiedProperties();
    QHash<QString, QVariant>::ConstIterator modifiedPropertiesIt;
    bool isTopLevel = isTopLevelWidget(w);
    KProperty *newProp = 0;
    WidgetInfo *winfo = library()->widgetInfoForClassName(w->metaObject()->className());
    if (!winfo) {
        qWarning() << "no widget info for class" << w->metaObject()->className();
        return;
    }

//! @todo ineffective, get property names directly
    const QList<QMetaProperty> propList(
        KexiUtils::propertiesForMetaObjectWithInherited(w->metaObject()));
    //qDebug() << "propList.count() ==" << propList.count();
    QSet<QByteArray> propNames;
    foreach(const QMetaProperty& mp, propList) {
        propNames.insert(mp.name());
    }

    // add subproperties if available
    WidgetWithSubpropertiesInterface* subpropIface
        = dynamic_cast<WidgetWithSubpropertiesInterface*>(w);
    if (subpropIface) {
        const QSet<QByteArray> subproperties(subpropIface->subproperties());
        foreach(const QByteArray& propName, subproperties) {
            propNames.insert(propName);
            //qDebug() << "Added subproperty: " << propName;
        }
    }

    // iterate over the property list, and create Property objects
    foreach(const QByteArray& propName, propNames) {
        //qDebug() << ">> " << propName;
        const QMetaProperty subMeta = // special case - subproperty
            subpropIface ? subpropIface->findMetaSubproperty(propName) : QMetaProperty();
        const QMetaProperty meta = subMeta.isValid() ? subMeta
                                   : KexiUtils::findPropertyWithSuperclasses(w, propName.constData());
        if (!meta.isValid()) {
            //qDebug() << "!meta.isValid()";
            continue;
        }
        const char* propertyName = meta.name();
        QWidget *subwidget = subMeta.isValid()//subpropIface
                             ? subpropIface->subwidget() : w;
        WidgetInfo *subwinfo = library()->widgetInfoForClassName(
                                   subwidget->metaObject()->className());
//  qDebug() << "$$$ " << subwidget->className();

        if (   subwinfo
            && meta.isDesignable(subwidget)
            && meta.isWritable()
            && meta.isReadable() && !d->propertySet.contains(propertyName)
           )
        {
            //! \todo add another list for property description
            QString desc(d->propCaption.value(meta.name()));
            //! \todo change i18n
            if (desc.isEmpty()) { //try to get property description from factory
                desc = library()->propertyDescForName(subwinfo, propertyName);
            }

            modifiedPropertiesIt = modifiedProperties->find(propertyName);
            const bool oldValueExists = modifiedPropertiesIt != modifiedProperties->constEnd();

            if (meta.isEnumType()) {
                if (qstrcmp(propertyName, "alignment") == 0)  {
                    createAlignProperty(meta, w, subwidget);
                    continue;
                }

                QStringList keys(KexiUtils::enumKeysForProperty(meta));
                newProp = new KProperty(
                    propertyName, d->createValueList(subwinfo, keys),
                    // assign current or older value
                    meta.enumerator().valueToKey(
                        oldValueExists ? modifiedPropertiesIt.value().toInt()
                        : subwidget->property(propertyName).toInt()),
                    desc, desc);
                //now set current value, so the old one is stored as old
                if (oldValueExists) {
                    newProp->setValue(
                        meta.enumerator().valueToKey(subwidget->property(propertyName).toInt()));
                }
            }
            else {
                int realType = subwinfo->customTypeForProperty(propertyName);
                if (realType == KoProperty::Invalid || realType == KoProperty::Auto) {
                    realType = meta.type();
                }
                newProp = new KoProperty::Property(
                    propertyName,
                    // assign current or older value
                    oldValueExists ? modifiedPropertiesIt.value() : subwidget->property(propertyName),
                    desc, desc, realType
                );
                //now set current value, so the old one is stored as old
                if (oldValueExists) {
                    newProp->setValue(subwidget->property(propertyName));
                }
            }

            if (!isPropertyVisible(propertyName, isTopLevel))
                newProp->setVisible(false);
//! @todo
            if (newProp->type() == KoProperty::Invalid) {
                newProp->setType(KoProperty::String);
            }

            d->propertySet.addProperty(newProp);
        }

        // update the Property.oldValue() and isModified() using the value stored in the ObjectTreeItem
        updatePropertyValue(tree, propertyName, meta);
    }

    const QString paletteBackgroundColorDesc(d->propCaption.value("paletteBackgroundColor"));
    newProp = new KProperty("paletteBackgroundColor",
                                       w->palette().color(w->backgroundRole()),
                                       paletteBackgroundColorDesc,
                                       paletteBackgroundColorDesc);

    const QString paletteForegroundColorDesc(d->propCaption.value("paletteForegroundColor"));
    d->propertySet.addProperty(newProp);
    newProp = new KProperty("paletteForegroundColor",
                                       w->palette().color(w->foregroundRole()),
                                       paletteForegroundColorDesc,
                                       paletteForegroundColorDesc);
    d->propertySet.addProperty(newProp);

    d->propertySet["objectName"].setAutoSync(false); // name should be updated only when pressing Enter

    if (winfo) {
        library()->setPropertyOptions(d->propertySet, *winfo, w);
        d->propertySet.addProperty(newProp = new KProperty("this:classString", winfo->name()));
        newProp->setVisible(false);
        d->propertySet.addProperty(newProp = new KProperty("this:iconName", winfo->iconName()));
        newProp->setVisible(false);
    }
    d->propertySet.addProperty(newProp = new KProperty("this:className",
            w->metaObject()->className()));
    newProp->setVisible(false);
}

void Form::updatePropertyValue(ObjectTreeItem *tree, const char *property, const QMetaProperty &meta)
{
    Q_UNUSED(tree);
    Q_UNUSED(property);
    Q_UNUSED(meta);
    return;
//! @todo ????
#if 0
    const char *propertyName = meta.isValid() ? meta.name() : property;
    if (!d->propertySet.contains(propertyName))
        return;
    KProperty &p = d->propertySet[propertyName];

//! \todo what about set properties, and lists properties
    const QHash<QString, QVariant>::ConstIterator it(tree->modifiedProperties()->find(propertyName));
    if (it != tree->modifiedProperties()->constEnd()) {
        blockSignals(true);
        if (meta.isValid() && meta.isEnumType()) {
            p.setValue(meta.enumerator().valueToKey(it.value().toInt()), false);
        }
        else {
            p.setValue(it.value(), false);
        }
        p.setValue(p.value(), true);
        blockSignals(false);
    }
#endif
}

//! @todo what about 'forceReload' arg? It's not passed to updatePropertiesForSelection() now...
void Form::emitSelectionChanged(QWidget *w, WidgetSelectionFlags flags)
{
    updatePropertiesForSelection(w, flags);
    emit selectionChanged(w, flags);
}

void Form::updatePropertiesForSelection(QWidget *w, WidgetSelectionFlags flags)
{
    if (!w) {
//! @todo clearSet()?
        return;
    }

    // if our list is empty,don't use add parameter value
    if (d->selected.isEmpty() == 0) {
        flags |= ReplacePreviousSelection;
    }

    QByteArray prevProperty;
    if (flags & ReplacePreviousSelection) {
        createPropertiesForWidget(w);
        w->installEventFilter(this);
        connect(w, SIGNAL(destroyed()), this, SLOT(widgetDestroyed()));
    }
    else {
        addWidget(w);
    }

    if (flags & LastSelection) {
        emit propertySetSwitched();
    }
 }

KPropertySet& Form::propertySet()
{
    return d->propertySet;
}

bool Form::isSnapToGridEnabled() const
{
    return d->snapToGrid;
}

void Form::setSnapToGridEnabled(bool enabled)
{
    d->snapToGrid = enabled;
}

void Form::createContextMenu(QWidget *w, Container *container, const QPoint& menuPos,
                             ContextMenuTarget target)
{
    if (!widget())
        return;
    const bool toplevelWidgetSelected = widget() == w;
    const uint widgetsCount = container->form()->selectedWidgets()->count();
    const bool multiple = widgetsCount > 1;

    //set title
    QString n( container->form()->library()->displayName(w->metaObject()->className()) );
    QIcon icon;
    QString titleText;
    if (!multiple) {
        if (w == container->form()->widget()) {
            icon = koIcon("form");
            titleText = xi18n("%1 : Form", w->objectName());
        }
        else {
            icon = QIcon::fromTheme(
                       container->form()->library()->iconName(w->metaObject()->className()));
            titleText = QString(w->objectName()) + " : " + n;
        }
    }
    else {
        icon = koIcon("multiple_obj");
        titleText = xi18n("Multiple Widgets (%1)", widgetsCount);
    }

    QMenu menu;
    menu.addSection(icon, titleText);

    QAction *a;
#define PLUG_ACTION(_name, forceVisible) \
    { a = d->collection->action(_name); \
        if (a && (forceVisible || a->isEnabled())) { \
            if (separatorNeeded) \
                menu.addSeparator(); \
            separatorNeeded = false; \
            menu.addAction(a); \
        } \
    }

    bool separatorNeeded = false;

    PLUG_ACTION("edit_cut", !toplevelWidgetSelected);
    PLUG_ACTION("edit_copy", !toplevelWidgetSelected);
    PLUG_ACTION("edit_paste", true);
    PLUG_ACTION("edit_delete", !toplevelWidgetSelected);
    separatorNeeded = true;
    PLUG_ACTION("align_menu", !toplevelWidgetSelected);
    PLUG_ACTION("adjust_size_menu", !toplevelWidgetSelected);
    separatorNeeded = true;

    // We create the buddy menu
    QAction *noBuddyAction = 0;
    QLabel *buddyLabelWidget = 0;
    QList<QString> sortedItemNames;
    if (!multiple) {
        buddyLabelWidget = qobject_cast<QLabel*>(w);
        if (buddyLabelWidget) {
            if (!buddyLabelWidget->text().contains("&")
                || buddyLabelWidget->textFormat() == Qt::RichText)
            {
                buddyLabelWidget = 0;
            }
        }
    }
    if (buddyLabelWidget) { // setup menu
        if (separatorNeeded)
            menu.addSeparator();

        QMenu *sub = new QMenu(w);
        QWidget *buddy = buddyLabelWidget->buddy();

        noBuddyAction = sub->addAction(xi18n("No Buddy"));
        if (!buddy)
            noBuddyAction->setChecked(true);
        sub->addSeparator();

        // Add all the widgets that can have focus
        // 1. Sort by name
        QHash<QString, ObjectTreeItem*> items;
        foreach (ObjectTreeItem *item, *container->form()->tabStops()) {
            items.insert(item->name().toLatin1(), item);
        }
        sortedItemNames = items.keys();
        qSort(sortedItemNames);
        foreach (const QString& name, sortedItemNames) {
            ObjectTreeItem *item = items.value(name);
            QAction* action = sub->addAction(
                QIcon::fromTheme(
                    container->form()->library()->iconName(item->className().toLatin1())),
                item->name()
            );
            if (item->widget() == buddy)
                action->setChecked(true);
        }
        separatorNeeded = true;
    }

#ifdef KFD_SIGSLOTS
    if (!multiple && (d->features & EnableEvents)) {
        if (separatorNeeded)
            menu.addSeparator();

        // We create the signals menu
        QMenu *sigMenu = new QMenu();
        const QList<QMetaMethod> list(
            KexiUtils::methodsForMetaObjectWithParents(w->metaObject(), QMetaMethod::Signal,
                    QMetaMethod::Public));
        foreach(const QMetaMethod& m, list) {
            sigMenu->addAction(m.signature());
        }
        QAction *eventsSubMenuAction = menu.addMenu(sigMenu);
        eventsSubMenuAction->setText(futureI18n("Events"));
        if (list.isEmpty())
            eventsSubMenuAction->setEnabled(false);
        connect(sigMenu, SIGNAL(triggered(QAction*)),
                this, SLOT(menuSignalChosen(QAction*)));
        separatorNeeded = true;
    }
#endif

    // Other items
    if (!multiple) {
        QAction* lastAction = 0;
        if (separatorNeeded) {
            lastAction = menu.addSeparator();
        }
        const uint oldIndex = menu.actions().count() - 1;
        container->form()->library()
            ->createMenuActions(w->metaObject()->className(), w, &menu, container);
        if (oldIndex == uint(menu.actions().count() - 1)) {
            //nothing added
            if (separatorNeeded) {
                menu.removeAction(lastAction);
            }
        }
    }

    //show the menu at the selected widget
    QPoint pos;
    switch (target) {
    case FormContextMenuTarget: {
        pos = w->mapToGlobal(menuPos);
        d->insertionPoint = menuPos;
        break;
    }
    case WidgetTreeContextMenuTarget: {
        pos = QCursor::pos();
        d->insertionPoint = container->widget()->mapToGlobal(w->pos() + QPoint(10, 10)); // user may still want to paste
        break;
    }
    }

    //qDebug() << w << container->widget() << "menuPos=" << menuPos << "pos=" << pos;
    QAction *result = menu.exec(pos);
    if (!result) {
        // nothing to do
    }
    else if (noBuddyAction && buddyLabelWidget && result == noBuddyAction) {
        buddyLabelWidget->setBuddy(0);
    }
    else if (sortedItemNames.contains(result->text())) {
        ObjectTreeItem *item = objectTree()->lookup(result->text());
        if (item && item->widget()) {
            buddyLabelWidget->setBuddy(item->widget());
        }
    }
    d->insertionPoint = QPoint();
}

void Form::deleteWidget()
{
    if (!objectTree()) {
        return;
    }

    QWidgetList *list = selectedWidgets();
    if (list->isEmpty()) {
        return;
    }

    if (widget() == list->first()) {
        //toplevel form is selected, cannot delete it
        return;
    }

    Command *com = new DeleteWidgetCommand(*this, *list);
    addCommand(com);
}

void Form::copyWidget()
{
    if (!objectTree() || isFormWidgetSelected()) {
        return;
    }

    QWidgetList *list = selectedWidgets();
    if (list->isEmpty()) {
        return;
    }

    QDomDocument doc;
    QHash<QByteArray, QByteArray> containers;
    QHash<QByteArray, QByteArray> parents;
    KFormDesigner::widgetsToXML(doc,
        containers, parents, *this, *list);
    KFormDesigner::copyToClipboard(doc.toString());
    emitActionSignals(); // to update 'Paste' item state
    emitUndoActionSignals();
}

bool Form::isFormWidgetSelected() const
{
    return selectedWidget() && selectedWidget() == widget();
}

void Form::cutWidget()
{
    if (!objectTree() || isFormWidgetSelected()) {
        return;
    }

    QWidgetList *list = selectedWidgets();
    if (list->isEmpty()) {
        return;
    }

    Command *com = new CutWidgetCommand(*this, *list);
    addCommand(com);
}

void Form::pasteWidget()
{
    if (!objectTree()) {
        return;
    }
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();
    const bool mimeDataHasXmlUiFormat = mimeData->hasFormat( KFormDesigner::mimeType() );
    if (!mimeDataHasXmlUiFormat && !mimeData->hasText()) {
        return;
    }
    QDomDocument doc;
    if (!doc.setContent( mimeDataHasXmlUiFormat 
        ? QString::fromUtf8( mimeData->data(KFormDesigner::mimeType())) : mimeData->text() ))
    {
        return;
    }
    if (!doc.firstChildElement("UI").hasChildNodes()) {
        return;
    }

    Command *com = new PasteWidgetCommand(doc, *activeContainer(), d->insertionPoint);
    addCommand(com);
}

void Form::editTabOrder()
{
    if (!objectTree()) {
        return;
    }
    QWidget *topLevel = widget()->topLevelWidget();
    TabStopDialog dlg(topLevel);
    if (dlg.exec(this) == QDialog::Accepted) {
        d->propertySet.changePropertyIfExists("autoTabStops", dlg.autoTabStops());
        //force set dirty
        setModified(true);
    }
}

void Form::editFormPixmapCollection()
{
    if (!objectTree()) {
        return;
    }
//! @todo pixmapcollection
#ifndef KEXI_NO_PIXMAPCOLLECTION
    PixmapCollectionEditor dialog(pixmapCollection(), widget()->topLevelWidget());
    dialog.exec();
#endif
}

void Form::editConnections()
{
#ifdef KFD_SIGSLOTS
    if (!(d->features & EnableConnections)) {
        return;
    }
    if (!objectTree()) {
        return;
    }

    ConnectionDialog dialog(this, widget()->topLevelWidget());
    dialog.exec();
#endif
}

void Form::alignWidgets(WidgetAlignment alignment)
{
    QWidgetList* selected = selectedWidgets();
    if (!objectTree() || selected->count() < 2) {
        return;
    }

    QWidget *parentWidget = selected->first()->parentWidget();

    foreach (QWidget *w, *selected) {
        if (w->parentWidget() != parentWidget) {
            //qDebug() << "alignment ==" << alignment <<  "widgets don't have the same parent widget";
            return;
        }
    }

    Command *com = new AlignWidgetsCommand(*this, alignment, *selected);
    addCommand(com);
}

void Form::alignWidgetsToLeft()
{
    alignWidgets(AlignToLeft);
}

void Form::alignWidgetsToRight()
{
    alignWidgets(AlignToRight);
}

void Form::alignWidgetsToTop()
{
    alignWidgets(AlignToTop);
}

void Form::alignWidgetsToBottom()
{
    alignWidgets(AlignToBottom);
}

void Form::adjustWidgetSize()
{
    if (!objectTree()) {
        return;
    }
    Command *com = new AdjustSizeCommand(*this, AdjustSizeCommand::SizeToFit, *selectedWidgets());
    addCommand(com);
}

void Form::alignWidgetsToGrid()
{
    if (!objectTree()) {
        return;
    }
    Command *com = new AlignWidgetsCommand(*this, AlignToGrid, *selectedWidgets());
    addCommand(com);
}

void Form::adjustSizeToGrid()
{
    if (!objectTree()) {
        return;
    }
    Command *com = new AdjustSizeCommand(*this, AdjustSizeCommand::SizeToGrid, *selectedWidgets());
    addCommand(com);
}

void Form::adjustWidthToSmall()
{
    if (!objectTree()) {
        return;
    }
    Command *com = new AdjustSizeCommand(*this, AdjustSizeCommand::SizeToSmallWidth, *selectedWidgets());
    addCommand(com);
}

void Form::adjustWidthToBig()
{
    if (!objectTree()) {
        return;
    }
    Command *com = new AdjustSizeCommand(*this, AdjustSizeCommand::SizeToBigWidth, *selectedWidgets());
    addCommand(com);
}

void Form::adjustHeightToSmall()
{
    if (!objectTree()) {
        return;
    }
    Command *com = new AdjustSizeCommand(*this, AdjustSizeCommand::SizeToSmallHeight, *selectedWidgets());
    addCommand(com);
}

void Form::adjustHeightToBig()
{
    if (!objectTree()) {
        return;
    }
    Command *com = new AdjustSizeCommand(*this, AdjustSizeCommand::SizeToBigHeight, *selectedWidgets());
    addCommand(com);
}

void Form::bringWidgetToFront()
{
    if (!objectTree()) {
        return;
    }
    foreach (QWidget *w, *selectedWidgets()) {
        w->raise();
    }
}

void Form::sendWidgetToBack()
{
    if (!objectTree()) {
        return;
    }

    foreach (QWidget *w, *selectedWidgets()) {
        w->lower();
    }
}

void Form::selectAll()
{
    if (!objectTree()) {
        return;
    }
    selectFormWidget();
    uint count = objectTree()->children()->count();
    foreach (ObjectTreeItem *titem, *objectTree()->children()) {
        selectWidget(
            titem->widget(),
            AddToPreviousSelection | ((count > 1) ? MoreWillBeSelected : LastSelection)
        );
        count--;
    }
}

void Form::clearWidgetContent()
{
    if (!objectTree()) {
        return;
    }
    foreach (QWidget *w, *selectedWidgets()) {
        library()->clearWidgetContent(w->metaObject()->className(), w);
    }
}

// Alignment-related functions /////////////////////////////

void Form::createAlignProperty(const QMetaProperty& meta, QWidget *widget, QWidget *subwidget)
{
    if (!objectTree())
        return;

    const int alignment = subwidget->property("alignment").toInt();
    const QList<QByteArray> keys(meta.enumerator().valueToKeys(alignment).split('|'));
    //qDebug() << "keys:" << keys;

    const QStringList possibleValues(KexiUtils::enumKeysForProperty(meta));
    //qDebug() << "possibleValues:" << possibleValues;
    ObjectTreeItem *tree = objectTree()->lookup(widget->objectName());
    const bool isTopLevel = isTopLevelWidget(widget);

    if (possibleValues.contains("AlignHCenter"))  {
        // Create the horizontal alignment property
        QString value;
        if (keys.contains("AlignHCenter") || keys.contains("AlignCenter"))
            value = "AlignHCenter";
        else if (keys.contains("AlignRight"))
            value = "AlignRight";
        else if (keys.contains("AlignLeft"))
            value = "AlignLeft";
        else if (keys.contains("AlignJustify"))
            value = "AlignJustify";
        else
            value = "AlignAuto";

        QStringList list;
        list << "AlignAuto" << "AlignLeft" << "AlignRight"
            << "AlignHCenter" << "AlignJustify";
        KProperty *p = new KProperty(
            "hAlign", d->createValueList(0, list), value,
            xi18nc("Translators: please keep this string short (less than 20 chars)", "Hor. Alignment"),
            xi18n("Horizontal Alignment"));
        d->propertySet.addProperty(p);
        if (!isPropertyVisible(p->name(), isTopLevel)) {
            p->setVisible(false);
        }
        updatePropertyValue(tree, "hAlign");
    }

    if (possibleValues.contains("AlignTop")) {
        // Create the ver alignment property
        QString value;
        if (keys.contains("AlignTop"))
            value = "AlignTop";
        else if (keys.contains("AlignBottom"))
            value = "AlignBottom";
        else
            value = "AlignVCenter";

        QStringList list;
        list << "AlignTop" << "AlignVCenter" << "AlignBottom";
        KProperty *p = new KProperty(
            "vAlign", d->createValueList(0, list), value,
            xi18nc("Translators: please keep this string short (less than 20 chars)", "Ver. Alignment"),
            xi18n("Vertical Alignment"));
        d->propertySet.addProperty(p);
        if (!isPropertyVisible(p->name(), isTopLevel)) {
            p->setVisible(false);
        }
        updatePropertyValue(tree, "vAlign");
    }
    
    if (possibleValues.contains("WordBreak")) {
        // Create the wordbreak property
        KProperty *p = new KProperty("wordbreak",
                QVariant((bool)(alignment & Qt::TextWordWrap)),
                xi18n("Word Break"), xi18n("Word Break"));
        d->propertySet.addProperty(p);
        updatePropertyValue(tree, "wordbreak");
        if (!library()->isPropertyVisible(
                subwidget->metaObject()->className(), subwidget, p->name(), false/*multiple*/, isTopLevel))
        {
            p->setVisible(false);
        }
    }
}

void Form::saveAlignProperty(const QString &property)
{
    QStringList list;
    if (d->propertySet.contains("hAlign"))
        list.append(d->propertySet["hAlign"].value().toString());
    if (d->propertySet.contains("vAlign"))
        list.append(d->propertySet["vAlign"].value().toString());
    if (d->propertySet.contains("wordbreak") && d->propertySet["wordbreak"].value().toBool())
        list.append("WordBreak");

    WidgetWithSubpropertiesInterface* subpropIface
        = dynamic_cast<WidgetWithSubpropertiesInterface*>(d->selected.first());
    QWidget *subwidget = (subpropIface && subpropIface->subwidget())
                         ? subpropIface->subwidget() : (QWidget*)d->selected.first();
    int count = subwidget->metaObject()->indexOfProperty("alignment");
    const QMetaProperty meta( subwidget->metaObject()->property(count) );
    const int valueForKeys = meta.enumerator().keysToValue(list.join("|").toLatin1());
    subwidget->setProperty("alignment", valueForKeys);

    ObjectTreeItem *tree = objectTree()->lookup(d->selected.first()->objectName());
    if (tree && d->propertySet[ property.toLatin1()].isModified()) {
        tree->addModifiedProperty(
            property.toLatin1(), d->propertySet[property.toLatin1()].oldValue());
    }

    if (d->isUndoing) {
        return;
    }

    if (d->lastCommand && d->lastCommand->propertyName() == "alignment") {
        d->lastCommand->setValue(valueForKeys);
    }
    else {
        d->lastCommand = new PropertyCommand(*this, d->selected.first()->objectName().toLatin1(),
                                 subwidget->property("alignment"), valueForKeys, "alignment");
        if (!addCommand(d->lastCommand, DontExecuteCommand)) {
            d->lastCommand = 0;
        }
    }
}

void Form::createPropertyCommandsInDesignMode(QWidget* widget,
        const QHash<QByteArray, QVariant> &propValues, Command *parentCommand, bool addToActiveForm)
{
    if (!widget || propValues.isEmpty())
        return;

    //is this widget selected? (if so, use property system)
    const bool widgetIsSelected = selectedWidget() == widget;

    d->slotPropertyChanged_addCommandEnabled = false;
    QHash<QByteArray, QVariant>::ConstIterator endIt = propValues.constEnd();
    for (QHash<QByteArray, QVariant>::ConstIterator it = propValues.constBegin(); it != endIt; ++it) {
        if (!d->propertySet.contains(it.key())) {
            qWarning() << "\"" << it.key() << "\" property not found";
            continue;
        }
        (void)new PropertyCommand(*this, widget->objectName().toLatin1(),
                                  widget->property(it.key()), it.value(), it.key(), parentCommand);
        if (widgetIsSelected) {
            d->propertySet.changeProperty(it.key(), it.value());
        }
        else {
            WidgetWithSubpropertiesInterface* subpropIface
                = dynamic_cast<WidgetWithSubpropertiesInterface*>(widget);
            QWidget *subwidget
                = (subpropIface && subpropIface->subwidget()) ? subpropIface->subwidget() : widget;
            if (subwidget && -1 != subwidget->metaObject()->indexOfProperty(it.key())
                    && subwidget->property(it.key()) != it.value()) {
                ObjectTreeItem *tree = objectTree()->lookup(widget->objectName());
                if (tree) {
                    tree->addModifiedProperty(it.key(), subwidget->property(it.key()));
                }
                subwidget->setProperty(it.key(), it.value());
                handleWidgetPropertyChanged(widget, it.key(), it.value());
            }
        }
    }
    d->lastCommand = 0;
    d->lastCommandGroup = 0;
    if (addToActiveForm) {
        addCommand(parentCommand, DontExecuteCommand);
    }
    d->slotPropertyChanged_addCommandEnabled = true;
}

void Form::handleWidgetPropertyChanged(QWidget *w, const QByteArray &name, const QVariant &value)
{
    Q_UNUSED(w);

    if (name == "autoTabStops") {
        //update autoTabStops setting at KFD::Form level
        setAutoTabStops(value.toBool());
    }
    else if (name == "geometry" && widget()) {
        //fall back to sizeInternal property....
        d->propertySet.changePropertyIfExists("sizeInternal", value.toRect().size());
    }
}

void Form::changeFont()
{
    QWidgetList *wlist = selectedWidgets();
    QWidgetList widgetsWithFontProperty;
    QFont font;
    bool oneFontSelected = true;
    foreach (QWidget* widget, *wlist) {
        if (library()->isPropertyVisible(widget->metaObject()->className(), widget, "font")) {
            widgetsWithFontProperty.append(widget);
            if (oneFontSelected) {
                if (widgetsWithFontProperty.count() == 1)
                    font = widget->font();
                else if (font != widget->font())
                    oneFontSelected = false;
            }
        }
    }
    if (widgetsWithFontProperty.isEmpty())
        return;
    if (!oneFontSelected) //many different fonts selected: pick a font from toplevel conatiner
        font = widget()->font();

    if (1 == widgetsWithFontProperty.count()) {
        //single widget's settings
        if (QDialog::Accepted != KFontDialog::getFont(
                font, KFontChooser::NoDisplayFlags, widget()))
        {
            return;
        }
        d->propertySet.changeProperty("font", font);
        return;
    }
    //multiple widgets
    QFlags<KFontChooser::FontDiff> diffFlags = KFontChooser::NoFontDiffFlags;
    if (QDialog::Accepted != KFontDialog::getFontDiff(
                font, diffFlags, KFontChooser::NoDisplayFlags, widget())
            || 0 == diffFlags) {
        return;
    }
    //update font
    foreach (QWidget* widget, widgetsWithFontProperty) {
        QFont prevFont(widget->font());
        if (diffFlags & KFontChooser::FontDiffFamily)
            prevFont.setFamily(font.family());
        if (diffFlags & KFontChooser::FontDiffStyle) {
            prevFont.setBold(font.bold());
            prevFont.setItalic(font.italic());
        }
        if (diffFlags & KFontChooser::FontDiffSize) {
            prevFont.setPointSize(font.pointSize());
        }
        //! @todo this modification is not added to UNDO BUFFER:
        //!      do it when KPropertySet supports multiple selections
        widget->setFont(prevFont);
    }
//! @todo temporary fix for dirty flag
    setModified(true);
}

void Form::setSlotPropertyChangedEnabled(bool set)
{
    d->slotPropertyChangedEnabled = set;
}

void Form::createInlineEditor(const KFormDesigner::WidgetFactory::InlineEditorCreationArguments& args)
{
    if (!args.execute)
        return;
    if (args.multiLine) {
        KTextEdit *textedit = new KTextEdit(args.widget->parentWidget());
        textedit->setPlainText(args.text);
        textedit->setAlignment(args.alignment);
        if (dynamic_cast<QTextEdit*>(args.widget)) {
            textedit->setWordWrapMode(dynamic_cast<QTextEdit*>(args.widget)->wordWrapMode());
            textedit->setLineWrapMode(dynamic_cast<QTextEdit*>(args.widget)->lineWrapMode());
        }
        textedit->moveCursor(QTextCursor::End);
        textedit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textedit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //ok?
        textedit->setFrameShape(args.useFrame ? QFrame::StyledPanel : QFrame::NoFrame);
        textedit->show();
        textedit->setFocus();
        textedit->selectAll();
        d->inlineEditor = textedit;

        connect(textedit, SIGNAL(textChanged()), this, SLOT(slotInlineTextChanged()));
        connect(args.widget, SIGNAL(destroyed()), this, SLOT(widgetDestroyed()));
        connect(textedit, SIGNAL(destroyed()), this, SLOT(inlineEditorDeleted()));
    } else {
        QLineEdit *editor = new QLineEdit(args.widget->parentWidget());
        d->inlineEditor = editor;
        editor->setText(args.text);
        editor->setAlignment(args.alignment);
        editor->setFrame(args.useFrame);
        editor->show();
        editor->setFocus();
        editor->selectAll();
        connect(editor, SIGNAL(textChanged(QString)), this, SLOT(changeInlineTextInternal(QString)));
        connect(args.widget, SIGNAL(destroyed()), this, SLOT(widgetDestroyed()));
        connect(editor, SIGNAL(destroyed()), this, SLOT(inlineEditorDeleted()));
    }
    d->inlineEditor->installEventFilter(this);
    d->inlineEditor->setFont(args.widget->font());
    d->inlineEditor->setGeometry(args.geometry);
    // setup palette
    d->inlineEditor->setBackgroundRole(args.widget->backgroundRole());
    QPalette pal(args.widget->palette());
    QBrush baseBrush;
    if (args.transparentBackground) {
        baseBrush = QBrush(Qt::transparent);
    }
    else {
        baseBrush = pal.base();
        QColor baseColor(baseBrush.color());
        if (!args.widget->inherits("KexiCommandLinkButton")) { //! @todo HACK! any idea??
            baseColor.setAlpha(120);
        }
        baseBrush.setColor(baseColor);
    }
    pal.setBrush(QPalette::Base, baseBrush);
    pal.setBrush(d->inlineEditor->backgroundRole(), pal.brush(args.widget->backgroundRole()));
    pal.setBrush(d->inlineEditor->foregroundRole(), pal.brush(args.widget->foregroundRole()));
    d->inlineEditor->setPalette(pal);

    //copy properties if available
    WidgetWithSubpropertiesInterface* subpropIface
        = dynamic_cast<WidgetWithSubpropertiesInterface*>(args.widget);
    QWidget *subwidget = (subpropIface && subpropIface->subwidget())
                         ? subpropIface->subwidget() : args.widget;
    if (   -1 != d->inlineEditor->metaObject()->indexOfProperty("margin")
        && -1 != subwidget->metaObject()->indexOfProperty("margin"))
    {
        d->inlineEditor->setProperty("margin", subwidget->property("margin"));
    }
    ResizeHandleSet *handles = resizeHandlesForWidget(args.widget);
    if (handles) {
        handles->setEditingMode(true);
        handles->raise();
    }

    ObjectTreeItem *tree = args.container->form()->objectTree()->lookup(args.widget->objectName());
    if (!tree)
        return;
    tree->eventEater()->setContainer(this);

    d->inlineEditorContainer = args.container;
    d->editedWidgetClass = args.classname;
    d->originalInlineText = args.text;

    d->slotPropertyChangedEnabled = false;
    InlineTextEditingCommand command( // to update size of the widget
        *this, selectedWidget(), d->editedWidgetClass, args.text);
    command.execute();
    d->slotPropertyChangedEnabled = true;
}

void Form::changeInlineTextInternal(const QString& text)
{
    if (d->editedWidgetClass.isEmpty())
        return;
    d->slotPropertyChangedEnabled = false;
    InlineTextEditingCommand *command = new InlineTextEditingCommand(
        *this, selectedWidget(), d->editedWidgetClass, text);
    addCommand(command);
    d->slotPropertyChangedEnabled = true;
}

bool Form::eventFilter(QObject *obj, QEvent *ev)
{
    if (   (ev->type() == QEvent::Resize || ev->type() == QEvent::Move)
        && obj == selectedWidget() && d->inlineEditor)
    {
        // resize widget using resize handles
        WidgetInfo *winfo = library()->widgetInfoForClassName(obj->metaObject()->className());
        if (winfo) {
            winfo->factory()->resizeEditor(
                d->inlineEditor, selectedWidget(), 
                selectedWidget()->metaObject()->className());
        }
    }
    else if (   ev->type() == QEvent::Paint && obj == selectedWidget()
             && d->inlineEditor && d->inlineEditorContainer)
    {
        // paint event for container edited (eg button group)
        return d->inlineEditorContainer->eventFilter(obj, ev);
    }
    else if (   ev->type() == QEvent::MouseButtonPress && obj == selectedWidget()
             && d->inlineEditor && d->inlineEditorContainer)
    {
        // click outside editor --> cancel editing
        resetInlineEditor();
        return d->inlineEditorContainer->eventFilter(obj, ev);
    }

    if (ev->type() == QEvent::FocusOut && d->inlineEditor) {
        QWidget *w = d->inlineEditor;
        if (obj != w)
            return false;

        QWidget *focus = w->topLevelWidget()->focusWidget();
        if (   focus
            && w != focus
            && !KexiUtils::findFirstChild<QWidget*>(w, focus->objectName().toLatin1(),
                                                    focus->metaObject()->className())
           )
        {
            resetInlineEditor();
        }
    }
    else if (ev->type() == QEvent::KeyPress) {
        QWidget *w = d->inlineEditor;
        if (obj != w)
            return false;

        QKeyEvent *e = static_cast<QKeyEvent*>(ev);
        if (   (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
            && e->modifiers() != Qt::AltModifier)
        {
            resetInlineEditor();
        }
        if (e->key() == Qt::Key_Escape) {
            setInlineEditorText(d->originalInlineText);
            resetInlineEditor();
        }
    }
    else if (ev->type() == QEvent::ContextMenu) {
        QWidget *w = d->inlineEditor;
        if (obj != w)
            return false;

        return true;
    }
    return false;
}

void Form::slotInlineTextChanged()
{
    changeInlineTextInternal(inlineEditorText());
}

QString Form::inlineEditorText() const
{
    QWidget *ed = d->inlineEditor;
    if (!ed)
        return QString();
    return dynamic_cast<KTextEdit*>(ed)
           ? dynamic_cast<KTextEdit*>(ed)->toPlainText() : dynamic_cast<QLineEdit*>(ed)->text();
}

void Form::setInlineEditorText(const QString& text)
{
    QWidget *ed = d->inlineEditor;
    if (!ed)
        return;

    if (dynamic_cast<KTextEdit*>(ed))
        dynamic_cast<KTextEdit*>(ed)->setPlainText(text);
    else if (dynamic_cast<QLineEdit*>(ed))
        dynamic_cast<QLineEdit*>(ed)->setText(text);
    else
        qWarning() << "Inline editor is neither KTextEdit nor QLineEdit";
}

void Form::disableFilter(QWidget *w, Container *container)
{
    Q_UNUSED(container);
    ObjectTreeItem *tree = objectTree()->lookup(w->objectName());
    if (!tree)
        return;
    tree->eventEater()->setContainer(this);

    w->setFocus();
    ResizeHandleSet *handles = resizeHandlesForWidget(w);
    if (handles) {
        handles->setEditingMode(true);
        handles->raise();
    }
    d->inlineEditor = 0;
    d->inlineEditorContainer = 0;
    d->editedWidgetClass.clear();

    if (!tree->isEnabled()) {
        //! @todo widget is disabled, so we re-enable it while editing
    }
    connect(w, SIGNAL(destroyed()), this, SLOT(widgetDestroyed()));
}

void Form::resetInlineEditor()
{
    if (!d->inlineEditorContainer) {
        return;
    }
    d->inlineEditorContainer->stopInlineEditing();

    QWidget *ed = d->inlineEditor;
    QWidget *widget = selectedWidget();
    if (widget) {
        FormWidgetInterface* fwiface = dynamic_cast<FormWidgetInterface*>(widget);
        if (fwiface)
            fwiface->setEditingMode(false);

        ObjectTreeItem *tree = objectTree()->lookup(widget->objectName());
        if (!tree) {
            qWarning() << "Cannot find tree item for widget" << widget->objectName();
            return;
        }
        tree->eventEater()->setContainer(d->inlineEditorContainer);

        // "disable" the widget if needed
        if (!ed && !tree->isEnabled()) {
            widget->setPalette(KexiUtils::paletteForReadOnly(widget->palette()));
        }
    }
    if (ed) {
        d->slotPropertyChangedEnabled = false;
        InlineTextEditingCommand command(
            *this, selectedWidget(), d->editedWidgetClass, inlineEditorText());
        command.execute();
        d->slotPropertyChangedEnabled = true;
    }
    d->inlineEditor = 0;
    d->inlineEditorContainer = 0;
    if (ed) {
        disconnect(ed, 0, this, 0);
        ed->deleteLater();
    }

    if (widget) {
        disconnect(widget, 0, this, 0);
        widget->update();
    }

    ResizeHandleSet *handles = resizeHandlesForWidget(widget);
    if (handles) {
        handles->setEditingMode(false);
    }
    d->editedWidgetClass.clear();
}

void Form::widgetDestroyed()
{
    if (d->inlineEditor) {
        d->inlineEditor->deleteLater();
        d->inlineEditor = 0;
    }

    ResizeHandleSet *handles = resizeHandlesForWidget(static_cast<QWidget*>(sender()));
    if (handles) {
        handles->setEditingMode(false);
    }
    d->inlineEditorContainer = 0;
    d->editedWidgetClass.clear();
}

void Form::inlineEditorDeleted()
{
    ResizeHandleSet *handles = resizeHandlesForWidget(static_cast<QWidget*>(sender()));
    if (handles) {
        handles->setEditingMode(false);
    }
    d->inlineEditor = 0;
    d->inlineEditorContainer = 0;
    d->editedWidgetClass.clear();
}

QByteArray Form::editedWidgetClass() const
{
    return d->editedWidgetClass;
}

