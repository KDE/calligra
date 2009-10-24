/* This file is part of the KDE project
   Copyright (C) 2003 - 2007 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTREQUESTRESOURCESPANEL_H
#define KPTREQUESTRESOURCESPANEL_H

#include "kplatoui_export.h"

#include "kptduration.h"

#include <QWidget>

namespace KPlato
{

class Task;
class Project;
class ResourceGroup;
class Resource;
class ResourceGroupRequest;
class ResourceRequest;
class MacroCommand;
class ResourceAllocationTreeView;

class RequestResourcesPanel : public QWidget
{
    Q_OBJECT
public:
    RequestResourcesPanel(QWidget *parent, Project &project, Task &task, bool baseline=false);

    MacroCommand *buildCommand();
    
    bool ok();

signals:
    void changed();

private:
    ResourceAllocationTreeView *m_view;
};

}  //KPlato namespace

#endif
