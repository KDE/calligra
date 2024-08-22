/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoParameterHandleMoveCommand.h"
#include "KoParameterShape.h"
#include <KLocalizedString>

KoParameterHandleMoveCommand::KoParameterHandleMoveCommand(KoParameterShape *shape,
                                                           int handleId,
                                                           const QPointF &startPoint,
                                                           const QPointF &endPoint,
                                                           Qt::KeyboardModifiers keyModifiers,
                                                           KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_shape(shape)
    , m_handleId(handleId)
    , m_startPoint(startPoint)
    , m_endPoint(endPoint)
    , m_keyModifiers(keyModifiers)
{
    setText(kundo2_i18n("Change parameter"));
}

KoParameterHandleMoveCommand::~KoParameterHandleMoveCommand() = default;

/// redo the command
void KoParameterHandleMoveCommand::redo()
{
    KUndo2Command::redo();
    m_shape->update();
    m_shape->moveHandle(m_handleId, m_endPoint, m_keyModifiers);
    m_shape->update();
}

/// revert the actions done in redo
void KoParameterHandleMoveCommand::undo()
{
    KUndo2Command::undo();
    m_shape->update();
    m_shape->moveHandle(m_handleId, m_startPoint);
    m_shape->update();
}
