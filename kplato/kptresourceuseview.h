/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTRESOURCEUSEVIEW_H
#define KPTRESOURCEUSEVIEW_H

#include <qsplitter.h>

#include "chart/kpttimescale.h"

class KListView;
class KAction;
class KActionCollection;
class QPaintEvent;
class QListViewItem;
class QPoint;

namespace KPlato
{

class KPTChartView;
class KPTChartDataSet;
class KPTView;
class KPTProject;
class KPTResource;
class KPTResourceGroup;
class KPTProject;
class KPTContext;

class ResourceGroupItemPrivate;

class KPTResourceUseView : public QSplitter
{
    Q_OBJECT
public:
    KPTResourceUseView(KPTView *view, QWidget* parent = 0, const char* name = 0);
    ~KPTResourceUseView();

    void initMenu(KActionCollection *collection);
    
    virtual bool setContext(KPTContext &context);
    virtual void getContext(KPTContext &context) const;

public slots:
    void draw(KPTProject &project);
    void slotLoadPrMinute();
    void slotLoadPrHour();
    void slotLoadPrDay();
    void slotLoadPrWeek();
    void slotLoadPrMonth();
    void slotAccumulated();
    
protected:
    KPTResource *currentResource();
    KPTResourceGroup *currentGroup();
    
    void drawResources(ResourceGroupItemPrivate *parent);
    void drawChart();
    void drawBarLoad(KPTResource *resource);
    KPTChartDataSet *drawBarLoadPrMinute(KPTResource *resource);
    KPTChartDataSet *drawBarLoadPrHour(KPTResource *resource);
    KPTChartDataSet *drawBarLoadPrDay(KPTResource *resource);
    KPTChartDataSet *drawBarLoadPrWeek(KPTResource *resource);
    KPTChartDataSet *drawBarLoadPrMonth(KPTResource *resource);
    void drawBarPeekLoad(KPTResource *resource);
    void drawLineAccumulated(KPTResource *resource);
    void drawLineAccumulated(KPTResourceGroup *resource);
    void drawLineAccumulated();
    
protected slots:
    void executed(QListViewItem *item);
    void slotChartMenuRequested(const QPoint &pos);
    
private:
    KPTView *m_mainview;
    KListView *m_resourcelist;
    KPTChartView *m_chartview;
    QListViewItem *m_selectedItem;
    KPTTimeHeaderWidget::Scale m_timeScaleUnit;
    KPTProject *m_project;

    enum ShowData { LoadPrMinute, LoadPrHour, LoadPrDay, LoadPrWeek, LoadPrMonth };
    ShowData m_dataShown;
     
    KAction *actionResourceuseLoadMinute;
    KAction *actionResourceuseLoadHour;
    KAction *actionResourceuseLoadDay;
    KAction *actionResourceuseLoadWeek;
    KAction *actionResourceuseLoadMonth;
    KAction *actionResourceuseAccumulated;

};

}  //KPlato namespace

#endif


