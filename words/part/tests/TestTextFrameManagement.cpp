#include "TestTextFrameManagement.h"
#include "../KWPageManager.h"
#include "../KWPage.h"
#include "../frames/KWTextFrameSet.h"
#include "../frames/KWTextFrame.h"

#include <KoTextShapeData.h>
#include <MockShapes.h>

TestTextFrameManagement::TestTextFrameManagement()
{
}

void TestTextFrameManagement::testFrameRemoval()
{
    KWTextFrameSet tfs(0);

    createFrame(QPointF(10, 10), tfs);
    createFrame(QPointF(10, 120), tfs);
    createFrame(QPointF(10, 1000), tfs);

    QCOMPARE(tfs.frameCount(), 3);
    tfs.framesEmpty(0);
    QCOMPARE(tfs.frameCount(), 3);
    tfs.framesEmpty(1);
    QCOMPARE(tfs.frameCount(), 3); // don't autodelete when we don't have a pagemanager

    KWPageManager pm;
    pm.appendPage();
    pm.appendPage();
    tfs.setPageManager(&pm);

    QCOMPARE(tfs.frameCount(), 3);
    tfs.framesEmpty(0);
    QCOMPARE(tfs.frameCount(), 3);
    tfs.framesEmpty(1);
    QCOMPARE(tfs.frameCount(), 2);
    tfs.framesEmpty(1);
    QCOMPARE(tfs.frameCount(), 2); // both on one page
    tfs.framesEmpty(2);
    QCOMPARE(tfs.frameCount(), 2); // leave one
}

KWTextFrame * TestTextFrameManagement::createFrame(const QPointF &position, KWTextFrameSet &fs)
{
    MockShape *shape = new MockShape();
    shape->setUserData(new KoTextShapeData());
    KWTextFrame *frame = new KWTextFrame(shape, &fs);
    shape->setPosition(position);
    return frame;
}


QTEST_KDEMAIN(TestTextFrameManagement, GUI)
