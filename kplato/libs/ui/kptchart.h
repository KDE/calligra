/* This file is part of the KDE project
   Copyright (C) 2007 Nicolas MICAS <nicolas.micas@gmail.com> and Laure Colbrant <laure.colbrant@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef KPTCHART_H
#define KPTCHART_H

//#include "kptchartpanel.h"

#include <klocale.h>

#include <kdebug.h>
#include <QVector>
#include <QDate>
#include "kptproject.h"
#include "kpteffortcostmap.h"

namespace KPlato
{
class ScheduleManager;


class Chart
{

private:

    float mTotalTime;
    float mTotalCostPlanned;

    void init_display(QVector<QPointF>& , QVector<QPointF>& , QVector<QPointF>&, int);
    
    void reCalculateY(QVector<QPointF> &data, QVector<QPointF> &display, int height);
    void reCalculateX(QVector<QPointF> &data,QVector<QPointF> &display, int width);

    void setMaxCost(float);
    
    void setMaxTime();

    void costToPercent(QVector<QPointF>&);
    void timeToPercent(QVector<QPointF>&);

    void calculatePlannedCost(const Project &,const  ScheduleManager &);
    void calculateActualCost(const Project &,const ScheduleManager &);
    void calculateBCWP(const Project &,const ScheduleManager &);

    QVector<QDate> calculateWeeks(const Project &,const ScheduleManager & );

public:

    /*
    *   
    *   
    */
    void api(QVector<QPointF>& data, QVector<QPointF>& display, const QSize &size );
    void calculateData(const Project & p, const ScheduleManager & sm);
    float getMaxCost();
    float getMaxTime();

    QVector<QPointF>    mBCWPPoints;
    QVector<QPointF>    mBCWSPoints;
    QVector<QPointF>    mACWPPoints;

    Chart();

protected:
    EffortCostMap acwpPrDay;
    EffortCostMap bcwpPrDay; // includes both bcws and bcwp

};

} //namespace KPlato

#endif
