/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "TextChange.h"

TextChange::TextChange()
    : m_formerPosition(0)
    , m_position(0)
    , m_previous(nullptr)
    , m_next(nullptr)
{
}

int TextChange::length() const
{
    return m_after.length() - m_before.length();
}

int TextChange::formerPosition() const
{
    return m_formerPosition;
}

int TextChange::position() const
{
    return m_position;
}

int TextChange::formerLength() const
{
    // TODO
    return -1;
}

void TextChange::setPosition(int pos)
{
    m_position = pos;
    m_formerPosition = pos;
}

void TextChange::setOldText(const QString &old)
{
    m_before = old;
}

void TextChange::setNewText(const QString &current)
{
    m_after = current;
}

void TextChange::setPrevious(TextChange *item)
{
    m_previous = item;
}

void TextChange::setNext(TextChange *item)
{
    m_next = item;
}

void TextChange::move(int length)
{
    m_position += length;
    if (m_next)
        m_next->move(length);
}

void TextChange::insertBefore(TextChange *node)
{
    move(node->length());
    node->setPrevious(previous());
    node->setNext(this);
    setPrevious(node);
    if (node->previous())
        node->previous()->setNext(node);
}

void TextChange::insertAfter(TextChange *node)
{
    node->setPrevious(this);
    node->setNext(next());
    setNext(node);
    if (node->next())
        node->next()->setPrevious(node);
}

void TextChange::merge(TextChange *other)
{
    // make sure the start of 'other' is within this change instance
    Q_ASSERT(other->position() >= position() && other->position() <= position() + length());

    /// this only does very simple merging for now.
    m_after.insert(other->position() - m_position, other->after());
}
