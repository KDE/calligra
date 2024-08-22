/* This file is part of the KDE project
 * Copyright (C) Boudewijn Rempt <boud@valdyas.org>, (C) 2007
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoProgressBar.h"

KoProgressBar::KoProgressBar(QWidget *parent)
    : QProgressBar(parent)
{
}

KoProgressBar::~KoProgressBar() = default;

int KoProgressBar::maximum() const
{
    return QProgressBar::maximum();
}

void KoProgressBar::setValue(int value)
{
    QProgressBar::setValue(value);

    if (value >= minimum() && value < maximum()) {
        setVisible(true);
    } else {
        Q_EMIT done();
        setVisible(false);
    }
}

void KoProgressBar::setRange(int minimum, int maximum)
{
    QProgressBar::setRange(minimum, maximum);
}

void KoProgressBar::setFormat(const QString &format)
{
    QProgressBar::setFormat(format);
}
