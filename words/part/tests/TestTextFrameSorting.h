#ifndef TESTTEXTFRAMESORTING_H
#define TESTTEXTFRAMESORTING_H

#include <QObject>
#include <qtest_kde.h>
#include <QtTest/QtTest>

class KWTextFrame;
class KWTextFrameSet;
class KWFrame;
class KoShape;

class TestTextFrameSorting : public QObject
{
    Q_OBJECT
public:
    TestTextFrameSorting();

private slots:
    // tests
    void testSimpleSorting();
    void testSimpleSorting2();
    void testSortingOnPagespread();
    void testRtlSorting();
    void testSortingById();
    void testCopyAfterTextSorting();

private:
    KWTextFrame* createFrame(const QPointF &position, KWTextFrameSet &fs);
    KWFrame* createCopyFrame(const QPointF &position, KoShape *orig, KWTextFrameSet &fs);
};

#endif
