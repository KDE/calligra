/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPACANVAS_H
#define KOPACANVAS_H

#include <KoPACanvasBase.h>
#include <QList>
#include <QWidget>

#include "kopageapp_export.h"

/// Widget that shows a KoPAPage
class KOPAGEAPP_EXPORT KoPACanvas : public QWidget, public KoPACanvasBase
{
    Q_OBJECT
public:
    explicit KoPACanvas(KoPAViewBase *view, KoPADocument *doc, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    void repaint() override;

    QWidget *canvasWidget() override;
    const QWidget *canvasWidget() const override;

    /// reimplemented method
    void updateCanvas(const QRectF &rc) override;

    /// reimplemented method
    void updateInputMethodInfo() override;

    /// Recalculates the size of the canvas (needed when zooming or changing pagelayout)
    void updateSize() override;

    void setCursor(const QCursor &cursor) override;

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
    bool event(QEvent *) override;
    /// reimplemented method from superclass
    void paintEvent(QPaintEvent *event) override;
    /// reimplemented method from superclass
    void tabletEvent(QTabletEvent *event) override;
    /// reimplemented method from superclass
    void mousePressEvent(QMouseEvent *event) override;
    /// reimplemented method from superclass
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    /// reimplemented method from superclass
    void mouseMoveEvent(QMouseEvent *event) override;
    /// reimplemented method from superclass
    void mouseReleaseEvent(QMouseEvent *event) override;
    /// reimplemented method from superclass
    void keyPressEvent(QKeyEvent *event) override;
    /// reimplemented method from superclass
    void keyReleaseEvent(QKeyEvent *event) override;
    /// reimplemented method from superclass
    void wheelEvent(QWheelEvent *event) override;
    /// reimplemented method from superclass
    void closeEvent(QCloseEvent *event) override;
    /// reimplemented method from superclass
    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;
    /// reimplemented method from superclass
    void inputMethodEvent(QInputMethodEvent *event) override;

    /// reimplemented method from superclass
    void resizeEvent(QResizeEvent *event) override;

    /**
     * Shows the default context menu
     * @param globalPos global position to show the menu at.
     * @param actionList action list to be inserted into the menu
     */
    void showContextMenu(const QPoint &globalPos, const QList<QAction *> &actionList);
};

#endif /* KOPACANVAS_H */
