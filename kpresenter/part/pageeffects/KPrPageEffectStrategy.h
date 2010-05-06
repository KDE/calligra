/* This file is part of the KDE project
   Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
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

#ifndef KPRPAGEEFFECTSTRATEGY_H
#define KPRPAGEEFFECTSTRATEGY_H

#include "KPrPageEffectFactory.h"

#include "kpresenter_export.h"

class KPRESENTER_EXPORT KPrPageEffectStrategy
{
public:
    /**
     * Constructor
     *
     * @param subType The sub type used for the UI
     * @param smilType The smil:type used for loading/saving
     * @param smilSubType The smil:subType used for loading/saving
     * @param revers The flag for the smil:direction used for loading/saving true means reverse false means forward
     */
    KPrPageEffectStrategy( int subType, const char * smilType, const char *smilSubType, bool reverse, bool graphicsView = false);
    virtual ~KPrPageEffectStrategy();

    /**
     * Get the sub type of this strategy
     *
     * This is not the smil:subType but the sub type that is used for displaying a common UI string for the effect.
     */
    int subType() const;

    /**
     * Setup the timeline used by this strategy
     */
    virtual void setup( const KPrPageEffect::Data &data, QTimeLine &timeLine ) = 0;

    /**
     * Paint the page effect
     *
     * This should repaint the whole widget. Due to clipping only the
     * relevant parts are repainted.
     *
     * @param p painter used for painting the effect.
     * @param curPos The current position (frame to the current time)
     * @param data The data used for painting the effect.
     *
     * @see next()
     */
    virtual void paintStep( QPainter &p, int curPos, const KPrPageEffect::Data &data ) = 0;

    /**
     * Trigger the next paint paint event.
     *
     * Trigger a repaint of the part of the widget that changed since
     * the last time to this call.
     *
     * @param data The data used for the effect.
     */
    virtual void next( const KPrPageEffect::Data &data ) = 0;

    /**
     * The default implementation triggers an update of the whole widget. If you only need to
     * update a smaller part of the widget reimplement this function.
     */
    virtual void finish( const KPrPageEffect::Data &data );

    /**
     * Save transitions in an xml writer
     */
    void saveOdfSmilAttributes( KoXmlWriter & xmlWriter ) const;

    /**
     * Save transition as part of the style
     */
    void saveOdfSmilAttributes( KoGenStyle & style ) const;

    /**
     * Get the smil:type
     *
     * @return the type used for saving
     */
    const QString & smilType() const;

    /**
     * Get the smil:subtype
     *
     * @return the subtype used for saving
     */
    const QString & smilSubType() const;

    /**
     * Get the smil:direction
     *
     * @return true if smil:direction is reverse, false otherwise
     */
    bool reverse() const;

    /**
    * @return true if we use QGraphicsView, false otherwise
    */
    bool useGraphicsView() const;
private:
    struct SmilData
    {
        SmilData( const char * type, const char * subType, bool reverse )
        : type( type )
        , subType( subType )
        , reverse( reverse )
        {}

        QString type;
        QString subType;
        bool reverse;
    };

    int m_subType;
    SmilData m_smilData;
    bool m_graphicsView;
};

#endif /* KPRPAGEEFFECTSTRATEGY_H */
