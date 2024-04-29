/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHCOMBINECOMMAND_H
#define KOPATHCOMBINECOMMAND_H

#include "KoPathShape.h"
#include "flake_export.h"
#include <QList>
#include <kundo2command.h>

class KoShapeBasedDocumentBase;

/// The undo / redo command for combining two or more paths into one
class FLAKE_EXPORT KoPathCombineCommand : public KUndo2Command
{
public:
    /**
     * Command for combining a list of paths into one single path.
     * @param controller the controller to used for removing/inserting.
     * @param paths the list of paths to combine
     * @param parent the parent command used for macro commands
     */
    KoPathCombineCommand(KoShapeBasedDocumentBase *controller, const QList<KoPathShape *> &paths, KUndo2Command *parent = nullptr);
    ~KoPathCombineCommand() override;
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    class Private;
    Private *const d;
};

#endif // KOPATHCOMBINECOMMAND_H
