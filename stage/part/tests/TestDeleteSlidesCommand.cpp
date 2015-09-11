#include "TestDeleteSlidesCommand.h"

#include "KPrDocument.h"
#include "KoPAMasterPage.h"
#include "KoPAPage.h"
#include "PAMock.h"
#include "commands/KPrDeleteSlidesCommand.h"
#include "KPrCustomSlideShows.h"

#include <QTest>


void TestDeleteSlidesCommand::delSlide()
{
    MockDocument doc;

    KoPAMasterPage *master1 = new KoPAMasterPage();
    doc.insertPage(master1, 0);

    KoPAPage *page1 = new KoPAPage(master1);
    doc.insertPage(page1, 0);

    KoPAPage *p1 = dynamic_cast<KoPAPage *>(doc.pageByIndex(0, false));
    KoPAMasterPage * m1 = dynamic_cast<KoPAMasterPage *>(doc.pageByIndex(0, true));

    QVERIFY(p1 != 0);
    QVERIFY(m1 != 0);

    KoPAPage *page2 = new KoPAPage(master1);
    doc.insertPage(page2, 0);

    KoPAPage *page3 = new KoPAPage(master1);
    doc.insertPage(page3, 0);

    KoPAPage *page4 = new KoPAPage(master1);
    doc.insertPage(page4, 0);

    KoPAPage *page5 = new KoPAPage(master1);
    doc.insertPage(page5, 0);

    //Create List 1 2 4 1 5 2 2 3
    QList<KoPAPageBase*> slideList1;

    slideList1.append(page1);
    slideList1.append(page2);
    slideList1.append(page4);
    slideList1.append(page1);
    slideList1.append(page5);
    slideList1.append(page2);
    slideList1.append(page2);
    slideList1.append(page3);

    //Create List 1 2 3 4 5 4 3 2
    QList<KoPAPageBase*> slideList2;

    slideList2.append(page1);
    slideList2.append(page2);
    slideList2.append(page3);
    slideList2.append(page4);
    slideList2.append(page5);
    slideList2.append(page4);
    slideList2.append(page3);
    slideList2.append(page2);

    //expected Lists
    QList<KoPAPageBase*> resultSlideList1(slideList1);
    resultSlideList1.removeAll(page2);

    QList<KoPAPageBase*> resultSlideList2(slideList2);
    resultSlideList2.removeAll(page2);

    QString customShowName1 = "test 1";
    QString customShowName2 = "test 2";

    doc.customSlideShows()->insert(customShowName1, slideList1);
    doc.customSlideShows()->insert(customShowName2, slideList2);

    KPrDeleteSlidesCommand cmd(&doc, page2);

    cmd.redo();
    //Page removed from document
    QVERIFY(!doc.pages(false).contains(page2));
    //Page removed from custom slide shows
    QCOMPARE(doc.customSlideShows()->getByName(customShowName1), resultSlideList1);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName2), resultSlideList2);

    cmd.undo();
    QVERIFY(doc.pages(false).contains(page2));
    QCOMPARE(doc.customSlideShows()->getByName(customShowName1), slideList1);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName2), slideList2);
}

QTEST_MAIN(TestDeleteSlidesCommand)
