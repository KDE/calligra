#ifndef TESTBASICLAYOUT_H
#define TESTBASICLAYOUT_H

#include <QObject>
#include <QtTest/QtTest>

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

private slots:
    // tests
    void testGetOrCreateFrameSet();
    void testCopyShapes();
    void testCreateNewFramesForPage();
    void testShouldHaveHeaderOrFooter();
    void headerPerPage();
    void testFrameCreation();
    void testCreateNewFrameForPage_data();
    void testCreateNewFrameForPage();
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
