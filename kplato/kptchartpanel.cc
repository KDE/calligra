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
bcwp=false;
bcws=false;
acwp=false;
}
    

void ChartPanel::slotBCPW()
{
	if(bcwp==false)
	{
		chart->drawBCWP();
		bcwp=true;
	}
	else
	{
		chart->undrawBCWP();
		bcwp=false;
	}

}
void ChartPanel::slotBCPS()
{
	if(bcws==false)
	{
		chart->drawBCWS();
		bcws=true;
	}
	else
	{
		chart->undrawBCWS();
		bcws=false;
	}
}
void ChartPanel::slotACPW()
{
	if(acwp==false){
		chart->drawACWP();
		acwp=true;
	}
	else
	{
		chart->undrawACWP();
		acwp=false;
	}
}   


} //namespace KPlato

#include "kptchartpanel.moc"
