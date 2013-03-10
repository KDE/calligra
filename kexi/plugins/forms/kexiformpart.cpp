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

#include <QVBoxLayout>

#include <KDebug>
#include <KDialog>
#include <ktabwidget.h>
#include <kcombobox.h>
#include <kapplication.h>
#include <kconfig.h>
#include <KAction>
#include <KActionMenu>
#include <KActionCollection>

#include <KoIcon.h>

#include <KexiView.h>
#include <KexiWindow.h>
#include <KexiMainWindowIface.h>
#include <kexiproject.h>
#include <kexipartitem.h>
#include <widget/KexiDataSourceComboBox.h>
#include <db/connection.h>
#include <db/fieldlist.h>
#include <db/field.h>
#include <kexiutils/utils.h>

#include <kexi_global.h>
#include <formeditor/form.h>
#include <formeditor/formIO.h>
//2.0 #include <formeditor/widgetpropertyset.h>
#include <formeditor/WidgetTreeWidget.h>

#include <koproperty/Property.h>
#include <koproperty/Set.h>

#include "kexiformview.h"
#include "widgets/kexidbform.h"
#include "kexiformscrollview.h"
#include "kexiformmanager.h"
#include "kexiformpart.h"
#include "kexidatasourcepage.h"

//! @todo #define KEXI_SHOW_SPLITTER_WIDGET

//! @internal
class KexiFormPart::Private
{
public:
    Private() {
    }
    ~Private() {
        delete static_cast<QWidget*>(widgetTreeWidget);
        delete static_cast<KexiDataSourcePage*>(dataSourcePage);
    }
    QPointer<KexiDataSourcePage> dataSourcePage;
    QPointer<KFormDesigner::WidgetTreeWidget> widgetTree;
    QPointer<QWidget> widgetTreeWidget;
    KexiDataSourceComboBox *dataSourceCombo;
};

KexiFormPart::KexiFormPart(QObject *parent, const QVariantList &l)
  : KexiPart::Part(parent,
        i18nc("Translate this word using only lowercase alphanumeric characters (a..z, 0..9). "
              "Use '_' character instead of spaces. First character should be a..z character. "
              "If you cannot use latin characters in your language, use english word.",
              "form"),
        i18nc("tooltip", "Create new form"),
        i18nc("what's this", "Creates new form."),
        l)
  , d(new Private)
{
    setInternalPropertyValue("newObjectsAreDirty", true);

    // Only create form manager if it's not yet created.
    // KexiReportPart could have created it already.

/* 2.0 removed
    KFormDesigner::FormManager *formManager = KFormDesigner::FormManager::self();
    if (!formManager) {*/
    KexiFormManager::self()->init(this, d->widgetTree); // this should create KexiFormManager singleton
//    }

/* 2.0 slotPropertyChanged() code moved to Form so this connection can be removed
    connect(KFormDesigner::FormManager::self()->propertySet(),
            SIGNAL(widgetPropertyChanged(QWidget*,QByteArray,QVariant)),
            this, SLOT(slotPropertyChanged(QWidget*,QByteArray,QVariant)));*/
//2.0 not needed, the code from slot receiving this signal is moved to Form itself
//    connect(KFormDesigner::FormManager::self(), SIGNAL(autoTabStopsSet(KFormDesigner::Form*,bool)),
//            this, SLOT(slotAutoTabStopsSet(KFormDesigner::Form*,bool)));
}

KexiFormPart::~KexiFormPart()
{
//2.0    static_formsLibrary = 0;
    delete d;
}

/* moved to KexiFormManager
KFormDesigner::WidgetLibrary* KexiFormPart::library()
{
    return static_formsLibrary;
}
*/


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
}

void KexiFormPart::initInstanceActions()
{
//    KActionCollection *col = actionCollectionForMode(Kexi::DesignViewMode);

//moved to KexiFormManager::init()
//KexiFormManager::self()->createActions(library(), col,
//            (KXMLGUIClient*)col->parentGUIClient());

    //connect actions provided by widget factories
//2.0 moved   connect(col->action("widget_assign_action"), SIGNAL(activated()),
//2.0 moved            this, SLOT(slotAssignAction()));

    createSharedAction(Kexi::DesignViewMode, i18n("Clear Widget Contents"),
                       koIconName("edit-clear"), KShortcut(), "formpart_clear_contents");
    createSharedAction(Kexi::DesignViewMode, i18n("Edit Tab Order..."),
                       koIconName("tab_order"), KShortcut(), "formpart_taborder");
//TODO createSharedAction(Kexi::DesignViewMode, i18n("Edit Pixmap Collection"), koIconName("icons"), 0, "formpart_pixmap_collection");
//TODO createSharedAction(Kexi::DesignViewMode, i18n("Edit Form Connections"), koIconName("connections"), 0, "formpart_connections");

// KFormDesigner::CreateLayoutCommand

    KAction *action = createSharedAction(Kexi::DesignViewMode, i18n("Layout Widgets"),
                                         QString(), KShortcut(), "formpart_layout_menu", "KActionMenu");
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
    createSharedAction(Kexi::DesignViewMode, i18n("Bring Widget to Front"), koIconName("raise"),
                       KShortcut(), "formpart_format_raise");
    createSharedAction(Kexi::DesignViewMode, i18n("Send Widget to Back"), koIconName("lower"),
                       KShortcut(), "formpart_format_lower");

#ifndef KEXI_NO_UNFINISHED
    action = createSharedAction(Kexi::DesignViewMode, i18n("Other Widgets"), QString(),
                                KShortcut(), "other_widgets_menu", "KActionMenu");
#endif

    action = createSharedAction(Kexi::DesignViewMode, i18n("Align Widgets Position"),
                                koIconName("aoleft"), KShortcut(), "formpart_align_menu", "KActionMenu");
    menu = static_cast<KActionMenu*>(action);
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Left"), koIconName("aoleft"),
                                       KShortcut(), "formpart_align_to_left"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Right"), koIconName("aoright"),
                                       KShortcut(), "formpart_align_to_right"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Top"), koIconName("aotop"),
                                       KShortcut(), "formpart_align_to_top"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Bottom"), koIconName("aobottom"),
                                       KShortcut(), "formpart_align_to_bottom"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Grid"), koIconName("aopos2grid"),
                                       KShortcut(), "formpart_align_to_grid"));

    action = createSharedAction(Kexi::DesignViewMode, i18n("Adjust Widgets Size"), koIconName("aogrid"),
                                KShortcut(), "formpart_adjust_size_menu", "KActionMenu");
    menu = static_cast<KActionMenu*>(action);
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Fit"), koIconName("aofit"),
                                       KShortcut(), "formpart_adjust_to_fit"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Grid"), koIconName("aogrid"),
                                       KShortcut(), "formpart_adjust_size_grid"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Shortest"), koIconName("aoshortest"),
                                       KShortcut(), "formpart_adjust_height_small"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Tallest"), koIconName("aotallest"),
                                       KShortcut(), "formpart_adjust_height_big"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Narrowest"), koIconName("aonarrowest"),
                                       KShortcut(), "formpart_adjust_width_small"));
    menu->addAction(createSharedAction(Kexi::DesignViewMode, i18n("To Widest"), koIconName("aowidest"),
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

//2.0 not needed, the code from slot receiving this signal is moved to Form itself
/*void KexiFormPart::slotAutoTabStopsSet(KFormDesigner::Form *form, bool set)
{
    if (form->propertySet().contains("autoTabStops")) {
        form->propertySet().changeProperty("autoTabStops", set);
    }
}*/

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

/*moved to Form 
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
}*/

/*KFormDesigner::FormManager*
KexiFormPart::manager() const
{
  return d->manager;
}*/

KexiDataSourcePage* KexiFormPart::dataSourcePage() const
{
    return d->dataSourcePage;
}

KFormDesigner::WidgetTreeWidget* KexiFormPart::widgetTreePage() const
{
    return d->widgetTree;
}

void KexiFormPart::setupCustomPropertyPanelTabs(KTabWidget *tab)
{
    if (!d->dataSourcePage) {
        d->dataSourcePage = new KexiDataSourcePage(0);
        d->dataSourcePage->setObjectName("dataSourcePage");
        connect(d->dataSourcePage,
                SIGNAL(jumpToObjectRequested(QString,QString)),
                KexiMainWindowIface::global()->thisWidget(),
                SLOT(highlightObject(QString,QString)));
        connect(d->dataSourcePage,
                SIGNAL(formDataSourceChanged(QString,QString)),
                KexiFormManager::self(),
                SLOT(setFormDataSource(QString,QString)));
        connect(d->dataSourcePage,
                SIGNAL(dataSourceFieldOrExpressionChanged(QString,QString,KexiDB::Field::Type)),
                KexiFormManager::self(),
                SLOT(setDataSourceFieldOrExpression(QString,QString,KexiDB::Field::Type)));
        connect(d->dataSourcePage,
                SIGNAL(insertAutoFields(QString,QString,QStringList)),
                KexiFormManager::self(),
                SLOT(insertAutoFields(QString,QString,QStringList)));
    }

    KexiProject *prj = KexiMainWindowIface::global()->project();
    d->dataSourcePage->setProject(prj);

    tab->addTab(d->dataSourcePage, koIcon("server-database"), QString());
    tab->setTabToolTip(tab->indexOf(d->dataSourcePage), i18n("Data Source"));

    if (!d->widgetTreeWidget) {
        d->widgetTreeWidget = new QWidget;
        QVBoxLayout *lyr = new QVBoxLayout(d->widgetTreeWidget);
        lyr->setContentsMargins(2, 2, 2, 2);
        d->widgetTree = new KFormDesigner::WidgetTreeWidget;
        d->widgetTree->setObjectName("KexiFormPart:WidgetTreeWidget");
        lyr->addWidget(d->widgetTree);
    }
    tab->addTab(d->widgetTreeWidget, koIcon("widgets"), QString());
    tab->setTabToolTip(tab->indexOf(d->widgetTreeWidget), i18n("Widgets"));
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
