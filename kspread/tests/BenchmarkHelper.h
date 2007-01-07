/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2007 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_BENCHMARK_HELPER
#define KSPREAD_BENCHMARK_HELPER

#include <QProcess>
#include <QRegExp>

namespace KSpread
{
namespace Time
{

typedef unsigned long tval;

inline tval stamp(void) 
{
        tval tsc;
        asm volatile("rdtsc" : "=a" (tsc) : : "edx");
        return tsc;
}

inline tval elapsed(tval t)
{
        tval tsc;
        asm volatile("rdtsc" : "=a" (tsc) : : "edx");
        if (tsc>t)
                return tsc-t;
        else
                return t-tsc;
}

static QString printAverage( tval ticks, int counter )
{
    QString str;
    QProcess procCpuInfo;
    procCpuInfo.start( "cat /proc/cpuinfo");
    if ( procCpuInfo.waitForFinished() )
    {
        QRegExp reg( "cpu MHz\\s+:\\s+(\\d{4}.\\d{3})" );
        reg.indexIn( procCpuInfo.readAllStandardOutput() );
        bool ok = true;
        double freq = reg.cap(1).toDouble( &ok );
        if ( ok )
        {
            double time = 1000.0 * ticks / counter / freq; // ns
            if ( time < 1000.0 )
                str = QString("%1 ns/operation").arg( QString::number( time, 'f', 2 ) );
            else
            {
                time /= 1000.0; // us
                if ( time < 1000.0 )
                    str = QString("%1 us/operation").arg( QString::number( time, 'f', 2 ) );
                else
                {
                    time /= 1000.0; // ms
                    str = QString("%1 ms/operation").arg( QString::number( time, 'f', 2 ) );
                }
            }
        }
    }
    return QString( "Average: %1/%2=%3 cycles/operation; %4" ).arg( ticks ).
      arg( counter ).arg( ticks/counter ).arg( str );
}

} // namespace Time
} // namespace KSpread

#endif // KSPREAD_BENCHMARK_HELPER
