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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KPTRESOURCEUSEVIEW_H
#define KPTRESOURCEUSEVIEW_H

#include <qsplitter.h>

class KListView;
class QPaintEvent;
class QListViewItem;

namespace KPlato
{

class KPTChartView;
class KPTView;
class KPTProject;
class KPTResource;

class ResourceGroupItemPrivate;

class KPTResourceUseView : public QSplitter
{
    Q_OBJECT
public:
    KPTResourceUseView(KPTView *view, QWidget* parent = 0, const char* name = 0);
    ~KPTResourceUseView();

public slots:
    void draw(KPTProject &project);
    
protected:
    void drawResources(ResourceGroupItemPrivate *parent);
    void drawChart();
    void drawLoadPrDay(KPTResource *resource);
    void drawPeek(KPTResource *resource);
    
protected slots:
    void executed(QListViewItem *item);
    
private:
    KPTView *m_mainview;
    KListView *m_resourcelist;
    KPTChartView *m_chartview;
    QListViewItem *m_selectedItem;

};

}  //KPlato namespace

#endif


