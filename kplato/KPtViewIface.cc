/*  This file is part of the KDE project
    Copyright (C) 2004 Laurent Montel <montel@kde.org>

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

#include "KPtViewIface.h"
#include "kptview.h"

#include <kapplication.h>
#include <dcopclient.h>
#include <dcopref.h>

namespace KPlato
{

/************************************************
 *
 * ViewIface
 *
 ************************************************/

ViewIface::ViewIface( View* t )
    : KoViewIface( t )
{
    m_view = t;
}

ViewIface::~ViewIface()
{
}

void ViewIface::slotEditResource()
{
  m_view->slotEditResource();
}

void ViewIface::slotEditCut()
{
  m_view->slotEditCut();
}

void ViewIface::slotEditCopy()
{
  m_view->slotEditCopy();
}

void ViewIface::slotEditPaste()
{
  m_view->slotEditPaste();
}

void ViewIface::slotViewGantt()
{
  m_view->slotViewGantt();
}

void ViewIface::slotViewPert()
{
  m_view->slotViewPert();
}

void ViewIface::slotViewResources()
{
  m_view->slotViewResources();
}

void ViewIface::slotAddTask()
{
  m_view->slotAddTask();
}

void ViewIface::slotAddSubTask()
{
  m_view->slotAddSubTask();
}

void ViewIface::slotAddMilestone()
{
  m_view->slotAddMilestone();
}

void ViewIface::slotProjectEdit()
{
  m_view->slotProjectEdit();
}

void ViewIface::slotConfigure()
{
  m_view->slotConfigure();
}

}  //KPlato namespace
