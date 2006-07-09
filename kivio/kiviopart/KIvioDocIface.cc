/* This file is part of the KDE project
   Copyright (C) 2001 Laurent MONTEL <lmontel@mandrakesoft.com>

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

#include "KIvioDocIface.h"

#include "kivio_doc.h"
#include "kivio_map.h"
#include <dcopclient.h>
#include <kapplication.h>
#include <kmainwindow.h>
#include "kivio_grid_data.h"
#include "kivio_config.h"

KIvioDocIface::KIvioDocIface( KivioDoc *doc_ )
    : KoDocumentIface( doc_ )
{
   doc = doc_;
}

DCOPRef KIvioDocIface::map()
{
    return DCOPRef( kapp->dcopClient()->appId(),
                    doc->map()->dcopObject()->objId() );
}

void KIvioDocIface::initConfig()
{
    doc->initConfig();
}

void KIvioDocIface::saveConfig()
{
    doc->saveConfig();
}

void KIvioDocIface::setShowGrid( bool b )
{
    Kivio::Config::setShowGrid(b);
    doc->updateButton();
    doc->setModified( true );
}

bool KIvioDocIface::showGrid() const
{
    KivioGridData d = doc->grid();
    return d.isShow;
}

int KIvioDocIface::undoRedoLimit() const
{
    return doc->undoRedoLimit();
}

void KIvioDocIface::setUndoRedoLimit(int val)
{
    doc->setUndoRedoLimit( val );
}

void KIvioDocIface::snapToGrid( bool b )
{
    Kivio::Config::setSnapGrid(b);
    doc->updateButton();
    doc->setModified( true );
}

bool KIvioDocIface::isSnapToGrid() const
{
    KivioGridData d = doc->grid();
    return d.isSnap;
}
