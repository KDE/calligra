#include "TestFrameLayout.h"
#include "TestDocumentLayout.h"

#include "../KWPageManager.h"
#include "../KWPage.h"
#include "../KWPageStyle.h"
#include "../frames/KWFrameLayout.h"
#include "../frames/KWCopyShape.h"
#include "../frames/KWTextFrameSet.h"
#include "../frames/KWTextFrame.h"
#include "../KWDocument.h"
#include "../Words.h"

#include <MockShapes.h>
#include <MockTextShape.h>

#include <KoTextDocumentLayout.h>
#include <KoColorBackground.h>
#include <QTextCursor>

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
}

void TestFrameLayout::testGetOrCreateFrameSet()
{
    Helper helper;
    m_frames.clear();
    KWPage page = helper.pageManager->page(1);
    KWFrameLayout bfl(helper.pageManager, m_frames);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    KWTextFrameSet *fs = bfl.getOrCreate(Words::OddPagesHeaderTextFrameSet, page);
    QVERIFY(fs);
    QCOMPARE(fs->textFrameSetType(), Words::OddPagesHeaderTextFrameSet);

    KWTextFrameSet *fs2 = bfl.getOrCreate(Words::OddPagesHeaderTextFrameSet, page);
    QVERIFY(fs2);
    QCOMPARE(fs, fs2);
    QVERIFY(m_frames.contains(fs2));

    KWTextFrameSet *main = new KWTextFrameSet(0, Words::MainTextFrameSet);
    m_frames.append(main);
    bfl.m_setup = false;
    KWTextFrameSet *main2 = bfl.getOrCreate(Words::MainTextFrameSet, page);
    QVERIFY(main2);
    QCOMPARE(main, main2);
    QCOMPARE(main->textFrameSetType(), Words::MainTextFrameSet);
}

void TestFrameLayout::testCopyShapes()
{
    Helper helper;
    m_frames.clear();
    KWPage page = helper.pageManager->page(1);
    KWFrameLayout bfl(helper.pageManager, m_frames);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    KWTextFrameSet *fs = bfl.getOrCreate(Words::OddPagesHeaderTextFrameSet, page);
    m_frames.append(fs);
    bfl.m_setup = false;

    helper.pageStyle.setHeaderPolicy(Words::HFTypeEvenOdd);

    KWTextFrame *tf = createFrame(QPointF(0,0), *fs);
    KWFrame *cf = createCopyFrame(QPointF(0,300), tf->shape(), *fs);
    QVERIFY(fs->frameCount()==2);
    //FIXME QVERIFY(bfl.hasFrameOn(fs, 1));
    delete tf->shape();

    QVERIFY(fs->frameCount()==1);
    //FIXME QVERIFY(!bfl.hasFrameOn(fs, 1));

    //now try and add a copyframe without crashing
    //sebsauer; it's not crashing but asserting at KWFrameLayout.cpp:750 now
    //bfl.createNewFramesForPage(1);
}

void TestFrameLayout::testCreateNewFramesForPage()
{
    Helper helper;
    m_frames.clear();
    QVERIFY(m_frames.count() == 0);
    KWFrameLayout bfl(helper.pageManager, m_frames);
    KWPage page = helper.pageManager->page(1);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    KWTextFrameSet *main = bfl.getOrCreate(Words::MainTextFrameSet, page);
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
    helper.pageStyle.setHeaderPolicy(Words::HFTypeNone);
    helper.pageStyle.setFooterPolicy(Words::HFTypeNone);
    Words::TextFrameSetType origin;
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, true, &origin), false);  // header
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, false, &origin), false); // footer

    helper.pageStyle.setHeaderPolicy(Words::HFTypeEvenOdd);
    helper.pageStyle.setFooterPolicy(Words::HFTypeUniform);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, true, &origin), true);
    QCOMPARE(origin, Words::OddPagesHeaderTextFrameSet);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, false, &origin), true);
    QCOMPARE(origin, Words::OddPagesFooterTextFrameSet);

    helper.pageStyle.setHeaderPolicy(Words::HFTypeUniform);
    helper.pageStyle.setFooterPolicy(Words::HFTypeEvenOdd);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, true, &origin), true);
    QCOMPARE(origin, Words::OddPagesHeaderTextFrameSet);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(1, false, &origin), true);
    QCOMPARE(origin, Words::OddPagesFooterTextFrameSet);

    // append the second page, same pageStyle like the first
    helper.pageManager->appendPage();
    QVERIFY(helper.pageManager->page(1).pageStyle() == helper.pageManager->page(2).pageStyle());

    // append the third page with another pagesettings
    KWPageStyle pagesettings3("Page3PageStyle");
    helper.pageManager->addPageStyle(pagesettings3);
    helper.pageManager->appendPage(pagesettings3);
    QVERIFY(helper.pageManager->page(3).pageStyle() == pagesettings3);

    // test the second page
    helper.pageStyle.setHeaderPolicy(Words::HFTypeNone);
    helper.pageStyle.setFooterPolicy(Words::HFTypeNone);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(2, true, &origin), false);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(2, false, &origin), false);

    helper.pageStyle.setHeaderPolicy(Words::HFTypeEvenOdd);
    helper.pageStyle.setFooterPolicy(Words::HFTypeUniform);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(2, true, &origin), true);
    QCOMPARE(origin, Words::EvenPagesHeaderTextFrameSet);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(2, false, &origin), true);
    QCOMPARE(origin, Words::OddPagesFooterTextFrameSet);

    // test the 3rd page
    pagesettings3.setHeaderPolicy(Words::HFTypeEvenOdd);
    pagesettings3.setFooterPolicy(Words::HFTypeUniform);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(3, true, &origin), true);
    QCOMPARE(origin, Words::OddPagesHeaderTextFrameSet);
    QCOMPARE(bfl.shouldHaveHeaderOrFooter(3, false, &origin), true);
    QCOMPARE(origin, Words::OddPagesFooterTextFrameSet);

    pagesettings3.setHeaderPolicy(Words::HFTypeNone);
    pagesettings3.setFooterPolicy(Words::HFTypeNone);
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
    myStyle.setHeaderPolicy(Words::HFTypeUniform);
    helper.pageManager->addPageStyle(myStyle);
    KWPage page2 = helper.pageManager->appendPage(myStyle);
    QVERIFY(page.pageStyle() != page2.pageStyle());
    QCOMPARE(bfl.m_pageStyles.count(), 0);

    KWTextFrameSet *fs = bfl.getOrCreate(Words::OddPagesHeaderTextFrameSet, page);
    QVERIFY(fs);
    QCOMPARE(fs->textFrameSetType(), Words::OddPagesHeaderTextFrameSet);

    QCOMPARE(bfl.m_pageStyles.count(), 1);
    QVERIFY(bfl.m_pageStyles.contains(page.pageStyle()));
    KWFrameLayout::FrameSets fsets = bfl.m_pageStyles[page.pageStyle()];
    QCOMPARE(fsets.oddHeaders, fs);
    QCOMPARE(fsets.evenHeaders, (void*) 0);
    QCOMPARE(fsets.oddFooters, (void*) 0);
    QCOMPARE(fsets.evenFooters, (void*) 0);

    KWTextFrameSet *fs2 = bfl.getOrCreate(Words::OddPagesHeaderTextFrameSet, page2);
    QVERIFY(fs2);
    QCOMPARE(fs2->textFrameSetType(), Words::OddPagesHeaderTextFrameSet);

    QVERIFY(fs != fs2);
    QCOMPARE(bfl.getOrCreate(Words::OddPagesHeaderTextFrameSet, page2), fs2);
    QCOMPARE(bfl.getOrCreate(Words::OddPagesHeaderTextFrameSet, page), fs);

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
    style.setHeaderPolicy(Words::HFTypeUniform);
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
    columns.count = 2;
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
        (int) Words::OddPagesFooterTextFrameSet << 1 << 0;
    QTest::newRow("noFooter2") << (QStringList() << QString("style1") << QString("style2")) <<
        (int) Words::EvenPagesFooterTextFrameSet << 1 << 0;
    QTest::newRow("noFooter3") << (QStringList() << QString("style1") << QString("style2")) <<
        (int) Words::EvenPagesFooterTextFrameSet << 2 << 0;

    QTest::newRow("noHeader1") << (QStringList() << QString("style1") << QString("style2")) <<
        (int) Words::OddPagesHeaderTextFrameSet << 1 << 0;
    QTest::newRow("noHeader2") << (QStringList() << QString("style1") << QString("style2")) <<
        (int) Words::OddPagesHeaderTextFrameSet << 2 << 0;
    QTest::newRow("noHeader3") << (QStringList() << QString("style1") << QString("style2")) <<
        (int) Words::EvenPagesHeaderTextFrameSet << 1 << 0;

    QTest::newRow("oddHeader1") << (QStringList() << QString("style2") << QString("style2")) <<
        (int) Words::OddPagesHeaderTextFrameSet << 1 << 1;
    QTest::newRow("oddHeader2") << (QStringList() << QString("style2") << QString("style2")) <<
        (int) Words::OddPagesHeaderTextFrameSet << 2 << 0;
    QTest::newRow("evenHeader1") << (QStringList() << QString("style2") << QString("style2")) <<
        (int) Words::EvenPagesHeaderTextFrameSet << 1 << 0;
    QTest::newRow("evenHeader2") << (QStringList() << QString("style2") << QString("style2")) <<
        (int) Words::EvenPagesHeaderTextFrameSet << 2 << 1;

    QTest::newRow("main1") << (QStringList() << QString("style1") << QString("style3") << QString("style4")) <<
        (int) Words::MainTextFrameSet << 1 << 1;
    QTest::newRow("main2") << (QStringList() << QString("style1") << QString("style3") << QString("style4")) <<
        (int) Words::MainTextFrameSet << 2 << 0;
    QTest::newRow("main3") << (QStringList() << QString("style1") << QString("style3") << QString("style4")) <<
        (int) Words::MainTextFrameSet << 3 << 2;
    QTest::newRow("main4") << (QStringList() << QString("style5")) <<
        (int) Words::MainTextFrameSet << 1 << 0;

    QTest::newRow("footer1") << (QStringList() << QString("style3") << QString("style5") << QString("style2")) <<
        (int) Words::EvenPagesFooterTextFrameSet << 1 << 0; // uniform goes to the odd
    QTest::newRow("footer2") << (QStringList() << QString("style3") << QString("style5") << QString("style2")) <<
        (int) Words::EvenPagesFooterTextFrameSet << 2 << 0;
    QTest::newRow("footer3") << (QStringList() << QString("style3") << QString("style5") << QString("style2")) <<
        (int) Words::EvenPagesFooterTextFrameSet << 3 << 0; // uniform goes to the odd

    QTest::newRow("footer4") << (QStringList() << QString("style3") << QString("style5") << QString("style2")) <<
        (int) Words::OddPagesFooterTextFrameSet << 1 << 1;
    QTest::newRow("footer5") << (QStringList() << QString("style3") << QString("style5") << QString("style2")) <<
        (int) Words::OddPagesFooterTextFrameSet << 2 << 0;
    QTest::newRow("footer6") << (QStringList() << QString("style3") << QString("style5") << QString("style2")) <<
        (int) Words::OddPagesFooterTextFrameSet << 3 << 1;
}

void TestFrameLayout::testCreateNewFrameForPage()
{
    QFETCH(QStringList, pages);
    QFETCH(int, frameSetType);
    QFETCH(int, pageNumber);
    QFETCH(int, expectedFrameCount);

    QHash<QString, KWPageStyle> styles;
    KWPageStyle style1("style1");
    style1.setHeaderPolicy(Words::HFTypeNone);
    style1.setHasMainTextFrame(true);
    style1.setFooterPolicy(Words::HFTypeNone);
    styles.insert(style1.name(), style1);

    KWPageStyle style2("style2");
    style2.setHeaderPolicy(Words::HFTypeEvenOdd);
    style2.setHasMainTextFrame(true);
    style2.setFooterPolicy(Words::HFTypeUniform);
    styles.insert(style2.name(), style2);

    KWPageStyle style3("style3"); // weird
    style3.setHeaderPolicy(Words::HFTypeEvenOdd);
    style3.setHasMainTextFrame(false);
    style3.setFooterPolicy(Words::HFTypeUniform);
    styles.insert(style3.name(), style3);

    KWPageStyle style4("style4");
    style4.setHeaderPolicy(Words::HFTypeUniform);
    style4.setHasMainTextFrame(true);
    style4.setFooterPolicy(Words::HFTypeEvenOdd);
    KoColumns columns;
    columns.count = 2;
    columns.gapWidth = 4;
    style4.setColumns(columns);
    styles.insert(style4.name(), style4);

    KWPageStyle style5("style5"); // blank
    style5.setHeaderPolicy(Words::HFTypeNone);
    style5.setHasMainTextFrame(false);
    style5.setFooterPolicy(Words::HFTypeNone);
    style5.setColumns(columns);
    styles.insert(style5.name(), style5);

    KWPageManager manager;
    foreach (const QString &styleName, pages) {
        QVERIFY(styles.contains(styleName));
        manager.appendPage(styles[styleName]);
    }

    m_frames.clear();
    KWTextFrameSet tfs(0, (Words::TextFrameSetType) frameSetType);
    m_frames << &tfs;
    KWFrameLayout frameLayout(&manager, m_frames);
    connect(&frameLayout, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    KWPage page = manager.page(pageNumber);
    QVERIFY(page.isValid());
    tfs.setPageStyle(page.pageStyle());

    frameLayout.createNewFramesForPage(pageNumber);
    QCOMPARE(tfs.frameCount(), expectedFrameCount);
    foreach(KoShape *shape, tfs.shapes()) {
        QVERIFY (page.rect().contains(shape->position()));
    }
}

void TestFrameLayout::testCopyFramesForPage()
{
    Helper helper;
    m_frames.clear();
    KWPage page = helper.pageManager->begin();

    // copyShape
    MockShape *copyShape = new MockShape();
    copyShape->setPosition(QPointF(9, 13));
    KWFrameSet *copyShapeFrameSet = new KWFrameSet();
    KWFrame *frame = new KWFrame(copyShape, copyShapeFrameSet);
    frame->setNewFrameBehavior(Words::CopyNewFrame);
    m_frames << copyShapeFrameSet;

    // copyShapeOdd
    MockShape *copyShapeOdd = new MockShape();
    copyShapeOdd->setPosition(QPointF(7, 12));
    KWFrameSet *copyShapeOddFrameSet = new KWFrameSet();
    frame = new KWFrame(copyShapeOdd, copyShapeOddFrameSet);
    frame->setNewFrameBehavior(Words::CopyNewFrame);
    frame->setFrameOnBothSheets(false);
    m_frames << copyShapeOddFrameSet;

    // textshapePlain
    MockTextShape *textshapePlain = new MockTextShape();
    textshapePlain->setPosition(QPointF(11, 15));
    KWTextFrameSet *textshapePlainFS = new KWTextFrameSet(0, Words::OtherTextFrameSet);
    KWTextFrame *tFrame = new KWTextFrame(textshapePlain, textshapePlainFS);
    tFrame->setNewFrameBehavior(Words::ReconnectNewFrame);
    m_frames << textshapePlainFS;

    // textShapeRotated
    MockTextShape *textShapeRotated = new MockTextShape();
    textShapeRotated->setPosition(QPointF(13, 107));
    KWTextFrameSet *textshapeRotFS = new KWTextFrameSet(0, Words::OtherTextFrameSet);
    tFrame = new KWTextFrame(textShapeRotated, textshapeRotFS);
    tFrame->setNewFrameBehavior(Words::ReconnectNewFrame);
    tFrame->shape()->rotate(90);
    m_frames << textshapeRotFS;

    // textShapeGeometryProtected
    MockTextShape *textShapeGeometryProtected = new MockTextShape();
    textShapeGeometryProtected->setPosition(QPointF(3, 14));
    KWTextFrameSet *textshapeGeometryProtectedFS = new KWTextFrameSet(0, Words::OtherTextFrameSet);
    tFrame = new KWTextFrame(textShapeGeometryProtected, textshapeGeometryProtectedFS);
    tFrame->setNewFrameBehavior(Words::ReconnectNewFrame);
    tFrame->shape()->setGeometryProtected(true);
    m_frames << textshapeGeometryProtectedFS;

    // textShapeContentProtected
    MockTextShape *textShapeContentProtected = new MockTextShape();
    textShapeContentProtected->setPosition(QPointF(19, 23));
    KWTextFrameSet *textshapeContentProtectedFS = new KWTextFrameSet(0, Words::OtherTextFrameSet);
    tFrame = new KWTextFrame(textShapeContentProtected, textshapeContentProtectedFS);
    tFrame->setNewFrameBehavior(Words::ReconnectNewFrame);
    tFrame->shape()->setContentProtected(true);
    m_frames << textshapeContentProtectedFS;

    // textShapeUnselectable
    MockTextShape *textShapeUnselectable = new MockTextShape();
    textShapeUnselectable->setPosition(QPointF(7, 24));
    KWTextFrameSet *textshapeUnselectableFS = new KWTextFrameSet(0, Words::OtherTextFrameSet);
    tFrame = new KWTextFrame(textShapeUnselectable, textshapeUnselectableFS);
    tFrame->setNewFrameBehavior(Words::ReconnectNewFrame);
    tFrame->shape()->setSelectable(false);
    m_frames << textshapeUnselectableFS;

    // layouter
    KWFrameLayout bfl(helper.pageManager, m_frames);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));
    // new page
    KWPage page2 = helper.pageManager->appendPage();
    bfl.createNewFramesForPage(page2.pageNumber());

    QCOMPARE(copyShapeFrameSet->frameCount(), 2);
    KWCopyShape *copy = dynamic_cast<KWCopyShape*>(copyShapeFrameSet->shapes()[1]);
    QVERIFY(copy);
    QCOMPARE(copy->position().x(), 9.);
    QCOMPARE(copy->position().y(), 13. + page2.offsetInDocument());

    // copyShapeOddFrameSet is not copied
    QCOMPARE(copyShapeOddFrameSet->frameCount(), 1);

    // textshapePlain
    QCOMPARE(textshapePlainFS->frameCount(), 2);
    QVERIFY(!dynamic_cast<KWCopyShape*>(textshapePlainFS->shapes()[1]);
    KoShape *shape = textshapePlainFS->shapes()[1];
    QCOMPARE(shape->position().x(), 11.);
    QCOMPARE(shape->position().y(), 15. + page2.offsetInDocument());
    // TODO test sizing

    // textShapeRotated
    QCOMPARE(textshapeRotFS->frameCount(), 2);
    QVERIFY(!dynamic_cast<KWCopyShape*>(textshapeRotFS->shapes()[1]);
    shape = textshapeRotFS->shapes()[1];
    QCOMPARE(shape->position().x(), 13.);
    QCOMPARE(shape->position().y(), 107. + page2.offsetInDocument());
    QCOMPARE(shape->absolutePosition(KoFlake::TopRightCorner), QPointF(13 + 50, 107 + 50
        + page2.offsetInDocument())); // 90° around center moves the top-right down

    // textShapeGeometryProtected
    QCOMPARE(textshapeGeometryProtectedFS->frameCount(), 2);
    QVERIFY(!dynamic_cast<KWCopyShape*>(textshapeGeometryProtectedFS->shapes()[1]);
    shape = textshapeGeometryProtectedFS->shapes()[1];
    QCOMPARE(shape->position().x(), 3.);
    QCOMPARE(shape->position().y(), 14. + page2.offsetInDocument());
    QCOMPARE(shape->isGeometryProtected(), true);
    QCOMPARE(shape->isContentProtected(), false);
    QCOMPARE(shape->isSelectable(), true);

    // textShapeContentProtected
    QCOMPARE(textshapeContentProtectedFS->frameCount(), 2);
    QVERIFY(!dynamic_cast<KWCopyShape*>(textshapeContentProtectedFS->shapes()[1]);
    shape = textshapeContentProtectedFS->shapes()[1];
    QCOMPARE(shape->isGeometryProtected(), false);
    QCOMPARE(shape->isContentProtected(), true);
    QCOMPARE(shape->isSelectable(), true);

    // textShapeUnselectable
    QCOMPARE(textshapeUnselectableFS->frameCount(), 2);
    QVERIFY(!dynamic_cast<KWCopyShape*>(textshapeUnselectableFS->shapes()[1]);
    shape = textshapeUnselectableFS->shapes()[1];
    QCOMPARE(shape->isGeometryProtected(), false);
    QCOMPARE(shape->isContentProtected(), false);
    QCOMPARE(shape->isSelectable(), false);
}

void TestFrameLayout::testLargeHeaders()
{
    // create a header with waaaaaaay to much text and do one page layout.
    // Check if the header has been truncated and no new page has been requested.
    Helper helper;
    m_frames.clear();
    KWPage page = helper.pageManager->begin();
    helper.pageStyle.setHeaderPolicy(Words::HFTypeUniform);

    KWFrameLayout bfl(helper.pageManager, m_frames);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    KWTextFrameSet *fs = bfl.getOrCreate(Words::OddPagesHeaderTextFrameSet, page);
    QVERIFY(fs);
    QCOMPARE(fs->shapeCount(), 0);
    bfl.createNewFramesForPage(page.pageNumber());
    QCOMPARE(fs->shapeCount(), 1);

    KoShape *shape = fs->shapes()[0];
    QVERIFY(shape->size().width() <= 200);
    // the header can never be bigger than a page.
    QVERIFY(shape->size().height() < 180);

    // the header can never force the main text fs to get too small
    KWTextFrameSet *mfs = bfl.getOrCreate(Words::MainTextFrameSet, page);
    QVERIFY(mfs);
    QCOMPARE(mfs->shapeCount(), 1);
    shape = mfs->shapes()[0];
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
    KWTextFrameSet *fs = bfl.getOrCreate(Words::MainTextFrameSet, spread);
    QCOMPARE(fs->frameCount(), 2);
    bfl.layoutFramesOnPage(spread.pageNumber());
    QCOMPARE(fs->shapes()[0]->position(), QPointF(20, 221)); // left
    QCOMPARE(fs->shapes()[0]->size(), QSizeF(155, 157));
    QCOMPARE(fs->shapes()[1]->position(), QPointF(225, 221)); // right
    QCOMPARE(fs->shapes()[1]->size(), QSizeF(155, 157));
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
    page1.pageStyle().setHeaderPolicy(Words::HFTypeUniform);

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
    KWTextFrameSet *mainFs = bfl.getOrCreate(Words::MainTextFrameSet, page1);
    QCOMPARE(bfl.m_maintext, mainFs);
    bfl.createNewFramesForPage(2);
    QCOMPARE(bfl.getOrCreate(Words::MainTextFrameSet, page2), mainFs);
    QVERIFY(!bfl.m_pageStyles.contains(style2));
    bfl.createNewFramesForPage(3);
    QVERIFY(bfl.m_pageStyles.contains(style2));
    QCOMPARE(bfl.getOrCreate(Words::MainTextFrameSet, page3), mainFs);
    bfl.createNewFramesForPage(4);
    QCOMPARE(bfl.getOrCreate(Words::MainTextFrameSet, page1), mainFs);
    QCOMPARE(bfl.getOrCreate(Words::MainTextFrameSet, page2), mainFs);
    QCOMPARE(bfl.getOrCreate(Words::MainTextFrameSet, page3), mainFs);
    QCOMPARE(bfl.getOrCreate(Words::MainTextFrameSet, page4), mainFs);

    KWFrameLayout::FrameSets fsets1 = bfl.m_pageStyles.value(page1.pageStyle());
    KWFrameLayout::FrameSets fsets2 = bfl.m_pageStyles.value(style2);
    QVERIFY(fsets1.oddHeaders);
    QVERIFY(fsets2.oddHeaders);
    QVERIFY(fsets1.oddHeaders != fsets2.oddHeaders);
    QCOMPARE(bfl.getOrCreate(Words::OddPagesHeaderTextFrameSet, page1), fsets1.oddHeaders);
    QCOMPARE(bfl.getOrCreate(Words::OddPagesHeaderTextFrameSet, page2), fsets1.oddHeaders);
    QCOMPARE(bfl.getOrCreate(Words::OddPagesHeaderTextFrameSet, page3), fsets2.oddHeaders);
    QCOMPARE(bfl.getOrCreate(Words::OddPagesHeaderTextFrameSet, page4), fsets2.oddHeaders);
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
    QCOMPARE(bfl.getOrCreate(Words::OddPagesHeaderTextFrameSet, page1), fsets1.oddHeaders);
    QCOMPARE(bfl.getOrCreate(Words::OddPagesHeaderTextFrameSet, page2), fsets2.oddHeaders);
    QCOMPARE(bfl.getOrCreate(Words::OddPagesHeaderTextFrameSet, page3), fsets2.oddHeaders);
    QCOMPARE(bfl.getOrCreate(Words::OddPagesHeaderTextFrameSet, page4), fsets2.oddHeaders);
    QCOMPARE(fsets1.oddHeaders->frameCount(), 1);
    QCOMPARE(fsets2.oddHeaders->frameCount(), 3);
}
#include <QDebug>
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

    KWFrameSet *bfs = bfl.m_backgroundFrameSet;
    foreach (KoShape *shape, bfs->shapes()) {
        QCOMPARE(shape->background(), page1.pageStyle().background());
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
        foreach (KoShape *shape, fs->shapes()) {
            fs->removeShape(shape);
            delete shape;
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
