/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2016 Camilla Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ListsSpinBox.h"

#include "ListItemsHelper.h"

ListsSpinBox::ListsSpinBox(QWidget *parent)
    : QSpinBox(parent)
{
}

void ListsSpinBox::setNumberFormat(const KoOdfNumberDefinition &format)
{
    m_format = format;
    update();
}

void ListsSpinBox::setLetterSynchronization(bool value)
{
    m_format.setLetterSynchronization(value);
}

int ListsSpinBox::valueFromText(const QString &text) const
{
    Q_UNUSED(text);
    return 0;
}

QString ListsSpinBox::textFromValue(int value) const
{
    return m_format.formattedNumber(value);
}
