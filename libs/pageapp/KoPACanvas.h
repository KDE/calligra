/* This file is part of the KDE project
   Copyright (C) 2006-2007 Thorsten Zachmann <zachmann@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KOPACANVAS_H
#define KOPACANVAS_H

#include <QWidget>
#include <QList>
#include <KoPACanvasBase.h>

#include "kopageapp_export.h"

/// Widget that shows a KoPAPage
class KOPAGEAPP_EXPORT KoPACanvas : public QWidget, public KoPACanvasBase
{
    Q_OBJECT
public:
    explicit KoPACanvas( KoPAViewBase * view, KoPADocument * doc, QWidget *parent = 0, Qt::WindowFlags f = 0);

    void repaint() override;

    QWidget* canvasWidget() override;
    const QWidget* canvasWidget() const override;

    /// reimplemented method
    void updateCanvas( const QRectF& rc ) override;

    /// reimplemented method
    void updateInputMethodInfo() override;

    /// Recalculates the size of the canvas (needed when zooming or changing pagelayout)
    void updateSize() override;

    void setCursor(const QCursor &cursor) override;

public Q_SLOTS:

    void slotSetDocumentOffset(const QPoint &offset) { setDocumentOffset(offset); }

Q_SIGNALS:
    void documentSize(const QSize &size);

    /**
     * Emitted when the entire controller size changes
     * @param size the size in widget pixels.
     */
    void sizeChanged( const QSize & size );

    /// Emitted when updateCanvas has been called.
    void canvasUpdated();

protected:
    /// reimplemented method from superclass
    bool event(QEvent *) override;
    /// reimplemented method from superclass
    void paintEvent( QPaintEvent* event ) override;
    /// reimplemented method from superclass
    void tabletEvent( QTabletEvent *event ) override;
    /// reimplemented method from superclass
    void mousePressEvent( QMouseEvent *event ) override;
    /// reimplemented method from superclass
    void mouseDoubleClickEvent( QMouseEvent *event ) override;
    /// reimplemented method from superclass
    void mouseMoveEvent( QMouseEvent *event ) override;
    /// reimplemented method from superclass
    void mouseReleaseEvent( QMouseEvent *event ) override;
    /// reimplemented method from superclass
    void keyPressEvent( QKeyEvent *event ) override;
    /// reimplemented method from superclass
    void keyReleaseEvent( QKeyEvent *event ) override;
    /// reimplemented method from superclass
    void wheelEvent ( QWheelEvent * event ) override;
    /// reimplemented method from superclass
    void closeEvent( QCloseEvent * event ) override;
    /// reimplemented method from superclass
    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;
    /// reimplemented method from superclass
    void inputMethodEvent(QInputMethodEvent *event) override;

    /// reimplemented method from superclass
    void resizeEvent( QResizeEvent * event ) override;

    /**
     * Shows the default context menu
     * @param globalPos global position to show the menu at.
     * @param actionList action list to be inserted into the menu
     */
    void showContextMenu( const QPoint& globalPos, const QList<QAction*>& actionList );
};

#endif /* KOPACANVAS_H */
