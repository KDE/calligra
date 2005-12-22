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
   Boston, MA 02110-1301, USA.
*/

#include "KIvioPageIface.h"

#include "kivio_page.h"
#include "kivio_layer.h"
#include <kapplication.h>
#include <dcopclient.h>

KIvioPageIface::KIvioPageIface( KivioPage *page_ )
    : DCOPObject( page_)
{
    m_page = page_;
}

bool KIvioPageIface::isHidden()const
{
    return m_page->isHidden();
}

void KIvioPageIface::deleteSelectedStencils()
{
    m_page->deleteSelectedStencils();
}

void KIvioPageIface::groupSelectedStencils()
{
    m_page->groupSelectedStencils();
}

void KIvioPageIface::ungroupSelectedStencils()
{
    m_page->ungroupSelectedStencils();
}


QString KIvioPageIface::pageName()const
{
    return m_page->pageName();
}

void KIvioPageIface::copy()
{
    m_page->copy();
}

void KIvioPageIface::cut()
{
    m_page->cut();
}

void KIvioPageIface::paste()
{
//    m_page->paste();
}

void KIvioPageIface::selectAllStencils()
{
    m_page->selectAllStencils();
}

void KIvioPageIface::unselectAllStencils()
{
    m_page->unselectAllStencils();
}


int KIvioPageIface::nbLayer() const
{
    return (int)(m_page->layers())->count();
}

DCOPRef KIvioPageIface::firstLayer()
{
    if ( m_page->firstLayer())
        return DCOPRef( kapp->dcopClient()->appId(),
                        m_page->firstLayer()->dcopObject()->objId() );
    else
        return DCOPRef();
}

DCOPRef KIvioPageIface::layerAt( int pos)
{
    if ( pos >= nbLayer() )
        return DCOPRef();
    return DCOPRef( kapp->dcopClient()->appId(),
                    m_page->layerAt(pos)->dcopObject()->objId() );
}

void KIvioPageIface::setHidePage(bool _hide)
{
    m_page->setHidePage(_hide);
}
