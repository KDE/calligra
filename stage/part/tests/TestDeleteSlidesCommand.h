#ifndef TESTDELETESLIDESCOMMAND_H
#define TESTDELETESLIDESCOMMAND_H

#include <QObject>

class TestDeleteSlidesCommand: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void delSlide();
};

#endif // TESTDELETESLIDESCOMMAND_H
