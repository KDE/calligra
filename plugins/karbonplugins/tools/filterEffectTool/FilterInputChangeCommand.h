/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef FILTERINPUTCHANGECOMMAND_H
#define FILTERINPUTCHANGECOMMAND_H

#include <kundo2command.h>

class KoShape;
class KoFilterEffect;

struct InputChangeData {
    InputChangeData()
        : filterEffect(nullptr)
        , inputIndex(-1)
    {
    }

    InputChangeData(KoFilterEffect *effect, int index, const QString &oldIn, const QString &newIn)
        : filterEffect(effect)
        , inputIndex(index)
        , oldInput(oldIn)
        , newInput(newIn)
    {
    }

    KoFilterEffect *filterEffect;
    int inputIndex;
    QString oldInput;
    QString newInput;
};

/// A command to change the input of a filter effect
class FilterInputChangeCommand : public KUndo2Command
{
public:
    explicit FilterInputChangeCommand(const InputChangeData &data, KoShape *shape = nullptr, KUndo2Command *parent = nullptr);

    explicit FilterInputChangeCommand(const QList<InputChangeData> &data, KoShape *shape = nullptr, KUndo2Command *parent = nullptr);

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    QList<InputChangeData> m_data;
    KoShape *m_shape;
};

#endif // FILTERINPUTCHANGECOMMAND_H
