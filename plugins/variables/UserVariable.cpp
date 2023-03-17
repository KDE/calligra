/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Sebastian Sauer <mail@dipe.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "UserVariable.h"

#include "UserVariableOptionsWidget.h"
#include "VariablesDebug.h"

#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoProperties.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>
#include <KoInlineTextObjectManager.h>
#include <KoVariableManager.h>
#include <KoTextDocument.h>

#include <klocalizedstring.h>

#include <QTextInlineObject>

UserVariable::UserVariable()
    : KoVariable(true)
    , m_variableManager(0)
    , m_property(0)
{
}

KoVariableManager *UserVariable::variableManager()
{
    if (m_variableManager) {
        return m_variableManager;
    }

    KoInlineTextObjectManager *textObjectManager = manager();
    Q_ASSERT(textObjectManager);
    m_variableManager = textObjectManager->variableManager();
    Q_ASSERT(m_variableManager);
    connect(m_variableManager, &KoVariableManager::valueChanged, this, &UserVariable::valueChanged);

    valueChanged(); // initial update

    return m_variableManager;
}

int UserVariable::property() const
{
    return m_property;
}

const QString& UserVariable::name() const
{
    return m_name;
}

void UserVariable::setName(const QString &name)
{
    m_name = name;
    valueChanged();
}

KoOdfNumberStyles::NumericStyleData UserVariable::numberstyle() const
{
    return m_numberstyle;
}

void UserVariable::setNumberStyle(KoOdfNumberStyles::NumericStyleData numberstyle)
{
    m_numberstyle = numberstyle;
    valueChanged();
}

QWidget* UserVariable::createOptionsWidget()
{
    UserVariableOptionsWidget *configWidget = new UserVariableOptionsWidget(this);
    return configWidget;
}

void UserVariable::valueChanged()
{
    //TODO apply following also to plugins/variables/DateVariable.cpp:96
    //TODO handle formula
    QString value = variableManager()->value(m_name);
    value = KoOdfNumberStyles::format(value, m_numberstyle);
    setValue(value);
}

void UserVariable::readProperties(const KoProperties *props)
{
    m_property = props->intProperty("varproperty");
    //m_name = props->stringProperty("varname");
    //debugVariables << m_property << m_name;
    //valueChanged();
}

void UserVariable::propertyChanged(Property property, const QVariant &value)
{
    Q_UNUSED(property);
    Q_UNUSED(value);
    //setValue(value.toString());
}

void UserVariable::saveOdf(KoShapeSavingContext &context)
{
    if (m_property == 0 && !variableManager()->userVariables().contains(m_name))
        return;

    KoXmlWriter *writer = &context.xmlWriter();

    if (m_property == KoInlineObject::UserGet)
        writer->startElement("text:user-field-get", false);
    else
        writer->startElement("text:user-field-input", false);

    if (!m_name.isEmpty())
        writer->addAttribute("text:name", m_name);

    QString styleName = KoOdfNumberStyles::saveOdfNumericStyle(context.mainStyles(), m_numberstyle);
    if (!styleName.isEmpty())
        writer->addAttribute("style:data-style-name", styleName);

    writer->addTextNode(value());
    writer->endElement();
}

bool UserVariable::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    if (element.localName() == "user-field-get") {
        m_property = KoInlineObject::UserGet;
    } else if (element.localName() == "user-field-input") {
        m_property = KoInlineObject::UserInput;
    } else {
        m_property = 0;
    }

    m_name = element.attributeNS(KoXmlNS::text, "name");

    QString dataStyle = element.attributeNS(KoXmlNS::style, "data-style-name");
    if (!dataStyle.isEmpty() && context.odfLoadingContext().stylesReader().dataFormats().contains(dataStyle)) {
        m_numberstyle = context.odfLoadingContext().stylesReader().dataFormats().value(dataStyle).first;
    } else {
        m_numberstyle = KoOdfNumberStyles::NumericStyleData();
    }

    return true;
}

void UserVariable::resize(const QTextDocument *document, QTextInlineObject &object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd)
{
    KoVariable::resize(document, object, posInDocument, format, pd);

    if (!m_variableManager) {
        variableManager();
    }
}
