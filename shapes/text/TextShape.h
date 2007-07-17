/* This file is part of the KDE project
 * Copyright (c) 2006 Boudewijn Rempt <boud@valdyas.org>
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

#ifndef KOTEXTSHAPE_H
#define KOTEXTSHAPE_H

#include <KoShapeContainer.h>
#include <KoTextShapeData.h>

#include <QTextDocument>
#include <QPainter>

#define TextShape_SHAPEID "TextShapeID"

/**
 * A text shape.
 * The Text shape is capable of drawing structured text.
 * @see KoTextShapeData
 */
class TextShape : public KoShapeContainer {
public:
    TextShape();
    virtual ~TextShape();

    /// reimplemented
    void paintComponent(QPainter &painter, const KoViewConverter &converter);
    /// reimplemented
    void paintDecorations(QPainter &painter, const KoViewConverter &converter, const KoCanvasBase *canvas);

    /// helper method.
    QPointF convertScreenPos(const QPointF &point);

    /**
     * Set the shape's text to be demot text or not.
     * If true, replace the content with an lorem ipsum demo text and don't complain
     *   when there is not enough space at the end
     * If false; remove the demo text again.
     */
    void setDemoText(bool on);
    /// return if the content of this shape is demo text.
    bool demoText() const { return m_demoText; }

    // reimplemented
    virtual void saveOdf(KoShapeSavingContext & context) const;
    // reimplemented
    virtual bool loadOdf( const KoXmlElement & element, KoShapeLoadingContext &context );

    KoTextShapeData *textShapeData() { return m_textShapeData; }

    bool hasFootnoteDocument() { return m_footnotes != 0; }
    QTextDocument *footnoteDocument();

private:
    void shapeChanged(ChangeType type);

    KoTextShapeData *m_textShapeData;
    QTextDocument *m_footnotes;

    bool m_demoText;
};

#endif
