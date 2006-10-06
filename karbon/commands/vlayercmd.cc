/* This file is part of the KDE project
   Copyright (C) 2001-2005, The Karbon Developers
   Copyright (C) 2006 Jan Hambrecht <jaham@gmx.net>

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

#include "vlayer.h"
#include "vlayercmd.h"
#include "vdocument.h"
#include <klocale.h>

VLayerCmd::VLayerCmd( VDocument* doc, KoLayerShape* layer, VLayerCmdType order )
: m_document( doc ), m_layer( layer ), m_cmdType( order ), m_deleteLayer( false )
{
    if( m_cmdType == addLayer )
        m_deleteLayer = true;
}

VLayerCmd::~VLayerCmd()
{
    if( m_deleteLayer )
        delete m_layer;
}

void VLayerCmd::execute()
{
    switch( m_cmdType )
    {
        case addLayer:
            m_document->insertLayer( m_layer );
            m_deleteLayer = false;
        break;

        case deleteLayer:
            m_document->removeLayer( m_layer );
            m_deleteLayer = true;
        break;
        case raiseLayer:
            m_document->raiseLayer( m_layer );
        break;

        case lowerLayer:
            m_document->lowerLayer( m_layer );
        break;
    }
}

void VLayerCmd::unexecute()
{
    switch ( m_cmdType )
    {
        case addLayer:
            m_document->removeLayer( m_layer );
            m_deleteLayer = true;
        break;

        case deleteLayer:
            m_document->insertLayer( m_layer );
            m_deleteLayer = false;
        break;

        case raiseLayer:
            m_document->lowerLayer( m_layer );
        break;

        case lowerLayer:
            m_document->raiseLayer( m_layer );
        break;
    }
}

QString VLayerCmd::name() const
{
    switch( m_cmdType )
    {
        case addLayer:
            return i18n( "Add Layer");
        case deleteLayer:
            return i18n( "Delete Layer");
        case raiseLayer:
            return i18n( "Raise Layer" );
        case lowerLayer:
            return i18n( "Lower Layer");
        default:
            return i18n( "Edit Layer");
    }
}
