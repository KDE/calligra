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

#include <KoShapeLayer.h>
#include "vlayercmd.h"
#include "vdocument.h"
#include <KoShapeControllerBase.h>
#include <KoShapeDeleteCommand.h>
#include <klocale.h>

VLayerDeleteCmd::VLayerDeleteCmd( VDocument* document, KoShapeControllerBase *shapeController, KoShapeLayer* layer, QUndoCommand* parent )
: QUndoCommand( parent ), m_document( document ), m_controller( shapeController ), m_deleteCmd( 0 ), m_deleteLayers( false )
{
    m_layers.append( layer );

    setText( i18n( "Delete Layer" ) );
}

VLayerDeleteCmd::VLayerDeleteCmd( VDocument* document, KoShapeControllerBase *shapeController, const QList<KoShapeLayer*> &layers, QUndoCommand* parent )
: QUndoCommand( parent ), m_document( document ), m_controller( shapeController ), m_layers( layers ), m_deleteCmd( 0 ), m_deleteLayers( false )
{
    setText( i18n( "Delete Layer" ) );
}

VLayerDeleteCmd::~VLayerDeleteCmd()
{
    if( m_deleteLayers )
        qDeleteAll( m_layers );
    if( m_deleteCmd )
        delete m_deleteCmd;
}

void VLayerDeleteCmd::redo()
{
    m_deleteLayers = true;
    QList<KoShape*> shapes;

    foreach( KoShapeLayer* layer, m_layers )
    {
        m_document->removeLayer( layer );
        shapes += layer->iterator();
    }
    if( ! m_deleteCmd )
        m_deleteCmd = new KoShapeDeleteCommand( m_controller, shapes );

    m_deleteCmd->redo();
}

void VLayerDeleteCmd::undo()
{
    m_deleteLayers = false;
    foreach( KoShapeLayer* layer, m_layers )
        m_document->insertLayer( layer );
    m_deleteCmd->undo();
}

VLayerCreateCmd::VLayerCreateCmd( VDocument* document, KoShapeLayer* layer, QUndoCommand* parent )
: QUndoCommand( parent ), m_document( document ), m_layer( layer ), m_deleteLayer( true )
{
    setText( i18n( "Create Layer") );
}

VLayerCreateCmd::~VLayerCreateCmd()
{
    if( m_deleteLayer )
        delete m_layer;
}

void VLayerCreateCmd::redo()
{
    m_document->insertLayer( m_layer );
    m_deleteLayer = false;
}

void VLayerCreateCmd::undo()
{
    m_document->removeLayer( m_layer );
    m_deleteLayer = true;
}

VLayerZOrderCmd::VLayerZOrderCmd( VDocument* document, KoShapeLayer* layer, VLayerCmdType commandType, QUndoCommand* parent )
: QUndoCommand( parent ), m_document( document ), m_cmdType( commandType )
{
    m_layers.append( layer );

    if( m_cmdType == raiseLayer )
        setText( i18n( "Raise Layer" ) );
    else
        setText( i18n( "Lower Layer") );
}

VLayerZOrderCmd::VLayerZOrderCmd( VDocument* document, QList<KoShapeLayer*> layers, VLayerCmdType commandType, QUndoCommand* parent )
: QUndoCommand( parent ), m_document( document ), m_layers( layers ), m_cmdType( commandType )
{
    if( m_cmdType == raiseLayer )
        setText( i18n( "Raise Layer" ) );
    else
        setText( i18n( "Lower Layer") );
}

VLayerZOrderCmd::~VLayerZOrderCmd()
{
}

void VLayerZOrderCmd::redo()
{
    foreach( KoShapeLayer* layer, m_layers )
    {
        if( m_cmdType == raiseLayer )
            m_document->raiseLayer( layer );
        else
            m_document->lowerLayer( layer );
    }
}

void VLayerZOrderCmd::undo()
{
    foreach( KoShapeLayer* layer, m_layers )
    {
        if( m_cmdType == raiseLayer )
            m_document->lowerLayer( layer );
        else
            m_document->raiseLayer( layer );
    }
}
