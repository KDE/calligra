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

#include "KIvioLayerIface.h"

#include "kivio_layer.h"

#include <dcopclient.h>

KIvioLayerIface::KIvioLayerIface( KivioLayer *layer_ )
    : DCOPObject(  )
{
    m_layer = layer_;
}

bool KIvioLayerIface::visible()
{
    return m_layer->visible();
}

void KIvioLayerIface::setVisible( bool f )
{
    m_layer->setVisible( f );
}

bool KIvioLayerIface::connectable()
{
    return m_layer->connectable();
}

void KIvioLayerIface::setConnectable( bool f )
{
    m_layer->setConnectable( f );
}

QString KIvioLayerIface::name() const
{
    return m_layer->name();
}

void KIvioLayerIface::setName( const QString &n )
{
    m_layer->setName( n );
}
