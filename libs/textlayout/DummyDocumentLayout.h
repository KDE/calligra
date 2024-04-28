/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef DUMMYDOCUMENTLAYOUT_H
#define DUMMYDOCUMENTLAYOUT_H

#include "kotextlayout_export.h"

#include <QAbstractTextDocumentLayout>

/**
 * Dummy TextLayouter that does nothing really, but without it the Table of Contents/Bibliography
 * can not be layout.TextLayouter
 * The real layout of the ToC/Bib still happens by the KoTextLayoutArea as part of
 * KoTextDocumentLayout of the main document
 *
 * You really shouldn't add anything to this class
 */

class KOTEXTLAYOUT_EXPORT DummyDocumentLayout : public QAbstractTextDocumentLayout
{
    Q_OBJECT
public:
    /// constructor
    explicit DummyDocumentLayout(QTextDocument *doc);
    ~DummyDocumentLayout() override;

    /// Returns the bounding rectangle of block.
    QRectF blockBoundingRect(const QTextBlock &block) const override;
    /**
     * Returns the total size of the document. This is useful to display
     * widgets since they can use to information to update their scroll bars
     * correctly
     */
    QSizeF documentSize() const override;

    /// Draws the layout on the given painter with the given context.
    void draw(QPainter *painter, const QAbstractTextDocumentLayout::PaintContext &context) override;

    QRectF frameBoundingRect(QTextFrame *) const override;

    /// reimplemented DO NOT CALL - USE HITTEST IN THE ROOTAREAS INSTEAD
    int hitTest(const QPointF &point, Qt::HitTestAccuracy accuracy) const override;

    /// reimplemented to always return 1
    int pageCount() const override;

    /// reimplemented from QAbstractTextDocumentLayout
    void documentChanged(int position, int charsRemoved, int charsAdded) override;
    /*
    protected:
        /// reimplemented
        virtual void drawInlineObject(QPainter *painter, const QRectF &rect, QTextInlineObject object, int position, const QTextFormat &format);
        /// reimplemented
        virtual void positionInlineObject(QTextInlineObject item, int position, const QTextFormat &format);
        /// reimplemented
        virtual void resizeInlineObject(QTextInlineObject item, int position, const QTextFormat &format);
    */
};

#endif
