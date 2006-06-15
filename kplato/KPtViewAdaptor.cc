/*  This file is part of the KDE project
    Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
              (C) 2004 Laurent Montel <montel@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301  USA
*/

#include "KPtViewAdaptor.h"
#include "kptview.h"

#include <kapplication.h>

namespace KPlato
{

/************************************************
 *
 * ViewAdaptor
 *
 ************************************************/

ViewAdaptor::ViewAdaptor( View* t )
    : KoViewAdaptor( t )
{
    setAutoRelaySignals(true);
    m_view = t;
}

ViewAdaptor::~ViewAdaptor()
{
}

void ViewAdaptor::slotEditResource()
{
  m_view->slotEditResource();
}

void ViewAdaptor::slotEditCut()
{
  m_view->slotEditCut();
}

void ViewAdaptor::slotEditCopy()
{
  m_view->slotEditCopy();
}

void ViewAdaptor::slotEditPaste()
{
  m_view->slotEditPaste();
}

void ViewAdaptor::slotViewGantt()
{
  m_view->slotViewGantt();
}

void ViewAdaptor::slotViewPert()
{
  m_view->slotViewPert();
}

void ViewAdaptor::slotViewResources()
{
  m_view->slotViewResources();
}

void ViewAdaptor::slotAddTask()
{
  m_view->slotAddTask();
}

void ViewAdaptor::slotAddSubTask()
{
  m_view->slotAddSubTask();
}

void ViewAdaptor::slotAddMilestone()
{
  m_view->slotAddMilestone();
}

void ViewAdaptor::slotProjectEdit()
{
  m_view->slotProjectEdit();
}

void ViewAdaptor::slotConfigure()
{
  m_view->slotConfigure();
}

}  //KPlato namespace

#include "KPtViewAdaptor.moc"
