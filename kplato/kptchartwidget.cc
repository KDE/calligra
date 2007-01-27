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
setMaximumSize(600,300);
//setFont(new QFont(blue));
}


void ChartWidget::paintEvent(QPaintEvent * ev)
{
QPainter painter(this);
kDebug() << "Print it PLease :D";

/* CHANGE COLORS !! */
painter.setPen(QColor(Qt::blue));

//painter.fillRect(QRectF(-1000, -1000, 1000, 1000),QBrush(QColor(Qt::red))); 
painter.drawText(200,150,"I am a Chart!");

/* attributes :  1er : par rapport au coté, 2eme : par rapport au haut !
*/
painter.drawText(5, 10,"Budget");
//painter.drawText(maximumWidth()-15, maximumHeight()-20,"Time");
painter.drawText(530,300 ,"Time");
painter.drawLine(QLine(12,15,12,maximumHeight()-10));
painter.drawLine(QLine(12,maximumHeight()-10,maximumWidth()-10,maximumHeight()-10));

painter.end();
}

} // namespace Kplato
