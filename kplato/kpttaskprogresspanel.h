/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KPTTASKPROGRESSPANEL_H
#define KPTTASKPROGRESSPANEL_H

#include "kpttaskprogresspanelbase.h"
#include "kpttask.h"

class KCommand;

namespace KPlato
{

class KPTPart;


class KPTTaskProgressPanel : public KPTTaskProgressPanelBase {
    Q_OBJECT
public:
    KPTTaskProgressPanel(KPTTask &task, QWidget *parent=0, const char *name=0);

    KCommand *buildCommand(KPTPart *part);
    
    bool ok();

private:
    KPTTask &m_task;
    struct KPTTask::Progress m_progress;
};

}  //KPlato namespace

#endif // KPTTASKPROGRESSPANEL_H
