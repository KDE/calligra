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
#include "kptabstractchartmodel.h"
#include <klocale.h>
#include <kdebug.h>
#include <stdio.h>

#include <QPointF>

namespace KPlato
{

PlotWidget::PlotWidget(QWidget * parent) : KPlotWidget(parent)
, mKpoBCWP(NULL)
, mKpoBCWS(NULL)
, mKpoACWP(NULL)
{
    kDebug() << "------------> ChartWidget :: Constructor";

    // UI Parameters
    setBackgroundColor(Qt::white);
    setForegroundColor(Qt::black);
    setGridColor(Qt::black);
    setAntialiasing(false);

//     // creating a plot object whose points are connected by red lines 
//     mKpoBCWP = new KPlotObject( Qt::black, KPlotObject::Lines );
//     mKpoBCWS = new KPlotObject( Qt::green, KPlotObject::Lines );
//     mKpoACWP = new KPlotObject( Qt::red, KPlotObject::Lines );
// 
//     // ... and adding the object to the plot widget
//     addPlotObject(mKpoBCWP);
//     addPlotObject(mKpoBCWS);
//     addPlotObject(mKpoACWP);

    kDebug() << "ChartWidget :: Constructor Ended";
}

PlotWidget::~PlotWidget()
{
/*    delete mKpoBCWP;
    delete mKpoBCWS;
    delete mKpoACWP;*/
}

void PlotWidget::draw( Project &p, ScheduleManager &sm ) 
{
   return drawModel( p, sm );
   
    // Retrieve data
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
    //mKpoBCWP->setShowLines(true);
}
void PlotWidget::undrawBCWP(){
    //mKpoBCWP->setShowLines(false);
}
void PlotWidget::drawBCWS(){
    //mKpoBCWS->setShowLines(true);
}
void PlotWidget::undrawBCWS(){
    //mKpoBCWS->setShowLines(false);
}
void PlotWidget::drawACWP(){
    //mKpoACWP->setShowLines(true);
}
void PlotWidget::undrawACWP(){
   //mKpoACWP->setShowLines(false);
}

void PlotWidget::drawModel( Project &p, ScheduleManager &sm ) 
{
    kDebug();
    resetPlot();
    model.setProject( &p );
    model.setScheduleManager( &sm );
    
    int axisCount = model.axisCount();
    for ( int i = 0; i < axisCount; ++i ) {
        ChartAxisIndex ai = model.axisIndex( i );
        if ( ! ai.isValid() ) {
            continue;
        }
        //drawAxis( ai );
        //drawData( ai );
    }
   
}

void PlotWidget::drawAxis( const ChartAxisIndex &idx ) 
{
    kDebug();
    int axisCount = model.axisCount( idx );
    QList<double> range;
    for ( int i = 0; i < axisCount; ++i ) {
        ChartAxisIndex ai = model.axisIndex( i, idx );
        if ( ! ai.isValid() ) {
            continue;
        }
        if ( model.hasAxisChildren( ai ) ) {
            kDebug()<<"multiple axis";
        } else {
            range << model.axisData( ai, AbstractChartModel::AxisMinRole ).toDouble();
            range << model.axisData( ai, AbstractChartModel::AxisMaxRole ).toDouble();
        }
    }
    setLimits( range[0], range[1], range[2], range[3] );
}

void PlotWidget::drawData( const ChartAxisIndex &axisSet ) 
{
    kDebug();
    int dataCount = model.dataSetCount( axisSet );
    for ( int i = 0; i < dataCount; ++i ) {
        kDebug()<<"create data index";
        ChartDataIndex di = model.index( i, axisSet );
        kDebug()<<"created data index:"<<di.number()<<di.userData;
        if ( ! di.isValid() ) {
            kDebug()<<"Invalid index";
            continue;
        }
        if ( model.hasChildren( di ) ) {
            kDebug()<<"sections";
            int c = model.childCount( di );
            for ( int ii = 0; ii < c; ++ii ) {
                ChartDataIndex cidx = model.index( ii, di );
                drawData( cidx, axisSet );
            }
        } else {
            kDebug()<<"no sections, go direct to data";
            drawData( di, axisSet );
        }
    }
}

void PlotWidget::drawData( const ChartDataIndex &index, const ChartAxisIndex &axisSet ) 
{
    kDebug()<<index.number()<<index.userData;
    QVariantList data;
    int axisCount = model.axisCount( axisSet );
    for ( int j = 0; j < axisCount; ++j ) {
        ChartAxisIndex axis = model.axisIndex( j, axisSet );
        if ( model.hasAxisChildren( axis ) ) {
            kDebug()<<"multiple axis";
        } else {
            data << model.data( index, axis );
        }
    }
    kDebug()<<data;
    Q_ASSERT( data.count() == 2 );
    QVariantList x = data[0].toList();
    QVariantList y = data[1].toList();
    QVariant color = model.data( index, Qt::ForegroundRole );
    KPlotObject *kpo = new KPlotObject( color.value<QColor>(), KPlotObject::Lines );
    for (int i = 0; i < y.count(); ++i ) {
        kDebug()<<"Add point:"<<x[i].toInt() << y[i].toDouble();
        kpo->addPoint( x[i].toInt(), y[i].toDouble() );
        kpo->setShowLines(true);
    }
    addPlotObject( kpo );
}

}
