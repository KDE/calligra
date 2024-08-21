/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "TextChanges.h"
#include "TextChange.h"

TextChanges::TextChanges()
    : m_root(nullptr)
{
}

TextChanges::~TextChanges()
{
    TextChange *change = m_root;
    while (change) {
        TextChange *prev = change;
        change = change->next();
        delete prev;
    }
    m_root = nullptr;
}

void TextChanges::inserted(int position, const QString &text)
{
    changed(position, QString(), text);
}

void TextChanges::changed(int position, const QString &former, const QString &latter)
{
    TextChange *change = new TextChange();
    change->setPosition(position);
    change->setNewText(latter);
    change->setOldText(former);
    if (m_root == nullptr) {
        m_root = change;
        return;
    }

    TextChange *cursor = m_root;
    while (cursor->next()) {
        if (cursor->position() + cursor->length() >= position)
            break;
        cursor = cursor->next();
    }
    Q_ASSERT(cursor);
    if (cursor->position() > position) { // insert new one before.
        cursor->insertBefore(change);
        if (cursor == m_root)
            m_root = change;
    } else if (position >= cursor->position() && position <= cursor->position() + cursor->length()) { // merge
        cursor->merge(change);
        delete change;
    } else { // insert new one after.
        cursor->insertAfter(change);
        if (change->next())
            change->next()->move(change->length());
    }
}

bool TextChanges::hasText(int position, int length) const
{
    Q_UNUSED(position);
    Q_UNUSED(length);
    return false;
}

QMap<int, const TextChange *> TextChanges::changes() const
{
    QMap<int, const TextChange *> result;
    return result;
}
