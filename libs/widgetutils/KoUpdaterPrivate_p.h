/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_UPDATERPRIVATE__P_H
#define KO_UPDATERPRIVATE__P_H

#include "KoProgressUpdater.h"

#include <QTime>
#include <QVector>

/**
 * KoUpdaterPrivate is the gui-thread side of KoUpdater. Communication
 * between KoUpdater and KoUpdaterPrivate is handled through queued
 * connections -- this is the main app thread part of the
 * KoUpdater-KoUpdaterPrivate bridge.
 *
 * The gui thread can iterate over its list of KoUpdaterPrivate
 * instances for the total progress computation: the queued signals
 * from the threads will only arrive when the eventloop in the gui
 * thread has a chance to deliver them.
 */
class KoUpdaterPrivate : public QObject
{
    Q_OBJECT

public:
    KoUpdaterPrivate(KoProgressUpdater *parent, int weight, const QString &name)
        : QObject(nullptr)
        , m_progress(0)
        , m_weight(weight)
        , m_interrupted(false)
        , m_hasOutput(parent->hasOutput())
        , m_parent(parent)
    {
        setObjectName(name);
    }

    /// when deleting an updater, make sure the accompanying thread is
    /// interrupted, too.
    ~KoUpdaterPrivate() override;

    bool interrupted() const
    {
        return m_interrupted;
    }

    int progress() const
    {
        return m_progress;
    }

    int weight() const
    {
        return m_weight;
    }

    class TimePoint
    {
    public:
        QTime time;
        int value;

        explicit TimePoint(int value_)
            : time(QTime::currentTime())
            , value(value_)
        {
        }
        TimePoint() = default;
    };

    void addPoint(int value)
    {
        if (m_hasOutput) {
            m_points.append(TimePoint(value));
        }
    }

    const QVector<TimePoint> &getPoints() const
    {
        return m_points;
    }

public Q_SLOTS:

    /// Cancel comes from KoUpdater
    void cancel();

    /// Interrupt comes from the gui, through KoProgressUpdater, goes
    /// to KoUpdater to signal running tasks they might as well quit.
    void interrupt();

    /// progress comes from KoUpdater
    void setProgress(int percent);

Q_SIGNALS:

    /// Emitted whenever the progress changed
    void sigUpdated();

    /// Emitted whenever the parent KoProgressUpdater is interrupted,
    /// for instance through a press on a cancel button
    void sigInterrupted();

private:
    int m_progress; // always in percent
    int m_weight;
    bool m_interrupted;
    bool m_hasOutput;

    KoProgressUpdater *m_parent;
    QVector<TimePoint> m_points;
};

Q_DECLARE_TYPEINFO(KoUpdaterPrivate::TimePoint, Q_MOVABLE_TYPE);

#endif
