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
    KPTTaskGeneralPanel(KPTTask &task, KPTStandardWorktime *workTime=0, QWidget *parent=0, const char *name=0);

    KMacroCommand *buildCommand(KPTPart *part);

    bool ok();

private:
    KPTTask &m_task;
};

} //KPlato namespace

#endif // KPTTASKGENERALPANEL_H
