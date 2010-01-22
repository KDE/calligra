/* This file is part of the KDE project
 * Copyright (C) 2009 Dag Andersen <danders@get2net.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KPTSCHEDULESDOCKER_H
#define KPTSCHEDULESDOCKER_H

#include <QDockWidget>
#include <KoDockFactoryBase.h>

#include "kptschedulemodel.h"

#include <QSortFilterProxyModel>

class QItemSelection;
class QTreeView;

namespace KPlato
{

class Project;
class ScheduleManager;

class SchedulesDocker : public QDockWidget
{
    Q_OBJECT
public:
    explicit SchedulesDocker();
    ~SchedulesDocker();

    ScheduleManager *selectedSchedule() const;

signals:
    void selectionChanged( ScheduleManager *sm );

public slots:
    void setProject( Project *project );
    void setSelectedSchedule( ScheduleManager *sm );

protected slots:
    void slotSelectionChanged();

private:
    QTreeView *m_view;
    QSortFilterProxyModel m_sfModel;
    ScheduleItemModel m_model;
};

class SchedulesDockerFactory : public KoDockFactoryBase
{
public:
    SchedulesDockerFactory();

    virtual QString id() const;
    virtual QDockWidget* createDockWidget();
    /// @return the dock widget area the widget should appear in by default
    virtual KoDockFactoryBase::DockPosition defaultDockPosition() const { return DockLeft; }

};

} //namespace KPlato

#endif
