/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "Styles_p.h"

#include "TextDebug.h"

StylePrivate::StylePrivate() = default;

StylePrivate::StylePrivate(const StylePrivate &other)

    = default;

StylePrivate &StylePrivate::operator=(const StylePrivate &other) = default;

StylePrivate::~StylePrivate() = default;

StylePrivate::StylePrivate(const QMap<int, QVariant> &other)
    : m_properties(other)
{
}

void StylePrivate::add(int key, const QVariant &value)
{
    m_properties.insert(key, value);
}

void StylePrivate::remove(int key)
{
    m_properties.remove(key);
}

const QVariant StylePrivate::value(int key) const
{
    return m_properties.value(key);
}

bool StylePrivate::contains(int key) const
{
    return m_properties.contains(key);
}

bool StylePrivate::isEmpty() const
{
    return m_properties.isEmpty();
}

void StylePrivate::copyMissing(const StylePrivate &other)
{
    copyMissing(other.m_properties);
}

void StylePrivate::copyMissing(const QMap<int, QVariant> &other)
{
    for (QMap<int, QVariant>::const_iterator it(other.constBegin()); it != other.constEnd(); ++it) {
        if (!m_properties.contains(it.key()))
            m_properties.insert(it.key(), it.value());
    }
}

void StylePrivate::removeDuplicates(const StylePrivate &other)
{
    foreach (int key, other.m_properties.keys()) {
        if (m_properties.value(key) == other.value(key)) {
            m_properties.remove(key);
        }
    }
}

void StylePrivate::removeDuplicates(const QMap<int, QVariant> &other)
{
    foreach (int key, other.keys()) {
        if (m_properties.value(key) == other.value(key))
            m_properties.remove(key);
    }
}

QList<int> StylePrivate::keys() const
{
    return m_properties.keys();
}

bool StylePrivate::operator==(const StylePrivate &other) const
{
    if (other.m_properties.size() != m_properties.size())
        return false;
    foreach (int key, m_properties.keys()) {
        if (m_properties.value(key) != other.value(key))
            return false;
    }
    return true;
}

bool StylePrivate::operator!=(const StylePrivate &other) const
{
    return !operator==(other);
}
