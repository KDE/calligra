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
 * Boston, MA 02110-1301, USA.
*/

#include "kptwbsdefinitiondialog.h"
#include "kptwbsdefinitionpanel.h"
#include "kptwbsdefinition.h"

#include <klocale.h>
#include <kcommand.h>

#include <kdebug.h>

namespace KPlato
{

WBSDefinitionDialog::WBSDefinitionDialog(WBSDefinition &def, QWidget *p, const char *n)
    : KDialogBase(Swallow, i18n("WBS Definition"), Ok|Cancel, Ok, p, n, true, true)
{
    
    m_panel = new WBSDefinitionPanel(def, this);
    setMainWidget(m_panel);
    
    enableButtonOK(false);
    connect(m_panel, SIGNAL(changed(bool)), SLOT(enableButtonOK(bool)));
}


KMacroCommand *WBSDefinitionDialog::buildCommand(Part *part) {
    KMacroCommand *m = new KMacroCommand(i18n("Modify WBS Definition"));
    bool modified = false;
    KCommand *cmd = m_panel->buildCommand(part);
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

void WBSDefinitionDialog::slotOk() {
    if (!m_panel->ok())
        return;
    accept();
}


}  //KPlato namespace

#include "kptwbsdefinitiondialog.moc"
