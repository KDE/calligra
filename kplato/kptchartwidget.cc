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
bcwpPoints.push_back(QPointF(125,20));
bcwpPoints.push_back(QPointF(200,50));
bcwpPoints.push_back(QPointF(400,70));
bcwpPoints.push_back(QPointF(600,100));
kDebug() << "ChartWidget :: Constructor Ended";

chartEngine.setMaxVector(bcwpPoints);
}

void ChartWidget::paintEvent(QPaintEvent * ev)
{
int j,k=0;

this->updateGeometry();
QPainter painter(this);
kDebug() << "Print it PLease :D UPDATE"<<maximumHeight()<<" UPDATE :"<<maximumWidth()<<" UPDATE PAINTEVENT !!!!!!!!!!!!!";
/* CHANGE COLORS !! */


painter.setPen(QColor(Qt::blue));
painter.drawText(200,150,"I am a Chart!");

/* attributes :  1er : par rapport au coté, 2eme : par rapport au haut !
*/
painter.drawText(2, 10,"Budget");
//painter.drawText(maximumWidth()-15, maximumHeight()-20,"Time");
painter.drawText(maximumWidth()-70,maximumHeight() ,"Time");

kDebug()<<"ICI 2";

painter.drawLine(QLine(LEFTMARGIN,TOPMARGIN,LEFTMARGIN,maximumHeight()-BOTTOMMARGIN)); //Y
int MarginY =(maximumHeight()-30)/10; // 10 : Number of division
while(j<=(maximumHeight()-(TOPMARGIN+BOTTOMMARGIN)))
{
    painter.drawLine(QLine(8,j+TOPMARGIN,LEFTMARGIN,j+TOPMARGIN));
    j+=MarginY;
kDebug()<<"ICI 3";
}

painter.drawLine(QLine(LEFTMARGIN,maximumHeight()-BOTTOMMARGIN,maximumWidth()-RIGHTMARGIN,maximumHeight()-BOTTOMMARGIN)); //X
int MarginX=(maximumWidth()-(RIGHTMARGIN+LEFTMARGIN))/10;
while(k<=(maximumWidth()-RIGHTMARGIN))
{
    painter.drawLine(QLine(k+LEFTMARGIN,maximumHeight()-TOPMARGIN,k+LEFTMARGIN,maximumHeight()-13));
    k+=MarginX;
kDebug()<<"ICI 4";
}

if(is_bcwp_draw==true)
{
    painter.setPen(QColor(Qt::red));
    //chartEngine.reCalculateY(bcwpPoints,BCWP,maximumHeight());
    painter.drawPolyline(QPolygonF(bcwpPoints));
    is_bcwp_draw=true;
}
kDebug()<<"ICI 5";
if(is_bcws_draw==true){
    painter.setPen(QColor(Qt::yellow));
    //painter.drawPolyline(QPolygonF(bcwsPoints));
    painter.drawLine(QLine(LEFTMARGIN,maximumHeight()-TOPMARGIN,maximumWidth()-10,150));
    is_bcws_draw=true;
}

if(is_acwp_draw==true)
{
    painter.setPen(QColor(Qt::green));
    //painter.drawPolyline(QPolygonF(acwpPoints));
    painter.drawLine(QLine(LEFTMARGIN,maximumHeight()-TOPMARGIN,maximumWidth()-10,100));
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
