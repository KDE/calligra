/* This file is part of the KDE project
  Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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
  Boston, MA 02110-1301, USA.
*/

#ifndef KPTSCHEDULERPLUGIN_H
#define KPTSCHEDULERPLUGIN_H

#include "kplatokernel_export.h"

#include <QObject>
#include <QString>

namespace KPlato
{

class Project;
class ScheduleManager;
 
class KPLATOKERNEL_EXPORT SchedulerPlugin : public QObject
{
    Q_OBJECT
public:
    SchedulerPlugin(QObject *parent);
    virtual ~SchedulerPlugin();

    /// Calculate the project
    virtual void calculate( Project &project, ScheduleManager *sm, bool nothread = false ) = 0;

    /// Localized name
    QString name() const;
    /// Name is normally set by the plugin loader, from Name in the desktop file
    void setName( const QString &name );
    /// Localized comment
    QString comment() const;
    /// Comment is normally set by the plugin loader, from Comment in the desktop file
    void setComment( const QString &name );

private:
    class Private;
    Private *d;
};

} //namespace KPlato

#endif
