/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoUpdaterPrivate_p.h"

KoUpdaterPrivate::~KoUpdaterPrivate()
{
    interrupt();
}

void KoUpdaterPrivate::cancel()
{
    m_parent->cancel();
}

void KoUpdaterPrivate::interrupt()
{
    m_interrupted = true;
    Q_EMIT sigInterrupted();
}

void KoUpdaterPrivate::setProgress(int percent)
{
    if (m_progress >= percent) {
        return;
    }
    m_progress = percent;
    Q_EMIT sigUpdated();
}
