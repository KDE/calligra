/* This file is part of the KDE project
   Copyright (C) 2002 Bo Thorsen  bo@sonofthor.dk

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KPTPROJECTDIALOG_H
#define KPTPROJECTDIALOG_H

#include "kptresource.h"
#include "kptprojectdialogbase.h"
//#include "kptresourcespanel.h"

#include <kdialogbase.h>

#include <qstring.h>

class KPTProject;
class KPTProjectDialogImpl;
class KPTResourcesPanel;

class KPTProjectDialogImpl : public KPTProjectDialogBase {
    Q_OBJECT
public:
    KPTProjectDialogImpl (QWidget *parent);

private slots:
    void slotCheckAllFieldsFilled();
    void slotSchedulingChanged(int activated);
	void slotChooseLeader();

signals:
    void obligatedFieldsFilled(bool yes);
    void schedulingTypeChanged(int activated);
};

class KPTProjectDialog : public KDialogBase {
    Q_OBJECT
public:
    KPTProjectDialog(KPTProject &project, QWidget *parent=0,
		     const char *name=0);


protected slots:
    void slotOk();
    void slotSchedulingChanged(int activated);

private:
    KPTProject &project;
    KPTProjectDialogImpl *dia;
    KPTResourcesPanel *resourcesTab;
};

#endif // KPTPROJECTDIALOG_H
