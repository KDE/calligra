/* This file is part of the KDE project
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

#include <q3scrollview.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qstyle.h>

#include <KLocale>
#include <KDebug>
#include <KIconLoader>
#include <KActionCollection>
#include <KStandardAction>
#include <KPluginFactory>

#include <formeditor/container.h>
#include <formeditor/form.h>
#include <formeditor/formIO.h>
#include <formeditor/objecttree.h>
#include <formeditor/utils.h>
#include <formeditor/widgetlibrary.h>
#include <core/kexi.h>
#include <core/kexipart.h>
#include <core/KexiMainWindowIface.h>
#include <kexidb/utils.h>
#include <kexidb/connection.h>
#include <kexiutils/utils.h>
#include <widget/kexicustompropertyfactory.h>
#include <widget/utils/kexicontextmenuutils.h>
#include <kexi_global.h>

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
#ifndef KEXI_NO_SUBFORM
# include "widgets/kexidbsubform.h"
#endif
#include "kexidataawarewidgetinfo.h"

#include "kexidbfactory.h"
#include <widget/dataviewcommon/kexiformdataiteminterface.h>


//////////////////////////////////////////

KexiDBFactory::KexiDBFactory(QObject *parent, const QVariantList &)
        : KFormDesigner::WidgetFactory(parent, "kexidb")
        , m_assignAction(0)
{
    {
        KexiDataAwareWidgetInfo *wi = new KexiDataAwareWidgetInfo(this);
        wi->setPixmap("form");
        wi->setClassName("KexiDBForm");
        wi->setName(i18n("Form"));
        wi->setNamePrefix(
            i18nc("Widget name. This string will be used to name widgets of this class. "
                  "It must _not_ contain white spaces and non latin1 characters.", "form"));
        wi->setDescription(i18n("A data-aware form widget"));
        addClass(wi);
    }

#ifndef KEXI_NO_SUBFORM
    {
        KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
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
    }
#endif

    {
        // inherited
        KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
        wi->setPixmap("lineedit");
        wi->setClassName("KexiDBLineEdit");
        wi->setParentFactoryName("stdwidgets");
        wi->setInheritedClassName("KLineEdit");
        wi->addAlternateClassName("QLineEdit", true/*override*/);
        wi->addAlternateClassName("KLineEdit", true/*override*/);
        wi->setIncludeFileName("klineedit.h");
        wi->setName(i18n("Text Box"));
        wi->setNamePrefix(
            i18nc("Widget name. This string will be used to name widgets of this class. "
                  "It must _not_ contain white spaces and non latin1 characters.", "textBox"));
        wi->setDescription(i18n("A widget for entering and displaying text"));
        wi->setInternalProperty("dontStartEditingOnInserting", true); // because we are most probably assign data source to this widget
        wi->setInlineEditingEnabledWhenDataSourceSet(false);
        addClass(wi);
    }
    {
        // inherited
        KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
        wi->setPixmap("textedit");
        wi->setClassName("KexiDBTextEdit");
        wi->setParentFactoryName("stdwidgets");
        wi->setInheritedClassName("KTextEdit");
        wi->addAlternateClassName("QTextEdit", true/*override*/);
        wi->addAlternateClassName("KTextEdit", true/*override*/);
        wi->setIncludeFileName("ktextedit.h");
        wi->setName(i18n("Text Editor"));
        wi->setNamePrefix(
            i18nc("Widget name. This string will be used to name widgets of this class. "
                  "It must _not_ contain white spaces and non latin1 characters.", "textEditor"));
        wi->setDescription(i18n("A multiline text editor"));
        wi->setInternalProperty("dontStartEditingOnInserting", true); // because we are most probably assign data source to this widget
        wi->setInlineEditingEnabledWhenDataSourceSet(false);
        addClass(wi);
    }
    {
        KFormDesigner::WidgetInfo* wi = new KFormDesigner::WidgetInfo(this);
        wi->setPixmap("frame");
        wi->setClassName("KexiFrame");
        wi->setParentFactoryName("containers");
        wi->setInheritedClassName("QFrame"); /* we are inheriting to get i18n'd strings already translated there */
        wi->addAlternateClassName("QFrame", true/*override*/);
        wi->addAlternateClassName("Q3Frame", true/*override*/);
        wi->setName(i18n("Frame"));
        wi->setNamePrefix(
            i18nc("Widget name. This string will be used to name widgets of this class. "
                  "It must _not_ contain white spaces and non latin1 characters.", "frame"));
        wi->setDescription(i18n("A simple frame widget"));
        addClass(wi);
    }
    {
        KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
        wi->setPixmap("label");
        wi->setClassName("KexiDBLabel");
        wi->setParentFactoryName("stdwidgets");
        wi->setInheritedClassName("QLabel"); /* we are inheriting to get i18n'd strings already translated there */
        wi->addAlternateClassName("QLabel", true/*override*/);
        wi->addAlternateClassName("KexiLabel", true/*override*/); //older
        wi->setName(i18nc("Text Label", "Label"));
        wi->setNamePrefix(
            i18nc("Widget name. This string will be used to name widgets of this class. "
                  "It must _not_ contain white spaces and non latin1 characters.", "label"));
        wi->setDescription(i18n("A widget for displaying text"));
        wi->setInlineEditingEnabledWhenDataSourceSet(false);
        addClass(wi);
    }

#ifndef KEXI_NO_IMAGEBOX_WIDGET
    {
        KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
        wi->setPixmap("pixmaplabel");
        wi->setClassName("KexiDBImageBox");
        wi->setParentFactoryName("stdwidgets");
        wi->setInheritedClassName("KexiPictureLabel"); /* we are inheriting to get i18n'd strings already translated there */
        wi->addAlternateClassName("KexiPictureLabel", true/*override*/);
        wi->addAlternateClassName("KexiImageBox", true/*override*/); //older
        wi->setName(i18n("Image Box"));
        wi->setNamePrefix(
            i18nc("Widget name. This string will be used to name widgets of this class. "
                  "It must _not_ contain white spaces and non latin1 characters.", "image"));
        wi->setDescription(i18n("A widget for displaying images"));
    // wi->setCustomTypeForProperty("pixmapData", KexiCustomPropertyFactory::PixmapData);
        wi->setCustomTypeForProperty("pixmapId", KexiCustomPropertyFactory::PixmapId);
        wi->setInternalProperty("dontStartEditingOnInserting", true);
        addClass(wi);
    }
#endif

#ifdef KEXI_DB_COMBOBOX_WIDGET
    {
        KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
        wi->setPixmap("combo");
        wi->setClassName("KexiDBComboBox");
        wi->setParentFactoryName("stdwidgets");
        wi->setInheritedClassName("KComboBox"); /* we are inheriting to get i18n'd strings already translated there */
        wi->addAlternateClassName("KComboBox", true/*override*/);
        wi->setName(i18n("Combo Box"));
        wi->setNamePrefix(
            i18nc("Widget name. This string will be used to name widgets of this class. "
                  "It must _not_ contain white spaces and non latin1 characters.", "comboBox"));
        wi->setDescription(i18n("A combo box widget"));
        addClass(wi);
    }
#endif
    {
        KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
        wi->setPixmap("check");
        wi->setClassName("KexiDBCheckBox");
        wi->setParentFactoryName("stdwidgets");
        wi->setInheritedClassName("QCheckBox"); /* we are inheriting to get i18n'd strings already translated there */
        wi->addAlternateClassName("QCheckBox", true/*override*/);
        wi->setName(i18n("Check Box"));
        wi->setNamePrefix(
            i18nc("Widget name. This string will be used to name widgets of this class. "
                  "It must _not_ contain white spaces and non latin1 characters.", "checkBox"));
        wi->setDescription(i18n("A check box with text label"));
        addClass(wi);
    }
#ifndef KEXI_NO_AUTOFIELD_WIDGET
    {
        KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
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
    }
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

    {
        // inherited
        KFormDesigner::WidgetInfo* wi = new KFormDesigner::WidgetInfo(this);
        wi->addAlternateClassName("KexiPushButton");
        wi->setName(i18n("Button"));
        wi->setNamePrefix(
            i18nc("Widget name. This string will be used to name widgets of this class. "
                  "It must _not_ contain white spaces and non latin1 characters.", "button"));
        wi->setDescription(i18n("A button for executing actions"));
        wi->setParentFactoryName("stdwidgets");
        wi->setInheritedClassName("KPushButton");
        addClass(wi);
    }

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
    m_propDesc["smoothTransformation"] = i18nc("Smoothing when contents are scaled", "Smoothing");
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
KexiDBFactory::createWidget(const QByteArray &c, QWidget *p, const char *n,
                            KFormDesigner::Container *container,
                            CreateWidgetOptions options)
{
    kDebug() << this;

    QWidget *w = 0;
    QString text(container->form()->library()->textForWidgetName(n, c));
    const bool designMode = options & KFormDesigner::WidgetFactory::DesignViewMode;
    bool createContainer = false;

    if (c == "KexiDBLineEdit") {
        w = new KexiDBLineEdit(p);
//2.0 moved to FormWidgetInterface
//        if (designMode)
//            w->setCursor(QCursor(Qt::ArrowCursor));
    }
#ifndef KEXI_NO_SUBFORM
    if (c == "KexiDBSubForm") {
        w = new KexiDBSubForm(container->form(), p);
    }
#endif
    else if (c == "KexiDBTextEdit") {
        w = new KexiDBTextEdit(p);
//2.0 moved to FormWidgetInterface
//        if (designMode)
//            w->setCursor(QCursor(Qt::ArrowCursor));
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
        w = new KexiDBAutoField(p);
#endif
    else if (c == "KexiDBCheckBox")
        w = new KexiDBCheckBox(text, p);
    else if (c == "KexiDBComboBox")
        w = new KexiDBComboBox(p);
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

bool KexiDBFactory::createMenuActions(const QByteArray &classname, QWidget *w, QMenu *menu,
                                      KFormDesigner::Container *)
{
    if (m_assignAction->isEnabled()) {
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
KexiDBFactory::startInlineEditing(InlineEditorCreationArguments& args)
{
//2.0    m_container = container;
    const KFormDesigner::WidgetInfo* wclass = args.container->form()->library()->widgetInfoForClassName(args.classname);
    const KexiDataAwareWidgetInfo* wDataAwareClass = dynamic_cast<const KexiDataAwareWidgetInfo*>(wclass);
    if (wDataAwareClass && !wDataAwareClass->inlineEditingEnabledWhenDataSourceSet()) {
        KexiFormDataItemInterface* iface = dynamic_cast<KexiFormDataItemInterface*>(args.widget);
        if (iface && !iface->dataSource().isEmpty()) {
//! @todo reimplement inline editing for KexiDBLineEdit using combobox with data sources
            return false;
        }
    }

    if (args.classname == "KexiDBLineEdit") {
//! @todo this code should not be copied here but
//! just inherited StdWidgetFactory::startInlineEditing() should be called

        KLineEdit *lineedit = static_cast<KLineEdit*>(args.widget);
        args.text = lineedit->text();
        args.alignment = lineedit->alignment();
        args.useFrame = true;
        return true;
    }
    else if (args.classname == "KexiDBTextEdit") {
//! @todo this code should not be copied here but
//! just inherited StdWidgetFactory::startInlineEditing() should be called
        KTextEdit *textedit = static_cast<KTextEdit*>(args.widget);
//! @todo rich text?
        args.text = textedit->toPlainText();
        args.alignment = textedit->alignment();
        args.useFrame = true;
        args.multiLine = true;
//! @todo
#if 0 
        //copy a few properties
        KTextEdit *ed = dynamic_cast<KTextEdit *>(editor(w));
        ed->setLineWrapMode(textedit->lineWrapMode());
        ed->setLineWrapColumnOrWidth(textedit->lineWrapColumnOrWidth());
        ed->setWordWrapMode(textedit->wordWrapMode());
        ed->setTabStopWidth(textedit->tabStopWidth());
        ed->setTextFormat(textedit->textFormat());
        ed->setHorizontalScrollBarPolicy(textedit->horizontalScrollBarPolicy());
        ed->setVerticalScrollBarPolicy(textedit->verticalScrollBarPolicy());
#endif
        return true;
    }
    else if (args.classname == "KexiDBLabel") {
        KexiDBLabel *label = static_cast<KexiDBLabel*>(args.widget);
//2.0        m_widget = w;
        if (label->textFormat() == Qt::RichText) {
            args.execute = false;
//            QString text = label->text();
            if (wclass && wclass->inheritedClass()) {
                const QByteArray thisClassname = args.classname; //save
                args.classname = wclass->inheritedClass()->className();
//! @todo OK?
                const bool result = wclass->inheritedClass()->factory()->startInlineEditing(args);
                args.classname = thisClassname;
                return result;
            }
            else {
                return false;
            }
//-->               KFormDesigner::EditRichTextAction(args.container, label, 0, this).trigger();
//2.0 moved to EditRichTextAction:
//2.0          if (editRichText(label, text)) {
//2.0              changeProperty(args.container->form(), label, "textFormat", "RichText", );
//2.0              changeProperty("text", text, args.container->form());
//2.0            }
//2.0          if (args.classname == "KexiDBLabel")
//2.0              args.widget->resize(args.widget->sizeHint());
        }
        else {
            args.text = label->text();
            args.alignment = label->alignment();
            args.multiLine = label->wordWrap();
        }
        return true;
    }
#ifndef KEXI_NO_SUBFORM
    else if (args.classname == "KexiDBSubForm") {
//! @todo
        // open the form in design mode
        KexiDBSubForm *subform = static_cast<KexiDBSubForm*>(args.widget);
        args.execute = false;
        if (KexiMainWindowIface::global()) {
            bool openingCancelled;
            KexiMainWindowIface::global()->openObject(
                "org.kexi-project.form", subform->formName(), Kexi::DesignViewMode,
                openingCancelled);
        }
        return true;
    }
#endif
#if 0
    else if (   args.classname == "KexiDBDateEdit" || args.classname == "KexiDBDateTimeEdit"
             || args.classname == "KexiDBTimeEdit" /*|| classname == "KexiDBIntSpinBox" || classname == "KexiDBDoubleSpinBox"*/)
    {
        disableFilter(w, container);
        return true;
    }
#endif
    else if (args.classname == "KexiDBAutoField") {
        if (static_cast<KexiDBAutoField*>(args.widget)->hasAutoCaption())
            return false; // caption is auto, abort editing
        QLabel *label = static_cast<KexiDBAutoField*>(args.widget)->label();
        args.text = label->text();
        args.widget = label;
        args.geometry = label->geometry();
        args.alignment = label->alignment();
//2.0        createEditor(classname, label->text(), label, container, label, );
        return true;
    }
    else if (args.classname == "KexiDBCheckBox") {
        KexiDBCheckBox *cb = static_cast<KexiDBCheckBox*>(args.widget);
        QStyleOption option;
        option.initFrom(cb);
        QRect r(cb->geometry());
        r.setLeft(
            r.left() + 2
            + cb->style()->subElementRect(QStyle::SE_CheckBoxIndicator, &option, cb).width());
        args.text = cb->text();
        args.geometry = r;
//2.0        createEditor(classname, cb->text(), cb, container, r, Qt::AlignAuto);
        return true;
    }
    else if (args.classname == "KexiDBImageBox") {
        KexiDBImageBox *image = static_cast<KexiDBImageBox*>(args.widget);
        image->insertFromFile();
        args.execute = false;
        return true;
    }
//    else if (args.classname == "QPushButton" || args.classname == "KPushButton" || args.classname == "KexiPushButton") {
//    }
    return false;
}

bool
KexiDBFactory::previewWidget(const QByteArray &, QWidget *, KFormDesigner::Container *)
{
    return false;
}

bool
KexiDBFactory::clearWidgetContent(const QByteArray & /*classname*/, QWidget *w)
{
//! @todo this code should not be copied here but
//! just inherited StdWidgetFactory::clearWidgetContent() should be called
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(w);
    if (iface)
        iface->clear();
    return true;
}

bool
KexiDBFactory::isPropertyVisibleInternal(const QByteArray& classname, QWidget *w,
        const QByteArray& property, bool isTopLevel)
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
             && property != "iconSize" /*! @todo reenable */
             && property != "stdItem" /*! @todo reenable stdItem */
#endif
             ;
    } else if (classname == "KexiDBLineEdit")
        ok = property != "urlDropsEnabled"
             && property != "vAlign"
             && property != "echoMode"
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
             && property != "documentTitle"
             && property != "cursorWidth"
#ifdef KEXI_NO_UNFINISHED
             && property != "paper"
#endif
             && property != "textInteractionFlags"
//! @todo support textInteractionFlags property of QLabel and QTextEdit
             ;
#ifndef KEXI_NO_SUBFORM
    else if (classname == "KexiDBSubForm")
        ok = property != "dragAutoScroll"
             && property != "resizePolicy"
             && property != "focusPolicy";
#endif
    else if (classname == "KexiDBForm")
        ok = property != "iconText"
             && property != "geometry" /*nonsense for toplevel widget; for size, "size" property is used*/;
    else if (classname == "KexiDBLabel")
        ok = property != "focusPolicy"
             && property != "textInteractionFlags";
//! @todo support textInteractionFlags property of QLabel
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
KexiDBFactory::propertySetShouldBeReloadedAfterPropertyChange(const QByteArray& classname,
        QWidget *w, const QByteArray& property)
{
    Q_UNUSED(classname);
    Q_UNUSED(w);
    if (property == "fieldTypeInternal" || property == "widgetType")
        return true;
    return false;
}

bool KexiDBFactory::changeInlineText(KFormDesigner::Form *form, QWidget *widget,
    const QString &text, QString &oldText)
{
//2.0    if (!form)
//2.0        return false;
//2.0    if (!form->selectedWidget())
//2.0        return false;
//2.0    QByteArray n(form->selectedWidget()->metaObject()->className());
    const QByteArray n(widget->metaObject()->className());
// QWidget *w = WidgetFactory::widget();
    if (n == "KexiDBAutoField") {
        oldText = widget->property("caption").toString();
        changeProperty(form, widget, "caption", text);
        return true;
    }
//! @todo check field's geometry
    return false;
}

void
KexiDBFactory::resizeEditor(QWidget *editor, QWidget *w, const QByteArray &classname)
{
    //QSize s = widget->size();
    //QPoint p = widget->pos();

    if (classname == "KexiDBAutoField")
        editor->setGeometry(static_cast<KexiDBAutoField*>(w)->label()->geometry());
}

void
KexiDBFactory::slotImageBoxIdChanged(KexiBLOBBuffer::Id_t id)
{
    KexiFormView *formView = KexiUtils::findParent<KexiFormView*>((QWidget*)sender());
    if (formView) {
        changeProperty(formView->form(), formView, "pixmapId", (uint)/*! @todo unsafe */id);
        formView->setUnsavedLocalBLOB(formView->form()->selectedWidget(), id);
    }
}

K_EXPORT_KEXI_FORM_WIDGET_FACTORY_PLUGIN(KexiDBFactory, kexidbwidgets)

#include "kexidbfactory.moc"
