#include "TestBasicLayout.h"
#include "TestDocumentLayout.h"

#include "KWPageManager.h"
#include "KWPage.h"
#include "KWPageSettings.h"
#include "KWFrameLayout.h"
#include "KWTextFrameSet.h"
#include "KWTextFrame.h"
#include "KWord.h"

#include <QList>
#include <kinstance.h>

class Helper {
public:
    Helper() {
        pageManager = new KWPageManager();
        pageManager->setStartPage(1);
        KWPage *page = pageManager->appendPage();
        page->setWidth(200);
        page->setHeight(200);
        pageSettings = new KWPageSettings();
        pageSettings->setFirstHeaderPolicy(KWord::HFTypeNone);
        pageSettings->setFirstFooterPolicy(KWord::HFTypeNone);
    }
    ~Helper() {
        delete pageManager;
        delete pageSettings;
    }

    KWPageManager *pageManager;
    KWPageSettings *pageSettings;
};

TestBasicLayout::TestBasicLayout() {
    new KInstance ("TestBasicLayout");
}

void TestBasicLayout::testGetOrCreateFrameSet() {
    Helper helper;
    qDeleteAll(m_frames);
    m_frames.clear();
    KWFrameLayout bfl(helper.pageManager, m_frames, helper.pageSettings);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    KWTextFrameSet *fs = bfl.getOrCreate(KWord::FirstPageHeaderTextFrameSet);
    QVERIFY(fs);
    QCOMPARE(fs->textFrameSetType(), KWord::FirstPageHeaderTextFrameSet);

    KWTextFrameSet *fs2 = bfl.getOrCreate(KWord::FirstPageHeaderTextFrameSet);
    QVERIFY(fs2);
    QCOMPARE(fs, fs2);

    KWTextFrameSet *main = new KWTextFrameSet(KWord::MainTextFrameSet);
    m_frames.append(main);
    bfl.m_setup = false;
    KWTextFrameSet *main2 = bfl.getOrCreate(KWord::MainTextFrameSet);
    QVERIFY(main2);
    QCOMPARE(main, main2);
}

void TestBasicLayout::testCreateNewFramesForPage() {
    Helper helper;
    qDeleteAll(m_frames);
    m_frames.clear();
    QVERIFY(m_frames.count() == 0);
    KWFrameLayout bfl(helper.pageManager, m_frames, helper.pageSettings);
    connect(&bfl, SIGNAL(newFrameSet(KWFrameSet*)), this, SLOT(addFS(KWFrameSet*)));

    KWTextFrameSet *main = bfl.getOrCreate(KWord::MainTextFrameSet);
    QVERIFY(main);
    QCOMPARE(bfl.hasFrameOn(main, 1), false);

    KoShape *shape = new MockTextShape();
    new KWTextFrame(shape, main);
    QCOMPARE(main->frameCount(), 1);

    QCOMPARE(bfl.hasFrameOn(main, 1), true);

    bfl.createNewFramesForPage(1);
    QCOMPARE(main->frameCount(), 1);
}

// helper method (slot)
void TestBasicLayout::addFS(KWFrameSet*fs) {
    m_frames.append(fs);
}

QTEST_MAIN(TestBasicLayout)
#include "TestBasicLayout.moc"
