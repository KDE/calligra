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
#include "kptchartpanel.h"

#include <klocale.h>

#include <kdebug.h>

namespace KPlato
{




ChartPanel::ChartPanel(QWidget *p) : ChartPanelBase(p)
{

kDebug()<<"Chart Panel : constructeur!!";
chart = new ChartWidget(horizontalLayout_2,"MyChartWidget");
hboxLayout->addWidget(chart,1,0);

horizontalLayout_2->setGeometry(QRect(10, 10, 600, 350));
verticalLayout->setGeometry(QRect(10, 365, 600, 80));

chart->update();
connect(curve2,SIGNAL(clicked()),SLOT(slotBCPW()));
connect(curve1,SIGNAL(clicked()),SLOT(slotBCPS()));
connect(curve3,SIGNAL(clicked()),SLOT(slotACPW()));
is_bcwp_draw=false;
is_bcws_draw=false;
is_acwp_draw=false;
}
   
ChartPanel::~ChartPanel()
{
	delete chart;
}

void ChartPanel::resizeEvent(QResizeEvent* ev)
{
	kDebug()<<"RESIZAGEEEEEEEEEEEEEEEE"<<(ev->size()).width()<<"    "<<(ev->size()).height();
	chart->setMaximumWidth((ev->size()).width());
	chart->setMaximumHeight((ev->size()).height());
}



void ChartPanel::slotBCPW()
{
	if(is_bcwp_draw==false)
	{
		chart->drawBCWP();
		is_bcwp_draw=true;
	}
	else
	{
		chart->undrawBCWP();
		is_bcwp_draw=false;
	}
}
void ChartPanel::slotBCPS()
{
	if(is_bcws_draw==false)
	{
		chart->drawBCWS();
		is_bcws_draw=true;
	}
	else
	{
		chart->undrawBCWS();
		is_bcws_draw=false;
	}
}
void ChartPanel::slotACPW()
{
	if(is_acwp_draw==false){
		chart->drawACWP();
		is_acwp_draw=true;
	}
	else
	{
		chart->undrawACWP();
		is_acwp_draw=false;
	}
}   


} //namespace KPlato

#include "kptchartpanel.moc"
