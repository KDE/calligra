/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextSoftPageBreak.h"

#include <KoShapeSavingContext.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include <QPainter>
#include <QTextInlineObject>

// Include Q_UNSUSED classes, for building on Windows
#include <KoShapeLoadingContext.h>

KoTextSoftPageBreak::KoTextSoftPageBreak() = default;

KoTextSoftPageBreak::~KoTextSoftPageBreak() = default;

bool KoTextSoftPageBreak::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    Q_UNUSED(element)
    Q_UNUSED(context)
    return true;
}

void KoTextSoftPageBreak::saveOdf(KoShapeSavingContext &context)
{
    KoXmlWriter &writer = context.xmlWriter();
    writer.startElement("text:soft-page-break");
    writer.endElement();
}

void KoTextSoftPageBreak::updatePosition(const QTextDocument *document, int posInDocument, const QTextCharFormat &format)
{
    Q_UNUSED(document)
    Q_UNUSED(posInDocument)
    Q_UNUSED(format)
}

void KoTextSoftPageBreak::resize(const QTextDocument *document, QTextInlineObject &object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd)
{
    Q_UNUSED(document)
    Q_UNUSED(object)
    Q_UNUSED(posInDocument)
    Q_UNUSED(format)
    Q_UNUSED(pd)
    object.setWidth(0); // set the width to 0 as otherwise it is negative which results in the text being moved to left
    object.setAscent(0);
    object.setDescent(0);
}

void KoTextSoftPageBreak::paint(QPainter &painter,
                                QPaintDevice *pd,
                                const QTextDocument *document,
                                const QRectF &rect,
                                const QTextInlineObject &object,
                                int posInDocument,
                                const QTextCharFormat &format)
{
    Q_UNUSED(painter)
    Q_UNUSED(pd)
    Q_UNUSED(document)
    Q_UNUSED(rect)
    Q_UNUSED(object)
    Q_UNUSED(posInDocument)
    Q_UNUSED(format)
    // TODO have a way to display the soft page break
}
