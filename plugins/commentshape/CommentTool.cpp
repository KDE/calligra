/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "CommentTool.h"
#include "CommentShape.h"
#include "InitialsCommentShape.h"

#include <KoCanvasBase.h>
#include <KoShape.h>
#include <KoShapeManager.h>

#include <KoPointerEvent.h>
#include <QSet>

CommentTool::CommentTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
    , m_canvas(canvas)
    , m_previouseActiveCommentShape(0)
{
}

CommentTool::~CommentTool()
{
}

void CommentTool::activate(KoToolBase::ToolActivation toolActivation, const QSet<KoShape *> & /*shapes*/)
{
    const QCursor cursor(Qt::ArrowCursor);
    Q_EMIT useCursor(cursor);

    QList<KoShape *> allShapes = m_canvas->shapeManager()->shapes();
    foreach (KoShape *shape, allShapes) {
        InitialsCommentShape *initialsShape = dynamic_cast<InitialsCommentShape *>(shape);
        if (initialsShape) {
            initialsShape->setSelectable(true);
        }
    }

    m_temporary = toolActivation == KoToolBase::TemporaryActivation;
}

void CommentTool::deactivate()
{
    QList<KoShape *> allShapes = m_canvas->shapeManager()->shapes();
    foreach (KoShape *shape, allShapes) {
        InitialsCommentShape *initialsShape = dynamic_cast<InitialsCommentShape *>(shape);
        if (initialsShape) {
            initialsShape->setSelectable(false);
        }
    }

    if (m_previouseActiveCommentShape) {
        m_previouseActiveCommentShape->toogleActive();
        m_previouseActiveCommentShape = 0;
    }
}

void CommentTool::mouseReleaseEvent(KoPointerEvent *event)
{
    // disable the previous activeshape
    if (m_previouseActiveCommentShape) {
        m_previouseActiveCommentShape->setActive(false);
    }

    InitialsCommentShape *initialsUnderCursor =
        dynamic_cast<InitialsCommentShape *>(m_canvas->shapeManager()->shapeAt(event->point, KoFlake::ShapeOnTop, false));
    if (initialsUnderCursor) {
        // don't re-activate the shape we just deactivated
        if (m_previouseActiveCommentShape == initialsUnderCursor->parent()) {
            m_previouseActiveCommentShape = 0;
            return;
        }

        CommentShape *commentUnderCursor = dynamic_cast<CommentShape *>(initialsUnderCursor->parent());
        Q_ASSERT(commentUnderCursor);
        commentUnderCursor->setActive(true);

        m_previouseActiveCommentShape = commentUnderCursor;
    }
    event->accept();
}

void CommentTool::mouseMoveEvent(KoPointerEvent *event)
{
    InitialsCommentShape *shapeUnderCursor = dynamic_cast<InitialsCommentShape *>(m_canvas->shapeManager()->shapeAt(event->point, KoFlake::ShapeOnTop, false));
    if (shapeUnderCursor) {
        const QCursor cursor(Qt::PointingHandCursor);
        Q_EMIT useCursor(cursor);
    } else {
        const QCursor cursor(Qt::ArrowCursor);
        Q_EMIT useCursor(cursor);
    }
}

void CommentTool::mousePressEvent(KoPointerEvent * /*event*/)
{
}

void CommentTool::paint(QPainter & /*painter*/, const KoViewConverter & /*converter*/)
{
}
