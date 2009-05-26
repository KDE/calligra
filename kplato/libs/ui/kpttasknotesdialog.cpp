/* This file is part of the KDE project
   Copyright (C) 2009 Dag Andersen <koffice-devel@kde.org

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

#include "kpttasknotesdialog.h"
#include "kpttask.h"
#include "kptcommand.h"

#include <KTextEdit>
#include <KLocale>
#include <KTextEdit>
#include <kdebug.h>

namespace KPlato
{

TaskNotesPanel::TaskNotesPanel(Task &task, QWidget *p )
    : TaskNotesPanelImpl( task, p )
{
    setStartValues( task );
    descriptionfield->setFocus();
}

void TaskNotesPanel::setStartValues( Task &task )
{
    namefield->setText(task.name());
    descriptionfield->setText(task.description());
}

MacroCommand *TaskNotesPanel::buildCommand()
{
    MacroCommand *cmd = new MacroCommand(i18n("Modify Task Note"));
    bool modified = false;

    if ( m_task.description() != descriptionfield->text() ) {
        cmd->addCommand(new NodeModifyDescriptionCmd(m_task, descriptionfield->text()));
        modified = true;
    }
    if (!modified) {
        delete cmd;
        return 0;
    }
    return cmd;
}

bool TaskNotesPanel::ok() {
    return true;
}

//-----------------------------
TaskNotesPanelImpl::TaskNotesPanelImpl( Task &task, QWidget *p )
    : QWidget(p),
      m_task(task)
{

    setupUi(this);

    connect( descriptionfield, SIGNAL( textChanged() ), SLOT( slotChanged() ) );
}

void TaskNotesPanelImpl::slotChanged()
{
    emit textChanged( descriptionfield->text() != m_task.description() );
}

//-----------------------------
TaskNotesDialog::TaskNotesDialog( Task &task, QWidget *p )
    : KDialog(p)
{
    setCaption( i18n( "Task Notes" ) );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );

    m_notesTab = new TaskNotesPanel( task, this );
    setMainWidget(m_notesTab);

    enableButtonOk(false);

    connect( m_notesTab, SIGNAL( textChanged( bool ) ), this, SLOT( enableButtonOk(bool) ) );
}

MacroCommand *TaskNotesDialog::buildCommand()
{
    MacroCommand *m = new MacroCommand(i18n("Modify Task Notes"));
    bool modified = false;
    MacroCommand *cmd = m_notesTab->buildCommand();
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

void TaskNotesDialog::slotButtonClicked( int button )
{
    if (button == KDialog::Ok) {
        if ( ! m_notesTab->ok() ) {
            return;
        }
        accept();
    } else {
        KDialog::slotButtonClicked( button );
    }
}


}  //KPlato namespace

#include "kpttasknotesdialog.moc"
