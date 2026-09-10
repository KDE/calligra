/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoFormShapeFactory.h"

#include "KoFormShape.h"

#include <KLocalizedString>
#include <KoProperties.h>
#include <KoXmlNS.h>

using namespace Qt::StringLiterals;

KoFormShapeFactory::KoFormShapeFactory()
    : KoShapeFactoryBase(u"FormShape"_s, i18nc("@title:menu", "Forms"))
{
    setFamily(u"forms"_s);
    setToolTip(i18nc("@info:tooltip", "Form control"));
    setLoadingPriority(10);
    setXmlElements({qMakePair(QString(KoXmlNS::draw), QStringList{u"control"_s})});

    const auto addFormTemplate = [this](const QString &kind, const QString &name, const QString &icon, const QString &toolTip) {
        KoShapeTemplate t;
        t.id = u"FormShape"_s;
        t.templateId = kind;
        t.name = name;
        t.iconName = icon;
        t.toolTip = toolTip;
        auto *properties = new KoProperties();
        properties->setProperty(u"control-kind"_s, kind);
        t.properties = properties;
        addTemplate(t);
    };
    addFormTemplate(u"text"_s, i18nc("@item:inmenu", "Text box"), u"form-text"_s, i18nc("@info:tooltip", "Insert a text box"));
    addFormTemplate(u"textarea"_s, i18nc("@item:inmenu", "Text area"), u"form-text"_s, i18nc("@info:tooltip", "Insert a text area"));
    addFormTemplate(u"formatted-text"_s,
                    i18nc("@item:inmenu", "Formatted field"),
                    u"form-formattedfield"_s,
                    i18nc("@info:tooltip", "Insert a formatted field"));
    addFormTemplate(u"button"_s, i18nc("@item:inmenu", "Button"), u"form-pushbutton"_s, i18nc("@info:tooltip", "Insert a button"));
    addFormTemplate(u"checkbox"_s, i18nc("@item:inmenu", "Check box"), u"form-checkbox"_s, i18nc("@info:tooltip", "Insert a check box"));
    addFormTemplate(u"radio"_s, i18nc("@item:inmenu", "Radio button"), u"form-radiobutton"_s, i18nc("@info:tooltip", "Insert a radio button"));
    addFormTemplate(u"combobox"_s, i18nc("@item:inmenu", "Combo box"), u"form-combobox"_s, i18nc("@info:tooltip", "Insert a combo box"));
    addFormTemplate(u"listbox"_s, i18nc("@item:inmenu", "List box"), u"form-listbox"_s, i18nc("@info:tooltip", "Insert a list box"));
    addFormTemplate(u"date"_s, i18nc("@item:inmenu", "Date field"), u"form-datefield"_s, i18nc("@info:tooltip", "Insert a date field"));
    addFormTemplate(u"time"_s, i18nc("@item:inmenu", "Time field"), u"form-timefield"_s, i18nc("@info:tooltip", "Insert a time field"));
    addFormTemplate(u"number"_s, i18nc("@item:inmenu", "Number field"), u"form-numericfield"_s, i18nc("@info:tooltip", "Insert a number field"));
    addFormTemplate(u"value-range"_s, i18nc("@item:inmenu", "Value range"), u"form-spinbutton"_s, i18nc("@info:tooltip", "Insert a value range"));
    addFormTemplate(u"fixed-text"_s, i18nc("@item:inmenu", "Label field"), u"form-label"_s, i18nc("@info:tooltip", "Insert a label field"));
    addFormTemplate(u"password"_s, i18nc("@item:inmenu", "Password field"), u"form-patternfield"_s, i18nc("@info:tooltip", "Insert a password field"));
    addFormTemplate(u"file"_s, i18nc("@item:inmenu", "File selection"), u"form-filecontrol"_s, i18nc("@info:tooltip", "Insert a file selection field"));
    addFormTemplate(u"image"_s, i18nc("@item:inmenu", "Image button"), u"form-imagebutton"_s, i18nc("@info:tooltip", "Insert an image button"));
    addFormTemplate(u"image-frame"_s, i18nc("@item:inmenu", "Image frame"), u"form-imagecontrol"_s, i18nc("@info:tooltip", "Insert an image frame"));
    addFormTemplate(u"frame"_s, i18nc("@item:inmenu", "Group box"), u"form-formgroup"_s, i18nc("@info:tooltip", "Insert a group box"));
    addFormTemplate(u"grid"_s, i18nc("@item:inmenu", "Grid"), u"form-viewformasgrid"_s, i18nc("@info:tooltip", "Insert a grid"));
}

KoShape *KoFormShapeFactory::createDefaultShape(KoDocumentResourceManager *) const
{
    return new KoFormShape();
}

KoShape *KoFormShapeFactory::createShape(const KoProperties *properties, KoDocumentResourceManager *documentResources) const
{
    auto *shape = static_cast<KoFormShape *>(createDefaultShape(documentResources));
    if (properties) {
        shape->setControlKind(properties->property(u"control-kind"_s).toString());
    }
    return shape;
}

bool KoFormShapeFactory::supports(const KoXmlElement &element, KoShapeLoadingContext &) const
{
    return element.localName() == "control"_L1 && element.namespaceURI() == KoXmlNS::draw && !element.attributeNS(KoXmlNS::draw, u"control"_s).isEmpty();
}
