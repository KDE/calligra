/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2009 Jarosław Staniek <staniek@kde.org>

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

#include <QApplication>
#include <QClipboard>
#include <QLabel>
#include <QLayout>
#include <QTimer>

#include <kdebug.h>
#include <KLocale>
#include <k3command.h>
#include <KAction>
#include <KMessageBox>
#include <KActionCollection>
#include <KMenu>
#include <KFontDialog>

#include "FormWidget.h"
#include "resizehandle.h"
#include "container.h"
#include "objecttree.h"
//unused #include "widgetpropertyset.h"
#include "formIO.h"
//unused #include "formmanager.h"
#include "widgetlibrary.h"
#include "spring.h"
#include "events.h"
#include "utils.h"
#include "form.h"
#include "commands.h"
#include "widgetwithsubpropertiesinterface.h"
#include "tabstopdialog.h"
#include <kexiutils/utils.h>
#include <kexiutils/identifier.h>
#include <kexi_global.h>

#include <koproperty/Set.h>
#include <koproperty/Property.h>

#ifdef __GNUC__
#warning pixmapcollection
#endif
#define KEXI_NO_PIXMAPCOLLECTION
#ifndef KEXI_NO_PIXMAPCOLLECTION
#include "pixmapcollection.h"
#endif

namespace KFormDesigner
{

//! @internal
class FormPrivate
{
public:
    FormPrivate(Form *form);
    ~FormPrivate();

    void enableAction(const char* name, bool enable);

    void initPropertiesDescription();

    inline QString propertyCaption(const QByteArray &name)
    {
        return propCaption.value(name);
    }

    inline QString valueCaption(const QByteArray &name)
    {
        return propValCaption.value(name);
    }

    void addPropertyCaption(const QByteArray &property, const QString &caption)
    {
        if (!propCaption.contains(property))
            propCaption.insert(property, caption);
    }

    void addValueCaption(const QByteArray &value, const QString &caption)
    {
        if (!propValCaption.contains(value))
            propValCaption.insert(value, caption);
    }

    KoProperty::Property::ListData* createValueList(WidgetInfo *winfo, const QStringList &list);

    Form::Mode mode;
    Form::State state;
    Form::Features features;

    QPoint insertionPoint;

//  FormManager  *manager;
    QPointer<Container>  toplevel;
    ObjectTree  *topTree;
    QPointer<QWidget> widget;

    KoProperty::Set propertySet;

    QWidgetList selected;
    ResizeHandleSet::Hash resizeHandles;
    QByteArray selectedClass;

    bool modified : 1;
    bool interactive : 1;
    bool isUndoing : 1;
    bool isRedoing : 1;
    bool isSnapWidgetsToGridEnabled : 1;
//removed, mode is used now:    bool  design;
    QString  filename;

    K3CommandHistory  *commandHistory;
    KActionCollection  *collection;
    KFormDesigner::ActionGroup* widgetActionGroup;

    ObjectTreeList  tabstops;
    bool autoTabstops;
#ifdef KFD_SIGSLOTS
    ConnectionBuffer  *connBuffer;
#endif

    PixmapCollection  *pixcollection;

    //! This map is used to store cursor shapes before inserting (so we can restore them later)
    QHash<QObject*, QCursor> cursors;

    //!This string list is used to store the widgets which hasMouseTracking() == true (eg lineedits)
    QStringList *mouseTrackers;

    FormWidget  *formWidget;

    //! A set of head properties to be stored in a .ui file.
    //! This includes KFD format version.
    QHash<QByteArray, QString> headerProperties;

    //! Format version, set by FormIO or on creating a new form.
    uint formatVersion;
    //! Format version, set by FormIO's loader or on creating a new form.
    uint originalFormatVersion;

// moved from FormManager
#ifdef KFD_SIGSLOTS
    //! true is slot connection is curently being painted
// removed, use state instead    bool creatingConnection;
    Connection *connection;
// not needed to store this one:    KMenu *m_sigSlotMenu;
#endif

// moved from WidgetPropertySet
    // used to update command's value when undoing
    PropertyCommand  *lastCommand;
    GeometryPropertyCommand  *lastGeoCommand;
    bool slotPropertyChangedEnabled : 1;
    bool slotPropertyChanged_addCommandEnabled : 1;
// end of moved from WidgetPropertySet

    // i18n stuff
    QMap<QByteArray, QString> propCaption;
    QMap<QByteArray, QString> propValCaption;

    // helper to change color palette when switching 'enabled' property
    QColorGroup* origActiveColors;

    Form *q;
};
}

using namespace KFormDesigner;

FormPrivate::FormPrivate(Form *form)
 : state(Form::WidgetSelecting)
 , q(form)
{
    toplevel = 0;
    topTree = 0;
    widget = 0;
//Qt4    resizeHandles.setAutoDelete(true);
    modified = false;
    interactive = true;
//    design = true;
    autoTabstops = false;
    isRedoing = false;
//! @todo get the default from globals...
    isSnapWidgetsToGridEnabled = true;
//    tabstops.setAutoDelete(false);
#ifdef KFD_SIGSLOTS
    connBuffer = new ConnectionBuffer();
#endif
    formatVersion = KFormDesigner::version();
    originalFormatVersion = KFormDesigner::version();
//    creatingConnection = false;

// moved from WidgetPropertySet
    lastCommand = 0;
    lastGeoCommand = 0;
    isUndoing = false;
    slotPropertyChangedEnabled = true;
    slotPropertyChanged_addCommandEnabled = true;
    initPropertiesDescription();
    origActiveColors = 0;
// end of moved from WidgetPropertySet
}

FormPrivate::~FormPrivate()
{
    delete commandHistory;
    delete topTree;
#ifdef KFD_SIGSLOTS
    delete connBuffer;
    connBuffer = 0;
#endif
//Qt4    resizeHandles.setAutoDelete(false);
    // otherwise, it tries to delete widgets which doesn't exist anymore
}

void FormPrivate::enableAction(const char *name, bool enable)
{
    QAction *a = collection->action(QLatin1String(name));
    //Q_ASSERT(a);
    if (a) {
        a->setEnabled(enable);
    }
}

////////////////////////////////////////// i18n related functions ////////

// moved from WidgetPropertySet
void FormPrivate::initPropertiesDescription()
{
//! \todo perhaps a few of them shouldn't be translated within KFD mode,
//!       to be more Qt Designer friendly?
    propCaption["name"] = i18n("Name");
    propCaption["caption"] = i18n("Caption");
    propCaption["text"] = i18n("Text");
    propCaption["paletteBackgroundPixmap"] = i18n("Background Pixmap");
    propCaption["enabled"] = i18n("Enabled");
    propCaption["geometry"] = i18n("Geometry");
    propCaption["sizePolicy"] = i18n("Size Policy");
    propCaption["minimumSize"] = i18n("Minimum Size");
    propCaption["maximumSize"] = i18n("Maximum Size");
    propCaption["font"] = i18n("Font");
    propCaption["cursor"] = i18n("Cursor");
    propCaption["paletteForegroundColor"] = i18n("Foreground Color");
    propCaption["paletteBackgroundColor"] = i18n("Background Color");
    propCaption["focusPolicy"] = i18n("Focus Policy");
    propCaption["margin"] = i18n("Margin");
    propCaption["readOnly"] = i18n("Read Only");
    //any QFrame
    propCaption["frame"] = i18n("Frame");
    propCaption["lineWidth"] = i18n("Frame Width");
    propCaption["midLineWidth"] = i18n("Mid Frame Width");
    propCaption["frameShape"] = i18n("Frame Shape");
    propCaption["frameShadow"] = i18n("Frame Shadow");
    //any QScrollbar
    propCaption["vScrollBarMode"] = i18n("Vertical ScrollBar");
    propCaption["hScrollBarMode"] = i18n("Horizontal ScrollBar");

    propValCaption["NoBackground"] = i18n("No Background");
    propValCaption["PaletteForeground"] = i18n("Palette Foreground");
    propValCaption["AutoText"] = i18nc("Auto (HINT: for AutoText)", "Auto");

    propValCaption["AlignAuto"] = i18nc("Auto (HINT: for Align)", "Auto");
    propValCaption["AlignLeft"] = i18nc("Left (HINT: for Align)", "Left");
    propValCaption["AlignRight"] = i18nc("Right (HINT: for Align)", "Right");
    propValCaption["AlignHCenter"] = i18nc("Center (HINT: for Align)", "Center");
    propValCaption["AlignJustify"] = i18nc("Justify (HINT: for Align)", "Justify");
    propValCaption["AlignVCenter"] = i18nc("Center (HINT: for Align)", "Center");
    propValCaption["AlignTop"] = i18nc("Top (HINT: for Align)", "Top");
    propValCaption["AlignBottom"] = i18nc("Bottom (HINT: for Align)", "Bottom");

    propValCaption["NoFrame"] = i18nc("No Frame (HINT: for Frame Shape)", "No Frame");
    propValCaption["Box"] = i18nc("Box (HINT: for Frame Shape)", "Box");
    propValCaption["Panel"] = i18nc("Panel (HINT: for Frame Shape)", "Panel");
    propValCaption["WinPanel"] = i18nc("Windows Panel (HINT: for Frame Shape)", "Windows Panel");
    propValCaption["HLine"] = i18nc("Horiz. Line (HINT: for Frame Shape)", "Horiz. Line");
    propValCaption["VLine"] = i18nc("Vertical Line (HINT: for Frame Shape)", "Vertical Line");
    propValCaption["StyledPanel"] = i18nc("Styled (HINT: for Frame Shape)", "Styled");
    propValCaption["PopupPanel"] = i18nc("Popup (HINT: for Frame Shape)", "Popup");
    propValCaption["MenuBarPanel"] = i18nc("Menu Bar (HINT: for Frame Shape)", "Menu Bar");
    propValCaption["ToolBarPanel"] = i18nc("Toolbar (HINT: for Frame Shape)", "Toolbar");
    propValCaption["LineEditPanel"] = i18nc("Text Box (HINT: for Frame Shape)", "Text Box");
    propValCaption["TabWidgetPanel"] = i18nc("Tab Widget (HINT: for Frame Shape)", "Tab Widget");
    propValCaption["GroupBoxPanel"] = i18nc("Group Box (HINT: for Frame Shape)", "Group Box");

    propValCaption["Plain"] = i18nc("Plain (HINT: for Frame Shadow)", "Plain");
    propValCaption["Raised"] = i18nc("Raised (HINT: for Frame Shadow)", "Raised");
    propValCaption["Sunken"] = i18nc("Sunken (HINT: for Frame Shadow)", "Sunken");
    propValCaption["MShadow"] = i18nc("for Frame Shadow", "Internal");

    propValCaption["NoFocus"] = i18nc("No Focus (HINT: for Focus)", "No Focus");
    propValCaption["TabFocus"] = i18nc("Tab (HINT: for Focus)", "Tab");
    propValCaption["ClickFocus"] = i18nc("Click (HINT: for Focus)", "Click");
    propValCaption["StrongFocus"] = i18nc("Tab/Click (HINT: for Focus)", "Tab/Click");
    propValCaption["WheelFocus"] = i18nc("Tab/Click/MouseWheel (HINT: for Focus)", "Tab/Click/MouseWheel");

    propValCaption["Auto"] = i18n("Auto");
    propValCaption["AlwaysOff"] = i18n("Always Off");
    propValCaption["AlwaysOn"] = i18n("Always On");

    //orientation
    propValCaption["Horizontal"] = i18n("Horizontal");
    propValCaption["Vertical"] = i18n("Vertical");
}

KoProperty::Property::ListData* FormPrivate::createValueList(WidgetInfo *winfo, const QStringList &list)
{
    QStringList names;
    foreach (const QString& name, list) {
        QString n(propValCaption.value(name.toLatin1()));
        if (n.isEmpty()) { //try within factory and (maybe) parent factory
            if (winfo) {
                n = q->library()->propertyDescForValue(winfo, name.toLatin1());
            }
            if (n.isEmpty()) {
                names.append(name);   //untranslated
            }
            else {
                names.append(n);
            }
        } else {
            names.append(n);
        }
    }
    return new KoProperty::Property::ListData(list, names);
}

//--------------------------------------

Form::Form(WidgetLibrary* library, Mode mode, KActionCollection &col, ActionGroup& group)
        : QObject(library)
{
    init(library, mode, col, group);
}

Form::Form(Form *parent)
        : QObject(parent->library())
{
    init(parent->library(), parent->mode(), *parent->actionCollection(), *parent->widgetActionGroup());
}

Form::~Form()
{
    emit destroying();
    delete d;
    d = 0;
}

void Form::init(WidgetLibrary* library, Mode mode, KActionCollection &col, KFormDesigner::ActionGroup &group)
{
    m_lib = library;
    d = new FormPrivate(this);
// d->manager = manager;
    d->mode = mode;
    d->features = 0;
    d->widgetActionGroup = &group;

    connect(&d->propertySet, SIGNAL(propertyChanged(KoProperty::Set&, KoProperty::Property&)),
            this, SLOT(slotPropertyChanged(KoProperty::Set&, KoProperty::Property&)));
    connect(&d->propertySet, SIGNAL(propertyReset(KoProperty::Set&, KoProperty::Property&)),
            this, SLOT(slotPropertyReset(KoProperty::Set&, KoProperty::Property&)));

    // Init actions
    d->collection = &col; //new KActionCollection(this);
    d->commandHistory = new K3CommandHistory(d->collection, true);
    connect(d->commandHistory, SIGNAL(commandExecuted(K3Command*)), this, SLOT(slotCommandExecuted(K3Command*)));
    connect(d->commandHistory, SIGNAL(documentRestored()), this, SLOT(slotFormRestored()));
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

int Form::gridSize() const
{
    return 10;
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

K3CommandHistory* Form::commandHistory() const
{
    return d->commandHistory;
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

PixmapCollection* Form::pixmapCollection() const
{
    return d->pixcollection;
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
Form::parentContainer(QWidget *w) const
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

void Form::setMode(Mode mode)
{
    d->mode = mode;
    if (mode == DesignMode)
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

void Form::selectWidget(QWidget *w, WidgetSelectionFlags flags)
{
    if (!w) {
        selectWidget(widget());
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
    emitActionSignals(false);

    // WidgetStack and TabWidget pages widgets shouldn't have resize handles, but their parent
//! @todo move special case to a factory?
#ifdef __GNUC__
#warning "Port this.."
#else
#pragma WARNING( Port this.. )
#endif
#if 0
    if (!isTopLevelWidget(w) && w->parentWidget()
            && KexiUtils::objectIsA(w->parentWidget(), "QWidgetStack")) {
        w = w->parentWidget();
        if (w->parentWidget() && w->parentWidget()->inherits("QTabWidget"))
            w = w->parentWidget();
    }
#endif

    if (w && w != widget())
        d->resizeHandles.insert(w->objectName(), new ResizeHandleSet(w, this));
}

bool Form::isTopLevelWidget(QWidget *w) const
{
    /* should not be used, just check w==formWidget() instead? */
    ObjectTreeItem *item = objectTree()->lookup(w->objectName());
    if (!item)
        return true;

    return !item->parent();
//! @todo OK?
//    return w == formWidget();
}

ResizeHandleSet*
Form::resizeHandlesForWidget(QWidget* w)
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
    emitActionSignals(false);
}

void Form::setInsertionPoint(const QPoint &p)
{
    d->insertionPoint = p;
}

/*KAction* Form::action(const QString& name)
{
    return d->collection(name);
}*/

void Form::emitActionSignals(bool withUndoAction)
{
    // Update menu and toolbar items
    if (d->selected.count() > 1)
        emitWidgetSelected(true);
    else if (d->selected.first() != widget())
        emitWidgetSelected(false);
    else
        emitFormWidgetSelected();

    if (!withUndoAction)
        return;

#ifdef __GNUC__
#warning pixmapcollection
#endif
#ifndef KEXI_NO_PIXMAPCOLLECTION
    KAction *undoAction = d->collection->action(QLatin1String("edit_undo"));
    if (undoAction)
        emitUndoEnabled(undoAction->isEnabled(), undoAction->text());

    KAction *redoAction = d->collection->action(QLatin1String("edit_redo"));
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
// for(QWidget *w = selectedWidgets()->next(); w; w = selectedWidgets()->next())
//  emit selectionChanged(selectedWidgets()->first(), true);
    foreach (QWidget *w, *selectedWidgets()) {
        emitSelectionChanged(w, LastSelection);
//prev        emit selectionChanged(w, true);
    }
}

/* moved to FormPrivate
void Form::enableAction(const char* name, bool enable)
{
#ifdef __GNUC__
#warning "Implement this.."
#else
#pragma WARNING( Implement this.. )
#endif
}*/

// moved from FormManager
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
        if (-1 != KexiUtils::indexOfPropertyWithSuperclasses(w, "font")) {
            fontEnabled = true;
            break;
        }
    }
    d->enableAction("format_font", fontEnabled);

    // If the widgets selected is a container, we enable layout actions
    bool containerSelected = false;
    if (!multiple) {
        KFormDesigner::ObjectTreeItem *item = 0;
        if (!wlist->isEmpty()) {
            objectTree()->lookup(wlist->first()->objectName());
        }
        if (item && item->container()) {
            containerSelected = true;
        }
    }
    const bool twoSelected = wlist->count() == 2;
    // Layout actions
    d->enableAction("layout_menu", multiple || containerSelected);
    d->enableAction("layout_hbox", multiple || containerSelected);
    d->enableAction("layout_vbox", multiple || containerSelected);
    d->enableAction("layout_grid", multiple || containerSelected);
    d->enableAction("layout_hsplitter", twoSelected);
    d->enableAction("layout_vsplitter", twoSelected);

    Container *container = activeContainer();
    if (container) {
        d->enableAction("break_layout", 
            (container->layoutType() != NoLayout));
    }
    emit widgetSelected(true);
}

// moved from FormManager
void Form::emitFormWidgetSelected()
{
// disableWidgetActions();
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

    const bool twoSelected = selectedWidgets()->count() == 2;
    const bool hasChildren = !objectTree()->children()->isEmpty();

    // Layout actions
    d->enableAction("layout_menu", hasChildren);
    d->enableAction("layout_hbox", hasChildren);
    d->enableAction("layout_vbox", hasChildren);
    d->enableAction("layout_grid", hasChildren);
    d->enableAction("layout_hsplitter", twoSelected);
    d->enableAction("layout_vsplitter", twoSelected);
    d->enableAction("break_layout", (toplevelContainer()->layoutType() != NoLayout));

    emit formWidgetSelected();
}

// moved from FormManager
void Form::emitNoFormSelected()
{
    disableWidgetActions();

    // Disable edit actions
// enableAction("edit_paste", false);
// enableAction("edit_undo", false);
// enableAction("edit_redo", false);

    // Disable 'Tools' actions
    d->enableAction("pixmap_collection", false);
#ifdef KFD_SIGSLOTS
    if (d->features & EnableConnections) {
        d->enableAction("form_connections", false);
    }
#endif
    d->enableAction("taborder", false);
    d->enableAction("change_style", true); //activeForm() != 0);

    // Disable items in 'File'
    if (d->features & EnableFileActions) {
        d->enableAction("file_save", false);
        d->enableAction("file_save_as", false);
        d->enableAction("preview_form", false);
    }

    emit noFormSelected();
}

// moved from FormManager
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

// moved from FormManager
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

    d->enableAction("layout_menu", false);
    d->enableAction("layout_hbox", false);
    d->enableAction("layout_vbox", false);
    d->enableAction("layout_grid", false);
    d->enableAction("layout_hsplitter", false);
    d->enableAction("layout_vsplitter", false);
    d->enableAction("break_layout", false);
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

//2.0 removed...    FormManager::self()->deleteForm(this);
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
        d->propertySet.changeProperty("name", oldname);
    }
    else {
#ifdef KFD_SIGSLOTS
        d->connBuffer->fixName(oldname, newname);
#endif
        ResizeHandleSet *temp = d->resizeHandles.take(oldname);
        d->resizeHandles.insert(newname, temp);
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

void Form::addCommand(K3Command *command, bool execute)
{
    d->modified = true;
    emit modified();
    d->commandHistory->addCommand(command, execute);
    if (!execute) // simulate command to activate 'undo' menu
        slotCommandExecuted(command);
}

void Form::clearCommandHistory()
{
    d->commandHistory->clear();
//2.0 todo...    FormManager::self()->emitUndoEnabled(false, QString());
//2.0 todo...    FormManager::self()->emitRedoEnabled(false, QString());
}

void Form::slotCommandExecuted(K3Command *command)
{
    Q_UNUSED(command)
    d->modified = true;
    emit modified();
    // because actions text is changed after the commandExecuted() signal is emitted
    QTimer::singleShot(10, this, SLOT(emitUndoEnabled()));
    QTimer::singleShot(10, this, SLOT(emitRedoEnabled()));
}

void Form::emitUndoEnabled()
{
#ifdef __GNUC__
#warning pixmapcollection
#endif
#ifndef KEXI_NO_PIXMAPCOLLECTION
    KAction *undoAction = d->collection->action(QLatin1String("edit_undo"));
    if (undoAction)
        emitUndoEnabled(undoAction->isEnabled(), undoAction->text());
#endif
}

void Form::emitRedoEnabled()
{
#ifdef __GNUC__
#warning pixmapcollection
#endif
#ifndef KEXI_NO_PIXMAPCOLLECTION
    KAction *redoAction = d->collection->action(QLatin1String("edit_redo"));
    if (redoAction)
        emitRedoEnabled(redoAction->isEnabled(), redoAction->text());
#endif
}

void Form::slotFormRestored()
{
    d->modified = false;
    emit modified();
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
//   if(obj->isWidgetType() && (((QWidget*)obj)->focusPolicy() != QWidget::NoFocus)) {
//   if(obj->isWidgetType() && (((QWidget*)obj)->focusPolicy() & QWidget::TabFocus)) {
            if (obj->isWidgetType()) {//QWidget::TabFocus flag will be checked later!
                if (!d->tabstops.contains(it)) {
                    d->tabstops.append(it);
                    return;
                }
            }
        }
    }
    else if (!d->tabstops.contains(it)) { // not yet in the list
        d->tabstops.append(it);
    }
}

void
Form::updateTabStopsOrder()
{
    ObjectTreeList newList(d->tabstops);
    foreach (ObjectTreeItem *item, d->tabstops) {
        if (!(item->widget()->focusPolicy() & Qt::TabFocus)) {
            kDebug() << "Widget removed because has no TabFocus:" 
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

void Form::autoAssignTabStops()
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

// moved from FormManager
void Form::enterWidgetInsertingState(const QByteArray &classname)
{
    enterWidgetSelectingState();
    d->state = WidgetInserting;

/* prev 
    foreach (Form *form, m_forms) {
        if (toplevelContainer()) {
            widget()->setCursor(QCursor(Qt::CrossCursor));
        }
        const QList<QWidget*> list(widget()->findChildren<QWidget*>());
        foreach (QWidget *w, list) {
            if (w->testAttribute(Qt::WA_SetCursor)) {
                d->cursors.insert(w, w->cursor());
                w->setCursor(QCursor(Qt::CrossCursor));
            }
        }
    }*/
    if (toplevelContainer()) {
        widget()->setCursor(QCursor(Qt::CrossCursor));
    }
    const QList<QWidget*> list(widget()->findChildren<QWidget*>());
    foreach (QWidget *w, list) {
        if (w->testAttribute(Qt::WA_SetCursor)) {
            d->cursors.insert(w, w->cursor());
            w->setCursor(QCursor(Qt::CrossCursor));
        }
    }

    d->selectedClass = classname;
    QAction *pointer_action = d->collection->action(QLatin1String("edit_pointer"));
    //Q_ASSERT(pointer_action);
    if (pointer_action) {
        pointer_action->setChecked(false);
    }
}

QByteArray Form::selectedClass() const
{
    return d->selectedClass;
}

// moved from FormManager
void Form::abortWidgetInserting()
{
    if (d->state != WidgetInserting)
        return;

    widget()->unsetCursor();
#ifdef __GNUC__
#warning "Port this.."
#else
#pragma WARNING( Port this.. )
#endif
#if 0
    foreach (Form *form, m_forms) {
        widget()->unsetCursor();
        const QList<QWidget*> list(widget()->findChildren<QWidget*>());
        foreach (QWidget *w, list) {
            w->unsetCursor();
        }
    }
#endif
    d->state = WidgetSelecting;
    QAction *pointer_action = d->widgetActionGroup->action(QLatin1String("edit_pointer"));
//    Q_ASSERT(pointer_action);
    if (pointer_action) {
        pointer_action->setChecked(true);
    }
}

// moved from FormManager
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
// moved from FormManager
void Form::enterConnectingState()
{
    if (!(d->features & EnableConnections))
        return;
    enterWidgetSelectingState();

#ifdef __GNUC__
#warning "Port this.."
#else
#pragma WARNING( Port this.. )
#endif
    // We set a Pointing hand cursor while drawing the connection
    foreach (Form *form, m_forms) {
        d->mouseTrackers = new QStringList();
        if (toplevelContainer()) {
            widget()->setCursor(QCursor(Qt::PointingHandCursor));
            widget()->setMouseTracking(true);
        }
        const QList<QWidget*> list(widget()->findChildren<QWidget*>());
        foreach(QWidget *w, list) {
            if (w->testAttribute(Qt::WA_SetCursor)) {
                d->cursors.insert(w, w->cursor());
                w->setCursor(QCursor(Qt::PointingHandCursor));
            }
            if (w->hasMouseTracking())
                d->mouseTrackers->append(w->objectName());
            w->setMouseTracking(true);
        }
    }
    delete m_connection;
    m_connection = new Connection();
    m_drawingSlot = true;
    if (m_dragConnection)
        m_dragConnection->setChecked(true);
}

// moved from FormManager
void Form::resetSelectedConnection()
{
//todo
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

// moved from FormManager
void Form::abortCreatingConnection()
{
//todo
    if (!(d->features & EnableConnections))
        return;
    if (d->state != Connecting)
        return;

    if (formWidget()) {
        formWidget()->clearForm();
    }

#ifdef __GNUC__
#warning "Port this.."
#else
#pragma WARNING( Port this.. )
#endif
    foreach (Form *form, m_forms) {
        widget()->unsetCursor();
        widget()->setMouseTracking(false);
        const QList<QWidget*> list(widget()->findChildren<QWidget*>());
        foreach (QWidget *w, list) {
            if (w->testAttribute(Qt::WA_SetCursor)) {
                QHash<QObject*, QCursor>::ConstIterator curIt(d->cursors.find(w));
                if (curIt != d->cursors.constEnd())
                    w->setCursor(*curIt);
            }
            w->setMouseTracking(d->mouseTrackers->contains(w->objectName()));
        }
        delete d->mouseTrackers;
        d->mouseTrackers = 0;
    }

    if (m_connection->slot().isNull())
        emit connectionAborted(this);
    delete m_connection;
    m_connection = 0;
    m_drawingSlot = false;
    QAction *pointer_action = d->widgetActionGroup->action(QLatin1String("edit_pointer"));
//    Q_ASSERT(pointer_action);
    if (pointer_action) {
        pointer_action->setChecked(true);
    }
}
#endif

Form::State Form::state() const
{
    return d->state;
}

// moved from FormManager
void Form::slotPropertyChanged(KoProperty::Set& set, KoProperty::Property& p)
{
    Q_UNUSED(set);

    if (!d->slotPropertyChangedEnabled || !objectTree())
        return;

    const QByteArray property( p.name() );
    if (property.startsWith("this:"))
        return; //starts with magical prefix: it's a "meta" prop.

    const QVariant value( p.value() );

    // check if the name is valid (ie is correct identifier) and there is no name conflict
    if (property == "name") {
        if (d->selected.count() != 1)
            return;
        if (!isNameValid(value.toString()))
            return;
    }
    else if (property == "paletteBackgroundPixmap") {
        // a widget with a background pixmap should have its own origin
//2.0 obsolete        d->propertySet["backgroundOrigin"] = "WidgetOrigin";
        //else if(property == "signals")
        // return;
        // special types of properties handled separately
    }
    else if (property == "hAlign" || property == "vAlign" || property == "wordbreak") {
        saveAlignProperty(property);
        return;
    }
    else if (property == "layout" || property == "layoutMargin" || property == "layoutSpacing") {
        saveLayoutProperty(property, value);
        return;
    }
    else if (property == "enabled")  {
        // we cannot really disable the widget, we just change its color palette
        saveEnabledProperty(value.toBool());
        return;
    }

    // make sure we are not already undoing -> avoid recursion
    if (d->isUndoing && !d->isRedoing)
        return;

    const bool alterLastCommand = d->lastCommand && d->lastCommand->property() == property;

    if (d->selected.count() == 1) { // one widget selected
        // If the last command is the same, we just change its value
        if (alterLastCommand && !d->isRedoing) {
            d->lastCommand->setValue(value);
            emit modified();
        }
        else  {
            if (d->slotPropertyChanged_addCommandEnabled && !d->isRedoing) {
                d->lastCommand = new PropertyCommand(*this, d->selected.first()->objectName().toLatin1(),
                                                     d->selected.first()->property(property), value, property);
                addCommand(d->lastCommand, false);
            }

            // If the property is changed, we add it in ObjectTreeItem modifProp
            ObjectTreeItem *tree = objectTree()->lookup(d->selected.first()->objectName());
            if (tree && p.isModified()) {
                tree->addModifiedProperty(property, d->selected.first()->property(property));
            }
        }

        if (property == "name")
            emit widgetNameChanged(d->selected.first()->objectName().toLatin1(), p.value().toByteArray());
        d->selected.first()->setProperty(property, value);
        handleWidgetPropertyChanged(d->selected.first(), property, value);
    }
    else {
        if (alterLastCommand && !d->isRedoing)
            d->lastCommand->setValue(value);
        else {
            if (d->slotPropertyChanged_addCommandEnabled && !d->isRedoing) {
                // We store old values for each widget
                QHash<QByteArray, QVariant> list;
                foreach(QWidget* widget, d->selected) {
                    list.insert(widget->objectName().toLatin1(), widget->property(property));
                }

                d->lastCommand = new PropertyCommand(*this, list, value, property);
                addCommand(d->lastCommand, false);
            }
        }

        foreach(QWidget* widget, d->selected) {
            if (!alterLastCommand) {
                ObjectTreeItem *titem = objectTree()->lookup(widget->objectName());
                if (titem && p.isModified())
                    titem->addModifiedProperty(property, widget->property(property));
            }
            widget->setProperty(property, value);
            handleWidgetPropertyChanged(widget, property, value);
        }
    }
}

// moved from FormManager
void Form::slotPropertyReset(KoProperty::Set& set, KoProperty::Property& property)
{
    Q_UNUSED(set);

    if (d->selected.count() < 2)
        return;

    // We use the old value in modifProp for each widget
    foreach(QWidget* widget, d->selected) {
        ObjectTreeItem *titem = objectTree()->lookup(
              widget->objectName());
        if (titem->modifiedProperties()->contains(property.name()))
            widget->setProperty(
                property.name(), titem->modifiedProperties()->find(property.name()).value());
    }
}

// moved from FormManager
bool Form::isNameValid(const QString &name)
{
    if (d->selected.isEmpty())
        return false;
//! @todo add to the undo buffer
    QWidget *w = d->selected.first();
    //also update widget's name in QObject member
    if (!KexiUtils::isIdentifier(name)) {
        KMessageBox::sorry(widget(),
                           i18n("Could not rename widget \"%1\" to \"%2\" because "
                                "\"%3\" is not a valid name (identifier) for a widget.\n",
                                w->objectName(), name, name));
        d->slotPropertyChangedEnabled = false;
        d->propertySet["name"].resetValue();
        d->slotPropertyChangedEnabled = true;
        return false;
    }

    if (objectTree()->lookup(name)) {
        KMessageBox::sorry(widget(),
                           i18n("Could not rename widget \"%1\" to \"%2\" "
                                "because a widget with the name \"%3\" already exists.\n",
                                w->objectName(), name, name));
        d->slotPropertyChangedEnabled = false;
        d->propertySet["name"].resetValue();
        d->slotPropertyChangedEnabled = true;
        return false;
    }

    return true;
}

// moved from FormManager
void Form::undo()
{
    if (!objectTree())
        return;

    d->commandHistory->undo();
}

// moved from FormManager
void Form::redo()
{
    if (!objectTree())
        return;

    d->isRedoing = true;
    d->commandHistory->redo();
    d->isRedoing = false;
}

// moved from WidgetPropertySet
void Form::setUndoing(bool undoing)
{
    d->isUndoing = undoing;
}

// moved from WidgetPropertySet
bool Form::isUndoing() const
{
    return d->isUndoing;
}

// moved from WidgetPropertySet
bool Form::isPropertyVisible(const QByteArray &property, bool isTopLevel,
                             const QByteArray &classname)
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

    return m_lib->isPropertyVisible(
               subwidget->metaObject()->className(), subwidget, property, multiple, isTopLevel);
}

// moved from WidgetPropertySet
void Form::addWidget(QWidget *w)
{
    d->selected.append(w);

    // Reset some stuff
    d->lastCommand = 0;
    d->lastGeoCommand = 0;
//    d->properties.clear();

    QByteArray classname;
    if (d->selected.first()->metaObject()->className() == w->metaObject()->className()) {
        classname = d->selected.first()->metaObject()->className();
    }

    // show only properties shared by widget (properties chosen by factory)
    bool isTopLevel = isTopLevelWidget(w);

    //WidgetWithSubpropertiesInterface* subpropIface = dynamic_cast<WidgetWithSubpropertiesInterface*>(w);
// QWidget *subwidget = isSubproperty ? subpropIface->subwidget() : w;

    for (KoProperty::Set::Iterator it(d->propertySet); it.current(); ++it) {
        kDebug() << it.current();
        if (!isPropertyVisible(it.current()->name(), isTopLevel, classname)) {
            d->propertySet[it.current()->name()].setVisible(false);
        }
    }

    if (d->selected.count() >= 2) {
        //second widget, update metainfo
        d->propertySet["this:className"].setValue("special:multiple");
        d->propertySet["this:classString"].setValue(
            i18n("Multiple Widgets") + QString(" (%1)").arg(d->selected.count()));
        d->propertySet["this:iconName"].setValue("multiple_obj");
        //name doesn't make sense for now
        d->propertySet["name"].setValue("");
    }
}

// moved from WidgetPropertySet
void Form::createPropertiesForWidget(QWidget *w)
{
    if (!objectTree()) {
        kWarning() << "no object tree!";
        return;
    }
    ObjectTreeItem *tree = objectTree()->lookup(w->objectName());
    if (!tree)
        return;

    const QHash<QString, QVariant>* modifiedProperties = tree->modifiedProperties();
    QHash<QString, QVariant>::ConstIterator modifiedPropertiesIt;
    bool isTopLevel = isTopLevelWidget(w);
// int count = 0;
    KoProperty::Property *newProp = 0;
    WidgetInfo *winfo = m_lib->widgetInfoForClassName(w->metaObject()->className());
    if (!winfo) {
        kWarning() << "no widget info for class" << w->metaObject()->className();
        return;
    }

//! @todo ineffective, get property names directly
    QList<QMetaProperty> propList(
        KexiUtils::propertiesForMetaObjectWithInherited(w->metaObject()));
    QList<QByteArray> propNames;
    foreach(QMetaProperty mp, propList) {
        propNames.append(mp.name());
    }

    // add subproperties if available
    WidgetWithSubpropertiesInterface* subpropIface
    = dynamic_cast<WidgetWithSubpropertiesInterface*>(w);
// QStrList tmpList; //used to allocate copy of names
    if (subpropIface) {
        const QSet<QByteArray> subproperies(subpropIface->subproperies());
        foreach(QByteArray propName, subproperies) {
//   tmpList.append( *it );
            propNames.append(propName);
            kDebug() << "Added subproperty: " << propName;
        }
    }

    // iterate over the property list, and create Property objects
    foreach(QByteArray propName, propNames) {
        //kDebug() << ">> " << it.current();
        const QMetaProperty subMeta = // special case - subproperty
            subpropIface ? subpropIface->findMetaSubproperty(propName) : QMetaProperty();
        const QMetaProperty meta = subMeta.isValid() ? subMeta
                                   : KexiUtils::findPropertyWithSuperclasses(w, propName.constData());
        if (meta.isValid()) {
            continue;
        }
        const char* propertyName = meta.name();
        QWidget *subwidget = subMeta.isValid()//subpropIface
                             ? subpropIface->subwidget() : w;
        WidgetInfo *subwinfo = m_lib->widgetInfoForClassName(
                                   subwidget->metaObject()->className());
//  kDebug() << "$$$ " << subwidget->className();

        if (subwinfo && meta.isDesignable(subwidget) && !d->propertySet.contains(propertyName)) {
            //! \todo add another list for property description
            QString desc(d->propCaption.value(meta.name()));
            //! \todo change i18n
            if (desc.isEmpty()) { //try to get property description from factory
                desc = m_lib->propertyDescForName(subwinfo, propertyName);
            }

            modifiedPropertiesIt = modifiedProperties->find(propertyName);
            const bool oldValueExists = modifiedPropertiesIt != modifiedProperties->constEnd();

            if (meta.isEnumType()) {
                if (qstrcmp(propertyName, "alignment") == 0)  {
                    createAlignProperty(meta, w, subwidget);
                    continue;
                }

                QStringList keys(KexiUtils::enumKeysForProperty(meta));
                newProp = new KoProperty::Property(
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
                newProp = new KoProperty::Property(
                    propertyName,
                    // assign current or older value
                    oldValueExists ? modifiedPropertiesIt.value() : subwidget->property(propertyName),
                    desc, desc, subwinfo->customTypeForProperty(propertyName)
                );
                //now set current value, so the old one is stored as old
                if (oldValueExists) {
                    newProp->setValue(subwidget->property(propertyName));
                }
            }

            d->propertySet.addProperty(newProp);
            if (!isPropertyVisible(propertyName, isTopLevel))
                newProp->setVisible(false);
            //! TMP
            if (newProp->type() == 0) // invalid type == null pixmap ?
                newProp->setType(KoProperty::Pixmap);
        }

//  if(0==qstrcmp(propertyName, "name"))
//   (*this)["name"].setAutoSync(0); // name should be updated only when pressing Enter

        // \todo js what does this mean? why do you use WidgetInfo and not WidgetLibrary
        //if (winfo) {
        //  tristate autoSync = winfo->autoSyncForProperty( propertyName );
        //  if (! ~autoSync)
        //    d->propertySet[propertyName].setAutoSync( autoSync );
        //}

        // update the Property.oldValue() and isModified() using the value stored in the ObjectTreeItem
        updatePropertyValue(tree, propertyName, meta);
    }

    d->propertySet["name"].setAutoSync(false); // name should be updated only when pressing Enter
    d->propertySet["enabled"].setValue(tree->isEnabled());

    if (winfo) {
        m_lib->setPropertyOptions(d->propertySet, *winfo, w);
        d->propertySet.addProperty(newProp = new KoProperty::Property("this:classString", winfo->name()));
        newProp->setVisible(false);
        d->propertySet.addProperty(newProp = new KoProperty::Property("this:iconName", winfo->pixmap()));
        newProp->setVisible(false);
    }
    d->propertySet.addProperty(newProp = new KoProperty::Property("this:className",
            w->metaObject()->className()));
    newProp->setVisible(false);

#if 0 // let's forget it for now, until we have new complete events editor
    if (m_manager->lib()->advancedPropertiesVisible()) {
      // add the signals property
      QStrList strlist = w->metaObject()->signalNames(true);
      QStrListIterator strIt(strlist);
      QStringList list;
      for(; strIt.current() != 0; ++strIt)
        list.append(*strIt);
      Property *prop = new Property("signals", i18n("Events")"",
        new KexiProperty::ListData(list, descList(winfo, list)),
        ));
    }
#endif

    if (tree->container()) { // we are a container -> layout property
        createLayoutProperty(tree);
    }
}

// moved from WidgetPropertySet
void Form::updatePropertyValue(ObjectTreeItem *tree, const char *property, const QMetaProperty &meta)
{
    const char *propertyName = meta.isValid() ? meta.name() : property;
    if (!d->propertySet.contains(propertyName))
        return;
    KoProperty::Property p(d->propertySet[propertyName]);

//! \todo what about set properties, and lists properties
    QHash<QString, QVariant>::ConstIterator it(tree->modifiedProperties()->find(propertyName));
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
}

// moved from WidgetPropertySet
// original: void Form::setSelectedWidget(QWidget *w, bool add, bool forceReload, bool moreWillBeSelected)
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
#ifdef __GNUC__
#warning 2.0: clearSet()?
#else
#pragma WARNING( 2.0: clearSet()? )
#endif
//        clearSet();
        return;
    }

#if 0
    // don't add a widget twice
    if (!forceReload && d->selected.contains(QPointer<QWidget>(w))) {
        kWarning() << "Widget is already selected";
        return;
    }
#endif
    // if our list is empty,don't use add parameter value
    if (d->selected.isEmpty() == 0) {
        flags |= ReplacePreviousSelection;
    }

    QByteArray prevProperty;
    if (flags & ReplacePreviousSelection) {
#if 0
//! @todo
        if (forceReload) {
            showPropertySet(
                0, 
                true //force
            );
            prevProperty = d->propertySet.previousSelection();
        }
#endif
//! @todo clearSet(true)?
#ifdef __GNUC__
#warning 2.0: clearSet(true)?
#else
#pragma WARNING( 2.0: clearSet(true)? )
#endif
//        clearSet(true); //clear but do not reload to avoid blinking
// 2.0: not needed        d->selected.append(QPointer<QWidget>(w));
        createPropertiesForWidget(w);

        w->installEventFilter(this);
        connect(w, SIGNAL(destroyed()), this, SLOT(slotWidgetDestroyed()));
    }
    else {
        addWidget(w);
    }

    if (flags & LastSelection) {
        emit propertySetSwitched();
#if 0 // 2.0
        KFormDesigner::FormManager::self()->showPropertySet(
            this, true, // force
            prevProperty
        );
#endif
   }
 }

KoProperty::Set& Form::propertySet()
{
    return d->propertySet;
}

bool Form::isSnapWidgetsToGridEnabled() const
{
    return d->isSnapWidgetsToGridEnabled;
}

void Form::setSnapWidgetsToGridEnabled(bool set)
{
    d->isSnapWidgetsToGridEnabled = set;
}

// moved from FormManager
void Form::createContextMenu(QWidget *w, Container *container, const QPoint& menuPos) //bool popupAtCursor)
{
    if (!widget())
        return;
    const bool toplevelWidgetSelected = widget() == w;
    const uint widgetsCount = container->form()->selectedWidgets()->count();
    const bool multiple = widgetsCount > 1;
    //const bool enableRemove = w != m_active->widget();
    // We only enablelayout creation if more than one widget with the same parent are selected
    const bool enableLayout = multiple || w == container->widget();

//unused...    m_menuWidget = w;
    QString n = container->form()->library()->displayName(w->metaObject()->className());

    //set title
    QIcon icon;
    QString titleText;
    if (!multiple) {
        if (w == container->form()->widget()) {
            icon = SmallIcon("form");
            titleText = i18n("%1 : Form", w->objectName());
        }
        else {
            icon = SmallIcon(
                       container->form()->library()->iconName(w->metaObject()->className()));
            titleText = QString(w->objectName()) + " : " + n;
        }
    }
    else {
        icon = SmallIcon("multiple_obj");
        titleText = i18n("Multiple Widgets") + QString(" (%1)").arg(widgetsCount);
    }

    KMenu menu;
    menu.addTitle(icon, titleText);

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
    PLUG_ACTION("layout_menu", enableLayout);
    PLUG_ACTION("break_layout", enableLayout);
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

        KMenu *sub = new KMenu(w);
        QWidget *buddy = buddyLabelWidget->buddy();

        noBuddyAction = sub->addAction(i18n("No Buddy"));
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
                KIcon(
                    container->form()->library()->iconName(item->className().toLatin1())),
                item->name()
            );
            if (item->widget() == buddy)
                action->setChecked(true);
        }

// 2.0 not needed        QAction *subAction = menu.addMenu(sub);
// 2.0 not needed        subAction->setText(i18n("Choose Buddy..."));
// 2.0 not needed        connect(sub, SIGNAL(triggered(QAction*)), this, SLOT(buddyChosen(QAction*)));
        separatorNeeded = true;
    }

#ifdef KEXI_DEBUG_GUI
    if (!multiple && (d->features & EnableEvents)) {
        if (separatorNeeded)
            menu.addSeparator();

        // We create the signals menu
        KMenu *sigMenu = new KMenu();
        QList<QMetaMethod> list(
            KexiUtils::methodsForMetaObjectWithParents(w->metaObject(), QMetaMethod::Signal,
                    QMetaMethod::Public));
        foreach(QMetaMethod m, list) {
            sigMenu->addAction(m.signature());
        }
        QAction *eventsSubMenuAction = menu.addMenu(sigMenu);
        eventsSubMenuAction->setText(i18n("Events"));
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
//   for (uint i=oldIndex; i<m_popup->count(); i++) {
//    int id = m_popup->idAt( i );
//    if (id!=-1)
//     menuIds->append( id );
//   }
            //nothing added
            if (separatorNeeded) {
                menu.removeAction(lastAction);
            }
        }
    }

    //show the menu at the selected widget
    /*2.0
    QPoint menuPos;
    if (popupAtCursor) {
        menuPos = QCursor::pos();
    }
    else {
        QWidgetList *lst = container->form()->selectedWidgets();
        QWidget * sel_w = lst ? lst->first() : container->form()->selectedWidget();
        menuPos = sel_w ? sel_w->mapToGlobal(QPoint(sel_w->width() / 2, sel_w->height() / 2)) : QCursor::pos();
    }*/
//    QWidgetList *lst = container->form()->selectedWidgets();
//    QWidget * sel_w = lst ? lst->first() : container->form()->selectedWidget();
//    QPoint realMenuPos = sel_w ? sel_w->mapToGlobal(QPoint(sel_w->width() / 2, sel_w->height() / 2)) : QCursor::pos();
    d->insertionPoint = menuPos; //container->widget()->mapToGlobal(menuPos);

    QAction *result = menu.exec( container->widget()->mapToGlobal(menuPos) );
    
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

// moved from FormManager
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

    K3Command *com = new DeleteWidgetCommand(*this, *list);
    addCommand(com, true);
}

// moved from FormManager
void Form::copyWidget()
{
    if (!objectTree()) {
        return;
    }

    QWidgetList *list = selectedWidgets();
    if (list->isEmpty()) {
        return;
    }

/* 1.x:
    removeChildrenFromList(*list);

    // We clear the current clipboard
    m_domDoc.setContent(QString(), true);
    QDomElement parent = m_domDoc.createElement("UI");
    m_domDoc.appendChild(parent);

    foreach (QWidget *w, *list) {
        ObjectTreeItem *it = objectTree()->lookup(w->objectName());
        if (!it)
            continue;

        FormIO::saveWidget(it, parent, m_domDoc);
    }

    FormIO::cleanClipboard(parent);
*/
// 2.x:
    QDomDocument doc;
    QHash<QByteArray, QByteArray> containers;
    QHash<QByteArray, QByteArray> parents;
    KFormDesigner::widgetsToXML(doc,
        containers, parents, *this, *list);
    KFormDesigner::copyToClipboard(doc.toString());
// \2.x
    emitActionSignals(); // to update 'Paste' item state
}

// moved from FormManager
void Form::cutWidget()
{
    if (!objectTree()) {
        return;
    }

    QWidgetList *list = selectedWidgets();
    if (list->isEmpty()) {
        return;
    }

    K3Command *com = new CutWidgetCommand(*this, *list);
    addCommand(com, true);
}

// moved from FormManager
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

    K3Command *com = new PasteWidgetCommand(doc, *activeContainer(), d->insertionPoint);
    addCommand(com, true);
}

// moved from FormManager
void Form::editTabOrder()
{
    if (!objectTree()) {
        return;
    }
    QWidget *topLevel = widget()->topLevelWidget();
    TabStopDialog dlg(topLevel);
    if (dlg.exec(this) == QDialog::Accepted) {
//2.0 not needed, the code from slot receiving this signal is moved to Form itself
//        //inform about changing "autoTabStop" property
//        // -- this will be received eg. by Kexi, so custom "autoTabStop" property can be updated
//        emit autoTabStopsSet(this, dlg.autoTabStops());
// <moved code>
        d->propertySet.changePropertyIfExists("autoTabStops", dlg.autoTabStops());
// </moved code>

        //force set dirty
        emit modified();
    }
}

// moved from FormManager
void Form::editFormPixmapCollection()
{
    if (!objectTree()) {
        return;
    }

#ifdef __GNUC__
#warning pixmapcollection
#endif
#ifndef KEXI_NO_PIXMAPCOLLECTION
    PixmapCollectionEditor dialog(pixmapCollection(), widget()->topLevelWidget());
    dialog.exec();
#endif
}

// moved from FormManager
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

// moved from FormManager
void Form::alignWidgets(WidgetAlignment alignment)
{
    QWidgetList* selected = selectedWidgets();
    if (!objectTree() || selected->count() < 2) {
        return;
    }

    QWidget *parentWidget = selected->first()->parentWidget();

    foreach (QWidget *w, *selected) {
        if (w->parentWidget() != parentWidget) {
            kDebug() << "alignment ==" << alignment <<  " widgets don't have the same parent widget";
            return;
        }
    }

    K3Command *com = new AlignWidgetsCommand(*this, alignment, *selected);
    addCommand(com, true);
}

// moved from FormManager
void Form::alignWidgetsToLeft()
{
    alignWidgets(AlignToLeft);
}

// moved from FormManager
void Form::alignWidgetsToRight()
{
    alignWidgets(AlignToRight);
}

// moved from FormManager
void Form::alignWidgetsToTop()
{
    alignWidgets(AlignToTop);
}

// moved from FormManager
void Form::alignWidgetsToBottom()
{
    alignWidgets(AlignToBottom);
}

// moved from FormManager
void Form::adjustWidgetSize()
{
    if (!objectTree()) {
        return;
    }

    K3Command *com = new AdjustSizeCommand(*this, AdjustSizeCommand::SizeToFit, *selectedWidgets());
    addCommand(com, true);
}

// moved from FormManager
void Form::alignWidgetsToGrid()
{
    if (!objectTree()) {
        return;
    }

    K3Command *com = new AlignWidgetsCommand(*this, AlignToGrid, *selectedWidgets());
    addCommand(com, true);
}

// moved from FormManager
void Form::adjustSizeToGrid()
{
    if (!objectTree()) {
        return;
    }

    K3Command *com = new AdjustSizeCommand(*this, AdjustSizeCommand::SizeToGrid, *selectedWidgets());
    addCommand(com, true);
}

// moved from FormManager
void Form::adjustWidthToSmall()
{
    if (!objectTree()) {
        return;
    }

    K3Command *com = new AdjustSizeCommand(*this, AdjustSizeCommand::SizeToSmallWidth, *selectedWidgets());
    addCommand(com, true);
}

// moved from FormManager
void Form::adjustWidthToBig()
{
    if (!objectTree()) {
        return;
    }

    K3Command *com = new AdjustSizeCommand(*this, AdjustSizeCommand::SizeToBigWidth, *selectedWidgets());
    addCommand(com, true);
}

// moved from FormManager
void Form::adjustHeightToSmall()
{
    if (!objectTree()) {
        return;
    }

    K3Command *com = new AdjustSizeCommand(*this, AdjustSizeCommand::SizeToSmallHeight, *selectedWidgets());
    addCommand(com, true);
}

// moved from FormManager
void Form::adjustHeightToBig()
{
    if (!objectTree()) {
        return;
    }

    K3Command *com = new AdjustSizeCommand(*this, AdjustSizeCommand::SizeToBigHeight, *selectedWidgets());
    addCommand(com, true);
}

// moved from FormManager
void Form::bringWidgetToFront()
{
    if (!objectTree()) {
        return;
    }

    foreach (QWidget *w, *selectedWidgets()) {
        w->raise();
    }
}

// moved from FormManager
void Form::sendWidgetToBack()
{
    if (!objectTree()) {
        return;
    }

    foreach (QWidget *w, *selectedWidgets()) {
        w->lower();
    }
}

// moved from FormManager
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

// moved from FormManager
void Form::clearWidgetContent()
{
    if (!objectTree()) {
        return;
    }

    foreach (QWidget *w, *selectedWidgets()) {
        library()->clearWidgetContent(w->metaObject()->className(), w);
    }
}

// moved from FormManager
void Form::layoutHBox()
{
    createLayout(HBox);
}

// moved from FormManager
void Form::layoutVBox()
{
    createLayout(VBox);
}

// moved from FormManager
void Form::layoutGrid()
{
    createLayout(Grid);
}

// moved from FormManager
void Form::layoutHSplitter()
{
    createLayout(HSplitter);
}

// moved from FormManager
void Form::layoutVSplitter()
{
    createLayout(VSplitter);
}

// moved from FormManager
void Form::layoutHFlow()
{
    createLayout(HFlow);
}

// moved from FormManager
void Form::layoutVFlow()
{
    createLayout(VFlow);
}

// moved from FormManager
void Form::createLayout(Form::LayoutType layoutType)
{
    QWidgetList *list = selectedWidgets();
    // if only one widget is selected (a container), we modify its layout
    if (list->isEmpty()) {//sanity check
        kWarning() << "list is empty!";
        return;
    }
    if (list->count() == 1) {
        ObjectTreeItem *item = objectTree()->lookup(list->first()->objectName());
        if (!item || !item->container() || !d->propertySet.contains("layout")) {
            return;
        }
        d->propertySet.changeProperty("layout", Container::layoutTypeToString(layoutType));
        return;
    }

    QWidget *parent = list->first()->parentWidget();
    foreach (QWidget *w, *list) {
        kDebug() << "comparing widget " << w->objectName() << " whose parent is " << w->parentWidget()->objectName() << " insteaed of " << parent->objectName();
        if (w->parentWidget() != parent) {
            KMessageBox::sorry(widget()->topLevelWidget(), 
                i18n("<b>Cannot create the layout.</b>\n"
                     "All selected widgets must have the same parent."));
            kDebug() << "widgets don't have the same parent widget";
            return;
        }
    }

    K3Command *com = new CreateLayoutCommand(*this, layoutType, *list);
    addCommand(com, true);
}

// moved from FormManager
void Form::breakLayout()
{
    if (!objectTree()) {
        return;
    }

    Container *container = activeContainer();
    QByteArray c(container->widget()->metaObject()->className());

    if ((c == "Grid") || (c == "VBox") || (c == "HBox") || (c == "HFlow") || (c == "VFlow")) {
        K3Command *com = new BreakLayoutCommand(*container);
        addCommand(com, true);
    }
    else { // normal container
        if (selectedWidgets()->count() == 1)
            d->propertySet.changeProperty("layout", "NoLayout");
        else
            container->setLayoutType(NoLayout);
    }
}

// Alignment-related functions /////////////////////////////

void Form::createAlignProperty(const QMetaProperty& meta, QWidget *widget, QWidget *subwidget)
{
    if (!objectTree())
        return;

    QStringList list;
    QString value;
    const int alignment = subwidget->property("alignment").toInt();
    const QList<QByteArray> keys(meta.enumerator().valueToKeys(alignment).split('|'));

    const QStringList possibleValues(KexiUtils::enumKeysForProperty(meta));
    ObjectTreeItem *tree = objectTree()->lookup(widget->objectName());
    const bool isTopLevel = isTopLevelWidget(widget);

    if (possibleValues.contains("AlignHCenter"))  {
        // Create the horizontal alignment property
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

        list << "AlignAuto" << "AlignLeft" << "AlignRight"
            << "AlignHCenter" << "AlignJustify";
        KoProperty::Property *p = new KoProperty::Property(
            "hAlign", d->createValueList(0, list), value,
            i18nc("Translators: please keep this string short (less than 20 chars)", "Hor. Alignment"),
            i18n("Horizontal Alignment"));
        d->propertySet.addProperty(p);
        if (!isPropertyVisible(p->name(), isTopLevel)) {
            p->setVisible(false);
        }
        updatePropertyValue(tree, "hAlign");
        list.clear();
    }

    if (possibleValues.contains("AlignTop")) {
        // Create the ver alignment property
        if (keys.contains("AlignTop"))
            value = "AlignTop";
        else if (keys.contains("AlignBottom"))
            value = "AlignBottom";
        else
            value = "AlignVCenter";

        list << "AlignTop" << "AlignVCenter" << "AlignBottom";
        KoProperty::Property *p = new KoProperty::Property(
            "vAlign", d->createValueList(0, list), value,
            i18nc("Translators: please keep this string short (less than 20 chars)", "Ver. Alignment"),
            i18n("Vertical Alignment"));
        d->propertySet.addProperty(p);
        if (!isPropertyVisible(p->name(), isTopLevel)) {
            p->setVisible(false);
        }
        updatePropertyValue(tree, "vAlign");
    }


    if (possibleValues.contains("WordBreak")
//  && isPropertyVisible("wordbreak", false, subwidget->className())
//   && !subWidget->inherits("QLineEdit") /* QLineEdit doesn't support 'word break' is this generic enough?*/
       ) {
        // Create the wordbreak property
        KoProperty::Property *p = new KoProperty::Property("wordbreak",
                QVariant((bool)(alignment & Qt::TextWordWrap)), i18n("Word Break"), i18n("Word Break"));
        d->propertySet.addProperty(p);
        updatePropertyValue(tree, "wordbreak");
        if (!m_lib->isPropertyVisible(
                    subwidget->metaObject()->className(), subwidget, p->name(), false/*multiple*/, isTopLevel)) {
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
    int count = KexiUtils::indexOfPropertyWithSuperclasses(subwidget, "alignment");
    const QMetaProperty meta(
        KexiUtils::findPropertyWithSuperclasses(subwidget, count));
    const int valueForKeys = meta.enumerator().keysToValue(list.join("|").toLatin1());
    subwidget->setProperty("alignment", valueForKeys);

    ObjectTreeItem *tree = objectTree()->lookup(d->selected.first()->objectName());
    if (tree && d->propertySet[ property.toLatin1()].isModified())
        tree->addModifiedProperty(
            property.toLatin1(), d->propertySet[property.toLatin1()].oldValue());

    if (d->isUndoing) {
        return;
    }

    if (d->lastCommand && d->lastCommand->property() == "alignment")
        d->lastCommand->setValue(valueForKeys);
    else {
        d->lastCommand = new PropertyCommand(*this, d->selected.first()->objectName().toLatin1(),
                                             subwidget->property("alignment"), valueForKeys, "alignment");
        addCommand(d->lastCommand, false);
    }
}

// Layout-related functions  //////////////////////////

void Form::createLayoutProperty(ObjectTreeItem *item)
{
    Container *container = item->container();
    if (!container || !objectTree() || !container->widget()) {
        return;
    }
    // special containers have no 'layout' property, as it should not be changed
    QByteArray className = container->widget()->metaObject()->className();
    if ((className == "HBox") || (className == "VBox") || (className == "Grid"))
        return;

    QStringList list;
    QString value = Container::layoutTypeToString(container->layoutType());

    list << "NoLayout" << "HBox" << "VBox" << "Grid" << "HFlow" << "VFlow";

    KoProperty::Property *p = new KoProperty::Property("layout", d->createValueList(0, list), value,
            i18n("Container's Layout"), i18n("Container's Layout"));
    p->setVisible(container->form()->library()->advancedPropertiesVisible());
    d->propertySet.addProperty(p);

    updatePropertyValue(item, "layout");

    p = new KoProperty::Property("layoutMargin", container->layoutMargin(),
                                 i18n("Layout Margin"), i18n("Layout Margin"));
    d->propertySet.addProperty(p);
    updatePropertyValue(item, "layoutMargin");
    if (container->layoutType() == NoLayout)
        p->setVisible(false);

    p = new KoProperty::Property("layoutSpacing", container->layoutSpacing(),
                                 i18n("Layout Spacing"), i18n("Layout Spacing"));
    d->propertySet.addProperty(p);
    updatePropertyValue(item, "layoutSpacing");
    if (container->layoutType() == NoLayout)
        p->setVisible(false);

}

void Form::saveLayoutProperty(const QString &prop, const QVariant &value)
{
    Container *container = 0;
    if (!objectTree()) {
        return;
    }
    ObjectTreeItem *item = objectTree()->lookup( d->selected.first()->objectName() );
    if (!item)
        return;
    container = item->container();

    if (prop == "layout") {
        LayoutType type = Container::stringToLayoutType(value.toString());

        if (d->lastCommand && d->lastCommand->property() == "layout" && !d->isUndoing)
            d->lastCommand->setValue(value);
        else if (!d->isUndoing)  {
            d->lastCommand = new LayoutPropertyCommand(*this, 
                d->selected.first()->objectName().toLatin1(),
                d->propertySet["layout"].oldValue(), value);
            addCommand(d->lastCommand, false);
        }

        container->setLayoutType(type);
        bool show = (type != NoLayout);
        if (show != d->propertySet["layoutMargin"].isVisible())  {
            d->propertySet["layoutMargin"].setVisible(show);
            d->propertySet["layoutSpacing"].setVisible(show);
#if 0
//! @todo
            showPropertySet(this, true/*force*/);
#endif
        }
        return;
    }

    if (prop == "layoutMargin" && container->layout()) {
        container->setLayoutMargin(value.toInt());
        container->layout()->setMargin(value.toInt());
    } else if (prop == "layoutSpacing" && container->layout())  {
        container->setLayoutSpacing(value.toInt());
        container->layout()->setSpacing(value.toInt());
    }

    ObjectTreeItem *tree = objectTree()->lookup( d->selected.first()->objectName() );
    if (tree && d->propertySet[ prop.toLatin1()].isModified())
        tree->addModifiedProperty(prop.toLatin1(), d->propertySet[prop.toLatin1()].oldValue());

    if (d->isUndoing)
        return;

    if (d->lastCommand && (QString(d->lastCommand->property()) == prop)) {
        d->lastCommand->setValue(value);
    }
    else  {
        d->lastCommand = new PropertyCommand(*this, 
            d->selected.first()->objectName().toLatin1(),
            d->propertySet[ prop.toLatin1()].oldValue(), value, prop.toLatin1()
        );
        addCommand(d->lastCommand, false);
    }
}

void Form::saveEnabledProperty(bool value)
{
    foreach(QWidget* widget, d->selected) {
        ObjectTreeItem *tree = objectTree()->lookup(widget->objectName());
        if (tree->isEnabled() == value)
            continue;

        QPalette p(widget->palette());
        if (!d->origActiveColors)
            d->origActiveColors = new QColorGroup(p.active());
        if (value) {
            if (d->origActiveColors)
                p.setActive(*d->origActiveColors);   //revert
        } else {
            QColorGroup cg = p.disabled();
            //also make base color a bit disabled-like
            cg.setColor(QColorGroup::Base, cg.color(QColorGroup::Background));
            p.setActive(cg);
        }
        widget->setPalette(p);

        tree->setEnabled(value);
        handleWidgetPropertyChanged(widget, "enabled", QVariant(value));
    }
}

void Form::createPropertyCommandsInDesignMode(QWidget* widget,
        const QHash<QByteArray, QVariant> &propValues, CommandGroup *group, bool addToActiveForm,
        bool execFlagForSubCommands)
{
    if (!widget || propValues.isEmpty())
        return;

    //is this widget selected? (if so, use property system)
    const bool widgetIsSelected = selectedWidget() == widget;

    d->slotPropertyChanged_addCommandEnabled = false;
    QHash<QByteArray, QVariant>::ConstIterator endIt = propValues.constEnd();
// CommandGroup *group = new CommandGroup(commandName);
    for (QHash<QByteArray, QVariant>::ConstIterator it = propValues.constBegin(); it != endIt; ++it) {
        if (!d->propertySet.contains(it.key())) {
            kWarning() << "\"" << it.key() << "\" property not found";
            continue;
        }
        PropertyCommand *subCommand = new PropertyCommand(*this, widget->objectName().toLatin1(),
                widget->property(it.key()), it.value(), it.key());
        group->addCommand(subCommand, execFlagForSubCommands);
        if (widgetIsSelected) {
            d->propertySet[it.key()].setValue(it.value());
        } else {
            WidgetWithSubpropertiesInterface* subpropIface = dynamic_cast<WidgetWithSubpropertiesInterface*>(widget);
            QWidget *subwidget = (subpropIface && subpropIface->subwidget()) ? subpropIface->subwidget() : widget;
            if (-1 != KexiUtils::indexOfPropertyWithSuperclasses(subwidget, it.key())
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
    if (addToActiveForm) {
        addCommand(group, false/*no exec*/);
    }
    d->slotPropertyChanged_addCommandEnabled = true;
// }
}

//moved from KexiFormPart::slotPropertyChanged()
void Form::handleWidgetPropertyChanged(QWidget *w, const QByteArray &name, const QVariant &value)
{
    Q_UNUSED(w);

    if (name == "autoTabStops") {
        //QWidget *w = KFormDesigner::FormManager::self()->activeForm()->selectedWidget();
        //update autoTabStops setting at KFD::Form level
        setAutoTabStops(value.toBool());
    }
    if (widget() && name == "geometry") {
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
        if (m_lib->isPropertyVisible(widget->metaObject()->className(), widget, "font")) {
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
//?        QWidget *widget = widgetsWithFontProperty.first();
        if (QDialog::Accepted != KFontDialog::getFont(font, false, widget()))
            return;
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
        //!      do it when KoProperty::Set supports multiple selections
        widget->setFont(prevFont);
    }
//! @todo temporary fix for dirty flag
    emit modified();
}

#include "form.moc"
