/* This file is part of the KDE project
   Copyright (C) 2005 - 2007 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTCHARTPANEL_H
#define KPTCHARTPANEL_H

#include "ui_kptchartpanelbase.h"
#include "kptchartwidget.h"
#include "kptplotwidget.h"
#include <qevent.h>
#include <QHash>

class QTreeWidget;
class QWidget;



namespace KPlato
{
class ScheduleManager;

class ChartPanelBase : public QWidget, public Ui::ChartPanelBase
{

public:
  explicit ChartPanelBase( QWidget *parent ) : QWidget( parent ) {
    setupUi( this );
  }
};


class ChartPanel : public ChartPanelBase {
    Q_OBJECT

private:
    //ChartWidget * chart;
    PlotWidget *   chart;
    bool is_bcwp_draw;
    bool is_bcws_draw;
    bool is_acwp_draw;

public:
    ChartPanel(QWidget *parent=0);
    
    void clear();
    void draw( Project &project, ScheduleManager &sm );

public slots:
    void slotBCWP();
    void slotBCWS();
    void slotACWP();
};

} //namespace KPlato

#endif
