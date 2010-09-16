#include "TestFrameLayout.h"
#include "TestDocumentLayout.h"

#include "../KWPageManager.h"
#include "../KWPage.h"
#include "../KWPageStyle.h"
#include "../frames/KWFrameLayout.h"
#include "../frames/KWTextFrameSet.h"
#include "../frames/KWTextFrame.h"
#include "../frames/KWCopyShape.h"
#include "../KWDocument.h"
#include "../KWord.h"

#include <MockShapes.h>

#include <KoTextDocumentLayout.h>
#include <KoColorBackground.h>
#include <QTextCursor>

#include <kcomponentdata.h>

class Helper
{
public:
    Helper() {
        pageManager = new KWPageManager();
        KWPage page = pageManager->appendPage();
        KoPageLayout pageLayout = page.pageStyle().pageLayout();
        pageLayout.width = 200;
        pageLayout.height = 200;
        page.pageStyle().setPageLayout(pageLayout);
        pageStyle = page.pageStyle();
    }
    ~Helper() {
        delete pageManager;
    }

    KWPageManager *pageManager;
    KWPageStyle pageStyle;
};

TestFrameLayout::TestFrameLayout()
{
    new KComponentData("TestFrameLayout");
}

void TestFrameLayout::testGetOrCreateFrameSet()
{
    Helper helper;
    m_frames.clear();
    KWPage page = helper.pageManager->page(1);
    KWFrameLayout bfl(helper.pageManager, m_frames);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    KWTextFrameSet *fs = bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page);
    QVERIFY(fs);
    QCOMPARE(fs->textFrameSetType(), KWord::OddPagesHeaderTextFrameSet);

    KWTextFrameSet *fs2 = bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page);
    QVERIFY(fs2);
    QCOMPARE(fs, fs2);
    QVERIFY(m_frames.contains(fs2));

    KWTextFrameSet *main = new KWTextFrameSet(0, KWord::MainTextFrameSet);
    m_frames.append(main);
    bfl.m_setup = false;
    KWTextFrameSet *main2 = bfl.getOrCreate(KWord::MainTextFrameSet, page);
    QVERIFY(main2);
    QCOMPARE(main, main2);
    QCOMPARE(main->textFrameSetType(), KWord::MainTextFrameSet);
}

void TestFrameLayout::testCopyShapes()
{
//    Helper helper;
//    m_frames.clear();
//    KWPage page = helper.pageManager->page(1);
//    KWFrameLayout bfl(helper.pageManager, m_frames);
//    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));
//
//    KWTextFrameSet *fs = bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page);
//    m_frames.append(fs);
//    bfl.m_setup = false;
//
//    helper.pageStyle.setHeaderPolicy(KWord::HFTypeEvenOdd);
//
//    KWTextFrame *tf = createFrame(QPointF(0,0), *fs);
//    KWFrame *cf = createCopyFrame(QPointF(0,300), tf->shape(), *fs);
//    QVERIFY(fs->frameCount()==2);
//    //FIXME QVERIFY(bfl.hasFrameOn(fs, 1));
//    delete tf->shape();
//
//    QVERIFY(fs->frameCount()==1);
//    //FIXME QVERIFY(!bfl.hasFrameOn(fs, 1));
//
//    //now try and add a copyframe without crashing
//    bfl.createNewFramesForPage(1);
}

void TestFrameLayout::testCreateNewFramesForPage()
{
    Helper helper;
    m_frames.clear();
    QVERIFY(m_frames.count() == 0);
    KWFrameLayout bfl(helper.pageManager, m_frames);
    KWPage page = helper.pageManager->page(1);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    KWTextFrameSet *main = bfl.getOrCreate(KWord::MainTextFrameSet, page);
    QVERIFY(main);
    QVERIFY(bfl.frameOn(main, 1) == 0);

    KoShape *shape = new MockTextShape();
    new KWTextFrame(shape, main);
    QCOMPARE(main->frameCount(), 1);

    QVERIFY(bfl.frameOn(main, 1));

    bfl.createNewFramesForPage(1);
    QCOMPARE(main->frameCount(), 1);
}

void TestFrameLayout::testShouldHaveHeaderOrFooter()
{
    Helper helper;
    m_frames.clear();
    KWFrameLayout bfl(helper.pageManager, m_frames);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    // test the first page
    helper.pageStyle.setHeaderPolicy(KWord::HFTypeNone);
    helper.pageStyle.setFooterPolicy(KWord::HFTypeNone);
    KWord::TextFrameSetType origin;
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, true, &origin), false);  // header
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, false, &origin), false); // footer

    helper.pageStyle.setHeaderPolicy(KWord::HFTypeEvenOdd);
    helper.pageStyle.setFooterPolicy(KWord::HFTypeUniform);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, true, &origin), true);
    QCOMPARE(origin, KWord::OddPagesHeaderTextFrameSet);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, false, &origin), true);
    QCOMPARE(origin, KWord::OddPagesFooterTextFrameSet);

    helper.pageStyle.setHeaderPolicy(KWord::HFTypeUniform);
    helper.pageStyle.setFooterPolicy(KWord::HFTypeEvenOdd);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, true, &origin), true);
    QCOMPARE(origin, KWord::OddPagesHeaderTextFrameSet);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, false, &origin), true);
    QCOMPARE(origin, KWord::OddPagesFooterTextFrameSet);

    // append the second page, same pageStyle like the first
    helper.pageManager->appendPage();
    QVERIFY(helper.pageManager->page(1).pageStyle() == helper.pageManager->page(2).pageStyle());

    // append the theird page with another pagesettings
    KWPageStyle pagesettings3("Page3PageStyle");
    helper.pageManager->addPageStyle(pagesettings3);
    helper.pageManager->appendPage(pagesettings3);
    QVERIFY(helper.pageManager->page(3).pageStyle() == pagesettings3);

    // test the second page
    helper.pageStyle.setHeaderPolicy(KWord::HFTypeNone);
    helper.pageStyle.setFooterPolicy(KWord::HFTypeNone);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(2, true, &origin), false);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(2, false, &origin), false);

    helper.pageStyle.setHeaderPolicy(KWord::HFTypeEvenOdd);
    helper.pageStyle.setFooterPolicy(KWord::HFTypeUniform);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(2, true, &origin), true);
    QCOMPARE(origin, KWord::EvenPagesHeaderTextFrameSet);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(2, false, &origin), true);
    QCOMPARE(origin, KWord::OddPagesFooterTextFrameSet);

    // test the 3rd page
    pagesettings3.setHeaderPolicy(KWord::HFTypeEvenOdd);
    pagesettings3.setFooterPolicy(KWord::HFTypeUniform);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(3, true, &origin), true);
    QCOMPARE(origin, KWord::OddPagesHeaderTextFrameSet);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(3, false, &origin), true);
    QCOMPARE(origin, KWord::OddPagesFooterTextFrameSet);

    pagesettings3.setHeaderPolicy(KWord::HFTypeNone);
    pagesettings3.setFooterPolicy(KWord::HFTypeNone);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(3, true, &origin), false);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(3, false, &origin), false);

    // test the first and the second pages again to be sure they still have there prev values
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, true, &origin), true);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(2, true, &origin), true);
}

void TestFrameLayout::headerPerPage()
{
    Helper helper;
    m_frames.clear();
    KWPage page = helper.pageManager->begin();
    KWFrameLayout bfl(helper.pageManager, m_frames);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    KWPageStyle myStyle("myStyle");
    myStyle.setHeaderPolicy(KWord::HFTypeUniform);
    helper.pageManager->addPageStyle(myStyle);
    KWPage page2 = helper.pageManager->appendPage(myStyle);
    QVERIFY(page.pageStyle() != page2.pageStyle());
    QCOMPARE(bfl.m_pageStyles.count(), 0);

    KWTextFrameSet *fs = bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page);
    QVERIFY(fs);
    QCOMPARE(fs->textFrameSetType(), KWord::OddPagesHeaderTextFrameSet);

    QCOMPARE(bfl.m_pageStyles.count(), 1);
    QVERIFY(bfl.m_pageStyles.contains(page.pageStyle()));
    KWFrameLayout::FrameSets fsets = bfl.m_pageStyles[page.pageStyle()];
    QCOMPARE(fsets.oddHeaders, fs);
    QCOMPARE(fsets.evenHeaders, (void*) 0);
    QCOMPARE(fsets.oddFooters, (void*) 0);
    QCOMPARE(fsets.evenFooters, (void*) 0);

    KWTextFrameSet *fs2 = bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page2);
    QVERIFY(fs2);
    QCOMPARE(fs2->textFrameSetType(), KWord::OddPagesHeaderTextFrameSet);

    QVERIFY(fs != fs2);
    QCOMPARE(bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page2), fs2);
    QCOMPARE(bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page), fs);

    QCOMPARE(bfl.m_pageStyles.count(), 2);
    QVERIFY(bfl.m_pageStyles.contains(page.pageStyle()));
    QVERIFY(bfl.m_pageStyles.contains(page2.pageStyle()));
    fsets = bfl.m_pageStyles[page.pageStyle()];
    QCOMPARE(fsets.oddHeaders, fs);
    QCOMPARE(fsets.evenHeaders, (void*) 0);
    QCOMPARE(fsets.oddFooters, (void*) 0);
    QCOMPARE(fsets.evenFooters, (void*) 0);
    KWFrameLayout::FrameSets fsets2 = bfl.m_pageStyles[page2.pageStyle()];
    QCOMPARE(fsets2.oddHeaders, fs2);
    QCOMPARE(fsets2.evenHeaders, (void*) 0);
    QCOMPARE(fsets2.oddFooters, (void*) 0);
    QCOMPARE(fsets2.evenFooters, (void*) 0);
}

void TestFrameLayout::testFrameCreation()
{
    Helper helper;
    m_frames.clear();
    KWFrameLayout bfl(helper.pageManager, m_frames);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    KWPageStyle style = helper.pageManager->defaultPageStyle();
    style.setHeaderPolicy(KWord::HFTypeUniform);
    style.setHasMainTextFrame(true);

    bfl.createNewFramesForPage(1);
    QVERIFY(bfl.m_maintext != 0);
    QCOMPARE(bfl.m_maintext->frameCount(), 1);

    KWFrameLayout::FrameSets frameSets = bfl.m_pageStyles.value(style);
    QVERIFY(frameSets.oddHeaders != 0);
    QCOMPARE(frameSets.oddHeaders->frameCount(), 1);
    QVERIFY(frameSets.evenHeaders == 0);
    QVERIFY(frameSets.oddFooters == 0);
    QVERIFY(frameSets.evenFooters == 0);

    KoColumns columns = style.columns();
    columns.columns = 2;
    style.setColumns(columns);

    removeAllFrames();
    bfl.createNewFramesForPage(1);
    QCOMPARE(bfl.m_maintext->frameCount(), 2);

    frameSets = bfl.m_pageStyles.value(style);
    QVERIFY(frameSets.oddHeaders != 0);
    QCOMPARE(frameSets.oddHeaders->frameCount(), 1);
    QVERIFY(frameSets.evenHeaders == 0);
    QVERIFY(frameSets.oddFooters == 0);
    QVERIFY(frameSets.evenFooters == 0);
}

void TestFrameLayout::testCreateNewFrameForPage_data()
{
    // tests void KWFrameLayout::createNewFrameForPage(KWTextFrameSet *fs, int pageNumber)
    QTest::addColumn<QStringList>("pages");
    QTest::addColumn<int>("frameSetType");
    QTest::addColumn<int>("pageNumber");
    QTest::addColumn<int>("expectedFrameCount");

    QTest::newRow("noFooter1") << (QStringList() << QString("style1") << QString("style2")) <<
        (int) KWord::OddPagesFooterTextFrameSet << 1 << 0;
    QTest::newRow("noFooter2") << (QStringList() << QString("style1") << QString("style2")) <<
        (int) KWord::EvenPagesFooterTextFrameSet << 1 << 0;
    QTest::newRow("noFooter3") << (QStringList() << QString("style1") << QString("style2")) <<
        (int) KWord::EvenPagesFooterTextFrameSet << 2 << 0;

    QTest::newRow("noHeader1") << (QStringList() << QString("style1") << QString("style2")) <<
        (int) KWord::OddPagesHeaderTextFrameSet << 1 << 0;
    QTest::newRow("noHeader2") << (QStringList() << QString("style1") << QString("style2")) <<
        (int) KWord::OddPagesHeaderTextFrameSet << 2 << 0;
    QTest::newRow("noHeader3") << (QStringList() << QString("style1") << QString("style2")) <<
        (int) KWord::EvenPagesHeaderTextFrameSet << 1 << 0;

    QTest::newRow("oddHeader1") << (QStringList() << QString("style2") << QString("style2")) <<
        (int) KWord::OddPagesHeaderTextFrameSet << 1 << 1;
    QTest::newRow("oddHeader2") << (QStringList() << QString("style2") << QString("style2")) <<
        (int) KWord::OddPagesHeaderTextFrameSet << 2 << 0;
    QTest::newRow("evenHeader1") << (QStringList() << QString("style2") << QString("style2")) <<
        (int) KWord::EvenPagesHeaderTextFrameSet << 1 << 0;
    QTest::newRow("evenHeader2") << (QStringList() << QString("style2") << QString("style2")) <<
        (int) KWord::EvenPagesHeaderTextFrameSet << 2 << 1;

    QTest::newRow("main1") << (QStringList() << QString("style1") << QString("style3") << QString("style4")) <<
        (int) KWord::MainTextFrameSet << 1 << 1;
    QTest::newRow("main2") << (QStringList() << QString("style1") << QString("style3") << QString("style4")) <<
        (int) KWord::MainTextFrameSet << 2 << 0;
    QTest::newRow("main3") << (QStringList() << QString("style1") << QString("style3") << QString("style4")) <<
        (int) KWord::MainTextFrameSet << 3 << 2;
    QTest::newRow("main4") << (QStringList() << QString("style5")) <<
        (int) KWord::MainTextFrameSet << 1 << 0;

    QTest::newRow("footer1") << (QStringList() << QString("style3") << QString("style5") << QString("style2")) <<
        (int) KWord::EvenPagesFooterTextFrameSet << 1 << 0; // uniform goes to the odd
    QTest::newRow("footer2") << (QStringList() << QString("style3") << QString("style5") << QString("style2")) <<
        (int) KWord::EvenPagesFooterTextFrameSet << 2 << 0;
    QTest::newRow("footer3") << (QStringList() << QString("style3") << QString("style5") << QString("style2")) <<
        (int) KWord::EvenPagesFooterTextFrameSet << 3 << 0; // uniform goes to the odd

    QTest::newRow("footer4") << (QStringList() << QString("style3") << QString("style5") << QString("style2")) <<
        (int) KWord::OddPagesFooterTextFrameSet << 1 << 1;
    QTest::newRow("footer5") << (QStringList() << QString("style3") << QString("style5") << QString("style2")) <<
        (int) KWord::OddPagesFooterTextFrameSet << 2 << 0;
    QTest::newRow("footer6") << (QStringList() << QString("style3") << QString("style5") << QString("style2")) <<
        (int) KWord::OddPagesFooterTextFrameSet << 3 << 1;
}

void TestFrameLayout::testCreateNewFrameForPage()
{
    QFETCH(QStringList, pages);
    QFETCH(int, frameSetType);
    QFETCH(int, pageNumber);
    QFETCH(int, expectedFrameCount);

    QHash<QString, KWPageStyle> styles;
    KWPageStyle style1("style1");
    style1.setHeaderPolicy(KWord::HFTypeNone);
    style1.setHasMainTextFrame(true);
    style1.setFooterPolicy(KWord::HFTypeNone);
    styles.insert(style1.name(), style1);

    KWPageStyle style2("style2");
    style2.setHeaderPolicy(KWord::HFTypeEvenOdd);
    style2.setHasMainTextFrame(true);
    style2.setFooterPolicy(KWord::HFTypeUniform);
    styles.insert(style2.name(), style2);

    KWPageStyle style3("style3"); // weird
    style3.setHeaderPolicy(KWord::HFTypeEvenOdd);
    style3.setHasMainTextFrame(false);
    style3.setFooterPolicy(KWord::HFTypeUniform);
    styles.insert(style3.name(), style3);

    KWPageStyle style4("style4");
    style4.setHeaderPolicy(KWord::HFTypeUniform);
    style4.setHasMainTextFrame(true);
    style4.setFooterPolicy(KWord::HFTypeEvenOdd);
    KoColumns columns;
    columns.columns = 2;
    columns.columnSpacing = 4;
    style4.setColumns(columns);
    styles.insert(style4.name(), style4);

    KWPageStyle style5("style5"); // blank
    style5.setHeaderPolicy(KWord::HFTypeNone);
    style5.setHasMainTextFrame(false);
    style5.setFooterPolicy(KWord::HFTypeNone);
    style5.setColumns(columns);
    styles.insert(style5.name(), style5);

    KWPageManager manager;
    foreach (const QString &styleName, pages) {
        QVERIFY(styles.contains(styleName));
        manager.appendPage(styles[styleName]);
    }

    m_frames.clear();
    KWTextFrameSet tfs(0, (KWord::TextFrameSetType) frameSetType);
    m_frames << &tfs;
    KWFrameLayout frameLayout(&manager, m_frames);
    connect(&frameLayout, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    KWPage page = manager.page(pageNumber);
    QVERIFY(page.isValid());
    tfs.setPageStyle(page.pageStyle());

    frameLayout.createNewFramesForPage(pageNumber);
    QCOMPARE(tfs.frameCount(), expectedFrameCount);
    foreach(KWFrame *frame, tfs.frames()) {
        QVERIFY (page.rect().contains(frame->shape()->position()));
    }
}

void TestFrameLayout::testLargeHeaders()
{
    // create a header with waaaaaaay to much text and do one page layout.
    // Check if the header has been trunkated and no new page has been requested.
    Helper helper;
    m_frames.clear();
    KWPage page = helper.pageManager->begin();
    helper.pageStyle.setHeaderPolicy(KWord::HFTypeUniform);

    KWFrameLayout bfl(helper.pageManager, m_frames);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    KWTextFrameSet *fs = bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page);
    QVERIFY(fs);
    QCOMPARE(fs->frameCount(), 0);
    bfl.createNewFramesForPage(page.pageNumber());
    QCOMPARE(fs->frameCount(), 1);

    // now we have to make sure the header looks pretty full
    KWTextFrame *tf = dynamic_cast<KWTextFrame*>(fs->frames().at(0));
    QVERIFY(tf);
    tf->setMinimumFrameHeight(300);
    bfl.layoutFramesOnPage(page.pageNumber());
    QCOMPARE(fs->frameCount(), 1);

    KoShape *shape = fs->frames()[0]->shape();
    QVERIFY(shape->size().width() <= 200);
    // the header can never be bigger than a page.
    QVERIFY(shape->size().height() < 180);

    // the header can never force the main text fs to get too small
    KWTextFrameSet *mfs = bfl.getOrCreate(KWord::MainTextFrameSet, page);
    QVERIFY(mfs);
    QCOMPARE(mfs->frameCount(), 1);
    shape = mfs->frames()[0]->shape();
    QVERIFY(shape->size().height() >= 10);
}

void TestFrameLayout::testLayoutPageSpread()
{
    Helper helper;
    m_frames.clear();

    //set up as a page spread;
    KoPageLayout pageLayout = helper.pageStyle.pageLayout();
    pageLayout.leftMargin = -1;
    pageLayout.rightMargin = -1;
    pageLayout.pageEdge = 20;
    pageLayout.bindingSide = 25;
    pageLayout.topMargin = 21;
    pageLayout.bottomMargin = 22;
    helper.pageStyle.setPageLayout(pageLayout);

    KWPage spread = helper.pageManager->appendPage();
    QCOMPARE(spread.pageSide(), KWPage::PageSpread);
    QCOMPARE(spread.pageNumber(), 2);

    KWFrameLayout bfl(helper.pageManager, m_frames);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    bfl.createNewFramesForPage(spread.pageNumber());
    KWTextFrameSet *fs = bfl.getOrCreate(KWord::MainTextFrameSet, spread);
    QCOMPARE(fs->frameCount(), 2);
    bfl.layoutFramesOnPage(spread.pageNumber());
    QCOMPARE(fs->frames()[0]->shape()->position(), QPointF(20, 221)); // left
    QCOMPARE(fs->frames()[0]->shape()->size(), QSizeF(155, 157));
    QCOMPARE(fs->frames()[1]->shape()->position(), QPointF(225, 221)); // right
    QCOMPARE(fs->frames()[1]->shape()->size(), QSizeF(155, 157));
}

void TestFrameLayout::testPageStyle()
{
    // on different page styles i want different framesets.
    // changing a page (in a sequence) to get a different style should
    // thus delete all auto-generated frames on that page and force
    // new ones to be created.

    Helper helper;
    m_frames.clear();
    KWFrameLayout bfl(helper.pageManager, m_frames);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    KWPage page1 = helper.pageManager->page(1);
    page1.pageStyle().setHeaderPolicy(KWord::HFTypeUniform);

    KWPageStyle style2 = page1.pageStyle();
    style2.detach("Style2"); // make it a copy of first style, but with new name
    helper.pageManager->addPageStyle(style2);
    KWPage page2 = helper.pageManager->appendPage();
    QCOMPARE(page1.pageStyle(), page2.pageStyle());
    KWPage page3 = helper.pageManager->appendPage(style2);
    QCOMPARE(page3.pageStyle(), style2);
    KWPage page4 = helper.pageManager->appendPage();
    QCOMPARE(page1.pageStyle(), page2.pageStyle());
    QCOMPARE(page3.pageStyle(), style2);
    QCOMPARE(page4.pageStyle(), style2);

    bfl.createNewFramesForPage(1);
    // mainFs is special; there is only one across all page styles
    QVERIFY(bfl.m_maintext);
    KWTextFrameSet *mainFs = bfl.getOrCreate(KWord::MainTextFrameSet, page1);
    QCOMPARE(bfl.m_maintext, mainFs);
    bfl.createNewFramesForPage(2);
    QCOMPARE(bfl.getOrCreate(KWord::MainTextFrameSet, page2), mainFs);
    QVERIFY(!bfl.m_pageStyles.contains(style2));
    bfl.createNewFramesForPage(3);
    QVERIFY(bfl.m_pageStyles.contains(style2));
    QCOMPARE(bfl.getOrCreate(KWord::MainTextFrameSet, page3), mainFs);
    bfl.createNewFramesForPage(4);
    QCOMPARE(bfl.getOrCreate(KWord::MainTextFrameSet, page1), mainFs);
    QCOMPARE(bfl.getOrCreate(KWord::MainTextFrameSet, page2), mainFs);
    QCOMPARE(bfl.getOrCreate(KWord::MainTextFrameSet, page3), mainFs);
    QCOMPARE(bfl.getOrCreate(KWord::MainTextFrameSet, page4), mainFs);

    KWFrameLayout::FrameSets fsets1 = bfl.m_pageStyles.value(page1.pageStyle());
    KWFrameLayout::FrameSets fsets2 = bfl.m_pageStyles.value(style2);
    QVERIFY(fsets1.oddHeaders);
    QVERIFY(fsets2.oddHeaders);
    QVERIFY(fsets1.oddHeaders != fsets2.oddHeaders);
    QCOMPARE(bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page1), fsets1.oddHeaders);
    QCOMPARE(bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page2), fsets1.oddHeaders);
    QCOMPARE(bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page3), fsets2.oddHeaders);
    QCOMPARE(bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page4), fsets2.oddHeaders);
    QCOMPARE(fsets1.oddHeaders->frameCount(), 2);
    QCOMPARE(fsets2.oddHeaders->frameCount(), 2);

    QVERIFY(bfl.frameOn(fsets1.oddHeaders, 1));
    QVERIFY(bfl.frameOn(fsets1.oddHeaders, 2));
    QVERIFY(bfl.frameOn(fsets1.oddHeaders, 3) == 0);
    QVERIFY(bfl.frameOn(fsets1.oddHeaders, 4) == 0);

    QVERIFY(bfl.frameOn(fsets2.oddHeaders, 1) == 0);
    QVERIFY(bfl.frameOn(fsets2.oddHeaders, 2) == 0);
    QVERIFY(bfl.frameOn(fsets2.oddHeaders, 3));
    QVERIFY(bfl.frameOn(fsets2.oddHeaders, 4));

    // now we change one and check if the frame moved
    page2.setPageStyle(style2);
    bfl.createNewFramesForPage(2);

    fsets1 = bfl.m_pageStyles.value(page1.pageStyle());
    fsets2 = bfl.m_pageStyles.value(style2);
    QVERIFY(fsets1.oddHeaders);
    QVERIFY(fsets2.oddHeaders);
    QVERIFY(fsets1.oddHeaders != fsets2.oddHeaders);
    QVERIFY(bfl.frameOn(fsets1.oddHeaders, 1));
    QVERIFY(bfl.frameOn(fsets1.oddHeaders, 2) == 0);
    QVERIFY(bfl.frameOn(fsets1.oddHeaders, 3) == 0);
    QVERIFY(bfl.frameOn(fsets1.oddHeaders, 4) == 0);

    QVERIFY(bfl.frameOn(fsets2.oddHeaders, 1) == 0);
    QVERIFY(bfl.frameOn(fsets2.oddHeaders, 2));
    QVERIFY(bfl.frameOn(fsets2.oddHeaders, 3));
    QVERIFY(bfl.frameOn(fsets2.oddHeaders, 4));
    QCOMPARE(bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page1), fsets1.oddHeaders);
    QCOMPARE(bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page2), fsets2.oddHeaders);
    QCOMPARE(bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page3), fsets2.oddHeaders);
    QCOMPARE(bfl.getOrCreate(KWord::OddPagesHeaderTextFrameSet, page4), fsets2.oddHeaders);
    QCOMPARE(fsets1.oddHeaders->frameCount(), 1);
    QCOMPARE(fsets2.oddHeaders->frameCount(), 3);
}
#include <qdebug.h>
void TestFrameLayout::testPageBackground()
{
    // creating a page with a pagestyle that has a background set should
    // trigger the creation of a shape that draws the page-background.
    // If there is no background or its removed (in a command) that should
    // remove the frame.
    Helper helper;
    m_frames.clear();
    KWFrameLayout bfl(helper.pageManager, m_frames);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    KWPage page1 = helper.pageManager->page(1);
    page1.pageStyle().setBackground(new KoColorBackground(Qt::red));

    KWPageStyle style2("No Background");
    helper.pageManager->addPageStyle(style2);
    KWPage page2 = helper.pageManager->appendPage();
    KWPage page3 = helper.pageManager->appendPage(style2);
    KWPage page4 = helper.pageManager->appendPage();

    QVERIFY(bfl.m_backgroundFrameSet == 0);
    bfl.createNewFramesForPage(1);
    QVERIFY(bfl.m_backgroundFrameSet);
    QCOMPARE(bfl.m_backgroundFrameSet->frameCount(), 1);
    bfl.createNewFramesForPage(2);
    QCOMPARE(bfl.m_backgroundFrameSet->frameCount(), 2);
    bfl.createNewFramesForPage(3);
    QVERIFY(bfl.m_backgroundFrameSet);
    QCOMPARE(bfl.m_backgroundFrameSet->frameCount(), 2);
    bfl.createNewFramesForPage(4);
    QCOMPARE(bfl.m_backgroundFrameSet->frameCount(), 2);

    foreach (KWFrame *frame, bfl.m_backgroundFrameSet->frames()) {
        QCOMPARE(frame->shape()->background(), page1.pageStyle().background());
    }

    QCOMPARE(bfl.frameOn(bfl.m_backgroundFrameSet,1)->shape()->position(), QPointF(0, 0)); //page 1 background position and size
    QCOMPARE(bfl.frameOn(bfl.m_backgroundFrameSet,1)->shape()->size(), QSizeF(200, 200));
    QCOMPARE(bfl.frameOn(bfl.m_backgroundFrameSet,2)->shape()->position(), QPointF(0, 200)); //page 2 background position and size
    QCOMPARE(bfl.frameOn(bfl.m_backgroundFrameSet,2)->shape()->size(), QSizeF(200, 200));
}


// helper method (slot)
void TestFrameLayout::addFS(KWFrameSet*fs)
{
    m_frames.append(fs);
}

void TestFrameLayout::removeAllFrames()
{
    foreach (KWFrameSet *fs, m_frames) {
        foreach (KWFrame *frame, fs->frames()) {
            fs->removeFrame(frame);
            delete frame->shape();
        }
    }
}

KWTextFrame *TestFrameLayout::createFrame(const QPointF &position, KWTextFrameSet &fs)
{
    MockShape *shape = new MockShape();
    shape->setUserData(new KoTextShapeData());
    KWTextFrame *frame = new KWTextFrame(shape, &fs);
    shape->setPosition(position);
    return frame;
}

KWFrame *TestFrameLayout::createCopyFrame(const QPointF &position, KoShape *orig, KWTextFrameSet &fs)
{
    KoShape *shape = new KWCopyShape(orig);
    KWFrame *frame = new KWFrame(shape, &fs);
    shape->setPosition(position);
    return frame;
}

QTEST_KDEMAIN(TestFrameLayout, GUI)

#include <TestFrameLayout.moc>
