/* This file is part of the KDE project
   Copyright (C) 2005 Frédéric Lambert <konkistadorr@gmail.com>

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
#include "kptchartwidget.h"

#include <klocale.h>
#include <kdebug.h>
#include <stdio.h>



namespace KPlato
{


ChartWidget::ChartWidget(Project &p, QWidget *parent, const char *name) : QWidget(parent,name)
{
    kDebug() << "ChartWidget :: Constructor"<<endl;
    setMaximumSize(610,350);

    is_bcwp_draw=false;
    is_bcws_draw=true;
    is_acwp_draw=false;


    kDebug() << "ChartWidget :: Constructor Ended"<<endl;

    //Calculer ici les indicateurs relatifs au projet!!!

    chartEngine.calculateWeeks(weeks,p);
    chartEngine.initXCurvesVectors(weeks,bcwpPoints,bcwsPoints,acwpPoints);

    chartEngine.calculateActualCost(acwpPoints,weeks,p);
    chartEngine.calculatePlannedCost(bcwsPoints,weeks,p);
    
    chartEngine.setMaxCost(bcwsPoints);

    chartEngine.costToPercent(acwpPoints);
    chartEngine.costToPercent(bcwsPoints);

    //chartEngine.costToPercent(bcwpPoints);

    maxYPercent=chartEngine.setMaxYPercent(bcwpPoints,bcwsPoints,acwpPoints);

    //chartEngine.timeToPercent(bcwpPoints);
    chartEngine.timeToPercent(acwpPoints);
    chartEngine.timeToPercent(bcwsPoints);

    chartEngine.init_display(bcwpPoints_display,bcwsPoints_display,acwpPoints_display,weeks.size());
}

void ChartWidget::paintEvent(QPaintEvent * ev)
{
   
    //this->updateGeometry();
    QPainter painter(this);

    drawBasicChart(painter);

    if(is_bcwp_draw==true)
    {
        painter.setPen(QColor(Qt::black));
        chartEngine.api(bcwpPoints,bcwsPoints,acwpPoints,bcwpPoints_display,bcwsPoints_display,acwpPoints_display,BCWP,size().height(),size().width());
        painter.drawPolyline(QPolygonF(bcwpPoints_display));
        is_bcwp_draw=true;
    }

    if(is_bcws_draw==true){
        painter.setPen(QColor(Qt::red));
        kDebug()<<" Height : "<<size().height()<<" Width : "<<size().width()<<endl;
        chartEngine.api(bcwpPoints,bcwsPoints,acwpPoints,bcwpPoints_display,bcwsPoints_display,acwpPoints_display,BCWS,size().height(),size().width());
        painter.drawPolyline(QPolygonF(bcwsPoints_display));
        //painter.drawLine(QLine(LEFTMARGIN,size().height()-TOPMARGIN,size().width()-10,150));
        is_bcws_draw=true;
    }

    if(is_acwp_draw==true)
    {
        painter.setPen(QColor(Qt::green));
        chartEngine.api(bcwpPoints,bcwsPoints,acwpPoints,bcwpPoints_display,bcwsPoints_display,acwpPoints_display,ACWP,size().height(),size().width());
        painter.drawPolyline(QPolygonF(acwpPoints_display));
        //painter.drawLine(QLine(LEFTMARGIN,size().height()-TOPMARGIN,size().width()-10,100));
        is_acwp_draw=true;
    }

}// end PaintEvent();


void ChartWidget::drawBasicChart(QPainter & painter)
    /* Calculate the percentage of the cost and replace the result in the vector */
{
    int j=0;
    int k=0;
    int Ypercent=0;
    int Xpercent=0;
    maxXPercent=100;
    char Xchar[30];
    char Ychar[30];
    
/* CHANGE COLORS !! */
    painter.setPen(QColor(Qt::blue));

   /* attributes :  chartEngine1er : par rapport au coté, 2eme : par rapport au haut !  */
    painter.drawText(2, 10,"Budget");
    //painter.drawText(size().width()-15, size().height()-20,"Time");
    painter.drawText(size().width()-70,size().height(),"Time");

    //Y
    painter.drawLine(QLine(LEFTMARGIN,TOPMARGIN,LEFTMARGIN,size().height()-BOTTOMMARGIN));
    kDebug()<<"maxYpercent: "<<maxYPercent<<endl;
    float MarginY_base =(size().height()-(TOPMARGIN+BOTTOMMARGIN))/(maxYPercent/10);// Number of division : 10% to 10%
    float MarginY=0;
    while(Ypercent<=maxYPercent)
    {
            int n=sprintf(Ychar,"%d",Ypercent);
            char * Yaffichage =strcat(Ychar,"%");
            //error first time FIX ME
            painter.drawText(2,size().height()-BOTTOMMARGIN-MarginY,QString(Yaffichage));
            Ypercent+=20;
            //painter.drawLine(QLine(LEFTMARGIN-5,size().height()-(BOTTOMMARGIN)+MarginY,LEFTMARGIN,size().height()-(BOTTOMMARGIN)+MarginY));
            painter.drawLine(QLine(LEFTMARGIN-4,size().height()-(BOTTOMMARGIN)-MarginY,LEFTMARGIN,size().height()-(BOTTOMMARGIN)-MarginY));
            MarginY+=MarginY_base;// FIX IT , it MUST BE COORDINATE , NOT % !!!!
            painter.drawLine(QLine(LEFTMARGIN-4,size().height()-(BOTTOMMARGIN)-MarginY,LEFTMARGIN,size().height()-(BOTTOMMARGIN)-MarginY));
            MarginY+=MarginY_base;
            strcpy(Ychar,"");
    }
    //X
    painter.drawLine(QLine(LEFTMARGIN,size().height()-BOTTOMMARGIN,size().width()-RIGHTMARGIN,size().height()-BOTTOMMARGIN));
   float MarginX_base=(size().width()-(RIGHTMARGIN+LEFTMARGIN))/(weeks.size()-1);
   float MarginX=0;
   QVector<QPointF>::iterator it_time = bcwsPoints.begin();
   int i=0;
   while(i<weeks.size())
   {
        int n=sprintf(Xchar,"%d",(int)it_time->x());
        char * Xaffichage =strcat(Xchar,"%");
        painter.drawText(MarginX+LEFTMARGIN,size().height()-BOTTOMMARGIN+19,Xaffichage);
        painter.drawLine(QLine(MarginX+LEFTMARGIN,size().height()-BOTTOMMARGIN,MarginX+LEFTMARGIN,size().height()-BOTTOMMARGIN+4));
        MarginX+=MarginX_base;
        strcpy(Xchar,"");
        i++;
        it_time++;
   }
}

void ChartWidget::drawBCWP(){
    is_bcwp_draw=true;
    this->update();
}

void ChartWidget::undrawBCWP(){
    is_bcwp_draw=false;
    this->update();
}
void ChartWidget::drawBCWS(){
    is_bcws_draw=true;
    this->update();
}

void ChartWidget::undrawBCWS(){
    is_bcws_draw=false;
    this->update();
}
void ChartWidget::drawACWP(){
    is_acwp_draw=true;
    this->update();
}

void ChartWidget::undrawACWP(){
    is_acwp_draw=false;
    this->update();
}

/* API CURVES TO GET POINTS*/

/* GETTERS AND SETTERS */ 

void ChartWidget::setPointsBCPW(QVector<QPointF> vec)
{
    this->bcwpPoints = vec;
}
void ChartWidget::setPointsBCPS(QVector<QPointF> vec)
{
    this->bcwsPoints = vec;
}
void ChartWidget::setPointsACPW(QVector<QPointF> vec)
{
    this->acwpPoints = vec;
}


} // namespace Kplato
