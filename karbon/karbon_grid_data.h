/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef KARBON_GRID_DATA_H
#define KARBON_GRID_DATA_H

#include <QSizeF>
#include <QColor>

class QDomElement;
class QPainter;
class QRectF;
class KoViewConverter;

class KarbonGridData
{
public:
    KarbonGridData();
    ~KarbonGridData();

    void save(QDomElement&, const QString&);
    void load(const QDomElement&, const QString&);
    void paint( QPainter &painter, const KoViewConverter &converter, const QRectF &area );
    void setSnapping( bool enable = true ) { m_isSnap = enable; };
    bool snapping() const { return m_isSnap; };
    void setVisible( bool visible ) { m_isShow = visible; };
    bool visible() const { return m_isShow; };

    void setSpacing( double spacingX, double spacingY );
    void spacing( double *spacingX, double *spacingY ) const;
    double spacingX() const;
    double spacingY() const;

    void setSnap( double snapX, double snapY );
    void snap( double *snapX, double *snapY ) const;
    double snapX() const;
    double snapY() const;

    const QColor& color() const { return m_color; };
    void setColor( const QColor &color ) { m_color = color; };

private:
    QSizeF m_spacing;
    QSizeF m_snap;
    QColor m_color;
    bool m_isSnap;
    bool m_isShow;
};

#endif
