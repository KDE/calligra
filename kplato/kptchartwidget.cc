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
//setFont(new QFont(blue));

is_bcwp_draw=false;
is_bcws_draw=false;
is_acwp_draw=false;

/* TEST */
bcwpPoints.push_back(QPointF(10,maximumHeight()-15));
bcwpPoints.push_back(QPointF(125,250));
bcwpPoints.push_back(QPointF(200,200));
bcwpPoints.push_back(QPointF(400,175));
bcwpPoints.push_back(QPointF(600,40));
}


void ChartWidget::paintEvent(QPaintEvent * ev)
{
int j,k=0;
this->updateGeometry();
QPainter painter(this);
kDebug() << "Print it PLease :D UPDATE"<<maximumHeight()<<" UPDATE :"<<maximumWidth()<<" UPDATE PAINTEVENT !!!!!!!!!!!!!";
/* CHANGE COLORS !! */
painter.setPen(QColor(Qt::blue));

//painter.fillRect(QRectF(-1000, -1000, 1000, 1000),QBrush(QColor(Qt::red))); 
painter.drawText(200,150,"I am a Chart!");

/* attributes :  1er : par rapport au coté, 2eme : par rapport au haut !
*/
painter.drawText(2, 10,"Budget");
//painter.drawText(maximumWidth()-15, maximumHeight()-20,"Time");
painter.drawText(maximumWidth()-70,maximumHeight() ,"Time");



painter.drawLine(QLine(10,15,10,maximumHeight()-15)); //Y

int MarginY =(maximumHeight()-30)/10;
while(j<=(maximumHeight()-15))
{
	painter.drawLine(QLine(8,j+15,10,j+15));
	j+=MarginY;
}

painter.drawLine(QLine(10,maximumHeight()-15,maximumWidth()-10,maximumHeight()-15)); //X

int MarginX=(maximumWidth()-20)/10;
while(k<=(maximumWidth()-10))
{
	painter.drawLine(QLine(k+10,maximumHeight()-15,k+10,maximumHeight()-13));
	k+=MarginX;
}

if(is_bcwp_draw==true)
{
    painter.setPen(QColor(Qt::red));    
    bcwpPoints[0].setY(maximumHeight()-15);
    painter.drawPolyline(QPolygonF(bcwpPoints));
   // painter.drawLine(QLine(10,maximumHeight()-15,maximumWidth()-10,32));
    is_bcwp_draw=true;
}

if(is_bcws_draw==true){
    painter.setPen(QColor(Qt::yellow));
    //painter.drawPolyline(QPolygonF(bcwsPoints));
    painter.drawLine(QLine(10,maximumHeight()-15,maximumWidth()-10,150));
    is_bcws_draw=true;
}

if(is_acwp_draw==true)
{
    painter.setPen(QColor(Qt::green));
    //painter.drawPolyline(QPolygonF(acwpPoints));
    painter.drawLine(QLine(10,maximumHeight()-15,maximumWidth()-10,100));
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
