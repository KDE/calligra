/*
 *  Copyright (c) 2006-2007 Thorsten Zachmann <zachmann@kde.org>
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _CANVAS_H
#define _CANVAS_H

#include <QWidget>
#include <QList>
#include <KoCanvasBase.h>

#include "kopageapp_export.h"

class KoPAView;
class Document;
class KAction;

/// Widget that shows a KoPAPage
class KOPAGEAPP_EXPORT Canvas : public QWidget, public KoCanvasBase
{
    Q_OBJECT
public:
    explicit Canvas( KoPAView * view, Document * doc );
    ~Canvas();

    /// Returns pointer to the KoPADocument
    Document* document() const { return m_doc; }

    /// reimplemented method
    virtual void gridSize( qreal *horizontal, qreal *vertical ) const;
    /// reimplemented method
    virtual bool snapToGrid() const;
    /// reimplemented method
    virtual void addCommand( QUndoCommand *command );
    /// reimplemented method
    virtual KoShapeManager * shapeManager() const;
    /// reimplemented method
    virtual void updateCanvas( const QRectF& rc );
    /// reimplemented method
    virtual void updateInputMethodInfo();
    /// reimplemented from KoCanvasBase
    virtual KoGuidesData * guidesData();

    KoToolProxy * toolProxy() const { return m_toolProxy; }
    const KoViewConverter *viewConverter() const;
    QWidget* canvasWidget() { return this; }
    const QWidget* canvasWidget() const { return this; }
    KoUnit unit() const;
    const QPoint & documentOffset() const;

    KoPAView* koPAView () const { return m_view; }

public slots:
    void setDocumentOffset(const QPoint &offset);

signals:
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
    void paintEvent( QPaintEvent* event );
    /// reimplemented method from superclass
    void tabletEvent( QTabletEvent *event );
    /// reimplemented method from superclass
    void mousePressEvent( QMouseEvent *event );
    /// reimplemented method from superclass
    void mouseDoubleClickEvent( QMouseEvent *event );
    /// reimplemented method from superclass
    void mouseMoveEvent( QMouseEvent *event );
    /// reimplemented method from superclass
    void mouseReleaseEvent( QMouseEvent *event );
    /// reimplemented method from superclass
    void keyPressEvent( QKeyEvent *event );
    /// reimplemented method from superclass
    void keyReleaseEvent( QKeyEvent *event );
    /// reimplemented method from superclass
    void wheelEvent ( QWheelEvent * event );
    /// reimplemented method from superclass
    void closeEvent( QCloseEvent * event );
    /// reimplemented method from superclass
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;
    /// reimplemented method from superclass
    virtual void inputMethodEvent(QInputMethodEvent *event);

    /// reimplemented method from superclass
    virtual void resizeEvent( QResizeEvent * event );

    /**
     * Shows the default context menu
     * @param globalPos global position to show the menu at.
     * @param actionList action list to be inserted into the menu
     */
    void showContextMenu( const QPoint& globalPos, const QList<QAction*>& actionList );

    KoPAView * m_view;
    Document * m_doc;
    KoShapeManager * m_shapeManager;
    KoToolProxy * m_toolProxy;
    QPoint m_documentOffset;
};

#endif /* KOPACANVAS_H */
