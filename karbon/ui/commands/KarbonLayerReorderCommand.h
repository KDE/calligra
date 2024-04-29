/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Lennart Kudling <kudling@kde.org>
   SPDX-FileCopyrightText: 2002 Benoît Vautrin <benoit.vautrin@free.fr>
   SPDX-FileCopyrightText: 2005 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2006 Stephan Kulow <coolo@kde.org>
   SPDX-FileCopyrightText: 2006 Peter Simonsson <psn@linux.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KARBONLAYERREORDERCOMMAND_H
#define KARBONLAYERREORDERCOMMAND_H

#include <karbonui_export.h>
#include <kundo2command.h>

class KarbonDocument;
class KoShapeLayer;

/// Command for raising or lowering layers
class KARBONUI_EXPORT KarbonLayerReorderCommand : public KUndo2Command
{
public:
    /// The different types of layer commands.
    enum ReorderType {
        RaiseLayer, ///< raises layer in z-order
        LowerLayer ///< lowers layer in z-order
    };

    /**
     * Layer command which works on a single layer.
     * @param document the document containing the layer
     * @param layer the layer which is subject to the command
     * @param commandType the type of the command to redo
     * @param parent the parent command used for stacking
     */
    KarbonLayerReorderCommand(KarbonDocument *document, KoShapeLayer *layer, ReorderType commandType, KUndo2Command *parent = nullptr);

    /**
     * Layer command which works on a single layer.
     * @param document the document containing the layer
     * @param layers the list of layers which are subject to the command
     * @param commandType the type of the command to redo
     * @param parent the parent command used for stacking
     */
    KarbonLayerReorderCommand(KarbonDocument *document, QList<KoShapeLayer *> layers, ReorderType commandType, KUndo2Command *parent = nullptr);

    virtual ~KarbonLayerReorderCommand();

    /// redo the command
    virtual void redo();
    /// revert the actions done in redo
    virtual void undo();

private:
    KarbonDocument *m_document; ///< the document to work on
    QList<KoShapeLayer *> m_layers; ///< the list of layers subject to the command
    ReorderType m_cmdType; ///< the type of the command to redo
};

#endif // KARBONLAYERREORDERCOMMAND_H
