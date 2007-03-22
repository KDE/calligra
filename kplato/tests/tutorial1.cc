
#include "tutorial1.h"

namespace KPlato
{

void testDate::testValidity()
{
    // 11 March 1967
    QDate date( 1967, 3, 11 );
    QVERIFY( date.isValid() );
}
 
void testDate::testMonth()
{
    // 11 March 1967
    QDate date;
    date.setYMD( 1967, 3, 11 );
    QCOMPARE( date.month(), 3 );
    QCOMPARE( QDate::longMonthName(date.month()),
              QString("March") );
}
 
 
} //namespace KPlato

QTEST_MAIN(KPlato::testDate)

#include "tutorial1.moc"
