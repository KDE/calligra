/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOZOOMTOOL_H
#define KOZOOMTOOL_H

#include "KoInteractionTool.h"

#include <QCursor>

class KoCanvasBase;
class KoCanvasController;

/// \internal
class KoZoomTool : public KoInteractionTool
{
    Q_OBJECT
public:
    /**
     * Create a new tool; typically not called by applications, only by the KoToolManager
     * @param canvas the canvas this tool works for.
     */
    explicit KoZoomTool(KoCanvasBase *canvas);
    /// reimplemented method
    void wheelEvent(KoPointerEvent *event) override;
    /// reimplemented method
    void mouseReleaseEvent(KoPointerEvent *event) override;
    /// reimplemented method
    void mouseMoveEvent(KoPointerEvent *event) override;
    /// reimplemented method
    void keyPressEvent(QKeyEvent *event) override;
    /// reimplemented method
    void keyReleaseEvent(QKeyEvent *event) override;
    /// reimplemented method
    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;
    /// reimplemented method
    void mouseDoubleClickEvent(KoPointerEvent *event) override;

    void setCanvasController(KoCanvasController *controller)
    {
        m_controller = controller;
    }

    void setZoomInMode(bool zoomIn);

protected:
    QWidget *createOptionWidget() override;

private:
    KoInteractionStrategy *createStrategy(KoPointerEvent *event) override;

    void updateCursor(bool swap);

    KoCanvasController *m_controller;
    QCursor m_inCursor;
    QCursor m_outCursor;
    bool m_temporary;
    bool m_zoomInMode;
};

#endif
