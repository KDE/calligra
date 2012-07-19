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

#include "constants.h"

Theme::Theme(QObject* parent)
    : QObject(parent)
{

}

QColor Theme::mainColor() const
{
    return QColor( 0, 0, 64 );
}

QColor Theme::highlightColor() const
{
    return QColor( 220, 220, 220 );
}

QColor Theme::secondaryColor() const
{
    return QColor( 128, 0, 0 );
}

QColor Theme::tertiaryColor() const
{
    return QColor( 0, 128, 0 );
}

QColor Theme::quaternaryColor() const
{
    return QColor( 255, 128, 0 );
}

QColor Theme::textColor() const
{
    return QColor( 50, 50, 50 );
}

QColor Theme::secondaryTextColor() const
{
    return QColor( 175, 175, 175 );
}

Constants::Constants(QObject* parent)
    : QObject(parent)
{
    m_theme = new Theme( this );
}

qreal Constants::gridHeight() const
{
    return 600 / 8;
}

qreal Constants::gridWidth() const
{
    return 1024 / 12;
}

int Constants::gridRows() const
{
    return 8;
}

int Constants::gridColumns() const
{
    return 12;
}

qreal Constants::defaultMargin() const
{
    return 8;
}

QObject* Constants::theme() const
{
    return m_theme;
}

qreal Constants::smallFontSize() const
{
    return 16;
}

qreal Constants::defaultFontSize() const
{
    return 20;
}

qreal Constants::largeFontSize() const
{
    return 24;
}

qreal Constants::hugeFontSize() const
{
    return 46;
}

#include "constants.moc"
