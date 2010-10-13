/* This file is part of the KDE project
 * Copyright (C) 2008-2009 Thorsten Zachmann <zachmann@kde.org>
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

#include "KPrPlaceholderTextStrategy.h"

#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QPainter>

#include <KoOdfLoadingContext.h>
#include <KoProperties.h>
#include <KoOdfStylesReader.h>
#include <KoGenStyles.h>
#include <KoXmlWriter.h>
#include <KoParagraphStyle.h>
#include <KoShape.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KoShapeSavingContext.h>
#include <KoTextShapeData.h>
#include <KoTextSharedLoadingData.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include <KoTextDocumentLayout.h>
#include <KoTextWriter.h>
#include <KoStyleManager.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>

KPrPlaceholderTextStrategy::KPrPlaceholderTextStrategy( const QString & presentationClass )
: KPrPlaceholderStrategy( presentationClass )
, m_textShape( 0 )
{
}

KPrPlaceholderTextStrategy::~KPrPlaceholderTextStrategy()
{
    delete m_textShape;
}

KoShape *KPrPlaceholderTextStrategy::createShape(KoResourceManager *documentResources)
{
    KoShape * shape = KPrPlaceholderStrategy::createShape(documentResources);
    if ( m_textShape ) {
        KoTextShapeData * data = qobject_cast<KoTextShapeData*>( m_textShape->userData() );
        KoTextShapeData * newData = qobject_cast<KoTextShapeData*>( shape->userData() );
        if ( data && newData ) {
            QTextCursor cursor( data->document() );
            QTextCursor newCursor( newData->document() );
            KoTextDocument textDocument( newData->document() );

            QTextBlockFormat blockFormat( cursor.blockFormat() );
            newCursor.setBlockFormat( blockFormat );

            QTextCharFormat chatFormat( cursor.blockCharFormat() );
            newCursor.setBlockCharFormat( chatFormat );
        }
    }
    return shape;
}

void KPrPlaceholderTextStrategy::paint( QPainter & painter, const KoViewConverter &converter, const QRectF & rect )
{
    if ( m_textShape ) {
        painter.save();
        m_textShape->setSize( rect.size() );
        // this code is needed to make sure the text of the textshape is layouted before it is painted
        KoTextShapeData * shapeData = qobject_cast<KoTextShapeData*>( m_textShape->userData() );
        QTextDocument * document = shapeData->document();
        KoTextDocumentLayout * lay = qobject_cast<KoTextDocumentLayout*>( document->documentLayout() );
        if ( lay ) {
            lay->layout();
        }
        m_textShape->paint( painter, converter );

        KoShape::applyConversion( painter, converter );
        QPen pen( Qt::gray );
        //pen.setStyle( Qt::DashLine ); // endless loop
        painter.setPen( pen );
        painter.drawRect( rect );
        painter.restore();
    }
    else {
        KPrPlaceholderStrategy::paint( painter, converter, rect );
    }
}

void KPrPlaceholderTextStrategy::saveOdf( KoShapeSavingContext & context )
{
    if (m_textShape) {
        if (KoTextShapeData *shapeData = qobject_cast<KoTextShapeData*>(m_textShape->userData())) {
            KoStyleManager *styleManager = KoTextDocument(shapeData->document()).styleManager();
            KoTextShapeData *shapeData = qobject_cast<KoTextShapeData*>(m_textShape->userData());
            if (styleManager && shapeData) {
                QTextDocument *document = shapeData->document();
                QTextBlock block = document->begin();
                QString styleName = KoTextWriter::saveParagraphStyle(block, styleManager, context);
                context.xmlWriter().addAttribute("draw:text-style-name", styleName);
            }
        }
    }
    KPrPlaceholderStrategy::saveOdf( context );
}

bool KPrPlaceholderTextStrategy::loadOdf( const KoXmlElement & element, KoShapeLoadingContext & context )
{
    if (KoTextSharedLoadingData *textSharedData = dynamic_cast<KoTextSharedLoadingData *>(context.sharedData(KOTEXT_SHARED_LOADING_ID))) {
        KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value("TextShapeID");
        Q_ASSERT(factory);
        m_textShape = factory->createDefaultShape(context.documentResourceManager());

        KoTextShapeData *shapeData = qobject_cast<KoTextShapeData*>(m_textShape->userData());
        shapeData->document()->setUndoRedoEnabled(false);

        QTextDocument *document = shapeData->document();
        QTextCursor cursor(document);
        QTextBlock block = cursor.block();

        const QString styleName = element.attributeNS(KoXmlNS::presentation, "style-name");
        if (!styleName.isEmpty()) {
            const KoXmlElement *style = context.odfLoadingContext().stylesReader().findStyle(styleName, "presentation", context.odfLoadingContext().useStylesAutoStyles());

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
        shapeData->foul();
        shapeData->document()->setUndoRedoEnabled(true);
    }
    return true;
}

void KPrPlaceholderTextStrategy::init(KoResourceManager *documentResources)
{
    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value( "TextShapeID" );
    Q_ASSERT( factory );
    KoProperties props;
    props.setProperty("text", text());
    m_textShape = factory->createShape(&props, documentResources);
}

KoShapeUserData * KPrPlaceholderTextStrategy::userData() const
{
    return m_textShape ? m_textShape->userData() : 0;
}
