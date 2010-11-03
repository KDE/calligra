/* This file is part of the KDE project
 * Copyright (C) 2002-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>
 * Copyright (C) 2009 Inge Wallin <inge@lysator.liu.se>
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
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
#ifndef KWCANVASBASE_H
#define KWCANVASBASE_H

#include "KWDocument.h"
#include "kword_export.h"
#include "KWViewMode.h"

#include <KoCanvasBase.h>
#include <QRectF>

class QRect;
class QPainter;

class KWGui;
class KoToolProxy;
class KoShape;
class KoViewConverter;

class KWORD_EXPORT KWCanvasBase : public KoCanvasBase
{
public:
    KWCanvasBase(KWDocument *document, QObject *parent = 0);
    ~KWCanvasBase();

public: // KoCanvasBase interface methods.

    /// reimplemented method from superclass
    virtual void gridSize(qreal *horizontal, qreal *vertical) const;

    /// reimplemented method from superclass
    virtual void addCommand(QUndoCommand *command);

    /// reimplemented method from superclass
    virtual KoShapeManager *shapeManager() const;

    /// reimplemented method from superclass
    virtual void updateCanvas(const QRectF &rc);

    /// reimplemented method from superclass
    virtual KoUnit unit() const;

    /// reimplemented method from superclass
    virtual KoToolProxy *toolProxy() const;

    /// reimplemented method from superclass
    virtual void clipToDocument(const KoShape *shape, QPointF &move) const;

    /// reimplemented method from superclass
    virtual KoGuidesData *guidesData();

    /// reimplemented method from superclass
    virtual const KoViewConverter *viewConverter() const;

    /// return the document that this canvas works on
    KWDocument *document() const;

    /// return the viewMode currently associated with this canvas
    KWViewMode *viewMode() const;

    /// reimplemented method from superclass
    virtual void ensureVisible(const QRectF &rect);

protected:

    void paint(QPainter &painter, const QRectF &paintRect);

    void paintPageDecorations(QPainter &painter, KWViewMode::ViewMap &viewMap);

    void paintBorder(QPainter &painter, const KoBorder &border, const QRectF &borderRect) const;

    /**
     * paint one border along one of the 4 sides.
     * @param inwardsX is the horizontal vector (with value -1, 0 or 1) for the vector
     * pointing inwards for the border part nearest the center of the page.
     * @param inwardsY is the vertical vector (with value -1, 0 or 1) for the vector
     * pointing inwards for the border part nearest the center of the page.
     */
    void paintBorderSide(QPainter &painter, const KoBorder::BorderData &borderData,
                         const QPointF &lineStart, const QPointF &lineEnd, qreal zoom,
                         int inwardsX, int inwardsY) const;

    virtual void updateCanvasInternal(const QRectF &clip) = 0;

protected:

    KWDocument *m_document;
    KoShapeManager *m_shapeManager;
    KoToolProxy *m_toolProxy;
    KWViewMode *m_viewMode;
    QPoint m_documentOffset;
    KoViewConverter *m_viewConverter;
};

#endif // KWCANVASBASE_H
