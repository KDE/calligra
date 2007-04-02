/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

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

#include <k3command.h>
#include <klocale.h>

#include <kdebug.h>

namespace KPlato
{

TaskProgressDialog::TaskProgressDialog(Task &task, StandardWorktime *workTime, QWidget *p)
    : KDialog( p)
{
    setCaption( i18n("Task Progress") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    m_panel = new TaskProgressPanel(task, workTime, this);

    setMainWidget(m_panel);

    enableButtonOk(false);

    connect(m_panel, SIGNAL( changed() ), SLOT(slotChanged()));
    connect(this,SIGNAL(okClicked()),this,SLOT(slotOk()));
}

void TaskProgressDialog::slotChanged() {
    enableButtonOk(true);
}

K3Command *TaskProgressDialog::buildCommand(Part *part) {
    K3MacroCommand *m = new K3MacroCommand(i18n("Modify Task Progress"));
    bool modified = false;
    K3Command *cmd = m_panel->buildCommand(part);
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

void TaskProgressDialog::slotOk() {
    if (!m_panel->ok())
        return;
    accept();
}


}  //KPlato namespace

#include "kpttaskprogressdialog.moc"
