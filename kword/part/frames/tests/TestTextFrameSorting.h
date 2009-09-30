#ifndef TESTTEXTFRAMESORTING_H
#define TESTTEXTFRAMESORTING_H

#include <QObject>
#include <qtest_kde.h>
#include <QtTest/QtTest>

class KWTextFrame;
class KWTextFrameSet;

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

private:
    KWTextFrame* createFrame(const QPointF &position, KWTextFrameSet &fs);
};

#endif
