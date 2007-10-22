/* This file is part of the KDE project
   Copyright (C) 2007 Dag Andersen <kplato@kde.org>

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

#ifndef KPTCHARTVIEW_H
#define KPTCHARTVIEW_H

#include "kptviewbase.h"

class QWidget;

namespace KPlato
{

class ChartPanel;
class Part;
class Project;
class ScheduleManager;

class ChartView : public ViewBase 
{
    Q_OBJECT
public:
    explicit ChartView( Part *part, QWidget *parent=0 );
    
    virtual void setProject( Project *project );
    
public slots:
    void setScheduleManager( ScheduleManager *sm );

protected slots:
    void slotNodeChanged( Node* );
    
private:
    ChartPanel *m_panel;
    Project *m_project;
    ScheduleManager *m_manager;

};

} //namespace KPlato

#endif
