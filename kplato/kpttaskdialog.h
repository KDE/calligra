/* This file is part of the KDE project
   Copyright (C) 2002 Bo Thorsen  bo@suse.de

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

#ifndef KPTTASKDIALOG_H
#define KPTTASKDIALOG_H

#include <kdialogbase.h>
#include <kptresource.h>
#include <qtable.h>

class KPTTaskGeneralPanelBase;
class KPTRequestResourcesPanel;
class KPTTaskNotesPanelBase;
class KPTTask;
class KLineEdit;
class KMacroCommand;
class QTextEdit;
class QDateTimeEdit;
class QSpinBox;
class QButtonGroup;
class QListBox;
class QTable;
class QDateTime;

class KPTTaskDialog : public KDialogBase {
    Q_OBJECT
public:
    KPTTaskDialog(KPTTask &task, QPtrList<KPTResourceGroup> &resourceGroups, QWidget *parent=0, const char *name=0);

    KMacroCommand *buildCommand();
    
protected slots:
    void slotOk();

private:
    KPTTask &m_task;
    KLineEdit *m_name;
    KLineEdit *m_leader;
    QTextEdit *m_description;

    KPTTaskGeneralPanelBase *m_generalTab;
    KPTRequestResourcesPanel *m_resourcesTab;
    KPTTaskNotesPanelBase *m_notesTab;

    // TODO: Duration and risk fields
};

#endif // KPTTASKDIALOG_H
