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
{
    kDebug() << "------------> ChartWidget :: Constructor"<<endl;

    
    is_bcwp_draw=false;
    is_bcws_draw=true;
    is_acwp_draw=false;
    clear();

    // UI Parameters
    setBackgroundColor(Qt::white);
    setForegroundColor(Qt::black);
    setGridColor(Qt::black);
    setAntialiasing(false);

    kDebug() << "ChartWidget :: Constructor Ended"<<endl;
}

void PlotWidget::clear()
{
    weeks.clear();
    bcwpPoints.clear();
    bcwsPoints.clear();
    acwpPoints.clear();
}

void PlotWidget::draw( Project &p, ScheduleManager &sm ) 
{
    //KPlotWidget *kpw = new KPlotWidget( this );
    // setting our limits for the plot
    setLimits( 1.0, 5.0, 1.0, 25.0 );
    
    // creating a plot object whose points are connected by red lines ...
    KPlotObject *kpo = new KPlotObject( Qt::red, KPlotObject::Lines );
    // ... adding some points to it ...
    for ( float x = 1.0; x <= 5.0; x += 0.1 )
        kpo->addPoint( x, x*x );
    
    // ... and adding the object to the plot widget
    addPlotObject( kpo );

    

 /*   clear();
    chartEngine.calculateWeeks(weeks,p,sm);
    chartEngine.initXCurvesVectors(weeks,bcwpPoints,bcwsPoints,acwpPoints);

    chartEngine.calculateBCWP(bcwpPoints,weeks,p,sm);
    chartEngine.calculateActualCost(acwpPoints,weeks,p,sm);
    chartEngine.calculatePlannedCost(bcwsPoints,weeks,p,sm);
    
    chartEngine.setMaxCost(bcwsPoints);

    chartEngine.costToPercent(bcwpPoints);
    chartEngine.costToPercent(acwpPoints);
    chartEngine.costToPercent(bcwsPoints);

    maxYPercent=chartEngine.setMaxYPercent(bcwpPoints,bcwsPoints,acwpPoints);

    chartEngine.timeToPercent(bcwpPoints);
    chartEngine.timeToPercent(acwpPoints);
    chartEngine.timeToPercent(bcwsPoints);

    bcwpPoints_display.clear();
    bcwsPoints_display.clear();
    acwpPoints_display.clear();
    chartEngine.init_display(bcwpPoints_display,bcwsPoints_display,acwpPoints_display,weeks.size());*/
}

void PlotWidget::drawBCWP(){
    is_bcwp_draw=true;
    this->update();
}

void PlotWidget::undrawBCWP(){
    is_bcwp_draw=false;
    this->update();
}
void PlotWidget::drawBCWS(){
    is_bcws_draw=true;
    this->update();
}

void PlotWidget::undrawBCWS(){
    is_bcws_draw=false;
    this->update();
}
void PlotWidget::drawACWP(){
    is_acwp_draw=true;
    this->update();
}

void PlotWidget::undrawACWP(){
    is_acwp_draw=false;
    this->update();
}

/* API CURVES TO GET POINTS*/

/* GETTERS AND SETTERS */ 

void PlotWidget::setPointsBCPW(QVector<QPointF> vec)
{
    
}
void PlotWidget::setPointsBCPS(QVector<QPointF> vec)
{
   
}
void PlotWidget::setPointsACPW(QVector<QPointF> vec)
{
    
}

}