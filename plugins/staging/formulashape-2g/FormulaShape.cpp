/* This file is part of the KDE project
 *
 * Copyright (C) 2013 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "FormulaShape.h"

#include <QPainter>

#include <KDebug>

#include <KoViewConverter.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoStoreDevice.h>
#include <KoGenStyle.h>

// This shape
//#include "Foo.h"

#include "qtmmlwidget/QtMmlWidget"

#include <QDebug>

FormulaShape::FormulaShape()
    : QObject()
    , KoShape()
    , m_mmlDocument(new QtMmlDocument())
{
    QString error_msg;
    int error_line, error_column;
    QString text = "<math><munderover><mo>&Integral;</mo><msub><mi>x</mi><mn>1</mn></msub><msub><mi>x</mi><mn>0</mn></msub></munderover><mi>f</mi><mfenced><mi>x</mi></mfenced><mi>d</mi><mi>x</mi><mo>=</mo><munder><mn>lim</mn><mrow><mi>n</mi><mo>&RightArrow;</mo><mn>&infin;</mn></mrow></munder><munderover><mo>&Sum;</mo><mi>n</mi><mrow><mi>i</mi><mo>=</mo><mn>1</mn></mrow></munderover><mi>f</mi><mfenced><msub><mi>c</mi><mi>i</mi></msub></mfenced><mi>&Delta;</mi><msub><mi>x</mi><mi>i</mi></msub></math>";
    bool result = m_mmlDocument->setContent(text, &error_msg, &error_line,
                        &error_column);
    this->setSize(m_mmlDocument->size());

    if (!result) {
        qDebug()<<"Parse error: line " + QString::number(error_line)
                    + ", col " + QString::number(error_column)+ ": " + error_msg;
    }
}

FormulaShape::~FormulaShape()
{
}

void FormulaShape::paint(QPainter &painter, const KoViewConverter &converter,
                          KoShapePaintingContext &context)
{
    painter.setPen(QPen(QColor(0, 0, 0)));

    // Example painting code: Draw a rectangle around the shape
    painter.drawRect(converter.documentToView(QRectF(QPoint(0, 0), size())));

    m_mmlDocument->paint(&painter, QPoint(0, 0));
}


void FormulaShape::saveOdf(KoShapeSavingContext &context) const
{
    KoXmlWriter &writer = context.xmlWriter();

    // Example code: Save with calligra:template as the top XML element.
    writer.startElement("calligra:template");

    // Save shape attributes that were loaded using loadOdfAttributes.
    saveOdfAttributes(context, OdfAllAttributes);

    writer.endElement(); // calligra:template
}

bool FormulaShape::loadOdf(const KoXmlElement &templateElement, KoShapeLoadingContext &context)
{
    kDebug(31000) << "========================== Starting Template shape";
    kDebug(31000) <<"Loading ODF element: " << templateElement.tagName();

    // Load all standard odf attributes and store into the KoShape
    loadOdfAttributes(templateElement, context, OdfAllAttributes);

    // Template: Load the actual content of the shape here.

    return true;
}


void FormulaShape::waitUntilReady(const KoViewConverter &converter, bool asynchronous) const
{
}


#include <FormulaShape.moc>
