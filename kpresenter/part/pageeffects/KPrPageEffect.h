/* This file is part of the KDE project
   Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef KPRPAGEEFFECT_H
#define KPRPAGEEFFECT_H

#include <QPoint>
#include <QTimeLine>
#include <QPixmap>

class QPainter;
class QWidget;

/**
 * This is the base class for all page effects.
 * It uses an internaly a QTimeLine for calulating the position. The timeline is 
 * feed by the time of an external timeline.
 */
class KPrPageEffect
{
public:
	KPrPageEffect( const QPixmap &px1, const QPixmap &px2, QWidget * w );
    virtual ~KPrPageEffect();

    /**
     * Paint the page effect
     *
     * @param painter painter used for painting the effect.
     * @param currentTime The time for which the effect should be painted.
     * @return true if the effect is finished, false otherwise
     */
    virtual bool paint( QPainter &painter, int currentTime ) = 0;

    /**
     * Trigger the next paint paint event.
     *
     * @param currentTime The current time.
     */
    void next( int currentTime );

    /**
     * Finish the the page effect.
     *
     * This only set the m_finish flag to true and triggers an update of the widget.
     */
    void finish();

    bool isFinished();

    /**
     * Get the duration of the page effect.
     *
     * @return The duration of the page effect.
     */
    int duration();

protected:    
    QPixmap m_px1;
    QPixmap m_px2;
    QWidget * m_widget;

    bool m_finish;
    QPoint m_lastPos;
    QTimeLine m_timeLine;
};

#endif // KPRPAGEEFFECT_H
