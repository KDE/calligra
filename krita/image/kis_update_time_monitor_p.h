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

#include "boost/date_time/posix_time/posix_time_types.hpp"

#include <QHash>
#include <QSet>
#include <QFile>
#include <QDebug>
#include <QPointF>
#include <QLineF>
#include <QRect>
#include <QRegion>


inline boost::posix_time::ptime getCurrentTime() {
    return boost::posix_time::microsec_clock::local_time();
}

inline qint64 msecTimeDiff(const boost::posix_time::ptime &a,
                    const boost::posix_time::ptime &b)
{
    boost::posix_time::time_period period(b, a);
    return period.length().total_microseconds() / 1000;
}


class Ticket
{
public:
    Ticket()
        : m_numUpdates(0),
          m_jobFinished(false)
    {
        m_startTime = getCurrentTime();
    }

    qint64 jobTime() const {
        return msecTimeDiff(m_jobTime, m_startTime);
    }

    qint64 totalTime() const {
        return msecTimeDiff(m_totalTime, m_startTime);
    }

    qint32 numUpdates() const {
        return m_numUpdates;
    }

    void updateRequested(const QVector<QRect> &rects, const QRect &bounds) {
        foreach(const QRect &rect, rects) {
            m_dirtyRegion += rect & bounds;
        }
    }

    void notifyJobFinished() {
        m_jobTime = getCurrentTime();
        m_jobFinished = true;
    }

    void notifyRectUpdated(const QRect &rect) {
        if(!m_dirtyRegion.intersects(rect)) return;

        m_dirtyRegion -= rect;

        if(m_dirtyRegion.isEmpty()) {
            m_totalTime = getCurrentTime();
        }
        m_numUpdates++;
    }

    bool isClosed() const {
        return m_jobFinished && m_dirtyRegion.isEmpty();
    }

private:
    boost::posix_time::ptime m_startTime;
    boost::posix_time::ptime m_jobTime;
    boost::posix_time::ptime m_totalTime;

    QRegion m_dirtyRegion;
    qint32 m_numUpdates;
    bool m_jobFinished;
};

class StrokeInfo
{
public:
    StrokeInfo(const QString &name = QString()) {
        m_name = name;
        m_startTime = m_lastMouseActionTime = getCurrentTime();

        m_mousePath = 0;
        m_numTickets = 0;
        m_numUpdates = 0;
        m_grossJobsTime = 0;
        m_grossTotalTime = 0;
        m_mouseSpeedMeasureStarted = false;
        m_finished = false;
    }

    void end() {
        m_finished = true;
    }

    bool jobsFinished() const {
        return m_finished && m_preliminaryTickets.isEmpty();
    }

    bool isFinished() const {
        return m_finished && m_preliminaryTickets.isEmpty() && m_finishedTickets.isEmpty();
    }

    void registerMouseEvent(const QPointF &pos) {
        m_lastMouseActionTime = getCurrentTime();

        if(m_mouseSpeedMeasureStarted) {
            qreal distance = QLineF(m_lastMousePos, pos).length();
            m_mousePath += distance;
        }

        m_mouseSpeedMeasureStarted = true;
        m_lastMousePos = pos;
    }

    void jobAdded(void *key) {
        Ticket *ticket = new Ticket();
        m_preliminaryTickets.insert(key, ticket);
    }

    void updateRequested(void *key, const QVector<QRect> &rects, const QRect &bounds) {
        Ticket *ticket = m_preliminaryTickets.value(key);

        if(!ticket && m_finishedTickets.contains(ticket)) {
            qFatal("ticket has been finished");
        }

        Q_ASSERT(ticket);

        ticket->updateRequested(rects, bounds);
    }

    void jobFinished(void *key) {
        Ticket *ticket = m_preliminaryTickets.take(key);
        Q_ASSERT(ticket);

        ticket->notifyJobFinished();
        m_finishedTickets.insert(ticket);
    }

    void updateFinished(const QRect &rect) {
        foreach(Ticket *ticket, m_finishedTickets) {
            ticket->notifyRectUpdated(rect);

            if(ticket->isClosed()) {
                registerTicket(ticket);
                m_finishedTickets.remove(ticket);
                delete ticket;
            }
        }
    }

    void printValues() {
        QFile logFile(m_name + ".rdata");
        logFile.open(QIODevice::Append);
        QTextStream stream(&logFile);
        stream << mouseSpeed() << "\t"
               << jobsPerUpdate() << "\t"
               << avgJobsTime() << "\t"
               << avgResponseTime() << "\n";
    }

private:
    friend class KisUpdateTimeMonitorTest;
    qreal mouseSpeed() const {
        qint64 strokeTime = msecTimeDiff(m_lastMouseActionTime, m_startTime);
        return strokeTime > 0 ? m_mousePath / strokeTime : 0;
    }

    qreal avgResponseTime() const {
        return qreal(m_grossTotalTime) / m_numTickets;
    }

    qreal avgJobsTime() const {
        return qreal(m_grossJobsTime) / m_numTickets;
    }

    qreal jobsPerUpdate() const {
        return qreal(m_numTickets) / m_numUpdates;
    }

private:
    void registerTicket(const Ticket *ticket) {
        m_numTickets++;
        m_numUpdates += ticket->numUpdates();
        m_grossJobsTime += ticket->jobTime();
        m_grossTotalTime += ticket->totalTime();
    }

private:
    boost::posix_time::ptime m_startTime;
    boost::posix_time::ptime m_lastMouseActionTime;

    bool m_finished;
    QString m_name;

    qint32 m_numTickets;
    qint32 m_numUpdates;
    qint64 m_grossJobsTime;
    qint64 m_grossTotalTime;

    QPointF m_lastMousePos;
    qreal m_mousePath;
    bool m_mouseSpeedMeasureStarted;

    QHash<void*, Ticket*> m_preliminaryTickets;
    QSet<Ticket*> m_finishedTickets;
};
