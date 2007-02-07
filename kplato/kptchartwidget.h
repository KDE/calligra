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


#ifndef KPTCHARTWIDGET_H
#define KPTCHARTWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QBrush>
namespace KPlato
{



class ChartWidget : public QWidget{

private:
int curve_draw;
QPainter painter;
bool is_bcwp_draw;
bool is_bcws_draw;
bool is_acwp_draw;

QVector<QPointF> bcwpPoints;
QVector<QPointF> bcwsPoints;
QVector<QPointF> acwpPoints;

public:
    ChartWidget(QWidget *parent=0, const char *name=0);

    void paintEvent(QPaintEvent * ev);

    void drawBCWP();
    void undrawBCWP();
    void drawBCWS();
    void undrawBCWS();
    void drawACWP();
    void undrawACWP();
    void setPointsBCPW(QVector<QPointF> );
    void setPointsBCPS(QVector<QPointF> );
    void setPointsACPW(QVector<QPointF> );
    const int getMaximumWidth();
    const int getMaximumHeight();
};

} //namespace KPlato

#endif
