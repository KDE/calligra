/* This file is part of the Calligra project
   Copyright (C) 2008-2010 Thomas Zander <zander@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#include "TestViewMode.h"

#include <KWPageManager.h>
#include <KWPage.h>
#include <KoZoomHandler.h>

#include <kdebug.h>

// this is also defined in the KWViewModeNormal
#define PAGEGAP 5

void TestViewMode::init()
{
}

void TestViewMode::testBasicConversion()
{
    const int A4_HEIGHT = 842;
    KWPageManager pageManager;
    KoZoomHandler zoomer;
    zoomer.setZoom(1);
    zoomer.setDpi(72, 72); // BORING :)
    MyViewModeNormal viewMode;
    viewMode.setPageManager(&pageManager);
    viewMode.setViewConverter(&zoomer);

    QCOMPARE(viewMode.pageTops().count(), 0);
    QCOMPARE(viewMode.pageSpreadMode(), false);
    QCOMPARE(viewMode.contents(), QSizeF(0, 0));

    pageManager.appendPage(); //one A4 page.
    viewMode.pageSetupChanged();
    QCOMPARE(viewMode.pageTops().count(), 1);
    QCOMPARE(viewMode.pageSpreadMode(), false);
    QCOMPARE(viewMode.contents().toSize(), QSize(595, A4_HEIGHT)); // approximate size is ok

    QCOMPARE(viewMode.documentToView(QPointF()).toPoint(), QPoint());
    QCOMPARE(viewMode.documentToView(QPointF(10, 10)).toPoint(), QPoint(10, 10));
    QCOMPARE(viewMode.documentToView(QPointF(590, 10)).toPoint(), QPoint(590, 10));
    QCOMPARE(viewMode.documentToView(QPointF(590, 840)).toPoint(), QPoint(590, 840));
    QCOMPARE(viewMode.documentToView(QPointF(600, 840)).toPoint(), QPoint(600, 840)); // no clipping horizontally
    QCOMPARE(viewMode.documentToView(QPointF(50, 1000)).toPoint(), QPoint(50, 1000)); // no clipping vertically

    QCOMPARE(viewMode.viewToDocument(QPointF()).toPoint(), QPoint());
    QCOMPARE(viewMode.viewToDocument(QPointF(10, 10)).toPoint(), QPoint(10, 10));
    QCOMPARE(viewMode.viewToDocument(QPointF(590, 10)).toPoint(), QPoint(590, 10));
    QCOMPARE(viewMode.viewToDocument(QPointF(590, 840)).toPoint(), QPoint(590, 840));
    QCOMPARE(viewMode.viewToDocument(QPointF(600, 840)).toPoint(), QPoint(600, 840)); // no clipping horizontally
    QCOMPARE(viewMode.viewToDocument(QPointF(50, 1000)).toPoint(), QPoint(50, 1000)); // no clipping vertically

    KWPage page2 = pageManager.appendPage();
    page2.setPageSide(KWPage::PageSpread);
    viewMode.pageSetupChanged();
    QCOMPARE(viewMode.pageTops().count(), 3);
    QCOMPARE(viewMode.pageSpreadMode(), true);
    QCOMPARE(viewMode.contents().toSize(), QSize(1191, A4_HEIGHT * 2 + PAGEGAP)); // just A4 times 2, plus the PAGEGAP px gap between pages

    QCOMPARE(viewMode.pageTops()[0], 0.0);
    QCOMPARE(qRound(viewMode.pageTops()[1]), A4_HEIGHT + PAGEGAP);
    QCOMPARE(qRound(viewMode.pageTops()[2]), A4_HEIGHT + PAGEGAP); // for a pageSpread we don't move the top of page

    QCOMPARE(viewMode.documentToView(QPointF()).toPoint(), QPoint());
    QCOMPARE(viewMode.documentToView(QPointF(10, 10)).toPoint(), QPoint(10, 10));
    QCOMPARE(viewMode.documentToView(QPointF(590, 10)).toPoint(), QPoint(590, 10));
    QCOMPARE(viewMode.documentToView(QPointF(590, 840)).toPoint(), QPoint(590, 840));
    QCOMPARE(viewMode.documentToView(QPointF(600, 840)).toPoint(), QPoint(600, 840));
    QCOMPARE(viewMode.documentToView(QPointF(50, 1000)).toPoint(), QPoint(50, 1000 + PAGEGAP));
    QCOMPARE(viewMode.documentToView(QPointF(1000, 1000)).toPoint(), QPoint(1000, 1000 + PAGEGAP)); // no clipping vertically

    QCOMPARE(viewMode.viewToDocument(QPointF()).toPoint(), QPoint());
    QCOMPARE(viewMode.viewToDocument(QPointF(10, 10)).toPoint(), QPoint(10, 10));
    QCOMPARE(viewMode.viewToDocument(QPointF(590, 10)).toPoint(), QPoint(590, 10));
    QCOMPARE(viewMode.viewToDocument(QPointF(590, 840)).toPoint(), QPoint(590, 840));
    QCOMPARE(viewMode.viewToDocument(QPointF(600, 840)).toPoint(), QPoint(600, 840));
    QCOMPARE(viewMode.viewToDocument(QPointF(50, 1000 + PAGEGAP)).toPoint(), QPoint(50, 1000));
    QCOMPARE(viewMode.viewToDocument(QPointF(1000, 1000 + PAGEGAP)).toPoint(), QPoint(1000, 1000)); // no clipping vertically

    // rectangles are more tricky ;)
    QVector<KWViewMode::ViewMap> answer;
    answer = viewMode.mapExposedRects(QRect(10, 10, 10, 10));
    QCOMPARE(answer.count(), 1);
    QCOMPARE(answer[0].clipRect, QRect(10, 10, 10, 10));
    QCOMPARE(answer[0].distance, QPointF());

    answer = viewMode.mapExposedRects(QRect(10, 10, 10, 1000));
    QCOMPARE(answer.count(), 2);
    QCOMPARE(answer[0].clipRect, QRect(10, 10, 10, A4_HEIGHT - 10));
    QCOMPARE(answer[0].distance, QPointF());
    QCOMPARE(answer[1].clipRect, QRect(10, A4_HEIGHT, 10, 1000 - A4_HEIGHT + PAGEGAP));
    QCOMPARE(answer[1].distance, QPointF(0, PAGEGAP));
}

void TestViewMode::testClipRectForPageSpread()
{
    KWPageManager pageManager;

    //set up as a page spread;
    KoPageLayout pageLayout = pageManager.defaultPageStyle().pageLayout();
    pageLayout.leftMargin = -1;
    pageLayout.rightMargin = -1;
    pageLayout.pageEdge = 20;
    pageLayout.bindingSide = 25;
    pageLayout.height = 200;
    pageLayout.width = 300;
    pageManager.defaultPageStyle().setPageLayout(pageLayout);

    KoZoomHandler zoomer;
    zoomer.setZoom(1.0);
    zoomer.setDpi(72, 72); // BORING :)

    MyViewModeNormal viewMode;
    viewMode.setPageManager(&pageManager);
    viewMode.setViewConverter(&zoomer);

    pageManager.appendPage(); // page 1
    pageManager.appendPage(); // page 2 & 3
    QCOMPARE(pageManager.pageCount(), 3);
    viewMode.pageSetupChanged();

    QVector<KWViewMode::ViewMap> answer;
    answer = viewMode.mapExposedRects(QRect(50, 180, 300, 100));
    QCOMPARE(answer.count(), 2);
    QCOMPARE(answer[0].clipRect, QRect(50, 180, 250, 20)); // page 1
    QCOMPARE(answer[0].distance, QPointF());
    QCOMPARE(answer[1].clipRect, QRect(50, 200, 300, 75)); // page 2 & 3
    QCOMPARE(answer[1].distance, QPointF(0, PAGEGAP));

    answer = viewMode.mapExposedRects(QRect(0, 250, 1000, 50));
    QCOMPARE(answer.count(), 1);
    QCOMPARE(answer[0].clipRect, QRect(0, 245, 600, 50)); // page 2&3
    QCOMPARE(answer[0].distance, QPointF(0, PAGEGAP));

    pageManager.appendPage(); // page 4 & 5
    QCOMPARE(pageManager.pageCount(), 5);
    viewMode.pageSetupChanged();

    answer = viewMode.mapExposedRects(QRect(0, 250, 1000, 50));
    QCOMPARE(answer.count(), 1);
    QCOMPARE(answer[0].clipRect, QRect(0, 245, 600, 50)); // page 2&3
    QCOMPARE(answer[0].distance, QPointF(0, PAGEGAP));

    answer = viewMode.mapExposedRects(QRect(0, 450, 1000, 50));
    QCOMPARE(answer.count(), 1);
    QCOMPARE(answer[0].clipRect, QRect(0, 440, 600, 50)); // page 4&5
    QCOMPARE(answer[0].distance, QPointF(0, PAGEGAP * 2));

    answer = viewMode.mapExposedRects(QRect(0, 250, 1000, 500));
    QCOMPARE(answer.count(), 2);
    QCOMPARE(answer[0].clipRect, QRect(0, 245, 600, 155)); // page 2&3
    QCOMPARE(answer[0].distance, QPointF(0, PAGEGAP));
    QCOMPARE(answer[1].clipRect, QRect(0, 400, 600, 200)); // page 4&5
    QCOMPARE(answer[1].distance, QPointF(0, PAGEGAP * 2));
}

void TestViewMode::testPageSetupChanged()
{
    KWPageManager pageManager;

    //set up as a page spread;
    KoPageLayout pageLayout = pageManager.defaultPageStyle().pageLayout();
    pageLayout.leftMargin = -1;
    pageLayout.rightMargin = -1;
    pageLayout.pageEdge = 20;
    pageLayout.bindingSide = 25;
    pageManager.defaultPageStyle().setPageLayout(pageLayout);

    KoZoomHandler zoomer;
    zoomer.setZoom(1.0);
    zoomer.setDpi(72, 72); // BORING :)

    MyViewModeNormal viewMode;
    viewMode.setPageManager(&pageManager);
    viewMode.setViewConverter(&zoomer);

    pageManager.appendPage(); // page 1
    pageManager.appendPage(); // page 2 & 3
    QCOMPARE(pageManager.pageCount(), 3);

    viewMode.pageSetupChanged();
    QCOMPARE(viewMode.pageTops().count(), 3);
    QCOMPARE(viewMode.pageSpreadMode(), true);

    KWPage lastPage = pageManager.appendPage(); // page 4 & 5
    viewMode.pageSetupChanged();
    QCOMPARE(viewMode.pageTops().count(), 5);
    QCOMPARE(viewMode.pageSpreadMode(), true);

    pageManager.removePage(lastPage);
    viewMode.pageSetupChanged();
    QCOMPARE(viewMode.pageTops().count(), 3);
    QCOMPARE(viewMode.pageSpreadMode(), true);
}

QTEST_KDEMAIN(TestViewMode, GUI)
