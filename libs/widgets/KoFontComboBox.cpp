/*
 * Copyright (c) 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#include "KoFontComboBox.h"
#include "WidgetsDebug.h"

KoFontComboBox::KoFontComboBox(QWidget *parent)
    : QFontComboBox(parent)
{
}


void KoFontComboBox::setCurrentFont(const QFont &font)
{
    if (font.family().toLower() == currentFont().family().toLower()) {
        return;
    }
    for (int i = 0; i < count(); ++i) {
        if (itemText(i).toLower() == font.family().toLower()) {
            debugWidgets<<Q_FUNC_INFO<<"found:"<<i<<':'<<itemText(i);
            setCurrentIndex(i);
            return;
        }
    }
    warnWidgets<<Q_FUNC_INFO<<"Failed to find:"<<font.family();
}
