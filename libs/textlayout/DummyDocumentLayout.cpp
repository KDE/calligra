/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 KO GmbH <cbo@kogmbh.com>
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "DummyDocumentLayout.h"

#include <KoPostscriptPaintDevice.h>

#include <QTextBlock>

#include <TextLayoutDebug.h>

// ------------------- DummyDocumentLayout --------------------
DummyDocumentLayout::DummyDocumentLayout(QTextDocument *doc)
    : QAbstractTextDocumentLayout(doc)
{
    setPaintDevice(new KoPostscriptPaintDevice());
}

DummyDocumentLayout::~DummyDocumentLayout() = default;

QRectF DummyDocumentLayout::blockBoundingRect(const QTextBlock &block) const
{
    Q_UNUSED(block);
    return QRect();
}

QSizeF DummyDocumentLayout::documentSize() const
{
    return QSizeF();
}

void DummyDocumentLayout::draw(QPainter *painter, const QAbstractTextDocumentLayout::PaintContext &context)
{
    // WARNING Text shapes ask their root area directly to paint.
    // It saves a lot of extra traversal, that is quite costly for big
    // documents
    Q_UNUSED(painter);
    Q_UNUSED(context);
}

int DummyDocumentLayout::hitTest(const QPointF &point, Qt::HitTestAccuracy accuracy) const
{
    Q_UNUSED(point);
    Q_UNUSED(accuracy);
    Q_ASSERT(false); // we should not call this method.
    return -1;
}

QRectF DummyDocumentLayout::frameBoundingRect(QTextFrame *) const
{
    return QRectF();
}

int DummyDocumentLayout::pageCount() const
{
    return 1;
}

void DummyDocumentLayout::documentChanged(int position, int charsRemoved, int charsAdded)
{
    Q_UNUSED(position);
    Q_UNUSED(charsRemoved);
    Q_UNUSED(charsAdded);
}

/*
void DummyDocumentLayout::drawInlineObject(QPainter *, const QRectF &, QTextInlineObject , int , const QTextFormat &)
{
}

// This method is called by qt every time  QTextLine.setWidth()/setNumColumns() is called
void DummyDocumentLayout::positionInlineObject(QTextInlineObject , int , const QTextFormat &)
{
}

void DummyDocumentLayout::resizeInlineObject(QTextInlineObject , int , const QTextFormat &)
{
}
*/
