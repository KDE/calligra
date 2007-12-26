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

#ifndef KPTWORKPACKAGEVIEW_H
#define KPTWORKPACKAGEVIEW_H

#include "kplatoui_export.h"

#include <kptviewbase.h>

#include <QTableView>

class KoDocument;

class QPoint;

class KTextBrowser;

namespace KPlato
{

class Project;
class Task;
class Resource;
class ScheduleManager;
class WorkPackageModel;

class KPLATOUI_EXPORT WorkPackageTableView : public QTableView
{
    Q_OBJECT
public:
    WorkPackageTableView( QWidget *parent );

    WorkPackageModel *itemModel() const;

    Project *project() const;
    void setProject( Project *project );
    Task *task() const;
    void setTask( Task *task );

    Resource *currentResource() const;
    QList<Resource*> selectedResources() const;

};

class KPLATOUI_EXPORT WorkPackageInfoView : public ViewBase
{
    Q_OBJECT
public:
    WorkPackageInfoView( KoDocument *doc, QWidget *parent );

    Project *project() const;
    void setProject( Project *project );
    Task *task() const;
    void setTask( Task *task );
    ScheduleManager *scheduleManager() const;
    void setScheduleManager( ScheduleManager *sm );
    using ViewBase::draw;
    void draw();

private:
    KTextBrowser *m_view;
    Project *m_project;
    Task *m_task;
    ScheduleManager *m_manager;
};


}  //KPlato namespace

#endif
