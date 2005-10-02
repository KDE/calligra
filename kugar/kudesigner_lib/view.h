/* This file is part of the KDE project
  Copyright (C) 2002-2004 Alexander Dymo <adymo@mksat.net>

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
#ifndef VIEW_H
#define VIEW_H

#include <qcanvas.h>
#include <qpainter.h>
#include <qptrlist.h>

#include <koproperty/property.h>

class KuDesignerPlugin;

class QMouseEvent;
class QCanvasItemList;

namespace KoProperty
{
class Buffer;
}

using namespace KoProperty;

namespace Kudesigner
{

class ReportItem;
class Box;
class Canvas;
class Band;

class SelectionRect: public QCanvasRectangle
{
public:
    SelectionRect( int x, int y, int width, int height, Canvas *canvas ) :
            QCanvasRectangle( x, y, width, height, ( QCanvas* ) canvas )
    {}

    virtual void draw( QPainter & painter );
};

class View: public QCanvasView
{
    Q_OBJECT
public:
    View( Canvas *canvas, QWidget *parent, const char *name = 0, WFlags f = 0 );

    int itemToInsert;

    enum RequestType {RequestNone = 0, RequestProps, RequestDelete};

    void setRequest( RequestType r );
    void clearRequest();
    bool requested();

    void finishSelection();

    void setPlugin( KuDesignerPlugin *plugin );

    void setCanvas( Canvas *canvas );

protected:
    void contentsMousePressEvent( QMouseEvent* );
    void contentsMouseReleaseEvent( QMouseEvent* );
    void contentsMouseMoveEvent( QMouseEvent* );
    void contentsMouseDoubleClickEvent( QMouseEvent * );

    void contentsDragEnterEvent ( QDragEnterEvent * );
    void contentsDragMoveEvent ( QDragMoveEvent * );
    //    void contentsDragLeaveEvent ( QDragLeaveEvent * );
    //    void contentsDropEvent ( QDropEvent * );
    void keyPressEvent( QKeyEvent * );

    void startMoveOrResizeOrSelectItem( QCanvasItemList &l, QMouseEvent *e, QPoint &p );
    bool startResizing( QMouseEvent *e, QPoint &p );
    void placeItem( QCanvasItemList &l, QMouseEvent *e );
    void editItem( QCanvasItemList &l );
    void deleteItem( QCanvasItemList &l );
    void selectItemFromList( QCanvasItemList &l );

    void stickToGrid( double &x, double &y );
    void stickDimToGrid( double x, double y, double &X, double &Y );

private:
    Buffer *selectionBuf;

    ReportItem *moving;
    QPoint moving_start;
    double moving_offsetY;
    double moving_offsetX;
    QRect resizing_constraint;
    QSize resizing_minSize;
    int resizing_type;
    class Box *resizing;
    bool selectionStarted;

    KuDesignerPlugin *m_plugin;

    SelectionRect *selectionRect;

    RequestType request;

    Canvas *m_canvas;

    void fixMinValues( double &pos, double minv, double &offset );
    void fixMaxValues( double &pos, double size, double maxv, double &offset );

signals:  // Signals
    /** Emitted when user clicks on the canvas, so a button
    or a menu item assosiated with the selected item should
    be unchecked. */
    void selectedActionProcessed();
    void selectedEditActionProcessed();
    void modificationPerformed();

    /** Emitted when selection is made, so that property editor can display properties
        for the selected items. */
    void selectionMade( Buffer *buf );
    void selectionClear();
    void changed();

    void itemPlaced( int x, int y, int band, int bandLevel );

public slots:
    void updateProperty();

    void selectItem();
    void setGridSize( int size );
};

}

#endif
