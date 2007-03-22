
#ifndef KPlato_testDate
#define KPlato_testDate

#include <QtTest/QtTest>
#include <QtCore>

namespace KPlato 
{

class testDate: public QObject
{
    Q_OBJECT
private slots:
    void testValidity();
    void testMonth();
};

} //namespace KPlato

#endif
