/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@valdyas.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPACANVASITEM_H
#define KOPACANVASITEM_H

#include <KoPACanvasBase.h>
#include <QGraphicsWidget>
#include <QList>

#include "kopageapp_export.h"

/// GraphicsWidget that shows a KoPAPage
class KOPAGEAPP_EXPORT KoPACanvasItem : public QGraphicsWidget, public KoPACanvasBase
{
    Q_OBJECT
public:
    explicit KoPACanvasItem(KoPADocument *doc);

    void repaint() override;

    void setCursor(const QCursor &cursor) override;

    QWidget *canvasWidget() override
    {
        return nullptr;
    }
    const QWidget *canvasWidget() const override
    {
        return nullptr;
    }

    QGraphicsObject *canvasItem() override
    {
        return this;
    }
    const QGraphicsObject *canvasItem() const override
    {
        return this;
    }

    /// reimplemented method
    void updateCanvas(const QRectF &rc) override;

    /// reimplemented method
    void updateInputMethodInfo() override;

    /// Recalculates the size of the canvas (needed when zooming or changing pagelayout)
    void updateSize() override;

public Q_SLOTS:

    void slotSetDocumentOffset(const QPoint &offset)
    {
        setDocumentOffset(offset);
    }

Q_SIGNALS:

    void documentSize(const QSize &size);

    /**
     * Emitted when the entire controller size changes
     * @param size the size in widget pixels.
     */
    void sizeChanged(const QSize &size);

    /// Emitted when updateCanvas has been called.
    void canvasUpdated();

protected:
    /// reimplemented method from superclass
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    /// reimplemented method from superclass
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    /// reimplemented method from superclass
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    /// reimplemented method from superclass
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    /// reimplemented method from superclass
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    /// reimplemented method from superclass
    void keyPressEvent(QKeyEvent *event) override;
    /// reimplemented method from superclass
    void keyReleaseEvent(QKeyEvent *event) override;
    /// reimplemented method from superclass
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;
    /// reimplemented method from superclass
    void closeEvent(QCloseEvent *event) override;
    /// reimplemented method from superclass
    bool event(QEvent *event) override;
    /// reimplemented method from superclass
    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;
    /// reimplemented method from superclass
    void inputMethodEvent(QInputMethodEvent *event) override;

    /// reimplemented method from superclass
    void resizeEvent(QGraphicsSceneResizeEvent *event) override;

    /**
     * Shows the default context menu
     * @param globalPos global position to show the menu at.
     * @param actionList action list to be inserted into the menu
     */
    void showContextMenu(const QPoint &globalPos, const QList<QAction *> &actionList);
};

#endif /* KOPACANVAS_H */
