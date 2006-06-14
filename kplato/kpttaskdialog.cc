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

#include <klocale.h>
#include <kcommand.h>


#include <kdebug.h>
#include <kvbox.h>

namespace KPlato
{

TaskDialog::TaskDialog(Task &task, Accounts &accounts, StandardWorktime *workTime, bool baseline, QWidget *p)
    : KPageDialog(p)
{
    setCaption( i18n("Task Settings") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    setFaceType( KPageDialog::Tabbed );
    enableButtonSeparator( true );
    KVBox *page;

    // Create all the tabs.
    page =  new KVBox();
    addPage(page, i18n("&General"));
    m_generalTab = new TaskGeneralPanel(task, workTime, baseline, page);

    page =  new KVBox();

    addPage(page, i18n("&Resources"));
    m_resourcesTab = new RequestResourcesPanel(page, task, baseline);
    page =  new KVBox();
    addPage(page, i18n("&Cost"));
    m_costTab = new TaskCostPanel(task, accounts, page);

    // Set the state of all the child widgets.
    enableButtonOK(false);

    connect(m_generalTab, SIGNAL( obligatedFieldsFilled(bool) ), this, SLOT( enableButtonOK(bool) ));
    connect(m_resourcesTab, SIGNAL( changed() ), m_generalTab, SLOT( checkAllFieldsFilled() ));
    connect(m_costTab, SIGNAL( changed() ), m_generalTab, SLOT( checkAllFieldsFilled() ));
}


KCommand *TaskDialog::buildCommand(Part *part) {
    KMacroCommand *m = new KMacroCommand(i18n("Modify Task"));
    bool modified = false;
    KCommand *cmd = m_generalTab->buildCommand(part);
    if (cmd) {
        m->addCommand(cmd);
        modified = true;
    }
    cmd = m_resourcesTab->buildCommand(part);
    if (cmd) {
        m->addCommand(cmd);
        modified = true;
    }
    cmd = m_costTab->buildCommand(part);
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

void TaskDialog::slotOk() {
    if (!m_generalTab->ok())
        return;
    if (!m_resourcesTab->ok())
        return;

    accept();
}


}  //KPlato namespace

#include "kpttaskdialog.moc"
