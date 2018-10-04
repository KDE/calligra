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

#define COMPARELAYOUT(text) QVERIFY2(compareLayout(text), text)

using namespace KoChart;

static void filterMessages(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QString cat(context.category);
    switch (type) {
        case QtDebugMsg:
//             fprintf(stderr, "%s\n%s\n%s\n%d\n%d\n", context.category, context.file, context.function, context.version, context.line);
            if (cat.startsWith("calligra.plugin.chart")) {
                fprintf(stderr, "%d: %s\n", context.line, localMsg.constData());
            }
            break;
        default:
            fprintf(stderr, "%s\n", localMsg.constData());
            break;
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
    qInstallMessageHandler(filterMessages);

    drm = new KoDocumentResourceManager(); // used throughout the test

    // Just used to check default layout
    // If changed tests may have to be changed too
    init();

    qreal spaceX = layout->spacing().x();
    qreal spaceY = layout->spacing().y();

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

    QCOMPARE(xAxisTitleRect.bottom(), area.bottom()-1);
    QCOMPARE(yAxisTitleRect.left(), area.left()+1);
    QCOMPARE(legendRect.right(), area.right()-1);
    QCOMPARE(plotAreaRect.top(), area.top()+1);
    QCOMPARE(plotAreaRect.left(), yAxisTitleRect.right() + spaceX);
    QCOMPARE(plotAreaRect.bottom(), xAxisTitleRect.top() - spaceY);
    QCOMPARE(plotAreaRect.right(), legendRect.left() - spaceX);

    cleanup();
}

void TestChartLayout::cleanupTestCase()
{
    delete drm;
}

void TestChartLayout::init()
{
    area = QRectF(0., 0., 300., 400.);
    chart = new ChartShape(drm);
    plotArea = chart->plotArea();
    xAxisTitle = plotArea->xAxis()->title();
    xAxisTitle->setVisible(true);
    yAxisTitle = plotArea->yAxis()->title();
    yAxisTitle->setVisible(true);
    // create a non-square legend
//     chart->legend()->setTitle("Legend");

    layout = chart->layout();

    chart->setSize(area.size());

    // use asymmetric padding/spacing
    layout->setPadding(KoInsets(2., 4., 6., 8.));
    layout->setSpacing(10, 12);

    area.adjust(layout->padding().left, layout->padding().top, -layout->padding().right, -layout->padding().bottom);

    layout->layout();

    chartTitle = chart->title();
    chartTitleRect = itemRect(chartTitle);
    chartSubTitle = chart->subTitle();
    chartSubTitleRect = itemRect(chartSubTitle);
    chartFooter = chart->footer();
    chartFooterRect = itemRect(chartFooter);
    plotAreaRect = itemRect(plotArea);
    legend = chart->legend();
    legendRect = itemRect(legend);
    xAxisTitleRect = itemRect(xAxisTitle);
    yAxisTitleRect = itemRect(yAxisTitle);

    secondaryXAxisTitle = plotArea->secondaryXAxis() ? plotArea->secondaryXAxis()->title() : 0;
    secondaryXAxisTitleRect = secondaryXAxisTitle ? itemRect(secondaryXAxisTitle) : QRectF();
    secondaryYAxisTitle = plotArea->secondaryYAxis() ? plotArea->secondaryYAxis()->title() : 0;
    secondaryYAxisTitleRect = secondaryYAxisTitle ? itemRect(secondaryYAxisTitle) : QRectF();
}

void TestChartLayout::cleanup()
{
    delete chart;
}

void TestChartLayout::updateRects()
{
    chartTitleRect = chartTitle && chartTitle->isVisible() ? itemRect(chartTitle) : QRectF();
    chartSubTitleRect = chartSubTitle && chartSubTitle->isVisible() ? itemRect(chartSubTitle) : QRectF();
    chartFooterRect = chartFooter && chartFooter->isVisible() ? itemRect(chartFooter) : QRectF();
    legendRect = legend && legend->isVisible() ? itemRect(legend) : QRectF();
    plotAreaRect = plotArea && plotArea->isVisible() ? itemRect(plotArea) : QRectF();
    xAxisTitleRect = xAxisTitle && xAxisTitle->isVisible() ? itemRect(xAxisTitle) : QRectF();
    yAxisTitleRect = yAxisTitle && yAxisTitle->isVisible() ? itemRect(yAxisTitle) : QRectF();
    secondaryXAxisTitleRect = secondaryXAxisTitle && secondaryXAxisTitle->isVisible() ? itemRect(secondaryXAxisTitle) : QRectF();
    secondaryYAxisTitleRect = secondaryYAxisTitle && secondaryYAxisTitle->isVisible() ? itemRect(secondaryYAxisTitle) : QRectF();
}

void dbg(const QString &test, const QString topic, const QRectF &expected, const QRectF &actual)
{
    QString t = topic + ':';
    if (!test.isEmpty()) {
        qInfo().noquote()<<"FAIL:"<<test;
    }
    qInfo().noquote()<<"\t"<<t
    <<endl<<"\t  actual:"<<actual<<':'<<"left:"<<actual.left()<<"top:"<<actual.top()<<"right:"<<actual.right()<<"bottom"<<actual.bottom()<<"center:"<<actual.center()
    <<endl<<"\texpected:"<<expected<<':'<<"left:"<<expected.left()<<"top:"<<expected.top()<<"right:"<<expected.right()<<"bottom"<<expected.bottom()<<"center:"<<expected.center();
}
bool TestChartLayout::compareLayout(const QString &test)
{
    bool result = true;
    if (chartTitle && chartTitle->isVisible()) {
        bool r = itemRect(chartTitle) == chartTitleRect;
        if (!r) {
            dbg(test, "Title", chartTitleRect, itemRect(chartTitle));
            result = r;
        }
    }
    if (chartSubTitle && chartSubTitle->isVisible()) {
        bool r = itemRect(chartSubTitle) == chartSubTitleRect;
        if (!r) {
            dbg(test, "Subtitle", chartSubTitleRect, itemRect(chartSubTitle));
            result = r;
        }
    }
    if (chartFooter && chartFooter->isVisible()) {
        bool r = itemRect(chartFooter) == chartFooterRect;
        if (!r) {
            dbg(test, "Footer", chartFooterRect, itemRect(chartFooter));
            result = r;
        }
    }
    if (legend && legend->isVisible()) {
        bool r = itemRect(legend) == legendRect;
        if (!r) {
            dbg(test, "Legend", legendRect, itemRect(legend));
            result = r;
        }
    }
    if (plotArea && plotArea->isVisible()) {
        bool r = itemRect(plotArea) == plotAreaRect;
        if (!r) {
            dbg(test, "PlotArea", plotAreaRect, itemRect(plotArea));
            result = r;
        }
    }
    if (xAxisTitle && xAxisTitle->isVisible()) {
        bool r = itemRect(xAxisTitle) == xAxisTitleRect;
        if (!r) {
            dbg(test, "X-axis", xAxisTitleRect, itemRect(xAxisTitle));
            result = r;
        }
    }
    if (yAxisTitle && yAxisTitle->isVisible()) {
        bool r = itemRect(yAxisTitle) == yAxisTitleRect;
        if (!r) {
            dbg(test, "Y-axis", yAxisTitleRect, itemRect(yAxisTitle));
            result = r;
        }
    }
    if (secondaryXAxisTitle && secondaryXAxisTitle->isVisible()) {
        bool r = itemRect(secondaryXAxisTitle) == secondaryXAxisTitleRect;
        if (!r) {
            dbg(test, "Secondary x-axis", secondaryXAxisTitleRect, itemRect(secondaryXAxisTitle));
            result = r;
        }
    }
    if (secondaryYAxisTitle && secondaryYAxisTitle->isVisible()) {
        bool r = itemRect(secondaryYAxisTitle) == secondaryYAxisTitleRect;
        if (!r) {
            dbg(test, "Secondary y-axis", secondaryYAxisTitleRect, itemRect(secondaryYAxisTitle));
            result = r;
        }
    }
    return result;
}

void TestChartLayout::testDefaultDiagram()
{
    qreal spaceX = layout->spacing().x();
    qreal spaceY = layout->spacing().y();

    qreal height = itemRect(xAxisTitle).height() + spaceY;
    plotAreaRect.setBottom(plotAreaRect.bottom() + height);
    legendRect.moveTop(plotAreaRect.center().y() - legendRect.height() / 2);
    yAxisTitleRect.moveTop(plotAreaRect.center().y() - yAxisTitleRect.height() / 2);
    xAxisTitle->setVisible(false);
    layout->scheduleRelayout();
    layout->layout();
    COMPARELAYOUT("Hide x axis title");

    qreal width = itemRect(yAxisTitle).width() + spaceX;
    plotAreaRect.setLeft(plotAreaRect.left() - width);
    yAxisTitle->setVisible(false);
    layout->scheduleRelayout();
    layout->layout();
    COMPARELAYOUT("Hide y axis title");

    chartTitle->setVisible(true);
    layout->scheduleRelayout();
    layout->layout();
    chartTitleRect = itemRect(chartTitle);
    chartTitleRect.moveTop(area.top()+1);
    plotAreaRect.setTop(chartTitleRect.bottom() + spaceY);
    legendRect.moveTop(plotAreaRect.center().y() - legendRect.height() / 2);
    COMPARELAYOUT("Show title");

    chartSubTitle->setVisible(true);
    layout->scheduleRelayout();
    layout->layout();
    chartSubTitleRect = itemRect(chartSubTitle);
    chartSubTitleRect.moveTop(chartTitleRect.bottom() + spaceY);
    plotAreaRect.setTop(chartSubTitleRect.bottom() + spaceY);
    legendRect.moveTop(plotAreaRect.center().y() - legendRect.height() / 2);
    COMPARELAYOUT("Show subtitle");

    chartFooter->setVisible(true);
    layout->scheduleRelayout();
    layout->layout();
    chartFooterRect = itemRect(chartFooter);
    chartFooterRect.moveBottom(area.bottom()-1);
    plotAreaRect.setBottom(chartFooterRect.top() - spaceY);
    legendRect.moveTop(plotAreaRect.center().y() - legendRect.height() / 2);
    COMPARELAYOUT("Show footer");

    legend->setVisible(false);
    layout->scheduleRelayout();
    layout->layout();
    plotAreaRect.setRight(area.right()-1);
    COMPARELAYOUT("Hide legend");

    chartTitle->setVisible(false);
    layout->scheduleRelayout();
    layout->layout();
    chartSubTitleRect.moveTop(area.top()+1);
    plotAreaRect.setTop(chartSubTitleRect.bottom() + spaceY);
    COMPARELAYOUT("Hide title");

    chartSubTitle->setVisible(false);
    layout->scheduleRelayout();
    layout->layout();
    plotAreaRect.setTop(area.top()+1);
    COMPARELAYOUT("Hide subtitle");

    chartFooter->setVisible(false);
    layout->scheduleRelayout();
    layout->layout();
    plotAreaRect.setBottom(area.bottom()-1);
    COMPARELAYOUT("Hide footer");

    KoChart::Axis *a = new KoChart::Axis(plotArea, KoChart::XAxisDimension);
    a->setOdfAxisPosition("end"); // above plot area
    a->updateKChartAxisPosition();
    secondaryXAxisTitle = a->title();
    secondaryXAxisTitle->setVisible(true);
    layout->setItemType(secondaryXAxisTitle, KoChart::SecondaryXAxisTitleType);
    layout->scheduleRelayout();
    layout->layout();
    secondaryXAxisTitleRect = itemRect(secondaryXAxisTitle);
    secondaryXAxisTitleRect.moveTop(area.top()+1);
    plotAreaRect.setTop(secondaryXAxisTitleRect.bottom() + spaceY);
    COMPARELAYOUT("Show secondary x axis");

    a = new KoChart::Axis(plotArea, KoChart::YAxisDimension);
    a->setOdfAxisPosition("end"); // right of plot area
    a->updateKChartAxisPosition();
    secondaryYAxisTitle = a->title();
    secondaryYAxisTitle->setVisible(true);
    secondaryYAxisTitle->rotate(90);
    layout->setItemType(secondaryYAxisTitle, KoChart::SecondaryYAxisTitleType);
    layout->scheduleRelayout();
    layout->layout();
    secondaryYAxisTitleRect = itemRect(secondaryYAxisTitle);
    secondaryYAxisTitleRect.moveRight(area.right()-1);
    plotAreaRect.setRight(secondaryYAxisTitleRect.left() - spaceX);
    secondaryXAxisTitleRect.moveLeft(plotAreaRect.center().x() - secondaryXAxisTitleRect.width() / 2);
    COMPARELAYOUT("Show secondary y axis");

    secondaryXAxisTitle->setVisible(false);
    layout->scheduleRelayout();
    layout->layout();
    plotAreaRect.setTop(area.top()+1);
    secondaryYAxisTitleRect.moveTop(plotAreaRect.center().y() - secondaryYAxisTitleRect.height() / 2);
    COMPARELAYOUT("Hide secondary x axis");

    secondaryYAxisTitle->setVisible(false);
    layout->scheduleRelayout();
    layout->layout();
    plotAreaRect.setRight(area.right()-1);
    COMPARELAYOUT("Hide secondary y axis");

    // show everyting in one go
    chartTitle->setVisible(true);
    chartSubTitle->setVisible(true);
    chartFooter->setVisible(true);
    legend->setVisible(true);
    xAxisTitle->setVisible(true);
    yAxisTitle->setVisible(true);
    secondaryXAxisTitle->setVisible(true);
    secondaryYAxisTitle->setVisible(true);

    layout->scheduleRelayout();
    layout->layout();

    chartTitleRect = itemRect(chartTitle);
    chartSubTitleRect = itemRect(chartSubTitle);
    chartFooterRect = itemRect(chartFooter);
    plotAreaRect = itemRect(plotArea);
    xAxisTitleRect = itemRect(xAxisTitle);
    yAxisTitleRect = itemRect(yAxisTitle);
    secondaryXAxisTitleRect = itemRect(secondaryXAxisTitle);
    secondaryYAxisTitleRect = itemRect(secondaryYAxisTitle);

    chartTitleRect.moveTop(area.top()+1);

    chartSubTitleRect.moveTop(chartTitleRect.bottom() + spaceY);

    chartFooterRect.moveBottom(area.bottom()-1);

    xAxisTitleRect.moveBottom(chartFooterRect.top() - spaceY);
    xAxisTitleRect.moveLeft(plotAreaRect.center().x() - xAxisTitleRect.width() / 2);

    yAxisTitleRect.moveBottom(area.left()+1);
    yAxisTitleRect.moveTop(plotAreaRect.center().y() - yAxisTitleRect.height() / 2);

    secondaryXAxisTitleRect.moveTop(chartSubTitleRect.bottom() + spaceY);
    secondaryXAxisTitleRect.moveLeft(plotAreaRect.center().x() - secondaryXAxisTitleRect.width() / 2);

    secondaryYAxisTitleRect.moveLeft(plotAreaRect.right() + spaceX);
    secondaryYAxisTitleRect.moveTop(plotAreaRect.center().y() - secondaryYAxisTitleRect.height() / 2);

    COMPARELAYOUT("Show everyting in one go");
}

void TestChartLayout::testLegendPositioning()
{
    qreal spaceX = layout->spacing().x();
    qreal spaceY = layout->spacing().y();

    legend->setLegendPosition(KoChart::TopPosition);

    legend->setVisible(true);
    legend->setAlignment(Qt::AlignCenter);
    layout->scheduleRelayout();
    layout->layout();
    updateRects();
    legendRect.moveTop(area.top()+1);
    legendRect.moveLeft(plotAreaRect.center().x() - legendRect.width() / 2);
    plotAreaRect.setTop(legendRect.bottom() + spaceY);
    xAxisTitleRect.moveLeft(plotAreaRect.center().x() - xAxisTitleRect.width() / 2);
    yAxisTitleRect.moveTop(plotAreaRect.center().y() - yAxisTitleRect.height() / 2);
    COMPARELAYOUT("Legend position: top, alignment: center");

    legend->setVisible(true);
    legend->setAlignment(Qt::AlignLeft);
    layout->scheduleRelayout();
    layout->layout();
    legendRect.moveLeft(plotAreaRect.left());
    COMPARELAYOUT("Legend position: top, alignment: left");

    legend->setVisible(true);
    legend->setAlignment(Qt::AlignRight);
    layout->scheduleRelayout();
    layout->layout();
    legendRect.moveRight(plotAreaRect.right());
    COMPARELAYOUT("Legend position: top, alignment: right");

    legend->setLegendPosition(KoChart::BottomPosition);

    legend->setVisible(true);
    legend->setAlignment(Qt::AlignCenter);
    layout->scheduleRelayout();
    layout->layout();
    updateRects();
    legendRect.moveBottom(area.bottom()-1);
    legendRect.moveLeft(plotAreaRect.center().x() - legendRect.width() / 2);
    xAxisTitleRect.setBottom(legendRect.top() - spaceY);
    plotAreaRect.setBottom(xAxisTitleRect.top() - spaceY);
    xAxisTitleRect.moveLeft(plotAreaRect.center().x() - xAxisTitleRect.width() / 2);
    yAxisTitleRect.moveTop(plotAreaRect.center().y() - yAxisTitleRect.height() / 2);
    COMPARELAYOUT("Legend position: bottom, alignment: center");

    legend->setVisible(true);
    legend->setAlignment(Qt::AlignLeft);
    layout->scheduleRelayout();
    layout->layout();
    legendRect.moveLeft(plotAreaRect.left());
    COMPARELAYOUT("Legend position: bottom, alignment: left");

    legend->setVisible(true);
    legend->setAlignment(Qt::AlignRight);
    layout->scheduleRelayout();
    layout->layout();
    legendRect.moveRight(plotAreaRect.right());
    COMPARELAYOUT("Legend position: bottom, alignment: right");

    legend->setLegendPosition(KoChart::EndPosition);

    legend->setVisible(true);
    legend->setAlignment(Qt::AlignCenter);
    layout->scheduleRelayout();
    layout->layout();
    updateRects();
    legendRect.moveTop(area.top()+1);
    legendRect.moveTop(plotAreaRect.center().y() - legendRect.height() / 2);
    plotAreaRect.setRight(legendRect.left() - spaceX);
    xAxisTitleRect.moveLeft(plotAreaRect.center().x() - xAxisTitleRect.width() / 2);
    yAxisTitleRect.moveTop(plotAreaRect.center().y() - yAxisTitleRect.height() / 2);
    COMPARELAYOUT("Legend position: end, alignment: center");

    legend->setVisible(true);
    legend->setAlignment(Qt::AlignLeft); // means top
    layout->scheduleRelayout();
    layout->layout();
    legendRect.moveTop(plotAreaRect.top());
    COMPARELAYOUT("Legend position: end, alignment: top");

    legend->setVisible(true);
    legend->setAlignment(Qt::AlignRight); // means bottom
    layout->scheduleRelayout();
    layout->layout();
    legendRect.moveBottom(plotAreaRect.bottom());
    COMPARELAYOUT("Legend position: end, alignment: bottom");

    legend->setLegendPosition(KoChart::StartPosition);

    legend->setVisible(true);
    legend->setAlignment(Qt::AlignCenter);
    layout->scheduleRelayout();
    layout->layout();
    updateRects();
    legendRect.moveLeft(area.left()+1);
    legendRect.moveTop(plotAreaRect.center().y() - legendRect.height() / 2);
    yAxisTitleRect.moveLeft(legendRect.right() + spaceX);
    plotAreaRect.setLeft(yAxisTitleRect.right() + spaceX);
    xAxisTitleRect.moveLeft(plotAreaRect.center().x() - xAxisTitleRect.width() / 2);
    yAxisTitleRect.moveTop(plotAreaRect.center().y() - yAxisTitleRect.height() / 2);
    COMPARELAYOUT("Legend position: left, alignment: center");

    legend->setVisible(true);
    legend->setAlignment(Qt::AlignLeft); // means top
    layout->scheduleRelayout();
    layout->layout();
    legendRect.moveTop(plotAreaRect.top());
    COMPARELAYOUT("Legend position: left, alignment: top");

    legend->setVisible(true);
    legend->setAlignment(Qt::AlignRight); // means bottom
    layout->scheduleRelayout();
    layout->layout();
    legendRect.moveBottom(plotAreaRect.bottom());
    COMPARELAYOUT("Legend position: left, alignment: bottom");

    legend->setLegendPosition(KoChart::TopStartPosition);

    legend->setVisible(true);
    layout->scheduleRelayout();
    layout->layout();
    updateRects();
    legendRect.moveLeft(area.left()+1);
    legendRect.moveTop(area.top()+1);
    yAxisTitleRect.moveRight(area.left() + spaceX); // preliminary
    plotAreaRect.setLeft(qMax(legendRect.right(), yAxisTitleRect.right()) + spaceX);
    yAxisTitleRect.moveRight(plotAreaRect.left() - spaceX);
    plotAreaRect.setTop(legendRect.bottom() + spaceY);
    xAxisTitleRect.moveLeft(plotAreaRect.center().x() - xAxisTitleRect.width() / 2);
    yAxisTitleRect.moveTop(plotAreaRect.center().y() - yAxisTitleRect.height() / 2);
    COMPARELAYOUT("Legend position: top/start");

    legend->setLegendPosition(KoChart::TopEndPosition);

    legend->setVisible(true);
    layout->scheduleRelayout();
    layout->layout();
    updateRects();
    legendRect.moveRight(area.right()-1);
    legendRect.moveTop(area.top()+1);
    yAxisTitleRect.moveLeft(area.left()+1);
    plotAreaRect.setRight(legendRect.left() - spaceX);
    plotAreaRect.setTop(legendRect.bottom() + spaceY);
    xAxisTitleRect.moveLeft(plotAreaRect.center().x() - xAxisTitleRect.width() / 2);
    yAxisTitleRect.moveTop(plotAreaRect.center().y() - yAxisTitleRect.height() / 2);
    COMPARELAYOUT("Legend position: top/end");

    legend->setLegendPosition(KoChart::BottomStartPosition);

    legend->setVisible(true);
    layout->scheduleRelayout();
    layout->layout();
    updateRects();
    legendRect.moveLeft(area.left()+1);
    legendRect.moveBottom(area.bottom()-1);

    yAxisTitleRect.moveLeft(area.left()); // preliminary
    plotAreaRect.setLeft(qMax(legendRect.right(), yAxisTitleRect.right()) + spaceX);
    yAxisTitleRect.moveRight(plotAreaRect.left() - spaceX);

    xAxisTitleRect.moveBottom(area.bottom());
    plotAreaRect.setBottom(qMin(legendRect.top(), xAxisTitleRect.top()) - spaceY);
    xAxisTitleRect.moveTop(plotAreaRect.bottom() + spaceY);

    xAxisTitleRect.moveLeft(plotAreaRect.center().x() - xAxisTitleRect.width() / 2);
    yAxisTitleRect.moveTop(plotAreaRect.center().y() - yAxisTitleRect.height() / 2);
    COMPARELAYOUT("Legend position: bottom/start");

    legend->setLegendPosition(KoChart::BottomEndPosition);

    legend->setVisible(true);
    layout->scheduleRelayout();
    layout->layout();
    updateRects();
    legendRect.moveRight(area.right()-1);
    legendRect.moveBottom(area.bottom()-1);

    plotAreaRect.setRight(legendRect.left() - spaceX);

    xAxisTitleRect.moveBottom(area.bottom());
    plotAreaRect.setBottom(qMin(legendRect.top(), xAxisTitleRect.top()) - spaceY);
    xAxisTitleRect.moveTop(plotAreaRect.bottom() + spaceY);

    xAxisTitleRect.moveLeft(plotAreaRect.center().x() - xAxisTitleRect.width() / 2);
    yAxisTitleRect.moveTop(plotAreaRect.center().y() - yAxisTitleRect.height() / 2);
    COMPARELAYOUT("Legend position: bottom/end");
}

QTEST_MAIN(TestChartLayout)
