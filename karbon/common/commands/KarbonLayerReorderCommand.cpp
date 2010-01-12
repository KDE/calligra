/* This file is part of the KDE project
   Copyright (C) 2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2002 Benoît Vautrin <benoit.vautrin@free.fr>
   Copyright (C) 2002 Rob Buis <buis@kde.org>
   Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
   Copyright (C) 2006-2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Peter Simonsson <psn@linux.se>
   Copyright (C) 2006 Thorsten Zachmann <t.zachmann@zagge.de>

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

#include "KarbonLayerReorderCommand.h"
#include "KarbonDocument.h"
#include <KoShapeLayer.h>
#include <klocale.h>

KarbonLayerReorderCommand::KarbonLayerReorderCommand(KarbonDocument* document, KoShapeLayer* layer, ReorderType commandType, QUndoCommand* parent)
        : QUndoCommand(parent), m_document(document), m_cmdType(commandType)
{
    m_layers.append(layer);

    if (m_cmdType == RaiseLayer)
        setText(i18n("Raise Layer"));
    else
        setText(i18n("Lower Layer"));
}

KarbonLayerReorderCommand::KarbonLayerReorderCommand(KarbonDocument* document, QList<KoShapeLayer*> layers, ReorderType commandType, QUndoCommand* parent)
        : QUndoCommand(parent), m_document(document), m_layers(layers), m_cmdType(commandType)
{
    if (m_cmdType == RaiseLayer)
        setText(i18n("Raise Layer"));
    else
        setText(i18n("Lower Layer"));
}

KarbonLayerReorderCommand::~KarbonLayerReorderCommand()
{
}

void KarbonLayerReorderCommand::redo()
{
    foreach(KoShapeLayer* layer, m_layers) {
        if (m_cmdType == RaiseLayer)
            m_document->raiseLayer(layer);
        else
            m_document->lowerLayer(layer);
    }
}

void KarbonLayerReorderCommand::undo()
{
    foreach(KoShapeLayer* layer, m_layers) {
        if (m_cmdType == RaiseLayer)
            m_document->lowerLayer(layer);
        else
            m_document->raiseLayer(layer);
    }
}
