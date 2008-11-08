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

#ifndef REPORTRECTENTITY_H
#define REPORTRECTENTITY_H

#include <QGraphicsRectItem>
#include "reportentities.h"
#include <koproperty/Set.h>
#include <KoUnit.h>
#include <krobjectdata.h>

class ReportDesigner;
class KRPos;
class KRSize;

/**
 @author
*/
class ReportRectEntity : public QGraphicsRectItem, public ReportEntity
{
public:
    ReportRectEntity(ReportDesigner*);

    virtual ~ReportRectEntity();
    void setUnit(KoUnit u);
protected:
    void init(KRPos*, KRSize*, KoProperty::Set*);
    int dpiX;
    int dpiY;

    void setSceneRect(QPointF, QSizeF);
    void setSceneRect(QRectF);

    void drawHandles(QPainter*);
    QRectF pointRect();
    QRectF sceneRect();
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    int grabHandle(QPointF);
    int _grabAction;

    KRPos* ppos;
    KRSize* psize;
    KoProperty::Set* pset;

};

#endif
