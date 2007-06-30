#ifndef TESTTEXTFRAMESORTING_H
#define TESTTEXTFRAMESORTING_H

#include <QObject>
#include <QtTest/QtTest>

class KWFrame;
class KWTextFrameSet;

class TestTextFrameSorting : public QObject {
    Q_OBJECT
public:
    TestTextFrameSorting();

private slots:
    // tests
    void testSimpleSorting();
    void testSortingOnPagespread();
    void testRtlSorting();

private:
    KWFrame* createFrame(const QPointF &position, KWTextFrameSet &fs);
};

#endif
