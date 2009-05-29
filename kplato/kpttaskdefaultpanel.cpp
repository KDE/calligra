/* This file is part of the KDE project
   Copyright (C) 2004, 2007 Dag Andersen <danders@get2net.dk>

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

#include "kpttaskdefaultpanel.h"

#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>

#include <QDateTime>
#include <QDateTimeEdit>

#include <kdebug.h>

namespace KPlato
{

TaskDefaultPanel::TaskDefaultPanel( QWidget *parent )
    : ConfigTaskPanelImpl( parent )
{
}

//-----------------------------
ConfigTaskPanelImpl::ConfigTaskPanelImpl(QWidget *p )
    : QWidget(p)
{

    setupUi(this);

    connect(chooseLeader, SIGNAL(clicked()), SLOT(changeLeader()));
    
    connect( kcfg_ConstraintStartTime, SIGNAL( dateTimeChanged ( const QDateTime& ) ), SLOT( startDateTimeChanged( const QDateTime& ) ) );
    
    connect( kcfg_ConstraintEndTime, SIGNAL( dateTimeChanged ( const QDateTime& ) ), SLOT( endDateTimeChanged( const QDateTime& ) ) );


}


void ConfigTaskPanelImpl::changeLeader()
{
    KABC::Addressee a = KABC::AddresseeDialog::getAddressee(this);
    if (!a.isEmpty())
    {
        kcfg_Leader->setText(a.fullEmail());
    }
}

void ConfigTaskPanelImpl::startDateTimeChanged( const QDateTime &dt )
{
    if ( dt > kcfg_ConstraintEndTime->dateTime() ) {
        kcfg_ConstraintEndTime->setDateTime( dt );
    }
}

void ConfigTaskPanelImpl::endDateTimeChanged( const QDateTime &dt )
{
    if ( kcfg_ConstraintStartTime->dateTime() > dt ) {
        kcfg_ConstraintStartTime->setDateTime( dt );
    }
}


}  //KPlato namespace

#include "kpttaskdefaultpanel.moc"
