/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef TextChanges_H
#define TextChanges_H

#include <QMap>

class TextChange;
class QString;

class TextChanges
{
public:
    TextChanges();
    ~TextChanges();
    void inserted(int position, const QString &text);
    void changed(int position, const QString &former, const QString &latter);

    /// return true if the current text and formatting for the parameter section is already in our database
    bool hasText(int position, int length) const;

    const TextChange *first() const
    {
        return m_root;
    }
    QMap<int, const TextChange *> changes() const;

private:
    QMap<int, TextChange *> m_index;
    TextChange *m_root;
};

#endif
