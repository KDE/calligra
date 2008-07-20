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

//
// Class ReportCanvas
//
//     Overrides the drawForeground() method to do the grid.
//

#ifndef __REPORTSCENE_H__
#define __REPORTSCENE_H__

#include <QGraphicsScene>
#include <KoUnit.h>
typedef QList<QGraphicsItem*> QGraphicsItemList;
class ReportDesigner;
class QGraphicsSceneContextMenuEvent;
class QGraphicsSceneMouseEvent;

class ReportScene : public QGraphicsScene
{
	Q_OBJECT
	public:
		ReportScene ( qreal w, qreal h, ReportDesigner* );
		virtual ~ReportScene();
		ReportDesigner* document(){return _rd;}
		QPointF gridPoint(const QPointF&);
		void raiseSelected();
		void lowerSelected();
		QGraphicsItemList itemsOrdered();
		qreal gridSize(){return pixel_increment;}
		
	protected:
		virtual void drawBackground ( QPainter* painter, const QRectF & clip );
		virtual void mousePressEvent ( QGraphicsSceneMouseEvent * e );
		virtual void focusOutEvent ( QFocusEvent * focusEvent );
		virtual void contextMenuEvent ( QGraphicsSceneContextMenuEvent * contextMenuEvent );

	signals:
		void clicked();
		void lostFocus();
		
	private:
		qreal lowestZValue();
		qreal highestZValue();
		
		ReportDesigner * _rd;
		
		KoUnit u;
		int minor;
		qreal major;
		qreal pixel_increment;
		
		
};

#endif
