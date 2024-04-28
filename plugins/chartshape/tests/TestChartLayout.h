/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2017 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KCHART_TESTCHARTLAYOUT_H
#define KCHART_TESTCHARTLAYOUT_H

#include <QObject>
#include <QRectF>

class KoShape;

class KoDocumentResourceManager;

namespace KoChart
{
class ChartShape;
class ChartLayout;
class PlotArea;
class Legend;
}

class TestChartLayout : public QObject
{
    Q_OBJECT
public:
    TestChartLayout();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testDefaultDiagram();
    void testLegendPositioning();

private:
    bool compareLayout(const QString &test = QString());
    void updateRects();

private:
    QtMessageHandler msgHandler;
    KoDocumentResourceManager *drm;
    KoChart::ChartShape *chart;
    KoChart::ChartLayout *layout;
    QRectF area;
    KoShape *chartTitle;
    QRectF chartTitleRect;
    KoShape *chartSubTitle;
    QRectF chartSubTitleRect;
    KoShape *chartFooter;
    QRectF chartFooterRect;
    KoChart::PlotArea *plotArea;
    QRectF plotAreaRect;
    KoChart::Legend *legend;
    QRectF legendRect;
    KoShape *xAxisTitle;
    QRectF xAxisTitleRect;
    KoShape *yAxisTitle;
    QRectF yAxisTitleRect;
    KoShape *secondaryXAxisTitle;
    QRectF secondaryXAxisTitleRect;
    KoShape *secondaryYAxisTitle;
    QRectF secondaryYAxisTitleRect;
};

#endif // KCHART_TESTCHARTLAYOUT_H
