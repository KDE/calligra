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

#ifndef KPTMAINPROJECTPANEL_H
#define KPTMAINPROJECTPANEL_H

#include "kplatoui_export.h"

#include "ui_kptmainprojectpanelbase.h"

#include "kptdatetime.h"

#include <QWidget>


namespace KPlato
{

class Project;
class MacroCommand;
class TaskDescriptionPanel;

class MainProjectPanel : public QWidget, public Ui_MainProjectPanelBase {
    Q_OBJECT
public:
    explicit MainProjectPanel(Project &project, QWidget *parent=0);

    virtual QDateTime startDateTime();
    virtual QDateTime endDateTime();

    MacroCommand *buildCommand();
    
    bool ok();

public slots:
    virtual void slotCheckAllFieldsFilled();
    virtual void slotChooseLeader();
    virtual void slotStartDateClicked();
    virtual void slotEndDateClicked();
    virtual void enableDateTime();

signals:
    void obligatedFieldsFilled(bool);
    void changed();

private:
    TaskDescriptionPanel *m_description;
    Project &project;
};


}  //KPlato namespace

#endif // MAINPROJECTPANEL_H
