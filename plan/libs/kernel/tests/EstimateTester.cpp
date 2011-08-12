/* This file is part of the KDE project
   Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "EstimateTester.h"
#include "kptduration.h"
#include "kptnode.h"

namespace KPlato
{

void EstimateTester::expected() {
    Estimate e1;
    e1.clear();
    QVERIFY( e1.expectedEstimate() == 0.0 );
    QVERIFY( e1.expectedValue().milliseconds() == 0.0 );
    
    e1.setExpectedEstimate( 1.0 );
    QVERIFY( e1.expectedEstimate() == 1.0 );
    QVERIFY( e1.expectedValue().milliseconds() == 1000*60*60 );
    
    e1.setUnit( Duration::Unit_ms );
    QVERIFY( e1.expectedEstimate() == 1.0 );
    QVERIFY( e1.expectedValue().milliseconds() == 1 );

    e1.setUnit( Duration::Unit_s );
    QVERIFY( e1.expectedEstimate() == 1.0 );
    QVERIFY( e1.expectedValue().milliseconds() == 1000 );

    e1.setUnit( Duration::Unit_m );
    QVERIFY( e1.expectedEstimate() == 1.0 );
    QVERIFY( e1.expectedValue().milliseconds() == 1000*60 );

    e1.setUnit( Duration::Unit_h );
    QVERIFY( e1.expectedEstimate() == 1.0 );
    QVERIFY( e1.expectedValue().milliseconds() == 1000*60*60 );

    e1.setUnit( Duration::Unit_d );
    QVERIFY( e1.expectedEstimate() == 1.0 );
    QVERIFY( e1.expectedValue().milliseconds() == 1000*60*60*24 );

    e1.setUnit( Duration::Unit_w );
    QVERIFY( e1.expectedEstimate() == 1.0 );
    QVERIFY( e1.expectedValue().milliseconds() == 1000*60*60*24*7 );

    e1.setUnit( Duration::Unit_M );
    QVERIFY( e1.expectedEstimate() == 1.0 );
    QCOMPARE( e1.expectedValue().milliseconds(), qint64(1000*60*60) * (24*30) );

    e1.setUnit( Duration::Unit_Y );
    QVERIFY( e1.expectedEstimate() == 1.0 );
    QCOMPARE( e1.expectedValue().milliseconds(), qint64(1000*60*60) * (24*365) );

}

void EstimateTester::optimistic() {
    Estimate e1;
    e1.clear();
    QVERIFY( e1.optimisticEstimate() == 0.0 );
    QVERIFY( e1.optimisticValue().milliseconds() == 0.0 );
    
    e1.setOptimisticEstimate( 1.0 );
    QVERIFY( e1.optimisticEstimate() == 1.0 );
    QVERIFY( e1.optimisticValue().milliseconds() == 1000*60*60 );
    
    e1.setUnit( Duration::Unit_ms );
    QVERIFY( e1.optimisticEstimate() == 1.0 );
    QVERIFY( e1.optimisticValue().milliseconds() == 1 );

    e1.setUnit( Duration::Unit_s );
    QVERIFY( e1.optimisticEstimate() == 1.0 );
    QVERIFY( e1.optimisticValue().milliseconds() == 1000 );

    e1.setUnit( Duration::Unit_m );
    QVERIFY( e1.optimisticEstimate() == 1.0 );
    QVERIFY( e1.optimisticValue().milliseconds() == 1000*60 );

    e1.setUnit( Duration::Unit_h );
    QVERIFY( e1.optimisticEstimate() == 1.0 );
    QVERIFY( e1.optimisticValue().milliseconds() == 1000*60*60 );

    e1.setUnit( Duration::Unit_d );
    QVERIFY( e1.optimisticEstimate() == 1.0 );
    QVERIFY( e1.optimisticValue().milliseconds() == 1000*60*60*24 );

}

void EstimateTester::pessimistic() {
    Estimate e1;
    e1.clear();
    QVERIFY( e1.pessimisticEstimate() == 0.0 );
    QVERIFY( e1.pessimisticValue().milliseconds() == 0.0 );
    
    e1.setPessimisticEstimate( 1.0 );
    QVERIFY( e1.pessimisticEstimate() == 1.0 );
    QVERIFY( e1.pessimisticValue().milliseconds() == 1000*60*60 );
    
    e1.setUnit( Duration::Unit_ms );
    QVERIFY( e1.pessimisticEstimate() == 1.0 );
    QVERIFY( e1.pessimisticValue().milliseconds() == 1 );

    e1.setUnit( Duration::Unit_s );
    QVERIFY( e1.pessimisticEstimate() == 1.0 );
    QVERIFY( e1.pessimisticValue().milliseconds() == 1000 );

    e1.setUnit( Duration::Unit_m );
    QVERIFY( e1.pessimisticEstimate() == 1.0 );
    QVERIFY( e1.pessimisticValue().milliseconds() == 1000*60 );

    e1.setUnit( Duration::Unit_h );
    QVERIFY( e1.pessimisticEstimate() == 1.0 );
    QVERIFY( e1.pessimisticValue().milliseconds() == 1000*60*60 );

    e1.setUnit( Duration::Unit_d );
    QVERIFY( e1.pessimisticEstimate() == 1.0 );
    QVERIFY( e1.pessimisticValue().milliseconds() == 1000*60*60*24 );

}

void EstimateTester::ratio() {
    Estimate e1;
    e1.clear();
    
    e1.setExpectedEstimate( 1.0 );
    e1.setOptimisticEstimate( 1.0 );
    e1.setPessimisticEstimate( 1.0 );

    QVERIFY( e1.pessimisticRatio() == 0 );
    QVERIFY( e1.optimisticRatio() == 0 );
    
    e1.setExpectedEstimate( 2.0 );
    e1.setOptimisticEstimate( 1.0 );
    e1.setPessimisticEstimate( 4.0 );

    QVERIFY( e1.pessimisticRatio() == 100 );
    QVERIFY( e1.optimisticRatio() == -50 );

    e1.setUnit( Duration::Unit_h );
    e1.setOptimisticEstimate( 0.5 );
    QVERIFY( e1.pessimisticRatio() == 100 );
    QVERIFY( e1.optimisticRatio() == -75 );

    e1.clear();
    e1.setUnit( Duration::Unit_d );
    e1.setExpectedEstimate( 1.0 );
    e1.setOptimisticRatio( -50 );
    e1.setPessimisticRatio( 100 );
    QVERIFY( e1.pessimisticEstimate() == 2.0 );
    QVERIFY( e1.optimisticEstimate() == 0.5 );
    QVERIFY( e1.pessimisticValue() == 1000*60*60 * 48 );
    QVERIFY( e1.optimisticValue() == 1000*60*60 * 12 );

}

void EstimateTester::defaultScale() {
    QList<qint64> s = Estimate::defaultScales();

    QCOMPARE( s.count(), 8 );

    Duration d = Estimate::scale( 1.0, Duration::Unit_Y, s );
    QCOMPARE( d.milliseconds(), s[0] );
    QCOMPARE( 1.0, Estimate::scale( d, Duration::Unit_Y, s ) );

    d = Estimate::scale( 1.0, Duration::Unit_M, s );
    QCOMPARE( d.milliseconds(), s[1] );
    QCOMPARE( 1.0, Estimate::scale( d, Duration::Unit_M, s ) );

    d = Estimate::scale( 1.0, Duration::Unit_w, s );
    QCOMPARE( d.milliseconds(), s[2] );
    QCOMPARE( 1.0, Estimate::scale( d, Duration::Unit_w, s ) );

    d = Estimate::scale( 1.0, Duration::Unit_d, s );
    QCOMPARE( d.milliseconds(), s[3] );
    QCOMPARE( 1.0, Estimate::scale( d, Duration::Unit_d, s ) );

    d = Estimate::scale( 1.0, Duration::Unit_h, s );
    QCOMPARE( d.milliseconds(), s[4] );
    QCOMPARE( 1.0, Estimate::scale( d, Duration::Unit_h, s ) );

    d = Estimate::scale( 1.0, Duration::Unit_m, s );
    QCOMPARE( d.milliseconds(), s[5] );
    QCOMPARE( 1.0, Estimate::scale( d, Duration::Unit_m, s ) );

    d = Estimate::scale( 1.0, Duration::Unit_s, s );
    QCOMPARE( d.milliseconds(), s[6] );
    QCOMPARE( 1.0, Estimate::scale( d, Duration::Unit_s, s ) );

    d = Estimate::scale( 1.0, Duration::Unit_ms, s );
    QCOMPARE( d.milliseconds(), s[7] );
    QCOMPARE( 1.0, Estimate::scale( d, Duration::Unit_ms, s ) );
}

void EstimateTester::scale() {
    StandardWorktime wt;
    QList<qint64> s = wt.scales();

    QCOMPARE( s.count(), 8 );

    Duration d = Estimate::scale( 1.0, Duration::Unit_Y, s );
    QCOMPARE( d.milliseconds(), s[0] );
    QCOMPARE( 1.0, Estimate::scale( d, Duration::Unit_Y, s ) );

    d = Estimate::scale( 1.0, Duration::Unit_M, s );
    QCOMPARE( d.milliseconds(), s[1] );
    QCOMPARE( 1.0, Estimate::scale( d, Duration::Unit_M, s ) );

    d = Estimate::scale( 1.0, Duration::Unit_w, s );
    QCOMPARE( d.milliseconds(), s[2] );
    QCOMPARE( 1.0, Estimate::scale( d, Duration::Unit_w, s ) );

    d = Estimate::scale( 1.0, Duration::Unit_d, s );
    QCOMPARE( d.milliseconds(), s[3] );
    QCOMPARE( 1.0, Estimate::scale( d, Duration::Unit_d, s ) );

    d = Estimate::scale( 1.0, Duration::Unit_h, s );
    QCOMPARE( d.milliseconds(), s[4] );
    QCOMPARE( 1.0, Estimate::scale( d, Duration::Unit_h, s ) );

    d = Estimate::scale( 1.0, Duration::Unit_m, s );
    QCOMPARE( d.milliseconds(), s[5] );
    QCOMPARE( 1.0, Estimate::scale( d, Duration::Unit_m, s ) );

    d = Estimate::scale( 1.0, Duration::Unit_s, s );
    QCOMPARE( d.milliseconds(), s[6] );
    QCOMPARE( 1.0, Estimate::scale( d, Duration::Unit_s, s ) );

    d = Estimate::scale( 1.0, Duration::Unit_ms, s );
    QCOMPARE( d.milliseconds(), s[7] );
    QCOMPARE( 1.0, Estimate::scale( d, Duration::Unit_ms, s ) );
}

void EstimateTester::pert() {
    Estimate e1;
    e1.clear();
    e1.setUnit( Duration::Unit_d );
    e1.setExpectedEstimate( 4.0 );
    e1.setOptimisticEstimate( 2.0 );
    e1.setPessimisticEstimate( 8.0 );
    
    QVERIFY( e1.deviation() == 1.0 );
    QVERIFY( e1.deviation( Duration::Unit_h ) == 24.0 );

    QVERIFY( e1.variance() == 1.0 );
    QVERIFY( e1.variance( Duration::Unit_h ) == 24.0*24.0 );
    
    qint64 day = 1000*60*60*24;

    e1.setRisktype( Estimate::Risk_None );
    QVERIFY( e1.pertExpected().milliseconds() == 4 * day );
    e1.setRisktype( Estimate::Risk_Low );
    QVERIFY( e1.pertExpected().milliseconds() == ((2 + 8 + (4*4))*day)/6 );
    e1.setRisktype( Estimate::Risk_High );
    QVERIFY( e1.pertExpected().milliseconds() == ((2 + 16 + (4*4))*day)/7 );
}

} //namespace KPlato

QTEST_MAIN( KPlato::EstimateTester )

#include "EstimateTester.moc"
