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
#include <QObject>
#include <QTimeLine>

#include <KoZoomHandler.h>

class QPainter;
class QPaintEvent;
class KoViewConverter;
class KoPACanvas;
class KoPAPageBase;
class KoPAView;
class KPrPageEffect;

class KPrAnimationDirector : public QObject
{
    Q_OBJECT
public:
	KPrAnimationDirector( KoPAView * view, const QList<KoPAPageBase*> & pages );
    virtual ~KPrAnimationDirector();

    void paintEvent( QPaintEvent* event );

    // do the next step in the presentation
    bool navigate();

protected:
    // set the page to be shon and update the UI
    void updateActivePage( KoPAPageBase * page );

    // update to the next step
    void nextStep();

    // paint the given step to the painter
    void paintStep( QPainter & painter );

protected slots:
    // update the zoom value
    void updateZoom( const QSize & size );

private slots:
    // acts on the time line event
    void animate();

private:
    KoPAView * m_view;
    KoPACanvas * m_canvas;
    QList<KoPAPageBase*> m_pages;

    KoZoomHandler m_zoomHandler;
    QPoint m_offset;
    QRect m_pageRect;

    KPrPageEffect * m_pageEffect;
    QTimeLine m_timeLine;
    int m_pageIndex;
};

#endif /* KPRANIMATIONDIRECTOR_H */
