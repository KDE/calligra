/*  -*- C++ -*-
    This file is part of the KDE libraries
    Copyright (C) 2003 Jason Harris <kstars@30doradus.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KPTPLOTWIDGET_H
#define KPTPLOTWIDGET_H

#include <kplotwidget.h>

#include "kptchart.h"
#include <QWidget>
#include <QPainter>
#include <QBrush>
#include <string>


namespace KPlato
{


class PlotWidget : public KPlotWidget {

private:
    
    KPlotObject *mKpoBCWP;
    KPlotObject *mKpoBCWS;
    KPlotObject *mKpoACWP;
    
    Chart chartEngine;

public:

    PlotWidget(QWidget *parent=0);
    ~PlotWidget();

    void draw( Project &project, ScheduleManager &sm );
    
    void drawBCWP();
    void undrawBCWP();
    void drawBCWS();
    void undrawBCWS();
    void drawACWP();
    void undrawACWP();
};

}

#endif
