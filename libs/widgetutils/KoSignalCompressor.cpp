// SPDX-FileCopyrightText: 2013 Dmitry Kazakov <dimula73@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later
// Adapted from Krita's KisSignalCompressor for Calligra.
#include "KoSignalCompressor.h"

KoSignalCompressor::KoSignalCompressor(std::chrono::milliseconds delay, Mode mode, QObject *parent)
    : QObject(parent)
    , m_mode(mode)
{
    m_timer.setInterval(delay);
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, [this] {
        Q_EMIT timeout();
    });
}

bool KoSignalCompressor::isActive() const
{
    return m_timer.isActive();
}

std::chrono::milliseconds KoSignalCompressor::delay() const
{
    return std::chrono::milliseconds(m_timer.interval());
}

void KoSignalCompressor::start()
{
    if (m_mode == Mode::FirstActive && !m_timer.isActive()) {
        Q_EMIT timeout();
    }
    m_timer.start();
}

void KoSignalCompressor::stop()
{
    m_timer.stop();
}

void KoSignalCompressor::setDelay(std::chrono::milliseconds delay)
{
    m_timer.setInterval(delay);
}
