/* This file is part of the KDE project
 * Copyright (C) 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
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

#include "AnnotationTextShape.h"

#include "ShrinkToFitShapeContainer.h"
#include <KoTextSharedLoadingData.h>
#include "SimpleRootAreaProvider.h"

#include <KoTextLayoutRootArea.h>
#include <KoTextEditor.h>

#include <KoCanvasBase.h>
#include <KoCanvasResourceManager.h>
#include <KoChangeTracker.h>
#include <KoInlineTextObjectManager.h>
#include <KoTextRangeManager.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoOdfWorkaround.h>
#include <KoParagraphStyle.h>
#include <KoPostscriptPaintDevice.h>
#include <KoSelection.h>
#include <KoShapeBackground.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeManager.h>
#include <KoShapePaintingContext.h>
#include <KoShapeSavingContext.h>
#include <KoText.h>
#include <KoTextDocument.h>
#include <KoTextDocumentLayout.h>
#include <KoTextEditor.h>
#include <KoTextPage.h>
#include <KoTextShapeContainerModel.h>
#include <KoPageProvider.h>
#include <KoViewConverter.h>
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>
#include <KoTextLoader.h>
#include <KoColorBackground.h>

#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QFont>
#include <QPainter>
#include <QPen>
#include <QTextLayout>
#include <QStaticText>

#include <kdebug.h>

AnnotationTextShape::AnnotationTextShape(KoInlineTextObjectManager *inlineTextObjectManager,
                                         KoTextRangeManager *textRangeManager)
    : TextShape(inlineTextObjectManager, textRangeManager)
{
    KoShapeBackground *fill = new KoColorBackground(Qt::yellow);
    setBackground(fill);
    setGeometryProtected(true);
}

AnnotationTextShape::~AnnotationTextShape()
{
}

void AnnotationTextShape::setAnnotaionTextData(KoTextShapeData *textShapeData)
{
    m_textShapeData = textShapeData;
    m_textShapeData->setTopPadding(25);
    m_textShapeData->setLeftPadding(4);
    m_textShapeData->setRightPadding(4);
    m_textShapeData->setResizeMethod(KoTextShapeData::AutoGrowHeight);
}

bool AnnotationTextShape::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    kDebug(31000) << "****** Start Load odf ******";

    KoTextLoader textLoader(context);
    QTextCursor cursor(textShapeData()->document());

    const QString localName(element.localName());

    if (localName == "annotation") {

        // FIXME: Load more attributes here

        // Load the metadata (author, date) and contents here.
        KoXmlElement el;
        forEachElement(el, element) {
            if (el.localName() == "creator" && el.namespaceURI() == KoXmlNS::dc) {
                m_creator = el.text();
                if (m_creator.isEmpty()) {
                    m_creator = "Unknown Author";
                }
            }
            else if (el.localName() == "date" && el.namespaceURI() == KoXmlNS::dc) {
                m_date = el.text();
            }
            else if (el.localName() == "datestring" && el.namespaceURI() == KoXmlNS::meta) {
                // FIXME: What to do here?
            }
        }
        textLoader.loadBody(element, cursor);
        kDebug(31000) << "****** End Load ******";
    }
    else {
        // something pretty weird going on...
        return false;
    }
    return true;
}

void AnnotationTextShape::saveOdf(KoShapeSavingContext &context) const
{
    kDebug(31000) << " ****** Start saveing annotation shape **********";
    KoXmlWriter *writer = &context.xmlWriter();

    writer->startElement("dc:creator", false);
    writer->addTextNode(m_creator);
    writer->endElement(); // dc:creator
    writer->startElement("dc:date", false);
    writer->addTextNode(m_date);
    writer->endElement(); // dc:date

    // I am not sure that this line is right or no?
    kDebug(31000) << m_textShapeData->document()->toPlainText();
    m_textShapeData->saveOdf(context, 0, 0, -1);
}
