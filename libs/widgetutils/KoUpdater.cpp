/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoUpdater.h"

#include "KoUpdaterPrivate_p.h"

KoUpdater::KoUpdater(KoUpdaterPrivate *p)
    : QObject(p)
    , m_progressPercent(0)
{
    d = p;
    Q_ASSERT(p);
    Q_ASSERT(!d.isNull());

    connect(this, &KoUpdater::sigCancel, d.data(), &KoUpdaterPrivate::cancel);
    connect(this, &KoUpdater::sigProgress, d.data(), &KoUpdaterPrivate::setProgress);
    connect(d.data(), &KoUpdaterPrivate::sigInterrupted, this, &KoUpdater::interrupt);

    setRange(0, 100);
    m_interrupted = false;
}

void KoUpdater::cancel()
{
    Q_EMIT sigCancel();
}

void KoUpdater::setProgress(int percent)
{
    if (m_progressPercent >= percent) {
        return;
    }
    d->addPoint(percent);

    m_progressPercent = percent;

    Q_EMIT sigProgress(percent);
}

int KoUpdater::progress() const
{
    return m_progressPercent;
}

bool KoUpdater::interrupted() const
{
    return m_interrupted;
}

int KoUpdater::maximum() const
{
    return 100;
}

void KoUpdater::setValue(int value)
{
    if (value < min)
        value = min;
    if (value > max)
        value = max;
    // Go from range to percent
    if (range == 0)
        return;
    setProgress(((100 * value) / range) + 1);
}

void KoUpdater::setRange(int minimum, int maximum)
{
    min = minimum - 1;
    max = maximum;
    range = max - min;
}

void KoUpdater::setFormat(const QString &format)
{
    Q_UNUSED(format);
    // XXX: Do nothing
}

void KoUpdater::interrupt()
{
    m_interrupted = true;
}
