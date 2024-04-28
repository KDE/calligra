/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTSOFTPAGEBREAK_H
#define KOTEXTSOFTPAGEBREAK_H

#include "KoInlineObject.h"

#include "kotext_export.h"

/**
 * This class defines a soft page break as defined in odf
 * <text:soft-page-break>
 *
 * The class does not have members as it's presence is enough.
 */
class KOTEXT_EXPORT KoTextSoftPageBreak : public KoInlineObject
{
    Q_OBJECT
public:
    KoTextSoftPageBreak();
    ~KoTextSoftPageBreak() override;

    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    void saveOdf(KoShapeSavingContext &context) override;

    void updatePosition(const QTextDocument *document, int posInDocument, const QTextCharFormat &format) override;

    void resize(const QTextDocument *document, QTextInlineObject &object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd) override;

    void paint(QPainter &painter,
               QPaintDevice *pd,
               const QTextDocument *document,
               const QRectF &rect,
               const QTextInlineObject &object,
               int posInDocument,
               const QTextCharFormat &format) override;
};

#endif /* KOTEXTSOFTPAGEBREAK_H */
