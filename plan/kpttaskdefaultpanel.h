/* This file is part of the KDE project
   Copyright (C) 2004-2007 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTTASKDEFAULTPANEL_H
#define KPTTASKDEFAULTPANEL_H

#include "kplato_export.h"

#include "ui_kptconfigtaskpanelbase.h"


namespace KPlato
{

class DateTime;
class Task;
class StandardWorktime;
class MacroCommand;


class ConfigTaskPanelImpl : public QWidget, public Ui_ConfigTaskPanelBase
{
    Q_OBJECT
public:
    ConfigTaskPanelImpl( QWidget *parent );
    
    void initDescription();

public slots:
    virtual void changeLeader();
    void startDateTimeChanged( const QDateTime& );
    void endDateTimeChanged( const QDateTime& );
    void unitChanged( int unit );
    void currentUnitChanged( int );
};

class KPLATO_EXPORT TaskDefaultPanel : public ConfigTaskPanelImpl
{
    Q_OBJECT
public:
    explicit TaskDefaultPanel( QWidget *parent=0 );
    
};

} //KPlato namespace

#endif // TASKDEFAULTPANEL_H
