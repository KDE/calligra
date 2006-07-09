/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

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


#include <dcopclient.h>

#include "karbon_view.h"
#include "karbon_view_iface.h"
#include "vcanvas.h"

KarbonViewIface::KarbonViewIface( KarbonView* view )
		: KoViewIface( view )
{
	m_view = view;
}

void
KarbonViewIface::editCut()
{
	m_view->editCut();
}

void
KarbonViewIface::editCopy()
{
	m_view->editCopy();
}

void
KarbonViewIface::editPaste()
{
	m_view->editPaste();
}

void
KarbonViewIface::editSelectAll()
{
	m_view->editSelectAll();
}

void
KarbonViewIface::editDeselectAll()
{
	m_view->editDeselectAll();
}

void
KarbonViewIface::editDeleteSelection()
{
	m_view->editDeleteSelection();
}

void
KarbonViewIface::editPurgeHistory()
{
	m_view->editPurgeHistory();
}

void
KarbonViewIface::objectMoveToTop()
{
	m_view->selectionBringToFront();
}

void
KarbonViewIface::objectMoveToBottom()
{
	m_view->selectionSendToBack();
}

void
KarbonViewIface::objectMoveUp()
{
	m_view->selectionMoveUp();
}

void
KarbonViewIface::objectMoveDown()
{
	m_view->selectionMoveDown();
}

double
KarbonViewIface::zoomFactor() const
{
	return m_view->zoom();
}

void KarbonViewIface::groupSelection()
{
	m_view->groupSelection();
}

void KarbonViewIface::ungroupSelection()
{
	m_view->ungroupSelection();
}

void KarbonViewIface::configure()
{
	m_view->configure();
}

void KarbonViewIface::setLineWidth( double val )
{
	m_view->setLineWidth( val );
}


void KarbonViewIface::insertKnots()
{
	//m_view->pathInsertKnots();
}

void KarbonViewIface::pathFlatten()
{
	//m_view->pathFlatten();
}

void KarbonViewIface::pathRoundCorners()
{
	//m_view->pathRoundCorners();
}

void KarbonViewIface::pathWhirlPinch()
{
	//m_view->pathWhirlPinch();
}

void KarbonViewIface::repaint()
{
	m_view->canvasWidget()->repaintAll();
}

