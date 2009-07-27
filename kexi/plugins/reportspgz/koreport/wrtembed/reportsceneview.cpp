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
// ReportSceneView method implementations
//
#include "reportsceneview.h"
#include "reportdesigner.h"

#include <kdebug.h>

ReportSceneView::ReportSceneView(ReportDesigner * rw, QGraphicsScene *scene, QWidget * parent, const char * name)
{
    m_reportDesigner = rw;

    viewport()->setMouseTracking(true);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    setCacheMode(QGraphicsView::CacheBackground);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setDragMode(QGraphicsView::RubberBandDrag);
    setScene(scene);
    setFrameStyle(0);
    m_reportDesigner->setActiveScene(scene);

}

ReportSceneView::~ReportSceneView()
{
    viewport()->setMouseTracking(false);
}

//TODO check
void ReportSceneView::resizeContents(QSize s)
{
    setMinimumSize(s);
    setMaximumSize(s);
}

ReportDesigner * ReportSceneView::designer()
{
    return m_reportDesigner;
}

void ReportSceneView::mouseReleaseEvent(QMouseEvent * e)
{
    m_reportDesigner->sectionMouseReleaseEvent(this, e);
    QGraphicsView::mouseReleaseEvent(e);
}

QSize ReportSceneView::sizeHint() const
{
    //kDebug() <<  scene()->width() << "x" << scene()->height();
    return QSize(scene()->width(), scene()->height());
}

