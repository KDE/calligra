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

#include "PAView.h"
#include <KoZoomController.h>
#include <libs/kopageapp/KoPADocument.h>
#include <KoPACanvasBase.h>
#include <libs/kopageapp/KoPAPageBase.h>
#include <libs/kopageapp/KoPAViewModeNormal.h>
#include <kpresenter/part/KPrDocument.h>


PAView::PAView(KoPACanvasBase* canvas, KPrDocument* prDocument, KoZoomController* zoomController,
                       KoZoomHandler* zoomHandler) : m_paCanvas(canvas), m_prDocument(prDocument),
                       m_zoomController(zoomController), m_zoomHandler(zoomHandler)
{
    KoPAViewModeNormal *mode = new KoPAViewModeNormal(this, m_paCanvas);
    setViewMode(mode);
    setActivePage(prDocument->pageByIndex(0, false));
}

PAView::~PAView()
{

}

void PAView::setShowRulers(bool show)
{

}

void PAView::editPaste()
{

}

void PAView::pagePaste()
{

}

void PAView::insertPage()
{

}

void PAView::updatePageNavigationActions()
{

}

void PAView::setActionEnabled(int actions, bool enable)
{

}

void PAView::navigatePage(KoPageApp::PageNavigation pageNavigation)
{

}

KoPAPageBase* PAView::activePage() const
{
    return 0;
}

void PAView::setActivePage(KoPAPageBase* page)
{

}

void PAView::doUpdateActivePage(KoPAPageBase* page)
{

}

KoZoomController* PAView::zoomController() const
{
    return m_zoomController;
}

KoPADocument* PAView::kopaDocument() const
{
    return 0;
}

KoPACanvasBase* PAView::kopaCanvas() const
{
    return m_paCanvas;
}
