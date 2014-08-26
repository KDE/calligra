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

#include <QLabel>
#include <QCursor>
#include <QRadioButton>
#include <QCheckBox>
#include <QSlider>
#include <QDomDocument>
#include <QStyle>
#ifndef KEXI_FORMS_NO_LIST_WIDGET
#include <QTreeWidget>
#endif
#include <QPixmap>
#include <QFrame>
#include <QList>
#include <QProgressBar>
#include <QTimeEdit>
#include <QDateEdit>
#include <QDateTimeEdit>

#include <kpushbutton.h>
#include <knuminput.h>
#include <kcombobox.h>
#include <ktextedit.h>
#include <klineedit.h>
#include <klocale.h>
#include <kdebug.h>

#include <kinputdialog.h>
#include <kaction.h>

#include <KoIcon.h>

#include <koproperty/Property.h>
#include <koproperty/Set.h>

#include "formIO.h"
#include "form.h"
#include "widgetlibrary.h"
#include "stdwidgetfactory.h"
#include "objecttree.h"

// Some widgets subclass to allow event filtering and some other things
KexiPictureLabel::KexiPictureLabel(const QPixmap &pix, QWidget *parent)
        : QLabel(parent)
{
    setPixmap(pix);
    setScaledContents(false);
}

KexiPictureLabel::~KexiPictureLabel()
{
}

bool
KexiPictureLabel::setProperty(const char *name, const QVariant &value)
{
    if (0 == qstrcmp(name, "pixmap")) {
        const QPixmap pm(value.value<QPixmap>());
        resize(pm.height(), pm.width());
    }
    return QLabel::setProperty(name, value);
}

Line::Line(Qt::Orientation orient, QWidget *parent)
        : QFrame(parent)
{
    setFrameShadow(Sunken);
    if (orient == Qt::Horizontal)
        setFrameShape(HLine);
    else
        setFrameShape(VLine);
}

Line::~Line()
{
}

void
Line::setOrientation(Qt::Orientation orient)
{
    if (orient == Qt::Horizontal)
        setFrameShape(HLine);
    else
        setFrameShape(VLine);
}

Qt::Orientation
Line::orientation() const
{
    if (frameShape() == HLine)
        return Qt::Horizontal;
    else
        return Qt::Vertical;
}

/////   Internal actions

//! Action of editing rich text for a label or text editor
//! Keeps context expressed using container and receiver widget
class EditRichTextAction : public KAction
{
public:
    EditRichTextAction(KFormDesigner::Container *container,
                       QWidget *receiver, QObject *parent,
                       StdWidgetFactory *factory);
protected slots:
    void slotTriggered();
private:
    KFormDesigner::Container *m_container;
    QWidget *m_receiver;
    StdWidgetFactory *m_factory;
};

EditRichTextAction::EditRichTextAction(KFormDesigner::Container *container, 
                                       QWidget *receiver, QObject *parent,
                                       StdWidgetFactory *factory)
    : KAction(koIcon("document-edit"),
              i18nc("Edit rich text for a widget", "Edit Rich Text"),
              parent)
    , m_container(container)
    , m_receiver(receiver)
    , m_factory(factory)
{
    connect(this, SIGNAL(triggered()), this, SLOT(slotTriggered()));
}

void EditRichTextAction::slotTriggered()
{
    const QByteArray classname( m_receiver->metaObject()->className() );
    QString text;
    if (classname == "KTextEdit") {
        KTextEdit* te = dynamic_cast<KTextEdit*>(m_receiver);
        if (te->acceptRichText()) {
            text = te->toHtml();
        }
        else {
            text = te->toPlainText();
        }
    }
    else if (classname == "QLabel") {
        text = dynamic_cast<QLabel*>(m_receiver)->text();
    }

    if (m_factory->editRichText(m_receiver, text)) {
//! @todo ok?
        m_factory->changeProperty(m_container->form(), m_receiver, "acceptRichText", true);
        m_factory->changeProperty(m_container->form(), m_receiver, "text", text);
    }

    if (classname == "QLabel") {
        m_receiver->resize(m_receiver->sizeHint());
    }
}

// The factory itself

StdWidgetFactory::StdWidgetFactory(QObject *parent, const QVariantList &)
        : KFormDesigner::WidgetFactory(parent, "stdwidgets")
{
    KFormDesigner::WidgetInfo *wFormWidget = new KFormDesigner::WidgetInfo(this);
    wFormWidget->setIconName(koIconName("form"));
    wFormWidget->setClassName("FormWidgetBase");
    wFormWidget->setName(i18n("Form"));
    wFormWidget->setNamePrefix(
        i18nc("A prefix for identifiers of form widgets. Based on that, identifiers such as "
            "form1, form2 are generated. "
            "This string can be used to refer the widget object as variables in programming "
            "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
            "should start with lower case letter and if there are subsequent words, these should "
            "start with upper case letter. Example: smallCamelCase. "
            "Moreover, try to make this prefix as short as possible.",
            "form"));
    wFormWidget->setDescription(i18n("A simple form widget"));
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
    wLineEdit->setClassName("KLineEdit");
    wLineEdit->addAlternateClassName("QLineEdit");
    wLineEdit->setIncludeFileName("klineedit.h");
    wLineEdit->setName(/* no i18n needed */ "Line Edit");
    wLineEdit->setNamePrefix(/* no i18n needed */ "lineEdit");
    wLineEdit->setDescription(/* no i18n needed */ "A widget to input text");
    addClass(wLineEdit);

    KFormDesigner::WidgetInfo *wPushButton = new KFormDesigner::WidgetInfo(this);
    wPushButton->setIconName(koIconName("button"));
    wPushButton->setClassName("KPushButton");
    wPushButton->addAlternateClassName("QPushButton");
    wPushButton->setIncludeFileName("kpushbutton.h");
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
    wSpinBox->setClassName("KIntSpinBox");
    wSpinBox->addAlternateClassName("QSpinBox");
    wSpinBox->setIncludeFileName("knuminput.h");
    wSpinBox->setName(/* no i18n needed */ "Spin Box");
    wSpinBox->setNamePrefix(/* no i18n needed */ "spinBox");
    wSpinBox->setDescription(/* no i18n needed */ "A spin box widget");
    addClass(wSpinBox);

    KFormDesigner::WidgetInfo *wComboBox = new KFormDesigner::WidgetInfo(this);
    wComboBox->setIconName(koIconName("combo"));
    wComboBox->setClassName("KComboBox");
    wComboBox->addAlternateClassName("QComboBox");
    wComboBox->setIncludeFileName("kcombobox.h");
    wComboBox->setName(/* no i18n needed */ "Combo Box");
    wComboBox->setNamePrefix(/* no i18n needed */ "comboBox");
    wComboBox->setDescription(/* no i18n needed */ "A combo box widget");
    wComboBox->setAutoSaveProperties(QList<QByteArray>() << "list_items");
    addClass(wComboBox);

#ifndef KEXI_FORMS_NO_LIST_WIDGET
// Unused, commented-out in Kexi 2.9 to avoid unnecessary translations:
//     KFormDesigner::WidgetInfo *wListBox = new KFormDesigner::WidgetInfo(this);
//     wListBox->setIconName(koIconName("listbox"));
//     wListBox->setClassName("KListBox");
//     wListBox->addAlternateClassName("QListBox");
//     wListBox->addAlternateClassName("KListBox");
//     wListBox->setIncludeFileName("qlistbox.h");
//     wListBox->setName(i18n("List Box"));
//     wListBox->setNamePrefix(
//         i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "listBox"));
//     wListBox->setDescription(i18n("A simple list widget"));
//     wListBox->setAutoSaveProperties(QList<QByteArray>() << "list_items");
//     addClass(wListBox);

// Unused, commented-out in Kexi 2.9 to avoid unnecessary translations:
//     KFormDesigner::WidgetInfo *wTreeWidget = new KFormDesigner::WidgetInfo(this);
//     wTreeWidget->setIconName(koIconName("listwidget"));
//     wTreeWidget->setClassName("QTreetWidget");
// //?    wTreeWidget->addAlternateClassName("QListView");
// //?    wTreeWidget->addAlternateClassName("KListView");
//     wTreeWidget->setIncludeFileName("qtreewidget.h");
//     wTreeWidget->setName(i18n("List Widget"));
//     wTreeWidget->setNamePrefix(
//         i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "listWidget"));
//     wTreeWidget->setDescription(i18n("A list (or tree) widget"));
//     wTreeWidget->setAutoSaveProperties(QList<QByteArray>() << "list_contents");
//     addClass(wTreeWidget);
#endif

    KFormDesigner::WidgetInfo *wTextEdit = new KFormDesigner::WidgetInfo(this);
    wTextEdit->setIconName(koIconName("textedit"));
    wTextEdit->setClassName("KTextEdit");
    wTextEdit->addAlternateClassName("QTextEdit");
    wTextEdit->setIncludeFileName("ktextedit.h");
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
    wLine->setName(i18n("Line"));
    wLine->setNamePrefix(
        i18nc("A prefix for identifiers of line widgets. Based on that, identifiers such as "
            "line1, line2 are generated. "
            "This string can be used to refer the widget object as variables in programming "
            "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
            "should start with lower case letter and if there are subsequent words, these should "
            "start with upper case letter. Example: smallCamelCase. "
            "Moreover, try to make this prefix as short as possible.",
            "line"));
    wLine->setDescription(i18n("A line to be used as a separator"));
    wLine->setAutoSaveProperties(QList<QByteArray>() << "orientation");
    wLine->setInternalProperty("orientationSelectionPopup", true);
    wLine->setInternalProperty("orientationSelectionPopup:horizontalIcon", "line_horizontal");
    wLine->setInternalProperty("orientationSelectionPopup:verticalIcon", "line_vertical");
    wLine->setInternalProperty("orientationSelectionPopup:horizontalText", i18n("Insert &Horizontal Line"));
    wLine->setInternalProperty("orientationSelectionPopup:verticalText", i18n("Insert &Vertical Line"));
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

    setPropertyDescription("checkable", i18nc("Property: Button is checkable", "On/Off"));
    setPropertyDescription("autoRepeat", i18nc("Property: Button", "Auto Repeat"));
    setPropertyDescription("autoRepeatDelay", i18nc("Property: Auto Repeat Button's Delay", "Auto Rep. Delay"));
    setPropertyDescription("autoRepeatInterval", i18nc("Property: Auto Repeat Button's Interval", "Auto Rep. Interval"));
    // unused (too advanced) setPropertyDescription("autoDefault", i18n("Auto Default"));
    // unused (too advanced) setPropertyDescription("default", i18nc("Property: Button is default", "Default"));
    setPropertyDescription("flat", i18nc("Property: Button is flat", "Flat"));
    setPropertyDescription("echoMode",
        i18nc("Property: Echo mode for Line Edit widget eg. Normal, NoEcho, Password", "Echo Mode"));
    setPropertyDescription("indent", i18n("Indent"));
    //line
    setPropertyDescription("orientation", i18n("Orientation"));
    //checkbox
    setPropertyDescription("checked", i18nc("Property: Checked checkbox", "Checked"));
    setPropertyDescription("tristate", i18nc("Property: Tristate checkbox", "Tristate"));

    //for labels
    setPropertyDescription("textFormat", i18n("Text Format"));
    setValueDescription("PlainText", i18nc("For Text Format", "Plain"));
    setValueDescription("RichText", i18nc("For Text Format", "Hypertext"));
    setValueDescription("AutoText", i18nc("For Text Format", "Auto"));
    setValueDescription("LogText", i18nc("For Text Format", "Log"));
    setPropertyDescription("openExternalLinks", i18nc("property: Can open external links in label", "Open Ext. Links"));

    //KLineEdit
#if QT_VERSION  >= 0x040700
    setPropertyDescription("placeholderText", i18nc("Property: line edit's placeholder text", "Placeholder Text"));
#else
    setPropertyDescription("clickMessage", i18nc("Property: \"Click Me\",message for line edit", "Click Message"));
#endif
    setPropertyDescription("showClearButton", i18nc("Property: Show Clear Button", "Clear Button"));
    //for EchoMode
    setPropertyDescription("passwordMode", i18nc("Property: Password Mode for line edit", "Password Mode"));
    setPropertyDescription("squeezedTextEnabled", i18nc("Property: Squeezed Text Mode for line edit", "Squeezed Text"));
    
    //KTextEdit
    setPropertyDescription("tabStopWidth", i18n("Tab Stop Width"));
    setPropertyDescription("tabChangesFocus", i18n("Tab Changes Focus"));
    setPropertyDescription("wrapPolicy", i18n("Word Wrap Policy"));
    setValueDescription("AtWordBoundary", i18nc("Property: For Word Wrap Policy", "At Word Boundary"));
    setValueDescription("Anywhere", i18nc("Property: For Word Wrap Policy", "Anywhere"));
    setValueDescription("AtWordOrDocumentBoundary", i18nc("Property: For Word Wrap Policy", "At Word Boundary If Possible"));
    setPropertyDescription("wordWrap", i18n("Word Wrapping"));
    setPropertyDescription("wrapColumnOrWidth", i18n("Word Wrap Position"));
    setValueDescription("NoWrap", i18nc("Property: For Word Wrap Position", "None"));
    setValueDescription("WidgetWidth", i18nc("Property: For Word Wrap Position", "Widget's Width"));
    setValueDescription("FixedPixelWidth", i18nc("Property: For Word Wrap Position", "In Pixels"));
    setValueDescription("FixedColumnWidth", i18nc("Property: For Word Wrap Position", "In Columns"));
    setPropertyDescription("linkUnderline", i18n("Links Underlined"));
    setPropertyDescription("horizontalScrollBarPolicy", i18n("Horizontal Scroll Bar"));
    setPropertyDescription("verticalScrollBarPolicy", i18n("Vertical Scroll Bar"));
    //ScrollBarPolicy
    setValueDescription("ScrollBarAsNeeded", i18nc("Property: Show Scroll Bar As Needed", "As Needed"));
    setValueDescription("ScrollBarAlwaysOff", i18nc("Property: Scroll Bar Always Off", "Always Off"));
    setValueDescription("ScrollBarAlwaysOn", i18nc("Property: Scroll Bar Always On", "Always On"));
    setPropertyDescription("acceptRichText", i18nc("Property: Text Edit accepts rich text", "Rich Text"));
    setPropertyDescription("HTML", i18nc("Property: HTML value of text edit", "HTML"));
}

StdWidgetFactory::~StdWidgetFactory()
{
}

QWidget*
StdWidgetFactory::createWidget(const QByteArray &c, QWidget *p, const char *n,
                               KFormDesigner::Container *container,
                               CreateWidgetOptions options)
{
    QWidget *w = 0;
    QString text(container->form()->library()->textForWidgetName(n, c));
    if (c == "QLabel")
        w = new QLabel(text, p);
    else if (c == "KexiPictureLabel")
        w = new KexiPictureLabel(koDesktopIcon("image-x-generic"), p);
    else if (c == "KLineEdit") {
        w = new KLineEdit(p);
    } else if (c == "KPushButton")
        w = new KPushButton(text, p);

    else if (c == "QRadioButton")
        w = new QRadioButton(text, p);

    else if (c == "QCheckBox")
        w = new QCheckBox(text, p);

    else if (c == "KIntSpinBox")
        w = new KIntSpinBox(p);

    else if (c == "KComboBox")
        w = new KComboBox(p);

    else if (c == "KTextEdit")
        w = new KTextEdit(text, p);

#ifndef KEXI_FORMS_NO_LIST_WIDGET
    else if (c == "QTreeWidget") {
        QTreeWidget *tw = new QTreeWidget(p);
        w = tw;
        if (container->form()->interactiveMode()) {
            tw->setColumnCount(1);
            tw->setHeaderItem(new QTreeWidetItem(tw));
            tw->headerItem()->setText(1, futureI18n("Column 1"));
        }
        lw->setRootIsDecorated(true);
    }
#endif
    else if (c == "QSlider")
        w = new QSlider(Qt::Horizontal, p);

    else if (c == "QProgressBar")
        w = new QProgressBar(p);

    else if (c == "KDateWidget" || c == "QDateEdit")
        w = new QDateEdit(QDate::currentDate(), p);

    else if (c == "KTimeWidget" || c == "QTimeEdit")
        w = new QTimeEdit(QTime::currentTime(), p);

    else if (c == "KDateTimeWidget" || c == "QDateTimeEdit")
        w = new QDateTimeEdit(QDateTime::currentDateTime(), p);

    else if (c == "Line")
        w = new Line(options & WidgetFactory::VerticalOrientation
                     ? Qt::Vertical : Qt::Horizontal, p);

    if (w) {
        w->setObjectName(n);
        kDebug() << w << w->objectName() << "created";
        return w;
    }
    kWarning() << "w == 0";
    return 0;
}

bool
StdWidgetFactory::previewWidget(const QByteArray &classname, 
                                QWidget *widget, KFormDesigner::Container *)
{
    Q_UNUSED(classname);
    Q_UNUSED(widget);
    return true;
}

bool
StdWidgetFactory::createMenuActions(const QByteArray &classname, QWidget *w,
                                    QMenu *menu, KFormDesigner::Container *container)
{
    if ((classname == "QLabel") || (classname == "KTextEdit")) {
        menu->addAction( new EditRichTextAction(container, w, menu, this) );
        return true;
    }
#ifndef KEXI_FORMS_NO_LIST_WIDGET
    else if (classname == "QTreeWidget") {
        menu->addAction(koIcon("document-properties"), i18n("Edit Contents of List Widget"), 
            this, SLOT(editListContents()));
        return true;
    }
#endif

    return false;
}

bool
StdWidgetFactory::startInlineEditing(InlineEditorCreationArguments& args)
{
    if (args.classname == "KLineEdit") {
        KLineEdit *lineedit = static_cast<KLineEdit*>(args.widget);
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
    else if (args.classname == "KPushButton") {
        KPushButton *push = static_cast<KPushButton*>(args.widget);
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
    } else if (args.classname == "KComboBox") {
        QStringList list;
        KComboBox *combo = dynamic_cast<KComboBox*>(args.widget);
        for (int i = 0; i < combo->count(); i++) {
            list.append(combo->itemText(i));
        }
        args.execute = false;
        if (editList(args.widget, list)) {
            dynamic_cast<KComboBox*>(args.widget)->clear();
            dynamic_cast<KComboBox*>(args.widget)->addItems(list);
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

bool
StdWidgetFactory::clearWidgetContent(const QByteArray &classname, QWidget *w)
{
    if (classname == "KLineEdit")
        dynamic_cast<KLineEdit*>(w)->clear();
#ifndef KEXI_FORMS_NO_LIST_WIDGET
    else if (classname == "QTreeWidget")
        dynamic_cast<KListWidget*>(w)->clear();
#endif
    else if (classname == "KComboBox")
        dynamic_cast<KComboBox*>(w)->clear();
    else if (classname == "KTextEdit")
        dynamic_cast<KTextEdit*>(w)->clear();
    else
        return false;
    return true;
}

bool
StdWidgetFactory::changeInlineText(KFormDesigner::Form *form, QWidget *widget, 
                                   const QString &text, QString &oldText)
{
    const QByteArray n(widget->metaObject()->className());
    if (n == "KIntSpinBox") {
        oldText = QString::number(dynamic_cast<KIntSpinBox*>(widget)->value());
        dynamic_cast<KIntSpinBox*>(widget)->setValue(text.toInt());
    }
    else {
        oldText = widget->property("text").toString();
        changeProperty(form, widget, "text", text);
    }
    return true;
}

void
StdWidgetFactory::resizeEditor(QWidget *editor, QWidget *widget, 
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
    } else if (classname == "KPushButton") {
        QStyleOption option;
        option.initFrom(widget);
        r = widget->style()->subElementRect(
                QStyle::SE_PushButtonContents, &option, widget);
        p += r.topLeft();
        s = r.size();
    }

    editor->resize(s);
    editor->move(p);
}

bool
StdWidgetFactory::saveSpecialProperty(const QByteArray &classname, 
                                      const QString &name, const QVariant &, 
                                      QWidget *w, QDomElement &parentNode, QDomDocument &domDoc)
{
    if (name == "list_items" && classname == "KComboBox") {
        KComboBox *combo = dynamic_cast<KComboBox*>(w);
        for (int i = 0; i < combo->count(); i++) {
            QDomElement item = domDoc.createElement("item");
            KFormDesigner::FormIO::savePropertyElement(item, domDoc, "property", "text", combo->itemText(i));
            parentNode.appendChild(item);
        }
        return true;
    }
#ifndef KEXI_FORMS_NO_LIST_WIDGET
    else if (name == "list_contents" && classname == "QTreeWidget") {
        QTreeWidget *treewidget = dynamic_cast<QTreeWidget*>(w);
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
    return false;
}

#ifndef KEXI_FORMS_NO_LIST_WIDGET
void
StdWidgetFactory::saveListItem(QListWidgetItem *item, 
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

bool
StdWidgetFactory::readSpecialProperty(const QByteArray &classname, 
                                      QDomElement &node, QWidget *w, 
                                      KFormDesigner::ObjectTreeItem *item)
{
    const QString tag( node.tagName() );
    const QString name( node.attribute("name") );
    KFormDesigner::Form *form = item->container() ? item->container()->form() : item->parent()->container()->form();

    if ((tag == "item") && (classname == "KComboBox")) {
        KComboBox *combo = dynamic_cast<KComboBox*>(w);
        QVariant val = KFormDesigner::FormIO::readPropertyValue(
                    form, node.firstChild().firstChild(), w, name);
        if (val.canConvert(QVariant::Pixmap))
            combo->addItem(val.value<QPixmap>(), QString());
        else
            combo->addItem(val.toString());
        return true;
    }

    if (false) {
    }
#ifndef KEXI_FORMS_NO_LIST_WIDGET
    else if (tag == "column" && classname == "QTreeWidget")) {
        QTreeWidget *tw = dynamic_cast<QTreeWidget*>(w);
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
        QTreeWidget *tw = dynamic_cast<QTreeWidget*>(w);
        readListItem(node, 0, tw);
        return true;
    }
#endif
    return false;
}

#ifndef KEXI_FORMS_NO_LIST_WIDGET
void
StdWidgetFactory::readTreeItem(
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

bool
StdWidgetFactory::isPropertyVisibleInternal(const QByteArray &classname,
                                            QWidget *w, const QByteArray &property, bool isTopLevel)
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
    } else if (classname == "KLineEdit") {
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
    } else if (classname == "KPushButton") {
//! @todo reenable autoDefault / default if the top level window is dialog...
        ok = KFormDesigner::WidgetFactory::advancedPropertiesVisible() || (property != "autoDefault" && property != "default");
    }
    return ok && WidgetFactory::isPropertyVisibleInternal(classname, w, property, isTopLevel);
}

#ifndef KEXI_FORMS_NO_LIST_WIDGET
void
StdWidgetFactory::editListContents()
{
    if (widget()->inherits("QTreeWidget"))
        editTreeWidget(dynamic_cast<QTreeWidget*>(widget()));
}
#endif

void
StdWidgetFactory::setPropertyOptions(KoProperty::Set& set, const KFormDesigner::WidgetInfo& info, QWidget *w)
{
    Q_UNUSED(info);
    Q_UNUSED(w);

    if (set.contains("indent")) {
        set["indent"].setOption("min", -1);
        set["indent"].setOption("minValueText", i18nc("default indent value", "default"));
    }
}

K_EXPORT_KEXIFORMWIDGETS_PLUGIN(StdWidgetFactory, stdwidgets)

#include "stdwidgetfactory.moc"

