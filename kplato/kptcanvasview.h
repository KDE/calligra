/* This file is part of the KDE project
   Copyright (C) 2002 The Koffice Team <koffice@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KPTCANVASVIEW_H
#define KPTCANVASVIEW_H

#include "kptnode.h"

#include <qcanvas.h>

class KPTProjectList;
class KPTTimeScale;
class QTimer;
class QPainter;
class QListViewItem;
class QPoint;

class KPTCanvasView : public QCanvasView
{
    Q_OBJECT

public:
    KPTCanvasView( QWidget *parent );
    virtual ~KPTCanvasView();

    void drawGantt( const KPTProjectList *list, KPTTimeScale *timescale );
    void drawPert();
    void clear();
/*    
    void repaint( bool erase = false );
    bool eventFilter( QObject *o, QEvent *e );
*/
public slots:
    void setContentsPos( int x, int y );
    
/*
protected:
    virtual void keyPressEvent( QKeyEvent *e );
    virtual void contentsMousePressEvent( QMouseEvent *e );
    virtual void contentsMouseMoveEvent( QMouseEvent *e );
    virtual void contentsMouseReleaseEvent( QMouseEvent *e );
    virtual void contentsMouseDoubleClickEvent( QMouseEvent *e );
    virtual void contentsDragEnterEvent( QDragEnterEvent *e );
    virtual void contentsDragMoveEvent( QDragMoveEvent *e );
    virtual void contentsDragLeaveEvent( QDragLeaveEvent *e );
    virtual void contentsDropEvent( QDropEvent *e );
    virtual void resizeEvent( QResizeEvent *e );

private slots:
   void slotContentsMoving( int, int );
*/

protected:
    void contentsMouseReleaseEvent ( QMouseEvent * e );

signals:
    void rightButtonPressed(QListViewItem *item, const QPoint & point, int col);
    
private:
    QCanvas *m_canvas;

    QTimer *m_scrollTimer;
    bool m_mousePressed;
    bool m_printing;
    
    const KPTProjectList *m_projectList;
    
    bool m_linkMode;
    KPTNode *m_linkParentNode;
    
#ifndef NDEBUG
    void printDebug( int );
#endif

};
#endif
