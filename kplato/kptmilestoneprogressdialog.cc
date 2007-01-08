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

#include "kptmilestoneprogressdialog.h"
#include "kptmilestoneprogresspanel.h"

#include <kcommand.h>
#include <klocale.h>

#include <kdebug.h>

namespace KPlato
{

MilestoneProgressDialog::MilestoneProgressDialog(Task &task, QWidget *p)
    : KDialog(p)
{
    setCaption( i18n("Milestone Progress") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    m_panel = new MilestoneProgressPanel(task, this);

    setMainWidget(m_panel);

    enableButtonOk(false);

    connect(this, SIGNAL(okClicked()), SLOT(slotOk()));
    connect(m_panel, SIGNAL(changed()), SLOT(slotChanged()));
}

void MilestoneProgressDialog::slotChanged() {
    enableButtonOk(true);
}

KCommand *MilestoneProgressDialog::buildCommand(Part *part) {
    return m_panel->buildCommand(part);;
}

void MilestoneProgressDialog::slotOk() {
    if (!m_panel->ok())
        return;
    accept();
}


}  //KPlato namespace

#include "kptmilestoneprogressdialog.moc"
