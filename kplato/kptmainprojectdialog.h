/* This file is part of the KDE project
   Copyright (C) 2003, 2004 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTMAINPROJECTDIALOG_H
#define KPTMAINPROJECTDIALOG_H

#include "kptmainprojectdialogbase.h"

#include <kdialogbase.h>

class KCommand;

namespace KPlato
{

class KPTProject;
class KPTResourcesPanel;
class KPTPart;

class KPTMainProjectDialogImpl : public KPTMainProjectDialogBase {
    Q_OBJECT
public:
    KPTMainProjectDialogImpl (QWidget *parent);

private slots:
    void slotCheckAllFieldsFilled();
	void slotChooseLeader();
    void slotStartDateClicked();
    void slotEndDateClicked();

signals:
    void obligatedFieldsFilled(bool yes);
};

class KPTMainProjectDialog : public KDialogBase {
    Q_OBJECT
public:
    KPTMainProjectDialog(KPTProject &project, QWidget *parent=0, const char *name=0);

    KCommand *buildCommand(KPTPart *part);
    
protected slots:
    void slotOk();

private:
    KPTProject &project;
    KPTMainProjectDialogImpl *dia;
    KPTResourcesPanel *resourcesTab;
};

}  //KPlato namespace

#endif // KPTMAINPROJECTDIALOG_H
