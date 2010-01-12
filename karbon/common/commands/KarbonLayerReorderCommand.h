/* This file is part of the KDE project
   Copyright (C) 2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2002 Benoît Vautrin <benoit.vautrin@free.fr>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>
   Copyright (C) 2006-2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Stephan Kulow <coolo@kde.org>
   Copyright (C) 2006 Peter Simonsson <psn@linux.se>

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

#ifndef KARBONLAYERREORDERCOMMAND_H
#define KARBONLAYERREORDERCOMMAND_H

#include <QUndoCommand>
#include <karboncommon_export.h>

class KarbonDocument;
class KoShapeLayer;

/// Command for raising or lowering layers
class KARBONCOMMON_EXPORT KarbonLayerReorderCommand : public QUndoCommand
{
public:
    /// The different types of layer commands.
    enum ReorderType {
        RaiseLayer, ///< raises layer in z-order
        LowerLayer  ///< lowers layer in z-order
    };

    /**
     * Layer command which works on a single layer.
     * @param document the document containing the layer
     * @param layer the layer which is subject to the command
     * @param commandType the type of the command to redo
     */
    KarbonLayerReorderCommand(KarbonDocument* document, KoShapeLayer* layer, ReorderType commandType, QUndoCommand* parent = 0);

    /**
     * Layer command which works on a single layer.
     * @param document the document containing the layer
     * @param layers the list of layers which are subject to the command
     * @param commandType the type of the command to redo
     */
    KarbonLayerReorderCommand(KarbonDocument* document, QList<KoShapeLayer*> layers, ReorderType commandType, QUndoCommand* parent = 0);

    virtual ~KarbonLayerReorderCommand();

    /// redo the command
    virtual void redo();
    /// revert the actions done in redo
    virtual void undo();

private:
    KarbonDocument *m_document;         ///< the document to work on
    QList<KoShapeLayer*> m_layers; ///< the list of layers subject to the command
    ReorderType m_cmdType;         ///< the type of the command to redo
};

#endif // KARBONLAYERREORDERCOMMAND_H

