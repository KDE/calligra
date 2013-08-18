/* This file is part of the KDE project
   Copyright (C) 2002 Bo Thorsen  bo@sonofthor.dk
   Copyright (C) 2004 -2010 Dag Andersen <danders@get2net.dk>

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

#include "kpttaskdialog.h"
#include "kpttaskcostpanel.h"
#include "kpttaskgeneralpanel.h"
#include "kptrequestresourcespanel.h"
#include "kptdocumentspanel.h"
#include "kpttaskdescriptiondialog.h"
#include "kptcommand.h"
#include "kptnode.h"
#include "kpttask.h"
#include "kptproject.h"

#include <klocale.h>

#include <kvbox.h>
#include <kdebug.h>

namespace KPlato
{

TaskDialog::TaskDialog( Project &project, Task &task, Accounts &accounts, QWidget *p)
    : KoPageDialog(p),
    m_project( project ),
    m_node( &task )
{
    setCaption( i18n("Task Settings") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    setFaceType( KoPageDialog::Tabbed );
    showButtonSeparator( true );
    KVBox *page;

    // Create all the tabs.
    page =  new KVBox();
    addPage(page, i18n("&General"));
    m_generalTab = new TaskGeneralPanel(project, task, page);

    page =  new KVBox();
    addPage(page, i18n("&Resources"));
    m_resourcesTab = new RequestResourcesPanel(page, project, task);

    page =  new KVBox();
    addPage(page, i18n("&Documents"));
    m_documentsTab = new DocumentsPanel( task, page );

    page =  new KVBox();
    addPage(page, i18n("&Cost"));
    m_costTab = new TaskCostPanel(task, accounts, page);

    page =  new KVBox();
    addPage(page, i18n("D&escription"));
    m_descriptionTab = new TaskDescriptionPanel(task, page);
    m_descriptionTab->namefield->hide();
    m_descriptionTab->namelabel->hide();

    enableButtonOk(false);

    connect(this, SIGNAL(currentPageChanged(KFakePageWidgetItem*,KFakePageWidgetItem*)), SLOT(slotCurrentChanged(KFakePageWidgetItem*,KFakePageWidgetItem*)));

    connect(m_generalTab, SIGNAL(obligatedFieldsFilled(bool)), this, SLOT(enableButtonOk(bool)));
    connect(m_resourcesTab, SIGNAL(changed()), m_generalTab, SLOT(checkAllFieldsFilled()));
    connect(m_documentsTab, SIGNAL(changed()), m_generalTab, SLOT(checkAllFieldsFilled()));
    connect(m_costTab, SIGNAL(changed()), m_generalTab, SLOT(checkAllFieldsFilled()));
    connect(m_descriptionTab, SIGNAL(textChanged(bool)), m_generalTab, SLOT(checkAllFieldsFilled()));

    connect(&project, SIGNAL(nodeRemoved(Node*)), this, SLOT(slotTaskRemoved(Node*)));
}

void TaskDialog::slotCurrentChanged( KFakePageWidgetItem *current, KFakePageWidgetItem */*prev*/ )
{
    //kDebug(planDbg())<<current->widget()<<m_descriptionTab->parent();
    // HACK: KoPageDialog grabs focus when a tab is clicked.
    // KRichTextWidget still flashes the caret so the user thinks it has the focus.
    // For now, just give the KRichTextWidget focus.
    if ( current->widget() == m_descriptionTab->parent() ) {
        m_descriptionTab->descriptionfield->setFocus();
    }
}

void TaskDialog::slotTaskRemoved( Node *node )
{
    if ( node == m_node ) {
        reject();
    }
}

MacroCommand *TaskDialog::buildCommand() {
    MacroCommand *m = new MacroCommand(i18nc("(qtundo-format)", "Modify task"));
    bool modified = false;
    MacroCommand *cmd = m_generalTab->buildCommand();
    if (cmd) {
        m->addCommand(cmd);
        modified = true;
    }
    cmd = m_resourcesTab->buildCommand();
    if (cmd) {
        m->addCommand(cmd);
        modified = true;
    }
    cmd = m_documentsTab->buildCommand();
    if (cmd) {
        m->addCommand(cmd);
        modified = true;
    }
    cmd = m_costTab->buildCommand();
    if (cmd) {
        m->addCommand(cmd);
        modified = true;
    }
    cmd = m_descriptionTab->buildCommand();
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

void TaskDialog::slotButtonClicked(int button) {
    if (button == KDialog::Ok) {
        if (!m_generalTab->ok())
            return;
        if (!m_resourcesTab->ok())
            return;
        if (!m_descriptionTab->ok())
            return;
        accept();
    } else {
        KDialog::slotButtonClicked(button);
    }
}

//---------------------------
TaskAddDialog::TaskAddDialog(Project &project, Task &task, Node *currentNode, Accounts &accounts, QWidget *p)
    : TaskDialog(project, task, accounts, p)
{
    m_currentnode = currentNode;
    // do not know wbs code yet
    m_generalTab->hideWbs();
    
    connect(&project, SIGNAL(nodeRemoved(Node*)), SLOT(slotNodeRemoved(Node*)));
}

TaskAddDialog::~TaskAddDialog()
{
    delete m_node; // in case of cancel
}

void TaskAddDialog::slotNodeRemoved( Node *node )
{
    if ( m_currentnode == node ) {
        reject();
    }
}

MacroCommand *TaskAddDialog::buildCommand()
{
    MacroCommand *c = new MacroCommand( i18nc( "(qtundo-format)", "Add task" ) );
    c->addCommand( new TaskAddCmd( &m_project, m_node, m_currentnode ) );
    MacroCommand *m = TaskDialog::buildCommand();
    if ( m ) {
        c->addCommand( m );
    }
    m_node = 0; // don't delete task
    return c;
}

//---------------------------
SubTaskAddDialog::SubTaskAddDialog(Project &project, Task &task, Node *currentNode, Accounts &accounts, QWidget *p)
    : TaskDialog(project, task, accounts, p)
{
    m_currentnode = currentNode;
    // do not know wbs code yet
    m_generalTab->hideWbs();

    connect(&project, SIGNAL(nodeRemoved(Node*)), SLOT(slotNodeRemoved(Node*)));
}

SubTaskAddDialog::~SubTaskAddDialog()
{
    delete m_node; // in case of cancel
}

void SubTaskAddDialog::slotNodeRemoved( Node *node )
{
    if ( m_currentnode == node ) {
        reject();
    }
}

MacroCommand *SubTaskAddDialog::buildCommand()
{
    QString s = i18n( "Add sub-task" );
    if ( m_currentnode == 0 ) {
        s = i18n( "Add task" ); // it will be added to project
    }
    MacroCommand *c = new MacroCommand( s );
    c->addCommand( new SubtaskAddCmd( &m_project, m_node, m_currentnode ) );
    MacroCommand *m = TaskDialog::buildCommand();
    if ( m ) {
        c->addCommand( m );
    }
    m_node = 0; // don't delete task
    return c;
}

}  //KPlato namespace

#include "kpttaskdialog.moc"
