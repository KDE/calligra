/*
 * SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
            debugWidgets << Q_FUNC_INFO << "found:" << i << ':' << itemText(i);
            setCurrentIndex(i);
            return;
        }
    }
    warnWidgets << Q_FUNC_INFO << "Failed to find:" << font.family();
}
