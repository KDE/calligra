/*
 *  Copyright (c) 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_update_time_monitor.h"

#include <kglobal.h>
#include <QHash>
#include <QSet>
#include <QMutex>
#include <QMutexLocker>
#include <QPointF>
#include <QLineF>
#include <QRect>
#include <QRegion>
#include <QFile>

#include "kis_debug.h"

#include <time.h>
struct StrokeTicket
{
    struct timespec startTime;
    struct timespec jobDoneTime;

    QRegion dirtyRegion;
};

struct KisUpdateTimeMonitor::Private
{
    Private() : jobsTime(0), responseTime(0), numTickets(0), numUpdates(0) {}

    QHash<void*, StrokeTicket*> preliminaryTickets;
    QSet<StrokeTicket*> finishedTickets;

    qint64 jobsTime;
    qint64 responseTime;
    qint32 numTickets;
    qint32 numUpdates;
    QMutex mutex;

    struct timespec strokeStartTime;
    struct timespec lastStrokeTime;
    qreal mousePath;
    QPointF lastMousePos;

    void processFinishedTicket(StrokeTicket *ticket, struct timespec currentTime);
};

qint64 msecTimeDiff(const struct timespec &a, const struct timespec &b)
{
    qint64 sec_diff = a.tv_sec - b.tv_sec;
    qint64 nsec_diff = a.tv_nsec - b.tv_nsec;
    qint64 msec_diff = nsec_diff / 1000000 + sec_diff * 1000;

    return msec_diff;
}

KisUpdateTimeMonitor::KisUpdateTimeMonitor()
    : m_d(new Private)
{
}

KisUpdateTimeMonitor::~KisUpdateTimeMonitor()
{
    delete m_d;
}

KisUpdateTimeMonitor* KisUpdateTimeMonitor::instance()
{
    K_GLOBAL_STATIC(KisUpdateTimeMonitor, s_instance);
    return s_instance;
}

void KisUpdateTimeMonitor::startStrokeMeasure(const QPointF &startPos)
{
    QMutexLocker locker(&m_d->mutex);

    if(m_d->numTickets) {
        printValues();
    }

    clock_gettime(CLOCK_MONOTONIC, &m_d->strokeStartTime);

    m_d->jobsTime = 0;
    m_d->responseTime = 0;
    m_d->numTickets = 0;
    m_d->numUpdates = 0;
    m_d->mousePath = 0;

    m_d->lastMousePos = startPos;
}

void KisUpdateTimeMonitor::reportMouseMove(const QPointF &pos)
{
    QMutexLocker locker(&m_d->mutex);
    clock_gettime(CLOCK_MONOTONIC, &m_d->lastStrokeTime);

    qreal distance = QLineF(m_d->lastMousePos, pos).length();
    m_d->mousePath += distance;
    m_d->lastMousePos = pos;
}

void KisUpdateTimeMonitor::printValues()
{
    qint64 strokeTime = msecTimeDiff(m_d->lastStrokeTime, m_d->strokeStartTime);
    qreal responseTime = qreal(m_d->responseTime) / m_d->numTickets;
    qreal nonUpdateTime = qreal(m_d->jobsTime) / m_d->numTickets;
    qreal jobsPerUpdate = qreal(m_d->numTickets) / m_d->numUpdates;
    qreal mouseSpeed = qreal(m_d->mousePath) / strokeTime;

    QFile logFile("stroke.rdata");
    logFile.open(QIODevice::Append);
    QTextStream stream(&logFile);
    stream << mouseSpeed << "\t"
           << jobsPerUpdate << "\t"
           << nonUpdateTime << "\t"
           << responseTime << "\n";
    logFile.close();
}

void KisUpdateTimeMonitor::reportJobStarted(void *key)
{
    QMutexLocker locker(&m_d->mutex);

    StrokeTicket *ticket = new StrokeTicket();
    clock_gettime(CLOCK_MONOTONIC, &ticket->startTime);

    m_d->preliminaryTickets.insert(key, ticket);
}

void KisUpdateTimeMonitor::reportJobFinished(void *key, const QVector<QRect> &rects)
{
    QMutexLocker locker(&m_d->mutex);

    StrokeTicket *ticket = m_d->preliminaryTickets.take(key);
    clock_gettime(CLOCK_MONOTONIC, &ticket->jobDoneTime);
    foreach(const QRect &rect, rects) {
        ticket->dirtyRegion += rect;
    }
    m_d->finishedTickets.insert(ticket);
}

void KisUpdateTimeMonitor::reportUpdateFinished(const QRect &rect)
{
    QMutexLocker locker(&m_d->mutex);

    struct timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);

    foreach(StrokeTicket *ticket, m_d->finishedTickets) {
        ticket->dirtyRegion -= rect;
        if(ticket->dirtyRegion.isEmpty()) {
            m_d->processFinishedTicket(ticket, currentTime);
            m_d->finishedTickets.remove(ticket);
            delete ticket;
        }
    }
    m_d->numUpdates++;
}

void KisUpdateTimeMonitor::Private::
processFinishedTicket(StrokeTicket *ticket, struct timespec currentTime)
{
    jobsTime += msecTimeDiff(ticket->jobDoneTime, ticket->startTime);
    responseTime += msecTimeDiff(currentTime, ticket->startTime);
    numTickets++;
}
