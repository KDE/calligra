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
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/
#include <algorithm>
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
#include <kdebug.h>

#include "mycanvas.h"
#include "canvbox.h"
#include "canvdefs.h"
#include "creportitem.h"
#include "clabel.h"
#include "cfield.h"
#include "cspecialfield.h"
#include "ccalcfield.h"
#include "cline.h"
#include "canvkutemplate.h"
#include "canvband.h"
#include "property.h"
#include "plugin.h"

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
    QPen pen(QColor(0,0,0), 0, Qt::DotLine);
    painter.setPen(pen);
    painter.setBrush(QBrush(NoBrush));
    painter.drawRect(rect());
//  QCanvasRectangle::draw(painter);
}



ReportCanvas::ReportCanvas(QCanvas * canvas, QWidget * parent, const char * name, WFlags f):
	QCanvasView(canvas, parent, name, f),m_plugin(0)
{
    m_canvas=(MyCanvas*)canvas;
    itemToInsert = 0;
    moving = 0;
    resizing = 0;
    selectionStarted = 0;
    request = RequestNone;
    selectionRect = new SelectionRect(0, 0, 0, 0, canvas);
}

void ReportCanvas::setPlugin(KuDesignerPlugin *plugin)
{
	m_plugin=plugin;
}

void ReportCanvas::deleteItem(QCanvasItemList &l)
{
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
    {
        unselectItem((CanvasBox*)*it);
        if ( ((MyCanvas*)(canvas()))->templ->removeReportItem(*it) )
            break;
    }
}

void ReportCanvas::editItem(QCanvasItemList &/* l */)
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
	    if ((*it)->rtti() == KuDesignerRttiKugarTemplate)
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
        if ((*it)->rtti() >= 1800 ) //include bands and the template itself
        {
            CanvasBox *b = (CanvasBox*)(*it);
            if (!m_canvas->selected.containsRef(b))
            {
                unselectAll();
                selectItem(b, false);
                canvas()->update();
//                qWarning("selected item set");
//                selected->drawHolders();
                return;
            }
            if (m_canvas->selected.containsRef(b))
            {
                if (m_canvas->selected.count() > 1)
                {
                    unselectAll();
                    selectItem(b, false);
                    canvas()->update();
                }    
                return;
            }
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


bool ReportCanvas::startResizing(QMouseEvent * /*e*/, QPoint &p)
{
	if (m_canvas->selected.count()==0) return false;
	for (CanvasBox *cbx=m_canvas->selected.first();
		cbx;cbx=m_canvas->selected.next())
	{
		resizing_type=cbx->isInHolder(p);
		if (resizing_type)
		{
			selectItem(cbx,false);
			//kdDebug()<<"A Widget should be resized"<<endl;
        	        moving = 0;
                	resizing = cbx;
			moving_start = p;
			moving_offsetX=0;
			moving_offsetY=0;

			if (cbx->rtti()>2001)
			{
        	                CanvasReportItem *item = (CanvasReportItem *)(cbx);
				resizing_constraint.setX((int)item->section()->x());
				resizing_constraint.setY((int)item->section()->y());
				resizing_constraint.setWidth(item->section()->width());
				resizing_constraint.setHeight(
					item->section()->height());
				if (cbx->rtti()!=KuDesignerRttiCanvasLine)
				{
					resizing_minSize.setWidth(10);
					resizing_minSize.setHeight(10);
				}
				else
				{
					resizing_minSize.setWidth(0);
					resizing_minSize.setHeight(0);
				}
			}
			else
				if (cbx->rtti()>=KuDesignerRttiReportHeader)
				{
					resizing_constraint=QRect(0,0,1000,1000);
					resizing_minSize.setWidth(0);
					resizing_minSize.setHeight(
						((CanvasBand*)cbx)->minHeight());
				}
				else
				{
					resizing_constraint=QRect(0,0,1000,1000);
					resizing_minSize.setWidth(0);
					resizing_minSize.setHeight(10);
				}
                	return true;
		}
	}
	return false;
}

void ReportCanvas::startMoveOrResizeOrSelectItem(QCanvasItemList &l,
    QMouseEvent * /*e*/, QPoint &p)
{
    //allow user to move any item except for page rectangle
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
    {
        CanvasBox *cb=(CanvasBox*)(*it);
        if (cb->rtti() >= 1700) //> 2001)
        {
            moving_start = p;
            moving_offsetX=0;
	    moving_offsetY=0;
            resizing_type=cb->isInHolder(p);
	    if ((*it)->rtti() >2001)
	    {
                        CanvasReportItem *item = (CanvasReportItem *)(*it);
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



/*    CanvasBox *b;
    qWarning("Selected items:");
    for (b = selected.first(); b; b = selected.next())
        qWarning("%s", b->props["Text"].first.latin1());

  */
    switch (e->button())
    {
        case LeftButton:
            if (itemToInsert)
            {
//                qWarning("placing item");
                unselectAll();
                placeItem(l, e);
            }
            else
            {
//                qWarning("starting move or resize");
		if (!startResizing(e,p))
		{
	                selectItemFromList(l);
	                startMoveOrResizeOrSelectItem(l, e, p);
		}
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
        case LeftButton:
            if (selectionStarted)
                finishSelection();
            break;
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


void ReportCanvas::fixMinValues(double &pos,double minv,double &offset)
{
	if (pos<minv)
	{
		offset=offset+pos-minv;
		pos=minv;
	}
	else
	{
		if (offset<0)
		{
			offset=offset+pos-minv;
			if (offset<0)
				pos=minv;
			else
			{
				pos=offset+minv;
				offset=0;
			}
		}
	}
}

void ReportCanvas::fixMaxValues(double &pos,double size,double maxv,double &offset)
{
	double tmpMax=pos+size;
	if (tmpMax>maxv)
	{
		offset=offset+tmpMax-maxv;
		pos=maxv-size;
	}
	else
	{
		if (offset>0)
		{
			offset=offset+tmpMax-maxv;
			if (offset>0)
				pos=maxv-size;
			else
			{
				pos=offset+maxv-size;
				offset=0;
			}
		}
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
	double newXPos=moving->x()+p.x()-moving_start.x();
	double newYPos=moving->y()+p.y()-moving_start.y();
	fixMinValues(newYPos,moving->parentSection->y(),moving_offsetY);
	fixMinValues(newXPos,moving->parentSection->x(),moving_offsetX);
	fixMaxValues(newYPos,moving->height(),moving->parentSection->y()+moving->parentSection->height(),moving_offsetY);
	fixMaxValues(newXPos,moving->width(),moving->parentSection->x()+moving->parentSection->width(),moving_offsetX);

	moving->move(newXPos,newYPos);
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
	double newXPos=r->x();
	double newYPos=r->y();
	double h=r->height();
	double w=r->width();
//	kdDebug()<<"resizing"<<endl;

//vertical resizing
	if (resizing_type & CanvasBox::ResizeBottom)
	{
//		kdDebug()<<"Resize bottom"<<endl;
		h = h + p.y() - moving_start.y();
		fixMaxValues(h,r->y(),resizing_constraint.bottom(),moving_offsetY);
		if(resizing->rtti() != KuDesignerRttiCanvasLine)
			fixMinValues(h,resizing_minSize.height(),moving_offsetY);
	}
	else
		if (resizing_type & CanvasBox::ResizeTop)
		{
//			kdDebug()<<"Resize top"<<endl;
			newYPos=r->y()+p.y()-moving_start.y();
			fixMinValues(newYPos,resizing_constraint.top(),moving_offsetY);
			if(resizing->rtti() != KuDesignerRttiCanvasLine)
				fixMaxValues(newYPos, resizing_minSize.height(),r->y()+r->height(),moving_offsetY);
			h=h+(r->y()-newYPos);
		}


//horizontal resizing
	if (resizing_type & CanvasBox::ResizeRight)
	{
//		kdDebug()<<"Resize right"<<endl;
        	w = w + p.x() - moving_start.x();
		fixMaxValues(w,r->x(),resizing_constraint.right(),moving_offsetX);
		if(resizing->rtti() != KuDesignerRttiCanvasLine)
			fixMinValues(w,resizing_minSize.width(),moving_offsetX);

	}
	else
		if (resizing_type & CanvasBox::ResizeLeft)
		{
//			kdDebug()<<"Resize left"<<endl;
			newXPos=r->x()+p.x()-moving_start.x();
			fixMinValues(newXPos,resizing_constraint.left(),moving_offsetX);
			if(resizing->rtti() != KuDesignerRttiCanvasLine)
				fixMaxValues(newXPos, resizing_minSize.width(),r->x()+r->width(),moving_offsetX);
			w=w+(r->x()-newXPos);
		}

	r->move(newXPos,newYPos);
	r->setSize((int)w, (int)h);
        moving_start = p;
        resizing->updateGeomProps();
        canvas()->update();
        emit modificationPerformed();
    }
    if (selectionStarted)
    {
/*        qDebug("x_start = %d, y_start = %d, x_end = %d, y_end = %d", moving_start.x(),
            moving_start.y(), e->pos().x(), e->pos().y());*/
        selectionRect->setSize((int) (e->pos().x() - selectionRect->x()),
            (int) (e->pos().y() - selectionRect->y() ));
        unselectAll();
        QCanvasItemList l = canvas()->collisions(selectionRect->rect());
        for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
        {
            QRect r;
            int left = selectionRect->rect().left();
            int right = selectionRect->rect().right();
            int top = selectionRect->rect().top();
            int bottom = selectionRect->rect().bottom();
            r.setLeft(left<right ? left : right);
            r.setRight(left<right ? right : left);
            r.setTop(top<bottom ? top : bottom);
            r.setBottom(top<bottom ? bottom : top);

            if ( ((*it)->rtti() > 2001) &&
                (r.contains(((CanvasBox*)(*it))->rect())) )
            {
                selectItem((CanvasBox*)(*it));
                canvas()->update();
            }
        }


/*        selectionRect->setSize(e->pos().x() - selectionRect->x(),
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
        }*/
    }
}


void ReportCanvas::contentsMouseDoubleClickEvent( QMouseEvent *e )
{
    CanvasReportItem *item = 0L;
    if ( e->button() == Qt::LeftButton && m_canvas->selected.count() == 1 )
        item = dynamic_cast<CanvasReportItem *>( m_canvas->selected.first() );
    if ( item )
    {
        item->fastProperty();
        item->hide();
        item->show();
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

    for (b = m_canvas->selected.first(); b; b = m_canvas->selected.next())
    {
        b->setSelected(false);
        canvas()->setChanged(b->rect());
    }

    m_canvas->selected.clear();
    canvas()->update();
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
    m_canvas->selected.append(it);
    it->setSelected(true);

    if (!selectionStarted)
        finishSelection();
}

void ReportCanvas::unselectItem(CanvasBox *it)
{
    m_canvas->selected.remove(it);
    it->setSelected(false);
}

void ReportCanvas::updateProperty(QString name, QString value)
{
    CanvasBox *b;
    for (b = m_canvas->selected.first(); b; b = m_canvas->selected.next())
    {
        b->props[name]->setValue(value);
        b->hide();
        b->show();
        if ((b->rtti()>=1800) && (b->rtti()<2000))
           ((MyCanvas*)(canvas()))->templ->arrangeSections();
    }
}


void ReportCanvas::finishSelection()
{
    selectionStarted = false;

    if (!m_canvas->selected.isEmpty())
        if (m_canvas->selected.count() > 1)
        {
            // handling multiple selection
            std::map<QString, PropPtr > curr = m_canvas->selected.first()->props;

            CanvasBox *b = m_canvas->selected.first();
            b = m_canvas->selected.next();
            while (b)
            {
                std::map<QString, PropPtr > selProps;
                std::insert_iterator< std::map<QString, PropPtr > > it(selProps, selProps.begin());
                std::set_intersection(curr.begin(), curr.end(), b->props.begin(), b->props.end(), it);

                curr = selProps;

/*                qWarning("Multiple selection property list:");
                for (std::map<QString, PropPtr>::const_iterator i = curr->begin(); i != curr->end(); ++i )
                {
                    qWarning("   %s = %s", i->first.latin1(), i->second->value().latin1());
                }*/

                b = m_canvas->selected.next();
            }

/*            qWarning("Multiple selection property list:");
            for (std::map<QString, PropPtr>::const_iterator i = curr->begin(); i != curr->end(); ++i )
            {
                qWarning("   %s = %s", i->first.latin1(), i->second->value().latin1());
            }*/

            std::map<QString, PropPtr > *x = new std::map<QString, PropPtr >(curr);
            emit selectionMade(x,0);
        }
        else
            emit selectionMade(&(m_canvas->selected.first()->props),
	    	m_canvas->selected.first());

/*  CanvasBox *b;
    for (b = selected.first(); b; b = selected.next())
    {
        qWarning("item with rtti = %d is selected", b->rtti());
    }*/

}

void ReportCanvas::contentsDragMoveEvent ( QDragMoveEvent * event) {
//perhaps this could be optimized a littlebit
	if (!m_plugin) return;
	QCanvasItemList l=canvas()->collisions(event->pos());
	kdDebug()<<l.count()<<endl;
	if (l.count()<2) 
	{
		event->ignore();
		return;
	}
	CanvasBox *b=(CanvasBox*) (*(l.begin()));
	if (m_plugin->dragMove(event,b))
	        event->accept();
	else
		event->ignore();
}

void ReportCanvas::contentsDragEnterEvent ( QDragEnterEvent * /*event*/) {
//	event->accept();
}



void ReportCanvas::keyPressEvent( QKeyEvent *e )
{
   qDebug("keyPress (selection : %d)", m_canvas->selected.count());
    
    if ( m_canvas->selected.count() == 1 ) {
        CanvasReportItem *item = static_cast<CanvasReportItem *>( m_canvas->selected.first() );
        
        switch ( e->key() ) {
            case Qt::Key_Delete:
                qDebug("Deleting selection");
                unselectItem(item);
                ( (MyCanvas*) canvas() )->templ->removeReportItem( item );
                clearRequest();
                return;

            /* Adjust height with - and +  */
            case Qt::Key_Minus:
            case Qt::Key_Plus:
                {
                    int size = item->props["FontSize"]->value().toInt();

                    if ( e->key() == Qt::Key_Minus )
                        size--;
                    else 
                        size++;

                    if ( size < 5 )
                        size = 5;
                    
                    if ( size > 50 )
                        size = 50;

                    updateProperty( "FontSize", QString::number( size ) );
                    return;
                }

            default:
                e->ignore();
        }

    }
}

#include "cv.moc"
