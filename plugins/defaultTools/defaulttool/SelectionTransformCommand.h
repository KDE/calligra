/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SELECTION_TRANSFORM_COMMAND_H
#define SELECTION_TRANSFORM_COMMAND_H

#include <QTransform>
#include <kundo2command.h>

class KoSelection;
class KoShape;

class SelectionTransformCommand : public KUndo2Command
{
public:
    SelectionTransformCommand(KoSelection *selection,
                              const QTransform &oldTransformation,
                              const QTransform &newTransformation,
                              KUndo2Command *parent = nullptr);

    /// reimplemented from KUndo2Command
    void redo() override;
    /// reimplemented from KUndo2Command
    void undo() override;

private:
    KoSelection *m_selection;
    QList<KoShape *> m_selectedShapes;
    QTransform m_oldTransformation;
    QTransform m_newTransformation;
};

#endif // SELECTION_TRANSFORM_COMMAND_H
