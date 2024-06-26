#ifndef TESTTEXTFRAMEMANAGEMENT_H
#define TESTTEXTFRAMEMANAGEMENT_H

#include <QObject>
#include <QtTest>
#include <qtest_kde.h>

class KWTextFrame;
class KWTextFrameSet;

class TestTextFrameManagement : public QObject
{
    Q_OBJECT
public:
    TestTextFrameManagement();

private Q_SLOTS:
    // tests
    void testFrameRemoval();

private:
    KWTextFrame *createFrame(const QPointF &position, KWTextFrameSet &fs);
};

#endif
