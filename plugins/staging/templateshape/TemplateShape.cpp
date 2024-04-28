/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "TemplateShape.h"

// Qt
#include <QPainter>

// KF5
#include <kdebug.h>

// Calligra
#include <KoGenStyle.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoViewConverter.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

// This shape
// #include "Foo.h"

TemplateShape::TemplateShape()
    : QObject()
    , KoShape()
// , m_member()  // initiate members here.
{
}

TemplateShape::~TemplateShape()
{
}

void TemplateShape::paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &context)
{
    painter.setPen(QPen(QColor(0, 0, 0), 0));

    // Example painting code: Draw a rectangle around the shape
    painter.drawRect(converter.documentToView(QRectF(QPoint(0, 0), size())));
}

void TemplateShape::saveOdf(KoShapeSavingContext &context) const
{
    KoXmlWriter &writer = context.xmlWriter();

    // Example code: Save with calligra:template as the top XML element.
    writer.startElement("calligra:template");

    // Save shape attributes that were loaded using loadOdfAttributes.
    saveOdfAttributes(context, OdfAllAttributes);

    writer.endElement(); // calligra:template
}

bool TemplateShape::loadOdf(const KoXmlElement &templateElement, KoShapeLoadingContext &context)
{
    kDebug(31000) << "========================== Starting Template shape";
    kDebug(31000) << "Loading ODF element: " << templateElement.tagName();

    // Load all standard odf attributes and store into the KoShape
    loadOdfAttributes(templateElement, context, OdfAllAttributes);

    // Template: Load the actual content of the shape here.

    return true;
}

void TemplateShape::waitUntilReady(const KoViewConverter &converter, bool asynchronous) const
{
}
