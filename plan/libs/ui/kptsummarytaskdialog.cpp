/* This file is part of the KDE project
   Copyright (C) 2002 Bo Thorsen  bo@sonofthor.dk
   Copyright (C) 2004 - 2010 Dag Andersen <danders@get2net.dk>

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

#include "kptsummarytaskdialog.h"
#include "kptsummarytaskgeneralpanel.h"
#include "kptcommand.h"
#include "kptnode.h"
#include "kpttask.h"
#include "kptproject.h"

#include <klocale.h>

#include <kdebug.h>

namespace KPlato
{

SummaryTaskDialog::SummaryTaskDialog(Task &task, QWidget *p)
    : KDialog(p),
    m_node( &task )
{
    setCaption( i18n("Summary Task Settings") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    m_generalTab = new SummaryTaskGeneralPanel(task, this);
    setMainWidget(m_generalTab);
    enableButtonOk(false);

    connect(m_generalTab, SIGNAL(obligatedFieldsFilled(bool)), SLOT(enableButtonOk(bool)));

    Project *proj = static_cast<Project*>( task.projectNode() );
    if ( proj ) {
        connect(proj, SIGNAL(nodeRemoved(Node*)), this, SLOT(slotTaskRemoved(Node*)));
    }
}

void SummaryTaskDialog::slotTaskRemoved( Node *node )
{
    if ( node == m_node ) {
        reject();
    }
}


MacroCommand *SummaryTaskDialog::buildCommand() {
    MacroCommand *m = new MacroCommand(i18nc("(qtundo-format)", "Modify Summary Task"));
    bool modified = false;
    MacroCommand *cmd = m_generalTab->buildCommand();
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

void SummaryTaskDialog::slotButtonClicked(int button) {
    if (button == KDialog::Ok) {
        if (!m_generalTab->ok())
            return;
        accept();
    } else {
        KDialog::slotButtonClicked(button);
    }
}


}  //KPlato namespace

#include "kptsummarytaskdialog.moc"
