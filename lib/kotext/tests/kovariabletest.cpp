#include <qstring.h>
#include <assert.h>

#include "../duration.h"

void testMinutes()
{
    int minutes = 145;
    QString str = minutesToISODuration( minutes );
    int result = ISODurationToMinutes( str );
    qDebug( "%d minutes -> %s -> %d", minutes, str.latin1(), result );
    assert( result == minutes );
}

void testNegativeMinutes()
{
    int minutes = -15;
    QString str = minutesToISODuration( minutes );
    int result = ISODurationToMinutes( str );
    qDebug( "%d minutes -> %s -> %d", minutes, str.latin1(), result );
    assert( result == minutes );
}

void testDays()
{
    int days = 14;
    QString str = daysToISODuration( days );
    int result = ISODurationToDays( str );
    qDebug( "%d days -> %s -> %d", days, str.latin1(), result );
    assert( result == days );
}

void testNegativeDays()
{
    int days = -14;
    QString str = daysToISODuration( days );
    int result = ISODurationToDays( str );
    qDebug( "%d days -> %s -> %d", days, str.latin1(), result );
    assert( result == days );
}

int main ( int argc, char ** argv )
{
    testMinutes();
    testDays();
    testNegativeMinutes();
    testNegativeDays();
    return 0;
}
