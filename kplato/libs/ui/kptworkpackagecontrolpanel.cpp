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

#include "kptworkpackagecontrolpanel.h"
#include "kptworkpackageview.h"
#include "kptworkpackagemodel.h"

#include "kptproject.h"
#include "kpttask.h"

#include <klocale.h>
#include <kdebug.h>

#include <QPushButton>

namespace KPlato
{


WorkPackageControlPanel::WorkPackageControlPanel( Project &project, Task &task, QWidget *p )
    : QWidget(p),
    m_project( project ),
    m_task( task )
{
    setupUi( this );
    
    task.workPackage().setScheduleManager( 0 );
    
    resourceTable->itemModel()->setProject( &project );
    resourceTable->itemModel()->setTask( &task );
    
    
    connect( transferWP, SIGNAL( clicked(bool) ), SLOT( slotTransferWPClicked() ) );
    connect( loadWP, SIGNAL( clicked(bool) ), SLOT( slotLoadWPClicked() ) );
    connect( viewWP, SIGNAL( clicked(bool) ), SLOT( slotViewWPClicked() ) );
    connect( mailTo, SIGNAL( clicked(bool) ), SLOT( slotMailToClicked() ) );

}

void WorkPackageControlPanel::slotSelectionChanged()
{
}

void WorkPackageControlPanel::slotTransferWPClicked()
{
    kDebug();
}

void WorkPackageControlPanel::slotLoadWPClicked()
{
}

void WorkPackageControlPanel::slotViewWPClicked()
{
}

void WorkPackageControlPanel::slotMailToClicked()
{
}



}  //KPlato namespace

#include "kptworkpackagecontrolpanel.moc"
