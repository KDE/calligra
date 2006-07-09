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

#include <qvbox.h>

#include <klocale.h>
#include <kcommand.h>

#include <kdebug.h>

#include "kptresourcesdialog.h"
#include "kptproject.h"
#include "kptresourcespanel.h"
#include "kptresource.h"

namespace KPlato
{

ResourcesDialog::ResourcesDialog(Project &p, QWidget *parent, const char *name)
    : KDialogBase( Swallow, i18n("Resources"), Ok|Cancel, Ok, parent, name, true, true),
      project(p)
{
    panel = new ResourcesPanel(this, &project);
    
    setMainWidget(panel);
    enableButtonOK(false);

    connect(panel, SIGNAL(changed()), SLOT(slotChanged()));
}


void ResourcesDialog::slotChanged() {
    enableButtonOK(true);
}

void ResourcesDialog::slotOk() {
    if (!panel->ok())
        return;
    accept();
}

KCommand *ResourcesDialog::buildCommand(Part *part) {
    KMacroCommand *m = 0;
    QString c = i18n("Modify resources");
    KCommand *cmd = panel->buildCommand(part);
    if (cmd) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(cmd);
    }
    return m;
}

}  //KPlato namespace

#include "kptresourcesdialog.moc"
