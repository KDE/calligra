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
#include <qbitmap.h>
#include <qimage.h>

#include <qprinter.h>

#include "mycanvas.h"
#include "canvbox.h"
#include "canvdefs.h"
#include "creportitem.h"
#include "clabel.h"
#include "cfield.h"
#include "cspecialfield.h"
#include "ccalcfield.h"
#include "cline.h"

void SelectionRect::draw(QPainter & painter)
{
//    painter.setPen(Qt::NoPen);

/*  QPrinter pr;
  if ( pr.setup() ) {
    QPainter p(&pr);
    canvas()->drawArea( canvas()->rect(), &p );
  } */

/*    qDebug("creating pixmap");
    QPixmap mp(rect().size());
    qDebug("creating painter");
    QPainter p(&mp);
    qDebug("filling pixmap");
    canvas()->drawArea(canvas()->rect(), &p);
    qDebug("converting to image");
    QImage im = mp.convertToImage();
    if (!im.isNull())
    {
        qDebug("do dither");
        mp.convertFromImage(im,  Qt::OrderedAlphaDither);

        qDebug("creating brush");
        QBrush br(KGlobalSettings::highlightColor(),Qt::CustomPattern);
        br.setPixmap(mp);
        painter.setBrush(br);
    }
    qDebug("drawing");*/
//    painter.drawRect(rect());
    QCanvasRectangle::draw(painter);
}



ReportCanvas::ReportCanvas(QCanvas * canvas, QWidget * parent, const char * name, WFlags f):
	QCanvasView(canvas, parent, name, f)
{
    itemToInsert = 0;
    moving = 0;
    resizing = 0;
    selectionStarted = 0;
    selected.setAutoDelete(false);
    request = RequestNone;
    selectionRect = new SelectionRect(0, 0, 0, 0, canvas);
}

void ReportCanvas::deleteItem(QCanvasItemList &l)
{
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
    {
        if ((*it)->rtti() > 2000)
        {
	    (*it)->hide();
	    ((CanvasReportItem *)(*it))->section()->items.remove((*it));
	    delete (*it);
	    canvas()->update();
	    emit modificationPerformed();
	    break;
        }
	if ((*it)->rtti() > 1800)
	{
/*	    if ((*it)->rtti() == RttiDetail)
	    {
		CanvasDetail *det = (CanvasDetail*)(*it);
		if ( det->props["Level"].first.toInt() <
		    ((MyCanvas*)(canvas()))->templ->detailsCount - 1)
		    return;
	    }*/
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
	    emit modificationPerformed();
	    break;
	}
    }
}

void ReportCanvas::editItem(QCanvasItemList &l)
{
    //display editor for report items or sections
/*  for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
    {
	if ((*it)->rtti() >= 1800) //for my own report items
	{
	    CanvasBox *l = (CanvasBox*)(*it);
	    dlgItemOptions *dlgOpts = new dlgItemOptions(&(l->props), this);
	    dlgOpts->exec();
	    delete dlgOpts;
	    if ((*it)->rtti() == RttiKugarTemplate)
		((CanvasKugarTemplate*)(*it))->updatePaperProps();
	    (*it)->hide();
	    (*it)->show();
	    if ((*it)->rtti() < 2000)
		((MyCanvas *)(canvas()))->templ->arrangeSections();
	    canvas()->update();
	    emit modificationPerformed();
	    break;
	}
    }*/
}

void ReportCanvas::selectItemFromList(QCanvasItemList &l)
{
    //display editor for report items or sections
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
    {
        if ((*it)->rtti() > 2001) //for report items but not bands
        {
            CanvasBox *b = (CanvasBox*)(*it);
            if (!selected.containsRef(b))
            {
                selectItem(b, false);
                canvas()->update();
//                qWarning("selected item set");
//                selected->drawHolders();
                return;
            }
            if (selected.containsRef(b)) return;
        }
    }
    unselectAll();
//    qWarning("unselect");
}


void ReportCanvas::placeItem(QCanvasItemList &l, QMouseEvent *e)
{
    bool used = false;
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
    {
        if ( ((*it)->rtti() > 1800) && (((*it)->rtti() < 2000)) )
        {
            itemToInsert->setX(e->x());
            itemToInsert->setY(e->y());
            itemToInsert->setSection((CanvasBand *)(*it));
            itemToInsert->updateGeomProps();

            selectItem(itemToInsert, false);

            itemToInsert->show();
            ((CanvasBand *)(*it))->items.append(itemToInsert);
            used = true;
            emit modificationPerformed();
        }
    }
    if (!used)
        delete itemToInsert;
    itemToInsert = 0;
    emit selectedActionProcessed();
}

void ReportCanvas::startMoveOrResizeOrSelectItem(QCanvasItemList &l,
    QMouseEvent *e, QPoint &p)
{
    //allow user to move any item except for page rectangle
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
    {
        if ((*it)->rtti() > 2001)
        {

    	    CanvasReportItem *item = (CanvasReportItem *)(*it);
/*	    if (item->topLeftResizableRect().contains(e->pos()) ||
	    item->bottomLeftResizableRect().contains(e->pos()) ||
	    item->topRightResizableRect().contains(e->pos()) ||
	    item->bottomRightResizableRect().contains(e->pos()))*/
            moving_start = p;
            if (item->bottomRightResizableRect().contains(e->pos()))
            {
                moving = 0;
                resizing = item;
                return;
            }
            else
            {
                moving = item;
                resizing = 0;
                return;
            }
        }
    }
    moving = 0;
    resizing = 0;
    selectionStarted = 1;
    selectionRect->setX(p.x());
    selectionRect->setY(p.y());
    selectionRect->setZ(50);
    selectionRect->show();
}

void ReportCanvas::contentsMousePressEvent(QMouseEvent* e)
{
    QPoint p = inverseWorldMatrix().QWMatrix::map(e->pos());
    QCanvasItemList l=canvas()->collisions(p);

    //if there is a request for properties or for delete operation
    //perform that and do not take care about mouse buttons

//    qWarning("mouse press");

    switch (request)
    {
        case RequestProps:
            clearRequest();
            editItem(l);
            return;
        case RequestDelete:
            deleteItem(l);
            clearRequest();
            return;
        case RequestNone:
            break;
    }

    moving = 0;
    resizing = 0;
    selectionStarted = 0;
    switch (e->button())
    {
        case LeftButton:
            if (itemToInsert)
            {
//                qWarning("placing item");
                placeItem(l, e);
            }
            else
            {
//                qWarning("starting move or resize");
                selectItemFromList(l);
                startMoveOrResizeOrSelectItem(l, e, p);
            }
            break;
        default:
            break;
    }
}

void ReportCanvas::contentsMouseReleaseEvent(QMouseEvent* e)
{
    selectionRect->setSize(0, 0);
    selectionRect->setX(0);
    selectionRect->setY(0);
    selectionRect->hide();

    QPoint p = inverseWorldMatrix().QWMatrix::map(e->pos());
    QCanvasItemList l=canvas()->collisions(p);

    switch (e->button())
    {
/*        case LeftButton:
            selectItem(l);
            break;*/
        case MidButton:
            deleteItem(l);
            break;
        case RightButton:
            editItem(l);
            break;
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
        emit modificationPerformed();
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
        emit modificationPerformed();
    }
    if (selectionStarted)
    {
/*        qDebug("x_start = %d, y_start = %d, x_end = %d, y_end = %d", moving_start.x(),
            moving_start.y(), e->pos().x(), e->pos().y());*/
        selectionRect->setSize(e->pos().x() - selectionRect->x(),
            e->pos().y() - selectionRect->y());
        unselectAll();
        QCanvasItemList l = canvas()->collisions(selectionRect->rect());
        for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
        {
            if ( ((*it)->rtti() > 2001) &&
                (selectionRect->rect().contains(((CanvasBox*)(*it))->rect())) )
            {
                selectItem((CanvasBox*)(*it));
                canvas()->update();
            }
        }
    }
}

void ReportCanvas::setRequest(RequestType r)
{
    switch (r)
    {
	case RequestProps:
	    QApplication::restoreOverrideCursor();
	    QApplication::setOverrideCursor(Qt::PointingHandCursor);
	    break;
	case RequestDelete:
	    QApplication::restoreOverrideCursor();
	    QApplication::setOverrideCursor(Qt::ForbiddenCursor);
	    break;
	case RequestNone:
	    QApplication::restoreOverrideCursor();
	    break;
    }
    request = r;
}

void ReportCanvas::clearRequest()
{
    QApplication::restoreOverrideCursor();
    request = RequestNone;
    emit selectedEditActionProcessed();
}
	
bool ReportCanvas::requested()
{
    if (request == RequestNone)
	return false;
    else
	return true;
}

void ReportCanvas::unselectAll()
{
    CanvasBox *b;

    for (b = selected.first(); b; b = selected.next())
        unselectItem(b);
}

void ReportCanvas::selectAll()
{
    for (QCanvasItemList::Iterator it=canvas()->allItems().begin(); it!=canvas()->allItems().end(); ++it)
    {
        if ((*it)->rtti() > 2001)
        {
            selectItem((CanvasBox*)(*it));
        }
    }
}

void ReportCanvas::selectItem(CanvasBox *it, bool addToSelection)
{
    if (!addToSelection)
        unselectAll();
    selected.append(it);
    it->setSelected(true);
}

void ReportCanvas::unselectItem(CanvasBox *it)
{
    selected.remove(it);
    it->setSelected(false);
}

#include "cv.moc"
