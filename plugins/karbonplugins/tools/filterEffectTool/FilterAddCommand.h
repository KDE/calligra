/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef FILTERADDCOMMAND_H
#define FILTERADDCOMMAND_H

#include <kundo2command.h>

class KoShape;
class KoFilterEffect;

/// A command do add a new filter effect to a filter effect stack
class FilterAddCommand : public KUndo2Command
{
public:
    FilterAddCommand(KoFilterEffect *filterEffect, KoShape *shape, KUndo2Command *parent = nullptr);
    ~FilterAddCommand() override;
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoFilterEffect *m_filterEffect;
    KoShape *m_shape;
    bool m_isAdded;
};

#endif // FILTERADDCOMMAND_H
