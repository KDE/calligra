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
#include "kptproject.h"

#include <KoDocument.h>

#include <klocale.h>

#include <QVBoxLayout>

namespace KPlato
{

ChartView::ChartView( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent ),
    m_project( 0 ),
    m_manager( 0 )
{
    QVBoxLayout *l = new QVBoxLayout( this );
    l->setContentsMargins( 0, 0, 0, 0 );
    m_panel = new ChartPanel( this );
    l->addWidget( m_panel );
}

void ChartView::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
    }
    m_project = project;
    if ( project ) {
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        
        if ( m_manager != 0 ) {
            m_panel->draw( *project, *m_manager );
        }
    }
}

void ChartView::setScheduleManager( ScheduleManager *sm )
{
    m_manager = sm;
    if ( m_project && sm ) {
        m_panel->draw( *m_project, *sm );
    }
}

void ChartView::slotNodeChanged( Node* )
{
    //kDebug()<<m_project<<m_manager;
    if ( m_project && m_manager) {
        m_panel->draw( *m_project, *m_manager );
    }
}


} //namespace KPlato

#include "kptchartview.moc"

