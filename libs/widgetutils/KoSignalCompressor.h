// SPDX-FileCopyrightText: 2013 Dmitry Kazakov <dimula73@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later
// Adapted from Krita's KisSignalCompressor for Calligra.

#pragma once

#include <QObject>
#include <QTimer>
#include <chrono>

#include "kowidgetutils_export.h"

class KOWIDGETUTILS_EXPORT KoSignalCompressor : public QObject
{
    Q_OBJECT
public:
    enum class Mode {
        Postpone,
        FirstActive,
        FirstInactive
    };
    explicit KoSignalCompressor(std::chrono::milliseconds delay, Mode mode = Mode::FirstInactive, QObject *parent = nullptr);
    bool isActive() const;
    std::chrono::milliseconds delay() const;

public Q_SLOTS:
    void start();
    void stop();
    void setDelay(std::chrono::milliseconds delay);

Q_SIGNALS:
    void timeout();

private:
    QTimer m_timer;
    Mode m_mode;
};
