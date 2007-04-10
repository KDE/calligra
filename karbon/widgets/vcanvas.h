/* This file is part of the KDE project
   Copyright (C) 2001-2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2001-2003 Rob Buis <buis@kde.org>
   Copyright (C) 2003 Dirk Mueller <mueller@kde.org>
   Copyright (C) 2005 Laurent Montel <montel@kde.org>
   Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
   Copyright (C) 2006-2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2006 Sven Langkamp <sven.langkamp@gmail.com>
   Copyright (C) 2006 Boudewijn Rempt <boud@valdyas.org>
   Copyright (C) 2006 Thorsten Zachmann <t.zachmann@zagge.de>
   Copyright (C) 2006 Casper Boemann <cbr@boemann.dk>
   Copyright (C) 2006 Peter Simonsson <psn@linux.se>
   Copyright (C) 2007 David Faure <faure@kde.org>

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

#ifndef __VCANVAS_H__
#define __VCANVAS_H__


#include <QResizeEvent>
#include <QEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QPaintEvent>
#include <QTabletEvent>
#include <QWheelEvent>
#include <QFocusEvent>
#include <QWidget>
#include <QList>

#include <karbon_export.h>
#include <KoCanvasBase.h>
#include <KoZoomHandler.h>
#include <KoViewConverter.h>
#include <KoShape.h>

#include <QBrush>
#include <QPainter>

class QPointF;
class QRectF;
class KoViewConverter;
class KoShapeManager;
class KoToolProxy;
class VDocument;
class KarbonPart;

class KarbonCanvas: public QWidget, public KoCanvasBase
{
    Q_OBJECT

public:
    explicit KarbonCanvas( KarbonPart *p );
    virtual ~KarbonCanvas();

    /// reimplemented from KoCanvasBase
    virtual void gridSize(double *horizontal, double *vertical) const;
    /// reimplemented from KoCanvasBase
    virtual bool snapToGrid() const;
    /// reimplemented from KoCanvasBase
    virtual KoUnit unit();
    /// reimplemented from KoCanvasBase
    void addCommand(QUndoCommand *command);
    /// reimplemented from KoCanvasBase
    KoShapeManager *shapeManager() const;
    /// reimplemented from KoCanvasBase
    KoViewConverter *viewConverter();
    /// reimplemented from KoCanvasBase
    KoToolProxy * toolProxy();
    /// reimplemented from KoCanvasBase
    virtual QPoint documentOrigin();
    /// reimplemented from KoCanvasBase
    QWidget *canvasWidget();
    /// reimplemented from KoCanvasBase
    void updateCanvas(const QRectF& rc);

    /// Enables/disables showing page margins
    void setShowPageMargins( bool on );

    /**
    * Sets the viewing margin around the document in pt
    * @param margin the viewing margin around the document
    */
    void setDocumentViewMargin( int margin );

    /// Returns the viewing margin around the document
    int documentViewMargin() const;

    /**
     * Returns the document bounding rect with the viewing margin applied.
     * The rect is returned in pt.
     * @return the document bounding rect with viewing margins applied.
     */
    QRectF documentViewRect();

public slots:

    /**
     * Tell the canvas that it has to adjust its document origin.
     * The new origin depends on the current document size, the actual zoom factor
     * and the actual canvas size.
     */
    void adjustOrigin();
    void setDocumentOffset(const QPoint &offset);

signals:

    /**
     * This signal is emitted when the document origin has changed.
     * The document origin is the point (in pixel) on the virtual
     * canvas where the documents origin (0,0) or the top left
     * corner of the page is.
     */
    void documentOriginChanged( const QPoint &origin );

    /**
     * This signal is emitted when the document view rect has changed.
     * The document view rect is the union of the document page rect,
     * the bounding rects of the all document objects and with the
     * viewing margin applied.
     */
    void documentViewRectChanged( const QRectF &viewRect );

protected:
    void paintEvent(QPaintEvent * ev);
    void mouseEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void keyReleaseEvent (QKeyEvent *e);
    void keyPressEvent (QKeyEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void tabletEvent( QTabletEvent *e );
    void wheelEvent( QWheelEvent *e );
    void resizeEvent( QResizeEvent *e );

    QPoint widgetToView( const QPoint& p ) const;
    QRect widgetToView( const QRect& r ) const;
    QPoint viewToWidget( const QPoint& p ) const;
    QRect viewToWidget( const QRect& r ) const;

private:
    /// paint the grid
    void paintGrid( QPainter &painter, const KoViewConverter &converter, const QRectF &area );
    /// paint page margins
    void paintMargins( QPainter &painter, const KoViewConverter &converter );

    class KarbonCanvasPrivate;
    KarbonCanvasPrivate * const d;
};

#endif
