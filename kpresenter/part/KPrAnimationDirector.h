/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
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
#include <QMap>
#include <QObject>
#include <QTimeLine>

#include <KoZoomHandler.h>

// TODO remove see where it is used
#include "pageeffects/KPrCoverDownEffect.h"

class QPainter;
class QPaintEvent;
class KoViewConverter;
class KoShape;
class KoPACanvas;
class KoPAPageBase;
class KoPAView;
class KPrPageEffect;
class KPrPageEffectRunner;
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

    KPrAnimationDirector( KoPAView * view, const QList<KoPAPageBase*> & pages );
    virtual ~KPrAnimationDirector();

    void paintEvent( QPaintEvent* event );

    KoViewConverter * viewConverter();

    /**
     * do the next step in the presentation
     */
    bool navigate( Navigation navigation );

    /**
     * Check if the shape is shown
     *
     * A shape is visible when there are no animations on it or when it
     * is animated at the moment even when it is a disappear animation. 
     */
    bool shapeShown( KoShape * shape );

    /**
     * Animate the shape by manipulating the painter matrix
     *
     * This checks if the shape has to be animated and if so it 
     * manipulated the painter.
     *
     * @param shape which should be animated
     * @painter painter to manipulate the shape position
     */
    KPrShapeAnimation * shapeAnimation( KoShape * shape );

protected:
    // set the page to be shon and update the UI
    void updateActivePage( KoPAPageBase * page );

    /**
     * Update to the next step
     *
     * @return true if the next step switched to a new page
     */
    bool nextStep();

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

    void animateShapes( int currentTime );

protected slots:
    // update the zoom value
    void updateZoom( const QSize & size );
    // set the animations to the current m_stepIndex
    void updateAnimations();
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
    // TODO remove when we read the effect from the page
    KPrCoverDownEffect m_pageEffect;
    QMap<KoShape *, KPrShapeAnimation *> m_animations;
    QTimeLine m_timeLine;
    int m_pageIndex;
    int m_stepIndex;
    QList<int> m_steps;
    int m_maxShapeDuration;
};

#endif /* KPRANIMATIONDIRECTOR_H */
