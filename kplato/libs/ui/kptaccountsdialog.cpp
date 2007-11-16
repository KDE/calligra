/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

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

#include "kptaccountsdialog.h"

#include "kptaccountspanel.h"

#include <klocale.h>
#include <kdebug.h>

namespace KPlato
{

AccountsDialog::AccountsDialog(Project &project, Accounts &acc, QWidget *p)
    : KDialog(p)
{
    setCaption( i18n("Edit Accounts") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    m_panel = new AccountsPanel(project, acc, this);
    setMainWidget(m_panel);

    enableButtonOk(false);
    connect(m_panel, SIGNAL(changed(bool)), SLOT(enableButtonOk(bool)));
    connect(this,SIGNAL(okClicked()),this,SLOT(slotOk()));
}


MacroCommand *AccountsDialog::buildCommand() {
    return m_panel->buildCommand();
}

void AccountsDialog::slotOk() {
    kDebug()<<"Dialog : slotok : appel panel";
    m_panel->slotOk();
    accept();
}

} //namespace KPlato

#include "kptaccountsdialog.moc"
