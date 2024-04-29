/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSUBPATHREMOVECOMMAND_H
#define KOSUBPATHREMOVECOMMAND_H

#include "KoPathShape.h"
#include "flake_export.h"
#include <kundo2command.h>

/// The undo / redo command for removing a subpath
class FLAKE_EXPORT KoSubpathRemoveCommand : public KUndo2Command
{
public:
    /**
     * Create a new command to remove a subpath.
     * @param pathShape the shape to work on.
     * @param subpathIndex the index. See KoPathShape::removeSubpath()
     * @param parent the parent command if the resulting command is a compound undo command.
     */
    KoSubpathRemoveCommand(KoPathShape *pathShape, int subpathIndex, KUndo2Command *parent = nullptr);
    ~KoSubpathRemoveCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoPathShape *m_pathShape;
    int m_subpathIndex;
    KoSubpath *m_subpath;
};

#endif // KOSUBPATHREMOVECOMMAND_H
