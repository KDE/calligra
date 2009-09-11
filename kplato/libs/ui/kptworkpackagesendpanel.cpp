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

#include "kptworkpackagesendpanel.h"
#include "kpttaskeditor.h"

#include "kptproject.h"
#include "kpttask.h"
#include "kptschedule.h"

#include <klocale.h>
#include <kdebug.h>

#include <QPushButton>
#include <QLineEdit>
#include <QList>

namespace KPlato
{


WorkPackageSendPanel::WorkPackageSendPanel( const QList<Node*> &tasks,  ScheduleManager *sm, QWidget *p )
    : QWidget(p)
{
    setupUi( this );
    long id = sm ? sm->id() : NOTSCHEDULED;
    foreach ( Node *n, tasks ) {
        Task *t = qobject_cast<Task*>( n );
        if ( t == 0 ) {
            continue;
        }
        foreach ( Resource *r, t->workPackage().fetchResources( id ) ) {
            m_resMap[ r ] << n;
        }
    }
    QMap<Resource*, QList<Node*> >::const_iterator it;
    for ( it = m_resMap.constBegin(); it != m_resMap.constEnd(); ++it ) {
        QPushButton *pb = new QPushButton( KIcon( "mail-send" ), i18n( "Send To..." ), scrollArea );
        QLineEdit *le = new QLineEdit( it.key()->name(), scrollArea );
        le->setReadOnly( true );
        formLayout->addRow( pb, le );

        connect( pb, SIGNAL( clicked(bool) ), SLOT( slotSendClicked() ) );
        m_pbMap[ pb ] = it.key();
    }

}

void WorkPackageSendPanel::slotSendClicked()
{
    Resource *r = m_pbMap[ qobject_cast<QPushButton*>( sender() ) ];
    emit sendWorkpackages( m_resMap[ r ], r );
}


}  //KPlato namespace

#include "kptworkpackagesendpanel.moc"
