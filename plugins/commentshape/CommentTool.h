/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef COMMENTTOOL_H
#define COMMENTTOOL_H

#include <KoToolBase.h>

class CommentShape;

class CommentTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit CommentTool(KoCanvasBase *canvas);
    virtual ~CommentTool();

    virtual void activate(KoToolBase::ToolActivation toolActivation, const QSet<KoShape *> &shapes);
    virtual void deactivate();

    virtual void mouseReleaseEvent(KoPointerEvent *event);
    virtual void mouseMoveEvent(KoPointerEvent *event);
    virtual void mousePressEvent(KoPointerEvent *event);
    virtual void paint(QPainter &painter, const KoViewConverter &converter);

private:
    KoCanvasBase *m_canvas;
    CommentShape *m_previouseActiveCommentShape;
    bool m_temporary;
};

#endif // COMMENTTOOL_H
