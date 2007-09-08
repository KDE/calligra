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

#include <QPixmap>
#include <QTimeLine>

class QPainter;

/**
 * This is the base class for all page effects.
 * It uses an internaly a QTimeLine for calulating the position. The timeline is 
 * feed by the time of an external timeline.
 */
class KPrPageEffect
{
public:
    struct Data
    {
        Data( const QPixmap &oldPage, const QPixmap &newPage, QWidget *w )
        : m_oldPage( oldPage )
        , m_newPage( newPage )
        , m_widget( w )
        , m_finished( false )
        , m_currentTime( 0 )
        , m_lastTime( 0 )
        {}

        QPixmap m_oldPage;
        QPixmap m_newPage;
        QWidget * m_widget;
        QTimeLine m_timeLine;
        bool m_finished;
        int m_currentTime;
        int m_lastTime;
    };

    KPrPageEffect();
    virtual ~KPrPageEffect();

    virtual void setup( const Data &data, QTimeLine &timeLine ) = 0;

    /**
     * Paint the page effect
     *
     * @param painter painter used for painting the effect.
     * @param currentTime The time for which the effect should be painted.
     * @return true if the effect is finished, false otherwise
     */
    virtual bool paint( QPainter &painter, const Data &data ) = 0;

    /**
     * Trigger the next paint paint event.
     *
     * @param currentTime The current time.
     */
    void next( const Data &data );

    /**
     * Finish the the page effect.
     *
     * This only set the m_finish flag to true and triggers an update of the widget.
     */
    void finish( const Data &data );

    /**
     * Get the duration of the page effect.
     *
     * @return The duration of the page effect.
     */
    int duration();

protected:
    int m_duration;
};

#endif // KPRPAGEEFFECT2_H

