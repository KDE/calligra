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
#include <QDateTime>
#include <QVector>
#include "kptproject.h"
#include "kptview.h"


namespace KPlato
{


    class Chart
    {
        private:
            float sizeSave[3][2];
            float totalYPercent;
            float totalCostPlanned;
            float totalTimePlanned;
            int totalWeek;
	    Project currentProject;
        public:

            Chart();
            ~Chart();
            void api(QVector<QPointF>& , QVector<QPointF>& , QVector<QPointF>& ,const int , int , int );
            void reCalculateY(QVector<QPointF> &,const int,int);
            void reCalculateX(QVector<QPointF> &,const int,int);
            void setMaxCost(QVector<QPointF>);
           // void setMaxTime(QVector<QPointF> BCWP);
            float setMaxYPercent(QVector<QPointF>, QVector<QPointF>, QVector<QPointF>);
            void costToPercent(QVector<QPointF>&);
            void timeToPercent(QVector<QPointF>&);
            //think to add a parameter to the function (date)
            //float calculateActualCost(Project &, QDate);
            //think to add a parameter to the function (date)
            //float calculatePlannedCost(Project &, QDate);
            //For latter but not to forget to do it :D
            void calculateValueOfBCWS(Project &, QVector<QPoint>*);
            /*void calculateValueOfBCWP(Project &, QVector<QPoint>*);
            void calculateValueOfACWP(Project &, QVector<QPoint>*);*/    
    };

} //namespace KPlato

#endif
