/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Lennart Kudling <kudling@kde.org>
   SPDX-FileCopyrightText: 2002 Benoît Vautrin <benoit.vautrin@free.fr>
   SPDX-FileCopyrightText: 2002 Rob Buis <buis@kde.org>
   SPDX-FileCopyrightText: 2005-2007 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2006 Peter Simonsson <psn@linux.se>
   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <t.zachmann@zagge.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KarbonLayerReorderCommand.h"
#include <KLocalizedString>
#include <KarbonDocument.h>
#include <KoShapeLayer.h>

KarbonLayerReorderCommand::KarbonLayerReorderCommand(KarbonDocument *document, KoShapeLayer *layer, ReorderType commandType, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_document(document)
    , m_cmdType(commandType)
{
    m_layers.append(layer);

    if (m_cmdType == RaiseLayer)
        setText(kundo2_i18n("Raise Layer"));
    else
        setText(kundo2_i18n("Lower Layer"));
}

KarbonLayerReorderCommand::KarbonLayerReorderCommand(KarbonDocument *document, QList<KoShapeLayer *> layers, ReorderType commandType, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_document(document)
    , m_layers(layers)
    , m_cmdType(commandType)
{
    if (m_cmdType == RaiseLayer)
        setText(kundo2_i18n("Raise Layer"));
    else
        setText(kundo2_i18n("Lower Layer"));
}

KarbonLayerReorderCommand::~KarbonLayerReorderCommand()
{
}

void KarbonLayerReorderCommand::redo()
{
    foreach (KoShapeLayer *layer, m_layers) {
        if (m_cmdType == RaiseLayer)
            m_document->raiseLayer(layer);
        else
            m_document->lowerLayer(layer);
    }
}

void KarbonLayerReorderCommand::undo()
{
    foreach (KoShapeLayer *layer, m_layers) {
        if (m_cmdType == RaiseLayer)
            m_document->lowerLayer(layer);
        else
            m_document->raiseLayer(layer);
    }
}
