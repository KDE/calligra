/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>
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

#ifndef LAYOUTHELPER_H
#define LAYOUTHELPER_H

#include <KoTextDocumentLayout.h>
#include <KoInsets.h>

#include <QTextLine>
#include <QTextBlock>

class KoStyleManager;
class KoTextBlockData;
class KoTextShapeData;

/**
 * The document layouter for KoText style docs.
 */
class Layout : public KoTextDocumentLayout::LayoutState {
public:
    explicit Layout(KoTextDocumentLayout *parent);
    /// start layouting, return false when there is nothing to do
    bool start();
    /// end layouting
    void end();
    void reset();
    /// returns true if reset has been called.
    bool interrupted();
    double width();
    double x();
    double y();
    /// return the y offset of the document at start of shape.
    double docOffsetInShape() const;
    /// when a line is added, update internal vars.  Return true if line does not fit in shape
    bool addLine(QTextLine &line);
    /// prepare for next paragraph; return false if there is no next parag.
    bool nextParag();
    double documentOffsetInShape();

    /// paint the document
    void draw(QPainter *painter, const QAbstractTextDocumentLayout::PaintContext & context);

    void setStyleManager(KoStyleManager *sm) { m_styleManager = sm; }
    KoStyleManager *styleManager() const { return m_styleManager; }

private:
    void updateBorders();
    double topMargin();
    double listIndent();
    void cleanupShapes();
    void cleanupShape(KoShape *daShape);
    void nextShape();
    void decorateParagraph(QPainter *painter, const QTextBlock &block);

    void resetPrivate();

private:
    KoStyleManager *m_styleManager;

    double m_y;
    QTextBlock m_block;
    KoTextBlockData *m_blockData;

    QTextBlockFormat m_format;
    QTextBlock::Iterator m_fragmentIterator;
    KoTextShapeData *m_data;
    bool m_newShape, m_newParag, m_reset, m_isRtl;
    KoInsets m_borderInsets;
    KoInsets m_shapeBorder;
    KoTextDocumentLayout *m_parent;
};

#endif
