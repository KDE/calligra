/* This file is part of the KDE project
   Copyright (C) 2003, 2004 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qvbox.h>

#include <klocale.h>
#include <kcommand.h>

#include <kdebug.h>

#include "kptmainprojectdialog.h"
#include "kptproject.h"
#include "kptmainprojectpanel.h"
#include "kptresourcespanel.h"
#include "kptresource.h"

namespace KPlato
{

KPTMainProjectDialog::KPTMainProjectDialog(KPTProject &p, QWidget *parent, const char *name)
    : KDialogBase( Tabbed, i18n("Project Settings"), Ok|Cancel, Ok, parent, name, true, true),
      project(p)
{
    QVBox *page = addVBoxPage(i18n("General"));    
    generalTab = new KPTMainProjectPanel(project, page);
    
    page = addVBoxPage(i18n("Resources"));
    resourcesTab = new KPTResourcesPanel(page, &project);
    
    setMainWidget(generalTab);
    enableButtonOK(false);

    connect(generalTab, SIGNAL(obligatedFieldsFilled(bool)), SLOT(enableButtonOK(bool)));

    connect(resourcesTab, SIGNAL(changed()), generalTab, SLOT(slotCheckAllFieldsFilled()));
}


void KPTMainProjectDialog::slotOk() {
    if (!generalTab->ok())
        return;
    if (!resourcesTab->ok())
        return;

    accept();
}

KCommand *KPTMainProjectDialog::buildCommand(KPTPart *part) {
    KMacroCommand *m = 0;
    QString c = i18n("Modify main project");
    KCommand *cmd = generalTab->buildCommand(part);
    if (cmd) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(cmd);
    }
    cmd = resourcesTab->buildCommand(part);
    if (cmd) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(cmd);
    }
    return m;
}

}  //KPlato namespace

#include "kptmainprojectdialog.moc"
