/* This file is part of the KDE project
   Copyright (C) 2005 Frédéric Lambert <konkistadorr.kde@gmail.com>
   Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#include "kptplotwidget.h"
#include <kplotwidget.h>
#include <kplotobject.h>

#include <klocale.h>
#include <kdebug.h>
#include <stdio.h>

namespace KPlato
{

PlotWidget::PlotWidget(QWidget * parent) : KPlotWidget(parent)
, mKpoBCWP(NULL)
, mKpoBCWS(NULL)
, mKpoACWP(NULL)
{
    kDebug() << "------------> ChartWidget :: Constructor"<<endl;

    // UI Parameters
    setBackgroundColor(Qt::white);
    setForegroundColor(Qt::black);
    setGridColor(Qt::black);
    setAntialiasing(false);

    // creating a plot object whose points are connected by red lines 
    mKpoBCWP = new KPlotObject( Qt::black, KPlotObject::Lines );
    mKpoBCWS = new KPlotObject( Qt::green, KPlotObject::Lines );
    mKpoACWP = new KPlotObject( Qt::red, KPlotObject::Lines );

    // ... and adding the object to the plot widget
    addPlotObject(mKpoBCWP);
    addPlotObject(mKpoBCWS);
    addPlotObject(mKpoACWP);

    kDebug() << "ChartWidget :: Constructor Ended"<<endl;
}

PlotWidget::~PlotWidget()
{
/*    delete mKpoBCWP;
    delete mKpoBCWS;
    delete mKpoACWP;*/
}

void PlotWidget::draw( Project &p, ScheduleManager &sm ) 
{
   
    // Retrieve datas
    mKpoBCWP->clearPoints();
    mKpoBCWS->clearPoints();
    mKpoACWP->clearPoints();
    
    chartEngine.calculateData(p,sm);
    
    // setting our limits for the plot
    setLimits( 0.0, chartEngine.getMaxTime(), 0.0, chartEngine.getMaxCost() );
    
    for (QVector<QPointF>::iterator iterData = chartEngine.mBCWPPoints.begin() ; iterData != chartEngine.mBCWPPoints.end(); ++iterData )
        mKpoBCWP->addPoint( iterData->x(), iterData->y() );

    for (QVector<QPointF>::iterator iterData = chartEngine.mBCWSPoints.begin() ; iterData != chartEngine.mBCWSPoints.end(); ++iterData )
        mKpoBCWS->addPoint( iterData->x(), iterData->y() );

    for (QVector<QPointF>::iterator iterData = chartEngine.mACWPPoints.begin() ; iterData != chartEngine.mACWPPoints.end(); ++iterData )
        mKpoACWP->addPoint( iterData->x(), iterData->y() );
}

void PlotWidget::drawBCWP(){
    mKpoBCWP->setShowLines(true);
}
void PlotWidget::undrawBCWP(){
    mKpoBCWP->setShowLines(false);
}
void PlotWidget::drawBCWS(){
    mKpoBCWS->setShowLines(true);
}
void PlotWidget::undrawBCWS(){
    mKpoBCWS->setShowLines(false);
}
void PlotWidget::drawACWP(){
    mKpoACWP->setShowLines(true);
}
void PlotWidget::undrawACWP(){
   mKpoACWP->setShowLines(false);
}

}
