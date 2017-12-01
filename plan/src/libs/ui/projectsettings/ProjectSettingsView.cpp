/* This file is part of the KDE project
  Copyright (C) 2017 Dag Andersen <danders@get2net.dk>

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

#include "ProjectSettingsView.h"

#include "kptdebug.h"

#include <KoDocument.h>

#include <QFileDialog>

namespace KPlato
{

//-----------------------------------
ProjectSettingsView::ProjectSettingsView(KoPart *part, KoDocument *doc, QWidget *parent)
    : ViewBase(part, doc, parent)
{
    widget.setupUi(this);
    setupGui();

    connect(widget.resourcesBrowseBtn, SIGNAL(clicked()), this, SLOT(slotOpenResourcesFile()));
    connect(widget.resourcesConnectBtn, SIGNAL(clicked()), this, SLOT(slotResourcesConnect()));
}

void ProjectSettingsView::updateReadWrite( bool /*readwrite */)
{
}

void ProjectSettingsView::setGuiActive( bool activate )
{
    debugPlan<<activate;
}

void ProjectSettingsView::slotContextMenuRequested( const QModelIndex &/*index*/, const QPoint& /*pos */)
{
    //debugPlan<<index.row()<<","<<index.column()<<":"<<pos;
}

void ProjectSettingsView::slotEnableActions( bool on )
{
    updateActionsEnabled( on );
}

void ProjectSettingsView::updateActionsEnabled(  bool /*on */)
{
}

void ProjectSettingsView::setupGui()
{
    // Add the context menu actions for the view options
}

KoPrintJob *ProjectSettingsView::createPrintJob()
{
    return 0;//m_view->createPrintJob( this );
}

void ProjectSettingsView::slotOpenResourcesFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Resources"), "", tr("Resources files (*.plan)"));
    widget.resourcesFile->setText(fileName);
}

void ProjectSettingsView::slotResourcesConnect()
{
    QString fn = widget.resourcesFile->text();
    if (fn.startsWith('/')) {
        fn.prepend("file:/");
    }
    emit connectResources(fn);
}

} // namespace KPlato
