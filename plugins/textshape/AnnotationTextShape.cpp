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
    , m_imageCollection(0)
{
    KoShapeBackground *fill = new KoColorBackground(Qt::yellow);
    setBackground(fill);
}

AnnotationTextShape::~AnnotationTextShape()
{
}

void AnnotationTextShape::setAnnotaionTextData(KoTextShapeData *textShape)
{
    m_textShapeData = textShape;
    m_textShapeData->setTopPadding(25);
}

void AnnotationTextShape::paintComponent(QPainter &painter, const KoViewConverter &converter,
                                         KoShapePaintingContext &paintcontext)
{
    //FIXME: Please fix me, Or tel me if there is something wrong here.

    if (paintcontext.showTextShapeOutlines) {
        painter.save();
        applyConversion(painter, converter);
        if (qAbs(rotation()) > 1)
            painter.setRenderHint(QPainter::Antialiasing);

        QPen pen(QColor(210, 210, 210)); // use cosmetic pen
        QPointF onePixel = converter.viewToDocument(QPointF(1.0, 1.0));
        QRectF rect(QPointF(0.0, 0.0), size() - QSizeF(onePixel.x(), onePixel.y()));
        //painter.setPen(pen);

        painter.setPen(pen);
        painter.drawRect(rect);


        if (background()) {
            QPainterPath p;
            p.addRect(QRectF(QPointF(), size()));
            background()->paint(painter, converter, paintcontext, p);
        }

        // Draw delete annotation button.
        QRect removeButton (rect.width(), 0, -20, 20);
        painter.fillRect(removeButton, QBrush(QColor(Qt::red)));

        // Set Author and date.
        QPen peninfo (Qt::darkYellow);
        QFont serifFont("Times", 7, QFont::Bold);
        painter.setPen(peninfo);
        painter.setFont(serifFont);
        QString info = "Author: " + m_creator + "\n Date: " + m_date;
        painter.drawText(rect, Qt::AlignTop, info);

        painter.restore();
    }

    if (m_textShapeData->isDirty()) { // not layouted yet.
        return;
    }

    QTextDocument *doc = m_textShapeData->document();
    Q_ASSERT(doc);
    KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout*>(doc->documentLayout());
    Q_ASSERT(lay);
    lay->showInlineObjectVisualization(paintcontext.showInlineObjectVisualization);

    applyConversion(painter, converter);

//    if (background()) {
//        QPainterPath p;
//        p.addRect(QRectF(QPointF(), size()));
//        background()->paint(painter, converter, paintcontext, p);
//    }

    // this enables to use the same shapes on different pages showing different page numbers
//    if (m_pageProvider) {
//        KoTextPage *page = m_pageProvider->page(this);
//        if (page) {
//            // this is used to not trigger repaints if layout during the painting is done
//            m_paintRegion = painter.clipRegion();
//            if (!m_textShapeData->rootArea()->page() || page->pageNumber() != m_textShapeData->rootArea()->page()->pageNumber()) {
//                m_textShapeData->rootArea()->setPage(page); // takes over ownership of the page
//            } else {
//                delete page;
//            }
//        }
//    }

    KoTextDocumentLayout::PaintContext pc;

    QAbstractTextDocumentLayout::Selection selection;
    KoTextEditor *textEditor = KoTextDocument(m_textShapeData->document()).textEditor();

    // FIXME: It gets error that textEditor->cursor(), cursor is private ????

    //selection.cursor = *(textEditor->cursor());
    QPalette palette = pc.textContext.palette;
    selection.format.setBackground(palette.brush(QPalette::Highlight));
    selection.format.setForeground(palette.brush(QPalette::HighlightedText));
    pc.textContext.selections.append(selection);

    pc.textContext.selections += KoTextDocument(doc).selections();
    pc.viewConverter = &converter;
    pc.imageCollection = m_imageCollection;
    pc.showFormattingCharacters = paintcontext.showFormattingCharacters;
    pc.showTableBorders = paintcontext.showTableBorders;
    pc.showSpellChecking = paintcontext.showSpellChecking;
    pc.showSelections = paintcontext.showSelections;

    // When clipping the painter we need to make sure not to cutoff cosmetic pens which
    // may used to draw e.g. table-borders for user convenience when on screen (but not
    // on e.g. printing). Such cosmetic pens are special cause they will always have the
    // same pen-width (1 pixel) independent of zoom-factor or painter transformations and
    // are not taken into account in any border-calculations.
    QRectF clipRect = outlineRect();
    qreal cosmeticPenX = 1 * 72. / painter.device()->logicalDpiX();
    qreal cosmeticPenY = 1 * 72. / painter.device()->logicalDpiY();
    painter.setClipRect(clipRect.adjusted(-cosmeticPenX, -cosmeticPenY, cosmeticPenX, cosmeticPenY), Qt::IntersectClip);

    painter.save();
    painter.translate(0, -m_textShapeData->documentOffset());
    painter.translate(m_textShapeData->leftPadding(), m_textShapeData->topPadding());
    m_textShapeData->rootArea()->paint(&painter, pc); // only need to draw ourselves
    painter.restore();

    m_paintRegion = QRegion();
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
