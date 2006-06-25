#include "TestBasicLayout.h"

#include "KWPageManager.h"
#include "KWPageSettings.h"
#include "KWFrameLayout.h"
#include "KWTextFrameSet.h"
#include "KWord.h"

#include <QList>
#include <kinstance.h>

class Helper {
public:
    Helper() {
        pageManager = new KWPageManager();
        pageSettings = new KWPageSettings();
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
    QList<KWFrameSet *> frames;
    KWFrameLayout bfl(helper.pageManager, frames, helper.pageSettings);

    KWTextFrameSet *fs = bfl.getOrCreate(KWord::FirstPageHeaderTextFrameSet);
    QVERIFY(fs);
    QCOMPARE(fs->textFrameSetType(), KWord::FirstPageHeaderTextFrameSet);

    KWTextFrameSet *fs2 = bfl.getOrCreate(KWord::FirstPageHeaderTextFrameSet);
    QVERIFY(fs2);
    QCOMPARE(fs, fs2);
}

QTEST_MAIN(TestBasicLayout)
#include "TestBasicLayout.moc"
