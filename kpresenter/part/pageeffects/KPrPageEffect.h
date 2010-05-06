/* This file is part of the KDE project
   Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>

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

#include <KoXmlReaderForward.h>
#include "kpresenter_export.h"

class QPainter;
class QGraphicsScene;
class QGraphicsPixmapItem;
class QGraphicsView;
class KoXmlWriter;
class KoGenStyle;
class KPrPageEffectStrategy;

/**
 * This is the base class for all page effects.
 * It uses an internaly a QTimeLine for calulating the position. The timeline is
 * feed by the time of an external timeline.
 */
class KPRESENTER_EXPORT KPrPageEffect
{
public:
    /**
     * Data used by the effect
     *
     * The effect itself contains no status about the effect. All data
     * is kept in this struct. It contains the old and new pixmap, the
     * widget on which the effect is painted and the time values.
     */
    struct Data
    {
        Data( const QPixmap &oldPage, const QPixmap &newPage, QWidget *w )
        : m_oldPage( oldPage )
        , m_newPage( newPage )
        , m_widget( w )
        , m_scene( 0 )
        , m_graphicsView( 0 )
        , m_finished( false )
        , m_currentTime( 0 )
        , m_lastTime( 0 )
        {}

        QPixmap m_oldPage;
        QPixmap m_newPage;
        QWidget * m_widget;
        QTimeLine m_timeLine;
        QGraphicsScene *m_scene;
        QGraphicsView *m_graphicsView;
        QGraphicsPixmapItem *m_oldPageItem;
        QGraphicsPixmapItem *m_newPageItem;
        bool m_finished;
        int m_currentTime;
        int m_lastTime;
    };

    /**
     * Constructor
     *
     * @param duration The duration in milliseconds
     * @param id The id of the page effect
     */
    KPrPageEffect( int duration, const QString & id, KPrPageEffectStrategy * strategy );
    virtual ~KPrPageEffect();

    virtual void setup( const Data &data, QTimeLine &timeLine );
    virtual bool useGraphicsView();
    /**
     * Paint the page effect
     *
     * This should repaint the whole widget. Due to clipping only the
     * relevant parts are repainted.
     *
     * @param painter painter used for painting the effect.
     * @param data The data used for painting the effect.
     * @return true if the effect is finished, false otherwise
     *
     * @see next()
     */
    virtual bool paint( QPainter &painter, const Data &data );

    /**
     * Trigger the next paint paint event.
     *
     * Trigger a repaint of the part of the widget that changed since
     * the last time to this call. The default implementation repaints
     * the full widget.
     *
     * @param data The data used for the effect.
     */
    virtual void next( const Data &data );

    /**
     * Finish the the page effect.
     *
     * Trigger a repaint of the part of the widget that changed since
     * the last call to next. The default implementation repaints the
     * full widget.
     *
     * @param data The data used for the effect.
     */
    virtual void finish( const Data &data );

    /**
     * Get the duration of the page effect.
     *
     * @return The duration of the page effect.
     */
    int duration() const;

    /**
     * Get the id of the page effect.
     *
     * @return id of the page effect
     */
    const QString & id() const;

    /**
     * Get the sub type of the page effect.
     *
     * @return The sub type of the page effect.
     */
    int subType() const;

    /**
     * Save the smil attributes of the effect
     *
     * @param xmlWriter The xml writer used for saving
     */
    void saveOdfSmilAttributes( KoXmlWriter & xmlWriter ) const;

    /**
     * Save transition as part of the style
     */
    void saveOdfSmilAttributes( KoGenStyle & style ) const;

    /**
     * Load effect from odf.
     *
     * Only generic data is loaded here. e.g. the duration of the effect
     * is loaded here
     */
    void loadOdf( const KoXmlElement & element );

protected:
    int m_duration;
    QString m_id;
    KPrPageEffectStrategy * m_strategy;
};

#endif // KPRPAGEEFFECT2_H

