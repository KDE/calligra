/***************************************************************************
                          cv.h  -  description
                             -------------------
    begin                : 07.06.2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mksat.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef CV_H
#define CV_H

#include <qcanvas.h>

class CanvasReportItem;
class QMouseEvent;

class ReportCanvas: public QCanvasView{
    Q_OBJECT
public:
    ReportCanvas(QCanvas * canvas, QWidget * parent = 0, const char * name = 0, WFlags f = 0);
    
    CanvasReportItem *selectedItem;
protected:
    void contentsMousePressEvent(QMouseEvent*);
    void contentsMouseMoveEvent(QMouseEvent*);
private:
    CanvasReportItem *moving;
    QPoint moving_start;
    CanvasReportItem *resizing;
signals: // Signals
  /** Emitted when user clicks on the canvas, so a button
or a menu item assosiated with the selected item should
be unchecked. */
  void selectedActionProcessed();
};

#endif
