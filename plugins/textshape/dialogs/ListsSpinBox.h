/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef lISTSSPINBOX_H
#define lISTSSPINBOX_H

#include <KoListStyle.h>

#include <QSpinBox>

#include <KoOdfNumberDefinition.h>

class ListsSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit ListsSpinBox(QWidget *parent = nullptr);

    void setNumberFormat(const KoOdfNumberDefinition &format);
    QString textFromValue(int value) const override;

public Q_SLOTS:
    void setLetterSynchronization(bool value);

private:
    int valueFromText(const QString &text) const override;

    KoOdfNumberDefinition m_format;
};

#endif
