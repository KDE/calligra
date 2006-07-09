/* This file is part of the KDE project
   Copyright (C) 2001,2002,2003,2004 Laurent Montel <montel@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include "KChartViewIface.h"

#include "kchart_view.h"

#include <dcopclient.h>

namespace KChart
{

KChartViewIface::KChartViewIface( KChartView *view_ )
    : KoViewIface( view_ )
{
    view = view_;
}

void KChartViewIface::wizard()
{
    view->wizard();
}

void KChartViewIface::editData()
{
    view->editData();
}

void KChartViewIface::configureChart()
{
    view->slotConfig();
}

void KChartViewIface::configureBackground()
{
    view->slotConfigBack();
}

void KChartViewIface::configureFont()
{
    view->slotConfigFont();
}

void KChartViewIface::configureColor()
{
    view->slotConfigColor();
}

void KChartViewIface::configureLegend()
{
    view->slotConfigLegend();
}

void KChartViewIface::updateGuiTypeOfChart()
{
    view->updateGuiTypeOfChart();
}

void KChartViewIface::saveConfig()
{
    view->saveConfig();
}

void KChartViewIface::loadConfig()
{
    view->loadConfig();
}

void KChartViewIface::defaultConfig()
{
    view->defaultConfig();
}

void KChartViewIface::configSubTypeChart()
{
    view->slotConfigSubTypeChart();
}

void KChartViewIface::configHeaderFooter()
{
    view->slotConfigHeaderFooterChart();
}

}  //KChart namespace
