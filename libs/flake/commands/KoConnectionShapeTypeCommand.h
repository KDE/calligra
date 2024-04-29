/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KoConnectionShapeTypeCommand_H
#define KoConnectionShapeTypeCommand_H

#include "KoConnectionShape.h"
#include <kundo2command.h>

/// The undo / redo command for configuring an KoConnection shape
class KoConnectionShapeTypeCommand : public KUndo2Command
{
public:
    /**
     * Changes the tyoe of a connection shape
     * @param connection the connection shape to change type of
     * @param type the connection type
     * @param parent the optional parent command
     */
    KoConnectionShapeTypeCommand(KoConnectionShape *connection, KoConnectionShape::Type type, KUndo2Command *parent = nullptr);
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoConnectionShape *m_connection;
    KoConnectionShape::Type m_oldType;
    KoConnectionShape::Type m_newType;
};

#endif // KoConnectionShapeTypeCommand_H
