/* This file is part of the KDE project
   Copyright (C) 2007 Dag Andersen <kplato@kde.org>

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

#include "kptchartview.h"
#include "kptchartpanel.h"
#include "kptpart.h"
#include "kptproject.h"

#include <klocale.h>

#include <QVBoxLayout>

namespace KPlato
{

ChartView::ChartView( Part *part, QWidget *parent )
    : ViewBase( part, parent ),
    m_project( 0 )
{
    QVBoxLayout *l = new QVBoxLayout( this );
    l->setContentsMargins( 0, 0, 0, 0 );
    m_panel = new ChartPanel( part->getProject(), this );
    l->addWidget( m_panel );
}

void ChartView::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL(currentViewScheduleIdChanged( long ) ), this, SLOT( slotCurrentViewScheduleIdChanged( long ) ) );
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
    }
    m_project = project;
    if ( project ) {
        connect( m_project, SIGNAL(currentViewScheduleIdChanged( long ) ), this, SLOT( slotCurrentViewScheduleIdChanged( long ) ) );
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
    }
    m_panel->draw( *project );
}

void ChartView::slotCurrentViewScheduleIdChanged( long id )
{
    if ( m_project ) {
        kDebug()<<k_funcinfo<<id<<endl;
        m_panel->draw( *m_project );
    }
}

void ChartView::slotNodeChanged( Node* )
{
    if ( m_project ) {
        kDebug()<<k_funcinfo<<endl;
        m_panel->draw( *m_project );
    }
}


} //namespace KPlato

#include "kptchartview.moc"

