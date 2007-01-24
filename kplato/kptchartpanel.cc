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
hboxLayout1->addWidget(chart,1,0);

horizontalLayout_2->setGeometry(QRect(20, 20, 600, 300));
horizontalLayout->setGeometry(QRect(20, 306, 600, 150));


chart->update();
}
    
void ChartPanel::slotClose() {
	
}

} //namespace KPlato

#include "kptchartpanel.moc"

