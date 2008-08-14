#include "TestBasicLayout.h"
#include "TestDocumentLayout.h"

#include "KWPageManager.h"
#include "KWPage.h"
#include "KWPageSettings.h"
#include "KWFrameLayout.h"
#include "KWTextFrameSet.h"
#include "KWTextFrame.h"
#include "KWDocument.h"
#include "KWord.h"

#include <kcomponentdata.h>

class Helper {
public:
    Helper() {
        pageManager = new KWPageManager(&doc);
        pageManager->setStartPage(1);
        KWPage *page = pageManager->appendPage();
        KoPageLayout pageLayout = page->pageSettings()->pageLayout();
        pageLayout.width = 200;
        pageLayout.height = 200;
        page->pageSettings()->setPageLayout(pageLayout);
        pageSettings = page->pageSettings();
    }
    ~Helper() {
        delete pageManager;
    }

    KWDocument doc;
    KWPageManager *pageManager;
    KWPageSettings *pageSettings;
};

TestBasicLayout::TestBasicLayout() {
    new KComponentData("TestBasicLayout");
}

void TestBasicLayout::testGetOrCreateFrameSet() {
    Helper helper;
    m_frames.clear();
    KWPage *page = helper.pageManager->page(1);
    KWFrameLayout bfl(helper.pageManager, m_frames);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    KWTextFrameSet *fs = bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page);
    QVERIFY(fs);
    QCOMPARE(fs->textFrameSetType(), KWord::OddPagesHeaderTextFrameSet);

    KWTextFrameSet *fs2 = bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page);
    QVERIFY(fs2);
    QCOMPARE(fs, fs2);

    KWTextFrameSet *main = new KWTextFrameSet(0, KWord::MainTextFrameSet);
    m_frames.append(main);
    bfl.m_setup = false;
    KWTextFrameSet *main2 = bfl.getOrCreate(KWord::MainTextFrameSet, page);
    QVERIFY(main2);
    QCOMPARE(main, main2);
}

void TestBasicLayout::testCreateNewFramesForPage() {
    Helper helper;
    m_frames.clear();
    QVERIFY(m_frames.count() == 0);
    KWFrameLayout bfl(helper.pageManager, m_frames);
    KWPage *page = helper.pageManager->page(1);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    KWTextFrameSet *main = bfl.getOrCreate(KWord::MainTextFrameSet, page);
    QVERIFY(main);
    QCOMPARE(bfl.hasFrameOn(main, 1), false);

    KoShape *shape = new MockTextShape();
    new KWTextFrame(shape, main);
    QCOMPARE(main->frameCount(), 1);

    QCOMPARE(bfl.hasFrameOn(main, 1), true);

    bfl.createNewFramesForPage(1);
    QCOMPARE(main->frameCount(), 1);
}

void TestBasicLayout::testShouldHaveHeaderOrFooter() {
    Helper helper;
    m_frames.clear();
    KWFrameLayout bfl(helper.pageManager, m_frames);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    // test the first page
    helper.pageSettings->setHeaderPolicy(KWord::HFTypeNone);
    helper.pageSettings->setFooterPolicy(KWord::HFTypeNone);
    KWord::TextFrameSetType origin;
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, true, &origin), false);  // header
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, false, &origin), false); // footer

    helper.pageSettings->setHeaderPolicy(KWord::HFTypeEvenOdd);
    helper.pageSettings->setFooterPolicy(KWord::HFTypeUniform);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, true, &origin), true);
    QCOMPARE(origin, KWord::OddPagesHeaderTextFrameSet);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, false, &origin), true);
    QCOMPARE(origin, KWord::OddPagesFooterTextFrameSet);

    helper.pageSettings->setHeaderPolicy(KWord::HFTypeUniform);
    helper.pageSettings->setFooterPolicy(KWord::HFTypeEvenOdd);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, true, &origin), true);
    QCOMPARE(origin, KWord::OddPagesHeaderTextFrameSet);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, false, &origin), true);
    QCOMPARE(origin, KWord::OddPagesFooterTextFrameSet);

    // append the second page, same pageSettings like the first
    helper.pageManager->appendPage();
    QCOMPARE(helper.pageManager->page(1)->pageSettings(), helper.pageManager->page(2)->pageSettings());

    // append the theird page with another pagesettings
    KWPageSettings* pagesettings3 = new KWPageSettings("Page3PageSettings");
    helper.pageManager->addPageSettings(pagesettings3); // takes over ownership
    helper.pageManager->appendPage(pagesettings3);
    QCOMPARE(helper.pageManager->page(3)->pageSettings(), pagesettings3);

    // test the second page
    helper.pageSettings->setHeaderPolicy(KWord::HFTypeNone);
    helper.pageSettings->setFooterPolicy(KWord::HFTypeNone);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(2, true, &origin), false);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(2, false, &origin), false);

    helper.pageSettings->setHeaderPolicy(KWord::HFTypeEvenOdd);
    helper.pageSettings->setFooterPolicy(KWord::HFTypeUniform);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(2, true, &origin), true);
    QCOMPARE(origin, KWord::EvenPagesHeaderTextFrameSet);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(2, false, &origin), true);
    QCOMPARE(origin, KWord::OddPagesFooterTextFrameSet);

    // test the theird page
    pagesettings3->setHeaderPolicy(KWord::HFTypeEvenOdd);
    pagesettings3->setFooterPolicy(KWord::HFTypeUniform);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(3, true, &origin), true);
    QCOMPARE(origin, KWord::OddPagesHeaderTextFrameSet);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(3, false, &origin), true);
    QCOMPARE(origin, KWord::OddPagesFooterTextFrameSet);

    pagesettings3->setHeaderPolicy(KWord::HFTypeNone);
    pagesettings3->setFooterPolicy(KWord::HFTypeNone);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(3, true, &origin), false);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(3, false, &origin), false);

    // test the first and the second pages again to be sure they still have there prev values
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, true, &origin), true);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(2, true, &origin), true);
}

// helper method (slot)
void TestBasicLayout::addFS(KWFrameSet*fs) {
    m_frames.append(fs);
}

QTEST_KDEMAIN(TestBasicLayout, GUI)

#include "TestBasicLayout.moc"
