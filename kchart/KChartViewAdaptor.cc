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

#include "KChartViewAdaptor.h"

#include "kchart_view.h"


namespace KChart
{

ViewAdaptor::ViewAdaptor( KChartView *view_ )
    : QDBusAbstractAdaptor( view_ )
{
    setAutoRelaySignals(true);
    view = view_;
}

void ViewAdaptor::wizard()
{
    view->wizard();
}

void ViewAdaptor::editData()
{
    view->editData();
}

void ViewAdaptor::configureChart()
{
    view->slotConfig();
}

void ViewAdaptor::configureBackground()
{
    view->slotConfigBack();
}

void ViewAdaptor::configureFont()
{
    view->slotConfigFont();
}

void ViewAdaptor::configureColor()
{
    view->slotConfigColor();
}

void ViewAdaptor::configureLegend()
{
    view->slotConfigLegend();
}

void ViewAdaptor::updateGuiTypeOfChart()
{
    view->updateGuiTypeOfChart();
}

void ViewAdaptor::saveConfig()
{
    view->saveConfig();
}

void ViewAdaptor::loadConfig()
{
    view->loadConfig();
}

void ViewAdaptor::defaultConfig()
{
    view->defaultConfig();
}

void ViewAdaptor::configSubTypeChart()
{
    view->slotConfigSubTypeChart();
}

void ViewAdaptor::configHeaderFooter()
{
    view->slotConfigHeaderFooterChart();
}

}  //KChart namespace

#include "KChartViewAdaptor.moc"
