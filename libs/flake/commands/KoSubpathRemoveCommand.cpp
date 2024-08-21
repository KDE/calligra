/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoSubpathRemoveCommand.h"

#include "KoPathPoint.h"
#include <KLocalizedString>

KoSubpathRemoveCommand::KoSubpathRemoveCommand(KoPathShape *pathShape, int subpathIndex, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_pathShape(pathShape)
    , m_subpathIndex(subpathIndex)
    , m_subpath(nullptr)
{
    setText(kundo2_i18n("Remove subpath"));
}

KoSubpathRemoveCommand::~KoSubpathRemoveCommand()
{
    if (m_subpath) {
        qDeleteAll(*m_subpath);
        delete m_subpath;
    }
}

void KoSubpathRemoveCommand::redo()
{
    KUndo2Command::redo();
    m_pathShape->update();
    m_subpath = m_pathShape->removeSubpath(m_subpathIndex);
    if (m_subpath) {
        QPointF offset = m_pathShape->normalize();

        QTransform matrix;
        matrix.translate(-offset.x(), -offset.y());
        foreach (KoPathPoint *point, *m_subpath) {
            point->map(matrix);
        }
        m_pathShape->update();
    }
}

void KoSubpathRemoveCommand::undo()
{
    KUndo2Command::undo();
    if (m_subpath) {
        m_pathShape->addSubpath(m_subpath, m_subpathIndex);
        m_pathShape->normalize();
        m_pathShape->update();
        m_subpath = nullptr;
    }
}
