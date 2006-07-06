/* This file is part of the KDE project
   Copyright (C) 2002 Bo Thorsen  bo@sonofthor.dk
   Copyright (C) 2004, 2006 Dag Andersen <danders@get2net.dk>

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

#include <klocale.h>
#include <kcommand.h>

#include <q3vbox.h>
#include <kdebug.h>

namespace KPlato
{

SummaryTaskDialog::SummaryTaskDialog(Task &task, QWidget *p)
    : KDialog(p)
{
    setCaption( i18n("Summary Task Settings") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    m_generalTab = new SummaryTaskGeneralPanel(task, this);
    setMainWidget(m_generalTab);
    enableButtonOk(false);

    connect(m_generalTab, SIGNAL(obligatedFieldsFilled(bool)), SLOT(enableButtonOk(bool)));
}


KCommand *SummaryTaskDialog::buildCommand(Part *part) {
    KMacroCommand *m = new KMacroCommand(i18n("Modify Summary Task"));
    bool modified = false;
    KCommand *cmd = m_generalTab->buildCommand(part);
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

void SummaryTaskDialog::slotOk() {
    if (!m_generalTab->ok())
        return;

    accept();
}


}  //KPlato namespace

#include "kptsummarytaskdialog.moc"
