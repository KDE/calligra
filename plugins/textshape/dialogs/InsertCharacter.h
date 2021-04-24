/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef INSERTCHARACTER_H
#define INSERTCHARACTER_H

#include <QDockWidget>
class KCharSelect;

class InsertCharacter : public QDockWidget
{
    Q_OBJECT
public:
    explicit InsertCharacter(QWidget *parent);

Q_SIGNALS:
    void insertCharacter(const QString &character);

private Q_SLOTS:
    void insertCharacter();

private:
    KCharSelect *m_charSelector;
};

#endif
