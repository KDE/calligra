#ifndef TESTBASICLAYOUT_H
#define TESTBASICLAYOUT_H

#include <QObject>
#include <QtTest/QtTest>

#include <KoShape.h>

class KWFrameSet;

class TestBasicLayout : public QObject {
    Q_OBJECT
public:
    TestBasicLayout();

private slots:
    // tests
    void testGetOrCreateFrameSet();
    void testCreateNewFramesForPage();

    // helper
    void addFS(KWFrameSet*);

private:
    QList<KWFrameSet*> m_frames;
};

#endif
