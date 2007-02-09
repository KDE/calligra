/* This file is part of the KDE project
 * Copyright (C) 2006 Boudewijn Rempt <boud@valdyas.org>
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
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

#include "TextShape.h"
#include "Layout.h"

#include <KoTextDocumentLayout.h>
#include <KoInlineTextObjectManager.h>
#include <KoViewConverter.h>
#include <KoCanvasBase.h>
#include <KoCanvasResourceProvider.h>
#include <KoShapeManager.h>
#include <KoText.h>
#include <KoSelection.h>

#include <QTextLayout>
#include <QFont>
#include <QPen>
#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <kdebug.h>


TextShape::TextShape()
{
    setShapeId(TextShape_SHAPEID);
    m_textShapeData = new KoTextShapeData();
    setUserData(m_textShapeData);
    KoTextDocumentLayout *lay = new KoTextDocumentLayout(m_textShapeData->document());
    lay->setLayout(new Layout(lay));
    lay->addShape(this);
    m_textShapeData->document()->setDocumentLayout(lay);

    lay->setInlineObjectTextManager(new KoInlineTextObjectManager(lay));
    setCollisionDetection(true);
}

TextShape::~TextShape() {
}

void TextShape::paint(QPainter &painter, const KoViewConverter &converter) {
    painter.fillRect(converter.documentToView(QRectF(QPointF(0.0,0.0), size())), background());
    if(m_textShapeData->endPosition() < 0) { // not layouted yet.
        QTextDocument *doc = m_textShapeData->document();
        KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*> (doc->documentLayout());
        if(lay == 0)
            kWarning(32500) << "Painting shape that doesn't have a kotext doc-layout, which can't work\n";
        else if(! lay->hasLayouter())
            lay->setLayout(new Layout(lay));
        return;
    }
    applyConversion(painter, converter);
    QAbstractTextDocumentLayout::PaintContext pc;
    pc.cursorPosition = -1;

    QTextDocument *doc = m_textShapeData->document();
    painter.setClipRect(QRectF(QPointF(0, 0), size()), Qt::IntersectClip);
    painter.translate(0, -m_textShapeData->documentOffset());
    doc->documentLayout()->draw( &painter, pc);
}

QPointF TextShape::convertScreenPos(const QPointF &point) {
    QPointF p = matrix().inverted().map(point);
    return p + QPointF(0.0, m_textShapeData->documentOffset());
}

void TextShape::shapeChanged(ChangeType type) {
    if(type == PositionChanged || type == SizeChanged || type == CollisionDetected) {
        m_textShapeData->faul();
        m_textShapeData->fireResizeEvent();
    }
}

void TextShape::paintDecorations(QPainter &painter, const KoViewConverter &converter, const KoCanvasBase *canvas) {
    applyConversion(painter, converter);

    bool showTextFrames = canvas->resourceProvider()->boolProperty(KoText::ShowTextFrames);
    if(showTextFrames) {
        painter.save();
        if(qAbs(rotation()) > 1)
            painter.setRenderHint(QPainter::Antialiasing);

        QPen pen(Qt::red);
        QPointF onePixel = converter.viewToDocument( QPointF(1.0, 1.0));
        pen.setWidthF( onePixel.y() );
        painter.setPen(pen);

        QPointF tl(0.0, 0.0);
        QRectF rect(tl, size());
        painter.drawLine(tl, rect.topRight());
        painter.drawLine(rect.bottomLeft(), rect.bottomRight());

        pen.setWidthF( onePixel.x() );
        painter.setPen(pen);
        painter.drawLine(rect.topRight(), rect.bottomRight());
        painter.drawLine(tl, rect.bottomLeft());
        painter.restore();
    }

    // draw a '+' bottom right if there is more text, and this is the last one.
}

