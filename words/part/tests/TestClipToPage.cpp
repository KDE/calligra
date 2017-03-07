#include "TestClipToPage.h"

#include <KWDocument.h>
#include <KWCanvas.h>
#include <MockShapes.h>
#include <KWPage.h>

#include "MockPart.h"

#include <QtTest>

void TestClipToPage::testClipToPage()
{
    KWDocument doc(new MockPart);
    KWPage page1 = doc.appendPage("Standard");
    KoPageLayout layout = page1.pageStyle().pageLayout();
    layout.width = 300;
    layout.height = 410;
    page1.pageStyle().setPageLayout(layout);
    KWCanvas canvas("bla", &doc, 0, 0);

    MockShape shape;
    shape.setPosition(QPointF(50, 50));
    shape.setSize(QSizeF(100, 100));
    QPointF distance(0, 0);
    canvas.clipToDocument(&shape, distance);
    QCOMPARE(distance, QPointF(0, 0));

    distance = QPointF(-200, -500);
    canvas.clipToDocument(&shape, distance);
    QCOMPARE(distance, QPointF(-145, -145));

    distance = QPointF(1000, 2000);
    canvas.clipToDocument(&shape, distance);
    QCOMPARE(distance, QPointF(245, 355));

    distance = QPointF(50, 50);
    canvas.clipToDocument(&shape, distance);
    QCOMPARE(distance, QPointF(50, 50));

    // test when we start outside the page
    shape.setPosition(QPointF(-200, -100));
    distance = QPointF(0, 0);
    canvas.clipToDocument(&shape, distance);
    QCOMPARE(distance, QPointF(105, 5));
    distance = QPointF(120, 120);
    canvas.clipToDocument(&shape, distance);
    QCOMPARE(distance, QPointF(120, 120));

    shape.setPosition(QPointF(400, 200));
    distance = QPointF(0, 0);
    canvas.clipToDocument(&shape, distance);
    QCOMPARE(distance, QPointF(-105, 0));

    distance = QPointF(-110, -50);
    canvas.clipToDocument(&shape, distance);
    QCOMPARE(distance, QPointF(-110, -50));
}

QTEST_MAIN(TestClipToPage)
