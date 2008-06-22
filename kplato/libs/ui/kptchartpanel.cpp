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

ChartPanel::ChartPanel(QWidget *parent) : ChartPanelBase(parent)
{
    //frame->setBackgroundRole( QPalette::Base );
    //chart = new ChartWidget(chartFrame );
    chart = new PlotWidget( chartFrame );
    QHBoxLayout *l = new QHBoxLayout( chartFrame );
    l->addWidget( chart );
    
    //chart->update();
    connect(curve2,SIGNAL(clicked()),SLOT(slotBCWP()));
    connect(curve1,SIGNAL(clicked()),SLOT(slotBCWS()));
    connect(curve3,SIGNAL(clicked()),SLOT(slotACWP()));
    is_bcwp_draw=false;
    is_bcws_draw=true;
    curve1->setChecked(true);
    is_acwp_draw=false;
}

void ChartPanel::clear()
{
    chart->clear();
}

void ChartPanel::draw( Project &project, ScheduleManager &sm )
{
    chart->draw( project, sm );

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
