/* This file is part of the KDE project
   Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPRVIEWMODEPREVIEWSHAPEANIMATIONS_H
#define KPRVIEWMODEPREVIEWSHAPEANIMATIONS_H

#include "stage_export.h"

#include <KoPAViewMode.h>
#include "KPrAnimationDirector.h"
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
