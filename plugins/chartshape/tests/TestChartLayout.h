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

#ifndef KCHART_TESTCHARTLAYOUT_H
#define KCHART_TESTCHARTLAYOUT_H

#include <QObject>
#include <QRectF>

class KoShape;

class KoDocumentResourceManager;

namespace KoChart {
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
