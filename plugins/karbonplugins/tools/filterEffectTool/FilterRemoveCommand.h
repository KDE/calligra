/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef FILTERREMOVECOMMAND_H
#define FILTERREMOVECOMMAND_H

#include <kundo2command.h>

class KoShape;
class KoFilterEffect;
class KoFilterEffectStack;

/// A command do remove a filter effect from a filter effect stack
class FilterRemoveCommand : public KUndo2Command
{
public:
    FilterRemoveCommand(int filterEffectIndex, KoFilterEffectStack *filterStack, KoShape *shape, KUndo2Command *parent = nullptr);
    ~FilterRemoveCommand() override;
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoFilterEffect *m_filterEffect;
    KoFilterEffectStack *m_filterStack;
    KoShape *m_shape;
    bool m_isRemoved;
    int m_filterEffectIndex;
};

#endif // FILTERREMOVECOMMAND_H
