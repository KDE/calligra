#ifndef TESTBASICLAYOUT_H
#define TESTBASICLAYOUT_H

#include <QObject>
#include <QtTest/QtTest>

class TestBasicLayout : public QObject
{
    Q_OBJECT
public:
    TestBasicLayout();

private slots:
    // tests
    void testGetOrCreateFrameSet();

private:
};

#endif
