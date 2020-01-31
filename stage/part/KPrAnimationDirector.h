/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
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

#ifndef KPRANIMATIONDIRECTOR_H
#define KPRANIMATIONDIRECTOR_H

#include <QList>
#include <QObject>
#include <QPair>
#include <QTimeLine>
#include <QTransform>
#include <QTimer>

#include <KoZoomHandler.h>
#include "KPrShapeAnimations.h"

class QPainter;
class QPaintEvent;
class KoViewConverter;
class KoShape;
class KoPACanvas;
class KoPAPageBase;
class KoPAView;
class KPrPageEffect;
class KPrPageEffectRunner;
class KPrPage;
class KPrShapeAnimation;


class KPrAnimationDirector : public QObject
{
    Q_OBJECT
public:
    enum Navigation
    {
        FirstPage,
        PreviousPage,
        PreviousStep,
        NextStep,
        NextPage,
        LastPage
    };

    enum State
    {
        PresentationState,
        EntryEffectState,
        EntryAnimationState
    };

    KPrAnimationDirector( KoPAView * view, KoPACanvas * canvas, const QList<KoPAPageBase*> & pages, KoPAPageBase* currentPage );
    ~KPrAnimationDirector() override;

    void paint(QPainter& painter, const QRectF &paintRect);
    void paintEvent( QPaintEvent* event );

    KoViewConverter * viewConverter();

    /**
     * get the number of pages
     */
    int numPages() const;

    /**
     * get the current page number
     */
    int currentPage() const;

    /**
     * get the number of steps in this page
     */
    int numStepsInPage() const;

    /**
     * get the current step
     */
    int currentStep() const;

    /**
     * do the next step in the presentation
     *
     * @return true if slideshow is finished and should be exited, false otherwise
     */
    bool navigate( Navigation navigation );

    /**
     * do the navigation to the page specified
     */
    void navigateToPage( int index );

    void deactivate();

    KoPAPageBase *page(int index) const;

protected:
    // set the page to be shon and update the UI
    void updateActivePage( KoPAPageBase * page );

    /**
     * change the page
     *
     * @return true if slideshow is finished and should be exited, false otherwise
     */
    bool changePage( Navigation navigation );

    /**
     * Update to the next step
     *
     * @return true if slideshow is finished and should be exited, false otherwise
     */
    bool nextStep();

    /**
     * Update to the previous step
     */
    void previousStep();

    // paint the given step to the painter
    void paintStep( QPainter & painter );

    /**
     * Finish the running shape animations
     */
    void finishAnimations();

    /**
     * Start the timeline
     */
    void startTimeLine( int duration );

    // helper method for freeing the resources of the animations
    void clearAnimations();
    // check if there is a set animation in m_animations
    bool hasAnimation() const;
    bool animationRunning() const;
    bool moreAnimationSteps() const;

    bool hasPageEffect() const;
    bool pageEffectRunning() const;

    bool hasAutoSlideTransition() const;
    void startAutoSlideTransition();

    void updatePageAnimation();
    void updateStepAnimation();

protected Q_SLOTS:
    // update the zoom value
    void updateZoom( const QSize & size );
    // acts on the time line event
    void animate();

    void nextPage();
    void slotTimelineFinished();

private:
    KoPAView * m_view;
    KoPACanvas * m_canvas;
    QList<KoPAPageBase*> m_pages;

    KoZoomHandler m_zoomHandler;
    QPoint m_offset;
    QRect m_pageRect;

    KPrPageEffectRunner * m_pageEffectRunner;
    QList<KPrAnimationStep *> m_animations;
    QTimeLine m_timeLine;
    int m_pageIndex;
    int m_stepIndex;
    int m_maxShapeDuration;
    // true when there is an animation in this step
    bool m_hasAnimation;
    KPrAnimationCache * m_animationCache;

    State m_state;
    QTimer m_autoTransitionTimer;
};

#endif /* KPRANIMATIONDIRECTOR_H */
