/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

/* This file is included by KexiDBLabel and KexiFrame */

//! @todo add more frame types
void ClassName::drawFrame(QPainter *p)
{
    if (frameShape() == QFrame::Box) {
        if (frameShadow() == Plain)
            qDrawPlainRect(p, frameRect(), d->frameColor, lineWidth());
        else
            qDrawShadeRect(p, frameRect(), palette(), frameShadow() == QFrame::Sunken,
                           lineWidth(), midLineWidth());
    } else {
        SuperClassName::drawFrame(p);
    }
}

void ClassName::setPalette(const QPalette &pal)
{
    QPalette pal2(pal);
    pal2.setColor(QPalette::Active, QPalette::Light, KexiUtils::bleachedColor(d->frameColor, 150));
    pal2.setColor(QPalette::Active, QPalette::Mid, d->frameColor);
    pal2.setColor(QPalette::Active, QPalette::Dark, d->frameColor.dark(150));
    pal2.setColor(QPalette::Inactive, QPalette::Light,
                  pal2.color(QPalette::Active, QPalette::Light));
    pal2.setColor(QPalette::Inactive, QPalette::Mid,
                  pal2.color(QPalette::Active, QPalette::Mid));
    pal2.setColor(QPalette::Inactive, QPalette::Dark,
                  pal2.color(QPalette::Active, QPalette::Dark));
    SuperClassName::setPalette(pal2);
}

QColor ClassName::frameColor() const
{
    return d->frameColor;
}

void ClassName::setFrameColor(const QColor& color)
{
    d->frameColor = color;
    //update light and dark colors
    setPalette(palette());
}
