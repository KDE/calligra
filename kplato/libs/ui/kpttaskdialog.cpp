/* This file is part of the KDE project
   Copyright (C) 2002 Bo Thorsen  bo@sonofthor.dk
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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

#include "kpttaskdialog.h"
#include "kpttaskcostpanel.h"
#include "kpttaskgeneralpanel.h"
#include "kptrequestresourcespanel.h"
#include "kptdocumentspanel.h"
#include "kpttaskdescriptiondialog.h"
#include "kptcommand.h"

#include <klocale.h>

#include <kvbox.h>
#include <kdebug.h>

namespace KPlato
{

TaskDialog::TaskDialog( Project &project, Task &task, Accounts &accounts, QWidget *p)
    : KPageDialog(p)
{
    setCaption( i18n("Task Settings") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    setFaceType( KPageDialog::Tabbed );
    showButtonSeparator( true );
    KVBox *page;

    // Create all the tabs.
    page =  new KVBox();
    addPage(page, i18n("&General"));
    m_generalTab = new TaskGeneralPanel(project, task, page);

    page =  new KVBox();
    addPage(page, i18n("&Resources"));
    m_resourcesTab = new RequestResourcesPanel(page, project, task);
    
    page =  new KVBox();
    addPage(page, i18n("&Documents"));
    m_documentsTab = new DocumentsPanel( task, page );
    
    page =  new KVBox();
    addPage(page, i18n("&Cost"));
    m_costTab = new TaskCostPanel(task, accounts, page);

    page =  new KVBox();
    addPage(page, i18n("D&escription"));
    m_descriptionTab = new TaskDescriptionPanel(task, page);
    m_descriptionTab->namefield->hide();
    m_descriptionTab->namelabel->hide();

    enableButtonOk(false);

    connect(m_generalTab, SIGNAL( obligatedFieldsFilled(bool) ), this, SLOT( enableButtonOk(bool) ));
    connect(m_resourcesTab, SIGNAL( changed() ), m_generalTab, SLOT( checkAllFieldsFilled() ));
    connect(m_documentsTab, SIGNAL( changed() ), m_generalTab, SLOT( checkAllFieldsFilled() ));
    connect(m_costTab, SIGNAL( changed() ), m_generalTab, SLOT( checkAllFieldsFilled() ));
    connect(m_descriptionTab, SIGNAL( textChanged(bool) ), m_generalTab, SLOT( checkAllFieldsFilled() ));

}

MacroCommand *TaskDialog::buildCommand() {
    MacroCommand *m = new MacroCommand(i18n("Modify Task"));
    bool modified = false;
    MacroCommand *cmd = m_generalTab->buildCommand();
    if (cmd) {
        m->addCommand(cmd);
        modified = true;
    }
    cmd = m_resourcesTab->buildCommand();
    if (cmd) {
        m->addCommand(cmd);
        modified = true;
    }
    cmd = m_documentsTab->buildCommand();
    if (cmd) {
        m->addCommand(cmd);
        modified = true;
    }
    cmd = m_costTab->buildCommand();
    if (cmd) {
        m->addCommand(cmd);
        modified = true;
    }
    cmd = m_descriptionTab->buildCommand();
    if (cmd) {
        m->addCommand(cmd);
        modified = true;
    }
    if (!modified) {
        delete m;
        return 0;
    }
    return m;
}

void TaskDialog::slotButtonClicked(int button) {
    if (button == KDialog::Ok) {
        if (!m_generalTab->ok())
            return;
        if (!m_resourcesTab->ok())
            return;
        if (!m_descriptionTab->ok())
            return;
        accept();
    } else {
        KDialog::slotButtonClicked(button);
    }
}

//---------------------------
TaskAddDialog::TaskAddDialog(Project &project, Task &task, Accounts &accounts, QWidget *p)
    : TaskDialog(project, task, accounts, p)
{
    // do not know wbs code yet
    m_generalTab->hideWbs();
}

}  //KPlato namespace

#include "kpttaskdialog.moc"
