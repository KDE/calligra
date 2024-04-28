/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef TEXTCHANGE_H
#define TEXTCHANGE_H

#include <QString>

class TextChange
{
public:
    TextChange();
    int formerLength() const;
    int length() const;
    int formerPosition() const;
    int position() const;

    TextChange *next()
    {
        return m_next;
    }
    const TextChange *next() const
    {
        return m_next;
    }
    TextChange *previous()
    {
        return m_previous;
    }
    const TextChange *previous() const
    {
        return m_previous;
    }

    QString before() const
    {
        return m_before;
    }
    QString after() const
    {
        return m_after;
    }

    void setPosition(int pos);
    void setOldText(const QString &old);
    void setNewText(const QString &current);
    void setPrevious(TextChange *item);
    void setNext(TextChange *item);
    void move(int length);

    void insertBefore(TextChange *node);
    void insertAfter(TextChange *node);

    void merge(TextChange *other);

private:
    QString m_before, m_after;
    int m_formerPosition, m_position;
    TextChange *m_previous, *m_next;
};

#endif
