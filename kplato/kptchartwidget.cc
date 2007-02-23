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


ChartWidget::ChartWidget(QWidget *parent, const char *name) : QWidget(parent,name)
{
    kDebug() << "ChartWidget :: Constructor";
    setMaximumSize(600,350);

    is_bcwp_draw=false;
    is_bcws_draw=false;
    is_acwp_draw=false;

    /* TEST */
    bcwpPoints.push_back(QPointF(0,0));
    bcwpPoints.push_back(QPointF(10,10));
    bcwpPoints.push_back(QPointF(40,20));
    bcwpPoints.push_back(QPointF(70,40));
    bcwpPoints.push_back(QPointF(100,70));

    maxYPercent=chartEngine.setMaxYPercent(bcwpPoints,bcwsPoints,acwpPoints);
    maxXPercent=chartEngine.setMaxYPercent(bcwpPoints,bcwsPoints,acwpPoints);
    //chartEngine.setMaxPercent(bcwpPoints,bcwsPoints,acwpPoints);
    //chartEngine.setMaxCost(bcwpPoints);
    kDebug() << "ChartWidget :: Constructor Ended"<<endl;
}


ChartWidget::~ChartWidget()
{

}


void ChartWidget::paintEvent(QPaintEvent * ev)
{
    int j=0;
    int k=0;
    int Xpercent=0;
    int Ypercent=0;
    maxXPercent=100;
    char Xchar[30];
    char Ychar[30];

    //this->updateGeometry();
    QPainter painter(this);

    /* CHANGE COLORS !! */
    painter.setPen(QColor(Qt::blue));
    painter.drawText(200,150,"I am a Chart!");

   /* attributes :  chartEngine1er : par rapport au coté, 2eme : par rapport au haut !  */
    painter.drawText(2, 10,"Budget");
    //painter.drawText(size().width()-15, size().height()-20,"Time");
    painter.drawText(size().width()-70,size().height() ,"Time");

    //Y
    painter.drawLine(QLine(LEFTMARGIN,TOPMARGIN,LEFTMARGIN,size().height()-BOTTOMMARGIN));
    
//float MarginY =(size().height()-30)/10; // 10 : Number of division
//float MaxCost=chartEngine.setMaxYPercent()/10;
//while(j<=(size().height()-(TOPMARGIN+BOTTOMMARGIN)))
    while (Ypercent<=maxYPercent)
    {
	
	int n=sprintf(Ychar,"%d",Ypercent);
	char * Yaffichage =strcat(Ychar,"%");
	painter.drawText(2,size().height()-BOTTOMMARGIN-Ypercent,Yaffichage);
        painter.drawLine(QLine(8,j+TOPMARGIN,LEFTMARGIN,j+TOPMARGIN));
        j+=10;// FIX IT , it MUST BE COORDINATE , NOT % !!!!
	painter.drawLine(QLine(8,j+TOPMARGIN,LEFTMARGIN,j+TOPMARGIN));
        j+=10;

	Ypercent+=20;
	strcpy(Ychar,"");
    }

    //X
    painter.drawLine(QLine(LEFTMARGIN,size().height()-BOTTOMMARGIN,size().width()-RIGHTMARGIN,size().height()-BOTTOMMARGIN));

//float MarginX=(size().width()-(RIGHTMARGIN+LEFTMARGIN))/10;
//float MaxPercent=chartEngine.setMaxXPercent()/10;
//while(k<=(size().width()-RIGHTMARGIN))/*
    while (Xpercent<=maxXPercent)
    {
	int n=sprintf(Xchar,"%d",Xpercent);
	char * Xaffichage =strcat(Xchar,"%");

	painter.drawText(Xpercent+LEFTMARGIN,size().height(),Xaffichage);
	Xpercent+=20;
	
	painter.drawLine(QLine(k+LEFTMARGIN,size().height()-TOPMARGIN,k+LEFTMARGIN,size().height()-13));
        k+=10;
	painter.drawLine(QLine(k+LEFTMARGIN,size().height()-TOPMARGIN,k+LEFTMARGIN,size().height()-13));
        k+=10;
	strcpy(Xchar,"");
    }

    if(is_bcwp_draw==true)
    {
        painter.setPen(QColor(Qt::red));
       //	chartEngine.api(bcwpPoints,bcwsPoints,acwpPoints,BCWP,size().height(),size().width());
      //  painter.drawPolyline(QPolygonF(bcwpPoints));
        is_bcwp_draw=true;
    }

    if(is_bcws_draw==true){
        painter.setPen(QColor(Qt::yellow));
        painter.drawPolyline(QPolygonF(bcwsPoints));
        painter.drawLine(QLine(LEFTMARGIN,size().height()-TOPMARGIN,size().width()-10,150));
        is_bcws_draw=true;
    }

    if(is_acwp_draw==true)
    {
        painter.setPen(QColor(Qt::green));
        //painter.drawPolyline(QPolygonF(acwpPoints));
        painter.drawLine(QLine(LEFTMARGIN,size().height()-TOPMARGIN,size().width()-10,100));
        is_acwp_draw=true;
    }
    

}// end PaintEvent();

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
