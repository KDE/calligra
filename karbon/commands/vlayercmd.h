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

/// Command for adding, deleting, raising, lowering layers
class VLayerCmd : public KCommand
{
public:
    /**
     * The different types of layer commands.
     */
    enum VLayerCmdType
    {
        addLayer,
        raiseLayer,
        lowerLayer,
        deleteLayer
    };

    VLayerCmd( VDocument* doc, KoLayerShape* layer, VLayerCmdType order );
    virtual ~VLayerCmd();

    /// execute the command
    virtual void execute ();
    /// revert the actions done in execute
    virtual void unexecute ();
    /// return the name of this command
    virtual QString name () const;

protected:
    VDocument *m_document;
    KoLayerShape* m_layer;
    VLayerCmdType m_cmdType;
    bool m_deleteLayer;
};

#endif

