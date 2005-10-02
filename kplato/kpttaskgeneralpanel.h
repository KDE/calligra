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

#ifndef KPTTASKGENERALPANEL_H
#define KPTTASKGENERALPANEL_H

#include "kpttaskgeneralpanelbase.h"

class KMacroCommand;

namespace KPlato
{

class KPTTaskGeneralPanel;
class KPTRequestResourcesPanel;
class KPTPart;
class KPTTask;
class KPTStandardWorktime;

class KPTTaskGeneralPanel : public KPTTaskGeneralPanelBase {
    Q_OBJECT
public:
    KPTTaskGeneralPanel(KPTTask &task, KPTStandardWorktime *workTime=0, bool baseline=false, QWidget *parent=0, const char *name=0);

    KMacroCommand *buildCommand(KPTPart *part);

    bool ok();

    void setStartValues(KPTTask &task, KPTStandardWorktime *workTime=0);

public slots:
    virtual void estimationTypeChanged(int type);
    virtual void scheduleTypeChanged(int value);
    
private:
    KPTTask &m_task;
    int m_dayLength;
    
    KPTDuration m_effort;
    KPTDuration m_duration;
};

} //KPlato namespace

#endif // KPTTASKGENERALPANEL_H
