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

ChartPanel::ChartPanel(Project &p, QWidget *parent) : ChartPanelBase(parent)
{
    chart = new ChartWidget(p,horizontalLayout_2,"MyChartWidget");
    hboxLayout->setMargin(0);
    hboxLayout->setSpacing(0);
    hboxLayout->addWidget(chart,1,0);

    horizontalLayout_2->setMinimumSize(610, 350);
    horizontalLayout_2->setGeometry(QRect(0, 0, 610, 350));

    verticalLayout->setGeometry(QRect(0, 365, 610, 80));
    verticalLayout->setMinimumSize(610,80);

    //chart->update();
    connect(curve2,SIGNAL(clicked()),SLOT(slotBCWP()));
    connect(curve1,SIGNAL(clicked()),SLOT(slotBCWS()));
    connect(curve3,SIGNAL(clicked()),SLOT(slotACWP()));
    is_bcwp_draw=false;
    is_bcws_draw=true;
    curve1->setChecked(true);
    curve3->setChecked(true);
    is_acwp_draw=true;

}

void ChartPanel::resizeEvent(QResizeEvent* ev)
{
    chart->setMaximumWidth((ev->size()).width());
    chart->setMaximumHeight((ev->size()).height()-100);
    horizontalLayout_2->setGeometry(QRect(0, 0, (ev->size()).width(), (ev->size()).height()-100));
    verticalLayout->setGeometry(QRect(10, (ev->size()).height()-80, 610, 80));
}

void ChartPanel::slotBCWP()
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
void ChartPanel::slotBCWS()
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
void ChartPanel::slotACWP()
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
