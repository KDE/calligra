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

#ifndef KCHART_VIEW_ADAPTOR_H
#define KCHART_VIEW_ADAPTOR_H

#include <dbus/qdbus.h>

#include <QString>

namespace KChart
{

class KChartView;

class ViewAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.koffice.chart.view")
public:
    ViewAdaptor( KChartView *view_ );

public Q_SLOTS: // METHODS
    virtual void wizard();
    virtual void editData();
    virtual void configureChart();

    virtual void configureBackground();

    virtual void configureFont();
    virtual void configureColor();
    virtual void configureLegend();
    virtual void configSubTypeChart();
    virtual void configHeaderFooter();

    virtual void updateGuiTypeOfChart();

    virtual void saveConfig();
    virtual void loadConfig();
    virtual void defaultConfig();

private:
    KChartView *view;

};

}  //KChart namespace

#endif
