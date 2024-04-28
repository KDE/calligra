/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPathPointRubberSelectStrategy.h"
#include "KoShapeRubberSelectStrategy_p.h"

#include "KoCanvasBase.h"
#include "KoPathTool.h"
#include "KoPathToolSelection.h"

KoPathPointRubberSelectStrategy::KoPathPointRubberSelectStrategy(KoPathTool *tool, const QPointF &clicked)
    : KoShapeRubberSelectStrategy(tool, clicked)
    , m_tool(tool)
{
}

void KoPathPointRubberSelectStrategy::finishInteraction(Qt::KeyboardModifiers modifiers)
{
    Q_D(KoShapeRubberSelectStrategy);
    KoPathToolSelection *selection = dynamic_cast<KoPathToolSelection *>(m_tool->selection());
    if (!selection)
        return;

    selection->selectPoints(d->selectedRect(), !(modifiers & Qt::ControlModifier));
    m_tool->canvas()->updateCanvas(d->selectedRect().normalized());
    selection->repaint();
}
