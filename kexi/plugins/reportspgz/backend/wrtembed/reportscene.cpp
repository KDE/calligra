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
// ReportScene method implementations
//

#include "reportscene.h"
#include <reportpageoptions.h>
#include <labelsizeinfo.h>
#include <qpainter.h>
#include "reportdesigner.h"

#include <KoGlobal.h>
#include <kdebug.h>

#include <KoPageFormat.h>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include "reportrectentity.h"
#include <QGraphicsSceneMouseEvent>

ReportScene::ReportScene(qreal w, qreal h, ReportDesigner *rd)
        : QGraphicsScene(0, 0, w, h)
{
    _rd = rd;
}
ReportScene::~ReportScene()
{
    // Qt should be handling everything for us
}

void ReportScene::drawBackground(QPainter* painter, const QRectF & clip)
{
    //Draw the default background colour
    QGraphicsScene::drawBackground(painter, clip);
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (_rd->propertySet()->property("ShowGrid").value().toBool()) {
        if (KoUnit::unitName(u) != KoUnit::unitName(_rd->pageUnit())) {
            u = _rd->pageUnit();
            if (KoUnit::unitName(u) == "cc" || KoUnit::unitName(u) == "pi" || KoUnit::unitName(u) == "mm")
                major = POINT_TO_INCH(u.fromUserValue(10)) * KoGlobal::dpiX();
            else if (KoUnit::unitName(u) == "pt")
                major = POINT_TO_INCH(u.fromUserValue(100)) * KoGlobal::dpiX();
            else
                major = POINT_TO_INCH(u.fromUserValue(1)) * KoGlobal::dpiX();

        }
        minor = _rd->propertySet()->property("GridDivisions").value().toInt();
        pixel_increment = (major / minor);

        QPen pen = painter->pen();
        painter->setPen(QColor(212, 212, 212));

        if (pixel_increment > 2) { // do not bother painting points if increments are so small
            int wpoints = qRound(sceneRect().width() / pixel_increment);
            int hpoints = qRound(sceneRect().height() / pixel_increment);
            for (int i = 0; i < wpoints; ++i) {
                for (int j = 0; j < hpoints; ++j) {
                    if (i % minor == 0 && j % minor == 0) {
                        painter->drawLine(QPointF(i * pixel_increment, j * pixel_increment), QPointF(i * pixel_increment, j * pixel_increment  + major));
                        painter->drawLine(QPointF(i * pixel_increment, j * pixel_increment), QPointF(i * pixel_increment + major, j * pixel_increment));
                    } else {
                        painter->drawPoint(QPointF(i * pixel_increment, j * pixel_increment));
                    }
                }
            }

        }

        pen.setWidth(1);
        //update ( clip );
    }
}

void ReportScene::mousePressEvent(QGraphicsSceneMouseEvent * e)
{
    _rd->setActiveScene(this);

    // clear the selection if Shift Key has not been pressed and it's a left mouse button   and
    // if the right mouse button has been pressed over an item which is not part of selected items
    if (((e->modifiers() & Qt::ShiftModifier) == 0 && e->button() == Qt::LeftButton)   ||
            (!(selectedItems().contains(itemAt(e->scenePos()))) && e->button() == Qt::RightButton)
       )
        clearSelection();

    //This will be caught by the section to display its properties, if an item is under the cursor then they will diplay their properties
    emit(clicked());

    QGraphicsScene::mousePressEvent(e);

}

void ReportScene::contextMenuEvent(QGraphicsSceneContextMenuEvent * e)
{
    _rd->sectionContextMenuEvent(this, e);
}

QPointF ReportScene::gridPoint(const QPointF& p)
{
    if (!_rd->propertySet()->property("GridSnap").value().toBool()) {
        return p;
    }

    if (KoUnit::unitName(u) != KoUnit::unitName(_rd->pageUnit())) {
        u = _rd->pageUnit();
        if (KoUnit::unitName(u) == "cc" || KoUnit::unitName(u) == "pi" || KoUnit::unitName(u) == "mm")
            major = POINT_TO_INCH(u.fromUserValue(10)) * KoGlobal::dpiX();
        else if (KoUnit::unitName(u) == "pt")
            major = POINT_TO_INCH(u.fromUserValue(100)) * KoGlobal::dpiX();
        else
            major = POINT_TO_INCH(u.fromUserValue(1)) * KoGlobal::dpiX();


    }
    minor = _rd->propertySet()->property("GridDivisions").value().toInt();
    pixel_increment = (major / minor);

    return QPointF(qRound((p.x() / pixel_increment)) * pixel_increment, qRound((p.y() / pixel_increment)) * pixel_increment);
}

void ReportScene::focusOutEvent(QFocusEvent * focusEvent)
{
    emit(lostFocus());
    QGraphicsScene::focusOutEvent(focusEvent);
}

qreal ReportScene::lowestZValue()
{
    qreal z;
    qreal zz;
    z = 0;
    QGraphicsItemList list = items();
    for (QGraphicsItemList::iterator it = list.begin();it != list.end(); it++) {
        zz = (*it)->zValue();
        if (zz < z) {
            z = zz;
        }

    }
    return z;
}

qreal ReportScene::highestZValue()
{
    qreal z;
    qreal zz;
    z = 0;
    QGraphicsItemList list = items();
    for (QGraphicsItemList::iterator it = list.begin();it != list.end(); it++) {
        zz = (*it)->zValue();
        if (zz > z) {
            z = zz;
        }

    }
    return z;
}

void ReportScene::lowerSelected()
{
    QGraphicsItemList list = selectedItems();
    for (QGraphicsItemList::iterator it = list.begin();
            it != list.end(); it++) {
        (*it)->setZValue(lowestZValue() - 1);
    }
}

void ReportScene::raiseSelected()
{
    QGraphicsItemList list = selectedItems();
    for (QGraphicsItemList::iterator it = list.begin();
            it != list.end(); it++) {
        (*it)->setZValue(highestZValue() + 1);
    }
}

QGraphicsItemList ReportScene::itemsOrdered()
{
    QGraphicsItemList r;
    QGraphicsItemList list = items();
    for (QGraphicsItemList::iterator it = list.begin();it != list.end(); it++) {
        for (QGraphicsItemList::iterator rit = r.begin();rit != r.end(); rit++) {


        }

    }


    return r;
}
