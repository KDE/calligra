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

#include <klocale.h>
#include <ktoolinvocation.h>
#include <ktemporaryfile.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>

#include <kdebug.h>

#include <QPushButton>

namespace KPlato
{


WPControlPanel::WPControlPanel( View *view, Task &task, QWidget *p )
    : WorkPackageControlPanel( view->getProject(), task, p ),
    m_view( view )
{
}

void WPControlPanel::slotTransferWPClicked()
{
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
    if ( ! m_view->getPart()->saveWorkPackageUrl( url, &m_task, m_view->currentScheduleId() ) ) {
        kDebug()<<"Failed to save to file";
        KMessageBox::error(0, i18n("Failed to save to temporary file: %1", url.url() ) );
        return;
    }
    QStringList attachURLs;
    attachURLs << url.url();
    QString to;
    QString cc;
    QString bcc;
    QString subject = i18n( "Work Package: %1", m_task.name() );
    QString body = m_task.description();
    QString messageFile;

    KToolInvocation::invokeMailer( to, cc, bcc, subject, body, messageFile, attachURLs );
    
}

void WPControlPanel::slotLoadWPClicked()
{
    kDebug();
}

void WPControlPanel::slotMailToClicked()
{
    kDebug();
}


}  //KPlato namespace

#include "kptwpcontrolpanel.moc"
