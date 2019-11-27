/* This file is part of the KDE project
 * Copyright (  C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KPRVIEWMODEPRESENTATION_H
#define KPRVIEWMODEPRESENTATION_H

#include <KoPAViewMode.h>

#include "KPrAnimationDirector.h"
#include "KPrPresentationTool.h"

class QWidget;
class KPrPresentationTool;
class KPrPresenterViewWidget;
class KPrEndOfSlideShowPage;
class KPrView;

class STAGE_EXPORT KPrViewModePresentation : public KoPAViewMode
{
    Q_OBJECT

public:
    KPrViewModePresentation( KoPAViewBase * view, KoPACanvasBase * m_canvas );
    ~KPrViewModePresentation() override;

    KoViewConverter * viewConverter( KoPACanvasBase * canvas ) override;

    void paint(KoPACanvasBase* canvas, QPainter& painter, const QRectF &paintRect) override;
    void tabletEvent( QTabletEvent *event, const QPointF &point ) override;
    void mousePressEvent( QMouseEvent *event, const QPointF &point ) override;
    void mouseDoubleClickEvent( QMouseEvent *event, const QPointF &point ) override;
    void mouseMoveEvent( QMouseEvent *event, const QPointF &point ) override;
    void mouseReleaseEvent( QMouseEvent *event, const QPointF &point ) override;
    void shortcutOverrideEvent( QKeyEvent *event ) override;
    void keyPressEvent( QKeyEvent *event ) override;
    void keyReleaseEvent( QKeyEvent *event ) override;
    void wheelEvent( QWheelEvent * event, const QPointF &point ) override;
    void closeEvent( QCloseEvent * event ) override;

    void activate( KoPAViewMode * previousViewMode ) override;
    void deactivate() override;

    /// reimplemented
    void updateActivePage( KoPAPageBase *page ) override;

    /**
     * @brief Activate the saved view mode
     *
     * This ends the presentation mode. The view mode that was active before the
     * presentation will be restored.
     */
    void activateSavedViewMode();

    /**
     * @brief Get the animation director
     *
     * Get the animation director used for effects
     *
     * @return animationDirector
     */
    KPrAnimationDirector * animationDirector();

    /**
     * Get the number of pages.
     *
     * This assumes that the presentation is active.
     */
    int numPages() const;

    /**
     * Get the current page number.
     *
     * This assumes that the presentation is active.
     */
    int currentPage() const;

    /**
     * Get the number of steps in this page.
     *
     * This assumes that the presentation is active.
     */
    int numStepsInPage() const;

    /**
     * get the current step.
     *
     * This assumes that the presentation is active.
     */
    int currentStep() const;

    /**
     * get the current Presentation Tool
     *
     * This assumes that the presentation is active.
     */
    KPrPresentationTool * presentationTool() const;

    /**
     * Navigate in the presentation.
     */
    void navigate( KPrAnimationDirector::Navigation navigation );

    Q_INVOKABLE void navigateToPage( int index );
    void blackPresentation();

    /**
     * Returns whether the presentation is activated or not
     */
    bool isActivated();

Q_SIGNALS:

    /**
     * Emitted when the presentation is activated.
     */
    void activated();

    /**
     * Emitted when the presentation is about to be deactivated.
     */
    void deactivated();

    /**
     * Emitted when the page changes.
     *
     * @param page new page index within the current slideshow
     * @param stepsInPage the number of steps in the new page
     */
    void pageChanged( int page, int stepsInPage );

    /**
     * Emitted when the step changes.
     *
     * @param step new step index within the page
     */
    void stepChanged( int step );

protected:
    KoPAViewMode * m_savedViewMode;
    QWidget * m_savedParent;
    KPrPresentationTool * m_tool;
    KPrAnimationDirector * m_animationDirector;
    KPrAnimationDirector * m_pvAnimationDirector;
    KoPACanvas * m_presenterViewCanvas;
    KoPACanvas * m_baseCanvas;
    KPrPresenterViewWidget * m_presenterViewWidget;
    KPrEndOfSlideShowPage * m_endOfSlideShowPage;
    KPrView *m_view;
};

Q_DECLARE_METATYPE(KPrViewModePresentation *)

#endif /* KPRVIEWMODEPRESENTATION_H */
