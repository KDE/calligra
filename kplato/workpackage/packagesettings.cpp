/* This file is part of the KDE project
   Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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

#include "packagesettings.h"
#include "workpackage.h"

#include <QComboBox>

#include <kdebug.h>
#include <klocale.h>

namespace KPlatoWork
{

PackageSettingsDialog::PackageSettingsDialog(WorkPackage &p, QWidget *parent)
    : KDialog(parent)
{
    setCaption( i18n("Work Package Settings") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    //kDebug()<<&p;

    dia = new PackageSettingsPanel(p, this);

    setMainWidget(dia);
    enableButtonOk(false);

    connect(dia, SIGNAL(changed(bool) ), SLOT(enableButtonOk(bool)));
}

QUndoCommand *PackageSettingsDialog::buildCommand()
{
    //kDebug();
    return dia->buildCommand();
}


PackageSettingsPanel::PackageSettingsPanel(WorkPackage &p, QWidget *parent)
    : QWidget(parent),
      m_package( p )
{
    setupUi(this);

    setSettings( p.settings() );

    connect( ui_usedEffort, SIGNAL( stateChanged( int ) ), SLOT( slotChanged() ) );
    connect( ui_progress, SIGNAL( stateChanged( int ) ), SLOT( slotChanged() ) );
    connect( ui_remainingEffort, SIGNAL( stateChanged( int ) ), SLOT( slotChanged() ) );
    connect( ui_documents, SIGNAL( stateChanged( int ) ), SLOT( slotChanged() ) );
}

QUndoCommand *PackageSettingsPanel::buildCommand()
{
    //kDebug();
    WorkPackageSettings s = settings();
    if ( s == m_package.settings() ) {
        return 0;
    }
    return new ModifyPackageSettingsCmd( &m_package, s, i18n( "Modify package settings" ) );
}

void PackageSettingsPanel::slotChanged() {
    emit changed( settings() != m_package.settings() );
}

WorkPackageSettings PackageSettingsPanel::settings() const
{
    WorkPackageSettings s;
    s.usedEffort = ui_usedEffort->checkState() == Qt::Checked;
    s.progress = ui_progress->checkState() == Qt::Checked;
    s.remainingEffort = ui_remainingEffort->checkState() == Qt::Checked;
    s.documents = ui_documents->checkState() == Qt::Checked;
    return s;
}

void PackageSettingsPanel::setSettings( const WorkPackageSettings &s )
{
    ui_usedEffort->setCheckState( s.usedEffort ? Qt::Checked : Qt::Unchecked );
    ui_progress->setCheckState( s.progress ? Qt::Checked : Qt::Unchecked );
    ui_remainingEffort->setCheckState( s.remainingEffort ? Qt::Checked : Qt::Unchecked );
    ui_documents->setCheckState( s.documents ? Qt::Checked : Qt::Unchecked );
}

}  //KPlatoWork namespace


#include "packagesettings.moc"
