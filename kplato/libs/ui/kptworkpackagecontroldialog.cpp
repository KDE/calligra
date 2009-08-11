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

#include "kptworkpackagecontroldialog.h"
#include "kptworkpackagecontrolpanel.h"
#include "kptdocumentspanel.h"
#include "kpttask.h"

#include <klocale.h>

#include <kdebug.h>

namespace KPlato
{

WorkPackageControlDialog::WorkPackageControlDialog( Project &project, Task &task, QWidget *p)
    : KPageDialog(p)
{
    setFaceType( Tabbed );
    setCaption( i18n("Work Package Control") );
    setButtons( Close );
    setDefaultButton( Close );
    showButtonSeparator( true );

    m_wp = new WorkPackageControlPanel( project, task, this);
    addPage( m_wp, i18n( "General" ) );

    m_docs = new DocumentsPanel( task, this );
    addPage( m_docs, i18n( "Documents" ) );

}

WorkPackageSendDialog::WorkPackageSendDialog( const QList<Node*> &tasks, QWidget *p)
    : KDialog(p)
{
    setCaption( i18n("Send Work Packages") );
    setButtons( Close );
    setDefaultButton( Close );
    showButtonSeparator( true );

    m_wp = new WorkPackageSendPanel( tasks, this);
    setMainWidget( m_wp );
}


}  //KPlato namespace

#include "kptworkpackagecontroldialog.moc"
