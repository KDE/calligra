/* This file is part of the KDE project
   Copyright (C) 2003 by Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2009-2014 Jarosław Staniek <staniek@kde.org>

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

#include "KexiStandardFormWidgetsFactory.h"
#include "KexiStandardFormWidgets.h"
#include "KexiStandardContainerFormWidgets.h"
#include "formIO.h"
#include "form.h"
#include "widgetlibrary.h"
#include "objecttree.h"
#include "WidgetInfo.h"
#include <kexiutils/utils.h>
#include <KexiIcon.h>
#include <kexi.h>

#include <KProperty>
#include <KPropertySet>

#include <KComboBox>
#include <KTextEdit>
#include <KLocalizedString>

#include <QStackedWidget>
#include <QLabel>
#include <QRadioButton>
#include <QCheckBox>
#include <QSlider>
#include <QDomDocument>
#include <QStyle>
#ifdef KEXI_LIST_FORM_WIDGET_SUPPORT
#include <QTreeWidget>
#endif
#include <QPixmap>
#include <QFrame>
#include <QList>
#include <QProgressBar>
#include <QTimeEdit>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QAction>
#include <QSpinBox>
#include <QDebug>

KEXI_PLUGIN_FACTORY(KexiStandardFormWidgetsFactory, "kexiforms_standardwidgetsplugin.json")

KexiStandardFormWidgetsFactory::KexiStandardFormWidgetsFactory(QObject *parent, const QVariantList &args)
        : KFormDesigner::WidgetFactory(parent)
{
    Q_UNUSED(args);
    KFormDesigner::WidgetInfo *wFormWidget = new KFormDesigner::WidgetInfo(this);
    wFormWidget->setIconName(koIconName("form"));
    wFormWidget->setClassName("FormWidgetBase");
    wFormWidget->setName(xi18n("Form"));
    wFormWidget->setNamePrefix(
        xi18nc("A prefix for identifiers of form widgets. Based on that, identifiers such as "
            "form1, form2 are generated. "
            "This string can be used to refer the widget object as variables in programming "
            "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
            "should start with lower case letter and if there are subsequent words, these should "
            "start with upper case letter. Example: smallCamelCase. "
            "Moreover, try to make this prefix as short as possible.",
            "form"));
    wFormWidget->setDescription(xi18n("A simple form widget"));
    addClass(wFormWidget);

    KFormDesigner::WidgetInfo *wCustomWidget = new KFormDesigner::WidgetInfo(this);
    wCustomWidget->setIconName(koIconName("unknown_widget"));
    wCustomWidget->setClassName("CustomWidget");
    wCustomWidget->setName(/* no i18n needed */ "Custom Widget");
    wCustomWidget->setNamePrefix(/* no i18n needed */ "customWidget");
    wCustomWidget->setDescription(/* no i18n needed */ "A custom or non-supported widget");
    addClass(wCustomWidget);

    KFormDesigner::WidgetInfo *wLabel = new KFormDesigner::WidgetInfo(this);
    wLabel->setIconName(koIconName("label"));
    wLabel->setClassName("QLabel");
    wLabel->setName(/* no i18n needed */ "Text Label");
    wLabel->setNamePrefix(/* no i18n needed */ "label");
    wLabel->setDescription(/* no i18n needed */ "A widget to display text");
    wLabel->setAutoSaveProperties(QList<QByteArray>() << "text");
    addClass(wLabel);

    KFormDesigner::WidgetInfo *wPixLabel = new KFormDesigner::WidgetInfo(this);
    wPixLabel->setIconName(koIconName("pixmaplabel"));
    wPixLabel->setClassName("KexiPictureLabel");
    wPixLabel->setName(/* no i18n needed */ "Picture Label");
//! @todo Qt designer compatibility: maybe use this class when QLabel has a pixmap set...?
    wPixLabel->setSavingName("KexiPictureLabel");
    wPixLabel->setNamePrefix(/* no i18n needed */ "picture");
    wPixLabel->setDescription(/* no i18n needed */ "A widget to display pictures");
    wPixLabel->setAutoSaveProperties(QList<QByteArray>() << "pixmap");
    addClass(wPixLabel);

    KFormDesigner::WidgetInfo *wLineEdit = new KFormDesigner::WidgetInfo(this);
    wLineEdit->setIconName(koIconName("lineedit"));
    wLineEdit->setClassName("QLineEdit");
    wLineEdit->addAlternateClassName("KLineEdit");
    wLineEdit->setIncludeFileName("qlineedit.h");
    wLineEdit->setName(/* no i18n needed */ "Line Edit");
    wLineEdit->setNamePrefix(/* no i18n needed */ "lineEdit");
    wLineEdit->setDescription(/* no i18n needed */ "A widget to input text");
    addClass(wLineEdit);

    KFormDesigner::WidgetInfo *wPushButton = new KFormDesigner::WidgetInfo(this);
    wPushButton->setIconName(koIconName("button"));
    wPushButton->setClassName("QPushButton");
    wPushButton->addAlternateClassName("KPushButton");
    wPushButton->setIncludeFileName("qpushbutton.h");
    wPushButton->setName(/* no i18n needed */ "Push Button");
    wPushButton->setNamePrefix(/* no i18n needed */ "button");
    wPushButton->setDescription(/* no i18n needed */ "A simple push button to execute actions");
    wPushButton->setAutoSaveProperties(QList<QByteArray>() << "text");
    addClass(wPushButton);

    KFormDesigner::WidgetInfo *wRadioButton = new KFormDesigner::WidgetInfo(this);
    wRadioButton->setIconName(koIconName("radio"));
    wRadioButton->setClassName("QRadioButton");
    wRadioButton->setName(/* no i18n needed */ "Option Button");
    wRadioButton->setNamePrefix(/* no i18n needed */ "option");
    wRadioButton->setDescription(/* no i18n needed */ "An option button with text or pixmap label");
    addClass(wRadioButton);

    KFormDesigner::WidgetInfo *wCheckBox = new KFormDesigner::WidgetInfo(this);
    wCheckBox->setIconName(koIconName("check"));
    wCheckBox->setClassName("QCheckBox");
    wCheckBox->setName(/* no i18n needed */ "Check Box");
    wCheckBox->setNamePrefix(/* no i18n needed */ "checkBox");
    wCheckBox->setDescription(/* no i18n needed */ "A check box with text or pixmap label");
    addClass(wCheckBox);

    KFormDesigner::WidgetInfo *wSpinBox = new KFormDesigner::WidgetInfo(this);
    wSpinBox->setIconName(koIconName("spin"));
    wSpinBox->setClassName("QSpinBox");
    wSpinBox->addAlternateClassName("KIntSpinBox");
    wSpinBox->setIncludeFileName("qspinbox.h");
    wSpinBox->setName(/* no i18n needed */ "Spin Box");
    wSpinBox->setNamePrefix(/* no i18n needed */ "spinBox");
    wSpinBox->setDescription(/* no i18n needed */ "A spin box widget");
    addClass(wSpinBox);

    KFormDesigner::WidgetInfo *wComboBox = new KFormDesigner::WidgetInfo(this);
    wComboBox->setIconName(koIconName("combo"));
    wComboBox->setClassName("KComboBox");
    wComboBox->addAlternateClassName("QComboBox");
    wComboBox->setIncludeFileName("KComboBox");
    wComboBox->setName(/* no i18n needed */ "Combo Box");
    wComboBox->setNamePrefix(/* no i18n needed */ "comboBox");
    wComboBox->setDescription(/* no i18n needed */ "A combo box widget");
    wComboBox->setAutoSaveProperties(QList<QByteArray>() << "list_items");
    addClass(wComboBox);

#ifdef KEXI_LIST_FORM_WIDGET_SUPPORT
// Unused, commented-out in Kexi 2.9 to avoid unnecessary translations:
//     KFormDesigner::WidgetInfo *wListBox = new KFormDesigner::WidgetInfo(this);
//     wListBox->setIconName(koIconName("listbox"));
//     wListBox->setClassName("KListBox");
//     wListBox->addAlternateClassName("QListBox");
//     wListBox->addAlternateClassName("KListBox");
//     wListBox->setIncludeFileName("qlistbox.h");
//     wListBox->setName(xi18n("List Box"));
//     wListBox->setNamePrefix(
//         xi18nc("Widget name. This string will be used to name widgets of this class. "
//    "It must _not_ contain white spaces and non latin1 characters.", "listBox"));
//     wListBox->setDescription(xi18n("A simple list widget"));
//     wListBox->setAutoSaveProperties(QList<QByteArray>() << "list_items");
//     addClass(wListBox);

// Unused, commented-out in Kexi 2.9 to avoid unnecessary translations:
//     KFormDesigner::WidgetInfo *wTreeWidget = new KFormDesigner::WidgetInfo(this);
//     wTreeWidget->setIconName(koIconName("listwidget"));
//     wTreeWidget->setClassName("QTreetWidget");
// //?    wTreeWidget->addAlternateClassName("QListView");
// //?    wTreeWidget->addAlternateClassName("KListView");
//     wTreeWidget->setIncludeFileName("qtreewidget.h");
//     wTreeWidget->setName(xi18n("List Widget"));
//     wTreeWidget->setNamePrefix(
//         xi18nc("Widget name. This string will be used to name widgets of this class. "
//    "It must _not_ contain white spaces and non latin1 characters.", "listWidget"));
//     wTreeWidget->setDescription(xi18n("A list (or tree) widget"));
//     wTreeWidget->setAutoSaveProperties(QList<QByteArray>() << "list_contents");
//     addClass(wTreeWidget);
#endif

    KFormDesigner::WidgetInfo *wTextEdit = new KFormDesigner::WidgetInfo(this);
    wTextEdit->setIconName(koIconName("textedit"));
    wTextEdit->setClassName("KTextEdit");
    wTextEdit->addAlternateClassName("QTextEdit");
    wTextEdit->setIncludeFileName("KTextEdit");
    wTextEdit->setName(/* no i18n needed */ "Text Editor");
    wTextEdit->setNamePrefix(/* no i18n needed */ "textEditor");
    wTextEdit->setDescription(/* no i18n needed */ "A simple single-page rich text editor");
    wTextEdit->setAutoSaveProperties(QList<QByteArray>() << "text");
    addClass(wTextEdit);

    KFormDesigner::WidgetInfo *wSlider = new KFormDesigner::WidgetInfo(this);
    wSlider->setIconName(koIconName("slider"));
    wSlider->setClassName("QSlider");
    wSlider->setName(/* no i18n needed */ "Slider");
    wSlider->setNamePrefix(/* no i18n needed */ "slider");
    wSlider->setDescription(/* no i18n needed */ "A Slider widget");
    addClass(wSlider);

    KFormDesigner::WidgetInfo *wProgressBar = new KFormDesigner::WidgetInfo(this);
    wProgressBar->setIconName(koIconName("progress"));
    wProgressBar->setClassName("QProgressBar");
    wProgressBar->setName(/* no i18n needed */ "Progress Bar");
    wProgressBar->setNamePrefix(/* no i18n needed */ "progressBar");
    wProgressBar->setDescription(/* no i18n needed */ "A progress indicator widget");
    addClass(wProgressBar);

    KFormDesigner::WidgetInfo *wLine = new KFormDesigner::WidgetInfo(this);
    wLine->setIconName(koIconName("line"));
    wLine->setClassName("Line");
    wLine->setName(xi18n("Line"));
    wLine->setNamePrefix(
        xi18nc("A prefix for identifiers of line widgets. Based on that, identifiers such as "
            "line1, line2 are generated. "
            "This string can be used to refer the widget object as variables in programming "
            "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
            "should start with lower case letter and if there are subsequent words, these should "
            "start with upper case letter. Example: smallCamelCase. "
            "Moreover, try to make this prefix as short as possible.",
            "line"));
    wLine->setDescription(xi18n("A line to be used as a separator"));
    wLine->setAutoSaveProperties(QList<QByteArray>() << "orientation");
    wLine->setInternalProperty("orientationSelectionPopup", true);
    wLine->setInternalProperty("orientationSelectionPopup:horizontalIcon", koIconName("line_horizontal"));
    wLine->setInternalProperty("orientationSelectionPopup:verticalIcon", koIconName("line_vertical"));
    wLine->setInternalProperty("orientationSelectionPopup:horizontalText", xi18n("Insert &Horizontal Line"));
    wLine->setInternalProperty("orientationSelectionPopup:verticalText", xi18n("Insert &Vertical Line"));
    addClass(wLine);

    KFormDesigner::WidgetInfo *wDate = new KFormDesigner::WidgetInfo(this);
    wDate->setIconName(koIconName("dateedit"));
    wDate->setClassName("QDateEdit");
    wDate->addAlternateClassName("KDateWidget");
    wDate->setIncludeFileName("qdateedit.h");
    wDate->setName(/* no i18n needed */ "Date Widget");
    wDate->setNamePrefix(/* no i18n needed */ "dateWidget");
    wDate->setDescription(/* no i18n needed */ "A widget to input and display a date");
    wDate->setAutoSaveProperties(QList<QByteArray>() << "date");
    addClass(wDate);

    KFormDesigner::WidgetInfo *wTime = new KFormDesigner::WidgetInfo(this);
    wTime->setIconName(koIconName("timeedit"));
    wTime->setClassName("QTimeEdit");
    wTime->addAlternateClassName("KTimeWidget");
    wTime->setIncludeFileName("qtimewidget.h");
    wTime->setName(/* no i18n needed */ "Time Widget");
    wTime->setNamePrefix(/* no i18n needed */ "timeWidget");
    wTime->setDescription(/* no i18n needed */ "A widget to input and display a time");
    wTime->setAutoSaveProperties(QList<QByteArray>() << "time");
    addClass(wTime);

    KFormDesigner::WidgetInfo *wDateTime = new KFormDesigner::WidgetInfo(this);
    wDateTime->setIconName(koIconName("datetimeedit"));
    wDateTime->setClassName("QDateTimeEdit");
    wDateTime->addAlternateClassName("KDateTimeWidget");
    wDateTime->setIncludeFileName("qdatetimewidget.h");
    wDateTime->setName(/* no i18n needed */ "Date/Time Widget");
    wDateTime->setNamePrefix(/* no i18n needed */ "dateTimeWidget");
    wDateTime->setDescription(/* no i18n needed */ "A widget to input and display a time and a date");
    wDateTime->setAutoSaveProperties(QList<QByteArray>() << "dateTime");
    addClass(wDateTime);

    setPropertyDescription("checkable", xi18nc("Property: Button is checkable", "On/Off"));
    setPropertyDescription("autoRepeat", xi18nc("Property: Button", "Auto Repeat"));
    setPropertyDescription("autoRepeatDelay", xi18nc("Property: Auto Repeat Button's Delay", "Auto Rep. Delay"));
    setPropertyDescription("autoRepeatInterval", xi18nc("Property: Auto Repeat Button's Interval", "Auto Rep. Interval"));
    // unused (too advanced) setPropertyDescription("autoDefault", xi18n("Auto Default"));
    // unused (too advanced) setPropertyDescription("default", xi18nc("Property: Button is default", "Default"));
    setPropertyDescription("flat", xi18nc("Property: Button is flat", "Flat"));
    setPropertyDescription("echoMode",
        xi18nc("Property: Echo mode for Line Edit widget eg. Normal, NoEcho, Password", "Echo Mode"));
    setPropertyDescription("indent", xi18n("Indent"));
    //line
    setPropertyDescription("orientation", xi18n("Orientation"));
    //checkbox
    setPropertyDescription("checked", xi18nc("Property: Checked checkbox", "Checked"));
    setPropertyDescription("tristate", xi18nc("Property: Tristate checkbox", "Tristate"));

    //for labels
    setPropertyDescription("textFormat", xi18n("Text Format"));
    setValueDescription("PlainText", xi18nc("For Text Format", "Plain"));
    setValueDescription("RichText", xi18nc("For Text Format", "Hypertext"));
    setValueDescription("AutoText", xi18nc("For Text Format", "Auto"));
    setValueDescription("LogText", xi18nc("For Text Format", "Log"));
    setPropertyDescription("openExternalLinks", xi18nc("property: Can open external links in label", "Open Ext. Links"));

    //QLineEdit
    setPropertyDescription("placeholderText", xi18nc("Property: line edit's placeholder text", "Placeholder Text"));
    setPropertyDescription("clearButtonEnabled", xi18nc("Property: Clear Button Enabled", "Clear Button"));
    //for EchoMode
    setPropertyDescription("passwordMode", xi18nc("Property: Password Mode for line edit", "Password Mode"));
    setPropertyDescription("squeezedTextEnabled", xi18nc("Property: Squeezed Text Mode for line edit", "Squeezed Text"));

    //KTextEdit
    setPropertyDescription("tabStopWidth", xi18n("Tab Stop Width"));
    setPropertyDescription("tabChangesFocus", xi18n("Tab Changes Focus"));
    setPropertyDescription("wrapPolicy", xi18n("Word Wrap Policy"));
    setValueDescription("AtWordBoundary", xi18nc("Property: For Word Wrap Policy", "At Word Boundary"));
    setValueDescription("Anywhere", xi18nc("Property: For Word Wrap Policy", "Anywhere"));
    setValueDescription("AtWordOrDocumentBoundary", xi18nc("Property: For Word Wrap Policy", "At Word Boundary If Possible"));
    setPropertyDescription("wordWrap", xi18n("Word Wrapping"));
    setPropertyDescription("wrapColumnOrWidth", xi18n("Word Wrap Position"));
    setValueDescription("NoWrap", xi18nc("Property: For Word Wrap Position", "None"));
    setValueDescription("WidgetWidth", xi18nc("Property: For Word Wrap Position", "Widget's Width"));
    setValueDescription("FixedPixelWidth", xi18nc("Property: For Word Wrap Position", "In Pixels"));
    setValueDescription("FixedColumnWidth", xi18nc("Property: For Word Wrap Position", "In Columns"));
    setPropertyDescription("linkUnderline", xi18n("Links Underlined"));
    setPropertyDescription("horizontalScrollBarPolicy", xi18n("Horizontal Scroll Bar"));
    setPropertyDescription("verticalScrollBarPolicy", xi18n("Vertical Scroll Bar"));
    //ScrollBarPolicy
    setValueDescription("ScrollBarAsNeeded", xi18nc("Property: Show Scroll Bar As Needed", "As Needed"));
    setValueDescription("ScrollBarAlwaysOff", xi18nc("Property: Scroll Bar Always Off", "Always Off"));
    setValueDescription("ScrollBarAlwaysOn", xi18nc("Property: Scroll Bar Always On", "Always On"));
    setPropertyDescription("acceptRichText", xi18nc("Property: Text Edit accepts rich text", "Rich Text"));
    setPropertyDescription("HTML", xi18nc("Property: HTML value of text edit", "HTML"));

    // --- containers ---

    KFormDesigner::WidgetInfo *wTabWidget = new KFormDesigner::WidgetInfo(this);
    wTabWidget->setIconName(koIconName("tabwidget"));
    wTabWidget->setClassName("KFDTabWidget");
    wTabWidget->addAlternateClassName("KTabWidget");
    wTabWidget->addAlternateClassName("QTabWidget");
    wTabWidget->setSavingName("QTabWidget");
    wTabWidget->setIncludeFileName("qtabwidget.h");
    wTabWidget->setName(xi18n("Tab Widget"));
    wTabWidget->setNamePrefix(
        xi18nc("A prefix for identifiers of tab widgets. Based on that, identifiers such as "
              "tab1, tab2 are generated. "
              "This string can be used to refer the widget object as variables in programming "
              "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
              "should start with lower case letter and if there are subsequent words, these should "
              "start with upper case letter. Example: smallCamelCase. "
              "Moreover, try to make this prefix as short as possible.",
              "tabWidget"));
    wTabWidget->setDescription(xi18n("A widget to display multiple pages using tabs"));
    addClass(wTabWidget);

    KFormDesigner::WidgetInfo *wWidget = new KFormDesigner::WidgetInfo(this);
    wWidget->setIconName(koIconName("frame"));
    wWidget->setClassName("QWidget");
    wWidget->addAlternateClassName("ContainerWidget");
    wWidget->setName(/* no i18n needed */ "Basic container");
    wWidget->setNamePrefix(/* no i18n needed */ "container");
    wWidget->setDescription(/* no i18n needed */ "An empty container with no frame");
    addClass(wWidget);

    KFormDesigner::WidgetInfo *wGroupBox = new KFormDesigner::WidgetInfo(this);
    wGroupBox->setIconName(koIconName("groupbox"));
    wGroupBox->setClassName("QGroupBox");
    wGroupBox->addAlternateClassName("GroupBox");
    wGroupBox->setName(xi18n("Group Box"));
    wGroupBox->setNamePrefix(
        xi18nc("A prefix for identifiers of group box widgets. Based on that, identifiers such as "
              "groupBox1, groupBox2 are generated. "
              "This string can be used to refer the widget object as variables in programming "
              "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
              "should start with lower case letter and if there are subsequent words, these should "
              "start with upper case letter. Example: smallCamelCase. "
              "Moreover, try to make this prefix as short as possible.",
              "groupBox"));
    wGroupBox->setDescription(xi18n("A container to group some widgets"));
    addClass(wGroupBox);

    KFormDesigner::WidgetInfo *wFrame = new KFormDesigner::WidgetInfo(this);
    wFrame->setIconName(koIconName("frame"));
    wFrame->setClassName("QFrame");
    wFrame->setName(/* no i18n needed */ "Frame");
    wFrame->setNamePrefix(/* no i18n needed */ "frame");
    wFrame->setDescription(/* no i18n needed */ "A simple frame container");
    addClass(wFrame);

    //groupbox
    setPropertyDescription("title", xi18nc("'Title' property for group box", "Title"));
    setPropertyDescription("flat", xi18nc("'Flat' property for group box", "Flat"));

    //tab widget
    setPropertyDescription("tabBarAutoHide", xi18n("Auto-hide Tabs"));
    setPropertyDescription("tabPosition", xi18n("Tab Position"));
    setPropertyDescription("currentIndex", xi18nc("'Current page' property for tab widget", "Current Page"));
    setPropertyDescription("tabShape", xi18n("Tab Shape"));
    setPropertyDescription("elideMode", xi18nc("Tab Widget's Elide Mode property", "Elide Mode"));
    setPropertyDescription("usesScrollButtons",
                           xi18nc("Tab Widget's property: true if can use scroll buttons", "Scroll Buttons"));

    setPropertyDescription("tabsClosable", xi18n("Closable Tabs"));
    setPropertyDescription("movable", xi18n("Movable Tabs"));
    setPropertyDescription("documentMode", xi18n("Document Mode"));

    setValueDescription("Rounded", xi18nc("Property value for Tab Shape", "Rounded"));
    setValueDescription("Triangular", xi18nc("Property value for Tab Shape", "Triangular"));
}

KexiStandardFormWidgetsFactory::~KexiStandardFormWidgetsFactory()
{
}

QWidget* KexiStandardFormWidgetsFactory::createWidget(const QByteArray &c, QWidget *p, const char *n,
                                                      KFormDesigner::Container *container,
                                                      CreateWidgetOptions options)
{
    QWidget *w = 0;
    bool createContainer = false;
    QString text(container->form()->library()->textForWidgetName(n, c));
    if (c == "QLabel") {
        w = new QLabel(text, p);
    } else if (c == "KexiPictureLabel") {
        w = new KexiPictureLabel(koDesktopIcon("image-x-generic"), p);
    } else if (c == "QLineEdit") {
        w = new QLineEdit(p);
    } else if (c == "QPushButton") {
        w = new QPushButton(text, p);
    } else if (c == "QRadioButton") {
        w = new QRadioButton(text, p);
    } else if (c == "QCheckBox") {
        w = new QCheckBox(text, p);
    } else if (c == "KIntSpinBox") {
        w = new QSpinBox(p);
    } else if (c == "KComboBox") {
        w = new KComboBox(p);
    } else if (c == "KTextEdit") {
        w = new KTextEdit(text, p);
#ifdef KEXI_LIST_FORM_WIDGET_SUPPORT
    } else if (c == "QTreeWidget") {
        QTreeWidget *tw = new QTreeWidget(p);
        w = tw;
        if (container->form()->interactiveMode()) {
            tw->setColumnCount(1);
            tw->setHeaderItem(new QTreeWidetItem(tw));
            tw->headerItem()->setText(1, futureI18n("Column 1"));
        }
        lw->setRootIsDecorated(true);
#endif
    } else if (c == "QSlider") {
        w = new QSlider(Qt::Horizontal, p);
    } else if (c == "QProgressBar") {
        w = new QProgressBar(p);
    } else if (c == "KDateWidget" || c == "QDateEdit") {
        w = new QDateEdit(QDate::currentDate(), p);
    } else if (c == "KTimeWidget" || c == "QTimeEdit") {
        w = new QTimeEdit(QTime::currentTime(), p);
    } else if (c == "KDateTimeWidget" || c == "QDateTimeEdit") {
        w = new QDateTimeEdit(QDateTime::currentDateTime(), p);
    } else if (c == "Line") {
        w = new Line(options & WidgetFactory::VerticalOrientation
                     ? Qt::Vertical : Qt::Horizontal, p);
    } // --- containers ---
    else if (c == "KFDTabWidget") {
        KFDTabWidget *tab = new KFDTabWidget(container, p);
        w = tab;
#if defined(USE_KTabWidget)
        tab->setTabReorderingEnabled(true);
        connect(tab, SIGNAL(movedTab(int,int)), this, SLOT(reorderTabs(int,int)));
#endif
        qDebug() << "Creating ObjectTreeItem:";
        container->form()->objectTree()->addItem(container->objectTree(),
                new KFormDesigner::ObjectTreeItem(
                    container->form()->library()->displayName(c), n, tab, container));
    } else if (c == "QWidget") {
        w = new ContainerWidget(p);
        w->setObjectName(n);
        new KFormDesigner::Container(container, w, p);
        return w;
    } else if (c == "QGroupBox") {
        QString text = container->form()->library()->textForWidgetName(n, c);
        w = new GroupBox(text, p);
        createContainer = true;
    } else if (c == "QFrame") {
        QFrame *frm = new QFrame(p);
        w = frm;
        frm->setLineWidth(2);
        frm->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
        createContainer = true;
    } else if (c == "QStackedWidget" || /* compat */ c == "QWidgetStack") {
        QStackedWidget *stack = new QStackedWidget(p);
        w = stack;
        stack->setLineWidth(2);
        stack->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
        qDebug() << "Creating ObjectTreeItem:";
        container->form()->objectTree()->addItem(container->objectTree(),
                new KFormDesigner::ObjectTreeItem(
                    container->form()->library()->displayName(c), n, stack, container));

        if (container->form()->interactiveMode()) {
            AddStackPageAction(container, stack, 0).trigger(); // addStackPage();
        }
    } else if (c == "HBox") {
        w = new HBox(p);
        createContainer = true;
    } else if (c == "VBox") {
        w = new VBox(p);
        createContainer = true;
    } else if (c == "Grid") {
        w = new Grid(p);
        createContainer = true;
    } else if (c == "HFlow") {
        w = new HFlow(p);
        createContainer = true;
    } else if (c == "VFlow") {
        w = new VFlow(p);
        createContainer = true;
    }

    if (w) {
        w->setObjectName(n);
        qDebug() << w << w->objectName() << "created";
    }
    if (createContainer) {
        (void)new KFormDesigner::Container(container, w, container);
    }
    if (c == "KFDTabWidget") {
        // if we are loading, don't add this tab
        if (container->form()->interactiveMode()) {
            TabWidgetBase *tab = qobject_cast<TabWidgetBase*>(w);
            AddTabAction(container, tab, 0).slotTriggered();
        }
    }
    return w;
}

bool KexiStandardFormWidgetsFactory::previewWidget(const QByteArray &classname,
                                QWidget *widget, KFormDesigner::Container *container)
{
    if (classname == "QStackedWidget" || /* compat */ classname == "QWidgetStack") {
        QStackedWidget *stack = qobject_cast<QStackedWidget*>(widget);
        KFormDesigner::ObjectTreeItem *tree = container->form()->objectTree()->lookup(
            widget->objectName());
        if (!tree->modifiedProperties()->contains("frameShape"))
            stack->setFrameStyle(QFrame::NoFrame);
    }
    return true;
}

bool KexiStandardFormWidgetsFactory::createMenuActions(const QByteArray &classname, QWidget *w,
                                                       QMenu *menu, KFormDesigner::Container *container)
{
    QWidget *pw = w->parentWidget();
    if ((classname == "QLabel") || (classname == "KTextEdit")) {
        menu->addAction( new EditRichTextAction(container, w, menu, this) );
        return true;
    }
#ifdef KEXI_LIST_FORM_WIDGET_SUPPORT
    else if (classname == "QTreeWidget") {
        menu->addAction(koIcon("document-properties"), xi18n("Edit Contents of List Widget"),
            this, SLOT(editListContents()));
        return true;
    }
#endif
    else if (classname == "KFDTabWidget" || pw->parentWidget()->inherits("QTabWidget")) {
//! @todo KEXI3 port this: setWidget(pw->parentWidget(), m_container->toplevel());
#if 0
        if (pw->parentWidget()->inherits("QTabWidget")) {
            setWidget(pw->parentWidget(), m_container->toplevel());
        }
#endif

        TabWidgetBase *tab = qobject_cast<TabWidgetBase*>(w);
        if (tab) {
            menu->addAction( new AddTabAction(container, tab, menu) );
            menu->addAction( new RenameTabAction(container, tab, menu) );
            menu->addAction( new RemoveTabAction(container, tab, menu) );
        }
        return true;
    }
    else if (    (KexiUtils::objectIsA(pw, "QStackedWidget") || /* compat */ KexiUtils::objectIsA(pw, "QWidgetStack"))
              && !pw->parentWidget()->inherits("QTabWidget")
            )
    {
        QStackedWidget *stack = qobject_cast<QStackedWidget*>(pw);
//! @todo KEXI3 port this: setWidget( pw, container->form()->objectTree()->lookup(stack->objectName())->parent()->container() );
#if 0
        setWidget(
            pw,
            container->form()->objectTree()->lookup(stack->objectName())->parent()->container()
        );
#endif
        KFormDesigner::Container *parentContainer
            = container->form()->objectTree()->lookup(stack->objectName())->parent()->container();
        menu->addAction( new AddStackPageAction(parentContainer, pw, menu) );
        menu->addAction( new RemoveStackPageAction(parentContainer, pw, menu) );
        menu->addAction( new GoToStackPageAction(GoToStackPageAction::Previous, parentContainer, pw, menu) );
        menu->addAction( new GoToStackPageAction(GoToStackPageAction::Next, parentContainer, pw, menu) );
        return true;
    }
    return false;
}

bool KexiStandardFormWidgetsFactory::startInlineEditing(InlineEditorCreationArguments& args)
{
    if (args.classname == "QLineEdit") {
        QLineEdit *lineedit = static_cast<QLineEdit*>(args.widget);
        args.text = lineedit->text();
        args.alignment = lineedit->alignment();
        args.useFrame = true;
        return true;
    }
    else if (args.widget->inherits("QLabel")) {
        QLabel *label = static_cast<QLabel*>(args.widget);
        if (label->textFormat() == Qt::RichText) {
            args.execute = false;
            EditRichTextAction(args.container, label, 0, this).trigger();
//! @todo
        } else {
            args.text = label->text();
            args.alignment = label->alignment();
        }
        return true;
    }
    else if (args.classname == "QPushButton") {
        QPushButton *push = static_cast<QPushButton*>(args.widget);
        QStyleOption option;
        option.initFrom(push);
        args.text = push->text();
        const QRect r(push->style()->subElementRect(
                          QStyle::SE_PushButtonContents, &option, push));
        args.geometry = QRect(push->x() + r.x(), push->y() + r.y(), r.width(), r.height());
//! @todo this is typical alignment, can we get actual from the style?
        args.alignment = Qt::AlignCenter;
        args.transparentBackground = true;
        return true;
    }
    else if (args.classname == "QRadioButton") {
        QRadioButton *radio = static_cast<QRadioButton*>(args.widget);
        QStyleOption option;
        option.initFrom(radio);
        args.text = radio->text();
        const QRect r(radio->style()->subElementRect(
                          QStyle::SE_RadioButtonContents, &option, radio));
        args.geometry = QRect(
            radio->x() + r.x(), radio->y() + r.y(), r.width(), r.height());
        return true;
    }
    else if (args.classname == "QCheckBox") {
        QCheckBox *check = static_cast<QCheckBox*>(args.widget);
        QStyleOption option;
        option.initFrom(check);
        const QRect r(args.widget->style()->subElementRect(
                          QStyle::SE_CheckBoxContents, &option, check));
        args.geometry = QRect(
            check->x() + r.x(), check->y() + r.y(), r.width(), r.height());
        return true;
    } else if (args.classname == "KComboBox" || args.classname == "QComboBox") {
        QStringList list;
        KComboBox *combo = qobject_cast<KComboBox*>(args.widget);
        for (int i = 0; i < combo->count(); i++) {
            list.append(combo->itemText(i));
        }
        args.execute = false;
        if (editList(args.widget, list)) {
            qobject_cast<KComboBox*>(args.widget)->clear();
            qobject_cast<KComboBox*>(args.widget)->addItems(list);
        }
        return true;
    }
    else if (   args.classname == "KTextEdit" || args.classname == "KDateTimeWidget"
             || args.classname == "KTimeWidget" || args.classname == "KDateWidget"
             || args.classname == "KIntSpinBox")
    {
        args.execute = false;
        disableFilter(args.widget, args.container);
        return true;
    }
    return false;
}

bool KexiStandardFormWidgetsFactory::clearWidgetContent(const QByteArray &classname, QWidget *w)
{
    if (classname == "QLineEdit")
        qobject_cast<QLineEdit*>(w)->clear();
#ifdef KEXI_LIST_FORM_WIDGET_SUPPORT
    else if (classname == "QTreeWidget")
        qobject_cast<QTreeWidget*>(w)->clear();
#endif
    else if (classname == "KComboBox")
        qobject_cast<KComboBox*>(w)->clear();
    else if (classname == "KTextEdit")
        qobject_cast<KTextEdit*>(w)->clear();
    else
        return false;
    return true;
}

bool KexiStandardFormWidgetsFactory::changeInlineText(KFormDesigner::Form *form, QWidget *widget,
                                                      const QString &text, QString &oldText)
{
    const QByteArray n(widget->metaObject()->className());
    if (n == "KIntSpinBox") {
        oldText = QString::number(qobject_cast<QSpinBox*>(widget)->value());
        qobject_cast<QSpinBox*>(widget)->setValue(text.toInt());
    }
    else {
        oldText = widget->property("text").toString();
        changeProperty(form, widget, "text", text);
    }
    return true;
}

void KexiStandardFormWidgetsFactory::resizeEditor(QWidget *editor, QWidget *widget,
                                                  const QByteArray &classname)
{
    QSize s = widget->size();
    QPoint p = widget->pos();
    QRect r;

    if (classname == "QRadioButton") {
        QStyleOption option;
        option.initFrom(widget);
        r = widget->style()->subElementRect(
                QStyle::SE_RadioButtonContents, &option, widget);
        p += r.topLeft();
        s.setWidth(r.width());
    } else if (classname == "QCheckBox") {
        QStyleOption option;
        option.initFrom(widget);
        r = widget->style()->subElementRect(
                QStyle::SE_CheckBoxContents, &option, widget);
        p += r.topLeft();
        s.setWidth(r.width());
    } else if (classname == "QPushButton") {
        QStyleOption option;
        option.initFrom(widget);
        r = widget->style()->subElementRect(
                QStyle::SE_PushButtonContents, &option, widget);
        p += r.topLeft();
        s = r.size();
    }

    editor->resize(s);
    editor->move(p);

    //! @todo KEXI3
    /* from ContainerFactory::resizeEditor(QWidget *editor, QWidget *widget, const QByteArray &):
        QSize s = widget->size();
        editor->move(widget->x() + 2, widget->y() - 5);
        editor->resize(s.width() - 20, widget->fontMetrics().height() + 10); */
}

bool KexiStandardFormWidgetsFactory::saveSpecialProperty(const QByteArray &classname,
                                      const QString &name, const QVariant &,
                                      QWidget *w, QDomElement &parentNode, QDomDocument &domDoc)
{
    if (name == "list_items" && classname == "KComboBox") {
        KComboBox *combo = qobject_cast<KComboBox*>(w);
        for (int i = 0; i < combo->count(); i++) {
            QDomElement item = domDoc.createElement("item");
            KFormDesigner::FormIO::savePropertyElement(item, domDoc, "property", "text", combo->itemText(i));
            parentNode.appendChild(item);
        }
        return true;
    }
#ifdef KEXI_LIST_FORM_WIDGET_SUPPORT
    else if (name == "list_contents" && classname == "QTreeWidget") {
        QTreeWidget *treewidget = qobject_cast<QTreeWidget*>(w);
        // First we save the columns
        QTreeWidgetItem *headerItem = treewidget->headerItem();
        if (headerItem) {
            for (int i = 0; i < treewidget->columnCount(); i++) {
                QDomElement item = domDoc.createElement("column");
                KFormDesigner::FormIO::savePropertyElement(
                    item, domDoc, "property", "text", headerItem->text(i));
                KFormDesigner::FormIO::savePropertyElement(
                    item, domDoc, "property", "width", treewidget->columnWidth(i));
                KFormDesigner::FormIO::savePropertyElement(
                    item, domDoc, "property", "resizable", treewidget->header()->isResizeEnabled(i));
                KFormDesigner::FormIO::savePropertyElement(
                    item, domDoc, "property", "clickable", treewidget->header()->isClickEnabled(i));
                KFormDesigner::FormIO::savePropertyElement(
                    item, domDoc, "property", "fullwidth", treewidget->header()->isStretchEnabled(i));
                parentNode.appendChild(item);
            }
        }
        // Then we save the list view items
        QTreeWidgetItem *item = listwidget->firstChild();
        while (item) {
            saveListItem(item, parentNode, domDoc);
            item = item->nextSibling();
        }
        return true;
    }
#endif
    else if ((name == "title") && (w->parentWidget()->parentWidget()->inherits("QTabWidget"))) {
        TabWidgetBase *tab = qobject_cast<TabWidgetBase*>(w->parentWidget()->parentWidget());
        KFormDesigner::FormIO::savePropertyElement(
            parentNode, domDoc, "attribute", "title", tab->tabText(tab->indexOf(w)));
    } else if ((name == "stackIndex")
        && (KexiUtils::objectIsA(w->parentWidget(), "QStackedWidget")
            || /*compat*/ KexiUtils::objectIsA(w->parentWidget(), "QWidgetStack")))
    {
        QStackedWidget *stack = qobject_cast<QStackedWidget*>(w->parentWidget());
        KFormDesigner::FormIO::savePropertyElement(
            parentNode, domDoc, "attribute", "stackIndex", stack->indexOf(w));
    } else
        return false;
    return true;
}

#ifdef KEXI_LIST_FORM_WIDGET_SUPPORT
void
KexiStandardFormWidgetsFactory::saveListItem(QListWidgetItem *item,
                               QDomNode &parentNode, QDomDocument &domDoc)
{
    QDomElement element = domDoc.createElement("item");
    parentNode.appendChild(element);

    // We save the text of each column
    for (int i = 0; i < item->listWidget()->columns(); i++) {
        KFormDesigner::FormIO::savePropertyElement(
            element, domDoc, "property", "text", item->text(i));
    }

    // Then we save every sub items
    QListWidgetItem *child = item->firstChild();
    while (child) {
        saveListItem(child, element, domDoc);
        child = child->nextSibling();
    }
}
#endif

bool KexiStandardFormWidgetsFactory::readSpecialProperty(const QByteArray &classname,
                                                         QDomElement &node, QWidget *w,
                                                         KFormDesigner::ObjectTreeItem *item)
{
    const QString tag( node.tagName() );
    const QString name( node.attribute("name") );
    KFormDesigner::Form *form = item->container()
            ? item->container()->form() : item->parent()->container()->form();

    if ((tag == "item") && (classname == "KComboBox")) {
        KComboBox *combo = qobject_cast<KComboBox*>(w);
        QVariant val = KFormDesigner::FormIO::readPropertyValue(
                    form, node.firstChild().firstChild(), w, name);
        if (val.canConvert(QVariant::Pixmap))
            combo->addItem(val.value<QPixmap>(), QString());
        else
            combo->addItem(val.toString());
        return true;
    }
#ifdef KEXI_LIST_FORM_WIDGET_SUPPORT
    else if (tag == "column" && classname == "QTreeWidget") {
        QTreeWidget *tw = qobject_cast<QTreeWidget*>(w);
        int id = 0;
        for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
            QString prop = n.toElement().attribute("name");
            QVariant val = KFormDesigner::FormIO::readPropertyValue(n.firstChild(), w, name);
            if (prop == "text")
                id = tw->addColumn(val.toString());
            else if (prop == "width")
                tw->setColumnWidth(id, val.toInt());
            else if (prop == "resizable")
                tw->header()->setResizeEnabled(val.toBool(), id);
            else if (prop == "clickable")
                tw->header()->setClickEnabled(val.toBool(), id);
            else if (prop == "fullwidth")
                tw->header()->setStretchEnabled(val.toBool(), id);
        }
        return true;
    }
    else if (tag == "item" && classname == "QTreeWidget") {
        QTreeWidget *tw = qobject_cast<QTreeWidget*>(w);
        readListItem(node, 0, tw);
        return true;
    }
#endif
    else if ((name == "title") && (item->parent()->widget()->inherits("QTabWidget"))) {
        TabWidgetBase *tab = qobject_cast<TabWidgetBase*>(w->parentWidget());
        tab->addTab(w, node.firstChild().toElement().text());
        item->addModifiedProperty("title", node.firstChild().toElement().text());
        return true;
    }
    else if (name == "stackIndex"
        && (KexiUtils::objectIsA(w->parentWidget(), "QStackedWidget")
            || /*compat*/ KexiUtils::objectIsA(w->parentWidget(), "QWidgetStack")))
    {
        QStackedWidget *stack = qobject_cast<QStackedWidget*>(w->parentWidget());
        int index = KFormDesigner::FormIO::readPropertyValue(form, node.firstChild(), w, name).toInt();
        stack->insertWidget(index, w);
        stack->setCurrentWidget(w);
        item->addModifiedProperty("stackIndex", index);
        return true;
    }
    return false;
}

#ifdef KEXI_LIST_FORM_WIDGET_SUPPORT
void
KexiStandardFormWidgetsFactory::readTreeItem(
    QDomElement &node, QTreeWidgetItem *parent, QTreeWidget *treewidget)
{
    QTreeWidgetItem *item;
    if (parent)
        item = new QTreeWidgetItem(parent);
    else
        item = new QTreeWidgetItem(treewidget);

    // We need to move the item at the end of the list
    QTreeWidgetItem *last;
    if (parent)
        last = parent->firstChild();
    else
        last = treewidget->firstChild();

    while (last->nextSibling())
        last = last->nextSibling();
    item->moveItem(last);

    int i = 0;
    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        QDomElement childEl = n.toElement();
        QString prop = childEl.attribute("name");
        QString tag = childEl.tagName();

        // We read sub items
        if (tag == "item") {
            item->setOpen(true);
            readListItem(childEl, item, treewidget);
        }
        // and column texts
        else if (tag == "property" && prop == "text") {
            QVariant val = KFormDesigner::FormIO::readPropertyValue(
                n.firstChild(), treewidget, "item");
            item->setText(i, val.toString());
            i++;
        }
    }
}
#endif

bool KexiStandardFormWidgetsFactory::isPropertyVisibleInternal(const QByteArray &classname,
                                                               QWidget *w, const QByteArray &property,
                                                               bool isTopLevel)
{
    bool ok = true;
    if (classname == "FormWidgetBase") {
        if (property == "windowIconText"
                || property == "geometry" /*nonsense for toplevel widget*/)
        {
            return false;
        }
    }
    else if (classname == "CustomWidget") {
    }
    else if (classname == "KexiPictureLabel") {
        if (   property == "text" || property == "indent"
            || property == "textFormat" || property == "font"
            || property == "alignment")
        {
            return false;
        }
    } else if (classname == "QLabel") {
        if (property == "pixmap")
            return false;
    } else if (classname == "QLineEdit") {
        if (property == "vAlign")
            return false;
    } else if (classname == "KTextEdit")
        ok = KFormDesigner::WidgetFactory::advancedPropertiesVisible() ||
             (   property != "undoDepth"
              && property != "undoRedoEnabled" //always true!
              && property != "dragAutoScroll" //always true!
              && property != "overwriteMode" //always false!
              && property != "resizePolicy"
              && property != "autoFormatting" //too complex
#ifndef KEXI_SHOW_UNFINISHED
              && property != "paper"
#endif
             );
    else if (classname == "Line") {
        if ((property == "frameShape") || (property == "font") || (property == "margin"))
            return false;
    } else if (classname == "QCheckBox") {
        ok = KFormDesigner::WidgetFactory::advancedPropertiesVisible() || (property != "autoRepeat");
    } else if (classname == "QRadioButton") {
        ok = KFormDesigner::WidgetFactory::advancedPropertiesVisible() || (property != "autoRepeat");
    } else if (classname == "QPushButton") {
//! @todo reenable autoDefault / default if the top level window is dialog...
        ok = KFormDesigner::WidgetFactory::advancedPropertiesVisible() || (property != "autoDefault" && property != "default");
    }
    else if (   classname == "HBox" || classname == "VBox" || classname == "Grid"
             || classname == "HFlow" || classname == "VFlow")
    {
        return property == "objectName" || property == "geometry";
    }
    else if (classname == "QGroupBox") {
        ok =
#ifndef KEXI_SHOW_UNFINISHED
            /*! @todo Hidden for now in Kexi. "checkable" and "checked" props need adding
            a fake properties which will allow to properly work in design mode, otherwise
            child widgets become frozen when checked==true */
            (KFormDesigner::WidgetFactory::advancedPropertiesVisible() || (property != "checkable" && property != "checked")) &&
#endif
            true;
    } else if (classname == "KFDTabWidget") {
        ok = (KFormDesigner::WidgetFactory::advancedPropertiesVisible()
              || (property != "tabReorderingEnabled" && property != "hoverCloseButton"
                  && property != "hoverCloseButtonDelayed"));
    }
    return ok && WidgetFactory::isPropertyVisibleInternal(classname, w, property, isTopLevel);
}

#ifdef KEXI_LIST_FORM_WIDGET_SUPPORT
void
KexiStandardFormWidgetsFactory::editListContents()
{
    if (widget()->inherits("QTreeWidget"))
        editTreeWidget(qobject_cast<QTreeWidget*>(widget()));
}
#endif

void KexiStandardFormWidgetsFactory::setPropertyOptions(KPropertySet& set,
                                                        const KFormDesigner::WidgetInfo& info,
                                                        QWidget *w)
{
    Q_UNUSED(info);
    Q_UNUSED(w);

    if (set.contains("indent")) {
        set["indent"].setOption("min", -1);
        set["indent"].setOption("minValueText", xi18nc("default indent value", "default"));
    }
}

void KexiStandardFormWidgetsFactory::reorderTabs(int oldpos, int newpos)
{
    KFDTabWidget *tabWidget = qobject_cast<KFDTabWidget*>(sender());
    KFormDesigner::ObjectTreeItem *tab
            = tabWidget->container()->form()->objectTree()->lookup(tabWidget->objectName());
    if (!tab)
        return;

    tab->children()->move(oldpos, newpos);
}

KFormDesigner::ObjectTreeItem* KexiStandardFormWidgetsFactory::selectableItem(
                                                KFormDesigner::ObjectTreeItem* item)
{
    if (item->parent() && item->parent()->widget()) {
        if (qobject_cast<QTabWidget*>(item->parent()->widget())) {
            // tab widget's page
            return item->parent();
        }
    }
    return item;
}

#include "KexiStandardFormWidgetsFactory.moc"
