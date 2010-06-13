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

#include <KoZoomHandler.h>
#include "KPrShapeAnimations.h"

class QPainter;
class QPaintEvent;
class KoViewConverter;
class KoShape;
class KoShapeManager;
class KoPACanvas;
class KoPAPageBase;
class KoPAView;
class KPrPageEffect;
class KPrPageEffectRunner;
class KPrAnimationData;
class KPrPage;
class KPrPageData;
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

    KPrAnimationDirector( KoPAView * view, KoPACanvas * canvas, const QList<KoPAPageBase*> & pages, KoPAPageBase* currentPage );
    virtual ~KPrAnimationDirector();

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

    /**
     * Check if the shape is shown
     *
     * A shape is visible when there are no animations on it or when it
     * is animated at the moment even when it is a disappear animation.
     */
    bool shapeShown( KoShape * shape );

    /**
     * Get the animation for the shape
     *
     * @param shape which should be animated
     * @return pair of the animation and the animation data for the shape or a 0, 0 if there is no animation
     */
    KPrShapeAnimation shapeAnimation( KoShape * shape );

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
    bool hasAnimation();

    void updatePageAnimation();
    void updateStepAnimation();

protected slots:
    // update the zoom value
    void updateZoom( const QSize & size );
    // acts on the time line event
    void animate();

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
    // true when there is an animtion in this step
    bool m_hasAnimation;
    KPrAnimationCache * m_animationCache;
};

#endif /* KPRANIMATIONDIRECTOR_H */
