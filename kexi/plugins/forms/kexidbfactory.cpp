/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2006 Jarosław Staniek <staniek@kde.org>

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

#include <q3scrollview.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qstyle.h>
//Added by qt3to4:
#include <QLabel>
#include <Q3ValueList>
#include <Q3CString>

#include <kgenericfactory.h>
#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kactioncollection.h>
#include <kstandardaction.h>

#include <formeditor/container.h>
#include <formeditor/form.h>
#include <formeditor/formIO.h>
#include <formeditor/formmanager.h>
#include <formeditor/objecttree.h>
#include <formeditor/utils.h>
#include <kexidb/utils.h>
#include <kexidb/connection.h>
#include <kexipart.h>
#include <formeditor/widgetlibrary.h>
#include <kexiutils/utils.h>
#include <widget/kexicustompropertyfactory.h>
#include <widget/utils/kexicontextmenuutils.h>

#include "kexiformview.h"
#include "widgets/kexidbautofield.h"
#include "widgets/kexidbcheckbox.h"
#include "widgets/kexidbimagebox.h"
//#include "widgets/kexidbdoublespinbox.h"
//#include "widgets/kexidbintspinbox.h"
#include "widgets/kexiframe.h"
#include "widgets/kexidblabel.h"
#include "widgets/kexidblineedit.h"
#include "widgets/kexidbtextedit.h"
#include "widgets/kexidbcombobox.h"
#include "widgets/kexipushbutton.h"
#include "widgets/kexidbform.h"
#include "widgets/kexidbsubform.h"
#include "kexidataawarewidgetinfo.h"

#include "kexidbfactory.h"
#include <core/kexi.h>
#include <kexi_global.h>
#include <KexiMainWindowIface.h>


//////////////////////////////////////////

KexiDBFactory::KexiDBFactory(QObject *parent, const QStringList &)
        : KFormDesigner::WidgetFactory(parent)
{
    KFormDesigner::WidgetInfo *wi;
    wi = new KexiDataAwareWidgetInfo(this);
    wi->setPixmap("form");
    wi->setClassName("KexiDBForm");
    wi->setName(i18n("Form"));
    wi->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. "
              "It must _not_ contain white spaces and non latin1 characters.", "form"));
    wi->setDescription(i18n("A data-aware form widget"));
    addClass(wi);

#ifndef KEXI_NO_SUBFORM
    wi = new KexiDataAwareWidgetInfo(this);
    wi->setPixmap("subform");
    wi->setClassName("KexiDBSubForm");
    wi->addAlternateClassName("KexiSubForm", true/*override*/); //older
    wi->setName(i18n("Sub Form"));
    wi->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. "
              "It must _not_ contain white spaces and non latin1 characters.", "subForm"));
    wi->setDescription(i18n("A form widget included in another Form"));
    wi->setAutoSyncForProperty("formName", false);
    addClass(wi);
#endif

    // inherited
    wi = new KexiDataAwareWidgetInfo(this, "stdwidgets", "KLineEdit");
    wi->setPixmap("lineedit");
    wi->setClassName("KexiDBLineEdit");
    wi->addAlternateClassName("QLineEdit", true/*override*/);
    wi->addAlternateClassName("KLineEdit", true/*override*/);
    wi->setIncludeFileName("klineedit.h");
    wi->setName(i18n("Text Box"));
    wi->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. "
              "It must _not_ contain white spaces and non latin1 characters.", "textBox"));
    wi->setDescription(i18n("A widget for entering and displaying text"));
    addClass(wi);

    // inherited
    wi = new KexiDataAwareWidgetInfo(this, "stdwidgets", "KTextEdit");
    wi->setPixmap("textedit");
    wi->setClassName("KexiDBTextEdit");
    wi->addAlternateClassName("QTextEdit", true/*override*/);
    wi->addAlternateClassName("KTextEdit", true/*override*/);
    wi->setIncludeFileName("ktextedit.h");
    wi->setName(i18n("Text Editor"));
    wi->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. "
              "It must _not_ contain white spaces and non latin1 characters.", "textEditor"));
    wi->setDescription(i18n("A multiline text editor"));
    addClass(wi);

    wi = new KFormDesigner::WidgetInfo(
        this, "containers", "QFrame" /*we are inheriting to get i18n'd strings already translated there*/);
    wi->setPixmap("frame");
    wi->setClassName("KexiFrame");
    wi->addAlternateClassName("QFrame", true/*override*/);
    wi->addAlternateClassName("Q3Frame", true/*override*/);
    wi->setName(i18n("Frame"));
    wi->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. "
              "It must _not_ contain white spaces and non latin1 characters.", "frame"));
    wi->setDescription(i18n("A simple frame widget"));
    addClass(wi);

    wi = new KexiDataAwareWidgetInfo(
        this, "stdwidgets", "QLabel" /*we are inheriting to get i18n'd strings already translated there*/);
    wi->setPixmap("label");
    wi->setClassName("KexiDBLabel");
    wi->addAlternateClassName("QLabel", true/*override*/);
    wi->addAlternateClassName("KexiLabel", true/*override*/); //older
    wi->setName(i18nc("Text Label", "Label"));
    wi->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. "
              "It must _not_ contain white spaces and non latin1 characters.", "label"));
    wi->setDescription(i18n("A widget for displaying text"));
    addClass(wi);

#ifndef KEXI_NO_IMAGEBOX_WIDGET
    wi = new KexiDataAwareWidgetInfo(
        this, "stdwidgets", "KexiPictureLabel" /*we are inheriting to get i18n'd strings already translated there*/);
    wi->setPixmap("pixmaplabel");
    wi->setClassName("KexiDBImageBox");
    wi->addAlternateClassName("KexiPictureLabel", true/*override*/);
    wi->addAlternateClassName("KexiImageBox", true/*override*/); //older
    wi->setName(i18n("Image Box"));
    wi->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. "
              "It must _not_ contain white spaces and non latin1 characters.", "image"));
    wi->setDescription(i18n("A widget for displaying images"));
// wi->setCustomTypeForProperty("pixmapData", KexiCustomPropertyFactory::PixmapData);
    wi->setCustomTypeForProperty("pixmapId", KexiCustomPropertyFactory::PixmapId);
    addClass(wi);

    setInternalProperty("KexiDBImageBox", "dontStartEditingOnInserting", "1");
// setInternalProperty("KexiDBImageBox", "forceShowAdvancedProperty:pixmap", "1");
#endif

#ifdef KEXI_DB_COMBOBOX_WIDGET
    wi = new KexiDataAwareWidgetInfo(
        this, "stdwidgets", "KComboBox" /*we are inheriting to get i18n'd strings already translated there*/);
    wi->setPixmap("combo");
    wi->setClassName("KexiDBComboBox");
    wi->addAlternateClassName("KComboBox", true/*override*/);
    wi->setName(i18n("Combo Box"));
    wi->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. "
              "It must _not_ contain white spaces and non latin1 characters.", "comboBox"));
    wi->setDescription(i18n("A combo box widget"));
    addClass(wi);
#endif

    wi = new KexiDataAwareWidgetInfo(this, "stdwidgets", "QCheckBox");
    wi->setPixmap("check");
    wi->setClassName("KexiDBCheckBox");
    wi->addAlternateClassName("QCheckBox", true/*override*/);
    wi->setName(i18n("Check Box"));
    wi->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. "
              "It must _not_ contain white spaces and non latin1 characters.", "checkBox"));
    wi->setDescription(i18n("A check box with text label"));
    addClass(wi);

#ifndef KEXI_NO_AUTOFIELD_WIDGET
    wi = new KexiDataAwareWidgetInfo(this);
    wi->setPixmap("autofield");
    wi->setClassName("KexiDBAutoField");
    wi->addAlternateClassName("KexiDBFieldEdit", true/*override*/); //older
    wi->setName(i18n("Auto Field"));
    wi->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. "
              "It must _not_ contain white spaces and non latin1 characters", "autoField"));
    wi->setDescription(i18n("A widget containing an automatically selected editor "
                            "and a label to edit the value of a database field of any type."));
    addClass(wi);
#endif

    /*
      KexiDataAwareWidgetInfo *wDate = new KexiDataAwareWidgetInfo(this, "stdwidgets", "KDateWidget");
      wDate->setPixmap("dateedit");
      wDate->setClassName("KexiDBDateEdit");
      wDate->addAlternateClassName("QDateEdit", true);//override
      wDate->addAlternateClassName("KDateWidget", true);//override
      wDate->setName(i18n("Date Widget"));
      wDate->setNamePrefix(
        i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "dateWidget"));
      wDate->setDescription(i18n("A widget to input and display a date"));
      addClass(wDate);

      KexiDataAwareWidgetInfo *wTime = new KexiDataAwareWidgetInfo(this, "stdwidgets", "KTimeWidget");
      wTime->setPixmap("timeedit");
      wTime->setClassName("KexiDBTimeEdit");
      wTime->addAlternateClassName("QTimeEdit", true);//override
      wTime->addAlternateClassName("KTimeWidget", true);//override
      wTime->setName(i18n("Time Widget"));
      wTime->setNamePrefix(
        i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "timeWidget"));
      wTime->setDescription(i18n("A widget to input and display a time"));
      addClass(wTime);

      KexiDataAwareWidgetInfo *wDateTime = new KexiDataAwareWidgetInfo(this, "stdwidgets", "KDateTimeWidget");
      wDateTime->setPixmap("datetimeedit");
      wDateTime->setClassName("KexiDBDateTimeEdit");
      wDateTime->addAlternateClassName("QDateTimeEdit", true);//override
      wDateTime->addAlternateClassName("KDateTimeWidget", true);//override
      wDateTime->setName(i18n("Date/Time Widget"));
      wDateTime->setNamePrefix(
        i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "dateTimeWidget"));
      wDateTime->setDescription(i18n("A widget to input and display a date and time"));
      addClass(wDateTime);
    */

    /* KexiDataAwareWidgetInfo *wIntSpinBox = new KexiDataAwareWidgetInfo(this, "stdwidgets", "KIntSpinBox");
      wIntSpinBox->setPixmap("spin");
      wIntSpinBox->setClassName("KexiDBIntSpinBox");
      wIntSpinBox->addAlternateClassName("QSpinBox", true);
      wIntSpinBox->addAlternateClassName("KIntSpinBox", true);
      wIntSpinBox->setName(i18n("Integer Number Spin Box"));
      wIntSpinBox->setNamePrefix(
        i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "intSpinBox"));
      wIntSpinBox->setDescription(i18n("A spin box widget to input and display integer numbers"));
      addClass(wIntSpinBox);

      KexiDataAwareWidgetInfo *wDoubleSpinBox = new KexiDataAwareWidgetInfo(this, "stdwidgets");
      wDoubleSpinBox->setPixmap("spin");
      wDoubleSpinBox->setClassName("KexiDBDoubleSpinBox");
      wDoubleSpinBox->addAlternateClassName("KDoubleSpinBox", true);
      wDoubleSpinBox->setName(i18n("Floating-point Number Spin Box"));
      wDoubleSpinBox->setNamePrefix(
        i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "dblSpinBox"));
      wDoubleSpinBox->setDescription(i18n("A spin box widget to input and display floating-point numbers"));
      addClass(wDoubleSpinBox);*/

    // inherited
    wi = new KFormDesigner::WidgetInfo(
        this, "stdwidgets", "KPushButton");
    wi->addAlternateClassName("KexiPushButton");
    wi->setName(i18n("Command Button"));
    wi->setNamePrefix(
        i18nc("Widget name. This string will be used to name widgets of this class. "
              "It must _not_ contain white spaces and non latin1 characters.", "button"));
    wi->setDescription(i18n("A command button to execute actions"));
    addClass(wi);

    m_propDesc["dataSource"] = i18n("Data Source");
    m_propDesc["formName"] = i18n("Form Name");
    m_propDesc["onClickAction"] = i18n("On Click");
    m_propDesc["onClickActionOption"] = i18n("On Click Option");
    m_propDesc["autoTabStops"] = i18n("Auto Tab Order");
    m_propDesc["shadowEnabled"] = i18n("Shadow Enabled");
    m_propDesc["on"] = i18nc("On: button", "On");

    m_propDesc["widgetType"] = i18n("Editor Type");
    //for autofield's type: inherit i18n from KexiDB
    m_propValDesc["Auto"] = i18nc("AutoField editor's type", "Auto");
    m_propValDesc["Text"] = KexiDB::Field::typeName(KexiDB::Field::Text);
    m_propValDesc["Integer"] = KexiDB::Field::typeName(KexiDB::Field::Integer);
    m_propValDesc["Double"] = KexiDB::Field::typeName(KexiDB::Field::Double);
    m_propValDesc["Boolean"] = KexiDB::Field::typeName(KexiDB::Field::Boolean);
    m_propValDesc["Date"] = KexiDB::Field::typeName(KexiDB::Field::Date);
    m_propValDesc["Time"] = KexiDB::Field::typeName(KexiDB::Field::Time);
    m_propValDesc["DateTime"] = KexiDB::Field::typeName(KexiDB::Field::DateTime);
    m_propValDesc["MultiLineText"] = i18nc("AutoField editor's type", "Multiline Text");
    m_propValDesc["ComboBox"] = i18nc("AutoField editor's type", "Drop-Down List");
    m_propValDesc["Image"] = i18nc("AutoField editor's type", "Image");

// m_propDesc["labelCaption"] = i18n("Label Text");
    m_propDesc["autoCaption"] = i18n("Auto Label");
    m_propDesc["foregroundLabelColor"] = i18n("Label Text Color");
    m_propDesc["backgroundLabelColor"] = i18nc("(a property name, keep the text narrow!)",
                                         "Label Background\nColor");

    m_propDesc["labelPosition"] = i18n("Label Position");
    m_propValDesc["Left"] = i18nc("Label Position", "Left");
    m_propValDesc["Top"] = i18nc("Label Position", "Top");
    m_propValDesc["NoLabel"] = i18nc("Label Position", "No Label");

    m_propDesc["sizeInternal"] = i18n("Size");
// m_propDesc["pixmap"] = i18n("Image");
    m_propDesc["pixmapId"] = i18n("Image");
    m_propDesc["scaledContents"] = i18n("Scaled Contents");
    m_propDesc["keepAspectRatio"] = i18nc("Keep Aspect Ratio (short)", "Keep Ratio");

    //hide classes that are replaced by db-aware versions
    hideClass("KexiPictureLabel");
    hideClass("KComboBox");

    //used in labels, frames...
    m_propDesc["frameColor"] = i18n("Frame Color");
    m_propDesc["dropDownButtonVisible"] =
        i18nc("Drop-Down Button for Image Box Visible (a property name, keep the text narrow!)",
              "Drop-Down\nButton Visible");

    //for checkbox
    m_propValDesc["TristateDefault"] = i18nc("Tristate checkbox, default", "Default");
    m_propValDesc["TristateOn"] = i18nc("Tristate checkbox, yes", "Yes");
    m_propValDesc["TristateOff"] = i18nc("Tristate checkbox, no", "No");

    //for combobox
    m_propDesc["editable"] = i18nc("Editable combobox", "Editable");
}

KexiDBFactory::~KexiDBFactory()
{
}

QWidget*
KexiDBFactory::createWidget(const Q3CString &c, QWidget *p, const char *n,
                            KFormDesigner::Container *container, int options)
{
    kexipluginsdbg << "KexiDBFactory::createWidget() " << this;

    QWidget *w = 0;
    QString text(container->form()->library()->textForWidgetName(n, c));
    const bool designMode = options & KFormDesigner::WidgetFactory::DesignViewMode;
    bool createContainer = false;

    if (c == "KexiDBSubForm")
        w = new KexiDBSubForm(container->form(), p);
    else if (c == "KexiDBLineEdit") {
        w = new KexiDBLineEdit(p);
        if (designMode)
            w->setCursor(QCursor(Qt::ArrowCursor));
    } else if (c == "KexiDBTextEdit") {
        w = new KexiDBTextEdit(p);
        if (designMode)
            w->setCursor(QCursor(Qt::ArrowCursor));
    } else if (c == "Q3Frame" || c == "QFrame" || c == "KexiFrame") {
        w = new KexiFrame(p);
        createContainer = true;
    } else if (c == "KexiDBLabel")
        w = new KexiDBLabel(text, p);
#ifndef KEXI_NO_IMAGEBOX_WIDGET
    else if (c == "KexiDBImageBox") {
        w = new KexiDBImageBox(designMode, p);
        connect(w, SIGNAL(idChanged(long)), this, SLOT(slotImageBoxIdChanged(long)));
    }
#endif
#ifndef KEXI_NO_AUTOFIELD_WIDGET
    else if (c == "KexiDBAutoField")
        w = new KexiDBAutoField(p, designMode);
#endif
    else if (c == "KexiDBCheckBox")
        w = new KexiDBCheckBox(text, p);
    else if (c == "KexiDBComboBox")
        w = new KexiDBComboBox(p, designMode);
    /* else if(c == "KexiDBTimeEdit")
        w = new KexiDBTimeEdit(QTime::currentTime(), p, n);
      else if(c == "KexiDBDateEdit")
        w = new KexiDBDateEdit(QDate::currentDate(), p, n);
      else if(c == "KexiDBDateTimeEdit")
        w = new KexiDBDateTimeEdit(QDateTime::currentDateTime(), p, n);*/
// else if(c == "KexiDBIntSpinBox")
//  w = new KexiDBIntSpinBox(p, n);
// else if(c == "KexiDBDoubleSpinBox")
//  w = new KexiDBDoubleSpinBox(p, n);
    else if (c == "KPushButton" || c == "KexiPushButton")
        w = new KexiPushButton(text, p);

    if (w)
        w->setObjectName(n);
    if (createContainer)
        (void)new KFormDesigner::Container(container, w, container);
    return w;
}

bool
KexiDBFactory::createMenuActions(const Q3CString &classname, QWidget *w, QMenu *menu,
                                 KFormDesigner::Container *)
{
    if (classname == "QPushButton" || classname == "KPushButton" || classname == "KexiPushButton") {
        /*! @todo also call createMenuActions() for inherited factory! */
        menu->addAction(m_assignAction);
        return true;
    } else if (classname == "KexiDBImageBox") {
        KexiDBImageBox *imageBox = static_cast<KexiDBImageBox*>(w);
        imageBox->contextMenu()->updateActionsAvailability();
        KActionCollection *ac = imageBox->contextMenu()->actionCollection();
        QMenu *subMenu = menu->addMenu(i18n("&Image"));
//! @todo make these actions undoable/redoable
        subMenu->addAction(ac->action("insert"));
        subMenu->addAction(ac->action("file_save_as"));
        subMenu->addSeparator();
        subMenu->addAction(ac->action("edit_cut"));
        subMenu->addAction(ac->action("edit_copy"));
        subMenu->addAction(ac->action("edit_paste"));
        subMenu->addAction(ac->action("delete"));
        if (ac->action("properties")) {
            subMenu->addSeparator();
            subMenu->addAction(ac->action("properties"));
        }
    }
    return false;
}

void
KexiDBFactory::createCustomActions(KActionCollection* col)
{
    //this will create shared instance action for design mode (special collection is provided)
    col->addAction("widget_assign_action",
                   m_assignAction = new KAction(KIcon("form_action"), i18n("&Assign Action..."), this));
}

bool
KexiDBFactory::startEditing(const Q3CString &classname, QWidget *w, KFormDesigner::Container *container)
{
    m_container = container;
    if (classname == "KexiDBLineEdit") {
//! @todo this code should not be copied here but
//! just inherited StdWidgetFactory::clearWidgetContent() should be called
        KLineEdit *lineedit = static_cast<KLineEdit*>(w);
        createEditor(classname, lineedit->text(), lineedit, container,
                     lineedit->geometry(), lineedit->alignment(), true);
        return true;
    }
    if (classname == "KexiDBTextEdit") {
//! @todo this code should not be copied here but
//! just inherited StdWidgetFactory::clearWidgetContent() should be called
        KTextEdit *textedit = static_cast<KTextEdit*>(w);
        createEditor(classname, textedit->text(), textedit, container,
                     textedit->geometry(), textedit->alignment(), true, true);
        //copy a few properties
        KTextEdit *ed = dynamic_cast<KTextEdit *>(editor(w));
        ed->setLineWrapMode(textedit->lineWrapMode());
        ed->setLineWrapColumnOrWidth(textedit->lineWrapColumnOrWidth());
        ed->setWordWrapMode(textedit->wordWrapMode());
        ed->setTabStopWidth(textedit->tabStopWidth());
#ifdef __GNUC__
#warning TODO Qt4:  ed->setLinkUnderline(textedit->linkUnderline());
#else
#pragma WARNING( TODO Qt4:  ed->setLinkUnderline(textedit->linkUnderline()); )
#endif
        ed->setTextFormat(textedit->textFormat());
#ifdef __GNUC__
#warning TODO Qt4:  ed->setHScrollBarMode(textedit->hScrollBarMode());
#else
#pragma WARNING( TODO Qt4:  ed->setHScrollBarMode(textedit->hScrollBarMode()); )
#endif
#ifdef __GNUC__
#warning TODO Qt4:  ed->setVScrollBarMode(textedit->vScrollBarMode());
#else
#pragma WARNING( TODO Qt4:  ed->setVScrollBarMode(textedit->vScrollBarMode()); )
#endif
        return true;
    } else if (classname == "KexiDBLabel") {
        KexiDBLabel *label = static_cast<KexiDBLabel*>(w);
        m_widget = w;
        if (label->textFormat() == Qt::RichText) {
            QString text = label->text();
            if (editRichText(label, text)) {
                changeProperty("textFormat", "RichText", container->form());
                changeProperty("text", text, container->form());
            }

            if (classname == "KexiDBLabel")
                w->resize(w->sizeHint());
        } else {
            createEditor(classname, label->text(), label, container,
                         label->geometry(), label->alignment(),
                         false, label->wordWrap() /*multiline*/);
        }
        return true;
    } else if (classname == "KexiDBSubForm") {
        // open the form in design mode
        KexiDBSubForm *subform = static_cast<KexiDBSubForm*>(w);
        if (KexiMainWindowIface::global()) {
            bool openingCancelled;
            KexiMainWindowIface::global()->openObject(
                "org.kexi-project.form", subform->formName(), Kexi::DesignViewMode,
                openingCancelled);
        }
        return true;
    }
#if 0
    else if ((classname == "KexiDBDateEdit") || (classname == "KexiDBDateTimeEdit") || (classname == "KexiDBTimeEdit")
             /*|| (classname == "KexiDBIntSpinBox") || (classname == "KexiDBDoubleSpinBox")*/) {
        disableFilter(w, container);
        return true;
    }
#endif
    else if (classname == "KexiDBAutoField") {
        if (static_cast<KexiDBAutoField*>(w)->hasAutoCaption())
            return false; // caption is auto, abort editing
        QLabel *label = static_cast<KexiDBAutoField*>(w)->label();
        createEditor(classname, label->text(), label, container, label->geometry(), label->alignment());
        return true;
    } else if (classname == "KexiDBCheckBox") {
        KexiDBCheckBox *cb = static_cast<KexiDBCheckBox*>(w);
        QRect r(cb->geometry());
        r.setLeft(
            r.left() + 2
            + cb->style()->subElementRect(QStyle::SE_CheckBoxIndicator, 0, cb).width());
        createEditor(classname, cb->text(), cb, container, r, Qt::AlignAuto);
        return true;
    } else if (classname == "KexiDBImageBox") {
        KexiDBImageBox *image = static_cast<KexiDBImageBox*>(w);
        image->insertFromFile();
        return true;
    }
    return false;
}

bool
KexiDBFactory::previewWidget(const Q3CString &, QWidget *, KFormDesigner::Container *)
{
    return false;
}

bool
KexiDBFactory::clearWidgetContent(const Q3CString & /*classname*/, QWidget *w)
{
//! @todo this code should not be copied here but
//! just inherited StdWidgetFactory::clearWidgetContent() should be called
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(w);
    if (iface)
        iface->clear();
    return true;
}

Q3ValueList<Q3CString>
KexiDBFactory::autoSaveProperties(const Q3CString & /*classname*/)
{
    Q3ValueList<Q3CString> lst;
// if(classname == "KexiDBSubForm")
    //lst << "formName";
// if(classname == "KexiDBLineEdit")
// lst += "dataSource";
// if(classname == "KexiDBAutoField")
//  lst << "labelCaption";
    return lst;
}

bool
KexiDBFactory::isPropertyVisibleInternal(const Q3CString& classname, QWidget *w,
        const Q3CString& property, bool isTopLevel)
{
    //general
    if (property == "dataSource" || property == "dataSourcePartClass") {
        return false; //force
    }

    bool ok = true;

    if (classname == "KexiPushButton") {
        ok = property != "isDragEnabled"
#ifdef KEXI_NO_UNFINISHED
             && property != "onClickAction" /*! @todo reenable */
             && property != "onClickActionOption" /*! @todo reenable */
             && property != "iconSet" /*! @todo reenable */
             && property != "stdItem" /*! @todo reenable stdItem */
#endif
             ;
    } else if (classname == "KexiDBLineEdit")
        ok = property != "urlDropsEnabled"
             && property != "vAlign"
#ifdef KEXI_NO_UNFINISHED
             && property != "inputMask"
             && property != "maxLength" //!< we may want to integrate this with db schema
#endif
             ;
    else if (classname == "KexiDBComboBox")
        ok = property != "autoCaption"
             && property != "labelPosition"
             && property != "widgetType"
             && property != "fieldTypeInternal"
             && property != "fieldCaptionInternal"; //hide properties that come with KexiDBAutoField
    else if (classname == "KexiDBTextEdit")
        ok = property != "undoDepth"
             && property != "undoRedoEnabled" //always true!
             && property != "dragAutoScroll" //always true!
             && property != "overwriteMode" //always false!
             && property != "resizePolicy"
             && property != "autoFormatting" //too complex
#ifdef KEXI_NO_UNFINISHED
             && property != "paper"
#endif
             ;
    else if (classname == "KexiDBSubForm")
        ok = property != "dragAutoScroll"
             && property != "resizePolicy"
             && property != "focusPolicy";
    else if (classname == "KexiDBForm")
        ok = property != "iconText"
             && property != "geometry" /*nonsense for toplevel widget; for size, "size" property is used*/;
    else if (classname == "KexiDBLabel")
        ok = property != "focusPolicy";
    else if (classname == "KexiDBAutoField") {
        if (!isTopLevel && property == "caption")
            return true; //force
        if (property == "fieldTypeInternal" || property == "fieldCaptionInternal"
//! @todo unhide in 2.0
                || property == "widgetType")
            return false;
        ok = property != "text"; /* "text" is not needed as "caption" is used instead */
    } else if (classname == "KexiDBImageBox") {
        ok = property != "font" && property != "wordbreak";
    } else if (classname == "KexiDBCheckBox") {
        //hide text property if the widget is a child of an autofield beause there's already "caption" for this purpose
        if (property == "text" && w && dynamic_cast<KFormDesigner::WidgetWithSubpropertiesInterface*>(w->parentWidget()))
            return false;
        ok = property != "autoRepeat";
    }

    return ok && WidgetFactory::isPropertyVisibleInternal(classname, w, property, isTopLevel);
}

bool
KexiDBFactory::propertySetShouldBeReloadedAfterPropertyChange(const Q3CString& classname,
        QWidget *w, const Q3CString& property)
{
    Q_UNUSED(classname);
    Q_UNUSED(w);
    if (property == "fieldTypeInternal" || property == "widgetType")
        return true;
    return false;
}

bool
KexiDBFactory::changeText(const QString &text)
{
    KFormDesigner::Form *form = m_container ? m_container->form() : 0;
    if (!form)
        return false;
    if (!form->selectedWidget())
        return false;
    Q3CString n(form->selectedWidget()->metaObject()->className());
// QWidget *w = WidgetFactory::widget();
    if (n == "KexiDBAutoField") {
        changeProperty("caption", text, form);
        return true;
    }
    //! \todo check field's geometry
    return false;
}

void
KexiDBFactory::resizeEditor(QWidget *editor, QWidget *w, const Q3CString &classname)
{
    //QSize s = widget->size();
    //QPoint p = widget->pos();

    if (classname == "KexiDBAutoField")
        editor->setGeometry(static_cast<KexiDBAutoField*>(w)->label()->geometry());
}

void
KexiDBFactory::slotImageBoxIdChanged(KexiBLOBBuffer::Id_t id)
{
//old KexiFormView *formView = KexiUtils::findParent<KexiFormView>((QWidget*)m_widget, "KexiFormView");

    // (js) heh, porting to KFormDesigner::FormManager::self() singleton took me entire day of work...
    KFormDesigner::Form *form = KFormDesigner::FormManager::self()->activeForm();
    KexiFormView *formView = form
                             ? KexiUtils::findParent<KexiFormView*>((QWidget*)form->widget()) : 0;
    if (formView) {
        changeProperty("pixmapId", (uint)/*! @todo unsafe */id, form);
//old  formView->setUnsavedLocalBLOB(m_widget, id);
        formView->setUnsavedLocalBLOB(form->selectedWidget(), id);
    }
}

KFORMDESIGNER_WIDGET_FACTORY(KexiDBFactory, kexidbwidgets)

#include "kexidbfactory.moc"
