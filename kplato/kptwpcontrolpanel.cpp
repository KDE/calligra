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

#include <kdebug.h>

#include <QPushButton>
#include <QTableWidget>

namespace KPlato
{


WPControlPanel::WPControlPanel( View *view, Task &task, QWidget *p )
    : WorkPackageControlPanel( view->getProject(), task, p ),
    m_view( view )
{
}

void WPControlPanel::slotTransferWPClicked()
{
    KUrl url( "workpackage.kplatowork" );
    kDebug()<<m_task.name()<<" -> "<<url;
    m_view->getPart()->saveWorkPackageUrl( url, &m_task, m_view->currentScheduleId() );
    
/*    QString to = m_task.leader();
    QString cc;
    QString bcc;
    QString subject = m_task.name();
    QString body = m_task.description();
    QString messageFile;
    QStringList attachURLs = QStringList() << url.url();
    KToolInvocation::invokeMailer( to, cc, bcc, subject, body, messageFile, attachURLs );*/
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
