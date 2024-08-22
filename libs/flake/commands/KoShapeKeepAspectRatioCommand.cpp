/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Peter Simonsson <peter.simonsson@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapeKeepAspectRatioCommand.h"

#include <KLocalizedString>

#include <KoShape.h>

KoShapeKeepAspectRatioCommand::KoShapeKeepAspectRatioCommand(const QList<KoShape *> &shapes,
                                                             const QList<bool> &oldKeepAspectRatio,
                                                             const QList<bool> &newKeepAspectRatio,
                                                             KUndo2Command *parent)
    : KUndo2Command(kundo2_i18n("Keep Aspect Ratio"), parent)
{
    m_shapes = shapes;
    m_oldKeepAspectRatio = oldKeepAspectRatio;
    m_newKeepAspectRatio = newKeepAspectRatio;
}

KoShapeKeepAspectRatioCommand::~KoShapeKeepAspectRatioCommand() = default;

void KoShapeKeepAspectRatioCommand::redo()
{
    KUndo2Command::redo();
    for (int i = 0; i < m_shapes.count(); ++i) {
        m_shapes[i]->setKeepAspectRatio(m_newKeepAspectRatio[i]);
    }
}

void KoShapeKeepAspectRatioCommand::undo()
{
    KUndo2Command::undo();
    for (int i = 0; i < m_shapes.count(); ++i) {
        m_shapes[i]->setKeepAspectRatio(m_oldKeepAspectRatio[i]);
    }
}
