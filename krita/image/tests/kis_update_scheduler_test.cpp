/*
 *  Copyright (c) 2010 Dmitry Kazakov <dimula73@gmail.com>
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

#include "kis_update_scheduler_test.h"
#include <qtest_kde.h>

#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>

#include "scheduler_utils.h"
#include "kis_update_scheduler.h"
#include "kis_updater_context.h"
#include "kis_update_job_item.h"
#include "kis_simple_update_queue.h"

#include "../../sdk/tests/testutil.h"


KisImageSP KisUpdateSchedulerTest::buildTestingImage()
{
    QImage sourceImage1(QString(FILES_DATA_DIR) + QDir::separator() + "hakonepa.png");
    QImage sourceImage2(QString(FILES_DATA_DIR) + QDir::separator() + "inverted_hakonepa.png");

    QRect imageRect = QRect(QPoint(0,0), sourceImage1.size());

    const KoColorSpace * cs = KoColorSpaceRegistry::instance()->rgb8();
    KisImageSP image = new KisImage(0, imageRect.width(), imageRect.height(), cs, "merge test");

    KisFilterSP filter = KisFilterRegistry::instance()->value("blur");
    Q_ASSERT(filter);
    KisFilterConfiguration *configuration = filter->defaultConfiguration(0);
    Q_ASSERT(configuration);

    KisPaintLayerSP paintLayer1 = new KisPaintLayer(image, "paint1", OPACITY_OPAQUE_U8);
    KisPaintLayerSP paintLayer2 = new KisPaintLayer(image, "paint2", OPACITY_OPAQUE_U8 / 3);
    KisLayerSP blur1 = new KisAdjustmentLayer(image, "blur1", configuration, 0);

    paintLayer1->paintDevice()->convertFromQImage(sourceImage1, "", 0, 0);
    paintLayer2->paintDevice()->convertFromQImage(sourceImage2, "", 0, 0);

    image->lock();
    image->addNode(paintLayer1);
    image->addNode(paintLayer2);
    image->addNode(blur1);
    image->unlock();

    return image;
}

void KisUpdateSchedulerTest::testMerge()
{
    KisImageSP image = buildTestingImage();
    QRect imageRect = image->bounds();
    KisNodeSP rootLayer = image->rootLayer();
    KisNodeSP paintLayer1 = rootLayer->firstChild();

    QCOMPARE(paintLayer1->name(), QString("paint1"));


    KisUpdateScheduler scheduler(image);

    /**
     * Test synchronous Full Refresh
     */

    scheduler.fullRefresh(rootLayer, image->bounds(), image->bounds());
    QCOMPARE(rootLayer->exactBounds(), image->bounds());

    QImage resultFRProjection = rootLayer->projection()->convertToQImage(0);
    resultFRProjection.save(QString(FILES_OUTPUT_DIR) + QDir::separator() + "scheduler_fr_merge_result.png");

    /**
     * Test incremental updates
     */

    rootLayer->projection()->clear();

    const qint32 num = 4;
    qint32 width = imageRect.width() / num;
    qint32 lastWidth = imageRect.width() - width;

    QVector<QRect> dirtyRects(num);

    for(qint32 i = 0; i < num-1; i++) {
        dirtyRects[i] = QRect(width*i, 0, width, imageRect.height());
    }
    dirtyRects[num-1] = QRect(width*(num-1), 0, lastWidth, imageRect.height());

    for(qint32 i = 0; i < num; i+=2) {
        scheduler.updateProjection(paintLayer1, dirtyRects[i], image->bounds());
    }

    for(qint32 i = 1; i < num; i+=2) {
        scheduler.updateProjection(paintLayer1, dirtyRects[i], image->bounds());
    }

    scheduler.waitForDone();

    QCOMPARE(rootLayer->exactBounds(), image->bounds());

    QImage resultDirtyProjection = rootLayer->projection()->convertToQImage(0);
    resultDirtyProjection.save(QString(FILES_OUTPUT_DIR) + QDir::separator() + "scheduler_dp_merge_result.png");

    QPoint pt;
    QVERIFY(TestUtil::compareQImages(pt, resultFRProjection, resultDirtyProjection));
}

void KisUpdateSchedulerTest::benchmarkOverlappedMerge()
{
    KisImageSP image = buildTestingImage();
    KisNodeSP rootLayer = image->rootLayer();
    KisNodeSP paintLayer1 = rootLayer->firstChild();
    QRect imageRect = image->bounds();

    QCOMPARE(paintLayer1->name(), QString("paint1"));
    QCOMPARE(imageRect, QRect(0,0,640,441));

    KisUpdateScheduler scheduler(image);

    const qint32 xShift = 10;
    const qint32 yShift = 0;
    const qint32 numShifts = 64;

    QBENCHMARK{
        QRect dirtyRect(0, 0, 200, imageRect.height());

        for(int i = 0; i < numShifts; i++) {
            // qDebug() << dirtyRect;
            scheduler.updateProjection(paintLayer1, dirtyRect, image->bounds());
            dirtyRect.translate(xShift, yShift);
        }

        scheduler.waitForDone();
    }
}

void KisUpdateSchedulerTest::testLocking()
{
    KisImageSP image = buildTestingImage();
    KisNodeSP rootLayer = image->rootLayer();
    KisNodeSP paintLayer1 = rootLayer->firstChild();
    QRect imageRect = image->bounds();

    QCOMPARE(paintLayer1->name(), QString("paint1"));
    QCOMPARE(imageRect, QRect(0,0,640,441));

    KisTestableUpdateScheduler scheduler(image, 2);

    QRect dirtyRect1(0,0,50,100);
    QRect dirtyRect2(0,0,100,100);
    QRect dirtyRect3(50,0,50,100);
    QRect dirtyRect4(150,150,50,50);


    KisTestableUpdaterContext *context = scheduler.updaterContext();
    QVector<KisUpdateJobItem*> jobs;

    scheduler.updateProjection(paintLayer1, imageRect, imageRect);

    jobs = context->getJobs();
    QCOMPARE(jobs[0]->isRunning(), true);
    QCOMPARE(jobs[1]->isRunning(), false);
    QVERIFY(checkWalker(jobs[0]->walker(), imageRect));

    context->clear();

    scheduler.lock();

    scheduler.updateProjection(paintLayer1, dirtyRect1, imageRect);
    scheduler.updateProjection(paintLayer1, dirtyRect2, imageRect);
    scheduler.updateProjection(paintLayer1, dirtyRect3, imageRect);
    scheduler.updateProjection(paintLayer1, dirtyRect4, imageRect);

    jobs = context->getJobs();
    QCOMPARE(jobs[0]->isRunning(), false);
    QCOMPARE(jobs[1]->isRunning(), false);

    scheduler.unlock();

    jobs = context->getJobs();
    QCOMPARE(jobs[0]->isRunning(), true);
    QCOMPARE(jobs[1]->isRunning(), true);
    QVERIFY(checkWalker(jobs[0]->walker(), dirtyRect2));
    QVERIFY(checkWalker(jobs[1]->walker(), dirtyRect4));
}

void KisUpdateSchedulerTest::testExclusiveStrokes()
{
    KisImageSP image = buildTestingImage();
    KisNodeSP rootLayer = image->rootLayer();
    KisNodeSP paintLayer1 = rootLayer->firstChild();
    QRect imageRect = image->bounds();

    QCOMPARE(paintLayer1->name(), QString("paint1"));
    QCOMPARE(imageRect, QRect(0,0,640,441));

    QRect dirtyRect1(0,0,50,100);

    KisTestableUpdateScheduler scheduler(image, 2);
    KisTestableUpdaterContext *context = scheduler.updaterContext();
    QVector<KisUpdateJobItem*> jobs;

    scheduler.updateProjection(paintLayer1, dirtyRect1, imageRect);

    jobs = context->getJobs();
    QCOMPARE(jobs[0]->isRunning(), true);
    QCOMPARE(jobs[1]->isRunning(), false);
    QVERIFY(checkWalker(jobs[0]->walker(), dirtyRect1));

    scheduler.startStroke(new KisTestingStrokeStrategy("excl_", true, false));

    jobs = context->getJobs();
    QCOMPARE(jobs[0]->isRunning(), true);
    QCOMPARE(jobs[1]->isRunning(), false);
    QVERIFY(checkWalker(jobs[0]->walker(), dirtyRect1));

    context->clear();
    scheduler.endStroke();

    jobs = context->getJobs();
    QCOMPARE(jobs[0]->isRunning(), true);
    QCOMPARE(jobs[1]->isRunning(), false);
    COMPARE_NAME(jobs[0], "excl_init");

    scheduler.updateProjection(paintLayer1, dirtyRect1, imageRect);

    jobs = context->getJobs();
    QCOMPARE(jobs[0]->isRunning(), true);
    QCOMPARE(jobs[1]->isRunning(), false);
    COMPARE_NAME(jobs[0], "excl_init");
}

#include "kis_update_time_monitor.h"

void KisUpdateSchedulerTest::testTimeMonitor()
{
    QVector<QRect> dirtyRects;

    KisUpdateTimeMonitor::instance()->startStrokeMeasure(QPointF());
    KisUpdateTimeMonitor::instance()->reportMouseMove(QPointF(100, 0));

    KisUpdateTimeMonitor::instance()->reportJobStarted((void*) 10);
    QTest::qSleep(300);
    KisUpdateTimeMonitor::instance()->reportJobStarted((void*) 20);
    QTest::qSleep(100);
    dirtyRects << QRect(10,10,10,10);
    KisUpdateTimeMonitor::instance()->reportJobFinished((void*) 10, dirtyRects);
    QTest::qSleep(100);
    dirtyRects.clear();
    dirtyRects << QRect(30,30,10,10);
    KisUpdateTimeMonitor::instance()->reportJobFinished((void*) 20, dirtyRects);
    QTest::qSleep(500);
    KisUpdateTimeMonitor::instance()->reportUpdateFinished(QRect(10,10,10,10));
    QTest::qSleep(300);
    KisUpdateTimeMonitor::instance()->reportUpdateFinished(QRect(30,30,10,10));

    KisUpdateTimeMonitor::instance()->reportMouseMove(QPointF(130, 0));

    KisUpdateTimeMonitor::instance()->startStrokeMeasure(QPointF(0,0));
}


QTEST_KDEMAIN(KisUpdateSchedulerTest, NoGUI)
#include "kis_update_scheduler_test.moc"

