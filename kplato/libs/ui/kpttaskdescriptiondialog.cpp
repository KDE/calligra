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

#include "kpttaskdescriptiondialog.h"
#include "kpttask.h"
#include "kptcommand.h"

#include <KTextEdit>
#include <KLocale>
#include <KTextEdit>
#include <kdebug.h>

namespace KPlato
{

TaskDescriptionPanel::TaskDescriptionPanel(Task &task, QWidget *p )
    : TaskDescriptionPanelImpl( task, p )
{
    setStartValues( task );
    descriptionfield->setFocus();
}

void TaskDescriptionPanel::setStartValues( Task &task )
{
    namefield->setText(task.name());
    descriptionfield->setText(task.description());
}

MacroCommand *TaskDescriptionPanel::buildCommand()
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

bool TaskDescriptionPanel::ok() {
    return true;
}

//-----------------------------
TaskDescriptionPanelImpl::TaskDescriptionPanelImpl( Task &task, QWidget *p )
    : QWidget(p),
      m_task(task)
{

    setupUi(this);

    connect( descriptionfield, SIGNAL( textChanged() ), SLOT( slotChanged() ) );
}

void TaskDescriptionPanelImpl::slotChanged()
{
    emit textChanged( descriptionfield->text() != m_task.description() );
}

//-----------------------------
TaskDescriptionDialog::TaskDescriptionDialog( Task &task, QWidget *p )
    : KDialog(p)
{
    setCaption( i18n( "Task Description" ) );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );

    m_descriptionTab = new TaskDescriptionPanel( task, this );
    setMainWidget(m_descriptionTab);

    enableButtonOk(false);

    connect( m_descriptionTab, SIGNAL( textChanged( bool ) ), this, SLOT( enableButtonOk(bool) ) );
}

MacroCommand *TaskDescriptionDialog::buildCommand()
{
    MacroCommand *m = new MacroCommand(i18n("Modify Task Description"));
    bool modified = false;
    MacroCommand *cmd = m_descriptionTab->buildCommand();
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

void TaskDescriptionDialog::slotButtonClicked( int button )
{
    if (button == KDialog::Ok) {
        if ( ! m_descriptionTab->ok() ) {
            return;
        }
        accept();
    } else {
        KDialog::slotButtonClicked( button );
    }
}


}  //KPlato namespace

#include "kpttaskdescriptiondialog.moc"
