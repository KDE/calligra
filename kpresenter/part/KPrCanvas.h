/* This file is part of the KDE project
   Copyright (C) 2006 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef KPRCANVAS_H
#define KPRCANVAS_H

#include <QWidget>
#include <KoCanvasBase.h>

class KPrView;
class KPrDocument;

class KPrCanvas : public QWidget, public KoCanvasBase
{
    Q_OBJECT
public:
    KPrCanvas( KPrView * view, KPrDocument * doc );
    ~KPrCanvas();
    
    void gridSize( double *horizontal, double *vertical ) const;
    bool snapToGrid() const;
    void addCommand( KCommand *command, bool execute = true );
    KoShapeManager * shapeManager() const;
    void updateCanvas( const QRectF& rc );

    KoToolProxy * toolProxy() { return m_toolProxy; }
    KoViewConverter *viewConverter();
    QWidget* canvasWidget() { return this; }
    KoUnit unit();

protected:    
    void paintEvent( QPaintEvent* event );
    void tabletEvent( QTabletEvent *event );
    void mousePressEvent( QMouseEvent *event );
    void mouseDoubleClickEvent( QMouseEvent *event );
    void mouseMoveEvent( QMouseEvent *event );
    void mouseReleaseEvent( QMouseEvent *event );
    void keyPressEvent( QKeyEvent *event );
    void keyReleaseEvent( QKeyEvent *event );
    void wheelEvent ( QWheelEvent * event );

    KPrView * m_view;
    KPrDocument * m_doc;
    KoShapeManager * m_shapeManager;
    KoToolProxy * m_toolProxy;
};

#endif /* KPRCANVAS_H */
