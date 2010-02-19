/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
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
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
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
    int m_dpiX;
    int m_dpiY;

    void setSceneRect(QPointF, QSizeF, bool update_property = true);
    void setSceneRect(QRectF, bool update_property = true);

    void drawHandles(QPainter*);
    QRectF pointRect();
    QRectF sceneRect();
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void propertyChanged(KoProperty::Set &s, KoProperty::Property &p);
private:
    int grabHandle(QPointF);
    int m_grabAction;

    KRPos* m_ppos;
    KRSize* m_psize;
    KoProperty::Set* m_pset;

};

#endif
