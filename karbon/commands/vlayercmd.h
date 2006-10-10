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

#ifndef __VLAYERCMD_H__
#define __VLAYERCMD_H__

#include <kcommand.h>

class VDocument;
class KoLayerShape;
class KoShapeControllerBase;
class KoShapeDeleteCommand;

/// Command for deleting layers
class VLayerDeleteCmd : public KCommand
{
public:
    /**
     * Layer command which works on a single layer.
     * @param document the document containing the layer
     * @param controller the controller to remove the layers shape from
     * @param layer the layer which is subject to the command
     */
    VLayerDeleteCmd( VDocument* document, KoShapeControllerBase *shapeController, KoLayerShape* layer );

    /**
     * Layer command which works on a list of layers.
     * @param document the document containing the layers
     * @param controller the controller to remove the layers shape from
     * @param layers the layers which are subject to the command
     */
    VLayerDeleteCmd( VDocument* document, KoShapeControllerBase *shapeController, const QList<KoLayerShape*> &layers );
    virtual ~VLayerDeleteCmd();

    /// execute the command
    virtual void execute ();
    /// revert the actions done in execute
    virtual void unexecute ();
    /// return the name of this command
    virtual QString name () const;
private:
    VDocument *m_document;               ///< the document to work on
    KoShapeControllerBase *m_controller; ///< the shape controller to remove the layers shapes from
    QList<KoLayerShape*> m_layers;       ///< the list of layers subject to the command
    KoShapeDeleteCommand *m_deleteCmd;   ///< the command for deleting the layers shapes
    bool m_deleteLayers;                 ///< controls if layers should be deleted when destroying the command
};

/// Command for creating layers
class VLayerCreateCmd : public KCommand
{
public:
    /**
     * Layer command which works on a single layer.
     * @param document the document containing the layer
     * @param layer the layer which is subject to the command
     */
    VLayerCreateCmd( VDocument* document, KoLayerShape* layer );

    virtual ~VLayerCreateCmd();

    /// execute the command
    virtual void execute ();
    /// revert the actions done in execute
    virtual void unexecute ();
    /// return the name of this command
    virtual QString name () const;

private:
    VDocument *m_document;    ///< the document to work on
    KoLayerShape* m_layer;    ///< the layer subject to the command
    bool m_deleteLayer;       ///< controls if layers should be deleted when destroying the command
};

/// Command for raising or lowering layers
class VLayerZOrderCmd : public KCommand
{
public:
    /// The different types of layer commands.
    enum VLayerCmdType
    {
        raiseLayer, ///< raises layer in z-order
        lowerLayer, ///< lowers layer in z-order
    };

    /**
     * Layer command which works on a single layer.
     * @param document the document containing the layer
     * @param layer the layer which is subject to the command
     * @param commandType the type of the command to execute
     */
    VLayerZOrderCmd( VDocument* document, KoLayerShape* layer, VLayerCmdType commandType );

    /**
     * Layer command which works on a single layer.
     * @param document the document containing the layer
     * @param layers the list of layers which are subject to the command
     * @param commandType the type of the command to execute
     */
    VLayerZOrderCmd( VDocument* document, QList<KoLayerShape*> layers, VLayerCmdType commandType );

    virtual ~VLayerZOrderCmd();

    /// execute the command
    virtual void execute ();
    /// revert the actions done in execute
    virtual void unexecute ();
    /// return the name of this command
    virtual QString name () const;

private:
    VDocument *m_document;         ///< the document to work on
    QList<KoLayerShape*> m_layers; ///< the list of layers subject to the command
    VLayerCmdType m_cmdType;       ///< the type of the command to execute
};

#endif

