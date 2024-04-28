/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoConnectionShapeTypeCommand.h"
#include <KLocalizedString>

KoConnectionShapeTypeCommand::KoConnectionShapeTypeCommand(KoConnectionShape *connection, KoConnectionShape::Type type, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_connection(connection)
    , m_newType(type)
{
    Q_ASSERT(m_connection);

    setText(kundo2_i18n("Change Connection"));

    m_oldType = m_connection->type();
}

void KoConnectionShapeTypeCommand::redo()
{
    KUndo2Command::redo();

    m_connection->update();

    if (m_oldType != m_newType)
        m_connection->setType(m_newType);

    m_connection->update();
}

void KoConnectionShapeTypeCommand::undo()
{
    KUndo2Command::undo();

    m_connection->update();

    if (m_oldType != m_newType)
        m_connection->setType(m_oldType);

    m_connection->update();
}
