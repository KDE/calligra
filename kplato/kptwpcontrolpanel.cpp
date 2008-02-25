/* This file is part of the KDE project
   Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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

#include "kptwpcontrolpanel.h"

#include "kptpart.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptview.h"
#include "kpttaskprogresspanel.h"
#include "kptcommand.h"

#include <klocale.h>
#include <ktoolinvocation.h>
#include <ktemporaryfile.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kurlrequesterdialog.h>

#include <kdebug.h>

#include <QPushButton>

namespace KPlato
{


WPControlPanel::WPControlPanel( View *view, Task &task, QWidget *p )
    : WorkPackageControlPanel( view->getProject(), task, p ),
    m_view( view )
{
    task.workPackage().setScheduleManager( view->currentScheduleManager() );
    
    transferWP->hide(); //
}

void WPControlPanel::slotTransferWPClicked()
{
}

void WPControlPanel::slotLoadWPClicked()
{
    kDebug();
    KUrlRequesterDialog *dlg = new KUrlRequesterDialog( QString(), QString(), this );
    dlg->setWindowTitle( i18n( "Work Package" ) );
    if ( dlg->exec() != QDialog::Accepted ) {
        delete dlg;
        return;
    }
    KUrl url = dlg->selectedUrl();
    delete dlg;
    Project project;
    if ( ! m_view->loadWorkPackage( project, url ) ) {
        kDebug()<<"Failed";
        return;
    }
    kDebug()<<project.numChildren();
    if ( project.numChildren() != 1 ) {
        kDebug()<<"No data or too much data:"<<project.numChildren();
        return;
    }
    if ( project.id() != m_project.id() ) {
        kDebug()<<"Different project";
        return;
    }
    Node *node = project.childNode( 0 );
    if ( node == 0 ) {
        kDebug()<<"No node after all?";
        return;
    }
    if ( node->type() != Node::Type_Task ) {
        kDebug()<<"Not a task:"<<node->typeToString();
    }
    Task *task = static_cast<Task*>( node );
    if ( task->id() != m_task.id() ) {
        kDebug()<<"Different task";
        return;
    }
    kDebug()<<project.name()<<"/"<<node->name();
    if ( task->completion() == m_task.completion() ) {
        kDebug()<<"No changes to completion";
        return;
    }
    kDebug()<<"Handle changes to completion";
    MacroCommand *cmd = TaskProgressPanel::buildCommand( m_project, m_task.completion(), task->completion() );
    if ( cmd ) {
        MacroCommand *m = new MacroCommand( i18n( "Update from Work Package" ) );
        m->addCommand( cmd );
        m_view->getPart()->addCommand( m );
        kDebug()<<"Completion updated";
    } else kDebug()<<"No changes";
}

void WPControlPanel::slotMailToClicked()
{
    kDebug();
    KTemporaryFile tmpfile;
    tmpfile.setAutoRemove( false );
    tmpfile.setSuffix( ".kplatowork" );
    if ( ! tmpfile.open() ) {
        kDebug()<<"Failed to open file";
        KMessageBox::error(0, i18n("Failed to open temporary file" ) );
        return;
    }
    KUrl url;
    url.setPath( tmpfile.fileName() );
    if ( ! m_view->getPart()->saveWorkPackageUrl( url, &m_task, m_view->activeScheduleId() ) ) {
        kDebug()<<"Failed to save to file";
        KMessageBox::error(0, i18n("Failed to save to temporary file: %1", url.url() ) );
        return;
    }
    QStringList attachURLs;
    attachURLs << url.url();
    QStringList lst;
    lst << m_task.leader();
    foreach ( Resource *r, m_task.workPackage().resources() ) {
        lst << r->name() + " <" + r->email() + '>';
    }
    QString to = lst.join( "," );
    QString cc;
    QString bcc;
    QString subject = i18n( "Work Package: %1", m_task.name() );
    QString body = m_task.description();
    QString messageFile;

    KToolInvocation::invokeMailer( to, cc, bcc, subject, body, messageFile, attachURLs );
}


}  //KPlato namespace

#include "kptwpcontrolpanel.moc"
