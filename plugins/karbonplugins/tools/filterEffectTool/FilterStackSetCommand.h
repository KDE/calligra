/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef FILTERSTACKSETCOMMAND_H
#define FILTERSTACKSETCOMMAND_H

#include <kundo2command.h>

class KoFilterEffectStack;
class KoShape;

/// Command to set a filter stack on a shape
class FilterStackSetCommand : public KUndo2Command
{
public:
    FilterStackSetCommand(KoFilterEffectStack *newStack, KoShape *shape, KUndo2Command *parent = nullptr);
    ~FilterStackSetCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoFilterEffectStack *m_newFilterStack;
    KoFilterEffectStack *m_oldFilterStack;
    KoShape *m_shape;
};

#endif // FILTERSTACKSETCOMMAND_H
