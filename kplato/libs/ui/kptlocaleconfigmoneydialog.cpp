/* This file is part of the KDE project
   Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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

#include "kptlocaleconfigmoneydialog.h"
#include "locale/localemon.h"

#include "kptcommand.h"

#include <klocale.h>

#include <kdebug.h>

namespace KPlato
{

LocaleConfigMoneyDialog::LocaleConfigMoneyDialog( KLocale *locale, QWidget *p)
    : KDialog( p)
{
    setCaption( i18n("Currency Settings") );
    setButtons( Ok|Cancel );
    showButtonSeparator( true );
    m_panel = new LocaleConfigMoney( locale, this);

    setMainWidget(m_panel);

    enableButtonOk(false);

    connect(m_panel, SIGNAL( localeChanged() ), SLOT(slotChanged()));
}

void LocaleConfigMoneyDialog::slotChanged() {
    enableButtonOk(true);
}

QUndoCommand *LocaleConfigMoneyDialog::buildCommand( Project &project ) {
    MacroCommand *m = new ModifyProjectLocaleCmd( project, i18n( "Modify currency settings" ) );
    MacroCommand *cmd = m_panel->buildCommand();
    if (cmd) {
        m->addCommand(cmd);
    }
    if ( m->isEmpty() ) {
        delete m;
        return 0;
    }
    return m;
}


}  //KPlato namespace

#include "kptlocaleconfigmoneydialog.moc"
