/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */

#include "reportrectentity.h"
#include "reportsceneview.h"
#include <koproperty/set.h>
#include <koproperty/property.h>
#include <koproperty/editor.h>
#include <QGraphicsSceneMouseEvent>
#include "reportdesigner.h"
#include <KoGlobal.h>
#include <kdebug.h>
#include <krpos.h>
#include <krsize.h>
#include "reportscene.h"
ReportRectEntity::ReportRectEntity ( ReportDesigner *r)
		: QGraphicsRectItem(), ReportEntity(r)
{
	_rd = r;
	dpiX = KoGlobal::dpiX();
	dpiY = KoGlobal::dpiY();
	
	ppos = 0;
	psize = 0;
	
	setAcceptsHoverEvents(true);
}

void ReportRectEntity::init(KRPos* p, KRSize* s, KoProperty::Set* se)
{
	ppos = p;
	psize = s;
	pset = se;
}

ReportRectEntity::~ReportRectEntity()
{
}

void ReportRectEntity::setUnit(KoUnit u)
{
	ppos->setUnit(u);
	psize->setUnit(u);
}

QRectF ReportRectEntity::sceneRect()
{
	return QRectF ( ppos->toScene(), psize->toScene() );
}

QRectF ReportRectEntity::pointRect()
{
	if (ppos && psize)
		return QRectF ( ppos->toPoint(), psize->toPoint() );
	else
		return QRectF(0,0,0,0);
}

void ReportRectEntity::setSceneRect ( QPointF p, QSizeF s )
{
	setSceneRect(QRectF(p,s));
}

void ReportRectEntity::setSceneRect ( QRectF r )
{
	QGraphicsRectItem::setPos ( r.x(), r.y() );
	setRect ( 0,0,r.width(), r.height() );
	ppos->setScenePos ( QPointF(r.x(), r.y()) );
	psize->setSceneSize( QSizeF(r.width(), r.height()));
	update();
}

void ReportRectEntity::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
	//Update and show properties
	ppos->setScenePos ( QPointF(sceneRect().x(), sceneRect().y()) );
	//_rd->changeSet ( pset );

	//Set selection
	if ( ! ( ( event->modifiers() & Qt::ShiftModifier ) == Qt::ShiftModifier ) )
	{
		scene()->clearSelection();
	}

	_rd->setActiveScene ( scene() );
	setSelected ( !isSelected() );
	scene()->update();
}

void ReportRectEntity::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
	//Keep the size and position in sync
	ppos->setScenePos ( pos() );
	psize->setSceneSize(QSizeF(rect().width(), rect().height()));
	_rd->changeSet ( pset );
	QGraphicsItem::mouseReleaseEvent ( event );
}

void ReportRectEntity::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
	int w;
	int h;
	
	QPointF p  = dynamic_cast<ReportScene*>(scene())->gridPoint(event->scenePos());
	w = p.x() - scenePos().x();
	h = p.y() - scenePos().y();
	
	//TODO All the other resize directions
	//TODO use an enum for the directions

	switch(_grabAction)
	{
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			if (w > 0)
			setRect (0,0, w, (int)(rect().height()));
			break;
		case 5:
			if ( h> 0 && w > 0)
			setRect ( 0,0, w,h );
			break;
		case 6:
			if (h > 0)
			setRect (0,0,(int)(rect().width()), h);
			break;
		case 7:
			break;
		case 8:
			break;	
		default:
			QGraphicsItem::mouseMoveEvent(event);
	}
}

void ReportRectEntity::hoverMoveEvent ( QGraphicsSceneHoverEvent * event )
{
	_grabAction = 0;
	
	if (isSelected())
	{
	_grabAction = grabHandle(event->pos());
	switch(_grabAction)
	{
		case 1:
			setCursor ( Qt::SizeFDiagCursor );
			break;
		case 2:
			setCursor ( Qt::SizeVerCursor );
			break;
		case 3:
			setCursor ( Qt::SizeBDiagCursor );
			break;
		case 4:
			setCursor ( Qt::SizeHorCursor );
			break;
		case 5:
			setCursor ( Qt::SizeFDiagCursor );
			break;
		case 6:
			setCursor ( Qt::SizeVerCursor );
			break;
		case 7:
			setCursor ( Qt::SizeBDiagCursor );
			break;
		case 8:
			setCursor ( Qt::SizeHorCursor );
			break;	
		default:
			unsetCursor();
	}
	}
}

void ReportRectEntity::drawHandles ( QPainter *painter )
{
	if ( isSelected() )
	{
		// draw a selected border for visual purposes
		painter->setPen ( QPen ( QColor ( 128,128,255 ), 0, Qt::DotLine ) );

		painter->drawRect ( rect() );

		const QRectF r = rect();
		int halfW = ( int ) ( r.width() / 2 );
		int halfH = ( int ) ( r.height() / 2 );
		QPointF center = r.center();

		painter->fillRect ( center.x()- halfW, center.y()- halfH ,5, 5, QColor ( 128, 128, 255 ) );
		painter->fillRect ( center.x() - 2, center.y()- halfH ,5, 5, QColor ( 128, 128, 255 ) );
		painter->fillRect ( center.x() + halfW - 4, center.y()- halfH,5, 5, QColor ( 128, 128, 255 ) );

		painter->fillRect ( center.x() + ( halfW - 4 ), center.y() - 2,5, 5, QColor ( 128, 128, 255 ) );

		painter->fillRect ( center.x() +  halfW - 4 , center.y() + halfH - 4 , 5, 5, QColor ( 128, 128, 255 ) );
		painter->fillRect ( center.x()-2, center.y() + halfH-4, 5, 5, QColor ( 128, 128, 255 ) );
		painter->fillRect ( center.x()- halfW, center.y() + halfH-4 ,5, 5, QColor ( 128, 128, 255 ) );

		painter->fillRect ( center.x()- halfW, center.y()-2,5, 5, QColor ( 128, 128, 255 ) );

	}
}

/**
	@return	1 2 3
		8 0 4
		7 6 5
*/
int ReportRectEntity::grabHandle ( QPointF pos )
{	
	QRectF r = boundingRect();
	int halfW = ( int ) ( r.width() / 2 );
	int halfH = ( int ) ( r.height() / 2 );
	QPointF center = r.center();
	
	if ( QRectF ( center.x()- ( halfW ),center.y()- ( halfH ),5,5 ).contains ( pos ) )
	{
		// we are over the top-left handle
		return 1;
	}
	else if ( QRectF ( center.x()-2,center.y()- ( halfH ),5,5 ).contains ( pos ) )
	{
		// top-middle handle
		return 2;
	}
	else if ( QRectF ( center.x() + ( halfW-4 ),center.y()- ( halfH ),5,5 ).contains ( pos ) )
	{
		// top-right
		return 3;
	}
	else if ( QRectF ( center.x() + ( halfW-4 ),center.y()-2,5,5 ).contains ( pos ) )
	{
		// middle-right
		return 4;
	}
	else if ( QRectF ( center.x() + ( halfW-4 ),center.y() + ( halfH-4 ),5,5 ).contains ( pos ) )
	{
		// bottom-left
		return 5;
	}
	else if ( QRectF ( center.x()-2,center.y() + ( halfH-4 ),5,5 ).contains ( pos ) )
	{
		// bottom-middle
		return 6;
	}
	else if ( QRectF ( center.x()- ( halfW ),center.y() + ( halfH-4 ),5,5 ).contains ( pos ) )
	{
		// bottom-right
		return 7;
	}
	else if ( QRectF ( center.x()- ( halfW ),center.y()-2,5,5 ).contains ( pos ) )
	{
		// middle-right
		return 8;
	}
	return 0;
}

QVariant ReportRectEntity::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == ItemPositionChange && scene()) 
	{
		QPointF newPos = value.toPointF();
			
		newPos = dynamic_cast<ReportScene*>(scene())->gridPoint(newPos);
		if (newPos.x() < 0)
			newPos.setX(0);
		else if (newPos.x() > (scene()->width() - rect().width()))
			newPos.setX(scene()->width() - rect().width());
		
		if (newPos.y() < 0)
			newPos.setY(0);
		else if (newPos.y() > (scene()->height() - rect().height()))
			newPos.setY(scene()->height() - rect().height());
		
		
		return newPos;
	}
	else if (change == ItemPositionHasChanged && scene()) 
	{
		ppos->setScenePos(value.toPointF());
	}
	
	return QGraphicsItem::itemChange(change, value);
}