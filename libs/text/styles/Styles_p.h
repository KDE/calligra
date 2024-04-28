/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXT_STYLES_PRIVATE_H
#define KOTEXT_STYLES_PRIVATE_H

#include <QMap>
#include <QVariant>

class StylePrivate
{
public:
    StylePrivate();
    StylePrivate(const StylePrivate &other);
    StylePrivate(const QMap<int, QVariant> &other);
    StylePrivate &operator=(const StylePrivate &other);
    ~StylePrivate();

    void add(int key, const QVariant &value);
    void remove(int key);
    const QVariant value(int key) const;
    bool contains(int key) const;
    void copyMissing(const StylePrivate &other);
    void copyMissing(const QMap<int, QVariant> &other);
    void removeDuplicates(const StylePrivate &other);
    void removeDuplicates(const QMap<int, QVariant> &other);
    void clearAll()
    {
        m_properties.clear();
    }
    QList<int> keys() const;
    bool operator==(const StylePrivate &other) const;
    bool operator!=(const StylePrivate &other) const;
    bool isEmpty() const;
    const QMap<int, QVariant> properties() const
    {
        return m_properties;
    }

private:
    QMap<int, QVariant> m_properties;
};

#endif
