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

#include "kis_update_time_monitor_test.h"

#include <qtest_kde.h>
#include "kis_update_time_monitor.h"
#include "kis_update_time_monitor_p.h"

template<typename T1, typename T2>
inline bool compare(T1 val, T2 ref, qreal tol)
{
    T1 realRef = static_cast<T1>(ref);
    bool result = qAbs(realRef - val) <= tol * realRef;

    if(!result) {
        qDebug() << "Actual:" << val;
        qDebug() << "Expected:" << realRef;
        qDebug() << "Tolerance (%):" << tol * 100;
    }

    return result;
}

void KisUpdateTimeMonitorTest::testTickets()
{
    QRect bounds(0,0,150,150);
    QVector<QRect> rects;
    rects << QRect(0,0,50,50);
    rects << QRect(100,100,100,100);

    Ticket ticket;

    QTest::qSleep(50);
    ticket.updateRequested(rects, bounds);
    ticket.notifyJobFinished();
    QTest::qSleep(70);
    ticket.notifyRectUpdated(QRect(0,0,100,100));
    QTest::qSleep(40);
    ticket.notifyRectUpdated(QRect(100,100,50,500));

    QVERIFY(ticket.isClosed());
    QVERIFY(compare(ticket.jobTime(), 50, 0.1)); // tolerance 10%
    QVERIFY(compare(ticket.totalTime(), 160, 0.1)); // tolerance 10%
    QCOMPARE(ticket.numUpdates(), 2);
}

void KisUpdateTimeMonitorTest::testStrokeInfoMouseSpeed()
{
    StrokeInfo info;
    // cold init
    info.registerMouseEvent(QPointF());

    QTest::qSleep(30);
    info.registerMouseEvent(QPointF(50, 0));
    QTest::qSleep(30);
    info.registerMouseEvent(QPointF(50, 50));
    QTest::qSleep(30);
    info.registerMouseEvent(QPointF(0, 50));
    QTest::qSleep(30);
    info.registerMouseEvent(QPointF(0, 0));

    QVERIFY(compare(info.mouseSpeed(), 5.0/3, 0.05)); // tolerance 5%
}

void KisUpdateTimeMonitorTest::testStrokeInfoResponse()
{
    QRect bounds(0,0,150,150);
    QVector<QRect> rects1;
    rects1 << QRect(0,0,50,50);
    rects1 << QRect(100,100,100,100);
    QVector<QRect> rects2;
    rects2 << QRect(50,0,50,50);
    rects2 << QRect(0,50,50,50);

    StrokeInfo info;

    QTest::qSleep(30);
    info.jobAdded((void*) 1);
    QTest::qSleep(30);
    info.jobAdded((void*) 2);
    QTest::qSleep(30);
    info.updateRequested((void*) 1, rects1, bounds);
    info.jobFinished((void*) 1);
    QTest::qSleep(30);
    info.updateRequested((void*) 2, rects2, bounds);
    info.jobFinished((void*) 2);

    QTest::qSleep(30);
    info.updateFinished(QRect(0,0,50,50));
    QTest::qSleep(30);
    info.updateFinished(QRect(50,0,50,50));
    QTest::qSleep(30);

    QVERIFY(!info.isFinished());
    info.end();
    QVERIFY(!info.isFinished());

    QTest::qSleep(30);
    info.updateFinished(QRect(0,50,50,50));
    QTest::qSleep(30);
    info.updateFinished(QRect(100,100,50,50));

    QVERIFY(info.isFinished());

    QCOMPARE(info.jobsPerUpdate(), 0.5);

    QVERIFY(compare(info.avgJobsTime(), 60, 0.05)); // tolerance 5%
    QVERIFY(compare(info.avgResponseTime(), 210, 0.05)); // tolerance 5%
}

void KisUpdateTimeMonitorTest::testTimeMonitor()
{
    QRect bounds(0,0,150,150);
    QVector<QRect> rects1;
    rects1 << QRect(0,0,50,50);
    rects1 << QRect(100,100,100,100);
    QVector<QRect> rects2;
    rects2 << QRect(50,0,50,50);
    rects2 << QRect(0,50,50,50);

    QFile logFile("test.rdata");
    logFile.remove();

    KisUpdateTimeMonitor *monitor = KisUpdateTimeMonitor::instance();

    monitor->setBounds(bounds);

    monitor->startStroke("test");
    monitor->jobAdded((void*) 1);
    monitor->endStroke();

    QTest::qSleep(30);

    monitor->startStroke("test");
    monitor->jobAdded((void*) 2);

    QTest::qSleep(30);

    // <-- interleaving start

    monitor->jobThreadStarted((void*) 1);
    monitor->jobThreadUpdateRequested(rects1);
    monitor->jobThreadFinished();

    QTest::qSleep(30);
    monitor->updateFinished(QRect(0,0,50,50));
    QTest::qSleep(30);
    monitor->updateFinished(QRect(100,100,50,50));
    QTest::qSleep(30);

    // interleaving ends -->

    monitor->jobThreadStarted((void*) 2);
    monitor->jobThreadUpdateRequested(rects2);
    monitor->jobThreadFinished();

    QTest::qSleep(30);

    monitor->updateFinished(QRect(50,0,50,50));
    QTest::qSleep(30);

    monitor->endStroke();

    monitor->updateFinished(QRect(0,50,50,50));
    QTest::qSleep(30);

    logFile.open(QIODevice::ReadOnly);
    QTextStream stream(&logFile);
    QString line;

    line = stream.readLine();
    QCOMPARE(line, QString("0\t0.5\t60\t120"));
    line = stream.readLine();
    QCOMPARE(line, QString("0\t0.5\t120\t180"));

    logFile.remove();
}

QTEST_KDEMAIN(KisUpdateTimeMonitorTest, GUI)
