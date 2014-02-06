/* This file is part of the KDE project
 * Copyright (C) 2014 Yue Liu <yue.liu@mail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KOTOSCONTAINERADDTEXTCOMMAND_H
#define KOTOSCONTAINERADDTEXTCOMMAND_H

#include "flake_export.h"
#include <kundo2command.h>

class KoTosContainer;
class KoDocumentResourceManager;

/**
 * A command to add textShape to KoTosContainers.
 */
class FLAKE_EXPORT KoTosContainerAddTextCommand : public KUndo2Command
{
public:

    /**
     * A command to add textShape to KoTosContainers.
     * @param tosContainers all the shapes that can add text
     */
    KoTosContainerAddTextCommand(const QList<KoTosContainer*> &tosContainers,
                                 KoDocumentResourceManager* resourceManager,
                                 KUndo2Command * parent = 0);
    ~KoTosContainerAddTextCommand();
    /// redo the command
    void redo();
    /// revert the actions done in redo
    void undo();

private:
    class Private;
    Private * const d;
};

#endif // KOTOSCONTAINERADDTEXTCOMMAND_H
