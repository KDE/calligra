/* This file is part of the KDE project
   Copyright (C) 2002 Bo Thorsen  bo@sonofthor.dk
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kpttaskdialog.h"
#include "kpttaskgeneralpanel.h"
#include "kptrequestresourcespanel.h"

#include <klocale.h>
#include <kcommand.h>

#include <qlayout.h>
#include <kdebug.h>

namespace KPlato
{

KPTTaskDialog::KPTTaskDialog(KPTTask &task, KPTStandardWorktime *workTime, QWidget *p, const char *n)
    : KDialogBase(Tabbed, i18n("Task Settings"), Ok|Cancel, Ok, p, n, true, true)
{
    QFrame *page;
    QVBoxLayout *topLayout;

    // Create all the tabs.
    page = addPage(i18n("&General"));
    topLayout = new QVBoxLayout(page, 0, spacingHint());
    m_generalTab = new KPTTaskGeneralPanel(task, workTime, page);
    topLayout->addWidget(m_generalTab);

    page = addPage(i18n("&Resources"));
    topLayout = new QVBoxLayout(page, 0, spacingHint());
    m_resourcesTab = new KPTRequestResourcesPanel(page, task);
    topLayout->addWidget(m_resourcesTab);

    // Set the state of all the child widgets.
    enableButtonOK(false);
    
    connect(m_generalTab, SIGNAL( obligatedFieldsFilled(bool) ), this, SLOT( enableButtonOK(bool) ));
    connect(m_resourcesTab, SIGNAL( changed() ), m_generalTab, SLOT( checkAllFieldsFilled() ));
}


KMacroCommand *KPTTaskDialog::buildCommand(KPTPart *part) {
    KMacroCommand *m = new KMacroCommand(i18n("Modify Task"));
    bool modified = false;
    KCommand *cmd = m_generalTab->buildCommand(part);
    if (cmd) {
        m->addCommand(cmd);
        modified = true;
    }
    cmd = m_resourcesTab->buildCommand(part);
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

void KPTTaskDialog::slotOk() {
    if (!m_generalTab->ok())
        return;
    if (!m_resourcesTab->ok())
        return;
    accept();
}


}  //KPlato namespace

#include "kpttaskdialog.moc"
