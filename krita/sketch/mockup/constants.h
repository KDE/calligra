/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QObject>
#include <QColor>

class Theme : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QColor MainColor READ mainColor CONSTANT )
    Q_PROPERTY( QColor HighlightColor READ highlightColor CONSTANT )
    Q_PROPERTY( QColor SecondaryColor READ secondaryColor CONSTANT )
    Q_PROPERTY( QColor TertiaryColor READ tertiaryColor CONSTANT )
    Q_PROPERTY( QColor QuaternaryColor READ quaternaryColor CONSTANT )
    Q_PROPERTY( QColor TextColor READ textColor CONSTANT )
    Q_PROPERTY( QColor SecondaryTextColor READ secondaryTextColor CONSTANT )
    Q_PROPERTY( QColor PositiveColor READ positiveColor CONSTANT )
    Q_PROPERTY( QColor NegativeColor READ negativeColor CONSTANT )

public:
    Theme( QObject* parent = 0 );

    QColor mainColor() const;
    QColor highlightColor() const;
    QColor secondaryColor() const;
    QColor tertiaryColor() const;
    QColor quaternaryColor() const;
    QColor textColor() const;
    QColor secondaryTextColor() const;
    QColor positiveColor() const;
    QColor negativeColor() const;
};

class Constants : public QObject
{
    Q_OBJECT
    Q_PROPERTY( int GridRows READ gridRows CONSTANT )
    Q_PROPERTY( int GridColumns READ gridColumns CONSTANT )
    Q_PROPERTY( QObject* Theme READ theme CONSTANT )

    Q_PROPERTY( qreal DefaultMargin READ defaultMargin NOTIFY gridSizeChanged )
    Q_PROPERTY( qreal GridHeight READ gridHeight NOTIFY gridSizeChanged )
    Q_PROPERTY( qreal GridWidth READ gridWidth NOTIFY gridSizeChanged )
    Q_PROPERTY( qreal SmallFontSize READ smallFontSize NOTIFY gridSizeChanged )
    Q_PROPERTY( qreal DefaultFontSize READ defaultFontSize NOTIFY gridSizeChanged )
    Q_PROPERTY( qreal LargeFontSize READ largeFontSize NOTIFY gridSizeChanged )
    Q_PROPERTY( qreal HugeFontSize READ hugeFontSize NOTIFY gridSizeChanged )

public:
    Constants( QObject* parent = 0 );

    qreal gridHeight() const;
    qreal gridWidth() const;
    int gridRows() const;
    int gridColumns() const;
    qreal defaultMargin() const;
    QObject* theme() const;
    qreal smallFontSize() const;
    qreal defaultFontSize() const;
    qreal largeFontSize() const;
    qreal hugeFontSize() const;

    void setGridWidth( qreal width );
    void setGridHeight( qreal height );

Q_SIGNALS:
    void gridSizeChanged();

private:
    Theme* m_theme;
    qreal m_gridWidth;
    qreal m_gridHeight;
};

#endif // CONSTANTS_H
