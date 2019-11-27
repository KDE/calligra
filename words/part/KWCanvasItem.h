/* This file is part of the KDE project

   Copyright (C) 2010 Boudewijn Rempt <boud@valdyas.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KWCANVASITEM_H
#define KWCANVASITEM_H

#include "KWDocument.h"
#include "KWCanvasBase.h"
#include "KWViewMode.h"
#include "words_export.h"

#include <QGraphicsWidget>

class QRect;
class QPainter;

class KWView;

/**
 * This class is responsible for the rendering of the frames to
 * the screen as well as the interaction with the user via mouse
 * and keyboard. There is one per view.
 */
class WORDS_EXPORT KWCanvasItem : public QGraphicsWidget, public KWCanvasBase
{
    Q_OBJECT

public:
    /**
     * Constructor
     * Creates a new canvas widget that can display pages and frames.
     * @param viewMode the initial KWViewMode this canvas should use
     * @param document as this is one view in the MVC design; the document holds all content
     */
    KWCanvasItem(const QString &viewMode, KWDocument *document);
    ~KWCanvasItem() override;

    /// ask the widget to set the size this canvas takes to display all content
    void updateSize();

    // KoCanvasBase interface methods.
    /// reimplemented method from superclass
    bool snapToGrid() const override;

    /// reimplemented method from superclass
    QWidget *canvasWidget() override {
        return 0;
    }

    /// reimplemented method from superclass
    const QWidget *canvasWidget() const override {
        return 0;
    }

    /// reimplemented method from superclass
    QGraphicsObject *canvasItem() override {
        return this;
    }

    /// reimplemented method from superclass
    const QGraphicsObject *canvasItem() const override {
        return this;
    }

    /// reimplemented method from superclass
    void updateInputMethodInfo() override;

    /// reimplemented method from superclass
    void updateCanvas(const QRectF &rc) override;

    void setCursor(const QCursor &cursor) override;

public Q_SLOTS:
    /**
     * sets the document offset in the scrollArea
     * @param offset the offset, in pixels.
     */
    void setDocumentOffset(const QPoint &offset);

Q_SIGNALS:
    /**
     * emitted when the contentsSize changes.
     * @see KWViewMode::contentsSize
     * @param size the content area size, in pixels.
     */
    void documentSize(const QSizeF &size);

    /// Emitted when updateCanvas has been called.
    void canvasUpdated();

protected: //QGraphicsWidget
    /// reimplemented method from superclass
    void keyPressEvent(QKeyEvent *e) override;

    /// reimplemented method from superclass
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e) override;

    /// reimplemented method from superclass
    void mousePressEvent(QGraphicsSceneMouseEvent *e) override;

    /// reimplemented method from superclass
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e) override;

    /// reimplemented method from superclass
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e) override;

    /// reimplemented method from superclass

    void keyReleaseEvent(QKeyEvent *e) override;

    /// reimplemented method from superclass
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

    /// reimplemented method from superclass
    //    virtual void tabletEvent(QTabletEvent *e);

    /// reimplemented method from superclass
    void wheelEvent(QGraphicsSceneWheelEvent *e) override;

    /// reimplemented method from superclass
    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;

    /// reimplemented method from superclass
    void inputMethodEvent(QInputMethodEvent *event) override;

    /// reimplemented method from superclass
    void updateCanvasInternal(const QRectF &clip) override { update(clip); }

private Q_SLOTS:
    /// Called whenever there was a page added/removed or simply resized.
    void pageSetupChanged();
};

#endif
