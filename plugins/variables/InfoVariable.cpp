/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Pierre Ducroquet <pinaraf@gmail.com>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "InfoVariable.h"

#include "VariablesDebug.h"

#include <KoProperties.h>
#include <KoShapeSavingContext.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include <QGlobalStatic>

static const struct {
    KoInlineObject::Property property;
    const char *tag;
    const char *saveTag;
} propertyData[] = {{KoInlineObject::AuthorName, "creator", "text:creator"},
                    {KoInlineObject::DocumentURL, "file-name", "text:file-name"},
                    {KoInlineObject::Title, "title", "text:title"},
                    {KoInlineObject::Subject, "subject", "text:subject"},
                    {KoInlineObject::Keywords, "keywords", "text:keywords"},
                    //{ KoInlineObject::Description, "description", "text:description" }
                    {KoInlineObject::Comments, "comments", "text:comments"}};

static const unsigned int numPropertyData = sizeof(propertyData) / sizeof(*propertyData);

QStringList InfoVariable::tags()
{
    QStringList tagList;
    for (unsigned int i = 0; i < numPropertyData; ++i) {
        tagList << propertyData[i].tag;
    }
    return tagList;
}

InfoVariable::InfoVariable()
    : KoVariable(true)
    , m_type(KoInlineObject::AuthorName)
{
}

void InfoVariable::readProperties(const KoProperties *props)
{
    m_type = (Property)props->property("vartype").value<int>();
}

void InfoVariable::propertyChanged(Property property, const QVariant &value)
{
    if (property == m_type) {
        setValue(value.toString());
    }
}

typedef QMap<KoInlineObject::Property, const char *> SaveMap;

Q_GLOBAL_STATIC(SaveMap, s_saveInfo)

void InfoVariable::saveOdf(KoShapeSavingContext &context)
{
    KoXmlWriter *writer = &context.xmlWriter();

    if (!s_saveInfo.exists()) {
        for (unsigned int i = 0; i < numPropertyData; ++i) {
            s_saveInfo->insert(propertyData[i].property, propertyData[i].saveTag);
        }
    }
    const char *nodeName = s_saveInfo->value(m_type, 0);
    if (nodeName) {
        writer->startElement(nodeName, false);
        writer->addTextNode(value());
        writer->endElement();
    }
}

typedef QMap<QString, KoInlineObject::Property> LoadMap;

Q_GLOBAL_STATIC(LoadMap, s_loadInfo)

bool InfoVariable::loadOdf(const KoXmlElement &element, KoShapeLoadingContext & /*context*/)
{
    if (!s_loadInfo.exists()) {
        for (unsigned int i = 0; i < numPropertyData; ++i) {
            s_loadInfo->insert(propertyData[i].tag, propertyData[i].property);
        }
    }

    const QString localName(element.localName());
    m_type = s_loadInfo->value(localName);

    for (KoXmlNode node = element.firstChild(); !node.isNull(); node = node.nextSibling()) {
        if (node.isText()) {
            setValue(node.toText().data());
            break;
        }
    }

    return true;
}
