/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <jhahoneyk@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "PAViewBase.h"
#include <KoZoomController.h>
#include <libs/kopageapp/KoPADocument.h>
#include <KoPACanvasBase.h>
#include <libs/kopageapp/KoPAPageBase.h>

PAViewBase::PAViewBase(KoCanvasBase* canvasBase, KPrDocument* prDocument, KoZoomController* zoomController,
                       KoZoomHandler* zoomHandler) : m_paCanvasBase(), m_prDocument(prDocument),
                       m_zoomController(zoomController), m_zoomHandler(zoomHandler)
{

}

PAViewBase::~PAViewBase()
{

}

void PAViewBase::setShowRulers(bool show)
{

}

void PAViewBase::editPaste()
{

}

void PAViewBase::pagePaste()
{

}

void PAViewBase::insertPage()
{

}

void PAViewBase::updatePageNavigationActions()
{

}

void PAViewBase::setActionEnabled(int actions, bool enable)
{

}

void PAViewBase::navigatePage(KoPageApp::PageNavigation pageNavigation)
{

}

KoPAPageBase* PAViewBase::activePage() const
{
    return 0;
}

void PAViewBase::setActivePage(KoPAPageBase* page)
{

}

void PAViewBase::doUpdateActivePage(KoPAPageBase* page)
{

}

KoZoomController* PAViewBase::zoomController() const
{
    return m_zoomController;
}

KoPADocument* PAViewBase::kopaDocument() const
{
    return 0;
}

KoPACanvasBase* PAViewBase::kopaCanvas() const
{
    return m_paCanvasBase;
}
