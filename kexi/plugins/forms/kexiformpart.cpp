/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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

#include <kdebug.h>
#include <kgenericfactory.h>
#include <KDialog>
#include <k3listview.h>
#include <ktabwidget.h>
#include <kiconloader.h>
#include <kcombobox.h>
#include <kapplication.h>
#include <kconfig.h>
#include <KAction>
#include <KActionMenu>
#include <KActionCollection>

#include <KexiView.h>
#include <KexiWindow.h>
#include <KexiMainWindowIface.h>
#include <kexiproject.h>
#include <kexipartitem.h>
#include <kexidatasourcecombobox.h>
#include <kexidb/connection.h>
#include <kexidb/fieldlist.h>
#include <kexidb/field.h>
#include <kexiutils/utils.h>

#include <kexi_global.h>
#include <form.h>
#include <formIO.h>
#include <widgetpropertyset.h>
#include <widgetlibrary.h>
#include <objecttreeview.h>
#include <koproperty/Property.h>

#include "kexiformview.h"
#include "widgets/kexidbform.h"
#include "kexiformscrollview.h"
#include "kexiactionselectiondialog.h"
#include "kexiformmanager.h"
#include "kexiformpart.h"
#include "kexidatasourcepage.h"

//! @todo #define KEXI_SHOW_SPLITTER_WIDGET

KFormDesigner::WidgetLibrary* KexiFormPart::static_formsLibrary = 0L;

//! @internal
class KexiFormPart::Private
{
public:
    Private() {
    }
    ~Private() {
        delete static_cast<KFormDesigner::ObjectTreeView*>(objectTreeView);
        delete static_cast<KexiDataSourcePage*>(dataSourcePage);
    }
//  QPointer<KFormDesigner::FormManager> manager;
    QPointer<KFormDesigner::ObjectTreeView> objectTreeView;
    QPointer<KexiDataSourcePage> dataSourcePage;
    KexiDataSourceComboBox *dataSourceCombo;
};

KexiFormPart::KexiFormPart(QObject *parent, const QStringList &l)
        : KexiPart::Part(parent, l)
        , d(new Private())
{
    kDebug();
    setInternalPropertyValue("instanceName",
                             i18nc("Translate this word using only lowercase alphanumeric characters (a..z, 0..9). "
                                   "Use '_' character instead of spaces. First character should be a..z character. "
                                   "If you cannot use latin characters in your language, use english word.",
                                   "form"));
    setInternalPropertyValue("instanceCaption", i18n("Form"));
    setInternalPropertyValue("instanceToolTip", i18nc("tooltip", "Create new form"));
    setInternalPropertyValue("instanceWhatsThis", i18nc("what's this", "Creates new form."));
    setSupportedViewModes(Kexi::DataViewMode | Kexi::DesignViewMode);
    setInternalPropertyValue("newObjectsAreDirty", true);

    // Only create form manager if it's not yet created.
    // KexiReportPart could have created it already.
    KFormDesigner::FormManager *formManager = KFormDesigner::FormManager::self();
    if (!formManager) {
        formManager = new KexiFormManager(this);
        formManager->setObjectName("kexi_form_and_report_manager");
    }

    // Create and store a handle to forms' library. Reports will have their own library too.
    /* @todo add configuration for supported factory groups */
    QStringList supportedFactoryGroups;
    supportedFactoryGroups += "kexi";
    static_formsLibrary = KFormDesigner::FormManager::createWidgetLibrary(
                              formManager, supportedFactoryGroups);
    static_formsLibrary->setAdvancedPropertiesVisible(false);
    connect(static_formsLibrary, SIGNAL(widgetCreated(QWidget*)),
            this, SLOT(slotWidgetCreatedByFormsLibrary(QWidget*)));

    connect(KFormDesigner::FormManager::self()->propertySet(),
            SIGNAL(widgetPropertyChanged(QWidget *, const QByteArray &, const QVariant&)),
            this, SLOT(slotPropertyChanged(QWidget *, const QByteArray &, const QVariant&)));
    connect(KFormDesigner::FormManager::self(), SIGNAL(autoTabStopsSet(KFormDesigner::Form*, bool)),
            this, SLOT(slotAutoTabStopsSet(KFormDesigner::Form*, bool)));
}

KexiFormPart::~KexiFormPart()
{
    delete d;
}

KFormDesigner::WidgetLibrary* KexiFormPart::library()
{
    return static_formsLibrary;
}

#if 0
void KexiFormPart::initPartActions(KActionCollection *collection)
{
//this is automatic? -no
//create child guicilent: guiClient()->setXMLFile("kexidatatableui.rc");

    kDebug() << "FormPart INIT ACTIONS***********************************************************************";
    //TODO

    //guiClient()->setXMLFile("kexiformui.rc");
//js m_manager->createActions(collection, 0);
}

void KexiFormPart::initInstanceActions(int mode, KActionCollection *col)
{
    if (mode == Kexi::DesignViewMode) {
        KFormDesigner::FormManager::self()->createActions(col, 0);
        new KAction(i18n("Edit Tab Order..."), "tab_order", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(editTabOrder()), col, "taborder");
        new KAction(i18n("Adjust Size"), "zoom-fit-best", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(adjustWidgetSize()), col, "adjust");
    }
    //TODO
}
#endif

void KexiFormPart::initPartActions()
{
// new KAction(i18n("Show Form UI Code"), "show_form_ui", CTRL+Qt::Key_U, m_manager, SLOT(showFormUICode()),
//  guiClient()->actionCollection(), "show_form_ui");
}

void KexiFormPart::initInstanceActions()
{
    KActionCollection *col = actionCollectionForMode(Kexi::DesignViewMode);
#ifdef KEXI_DEBUG_GUI
    KConfigGroup generalGroup(KGlobal::config()->group("General"));
    if (generalGroup.readEntry("showInternalDebugger", false)) {
        KAction *a;
        col->addAction("show_form_ui",
                       a = new KAction(KIcon("run-build-file"), i18n("Show Form UI Code"), this));
        a->setShortcut(Qt::CTRL + Qt::Key_U);
        connect(a, SIGNAL(triggered()),
                KFormDesigner::FormManager::self(), SLOT(showFormUICode()));
    }
#endif

    KFormDesigner::FormManager::self()->createActions(library(), col,
            (KXMLGUIClient*)col->parentGUIClient());

    //connect actions provided by widget factories
    connect(col->action("widget_assign_action"), SIGNAL(activated()),
            this, SLOT(slotAssignAction()));

    createSharedAction(Kexi::DesignViewMode, i18n("Clear Widget Contents"),
                       "edit-clear", KShortcut(), "formpart_clear_contents");
    createSharedAction(Kexi::DesignViewMode, i18n("Edit Tab Order..."),
                       "tab_order", KShortcut(), "formpart_taborder");
//TODO createSharedAction(Kexi::DesignViewMode, i18n("Edit Pixmap Collection"), "icons", 0, "formpart_pixmap_collection");
//TODO createSharedAction(Kexi::DesignViewMode, i18n("Edit Form Connections"), "connections", 0, "formpart_connections");

// KFormDesigner::CreateLayoutCommand

    KAction *action = createSharedAction(Kexi::DesignViewMode, i18n("Layout Widgets"),
                                         "", KShortcut(), "formpart_layout_menu", "KActionMenu");
    KActionMenu *menu = static_cast<KActionMenu*>(action);

    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("&Horizontally"),
                                       QString(), KShortcut(), "formpart_layout_hbox"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("&Vertically"),
                                       QString(), KShortcut(), "formpart_layout_vbox"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("In &Grid"),
                                       QString(), KShortcut(), "formpart_layout_grid"));
#ifdef KEXI_SHOW_SPLITTER_WIDGET
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("Horizontally in &Splitter"),
                                       QString(), KShortcut(), "formpart_layout_hsplitter"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("Verti&cally in Splitter"),
                                       QString(), KShortcut(), "formpart_layout_vsplitter"));
#endif

    createSharedAction(Kexi::DesignViewMode, i18n("&Break Layout"), QString(),
                       KShortcut(), "formpart_break_layout");
    /*
      createSharedAction(Kexi::DesignViewMode, i18n("Lay Out Widgets &Horizontally"), QString(), 0, "formpart_layout_hbox");
      createSharedAction(Kexi::DesignViewMode, i18n("Lay Out Widgets &Vertically"), QString(), 0, "formpart_layout_vbox");
      createSharedAction(Kexi::DesignViewMode, i18n("Lay Out Widgets in &Grid"), QString(), 0, "formpart_layout_grid");
    */
    createSharedAction(Kexi::DesignViewMode, i18n("Bring Widget to Front"), "raise",
                       KShortcut(), "formpart_format_raise");
    createSharedAction(Kexi::DesignViewMode, i18n("Send Widget to Back"), "lower",
                       KShortcut(), "formpart_format_lower");

#ifndef KEXI_NO_UNFINISHED
    action = createSharedAction(Kexi::DesignViewMode, i18n("Other Widgets"), "",
                                KShortcut(), "other_widgets_menu", "KActionMenu");
#endif

    action = createSharedAction(Kexi::DesignViewMode, i18n("Align Widgets Position"),
                                "aoleft", KShortcut(), "formpart_align_menu", "KActionMenu");
    menu = static_cast<KActionMenu*>(action);
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Left"), "aoleft",
                                       KShortcut(), "formpart_align_to_left"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Right"), "aoright",
                                       KShortcut(), "formpart_align_to_right"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Top"), "aotop",
                                       KShortcut(), "formpart_align_to_top"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Bottom"), "aobottom",
                                       KShortcut(), "formpart_align_to_bottom"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Grid"), "aopos2grid",
                                       KShortcut(), "formpart_align_to_grid"));

    action = createSharedAction(Kexi::DesignViewMode, i18n("Adjust Widgets Size"), "aogrid",
                                KShortcut(), "formpart_adjust_size_menu", "KActionMenu");
    menu = static_cast<KActionMenu*>(action);
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Fit"), "aofit",
                                       KShortcut(), "formpart_adjust_to_fit"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Grid"), "aogrid",
                                       KShortcut(), "formpart_adjust_size_grid"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Shortest"), "aoshortest",
                                       KShortcut(), "formpart_adjust_height_small"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Tallest"), "aotallest",
                                       KShortcut(), "formpart_adjust_height_big"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Narrowest"), "aonarrowest",
                                       KShortcut(), "formpart_adjust_width_small"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Widest"), "aowidest",
                                       KShortcut(), "formpart_adjust_width_big"));
}

KexiWindowData*
KexiFormPart::createWindowData(KexiWindow* window)
{
    return new KexiFormPart::TempData(window);
}

KexiView* KexiFormPart::createView(QWidget *parent, KexiWindow* window,
                                   KexiPart::Item &item, Kexi::ViewMode viewMode, QMap<QString, QVariant>*)
{
    Q_UNUSED(window);
    Q_UNUSED(viewMode);

    kDebug();
    KexiMainWindowIface *win = KexiMainWindowIface::global();
    if (!win || !win->project() || !win->project()->dbConnection())
        return 0;

    KexiFormView *view = new KexiFormView(parent, win->project()->dbConnection());
    view->setObjectName(item.name().toLatin1());
    return view;
}

void
KexiFormPart::generateForm(KexiDB::FieldList *list, QDomDocument &domDoc)
{
    //this form generates a .ui from FieldList list
    //basically that is a Label and a LineEdit for each field
    domDoc = QDomDocument("UI");
    QDomElement uiElement = domDoc.createElement("UI");
    domDoc.appendChild(uiElement);
    uiElement.setAttribute("version", "3.1");
    uiElement.setAttribute("stdsetdef", 1);

    QDomElement baseClass = domDoc.createElement("class");
    uiElement.appendChild(baseClass);
    QDomText baseClassV = domDoc.createTextNode("QWidget");
    baseClass.appendChild(baseClassV);
    QDomElement baseWidget = domDoc.createElement("widget");
    baseWidget.setAttribute("class", "QWidget");

    int y = 0;

    for (unsigned int i = 0; i < list->fieldCount(); i++) {
        QDomElement lclass = domDoc.createElement("widget");
        baseWidget.appendChild(lclass);
        lclass.setAttribute("class", "QLabel");
        QDomElement lNameProperty = domDoc.createElement("property");
        lNameProperty.setAttribute("name", "name");
        QDomElement lType = domDoc.createElement("cstring");
        QDomText lClassN = domDoc.createTextNode(QString("l%1").arg(list->field(i)->name()));
        lType.appendChild(lClassN);
        lNameProperty.appendChild(lType);
        lclass.appendChild(lNameProperty);

        QDomElement gNameProperty = domDoc.createElement("property");
        gNameProperty.setAttribute("name", "geometry");
        QDomElement lGType = domDoc.createElement("rect");

        QDomElement lx = domDoc.createElement("x");
        QDomText lxV = domDoc.createTextNode("10");
        lx.appendChild(lxV);
        QDomElement ly = domDoc.createElement("y");
        QDomText lyV = domDoc.createTextNode(QString::number(y + 10));
        ly.appendChild(lyV);
        QDomElement lWidth = domDoc.createElement("width");
        QDomText lWidthV = domDoc.createTextNode("100");
        lWidth.appendChild(lWidthV);
        QDomElement lHeight = domDoc.createElement("height");
        QDomText lHeightV = domDoc.createTextNode("20");
        lHeight.appendChild(lHeightV);

        lGType.appendChild(lx);
        lGType.appendChild(ly);
        lGType.appendChild(lWidth);
        lGType.appendChild(lHeight);

        gNameProperty.appendChild(lGType);
        lclass.appendChild(gNameProperty);

        QDomElement tNameProperty = domDoc.createElement("property");
        tNameProperty.setAttribute("name", "text");
        QDomElement lTType = domDoc.createElement("string");
        QDomText lTextV = domDoc.createTextNode(list->field(i)->name());
        lTType.appendChild(lTextV);
        tNameProperty.appendChild(lTType);
        lclass.appendChild(tNameProperty);


        ///line edit!


        QDomElement vclass = domDoc.createElement("widget");
        baseWidget.appendChild(vclass);
        vclass.setAttribute("class", "KLineEdit");
        QDomElement vNameProperty = domDoc.createElement("property");
        vNameProperty.setAttribute("name", "name");
        QDomElement vType = domDoc.createElement("cstring");
        QDomText vClassN = domDoc.createTextNode(list->field(i)->name());
        vType.appendChild(vClassN);
        vNameProperty.appendChild(vType);
        vclass.appendChild(vNameProperty);

        QDomElement vgNameProperty = domDoc.createElement("property");
        vgNameProperty.setAttribute("name", "geometry");
        QDomElement vGType = domDoc.createElement("rect");

        QDomElement vx = domDoc.createElement("x");
        QDomText vxV = domDoc.createTextNode("110");
        vx.appendChild(vxV);
        QDomElement vy = domDoc.createElement("y");
        QDomText vyV = domDoc.createTextNode(QString::number(y + 10));
        vy.appendChild(vyV);
        QDomElement vWidth = domDoc.createElement("width");
        QDomText vWidthV = domDoc.createTextNode("200");
        vWidth.appendChild(vWidthV);
        QDomElement vHeight = domDoc.createElement("height");
        QDomText vHeightV = domDoc.createTextNode("20");
        vHeight.appendChild(vHeightV);

        vGType.appendChild(vx);
        vGType.appendChild(vy);
        vGType.appendChild(vWidth);
        vGType.appendChild(vHeight);

        vgNameProperty.appendChild(vGType);
        vclass.appendChild(vgNameProperty);

        y += 20;
    }

    QDomElement lNameProperty = domDoc.createElement("property");
    lNameProperty.setAttribute("name", "name");
    QDomElement lType = domDoc.createElement("cstring");
    QDomText lClassN = domDoc.createTextNode("DBForm");
    lType.appendChild(lClassN);
    lNameProperty.appendChild(lType);
    baseWidget.appendChild(lNameProperty);

    QDomElement wNameProperty = domDoc.createElement("property");
    wNameProperty.setAttribute("name", "geometry");
    QDomElement wGType = domDoc.createElement("rect");

    QDomElement wx = domDoc.createElement("x");
    QDomText wxV = domDoc.createTextNode("0");
    wx.appendChild(wxV);
    QDomElement wy = domDoc.createElement("y");
    QDomText wyV = domDoc.createTextNode("0");
    wy.appendChild(wyV);
    QDomElement wWidth = domDoc.createElement("width");
    QDomText wWidthV = domDoc.createTextNode("340");
    wWidth.appendChild(wWidthV);
    QDomElement wHeight = domDoc.createElement("height");
    QDomText wHeightV = domDoc.createTextNode(QString::number(y + 30));
    wHeight.appendChild(wHeightV);

    wGType.appendChild(wx);
    wGType.appendChild(wy);
    wGType.appendChild(wWidth);
    wGType.appendChild(wHeight);

    wNameProperty.appendChild(wGType);
    baseWidget.appendChild(wNameProperty);

    uiElement.appendChild(baseWidget);
}

void KexiFormPart::slotAutoTabStopsSet(KFormDesigner::Form *form, bool set)
{
    Q_UNUSED(form);

    KoProperty::Property &p
    = (*KFormDesigner::FormManager::self()->propertySet())["autoTabStops"];
    if (!p.isNull())
        p.setValue(QVariant(set));
}

void KexiFormPart::slotAssignAction()
{
    KexiDBForm *dbform;
    if (!KFormDesigner::FormManager::self()->activeForm()
            || !KFormDesigner::FormManager::self()->activeForm()->designMode()
            || !(dbform = dynamic_cast<KexiDBForm*>(KFormDesigner::FormManager::self()->activeForm()->formWidget())))
        return;

    KFormDesigner::WidgetPropertySet * propSet
    = KFormDesigner::FormManager::self()->propertySet();

    KexiFormEventAction::ActionData data;
    KoProperty::Property &onClickActionProp = propSet->property("onClickAction");
    if (! onClickActionProp.isNull())
        data.string = onClickActionProp.value().toString();

    KoProperty::Property &onClickActionOptionProp = propSet->property("onClickActionOption");
    if (!onClickActionOptionProp.isNull())
        data.option = onClickActionOptionProp.value().toString();

    KexiFormScrollView *scrollViewWidget
    = dynamic_cast<KexiFormScrollView*>(dbform->dataAwareObject());
    if (!scrollViewWidget)
        return;
    KexiFormView* formViewWidget = dynamic_cast<KexiFormView*>(scrollViewWidget->parent());
    if (!formViewWidget)
        return;

    KexiActionSelectionDialog dlg(dbform, data,
                                  propSet->property("name").value().toString());

    if (dlg.exec() == QDialog::Accepted) {
        data = dlg.currentAction();
        //update property value
        propSet->property("onClickAction").setValue(data.string);
        propSet->property("onClickActionOption").setValue(data.option);
    }
}

KLocalizedString KexiFormPart::i18nMessage(
    const QString& englishMessage, KexiWindow* window) const
{
    Q_UNUSED(window);
    if (englishMessage == "Design of object \"%1\" has been modified.")
        return ki18n(I18N_NOOP("Design of form \"%1\" has been modified."));
    if (englishMessage == "Object \"%1\" already exists.")
        return ki18n(I18N_NOOP("Form \"%1\" already exists."));

    return Part::i18nMessage(englishMessage, window);
}

void
KexiFormPart::slotPropertyChanged(QWidget *w, const QByteArray &name, const QVariant &value)
{
    Q_UNUSED(w);

    if (!KFormDesigner::FormManager::self()->activeForm())
        return;
    if (name == "autoTabStops") {
        //QWidget *w = KFormDesigner::FormManager::self()->activeForm()->selectedWidget();
        //update autoTabStops setting at KFD::Form level
        KFormDesigner::FormManager::self()->activeForm()->setAutoTabStops(value.toBool());
    }
    if (KFormDesigner::FormManager::self()->activeForm()->widget() && name == "geometry") {
        //fall back to sizeInternal property....
        if (KFormDesigner::FormManager::self()->propertySet()->contains("sizeInternal"))
            KFormDesigner::FormManager::self()->propertySet()->property("sizeInternal").setValue(
                value.toRect().size());
    }
}

/*KFormDesigner::FormManager*
KexiFormPart::manager() const
{
  return d->manager;
}*/

KexiDataSourcePage* KexiFormPart::dataSourcePage() const
{
    return d->dataSourcePage;
}

void KexiFormPart::setupCustomPropertyPanelTabs(KTabWidget *tab)
{
    if (!d->objectTreeView) {
        d->objectTreeView = new KFormDesigner::ObjectTreeView(0);
        d->objectTreeView->setObjectName("KexiFormPart:ObjectTreeView");
        KFormDesigner::FormManager::self()->setObjectTreeView(d->objectTreeView); //important: assign to manager
        d->dataSourcePage = new KexiDataSourcePage(0);
        d->dataSourcePage->setObjectName("dataSourcePage");
        connect(d->dataSourcePage,
                SIGNAL(jumpToObjectRequested(const QString&, const QString&)),
                KexiMainWindowIface::global()->thisWidget(),
                SLOT(highlightObject(const QString&, const QString&)));
        connect(d->dataSourcePage,
                SIGNAL(formDataSourceChanged(const QByteArray&, const QByteArray&)),
                KFormDesigner::FormManager::self(),
                SLOT(setFormDataSource(const QByteArray&, const QByteArray&)));
        connect(d->dataSourcePage,
                SIGNAL(dataSourceFieldOrExpressionChanged(const QString&, const QString&, KexiDB::Field::Type)),
                KFormDesigner::FormManager::self(),
                SLOT(setDataSourceFieldOrExpression(const QString&, const QString&, KexiDB::Field::Type)));
        connect(d->dataSourcePage,
                SIGNAL(insertAutoFields(const QString&, const QString&, const QStringList&)),
                KFormDesigner::FormManager::self(),
                SLOT(insertAutoFields(const QString&, const QString&, const QStringList&)));
    }

    KexiProject *prj = KexiMainWindowIface::global()->project();
    d->dataSourcePage->setProject(prj);

    tab->addTab(d->dataSourcePage, KIcon("database"), "");
    tab->setTabToolTip(tab->indexOf(d->dataSourcePage), i18n("Data Source"));

    tab->addTab(d->objectTreeView, KIcon("widgets"), "");
    tab->setTabToolTip(tab->indexOf(d->objectTreeView), i18n("Widgets"));
}

void KexiFormPart::slotWidgetCreatedByFormsLibrary(QWidget* widget)
{
    QList<QMetaMethod> _signals(KexiUtils::methodsForMetaObject(
                                    widget->metaObject(), QMetaMethod::Signal));

    if (!_signals.isEmpty()) {
        const char *handleDragMoveEventSignal = "handleDragMoveEvent(QDragMoveEvent*)";
        const char *handleDropEventSignal = "handleDropEvent(QDropEvent*)";
        KexiFormView *formView = KexiUtils::findParent<KexiFormView*>(widget);

        foreach(const QMetaMethod& method, _signals) {
            if (0 == qstrcmp(method.signature(), handleDragMoveEventSignal)) {
                kDebug() << method.signature();
                if (formView) {
                    connect(widget, SIGNAL(handleDragMoveEvent(QDragMoveEvent*)),
                            formView, SLOT(slotHandleDragMoveEvent(QDragMoveEvent*)));
                }
            } else if (0 == qstrcmp(method.signature(), handleDropEventSignal)) {
                kDebug() << method.signature();
                if (formView) {
                    connect(widget, SIGNAL(handleDropEvent(QDropEvent*)),
                            formView, SLOT(slotHandleDropEvent(QDropEvent*)));
                }
            }
        }
    }
}

//----------------

KexiFormPart::TempData::TempData(QObject* parent)
        : KexiWindowData(parent)
{
}

KexiFormPart::TempData::~TempData()
{
}

#include "kexiformpart.moc"
