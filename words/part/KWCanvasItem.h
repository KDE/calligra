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
#include "kword_export.h"

#include <QGraphicsWidget>

class QRect;
class QPainter;

class KWGui;
class KWView;
class KoToolProxy;
class KoShape;

/**
 * This class is responsible for the rendering of the frames to
 * the screen as well as the interaction with the user via mouse
 * and keyboard. There is one per view.
 */
class KWCanvasItem : public QGraphicsWidget, public KWCanvasBase
{
    Q_OBJECT

public:
    /**
     * Constructor
     * Creates a new canvas widget that can display pages and frames.
     * @param viewMode the initial KWViewMode this canvas should use
     * @param document as this is one view in the MVC design; the document holds all content
     * @param parent the parent widget.
     */
    KWCanvasItem(const QString &viewMode, KWDocument *document);
    virtual ~KWCanvasItem();

    /// ask the widget to set the size this canvas takes to display all content
    void updateSize();

    // KoCanvasBase interface methods.
    /// reimplemented method from superclass
    virtual bool snapToGrid() const;

    /// reimplemented method from superclass
    virtual QWidget *canvasWidget() {
        return 0;
    }

    /// reimplemented method from superclass
    virtual const QWidget *canvasWidget() const {
        return 0;
    }

    /// reimplemented method from superclass
    virtual QGraphicsWidget *canvasItem() {
        return this;
    }

    /// reimplemented method from superclass
    virtual const QGraphicsWidget *canvasItem() const {
        return this;
    }

    /// reimplemented method from superclass
    virtual void updateInputMethodInfo();

    virtual void setCursor(const QCursor &cursor);

public slots:
    /**
     * sets the document offset in the scrollArea
     * @param offset the offset, in pixels.
     */
    void setDocumentOffset(const QPoint &offset);

signals:
    /**
     * emitted when the contentsSize changes.
     * @see KWViewMode::contentsSize
     * @param size the content area size, in pixels.
     */
    void documentSize(const QSizeF &size);

protected: //QGraphicsWidget
    /// reimplemented method from superclass
    virtual void keyPressEvent(QKeyEvent *e);

    /// reimplemented method from superclass
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *e);

    /// reimplemented method from superclass
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *e);

    /// reimplemented method from superclass
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);

    /// reimplemented method from superclass
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e);

    /// reimplemented method from superclass

    virtual void keyReleaseEvent(QKeyEvent *e);

    /// reimplemented method from superclass
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    /// reimplemented method from superclass
    //    virtual void tabletEvent(QTabletEvent *e);

    /// reimplemented method from superclass
    virtual void wheelEvent(QGraphicsSceneWheelEvent *e);

    /// reimplemented method from superclass
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;

    /// reimplemented method from superclass
    virtual void inputMethodEvent(QInputMethodEvent *event);

    /// reimplemented method from superclass
    virtual void updateCanvasInternal(const QRectF &clip) { update(clip); }

private slots:
    /// Called whenever there was a page added/removed or simply resized.
    void pageSetupChanged();
};

#endif
