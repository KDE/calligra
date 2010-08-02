#ifndef TESTBASICLAYOUT_H
#define TESTBASICLAYOUT_H

#include <QObject>
#include <QtTest/QtTest>

#include <KoShape.h>

class KWFrameSet;

class TestFrameLayout : public QObject
{
    Q_OBJECT
public:
    TestFrameLayout();

private slots:
    // tests
    void testGetOrCreateFrameSet();
    void testCreateNewFramesForPage();
    void testShouldHaveHeaderOrFooter();
    void headerPerPage();
    void testFrameCreation();
    void testCreateNewFrameForPage_data();
    void testCreateNewFrameForPage();
    void testLargeHeaders();

    // helper
    void addFS(KWFrameSet*);

private:
    void removeAllFrames();

    QList<KWFrameSet*> m_frames;
};

#endif
