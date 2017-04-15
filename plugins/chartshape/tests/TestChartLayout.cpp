/* This file is part of the KDE project
 * 
 * Copyright 2017 Dag Andersen <danders@get2net.dk>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "TestChartLayout.h"
#include "ChartShape.h"
#include "ChartLayout.h"
#include "PlotArea.h"
#include "Axis.h"
#include "Legend.h"

#include "KoDocumentResourceManager.h"
#include "KoShape.h"

#include <QTest>
#include <QTransform>
#include <QMap>
#include <QDebug>

using namespace KoChart;

static void filterMessages(QtMsgType type, const QMessageLogContext &/*context*/, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
        case QtDebugMsg:
//             fprintf(stderr, "%s\n%s\n%s\n%d\n%d\n", context.category, context.file, context.function, context.version, context.line);
            break;
        default:
            fprintf(stderr, "%s\n", localMsg.constData());
            break;
    }
}

static void updateItems(const QMap<KoShape*, QRectF> &map)
{
    QMap<KoShape*, QRectF>::const_iterator it;
    for (it = map.constBegin(); it != map.constEnd(); ++it) {
        it.key()->setPosition(it.value().topLeft());
        it.key()->setSize(it.value().size());
    }
}

static QRectF itemRect(const KoShape *shape)
{
    return ChartLayout::itemRect(shape);
}

static void setItemPosition(KoShape *shape, const QPointF& pos)
{
    ChartLayout::setItemPosition(shape, pos);
}


TestChartLayout::TestChartLayout()
{
}

void TestChartLayout::initTestCase()
{
    msgHandler = qInstallMessageHandler(filterMessages);

    drm = new KoDocumentResourceManager(); // used throughout the test

    // Just used to check default layout
    // If changed tests may have to be changed too
    init();

    QVERIFY(plotArea->isVisible());
    QVERIFY(legend->isVisible());
    QVERIFY(!chartTitle->isVisible());
    QVERIFY(!chartSubTitle->isVisible());
    QVERIFY(!chartFooter->isVisible());
    QVERIFY(plotArea->xAxis() != 0 && xAxisTitle != 0);
    QVERIFY(plotArea->yAxis() != 0 && yAxisTitle != 0);
    QVERIFY(xAxisTitle->isVisible());
    QVERIFY(yAxisTitle->isVisible());
    QVERIFY(secondaryXAxisTitle == 0);
    QVERIFY(secondaryYAxisTitle == 0);

    qInstallMessageHandler(msgHandler);

    QCOMPARE(xAxisTitleRect.bottom(), chart->size().height() - layout->margins().y());
    QCOMPARE(yAxisTitleRect.left(), layout->margins().x());
    QCOMPARE(legendRect.right(), chart->size().width() - layout->margins().x());
    QCOMPARE(plotAreaRect.top(), layout->margins().y());
    QCOMPARE(plotAreaRect.left(), yAxisTitleRect.right() + layout->spacing().x());
    QCOMPARE(plotAreaRect.bottom(), xAxisTitleRect.top() - layout->spacing().x());
    QCOMPARE(plotAreaRect.right(), legendRect.left() - layout->spacing().x());

    cleanup();
}

void TestChartLayout::cleanupTestCase()
{
    delete drm;
}

void TestChartLayout::init()
{
    qInstallMessageHandler(filterMessages);

    chart = new ChartShape(drm);
    layout = chart->layout();
    layout->setAutoLayoutEnabled(true);
    chart->setSize(QSizeF(300., 200.));
    layout->layout();
    layout->setAutoLayoutEnabled(false);

//     qInfo()<<(KoShape*)chart<<chart->size();
//     debug(chart);

    chartTitle = chart->title();
    chartTitleRect = itemRect(chartTitle);
    chartSubTitle = chart->subTitle();
    chartSubTitleRect = itemRect(chartSubTitle);
    chartFooter = chart->footer();
    chartFooterRect = itemRect(chartFooter);
    plotArea = chart->plotArea();
    plotAreaRect = itemRect(plotArea);
    legend = chart->legend();
    legendRect = itemRect(legend);
    xAxisTitle = plotArea->xAxis()->title();
    xAxisTitleRect = itemRect(xAxisTitle);
    yAxisTitle = plotArea->yAxis()->title();
    yAxisTitleRect = itemRect(yAxisTitle);

    secondaryXAxisTitle = plotArea->secondaryXAxis() ? plotArea->secondaryXAxis()->title() : 0;
    secondaryXAxisTitleRect = secondaryXAxisTitle ? itemRect(secondaryXAxisTitle) : QRectF();
    secondaryYAxisTitle = plotArea->secondaryYAxis() ? plotArea->secondaryYAxis()->title() : 0;
    secondaryYAxisTitleRect = secondaryYAxisTitle ? itemRect(secondaryYAxisTitle) : QRectF();

    qInstallMessageHandler(msgHandler);
}

void TestChartLayout::cleanup()
{
    delete chart;
}

void TestChartLayout::testRelativePositioning() {
    qreal start1 = 10.0;
    qreal length1 = 40.0;
    qreal start2 = 10.0;
    qreal length2 = 60.0;
    qreal start = 5.0;
    qreal length = 10.0;
    qreal pos;

    qInfo()<<"Test resize only (start1 == start2)";

    qInfo()<<"Test start before start1";
    pos = ChartLayout::relativePosition(start1, length1, start2, length2, start, length);
    QCOMPARE(pos, 5.0);

    qInfo()<<"Test start at start1";
    start = start1;
    pos = ChartLayout::relativePosition(start1, length1, start2, length2, start, length);
    QCOMPARE(pos, 10.0);

    qInfo()<<"Test centered";
    start = start1 + (0.5 * (length1 - length));
    pos = ChartLayout::relativePosition(start1, length1, start2, length2, start, length);
    QCOMPARE(pos, start2 + (0.5 * (length2 - length)));

    qInfo()<<"Test end at end";
    start = start1 + length1 - length;
    pos = ChartLayout::relativePosition(start1, length1, start2, length2, start, length);
    QCOMPARE(pos, start2 + length2 - length);

    qInfo()<<"Test end after end";
    start = start1 + length1 - length + 1.0;
    pos = ChartLayout::relativePosition(start1, length1, start2, length2, start, length);
    QCOMPARE(pos, start2 + length2 - length + 1.0);

    qInfo()<<"Test both increase size and move";
    start2 = 20.0;

    qInfo()<<"Test item at start";
    start = start1;
    pos = ChartLayout::relativePosition(start1, length1, start2, length2, start, length);
    QCOMPARE(pos, start2);

    qInfo()<<"Test item before start";
    start = start1 - 1.0;
    pos = ChartLayout::relativePosition(start1, length1, start2, length2, start, length);
    QCOMPARE(pos, start2 - 1.0);

    qInfo()<<"Test item at end";
    start = start1 + length1 - (0.5 * length);
    pos = ChartLayout::relativePosition(start1, length1, start2, length2, start, length);
    QCOMPARE(pos, start2 + length2 - (0.5 * length));

    qInfo()<<"Test item after end";
    start = start1 + length1 - (0.5 * length) + 1.0;
    pos = ChartLayout::relativePosition(start1, length1, start2, length2, start, length);
    QCOMPARE(pos, start2 + length2 - (0.5 * length) + 1.0);

    qInfo()<<"Test item centered";
    start = start1 + (0.5 * (length1 - length));
    pos = ChartLayout::relativePosition(start1, length1, start2, length2, start, length);
    QCOMPARE(pos, start2 + (0.5 * (length2 - length)));

    qInfo()<<"Test both decrease size and move";
    qSwap(length1, length2);

    qInfo()<<"Test item centered";
    start = start1 + (length1 / 2.) - (0.5 * length);
    pos = ChartLayout::relativePosition(start1, length1, start2, length2, start, length);
    QCOMPARE(pos, start2 + (length2 / 2.) - (0.5 * length));

    qInfo()<<"Test item 1/3";
    start = start1 + (length1 / 3.0) - (0.5 * length);
    pos = ChartLayout::relativePosition(start1, length1, start2, length2, start, length);
    QCOMPARE(pos, start2 + (length2 / 3.0) - (0.5 * length));

    qInfo()<<"Test item 2/3";
    start = start1 + (2.0 * length1 / 3.0) - (0.5 * length);
    pos = ChartLayout::relativePosition(start1, length1, start2, length2, start, length);
    QCOMPARE(pos, start2 + (2.0 * length2 / 3.0) - (0.5 * length));
}


void TestChartLayout::testLayoutTop()
{
    QMap<KoShape*, QRectF> map;

    QVERIFY(!chartTitle->isVisible());
    map = layout->calculateLayout(chartTitle, true);
    chartTitle->setVisible(true);
    QVERIFY(map.contains(plotArea));
    updateItems(map);
    QVERIFY(itemRect(plotArea) != plotAreaRect);
    QCOMPARE(itemRect(chartTitle).top(), layout->margins().y());
    QCOMPARE(itemRect(plotArea).top(), itemRect(chartTitle).bottom() + layout->spacing().y());

    map = layout->calculateLayout(chartTitle, false);
    chartTitle->setVisible(false);
    QVERIFY(map.contains(plotArea));
    updateItems(map);
    QCOMPARE(itemRect(plotArea), plotAreaRect);
    QCOMPARE(itemRect(plotArea).top(), layout->margins().y());

    map = layout->calculateLayout(chartSubTitle, true);
    chartSubTitle->setVisible(true);
    QVERIFY(map.contains(plotArea));
    updateItems(map);
    QVERIFY(itemRect(plotArea) != plotAreaRect);
    QCOMPARE(itemRect(chartSubTitle).top(), layout->margins().y());
    QCOMPARE(itemRect(plotArea).top(), itemRect(chartSubTitle).bottom() + layout->spacing().y());

    map = layout->calculateLayout(chartSubTitle, false);
    chartSubTitle->setVisible(false);
    QVERIFY(map.contains(plotArea));
    updateItems(map);
    QCOMPARE(itemRect(plotArea), plotAreaRect);
    QCOMPARE(itemRect(plotArea).top(), layout->margins().y());

    map = layout->calculateLayout(chartTitle, true);
    chartTitle->setVisible(true);
    QVERIFY(map.contains(plotArea));
    updateItems(map);
    QVERIFY(itemRect(plotArea) != plotAreaRect);
    QCOMPARE(itemRect(chartTitle).top(), layout->margins().y());
    QCOMPARE(itemRect(plotArea).top(), itemRect(chartTitle).bottom() + layout->spacing().y());

    map = layout->calculateLayout(chartSubTitle, true);
    chartSubTitle->setVisible(true);
    QVERIFY(map.contains(plotArea));
    updateItems(map);
    QVERIFY(itemRect(plotArea) != plotAreaRect);
    QCOMPARE(itemRect(chartSubTitle).top(), itemRect(chartTitle).bottom() + layout->spacing().y());
    QCOMPARE(itemRect(plotArea).top(), itemRect(chartSubTitle).bottom() + layout->spacing().y());

    map = layout->calculateLayout(chartTitle, false);
    chartTitle->setVisible(false);
    QVERIFY(map.contains(plotArea));
    updateItems(map);
    QVERIFY(itemRect(plotArea) != plotAreaRect);
    QCOMPARE(itemRect(chartSubTitle).top(), layout->margins().y());
    QCOMPARE(itemRect(plotArea).top(), itemRect(chartSubTitle).bottom() + layout->spacing().y());

    map = layout->calculateLayout(chartSubTitle, false);
    chartSubTitle->setVisible(false);
    QVERIFY(map.contains(plotArea));
    updateItems(map);
    QVERIFY(!chartSubTitle->isVisible());
    QCOMPARE(itemRect(plotArea), plotAreaRect);
    QCOMPARE(itemRect(plotArea).top(), layout->margins().y());
}

void TestChartLayout::testLayoutBottom()
{
    QMap<KoShape*, QRectF> map;

    // hide xAxisTitle
    map = layout->calculateLayout(xAxisTitle, false);
    xAxisTitle->setVisible(false);
    QVERIFY(map.contains(plotArea));
    updateItems(map);
    QVERIFY(itemRect(plotArea) != plotAreaRect);
    QCOMPARE(itemRect(plotArea).height(), plotAreaRect.height() + xAxisTitleRect.height() + layout->spacing().y());

    // show xAxisTitle
    map = layout->calculateLayout(xAxisTitle, true);
    xAxisTitle->setVisible(true);
    QVERIFY(map.contains(plotArea));
    updateItems(map);
    QCOMPARE(itemRect(plotArea), plotAreaRect);
    QCOMPARE(itemRect(plotArea).bottom(), itemRect(xAxisTitle).top() - layout->spacing().y());

    // show both chartFooter and xAxisTitle
    QVERIFY(!chartFooter->isVisible());
    map = layout->calculateLayout(chartFooter, true);
    chartFooter->setVisible(true);
    updateItems(map);
    QVERIFY(map.contains(plotArea));
    QVERIFY(map.contains(xAxisTitle));
    QVERIFY(map.contains(yAxisTitle));
    QVERIFY(map.contains(legend));
    QCOMPARE(itemRect(chartFooter).bottom(), chart->size().height() - layout->margins().y());
    QCOMPARE(itemRect(xAxisTitle).bottom(), itemRect(chartFooter).top() - layout->spacing().y());
    QCOMPARE(itemRect(plotArea).bottom(), itemRect(xAxisTitle).top() - layout->spacing().y());

    // hide xAxisTitle
    map = layout->calculateLayout(xAxisTitle, false);
    xAxisTitle->setVisible(false);
    QVERIFY(map.contains(plotArea));
    updateItems(map);
    QVERIFY(itemRect(plotArea) != plotAreaRect);
    QCOMPARE(itemRect(chartFooter).bottom(), chart->size().height() - layout->margins().y());
    QCOMPARE(itemRect(plotArea).bottom(), itemRect(chartFooter).top() - layout->spacing().y());

    // set footer visible to give space to yAxis tests below
    if (!chartFooter->isVisible()) {
        map = layout->calculateLayout(chartFooter, true);
        chartFooter->setVisible(true);
        updateItems(map);
    }
    if (!xAxisTitle->isVisible()) {
        map = layout->calculateLayout(xAxisTitle, true);
        xAxisTitle->setVisible(true);
        updateItems(map);
    }
    QCOMPARE(itemRect(chartFooter).bottom(), chart->size().height() - layout->margins().y());
    QCOMPARE(itemRect(xAxisTitle).bottom(), itemRect(chartFooter).top() - layout->spacing().y());
    QCOMPARE(itemRect(plotArea).bottom(), itemRect(xAxisTitle).top() - layout->spacing().y());

    plotAreaRect = itemRect(plotArea);

    // align y title bottom edge with top edge of plot area
    yAxisTitle->setSize(QSizeF(15., yAxisTitle->size().height())); // to make it fit left of plot area
    QPointF pos = xAxisTitleRect.topLeft();
    pos.setY(plotAreaRect.top() - itemRect(yAxisTitle).height());
    setItemPosition(yAxisTitle, pos);
    QCOMPARE(itemRect(yAxisTitle).bottom(), plotAreaRect.top());

    // remove chart xAxisTitle to get plot area resized (but not moved)
    map = layout->calculateLayout(xAxisTitle, false);
    QVERIFY(map.contains(plotArea));
    QVERIFY2(!map.contains(yAxisTitle), "yAxis title shall not be moved");
    QVERIFY(!map.contains(xAxisTitle));

    // move y title bottom edge above top of plot area
    pos = yAxisTitleRect.topLeft();
    pos.setY(plotAreaRect.top() - itemRect(yAxisTitle).height() - 1.);
    setItemPosition(yAxisTitle, pos);
    QCOMPARE(itemRect(yAxisTitle).bottom(), plotAreaRect.top() - 1.);

    // remove chart xAxisTitle to get plot area resized (but not moved)
    map = layout->calculateLayout(xAxisTitle, false);
    QVERIFY(map.contains(plotArea));
    QVERIFY2(!map.contains(yAxisTitle), "yAxis title shall not be moved");
    QVERIFY(!map.contains(xAxisTitle));

    // align y title bottom edge with bottom edge of plot area
    pos = yAxisTitleRect.topLeft();
    pos.setY(plotAreaRect.bottom() - itemRect(yAxisTitle).height());
    setItemPosition(yAxisTitle, pos);
    QCOMPARE(itemRect(yAxisTitle).bottom(), plotAreaRect.bottom());

    // remove chart xAxisTitle to get plot area resized (but not moved)
    map = layout->calculateLayout(xAxisTitle, false);
    QVERIFY(map.contains(plotArea));
    QVERIFY2(map.contains(yAxisTitle), "yAxis title shall be moved");
    updateItems(map);
    QCOMPARE(itemRect(yAxisTitle).bottom(), itemRect(plotArea).bottom());
    QVERIFY(!map.contains(xAxisTitle));

    // move y title bottom edge below bottom edge of plot area
    pos = itemRect(yAxisTitle).topLeft();
    pos.setY(itemRect(plotArea).bottom() - itemRect(yAxisTitle).height() + 1.);
    setItemPosition(yAxisTitle, pos);
    QCOMPARE(itemRect(yAxisTitle).bottom(), itemRect(plotArea).bottom() + 1.);

    // insert chart xAxisTitle to get plot area resized (but not moved)
    xAxisTitle->setVisible(false);
    map = layout->calculateLayout(xAxisTitle, true);
    xAxisTitle->setVisible(true);
    updateItems(map);
    QVERIFY(map.contains(plotArea));
    QVERIFY2(map.contains(yAxisTitle), "yAxis title shall be moved");
    QCOMPARE(itemRect(yAxisTitle).bottom(), itemRect(plotArea).bottom() + 1.);
}

void TestChartLayout::testLayoutStart()
{
    QMap<KoShape*, QRectF> map;

    map = layout->calculateLayout(yAxisTitle, false);
    yAxisTitle->setVisible(false);
    updateItems(map);
    QVERIFY(itemRect(plotArea) != plotAreaRect);
    QCOMPARE(itemRect(plotArea).left(), layout->margins().x());
    QCOMPARE(itemRect(plotArea).left(), layout->margins().x());

    map = layout->calculateLayout(yAxisTitle, true);
    yAxisTitle->setVisible(true);
    updateItems(map);
    QCOMPARE(itemRect(plotArea), plotAreaRect);
    QCOMPARE(itemRect(plotArea).left(), itemRect(yAxisTitle).right() + layout->spacing().x());
    QCOMPARE(itemRect(yAxisTitle).left(), layout->margins().x());
    QCOMPARE(itemRect(plotArea).left(), itemRect(yAxisTitle).right() + layout->spacing().x());
}

void TestChartLayout::testLayoutEnd()
{
    QMap<KoShape*, QRectF> map;

    map = layout->calculateLayout(legend, false);
    legend->setVisible(false);
    QVERIFY(map.contains(plotArea));
    updateItems(map);
    QVERIFY(itemRect(plotArea) != plotAreaRect);
    QCOMPARE(itemRect(plotArea).right(), chart->size().width() - layout->margins().x());

    map = layout->calculateLayout(legend, true);
    legend->setVisible(true);
    QVERIFY(map.contains(plotArea));
    updateItems(map);
    QCOMPARE(itemRect(legend).right(), chart->size().width() - layout->margins().x());
    QCOMPARE(itemRect(plotArea).right(), itemRect(legend).left() - layout->spacing().x());
    switch(legend->alignment()) {
        case Qt::AlignLeft:
            QCOMPARE(itemRect(legend).top(), layout->diagramArea(plotArea).top());
            break;
        case Qt::AlignRight:
            QCOMPARE(itemRect(legend).bottom(), layout->diagramArea(plotArea).bottom());
            break;
        case Qt::AlignCenter: {
            QRectF dr = layout->diagramArea(plotArea);
            QRectF lr = itemRect(legend);
            QCOMPARE(dr.top() + (0.5 * dr.height()), lr.top() + (0.5 *lr.height()));
            break;
        }
        default:
            QCOMPARE(itemRect(legend), legendRect); // should not have moved
            break;
    }

    // align x title right edge with left edge of plot area
    xAxisTitle->setSize(QSizeF(15., xAxisTitle->size().height())); // to make it fit left of plot area
    QPointF pos = xAxisTitleRect.topLeft();
    pos.setX(plotAreaRect.left() - itemRect(xAxisTitle).width());
    setItemPosition(xAxisTitle, pos);
    QCOMPARE(itemRect(xAxisTitle).right(), plotAreaRect.left());

    // remove chart legend to get plot area resized (but not moved)
    map = layout->calculateLayout(legend, false);
    QVERIFY(map.contains(plotArea));
    QVERIFY2(!map.contains(xAxisTitle), "xAxis title shall not be moved");
    QVERIFY(!map.contains(legend));

    // move x title right edge left of left edge of plot area
    pos = xAxisTitleRect.topLeft();
    pos.setX(plotAreaRect.left() - itemRect(xAxisTitle).width() - 1.);
    setItemPosition(xAxisTitle, pos);
    QCOMPARE(itemRect(xAxisTitle).right(), plotAreaRect.left() - 1.);

    // remove chart legend to get plot area resized (but not moved)
    map = layout->calculateLayout(legend, false);
    QVERIFY(map.contains(plotArea));
    QVERIFY2(!map.contains(xAxisTitle), "xAxis title shall not be moved");
    QVERIFY(!map.contains(legend));

    // align x title left edge with right edge of plot area
    pos = xAxisTitleRect.topLeft();
    pos.setX(plotAreaRect.right());
    setItemPosition(xAxisTitle, pos);
    QCOMPARE(itemRect(xAxisTitle).left(), plotAreaRect.right());

    // remove chart legend to get plot area resized (but not moved)
    map = layout->calculateLayout(legend, false);
    QVERIFY(map.contains(plotArea));
    QVERIFY2(map.contains(xAxisTitle), "xAxis title shall be moved");
    QCOMPARE(map[xAxisTitle].left(), map[plotArea].right());
    QVERIFY(!map.contains(legend));

    // move x title left edge to the right of right edge of plot area
    pos = xAxisTitleRect.topLeft();
    pos.setX(plotAreaRect.right() + 1.);
    setItemPosition(xAxisTitle, pos);
    QCOMPARE(itemRect(xAxisTitle).left(), plotAreaRect.right() + 1.);

    // remove chart legend to get plot area resized (but not moved)
    map = layout->calculateLayout(legend, false);
    QVERIFY(map.contains(plotArea));
    QVERIFY2(map.contains(xAxisTitle), "xAxis title shall be moved");
    QCOMPARE(map[xAxisTitle].left(), map[plotArea].right() + 1.);
    QVERIFY(!map.contains(legend));

    QVERIFY(legend->isVisible());
    QCOMPARE(legend->alignment(), Qt::AlignCenter);

    map = layout->calculateLayout(legend, false);
    QVERIFY(map.contains(plotArea));
    QVERIFY(map.contains(xAxisTitle));
    QCOMPARE(map.count(), 2);

    // test alignment
    legend->setVisible(false);
    layout->setAutoLayoutEnabled(true);
    layout->scheduleRelayout();
    layout->layout();
    layout->setAutoLayoutEnabled(true);

    legend->setAlignment(Qt::AlignLeft);
    map = layout->calculateLayout(legend, true);
    updateItems(map);
    QCOMPARE(itemRect(legend).top(), layout->diagramArea(plotArea).top());

    legend->setAlignment(Qt::AlignRight);
    map = layout->calculateLayout(legend, true);
    updateItems(map);
    QCOMPARE(itemRect(legend).bottom(), layout->diagramArea(plotArea).bottom());

    legend->setAlignment(Qt::AlignCenter);
    map = layout->calculateLayout(legend, true);
    updateItems(map);
    QCOMPARE(itemRect(legend).top() + (0.5 * itemRect(legend).height()), layout->diagramArea(plotArea).top() + (0.5 * layout->diagramArea(plotArea).height()));
}

void TestChartLayout::testLayoutTopStart()
{
    QEXPECT_FAIL("", "Re-layout in TopStart area not implemented", Continue);
    QVERIFY(false);
}

void TestChartLayout::testLayoutTopEnd()
{
    QEXPECT_FAIL("", "Re-layout in TopEnd area not implemented", Continue);
    QVERIFY(false);
}

void TestChartLayout::testLayoutBottomStart()
{
    QEXPECT_FAIL("", "Re-layout in BottomStart area not implemented", Continue);
    QVERIFY(false);
}

void TestChartLayout::testLayoutBottomEnd()
{
    QEXPECT_FAIL("", "Re-layout in BottomEnd area not implemented", Continue);
    QVERIFY(false);
}

QTEST_MAIN(TestChartLayout)
