/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008-2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrPlaceholderTextStrategy.h"

#include <QPainter>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoParagraphStyle.h>
#include <KoProperties.h>
#include <KoShape.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeRegistry.h>
#include <KoShapeSavingContext.h>
#include <KoStyleManager.h>
#include <KoTextDocument.h>
#include <KoTextDocumentLayout.h>
#include <KoTextEditor.h>
#include <KoTextShapeData.h>
#include <KoTextSharedLoadingData.h>
#include <KoTextWriter.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include "StageDebug.h"

KPrPlaceholderTextStrategy::KPrPlaceholderTextStrategy(const QString &presentationClass)
    : KPrPlaceholderStrategy(presentationClass)
    , m_textShape(nullptr)
{
}

KPrPlaceholderTextStrategy::~KPrPlaceholderTextStrategy()
{
    delete m_textShape;
}

KoShape *KPrPlaceholderTextStrategy::createShape(KoDocumentResourceManager *documentResources)
{
    KoShape *shape = KPrPlaceholderStrategy::createShape(documentResources);
    if (m_textShape) {
        KoTextShapeData *data = qobject_cast<KoTextShapeData *>(m_textShape->userData());
        KoTextShapeData *newData = qobject_cast<KoTextShapeData *>(shape->userData());
        if (data && newData) {
            QTextCursor cursor(data->document());
            QTextCursor newCursor(newData->document());
            KoTextDocument textDocument(newData->document());

            QTextBlockFormat blockFormat(cursor.blockFormat());
            newCursor.setBlockFormat(blockFormat);

            QTextCharFormat chatFormat(cursor.blockCharFormat());
            newCursor.setBlockCharFormat(chatFormat);
        }
    }
    return shape;
}

void KPrPlaceholderTextStrategy::paint(QPainter &painter, const KoViewConverter &converter, const QRectF &rect, KoShapePaintingContext &paintcontext)
{
    if (m_textShape) {
        painter.save();
        m_textShape->setSize(rect.size());
        // this code is needed to make sure the text of the textshape is layouted before it is painted
        KoTextShapeData *shapeData = qobject_cast<KoTextShapeData *>(m_textShape->userData());
        QTextDocument *document = shapeData->document();
        KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout *>(document->documentLayout());
        if (lay) {
            lay->layout();
        }
        m_textShape->paint(painter, converter, paintcontext);

        KoShape::applyConversion(painter, converter);
        QPen pen(Qt::gray, 0);
        // pen.setStyle( Qt::DashLine ); // endless loop
        painter.setPen(pen);
        painter.drawRect(rect);
        painter.restore();
    } else {
        KPrPlaceholderStrategy::paint(painter, converter, rect, paintcontext);
    }
}

void KPrPlaceholderTextStrategy::saveOdf(KoShapeSavingContext &context)
{
    if (m_textShape) {
        KoTextShapeData *shapeData = qobject_cast<KoTextShapeData *>(m_textShape->userData());
        if (shapeData) {
            KoStyleManager *styleManager = KoTextDocument(shapeData->document()).styleManager();
            if (styleManager) {
                QTextDocument *document = shapeData->document();
                QTextBlock block = document->begin();
                QString styleName = KoTextWriter::saveParagraphStyle(block, styleManager, context);
                context.xmlWriter().addAttribute("draw:text-style-name", styleName);
            }
        }
    }
    KPrPlaceholderStrategy::saveOdf(context);
}

bool KPrPlaceholderTextStrategy::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    if (KoTextSharedLoadingData *textSharedData = dynamic_cast<KoTextSharedLoadingData *>(context.sharedData(KOTEXT_SHARED_LOADING_ID))) {
        KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value("TextShapeID");
        Q_ASSERT(factory);
        if (!factory) {
            warnStage << "text shape factory not found";
            return false;
        }
        delete m_textShape;
        m_textShape = factory->createDefaultShape(context.documentResourceManager());

        KoTextShapeData *shapeData = qobject_cast<KoTextShapeData *>(m_textShape->userData());
        shapeData->document()->setUndoRedoEnabled(false);

        QTextDocument *document = shapeData->document();
        QTextCursor cursor(document);
        QTextBlock block = cursor.block();

        const QString styleName = element.attributeNS(KoXmlNS::presentation, "style-name");
        if (!styleName.isEmpty()) {
            const KoXmlElement *style =
                context.odfLoadingContext().stylesReader().findStyle(styleName, "presentation", context.odfLoadingContext().useStylesAutoStyles());

            if (style) {
                KoParagraphStyle paragraphStyle;
                paragraphStyle.loadOdf(style, context);
                paragraphStyle.applyStyle(block, false); // TODO t.zachmann is the false correct?
            }
        }

        const QString textStyleName = element.attributeNS(KoXmlNS::draw, "text-style-name");
        if (!textStyleName.isEmpty()) {
            KoParagraphStyle *style = textSharedData->paragraphStyle(textStyleName, context.odfLoadingContext().useStylesAutoStyles());
            if (style) {
                style->applyStyle(block, false); // TODO t.zachmann is the false correct?
            }
        }

        cursor.insertText(text());
        shapeData->setDirty();
        shapeData->document()->setUndoRedoEnabled(true);
    }
    return true;
}

void KPrPlaceholderTextStrategy::init(KoDocumentResourceManager *documentResources)
{
    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value("TextShapeID");
    Q_ASSERT(factory);
    if (!factory) {
        warnStage << "text shape factory not found";
        return;
    }
    KoProperties props;
    props.setProperty("text", text());
    delete m_textShape;
    m_textShape = factory->createShape(&props, documentResources);
}

KoShapeUserData *KPrPlaceholderTextStrategy::userData() const
{
    return m_textShape ? m_textShape->userData() : nullptr;
}
