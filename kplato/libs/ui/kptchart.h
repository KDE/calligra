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

namespace KPlato
{
class ScheduleManager;

class Chart
{

private:
    float totalYPercent;
    float totalCostPlanned;
    float totalTimePlanned;
    int totalWeek;
public:

    Chart();
    void init_display(QVector<QPointF>& , QVector<QPointF>& , QVector<QPointF>&, int);
    void api(QVector<QPointF>& data, QVector<QPointF>& display, const QSize &size );
    void reCalculateY(QVector<QPointF> &data, QVector<QPointF> &display, int height);
    void reCalculateX(QVector<QPointF> &data,QVector<QPointF> &display, int width);
    void setMaxCost(QVector<QPointF>);
    //void setMaxTime(QVector<QDate>);
    float setMaxYPercent(QVector<QPointF>, QVector<QPointF>, QVector<QPointF>);

    void costToPercent(QVector<QPointF>&);
    void timeToPercent(QVector<QPointF>&);

    void calculatePlannedCost(QVector<QPointF> &,QVector<QDate> ,Project &,  ScheduleManager &);
    void calculateActualCost(QVector<QPointF> &,QVector<QDate> ,Project &, ScheduleManager &);
    void calculateBCWP(QVector<QPointF> &,QVector<QDate> ,Project &, ScheduleManager &);

    void initXCurvesVectors(QVector<QDate> ,QVector<QPointF> & , QVector<QPointF> & , QVector<QPointF> & );
    void calculateWeeks(QVector<QDate> & ,Project &, ScheduleManager & );

};

} //namespace KPlato

#endif
