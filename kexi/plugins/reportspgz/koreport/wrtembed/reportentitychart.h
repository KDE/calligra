/*
 * Kexi Report Plugin
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
#ifndef REPORTENTITYCHART_H
#define REPORTENTITYCHART_H

#include <QObject>
#include <krchartdata.h>
#include "reportrectentity.h"

/**
 @author Adam Pigg <adam@piggz.co.uk>
*/
class ReportEntityChart : public QObject, public ReportRectEntity, public KRChartData
{
    Q_OBJECT
public:
    ReportEntityChart(ReportDesigner *, QGraphicsScene* scene);
    ReportEntityChart(QDomNode & element, ReportDesigner *, QGraphicsScene* scene);

    virtual ~ReportEntityChart();
    virtual void buildXML(QDomDocument & doc, QDomElement & parent);
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual ReportEntityChart* clone();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);

private:
    void init(QGraphicsScene*, ReportDesigner *r);

private slots:
    void slotPropertyChanged(KoProperty::Set &, KoProperty::Property &);

};

#endif
