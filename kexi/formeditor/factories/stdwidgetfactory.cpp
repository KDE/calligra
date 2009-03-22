/* This file is part of the KDE project
   Copyright (C) 2003 by Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2009 Jarosław Staniek <staniek@kde.org>

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
#include <q3header.h>
#include <qdom.h>
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

#include <klineedit.h>
#include <kpushbutton.h>
#include <knuminput.h>
#include <kcombobox.h>
#include <ktextedit.h>
#include <kiconloader.h>
#include <kicon.h>
#include <kgenericfactory.h>
#include <klocale.h>
#include <kdebug.h>
#include <kdeversion.h>

#include <koproperty/Property.h>
#include <koproperty/Set.h>

#include "spring.h"
#include "formIO.h"
#include "form.h"
#include "widgetlibrary.h"
#include "stdwidgetfactory.h"

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

// The factory itself

StdWidgetFactory::StdWidgetFactory(QObject *parent, const QStringList &)
        : KFormDesigner::WidgetFactory(parent, "stdwidgets")
{
    KFormDesigner::WidgetInfo *wFormWidget = new KFormDesigner::WidgetInfo(this);
    wFormWidget->setPixmap("form");
    wFormWidget->setClassName("FormWidgetBase");
    wFormWidget->setName(i18n("Form"));
    wFormWidget->setNamePrefix(i18nc("This string will be used to name widgets of this class. It must _not_ contain white "
                                     "spaces and non latin1 characters.", "form"));
    wFormWidget->setDescription(i18n("A simple form widget"));
    addClass(wFormWidget);

    KFormDesigner::WidgetInfo *wCustomWidget = new KFormDesigner::WidgetInfo(this);
    wCustomWidget->setPixmap("unknown_widget");
    wCustomWidget->setClassName("CustomWidget");
    wCustomWidget->setName(i18n("Custom Widget"));
    wCustomWidget->setNamePrefix(i18nc("This string will be used to name widgets of this class. It must _not_ contain white "
                                       "spaces and non latin1 characters.", "customWidget"));
    wCustomWidget->setDescription(i18n("A custom or non-supported widget"));
    addClass(wCustomWidget);

    KFormDesigner::WidgetInfo *wLabel = new KFormDesigner::WidgetInfo(this);
    wLabel->setPixmap("label");
    wLabel->setClassName("QLabel");
    wLabel->setName(i18n("Text Label"));
    wLabel->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "label"));
    wLabel->setDescription(i18n("A widget to display text"));
    addClass(wLabel);

    KFormDesigner::WidgetInfo *wPixLabel = new KFormDesigner::WidgetInfo(this);
    wPixLabel->setPixmap("pixmaplabel");
    wPixLabel->setClassName("KexiPictureLabel");
    wPixLabel->setName(i18n("Picture Label"));
//! @todo Qt designer compatibility: maybe use this class when QLabel has a pixmap set...?
    //wPixLabel->addAlternateClassName("QLabel");
    wPixLabel->setSavingName("KexiPictureLabel");
    wPixLabel->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "picture"));
    wPixLabel->setDescription(i18n("A widget to display pictures"));
    addClass(wPixLabel);

    KFormDesigner::WidgetInfo *wLineEdit = new KFormDesigner::WidgetInfo(this);
    wLineEdit->setPixmap("lineedit");
    wLineEdit->setClassName("KLineEdit");
    wLineEdit->addAlternateClassName("QLineEdit");
    wLineEdit->setIncludeFileName("klineedit.h");
    wLineEdit->setName(i18n("Line Edit"));
    wLineEdit->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "lineEdit"));
    wLineEdit->setDescription(i18n("A widget to input text"));
    addClass(wLineEdit);

    KFormDesigner::WidgetInfo *wSpring = new KFormDesigner::WidgetInfo(this);
    wSpring->setPixmap("spring");
    wSpring->setClassName("Spring");
    wSpring->setName(i18n("Spring"));
    wSpring->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "spring"));
    wSpring->setDescription(i18n("A spring to place between widgets"));
    addClass(wSpring);

    KFormDesigner::WidgetInfo *wPushButton = new KFormDesigner::WidgetInfo(this);
    wPushButton->setPixmap("button");
    wPushButton->setClassName("KPushButton");
    wPushButton->addAlternateClassName("QPushButton");
    wPushButton->setIncludeFileName("kpushbutton.h");
    wPushButton->setName(i18n("Push Button"));
    wPushButton->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "button"));
    wPushButton->setDescription(i18n("A simple push button to execute actions"));
    addClass(wPushButton);

    KFormDesigner::WidgetInfo *wRadioButton = new KFormDesigner::WidgetInfo(this);
    wRadioButton->setPixmap("radio");
    wRadioButton->setClassName("QRadioButton");
    wRadioButton->setName(i18n("Option Button"));
    wRadioButton->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "optionButton"));
    wRadioButton->setDescription(i18n("An option button with text or pixmap label"));
    addClass(wRadioButton);

    KFormDesigner::WidgetInfo *wCheckBox = new KFormDesigner::WidgetInfo(this);
    wCheckBox->setPixmap("check");
    wCheckBox->setClassName("QCheckBox");
    wCheckBox->setName(i18n("Check Box"));
    wCheckBox->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "checkBox"));
    wCheckBox->setDescription(i18n("A check box with text or pixmap label"));
    addClass(wCheckBox);

    KFormDesigner::WidgetInfo *wSpinBox = new KFormDesigner::WidgetInfo(this);
    wSpinBox->setPixmap("spin");
    wSpinBox->setClassName("KIntSpinBox");
    wSpinBox->addAlternateClassName("QSpinBox");
    wSpinBox->setIncludeFileName("knuminput.h");
    wSpinBox->setName(i18n("Spin Box"));
    wSpinBox->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "spinBox"));
    wSpinBox->setDescription(i18n("A spin box widget"));
    addClass(wSpinBox);

    KFormDesigner::WidgetInfo *wComboBox = new KFormDesigner::WidgetInfo(this);
    wComboBox->setPixmap("combo");
    wComboBox->setClassName("KComboBox");
    wComboBox->addAlternateClassName("QComboBox");
    wComboBox->setIncludeFileName("kcombobox.h");
    wComboBox->setName(i18n("Combo Box"));
    wComboBox->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "comboBox"));
    wComboBox->setDescription(i18n("A combo box widget"));
    addClass(wComboBox);

#ifndef KEXI_FORMS_NO_LIST_WIDGET
    KFormDesigner::WidgetInfo *wListBox = new KFormDesigner::WidgetInfo(this);
    wListBox->setPixmap("listbox");
    wListBox->setClassName("KListBox");
    wListBox->addAlternateClassName("QListBox");
    wListBox->addAlternateClassName("KListBox");
    wListBox->setIncludeFileName("qlistbox.h");
    wListBox->setName(i18n("List Box"));
    wListBox->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "listBox"));
    wListBox->setDescription(i18n("A simple list widget"));
    addClass(wListBox);
#endif

    KFormDesigner::WidgetInfo *wTextEdit = new KFormDesigner::WidgetInfo(this);
    wTextEdit->setPixmap("textedit");
    wTextEdit->setClassName("KTextEdit");
    wTextEdit->addAlternateClassName("QTextEdit");
    wTextEdit->setIncludeFileName("ktextedit.h");
    wTextEdit->setName(i18n("Text Editor"));
    wTextEdit->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "textEditor"));
    wTextEdit->setDescription(i18n("A simple single-page rich text editor"));
    addClass(wTextEdit);

#ifndef KEXI_FORMS_NO_LIST_WIDGET
    KFormDesigner::WidgetInfo *wTreeWidget = new KFormDesigner::WidgetInfo(this);
    wTreeWidget->setPixmap("listwidget");
    wTreeWidget->setClassName("QTreetWidget");
//?    wTreeWidget->addAlternateClassName("QListView");
//?    wTreeWidget->addAlternateClassName("KListView");
    wTreeWidget->setIncludeFileName("qtreewidget.h");
    wTreeWidget->setName(i18n("List Widget"));
    wTreeWidget->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "listWidget"));
    wTreeWidget->setDescription(i18n("A list (or tree) widget"));
    addClass(wTreeWidget);
#endif

    KFormDesigner::WidgetInfo *wSlider = new KFormDesigner::WidgetInfo(this);
    wSlider->setPixmap("slider");
    wSlider->setClassName("QSlider");
    wSlider->setName(i18n("Slider"));
    wSlider->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "slider"));
    wSlider->setDescription(i18n("An horizontal slider"));
    addClass(wSlider);

    KFormDesigner::WidgetInfo *wProgressBar = new KFormDesigner::WidgetInfo(this);
    wProgressBar->setPixmap("progress");
    wProgressBar->setClassName("QProgressBar");
    wProgressBar->setName(i18n("Progress Bar"));
    wProgressBar->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "progressBar"));
    wProgressBar->setDescription(i18n("A progress indicator widget"));
    addClass(wProgressBar);

    KFormDesigner::WidgetInfo *wLine = new KFormDesigner::WidgetInfo(this);
    wLine->setPixmap("line");
    wLine->setClassName("Line");
    wLine->setName(i18n("Line"));
    wLine->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "line"));
    wLine->setDescription(i18n("A line to be used as a separator"));
    addClass(wLine);

    KFormDesigner::WidgetInfo *wDate = new KFormDesigner::WidgetInfo(this);
    wDate->setPixmap("dateedit");
    wDate->setClassName("QDateEdit");
    wDate->addAlternateClassName("KDateWidget");
    wDate->setIncludeFileName("qdateedit.h");
    wDate->setName(i18n("Date Widget"));
    wDate->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "dateWidget"));
    wDate->setDescription(i18n("A widget to input and display a date"));
    addClass(wDate);

    KFormDesigner::WidgetInfo *wTime = new KFormDesigner::WidgetInfo(this);
    wTime->setPixmap("timeedit");
    wTime->setClassName("QTimeEdit");
    wTime->addAlternateClassName("KTimeWidget");
    wTime->setIncludeFileName("qtimewidget.h");
    wTime->setName(i18n("Time Widget"));
    wTime->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "timeWidget"));
    wTime->setDescription(i18n("A widget to input and display a time"));
    addClass(wTime);

    KFormDesigner::WidgetInfo *wDateTime = new KFormDesigner::WidgetInfo(this);
    wDateTime->setPixmap("datetimeedit");
    wDateTime->setClassName("QDateTimeEdit");
    wDateTime->addAlternateClassName("KDateTimeWidget");
    wDateTime->setIncludeFileName("qdatetimewidget.h");
    wDateTime->setName(i18n("Date/Time Widget"));
    wDateTime->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "dateTimeWidget"));
    wDateTime->setDescription(i18n("A widget to input and display a time and a date"));
    addClass(wDateTime);

    m_propDesc["toggleButton"] = i18n("Toggle");
    m_propDesc["checkable"] = i18nc("Button is checkable", "Checkable");
    m_propDesc["autoRepeat"] = i18n("Auto Repeat");
    m_propDesc["autoRepeatDelay"] = i18nc("Auto Repeat Button's Delay", "Auto Rep. Delay");
    m_propDesc["autoRepeatInterval"] = i18nc("Auto Repeat Button's Interval", "Auto Rep. Interval");
    m_propDesc["autoDefault"] = i18n("Auto Default");
    m_propDesc["default"] = i18n("Default");
    m_propDesc["flat"] = i18n("Flat");
    m_propDesc["echoMode"] =
        i18nc("Echo mode for Line Edit widget eg. Normal, NoEcho, Password", "Echo Mode");
    m_propDesc["indent"] = i18n("Indent");
    //line
    m_propDesc["orientation"] = i18n("Orientation");
    //checkbox
    m_propDesc["checked"] = i18nc("Checked checkbox", "Checked");
    m_propDesc["tristate"] = i18nc("Tristate checkbox", "Tristate");

    //for spring
    m_propDesc["sizeType"] = i18n("Size Type");

    //for labels
    m_propDesc["textFormat"] = i18n("Text Format");
    m_propValDesc["PlainText"] = i18nc("For Text Format", "Plain");
    m_propValDesc["RichText"] = i18nc("For Text Format", "Hypertext");
    m_propValDesc["AutoText"] = i18nc("For Text Format", "Auto");
    m_propValDesc["LogText"] = i18nc("For Text Format", "Log");
    m_propDesc["openExternalLinks"] = i18nc("property: Can open external links in label", "Open Ext. Links");

    //KLineEdit
    m_propDesc["clickMessage"] = i18n("Property: \"Click Me\" message for line edit", "Click Message");
    m_propDesc["showClearButton"] = i18n("Property: Show Clear Button", "Clear Button");
    //for EchoMode
/* obsolete
    m_propValDesc["Normal"] = i18nc("For Echo Mode", "Normal");
    m_propValDesc["NoEcho"] = i18nc("For Echo Mode", "No Echo");
    m_propValDesc["Password"] = i18nc("For Echo Mode", "Password"); */
    m_propDesc["passwordMode"] = i18nc("Password Mode for line edit", "Password Mode");
    m_propDesc["squeezedTextEnabled"] = i18nc("Squeezed Text Mode for line edit", "Squeezed Text");
    
    //KTextEdit
    m_propDesc["tabStopWidth"] = i18n("Tab Stop Width");
    m_propDesc["tabChangesFocus"] = i18n("Tab Changes Focus");
    m_propDesc["wrapPolicy"] = i18n("Word Wrap Policy");
    m_propValDesc["AtWordBoundary"] = i18nc("For Word Wrap Policy", "At Word Boundary");
    m_propValDesc["Anywhere"] = i18nc("For Word Wrap Policy", "Anywhere");
    m_propValDesc["AtWordOrDocumentBoundary"] = i18nc("For Word Wrap Policy", "At Word Boundary If Possible");
    m_propDesc["wordWrap"] = i18n("Word Wrapping");
    m_propDesc["wrapColumnOrWidth"] = i18n("Word Wrap Position");
    m_propValDesc["NoWrap"] = i18nc("For Word Wrap Position", "None");
    m_propValDesc["WidgetWidth"] = i18nc("For Word Wrap Position", "Widget's Width");
    m_propValDesc["FixedPixelWidth"] = i18nc("For Word Wrap Position", "In Pixels");
    m_propValDesc["FixedColumnWidth"] = i18nc("For Word Wrap Position", "In Columns");
    m_propDesc["linkUnderline"] = i18n("Links Underlined");
    m_propDesc["horizontalScrollBarPolicy"] = i18n("Horizontal Scroll Bar");
    m_propDesc["verticalScrollBarPolicy"] = i18n("Vertical Scroll Bar");
    //ScrollBarPolicy
    m_propValDesc["ScrollBarAsNeeded"] = i18nc("Show Scroll Bar As Needed", "As Needed");
    m_propValDesc["ScrollBarAlwaysOff"] = i18nc("Scroll Bar Always Off", "Always Off");
    m_propValDesc["ScrollBarAlwaysOn"] = i18nc("Scroll Bar Always On", "Always On");
    m_propDesc["acceptRichText"] = i18nc("Property: Text Edit accepts rich text", "Rich Text");
    m_propDesc["HTML"] = i18nc("Property: HTML value of text edit", "HTML");

    //internal props
    setInternalProperty("Line", "orientationSelectionPopup", "1");
    setInternalProperty("Line", "orientationSelectionPopup:horizontalIcon", "line_horizontal");
    setInternalProperty("Line", "orientationSelectionPopup:verticalIcon", "line_vertical");
    setInternalProperty("Line", "orientationSelectionPopup:horizontalText", i18n("Insert &Horizontal Line"));
    setInternalProperty("Line", "orientationSelectionPopup:verticalText", i18n("Insert &Vertical Line"));
    setInternalProperty("Spring", "orientationSelectionPopup", "1");
    setInternalProperty("Spring", "orientationSelectionPopup:horizontalIcon", "spring");
    setInternalProperty("Spring", "orientationSelectionPopup:verticalIcon", "spring_vertical");
    setInternalProperty("Spring", "orientationSelectionPopup:horizontalText", i18n("Insert &Horizontal Spring"));
    setInternalProperty("Spring", "orientationSelectionPopup:verticalText", i18n("Insert &Vertical Spring"));
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
    const bool designMode = options & KFormDesigner::WidgetFactory::DesignViewMode;

    if (c == "QLabel")
        w = new QLabel(text, p);
    else if (c == "KexiPictureLabel")
        w = new KexiPictureLabel(DesktopIcon("image-x-generic"), p);
    else if (c == "KLineEdit") {
        w = new KLineEdit(p);
        if (designMode)
            w->setCursor(QCursor(Qt::ArrowCursor));
    } else if (c == "KPushButton")
        w = new KPushButton(/*i18n("Button")*/text, p);

    else if (c == "QRadioButton")
        w = new QRadioButton(/*i18n("Radio Button")*/text, p);

    else if (c == "QCheckBox")
        w = new QCheckBox(/*i18n("Check Box")*/text, p);

    else if (c == "KIntSpinBox")
        w = new KIntSpinBox(p);

    else if (c == "KComboBox")
        w = new KComboBox(p);

    else if (c == "KTextEdit")
        w = new KTextEdit(/*i18n("Enter your text here")*/text, p);

#ifndef KEXI_FORMS_NO_LIST_WIDGET
    else if (c == "QTreeWidget") {
        QTreeWidget *tw = new QTreeWidget(p);
        w = tw;
        if (container->form()->interactiveMode()) {
            tw->setColumnCount(1);
            tw->setHeaderItem(new QTreeWidetItem(tw));
            tw->headerItem()->setText(1, i18n("Column 1"));
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

    else if (c == "Spring") {
        w = new Spring(p);
        if (0 == (options & WidgetFactory::AnyOrientation))
            static_cast<Spring*>(w)->setOrientation(
                (options & WidgetFactory::VerticalOrientation)
                ? Qt::Vertical : Qt::Horizontal);
    }

    if (w) {
        setObjectName(n);
        return w;
    }
    kDebug() << "WARNING :: w == 0 ";
    return 0;
}

bool
StdWidgetFactory::previewWidget(const QByteArray &classname, 
                                QWidget *widget, KFormDesigner::Container *)
{
    if (classname == "Spring") {
        dynamic_cast<Spring*>(widget)->setPreviewMode();
        return true;
    }
    return false;
}

bool
StdWidgetFactory::createMenuActions(const QByteArray &classname, QWidget *,
                                    QMenu *menu, KFormDesigner::Container *)
{
    if ((classname == "QLabel") || (classname == "KTextEdit")) {
        menu->addAction(KIcon("document-properties"), i18n("Edit Rich Text"), this, SLOT(editText()));
        return true;
    }
#ifndef KEXI_FORMS_NO_LIST_WIDGET
    else if (classname == "QTreeWidget") {
        menu->addAction(KIcon("document-properties"), i18n("Edit Contents of List Widget"), 
            this, SLOT(editListContents()));
        return true;
    }
#endif

    return false;
}

bool
StdWidgetFactory::startEditing(const QByteArray &classname, QWidget *w, KFormDesigner::Container *container)
{
    setWidget(w, container);
// m_container = container;
    if (classname == "KLineEdit") {
        KLineEdit *lineedit = static_cast<KLineEdit*>(w);
        createEditor(classname, lineedit->text(), lineedit, container, lineedit->geometry(), lineedit->alignment(), true);
        return true;
    } else if (classname == "QLabel") {
        QLabel *label = static_cast<QLabel*>(w);
        if (label->textFormat() == Qt::RichText) {
            //m_widget = w;
//   setWidget(w, container);
            editText();
        } else
            createEditor(classname, label->text(), label, container, label->geometry(), label->alignment());
        return true;
    } else if (classname == "KPushButton") {
        KPushButton *push = static_cast<KPushButton*>(w);
        QStyleOption option;
        option.initFrom(w);
        const QRect r(w->style()->subElementRect(
                          QStyle::SE_PushButtonContents, &option, w));
        const QRect editorRect(push->x() + r.x(), push->y() + r.y(), r.width(), r.height());
        //r.setX(r.x() + 5);
        //r.setY(r.y() + 5);
        //r.setWidth(r.width()-10);
        //r.setHeight(r.height() - 10);
        createEditor(classname, push->text(), push, container, editorRect, Qt::AlignCenter, false, false, Qt::PaletteButton);
        return true;
    } else if (classname == "QRadioButton") {
        QRadioButton *radio = static_cast<QRadioButton*>(w);
        QStyleOption option;
        option.initFrom(w);
        const QRect r(w->style()->subElementRect(
                          QStyle::SE_RadioButtonContents, &option, w));
        const QRect editorRect(
            radio->x() + r.x(), radio->y() + r.y(), r.width(), r.height());
        createEditor(classname, radio->text(), radio, container, editorRect, Qt::AlignLeft);
        return true;
    } else if (classname == "QCheckBox") {
        QCheckBox *check = static_cast<QCheckBox*>(w);
        //QRect r(check->geometry());
        //r.setX(r.x() + 20);
        QStyleOption option;
        option.initFrom(w);
        const QRect r(w->style()->subElementRect(
                          QStyle::SE_CheckBoxContents, &option, w));
        const QRect editorRect(
            check->x() + r.x(), check->y() + r.y(), r.width(), r.height());
        createEditor(classname, check->text(), check, container, editorRect, Qt::AlignLeft);
        return true;
    } else if (classname == "KComboBox") {
        QStringList list;
        KComboBox *combo = dynamic_cast<KComboBox*>(w);
        for (int i = 0; i < combo->count(); i++)
            list.append(combo->itemText(i));

        if (editList(w, list)) {
            dynamic_cast<KComboBox*>(w)->clear();
            dynamic_cast<KComboBox*>(w)->addItems(list);
        }
        return true;
    } else if ((classname == "KTextEdit") || (classname == "KDateTimeWidget") || (classname == "KTimeWidget") ||
               (classname == "KDateWidget") || (classname == "KIntSpinBox")) {
        disableFilter(w, container);
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
StdWidgetFactory::changeText(const QString &text)
{
    QByteArray n = WidgetFactory::widget()->metaObject()->className();
    QWidget *w = WidgetFactory::widget();
    if (n == "KIntSpinBox")
        dynamic_cast<KIntSpinBox*>(w)->setValue(text.toInt());
    else
        changeProperty("text", text, m_container->form());

    /* By-hand method not needed as sizeHint() can do that for us
    QFontMetrics fm = w->fontMetrics();
    QSize s(fm.width( text ), fm.height());
    int width;
    if(n == "QLabel") // labels are resized to fit the text
    {
      w->resize(w->sizeHint());
      WidgetFactory::m_editor->resize(w->size());
      return;
    }
    // and other widgets are just enlarged if needed
    else if(n == "KPushButton")
      width = w->style().sizeFromContents( QStyle::CT_PushButton, w, s).width();
    else if(n == "QCheckBox")
      width = w->style().sizeFromContents( QStyle::CT_CheckBox, w, s).width();
    else if(n == "QRadioButton")
      width = w->style().sizeFromContents( QStyle::CT_RadioButton, w, s).width();
    else
      return;
    int width = w->sizeHint().width();*/

#if 0 //not needed here, size hint is used on creation in InsertWidgetCommand::execute()
    if (w->width() < width) {
        w->resize(width, w->height());
        //WidgetFactory::m_editor->resize(w->size());
    }
#endif
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
                                      KFormDesigner::ObjectTreeItem *)
{
    const QString tag( node.tagName() );
    const QString name( node.attribute("name") );

    if ((tag == "item") && (classname == "KComboBox")) {
        KComboBox *combo = dynamic_cast<KComboBox*>(w);
        QVariant val = KFormDesigner::FormIO::readPropertyValue(node.firstChild().firstChild(), w, name);
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
            return false;
    } else if (classname == "CustomWidget") {
    } else if (classname == "Spring") {
        return Spring::isPropertyVisible(property);
    } else if (classname == "KexiPictureLabel") {
        if ((property == "text") || (property == "indent") || (property == "textFormat") || (property == "font") || (property == "alignment"))
            return false;
    } else if (classname == "QLabel") {
        if (property == "pixmap")
            return false;
    } else if (classname == "KLineEdit") {
        if (property == "vAlign")
            return false;
    } else if (classname == "KTextEdit")
        ok = m_showAdvancedProperties ||
             property != "undoDepth"
             && property != "undoRedoEnabled" //always true!
             && property != "dragAutoScroll" //always true!
             && property != "overwriteMode" //always false!
             && property != "resizePolicy"
             && property != "autoFormatting" //too complex
#ifdef KEXI_NO_UNFINISHED
             && property != "paper"
#endif
             ;
    else if (classname == "Line") {
        if ((property == "frameShape") || (property == "font") || (property == "margin"))
            return false;
    } else if (classname == "QCheckBox") {
        ok = m_showAdvancedProperties || (property != "autoRepeat");
    } else if (classname == "QRadioButton") {
        ok = m_showAdvancedProperties || (property != "autoRepeat");
    } else if (classname == "KPushButton") {
//! @todo reenable autoDefault / default if the top level window is dialog...
        ok = m_showAdvancedProperties || (property != "autoDefault" && property != "default");
    }
    return ok && WidgetFactory::isPropertyVisibleInternal(classname, w, property, isTopLevel);
}

QList<QByteArray>
StdWidgetFactory::autoSaveProperties(const QByteArray &classname)
{
    QList<QByteArray> l;

    if (classname == "QLabel")
        l << "text";
    if (classname == "KPushButton")
        l << "text";
    else if (classname == "KexiPictureLabel")
        l << "pixmap";
    else if (classname == "KComboBox")
        l << "list_items";
#ifndef KEXI_FORMS_NO_LIST_WIDGET
    else if (classname == "QListBox" || classname == "KListBox")
        l << "list_items";
    else if (classname == "QTreeWidget")
        l << "list_contents";
#endif
    else if (classname == "Line")
        l << "orientation";
    else if (classname == "KTimeWidget")
        l << "time";
    else if (classname == "KDateWidget")
        l << "date";
    else if (classname == "KDateTimeWidget")
        l << "dateTime";
    else if (classname == "Spring")
        l << "sizeType" << "orientation";
    else if (classname == "KTextEdit")
        l << "textFormat" << "text";

    return l;
}

void
StdWidgetFactory::editText()
{
    QByteArray classname = widget()->metaObject()->className();
    QString text;
    if (classname == "KTextEdit") {
        KTextEdit* te = dynamic_cast<KTextEdit*>(widget());
        if (te->textFormat() == Qt::RichText || te->textFormat() == Qt::LogText)
            text = te->toHtml();
        else
            text = te->toPlainText();
    } else if (classname == "QLabel")
        text = dynamic_cast<QLabel*>(widget())->text();

    if (editRichText(widget(), text)) {
        changeProperty("textFormat", "RichText", m_container->form());
        changeProperty("text", text, m_container->form());
    }

    if (classname == "QLabel")
        widget()->resize(widget()->sizeHint());
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

KFORMDESIGNER_WIDGET_FACTORY(StdWidgetFactory, stdwidgets)

#include "stdwidgetfactory.moc"

