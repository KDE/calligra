/***************************************************************************
                          cv.cpp  -  description
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
#include "cv.h"
#include <qwmatrix.h>
#include <qevent.h>
#include <qpoint.h>
#include <qcanvas.h>
#include <qaction.h>
#include <qcursor.h>
#include <qpainter.h>
#include "mycanvas.h"
#include "canvbox.h"
#include "canvdefs.h"
#include "creportitem.h"
#include "clabel.h"
#include "cfield.h"
#include "cspecialfield.h"
#include "ccalcfield.h"
#include "cline.h"
#include "itemopts.h"

ReportCanvas::ReportCanvas(QCanvas * canvas, QWidget * parent, const char * name, WFlags f):
	QCanvasView(canvas, parent, name, f)
{
    selectedItem = 0;
    moving = 0;
    resizing = 0;
}

void ReportCanvas::contentsMousePressEvent(QMouseEvent* e)
{
    QPoint p = inverseWorldMatrix().QWMatrix::map(e->pos());
    QCanvasItemList l=canvas()->collisions(p);
//    QCanvasItemList::Iterator it=l.begin();
//    QString str = QString("%1").arg((*it)->rtti());
    switch (e->button())
    {
        case MidButton:
	    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
	    {
	        if ((*it)->rtti() > 2000)
	        {
		    (*it)->hide();
		    ((CanvasReportItem *)(*it))->section()->items.remove((*it));
		    delete (*it);
		    canvas()->update();
		    break;
	        }
		if ((*it)->rtti() > 1800)
		{
		    if ((*it)->rtti() == RttiDetail)
		    {
			CanvasDetail *det = (CanvasDetail*)(*it);
			if ( det->props["Level"].first.toInt() <
			    ((MyCanvas*)(canvas()))->templ->detailsCount - 1)
			    return;
		    }
		    CanvasDetailHeader *header = 0;
		    CanvasDetailFooter *footer = 0;
		    ((MyCanvas*)(canvas()))->templ->removeSection((CanvasBand *)(*it),
				&header, &footer);
		    (*it)->hide();
		    delete (*it);
		    if (header)
		    {
			header->hide();
			delete header;
		    }
		    if (footer)
		    {
			footer->hide();
			delete footer;
		    }
		    ((MyCanvas*)(canvas()))->templ->arrangeSections();
		    canvas()->update();
		    break;
		}
	    }
	    break;
	case RightButton:
	    //display editor for report items or sections

	    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
	    {
		if ((*it)->rtti() >= 1800) //for my own report items
		{
		    CanvasBox *l = (CanvasBox*)(*it);
		    dlgItemOptions *dlgOpts = new dlgItemOptions(&(l->props), this);
		    dlgOpts->exec();
		    delete dlgOpts;
		    (*it)->hide();
		    (*it)->show();
		    if ((*it)->rtti() < 2000)
			((MyCanvas *)(canvas()))->templ->arrangeSections();
		    canvas()->update();
		    break;
		}
	    }
	    break;
	case LeftButton:
	    if (selectedItem)
	    {
	    	bool used = false;
		    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
    		{
	    	    if ( ((*it)->rtti() > 1800) && (((*it)->rtti() < 2000)) )
    		    {
    			    selectedItem->setX(e->x());
		        	selectedItem->setY(e->y());
    	    		selectedItem->setSection((CanvasBand *)(*it));
        			selectedItem->updateGeomProps();
        			selectedItem->show();
       	    		((CanvasBand *)(*it))->items.append(selectedItem);
                    used = true;
    		    }
	    	}
    		if (!used)
        		delete selectedItem;
            selectedItem = 0;
      		emit selectedActionProcessed();
//            ((fmMain *)parent())->reportAddNothing->toggle();
	    }
	    else
	    {
		//allow user to move any item except for page rectangle
		for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
		{
		    if ((*it)->rtti() > 2001)
		    {
			CanvasReportItem *item = (CanvasReportItem *)(*it);
/*			    if (item->topLeftResizableRect().contains(e->pos()) ||
				item->bottomLeftResizableRect().contains(e->pos()) ||
				item->topRightResizableRect().contains(e->pos()) ||
				item->bottomRightResizableRect().contains(e->pos()))*/
			if (item->bottomRightResizableRect().contains(e->pos()))
			{
			    moving = 0;
			    resizing = item;
			    moving_start = p;
			    return;
			}
			else
			{
			    moving = item;
			    resizing = 0;
			    moving_start = p;
			    return;
			}
		    }
		}
		moving = 0;
		resizing = 0;
		break;
	    }
        default:
            break;
    }
}

void ReportCanvas::contentsMouseMoveEvent(QMouseEvent* e)
{
    QPoint p = inverseWorldMatrix().map(e->pos());

/*    QCanvasItemList l=canvas()->collisions(p);
  setCursor(QCursor(Qt::ArrowCursor));
    unsetCursor();
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
    {
	if ((*it)->rtti() > 2000)
	{
	    CanvasReportItem *item = (CanvasReportItem*)(*it);
	    if (item->bottomRightResizableRect().contains(e->pos()))
		setCursor(QCursor(Qt::SizeFDiagCursor));
	}
    }*/

    if ( moving )
    {
	moving->moveBy(p.x() - moving_start.x(),
		       p.y() - moving_start.y());
/*	attempt to prevent item collisions
        QCanvasItemList l=canvas()->collisions(moving->rect());
	if (l.count() > 2)
	{
	    moving->moveBy(-(p.x() - moving_start.x()),
			   -(p.y() - moving_start.y()));
	    canvas()->update();
	    return;
	}*/
	moving_start = p;
	moving->updateGeomProps();
	canvas()->update();
    }
    if (resizing)
    {
	QCanvasRectangle *r = (QCanvasRectangle *)resizing;
	int w = r->width() + p.x() - moving_start.x();
	int h = r->height() + p.y() - moving_start.y();
	if (((w > 10) && (h > 10)) || (resizing->rtti() == RttiCanvasLine))
	    r->setSize(w, h);
	moving_start = p;
	resizing->updateGeomProps();
	canvas()->update();
    }
}
#include "cv.moc"
