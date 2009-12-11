/* This file is part of the KDE project

   Copyright 2007 Johannes Simon <johannes.simon@gmail.com>

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


#ifndef KCHARTCANVAS_H
#define KCHARTCANVAS_H

// KOffice
#include <KoCanvasBase.h>

// Qt
#include <QWidget>

class QPaintEvent;
class QUndoCommand;
class KoShapeManager;
class KoToolProxy;
class KoViewConverter;
class KoUnit;

namespace KChart
{

class KChartPart;
class KChartView;

class KChartCanvas : public QWidget, public KoCanvasBase
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param parent the parent widget
     */
    KChartCanvas( KChartView *view, KChartPart *parent );
    ~KChartCanvas();

    virtual void gridSize( qreal*, qreal* ) const;
    virtual bool snapToGrid() const;
    virtual void addCommand( QUndoCommand* );
    virtual KoShapeManager *shapeManager() const;
    virtual void updateCanvas( const QRectF& );
    virtual KoToolProxy *toolProxy() const;
    virtual const KoViewConverter *viewConverter() const;
    virtual KoUnit unit() const;
    virtual void updateInputMethodInfo();

    QRectF documentViewRect();
    QPoint widgetToView( const QPoint &point );
    QPoint viewToWidget( const QPoint &point );
    QRect  widgetToView( const QRect &rect );
    QRect  viewToWidget( const QRect &rect );

    QWidget *canvasWidget() { return this; }
    const QWidget *canvasWidget() const { return this; }

public slots:
    void adjustOrigin();
    void setDocumentOffset( const QPoint &point );

signals:
    void documentViewRectChanged( const QRectF &viewRect );
    void documentOriginChanged( const QPoint &origin );

protected:
    void paintEvent(QPaintEvent * ev);
    void mouseReleaseEvent(QMouseEvent *e);
    void keyReleaseEvent (QKeyEvent *e);
    void keyPressEvent (QKeyEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void tabletEvent( QTabletEvent *e );
    void wheelEvent( QWheelEvent *e );
    void resizeEvent( QResizeEvent *e );
    virtual QVariant inputMethodQuery( Qt::InputMethodQuery query ) const;
    virtual void inputMethodEvent( QInputMethodEvent *e );

private:
    KChartPart     *m_part;
    KChartView     *m_view;
    KoShapeManager *m_shapeManager;
    KoToolProxy    *m_toolProxy;

    QRectF          m_documentRect;
    QPoint          m_origin;
    QPoint          m_documentOffset;
};

}

#endif
