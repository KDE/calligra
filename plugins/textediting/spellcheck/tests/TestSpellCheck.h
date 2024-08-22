#ifndef TESTSPELLCHECK_H
#define TESTSPELLCHECK_H

#include <QObject>

class TestSpellCheck : public QObject
{
    Q_OBJECT
public:
    TestSpellCheck() = default;

private Q_SLOTS:
    void testFetchMoreText();
    void testFetchMoreText2();
};

#endif
