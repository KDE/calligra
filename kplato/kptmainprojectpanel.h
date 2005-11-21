/* This file is part of the KDE project
   Copyright (C) 2004, 2005 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTMAINPROJECTPANEL_H
#define KPTMAINPROJECTPANEL_H

#include "kptmainprojectpanelbase.h"

#include <qdatetime.h>

class QWidget;

class KCommand;

namespace KPlato
{

class Project;
class Part;

class MainProjectPanelImpl : public MainProjectPanelBase {
    Q_OBJECT
public:
    MainProjectPanelImpl(QWidget *parent=0, const char *name=0);

    virtual QDateTime startDateTime();
    virtual QDateTime endDateTime();

public slots:
    virtual void slotCheckAllFieldsFilled();
    virtual void slotChooseLeader();
    virtual void slotStartDateClicked();
    virtual void slotEndDateClicked();
    virtual void enableDateTime();
    virtual void slotBaseline();

signals:
    void obligatedFieldsFilled(bool);
    void changed();

};

class MainProjectPanel : public MainProjectPanelImpl {
    Q_OBJECT
public:
    MainProjectPanel(Project &project, QWidget *parent=0, const char *name=0);

    KCommand *buildCommand(Part *part);
    
    bool ok();

private:
    Project &project;
};

}  //KPlato namespace

#endif // MAINPROJECTPANEL_H
