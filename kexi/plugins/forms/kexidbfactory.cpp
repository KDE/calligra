/* This file is part of the KDE project
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

#include "kexidbfactory.h"
#include <KexiIcon.h>
#include <formeditor/container.h>
#include <formeditor/form.h>
#include <formeditor/formIO.h>
#include <formeditor/objecttree.h>
#include <formeditor/utils.h>
#include <formeditor/widgetlibrary.h>
#include <core/kexi.h>
#include <core/kexipart.h>
#include <core/KexiMainWindowIface.h>
#include <kexiutils/utils.h>
#include <kexiutils/KexiCommandLinkButton.h>
#include <widget/properties/KexiCustomPropertyFactory.h>
#include <widget/utils/kexicontextmenuutils.h>
#include <kexi_global.h>
#include "kexiformview.h"
#include "widgets/kexidbautofield.h"
#include "widgets/kexidbcheckbox.h"
#include "widgets/kexidbimagebox.h"
#include "widgets/kexiframe.h"
#include "widgets/kexidblabel.h"
#include "widgets/kexidblineedit.h"
#include "widgets/kexidbtextedit.h"
#include "widgets/kexidbcombobox.h"
#include "widgets/KexiDBPushButton.h"
#include "widgets/kexidbform.h"
#include "widgets/kexidbcommandlinkbutton.h"
#include "widgets/kexidbslider.h"
#include "widgets/kexidbprogressbar.h"
#include "widgets/kexidbdatepicker.h"
#ifndef KEXI_NO_SUBFORM
# include "widgets/kexidbsubform.h"
#endif
#include "kexidataawarewidgetinfo.h"
#include <widget/dataviewcommon/kexiformdataiteminterface.h>

#include <KDbUtils>
#include <KDbConnection>

#include <KActionCollection>
#include <KLocalizedString>

#include <QStyle>
#include <QFontMetrics>
//////////////////////////////////////////

KexiDBFactory::KexiDBFactory(QObject *parent, const QVariantList &)
        : KexiDBFactoryBase(parent, "kexidb")
        , m_assignAction(0)
{
    {
        KexiDataAwareWidgetInfo *wi = new KexiDataAwareWidgetInfo(this);
        wi->setIconName(koIconName("form"));
        wi->setClassName("KexiDBForm");
        wi->setName(xi18nc("Form widget", "Form"));
        wi->setNamePrefix(
            xi18nc("A prefix for identifiers of forms. Based on that, identifiers such as "
                "form1, form2 are generated. "
                "This string can be used to refer the widget object as variables in programming "
                "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
                "should start with lower case letter and if there are subsequent words, these should "
                "start with upper case letter. Example: smallCamelCase. "
                "Moreover, try to make this prefix as short as possible.",
                "form"));
        wi->setDescription(xi18n("A form widget"));
        addClass(wi);
    }

#ifndef KEXI_NO_SUBFORM
    {
// Unused, commented-out in Kexi 2.9 to avoid unnecessary translations:
//         KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
//         wi->setIconName(koIconName("subform"));
//         wi->setClassName("KexiDBSubForm");
//         wi->addAlternateClassName("KexiSubForm", true/*override*/); //older
//         wi->setName(xi18nc("Sub Form widget", "Sub Form"));
//         wi->setNamePrefix(
//             i18nc("Widget name. This string will be used to name widgets of this class. "
//                   "It must _not_ contain white spaces and non latin1 characters.", "subForm"));
//         wi->setDescription(xi18n("A form widget included in another Form"));
//         wi->setAutoSyncForProperty("formName", false);
//         addClass(wi);
    }
#endif

    {
        // inherited
        KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
        wi->setIconName(koIconName("lineedit"));
        wi->setClassName("KexiDBLineEdit");
        wi->setParentFactoryName("stdwidgets");
        wi->setInheritedClassName("QLineEdit");
        wi->addAlternateClassName("QLineEdit", true/*override*/);
        wi->addAlternateClassName("QLineEdit", true/*override*/);
        wi->setIncludeFileName("qlineedit.h");
        wi->setName(xi18nc("Text Box widget", "Text Box"));
        wi->setNamePrefix(
            xi18nc("A prefix for identifiers of text box widgets. Based on that, identifiers such as "
                "textBox1, textBox2 are generated. "
                "This string can be used to refer the widget object as variables in programming "
                "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
                "should start with lower case letter and if there are subsequent words, these should "
                "start with upper case letter. Example: smallCamelCase. "
                "Moreover, try to make this prefix as short as possible.",
                "textBox"));
        wi->setDescription(xi18n("A widget for entering and displaying line of text text"));
        wi->setInternalProperty("dontStartEditingOnInserting", true); // because we are most probably assign data source to this widget
        wi->setInlineEditingEnabledWhenDataSourceSet(false);
        addClass(wi);
    }
    {
        // inherited
        KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
        wi->setIconName(koIconName("textedit"));
        wi->setClassName("KexiDBTextEdit");
        wi->setParentFactoryName("stdwidgets");
        wi->setInheritedClassName("KTextEdit");
        wi->addAlternateClassName("QTextEdit", true/*override*/);
        wi->addAlternateClassName("KTextEdit", true/*override*/);
        wi->setIncludeFileName("ktextedit.h");
        wi->setName(xi18nc("Text Editor widget", "Text Editor"));
        wi->setNamePrefix(
            xi18nc("A prefix for identifiers of text editor widgets. Based on that, identifiers such as "
                "textEditor1, textEditor2 are generated. "
                "This string can be used to refer the widget object as variables in programming "
                "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
                "should start with lower case letter and if there are subsequent words, these should "
                "start with upper case letter. Example: smallCamelCase. "
                "Moreover, try to make this prefix as short as possible.",
                "textEditor"));
        wi->setDescription(xi18n("A multiline text editor"));
        wi->setInternalProperty("dontStartEditingOnInserting", true); // because we are most probably assign data source to this widget
        wi->setInlineEditingEnabledWhenDataSourceSet(false);
        addClass(wi);
    }
    {
        KFormDesigner::WidgetInfo* wi = new KFormDesigner::WidgetInfo(this);
        wi->setIconName(koIconName("frame"));
        wi->setClassName("KexiFrame");
        wi->setParentFactoryName("containers");
        wi->setInheritedClassName("QFrame"); /* we are inheriting to get i18n'd strings already translated there */
        wi->addAlternateClassName("QFrame", true/*override*/);
        wi->addAlternateClassName("Q3Frame", true/*override*/);
        wi->setName(xi18nc("Frame widget", "Frame"));
        wi->setNamePrefix(
            xi18nc("A prefix for identifiers of frame widgets. Based on that, identifiers such as "
                "frame1, frame2 are generated. "
                "This string can be used to refer the widget object as variables in programming "
                "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
                "should start with lower case letter and if there are subsequent words, these should "
                "start with upper case letter. Example: smallCamelCase. "
                "Moreover, try to make this prefix as short as possible.",
                "frame"));
        wi->setDescription(xi18n("A frame widget"));
        addClass(wi);
    }
    {
        KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
        wi->setIconName(koIconName("label"));
        wi->setClassName("KexiDBLabel");
        wi->setParentFactoryName("stdwidgets");
        wi->setInheritedClassName("QLabel"); /* we are inheriting to get i18n'd strings already translated there */
        wi->addAlternateClassName("QLabel", true/*override*/);
        wi->addAlternateClassName("KexiLabel", true/*override*/); //older
        wi->setName(xi18nc("Text Label widget", "Label"));
        wi->setNamePrefix(
            xi18nc("A prefix for identifiers of label widgets. Based on that, identifiers such as "
                "label1, label2 are generated. "
                "This string can be used to refer the widget object as variables in programming "
                "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
                "should start with lower case letter and if there are subsequent words, these should "
                "start with upper case letter. Example: smallCamelCase. "
                "Moreover, try to make this prefix as short as possible.",
                "label"));
        wi->setDescription(xi18n("A widget for displaying text"));
        wi->setInlineEditingEnabledWhenDataSourceSet(false);
        addClass(wi);
    }

    {
        KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
        wi->setIconName(koIconName("pixmaplabel"));
        wi->setClassName("KexiDBImageBox");
        wi->setParentFactoryName("stdwidgets");
        wi->setInheritedClassName("KexiPictureLabel"); /* we are inheriting to get i18n'd strings already translated there */
        wi->addAlternateClassName("KexiPictureLabel", true/*override*/);
        wi->addAlternateClassName("KexiImageBox", true/*override*/); //older
        wi->setName(xi18nc("Image Box widget", "Image Box"));
        wi->setNamePrefix(
            xi18nc("A prefix for identifiers of image box widgets. Based on that, identifiers such as "
                "image1, image2 are generated. "
                "This string can be used to refer the widget object as variables in programming "
                "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
                "should start with lower case letter and if there are subsequent words, these should "
                "start with upper case letter. Example: smallCamelCase. "
                "Moreover, try to make this prefix as short as possible.",
                "image"));
        wi->setDescription(xi18n("A widget for displaying images"));
    // wi->setCustomTypeForProperty("pixmapData", KexiCustomPropertyFactory::PixmapData);
        wi->setCustomTypeForProperty("pixmapId", KexiCustomPropertyFactory::PixmapId);
        wi->setInternalProperty("dontStartEditingOnInserting", true);
        addClass(wi);
    }

    {
        KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
        wi->setIconName(koIconName("combo"));
        wi->setClassName("KexiDBComboBox");
        wi->setParentFactoryName("stdwidgets");
        wi->setInheritedClassName("KComboBox"); /* we are inheriting to get i18n'd strings already translated there */
        wi->addAlternateClassName("KComboBox", true/*override*/);
        wi->setName(xi18nc("Combo Box widget", "Combo Box"));
        wi->setNamePrefix(
            xi18nc("A prefix for identifiers of combo box widgets. Based on that, identifiers such as "
                "comboBox1, comboBox2 are generated. "
                "This string can be used to refer the widget object as variables in programming "
                "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
                "should start with lower case letter and if there are subsequent words, these should "
                "start with upper case letter. Example: smallCamelCase. "
                "Moreover, try to make this prefix as short as possible.",
                "comboBox"));
        wi->setDescription(xi18n("A combo box widget"));
        addClass(wi);
    }
    {
        KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
        wi->setIconName(koIconName("check"));
        wi->setClassName("KexiDBCheckBox");
        wi->setParentFactoryName("stdwidgets");
        wi->setInheritedClassName("QCheckBox"); /* we are inheriting to get i18n'd strings already translated there */
        wi->addAlternateClassName("QCheckBox", true/*override*/);
        wi->setName(xi18nc("Check Box widget", "Check Box"));
        wi->setNamePrefix(
            xi18nc("A prefix for identifiers of combo box widgets. Based on that, identifiers such as "
                "checkBox1, checkBox2 are generated. "
                "This string can be used to refer the widget object as variables in programming "
                "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
                "should start with lower case letter and if there are subsequent words, these should "
                "start with upper case letter. Example: smallCamelCase. "
                "Moreover, try to make this prefix as short as possible.",
                "checkBox"));
        wi->setDescription(xi18n("A check box with text label"));
        addClass(wi);
    }
#ifndef KEXI_NO_AUTOFIELD_WIDGET
    {
// Unused, commented-out in Kexi 2.9 to avoid unnecessary translations:
//         KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
//         wi->setIconName(koIconName("autofield"));
//         wi->setClassName("KexiDBAutoField");
//         wi->addAlternateClassName("KexiDBFieldEdit", true/*override*/); //older
//         wi->setName(xi18n("Auto Field"));
//         wi->setNamePrefix(
//             i18nc("Widget name. This string will be used to name widgets of this class. "
//                   "It must _not_ contain white spaces and non latin1 characters", "autoField"));
//         wi->setDescription(xi18n("A widget containing an automatically selected editor "
//                                 "and a label to edit the value of a database field of any type."));
//         addClass(wi);
    }
#endif

    {
        // inherited
        KFormDesigner::WidgetInfo* wi = new KFormDesigner::WidgetInfo(this);
        wi->addAlternateClassName("KexiDBPushButton");
        wi->addAlternateClassName("KexiPushButton");
        wi->setName(xi18nc("Button widget", "Button"));
        wi->setNamePrefix(
            xi18nc("A prefix for identifiers of button widgets. Based on that, identifiers such as "
                "button1, button2 are generated. "
                "This string can be used to refer the widget object as variables in programming "
                "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
                "should start with lower case letter and if there are subsequent words, these should "
                "start with upper case letter. Example: smallCamelCase. "
                "Moreover, try to make this prefix as short as possible.",
                "button"));
        wi->setDescription(xi18n("A button for executing actions"));
        wi->setParentFactoryName("stdwidgets");
        wi->setInheritedClassName("QPushButton");
        addClass(wi);
    }
    {
        KFormDesigner::WidgetInfo* wi = new KFormDesigner::WidgetInfo(this);
        wi->setClassName("KexiDBCommandLinkButton");
        wi->setIconName(koIconName("button"));
        wi->setName(xi18nc("Link Button widget", "Link Button"));
        wi->setNamePrefix(
            xi18nc("A prefix for identifiers of link button widgets. Based on that, identifiers such as "
                "linkButton1, linkButton2 are generated. "
                "This string can be used to refer the widget object as variables in programming "
                "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
                "should start with lower case letter and if there are subsequent words, these should "
                "start with upper case letter. Example: smallCamelCase. "
                "Moreover, try to make this prefix as short as possible.",
                "linkButton"));
        wi->setDescription(xi18n("A Link button for executing actions"));
        addClass(wi);
    }
    {
        KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
        wi->setIconName(koIconName("slider"));
        wi->setClassName("KexiDBSlider");
        wi->setName(xi18nc("Slider widget", "Slider"));
        wi->setNamePrefix(
            xi18nc("A prefix for identifiers of slider widgets. Based on that, identifiers such as "
                "slider1, slider2 are generated. "
                "This string can be used to refer the widget object as variables in programming "
                "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
                "should start with lower case letter and if there are subsequent words, these should "
                "start with upper case letter. Example: smallCamelCase. "
                "Moreover, try to make this prefix as short as possible.",
                "slider"));
        wi->setDescription(xi18n("A Slider widget"));
        addClass(wi);
    }
    {
        KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
        wi->setIconName(koIconName("progress"));
        wi->setClassName("KexiDBProgressBar");
        wi->setName(xi18nc("Progress Bar widget", "Progress Bar"));
        wi->setNamePrefix(
            xi18nc("A prefix for identifiers of progress bar widgets. Based on that, identifiers such as "
                "progressBar1, progressBar2 are generated. "
                "This string can be used to refer the widget object as variables in programming "
                "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
                "should start with lower case letter and if there are subsequent words, these should "
                "start with upper case letter. Example: smallCamelCase. "
                "Moreover, try to make this prefix as short as possible.",
                "progressBar"));
        wi->setDescription(xi18n("A Progress Bar widget"));
        addClass(wi);
    }
    {
        KexiDataAwareWidgetInfo* wi = new KexiDataAwareWidgetInfo(this);
        wi->setIconName(koIconName("dateedit"));
        wi->setClassName("KexiDBDatePicker");
        wi->setName(xi18nc("Date Picker widget", "Date Picker"));
        wi->setNamePrefix(
            xi18nc("A prefix for identifiers of date picker widgets. Based on that, identifiers such as "
                "datePicker1, datePicker2 are generated. "
                "This string can be used to refer the widget object as variables in programming "
                "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
                "should start with lower case letter and if there are subsequent words, these should "
                "start with upper case letter. Example: smallCamelCase. "
                "Moreover, try to make this prefix as short as possible.",
                "datePicker"));
        wi->setDescription(xi18n("A Date Picker widget"));
        addClass(wi);
    }

    setPropertyDescription("invertedAppearance", xi18n("Inverted"));
    setPropertyDescription("minimum", xi18n("Minimum"));
    setPropertyDescription("maximum", xi18n("Maximum"));
    setPropertyDescription("format", xi18n("Format"));
    setPropertyDescription("orientation", xi18n("Orientation"));
    setPropertyDescription("textDirection", xi18n("Text Direction"));
    setPropertyDescription("textVisible", xi18n("Text Visible"));
    setPropertyDescription("value", xi18n("Value"));
    setPropertyDescription("date", xi18n("Date"));
    setPropertyDescription("arrowVisible", xi18n("Arrow Visible"));
    setPropertyDescription("description", xi18n("Description"));
    setPropertyDescription("pageStep", xi18nc("Property of slider widgets", "Page Step"));
    setPropertyDescription("singleStep", xi18nc("Property of slider widgets", "Single Step"));
    setPropertyDescription("tickInterval", xi18nc("Property of slider widgets", "Tick Interval"));
    setPropertyDescription("tickPosition", xi18nc("Property of slider widgets", "Tick Position"));
    setPropertyDescription("showEditor", xi18n("Show Editor"));
    setPropertyDescription("formName", xi18n("Form Name"));
    setPropertyDescription("onClickAction", xi18n("On Click"));
    setPropertyDescription("onClickActionOption", xi18n("On Click Option"));
    setPropertyDescription("autoTabStops", xi18n("Auto Tab Order"));
    setPropertyDescription("checkSpellingEnabled", xi18n("Spell Checking"));
    setPropertyDescription("html", xi18nc("Widget Property", "HTML"));
    setPropertyDescription("lineWrapColumnOrWidth", xi18n("Line Wrap At"));
    setPropertyDescription("lineWrapMode", xi18n("Line Wrap Mode"));
    setPropertyDescription("spellCheckingLanguage", xi18n("Spell Checking Language"));

    setPropertyDescription("widgetType", xi18n("Editor Type"));
#ifndef KEXI_NO_AUTOFIELD_WIDGET
    //for autofield's type: inherit i18n from KexiDB
    setValueDescription("Auto", futureI18nc("AutoField editor's type", "Auto"));
    setValueDescription("Text", KDbField::typeName(KDbField::Text));
    setValueDescription("Integer", KDbField::typeName(KDbField::Integer));
    setValueDescription("Double", KDbField::typeName(KDbField::Double));
    setValueDescription("Boolean", KDbField::typeName(KDbField::Boolean));
    setValueDescription("Date", KDbField::typeName(KDbField::Date));
    setValueDescription("Time", KDbField::typeName(KDbField::Time));
    setValueDescription("DateTime", KDbField::typeName(KDbField::DateTime));
    setValueDescription("MultiLineText", xi18nc("AutoField editor's type", "Multiline Text"));
    setValueDescription("ComboBox", xi18nc("AutoField editor's type", "Drop-Down List"));
    setValueDescription("Image", xi18nc("AutoField editor's type", "Image"));
#endif

    setValueDescription("NoTicks", xi18nc("Possible value of slider widget's \"Tick position\" property", "No Ticks"));
    setValueDescription("TicksAbove", xi18nc("Possible value of slider widget's \"Tick position\" property", "Above"));
    setValueDescription("TicksLeft", xi18nc("Possible value of slider widget's \"Tick position\" property", "Left"));
    setValueDescription("TicksBelow", xi18nc("Possible value of slider widget's \"Tick position\" property", "Below"));
    setValueDescription("TicksRight", xi18nc("Possible value of slider widget's \"Tick position\" property", "Right"));
    setValueDescription("TicksBothSides", xi18nc("Possible value of slider widget's \"Tick position\" property", "Both Sides"));

// auto field:
//    setPropertyDescription("autoCaption", futureI18n("Auto Label"));
//    setPropertyDescription("foregroundLabelColor", futureI18n("Label Text Color"));
//    setPropertyDescription("backgroundLabelColor", futureI18nc("(a property name, keep the text narrow!)",
//                                         "Label Background\nColor"));

//    setPropertyDescription("labelPosition", futureI18n("Label Position"));
//    setValueDescription("Left", futureI18nc("Label Position", "Left"));
//    setValueDescription("Top", futureI18nc("Label Position", "Top"));
//    setValueDescription("NoLabel", futureI18nc("Label Position", "No Label"));

    setPropertyDescription("sizeInternal", xi18n("Size"));
    setPropertyDescription("pixmapId", xi18n("Image"));
    setPropertyDescription("scaledContents", xi18n("Scaled Contents"));
    setPropertyDescription("smoothTransformation", xi18nc("Property: Smoothing when contents are scaled", "Smoothing"));
    setPropertyDescription("keepAspectRatio", xi18nc("Property: Keep Aspect Ratio (keep short)", "Keep Ratio"));

    //hide classes that are replaced by db-aware versions
    hideClass("KexiPictureLabel");
    hideClass("KComboBox");

    //used in labels, frames...
    setPropertyDescription("dropDownButtonVisible",
        xi18nc("Drop-Down Button for Image Box Visible (a property name, keep the text narrow!)",
              "Drop-Down\nButton Visible"));

    //for checkbox
    setValueDescription("TristateDefault", xi18nc("Value of \"Tristate\" property in checkbox: default", "Default"));
    setValueDescription("TristateOn", xi18nc("Value of \"Tristate\" property in checkbox: yes", "Yes"));
    setValueDescription("TristateOff", xi18nc("Value of \"Tristate\" property in checkbox: no", "No"));

    //for combobox
    setPropertyDescription("editable", xi18nc("Editable combobox", "Editable"));

    //for kexipushbutton
    setPropertyDescription("hyperlink" , xi18nc("Hyperlink address", "Hyperlink"));
    setPropertyDescription("hyperlinkType", xi18nc("Type of hyperlink", "Hyperlink Type"));
    setPropertyDescription("hyperlinkTool", xi18nc("Tool used for opening a hyperlink", "Hyperlink Tool"));
    setPropertyDescription("remoteHyperlink", xi18nc("Allow to open remote hyperlinks", "Remote Hyperlink"));
    setPropertyDescription("hyperlinkExecutable", xi18nc("Allow to open executables", "Executable Hyperlink"));

    setValueDescription("NoHyperlink", xi18nc("Hyperlink type, NoHyperlink", "No Hyperlink"));
    setValueDescription("StaticHyperlink", xi18nc("Hyperlink type, StaticHyperlink", "Static"));
    setValueDescription("DynamicHyperlink", xi18nc("Hyperlink type, DynamicHyperlink", "Dynamic"));

    setValueDescription("DefaultHyperlinkTool", xi18nc("Hyperlink tool, DefaultTool", "Default"));
    setValueDescription("BrowserHyperlinkTool", xi18nc("Hyperlink tool, BrowserTool", "Browser"));
    setValueDescription("MailerHyperlinkTool", xi18nc("Hyperlink tool, MailerTool", "Mailer"));
}

KexiDBFactory::~KexiDBFactory()
{
}

QWidget*
KexiDBFactory::createWidget(const QByteArray &c, QWidget *p, const char *n,
                            KFormDesigner::Container *container,
                            CreateWidgetOptions options)
{
    QWidget *w = 0;
    QString text(container->form()->library()->textForWidgetName(n, c));
    const bool designMode = options & KFormDesigner::WidgetFactory::DesignViewMode;
    bool createContainer = false;

    if (c == "KexiDBLineEdit") {
        w = new KexiDBLineEdit(p);
    }
#ifndef KEXI_NO_SUBFORM
    if (c == "KexiDBSubForm") {
        w = new KexiDBSubForm(container->form(), p);
    }
#endif
    else if (c == "KexiDBTextEdit") {
        w = new KexiDBTextEdit(p);
    }
    else if (c == "Q3Frame" || c == "QFrame" || c == "KexiFrame") {
        w = new KexiFrame(p);
        createContainer = true;
    } else if (c == "KexiDBLabel") {
        w = new KexiDBLabel(text, p);
    }
    else if (c == "KexiDBImageBox") {
        w = new KexiDBImageBox(designMode, p);
        connect(w, SIGNAL(idChanged(long)), this, SLOT(slotImageBoxIdChanged(long)));
    }
#ifndef KEXI_NO_AUTOFIELD_WIDGET
    else if (c == "KexiDBAutoField") {
        w = new KexiDBAutoField(p);
    }
#endif
    else if (c == "KexiDBCheckBox") {
        w = new KexiDBCheckBox(text, p);
    }
    else if (c == "KexiDBSlider") {
        w = new KexiDBSlider(p);
    } else if (c == "KexiDBProgressBar") {
        w = new KexiDBProgressBar(p);
    } else if (c == "KexiDBDatePicker") {
        w = new KexiDBDatePicker(p);
    }
    else if (c == "KexiDBComboBox") {
        w = new KexiDBComboBox(p);
    }
    else if (c == "QPushButton" || c == "KPushButton" || c == "KexiDBPushButton" || c == "KexiPushButton") {
        w = new KexiDBPushButton(text, p);
    }
    else if (c == "KexiDBCommandLinkButton" || c == "KexiCommandLinkButton") {
        w = new KexiDBCommandLinkButton(text, QString(), p);
    }

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
        QMenu *subMenu = menu->addMenu(xi18n("&Image"));
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
                   m_assignAction = new QAction(koIcon("form_action"), xi18n("&Assign Action..."), this));
}

bool
KexiDBFactory::startInlineEditing(InlineEditorCreationArguments& args)
{
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

        QLineEdit *lineedit = static_cast<QLineEdit*>(args.widget);
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
    // KexiDBCommandLinkButton
    else if (args.classname == "KexiDBCommandLinkButton" ){
        KexiDBCommandLinkButton *linkButton=static_cast<KexiDBCommandLinkButton*>(args.widget);
        QStyleOption option;
        option.initFrom(linkButton);
        args.text = linkButton->text();
        const QRect r(linkButton->style()->subElementRect(
                        QStyle::SE_PushButtonContents, &option, linkButton));

        QFontMetrics fm(linkButton->font());
        args.geometry = QRect(linkButton->x() + linkButton->iconSize().width() + 6, linkButton->y() + r.y(), r.width()  - 6, fm.height()+14);

        return true;
    }
    else if (args.classname == "KexiDBLabel") {
        KexiDBLabel *label = static_cast<KexiDBLabel*>(args.widget);
        if (label->textFormat() == Qt::RichText) {
            args.execute = false;
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
                &openingCancelled);
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
        return true;
    }
    else if (args.classname == "KexiDBImageBox") {
        KexiDBImageBox *image = static_cast<KexiDBImageBox*>(args.widget);
        image->insertFromFile();
        args.execute = false;
        return true;
    }
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
    bool ok = true;
    if (classname == "KexiDBPushButton" || classname == "KexiPushButton") {
        ok = property != "isDragEnabled"
#ifndef KEXI_SHOW_UNFINISHED
             && property != "onClickAction" /*! @todo reenable */
             && property != "onClickActionOption" /*! @todo reenable */
             && property != "iconSet" /*! @todo reenable */
             && property != "iconSize" /*! @todo reenable */
             && property != "stdItem" /*! @todo reenable stdItem */
#endif
             ;
     } else if (classname == "KexiDBCommandLinkButton") {
        ok = property != "isDragEnabled"
             && property != "default"
             && property != "checkable"
             && property != "autoDefault"
             && property != "autoRepeat"
             && property != "autoRepeatDelay"
             && property != "autoRepeatInterval"
#ifndef KEXI_SHOW_UNFINISHED
             && property != "onClickAction" /*! @todo reenable */
             && property != "onClickActionOption" /*! @todo reenable */
             && property != "iconSet" /*! @todo reenable */
             && property != "iconSize" /*! @todo reenable */
             && property != "stdItem" /*! @todo reenable stdItem */
#endif
             ;
     } else if (classname == "KexiDBSlider") {
        ok = property != "sliderPosition"
             && property != "tracking";
     } else if (classname == "KexiDBProgressBar") {
        ok = property != "focusPolicy"
             && property != "value";
     } else if (classname == "KexiDBLineEdit")
        ok = property != "urlDropsEnabled"
             && property != "vAlign"
             && property != "echoMode"
             && property != "clickMessage" // Replaced by placeholderText in 2.9,
                                           // kept for backward compatibility Kexi projects created with Qt < 4.7.
             && property != "showClearButton" // Replaced by clearButtonEnabled in 3.0,
                                              // kept for backward compatibility Kexi projects created with Qt 4.
#ifndef KEXI_SHOW_UNFINISHED
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
#ifndef KEXI_SHOW_UNFINISHED
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
    }
    else if (classname == "KexiDBImageBox") {
        ok = property != "font" && property != "wordbreak" && property != "pixmapId";
    }
    else if (classname == "KexiDBCheckBox") {
        //hide text property if the widget is a child of an autofield beause there's already "caption" for this purpose
        if (property == "text" && w && dynamic_cast<KFormDesigner::WidgetWithSubpropertiesInterface*>(w->parentWidget()))
            return false;
        ok = property != "autoRepeat";
    }
    else if (classname == "KexiDBDatePicker") {
        ok = property != "closeButton"
             && property != "fontSize";
    }


    return ok && KexiDBFactoryBase::isPropertyVisibleInternal(classname, w, property, isTopLevel);
}

bool
KexiDBFactory::propertySetShouldBeReloadedAfterPropertyChange(const QByteArray& classname,
        QWidget *w, const QByteArray& property)
{
    Q_UNUSED(classname);
    Q_UNUSED(w);
    return property == "fieldTypeInternal" || property == "widgetType"
           || property == "paletteBackgroundColor" || property == "autoFillBackground";
}

bool KexiDBFactory::changeInlineText(KFormDesigner::Form *form, QWidget *widget,
    const QString &text, QString &oldText)
{
    const QByteArray n(widget->metaObject()->className());
    if (n == "KexiDBAutoField") {
        oldText = widget->property("caption").toString();
        changeProperty(form, widget, "caption", text);
        return true;
    } else if (n == "KexiDBCommandLinkButton") {
        oldText = widget->property("text").toString();
        changeProperty(form, widget, "text", text);
        return true;
    }

//! @todo check field's geometry
    return false;
}

void
KexiDBFactory::resizeEditor(QWidget *editor, QWidget *w, const QByteArray &classname)
{
    if (classname == "KexiDBAutoField")
        editor->setGeometry(static_cast<KexiDBAutoField*>(w)->label()->geometry());
}

void
KexiDBFactory::slotImageBoxIdChanged(KexiBLOBBuffer::Id_t id)
{
    KexiFormView *formView = KDbUtils::findParent<KexiFormView*>((QWidget*)sender());
    if (formView) {
        changeProperty(formView->form(), formView, "pixmapId", (uint)/*! @todo unsafe */id);
        formView->setUnsavedLocalBLOB(formView->form()->selectedWidget(), id);
    }
}

K_EXPORT_KEXIFORMWIDGETS_PLUGIN(KexiDBFactory, kexidbwidgets)

