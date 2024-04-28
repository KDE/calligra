/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "TemplateShapeFactory.h"

// Qt
#include <QList>

// KF5
#include <KLocalizedString>
#include <kdebug.h>

// Calligra
#include <KoDocumentResourceManager.h>
#include <KoIcon.h>
#include <KoOdfLoadingContext.h>
#include <KoProperties.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>

// This shape
#include "TemplateShape.h"
// #include "TemplateShapeConfigWidget.h"

TemplateShapeFactory::TemplateShapeFactory()
    : KoShapeFactoryBase(TEMPLATESHAPEID, i18n("Template shape")) // Template: Change to your own description
{
    setToolTip(i18n("Simple shape that is used as a template for developing other shapes."));
    setIconName(koIconName("x-shape-template"));
    setLoadingPriority(1);

    // Tell the shape loader which tag we can store
    // Template: You must change this.
    QList<QPair<QString, QStringList>> elementNamesList;
    elementNamesList.append(qMakePair(QString(KoXmlNS::calligra), QStringList("template")));
    setXmlElements(elementNamesList);
}

bool TemplateShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    // Template: Change this to your own supported element and namespace.
    if (e.localName() == "template" && e.namespaceURI() == KoXmlNS::calligra) {
        return true;
    }

    return false;
}

KoShape *TemplateShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    TemplateShape *defaultShape = new TemplateShape();
    defaultShape->setShapeId(TEMPLATESHAPEID);

    // Template: Insert code to initiate the defaults of your shape here.

    return defaultShape;
}

KoShape *TemplateShapeFactory::createShape(const KoProperties *params, KoDocumentResourceManager *documentResources) const
{
    TemplateShape *shape = static_cast<TemplateShape *>(createDefaultShape(documentResources));

    return shape;
}

QList<KoShapeConfigWidgetBase *> TemplateShapeFactory::createShapeOptionPanels()
{
    QList<KoShapeConfigWidgetBase *> result;

    return result;
}
