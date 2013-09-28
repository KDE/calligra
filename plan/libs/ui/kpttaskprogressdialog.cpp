/* This file is part of the KDE project
   Copyright (C) 2005-2010 Dag Andersen <danders@get2net.dk>

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

#include "kpttaskprogressdialog.h"
#include "kpttaskprogresspanel.h"
#include "kptcommand.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptnode.h"

#include <klocale.h>

#include <kdebug.h>

namespace KPlato
{

TaskProgressDialog::TaskProgressDialog(Task &task, ScheduleManager *sm, StandardWorktime *workTime, QWidget *p)
    : KDialog( p),
    m_node( &task )
{
    setCaption( i18n("Task Progress") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    m_panel = new TaskProgressPanel(task, sm, workTime, this);

    setMainWidget(m_panel);

    enableButtonOk(false);

    connect(m_panel, SIGNAL(changed()), SLOT(slotChanged()));
    Project *proj = static_cast<Project*>( task.projectNode() );
    if ( proj ) {
        connect(proj, SIGNAL(nodeRemoved(Node*)), SLOT(slotNodeRemoved(Node*)));
    }
}

void TaskProgressDialog::slotNodeRemoved( Node *node )
{
    if ( m_node == node ) {
        reject();
    }
}

void TaskProgressDialog::slotChanged() {
    enableButtonOk(true);
}

MacroCommand *TaskProgressDialog::buildCommand() {
    MacroCommand *m = new MacroCommand(i18nc("(qtundo-format)", "Modify Task Progress"));
    bool modified = false;
    MacroCommand *cmd = m_panel->buildCommand();
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


}  //KPlato namespace

#include "kpttaskprogressdialog.moc"
