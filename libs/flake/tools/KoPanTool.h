/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOPANTOOL_H
#define KOPANTOOL_H

#include "KoToolBase.h"

#include <QPointF>

class KoCanvasController;

#define KoPanTool_ID "PanTool"

/**
 * This is the tool that allows you to move the canvas by dragging it and 'panning' around.
 */
class KoPanTool : public KoToolBase
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param canvas the canvas this tool works on.
     */
    explicit KoPanTool(KoCanvasBase *canvas);

    /// reimplemented from superclass
    bool wantsAutoScroll() const override;
    /// reimplemented from superclass
    void mousePressEvent(KoPointerEvent *event) override;
    /// reimplemented from superclass
    void mouseMoveEvent(KoPointerEvent *event) override;
    /// reimplemented from superclass
    void mouseReleaseEvent(KoPointerEvent *event) override;
    /// reimplemented from superclass
    void keyPressEvent(QKeyEvent *event) override;
    /// reimplemented from superclass
    void paint(QPainter &, const KoViewConverter &) override
    {
    }
    /// reimplemented from superclass
    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;
    /// reimplemented method
    void customMoveEvent(KoPointerEvent *event) override;
    /// reimplemented method
    void mouseDoubleClickEvent(KoPointerEvent *event) override;

    /// set the canvasController this tool works on.
    void setCanvasController(KoCanvasController *controller)
    {
        m_controller = controller;
    }

private:
    QPointF documentToViewport(const QPointF &p);
    KoCanvasController *m_controller;
    QPointF m_lastPosition;
    bool m_temporary;
    Q_DECLARE_PRIVATE(KoToolBase)
};

#endif
