#ifndef TESTDELETESLIDESCOMMAND_H
#define TESTDELETESLIDESCOMMAND_H

#include <QtTest/QtTest>

class TestDeleteSlidesCommand: public QObject
{
    Q_OBJECT
private slots:
    void delSlide();
    void delSlideWithCopies();
    void delMultipleSlides();
};

#endif // TESTDELETESLIDESCOMMAND_H
