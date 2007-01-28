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
curve_draw=0;
bcwp=false;
bcws=false;
acwp=false;
}


void ChartWidget::paintEvent(QPaintEvent * ev)
{
painter=new QPainter(this);
kDebug() << "Print it PLease :D";
/* CHANGE COLORS !! */
painter->setPen(QColor(Qt::blue));

//painter.fillRect(QRectF(-1000, -1000, 1000, 1000),QBrush(QColor(Qt::red))); 
painter->drawText(200,150,"I am a Chart!");

/* attributes :  1er : par rapport au coté, 2eme : par rapport au haut !
*/
painter->drawText(2, 10,"Budget");
//painter.drawText(maximumWidth()-15, maximumHeight()-20,"Time");
painter->drawText(maximumWidth()-70,maximumHeight() ,"Time");
painter->drawLine(QLine(10,15,10,maximumHeight()-15));
painter->drawLine(QLine(10,maximumHeight()-15,maximumWidth()-10,maximumHeight()-15));

if(bcwp==true)
{
    painter->setPen(QColor(Qt::red));
    // DRAW BCWP FUNCTION
    painter->drawLine(QLine(10,maximumHeight()-15,maximumWidth()-10,32));
    bcwp=true;
}
else{

}
if(bcws==true){
    painter->setPen(QColor(Qt::yellow));
    // DRAW BCWP FUNCTION
    painter->drawLine(QLine(10,maximumHeight()-15,maximumWidth()-10,150));
    bcws=true;
}
else
{

}
if(acwp==true)
{
    painter->setPen(QColor(Qt::green));
    // DRAW BCWP FUNCTION
    painter->drawLine(QLine(10,maximumHeight()-15,maximumWidth()-10,100));
    acwp=true;
}
else{ 

}	
painter->end();
}

void ChartWidget::drawBCWP(){
	bcwp=true;
	this->update();
}

void ChartWidget::undrawBCWP(){
	bcwp=false;
	this->update();
}
void ChartWidget::drawBCWS(){
	bcws=true;
	this->update();
}

void ChartWidget::undrawBCWS(){
	bcws=false;
	this->update();
}
void ChartWidget::drawACWP(){
	acwp=true;
	this->update();
}

void ChartWidget::undrawACWP(){
	acwp=false;
	this->update();
}

} // namespace Kplato
