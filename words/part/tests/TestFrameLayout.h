#ifndef TESTFRAMELAYOUT_H
#define TESTFRAMELAYOUT_H

#include <QObject>
#include <QtTest>

#include <KoShape.h>

class KWFrameSet;
class KWTextFrameSet;
class KWTextFrame;
class KWFrame;
class QPointF;

class TestFrameLayout : public QObject
{
    Q_OBJECT
public:
    TestFrameLayout();

private Q_SLOTS:
    // tests
    void testGetOrCreateFrameSet();
    void testCopyShapes();
    void testCreateNewFramesForPage();
    void testShouldHaveHeaderOrFooter();
    void headerPerPage();
    void testFrameCreation();
    void testCreateNewFrameForPage_data();
    void testCreateNewFrameForPage();
    void testCopyFramesForPage();
    void testLargeHeaders();
    void testLayoutPageSpread();
    void testPageStyle();
    void testPageBackground();

    // helper
    void addFS(KWFrameSet*);

private:
    void removeAllFrames();
    KWTextFrame *createFrame(const QPointF &position, KWTextFrameSet &fs);
    KWFrame *createCopyFrame(const QPointF &position, KoShape *orig, KWTextFrameSet &fs);

    QList<KWFrameSet*> m_frames;
};

#endif
