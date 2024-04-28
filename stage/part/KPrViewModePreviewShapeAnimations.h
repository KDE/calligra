/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Paul Mendez <paulestebanms@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRVIEWMODEPREVIEWSHAPEANIMATIONS_H
#define KPRVIEWMODEPREVIEWSHAPEANIMATIONS_H

#include "stage_export.h"

#include "KPrAnimationDirector.h"
#include <KoPAViewMode.h>
#include <QTimeLine>

class KPrShapeAnimation;

/**
 * This view mode play a specific shape animation preview
 * and return to the previous view mode after finish.
 */
class STAGE_EXPORT KPrViewModePreviewShapeAnimations : public KoPAViewMode
{
    Q_OBJECT
public:
    KPrViewModePreviewShapeAnimations(KoPAViewBase *view, KoPACanvasBase *canvas);
    ~KPrViewModePreviewShapeAnimations() override;

    void paint(KoPACanvasBase *canvas, QPainter &painter, const QRectF &paintRect) override;
    void tabletEvent(QTabletEvent *event, const QPointF &point) override;
    void mousePressEvent(QMouseEvent *event, const QPointF &point) override;
    void mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point) override;
    void mouseMoveEvent(QMouseEvent *event, const QPointF &point) override;
    void mouseReleaseEvent(QMouseEvent *event, const QPointF &point) override;
    void shortcutOverrideEvent(QKeyEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event, const QPointF &point) override;

    void activate(KoPAViewMode *previousViewMode) override;
    void deactivate() override;

    /// reimplemented
    void updateActivePage(KoPAPageBase *page) override;

    /// Set the shape animation to be played
    void setShapeAnimation(KPrShapeAnimation *shapeAnimation);

    void stopAnimation();

public Q_SLOTS:
    /**
     * @brief Activate the saved view mode
     *
     * This ends the presentation mode. The view mode that was active before the
     * presentation will be restored.
     */
    void activateSavedViewMode();

protected Q_SLOTS:
    void animate();

private:
    KoPAViewMode *m_savedViewMode;
    QTimeLine m_timeLine;
    KPrShapeAnimation *m_shapeAnimation;
    KPrAnimationCache *m_animationCache;
    QRect m_pageRect;
};

#endif // KPRVIEWMODEPREVIEWSHAPEANIMATIONS_H
