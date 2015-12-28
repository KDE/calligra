/* This file is part of the KDE project
 * Copyright (C) 2015 Yue Liu <yue.liu@mail.com>
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

#ifndef FORMULAFONTFAMILYACTION_H
#define FORMULAFONTFAMILYACTION_H

#include <kselectaction.h>

class QIcon;

/**
 * An action to select font family for the formula, only a bundled set of OpenType MATH enabled fonts can be selected.
 */
class FormulaFontFamilyAction : public KSelectAction
{
    Q_OBJECT
    Q_PROPERTY(QString fontFamily READ fontFamily)

public:
    explicit FormulaFontFamilyAction(QObject *parent);
    FormulaFontFamilyAction(const QString &text, QObject *parent);
    FormulaFontFamilyAction(const QIcon &icon, const QString &text, QObject *parent);

    virtual ~FormulaFontFamilyAction();

    QString fontFamily() const;

Q_SIGNALS:
    void fontFamilyChanged(QString);

protected Q_SLOTS:
    /**
     * This function is called whenever an action from the selections is triggered.
     */
    virtual void actionTriggered(QAction* action);

private:
    class Private;
    Private* const d;
};

#endif // FORMULAFONTFAMILYACTION_H
