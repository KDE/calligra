/* This file is part of the KDE project
   Copyright (C) 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             (C) 2006 Fredrik Edemar <f_edemar@linux.se>
             (C) 2005-2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
             (C) 2004 Tomas Mecir <mecirt@gmail.com>
             (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2000 David Faure <faure@kde.org>
             (C) 2000 Werner Trobin <trobin@kde.org>
             (C) 2000-2006 Laurent Montel <montel@kde.org>
             (C) 1999, 2000 Torben Weis <weis@kde.org>
             (C) 1999 Stephan Kulow <coolo@kde.org>

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

#ifndef KSPREAD_GLOBAL_H
#define KSPREAD_GLOBAL_H

#include <kdebug.h>
#include <kiconloader.h>
#include <QTime>

namespace KSpread
{

class ElapsedTime
{
public:
    enum OutputMode { Default, PrintOnlyTime };

#ifdef NDEBUG

    ElapsedTime() {}
    explicit ElapsedTime(QString const& , OutputMode = Default) {}

#else // NDEBUG

    ElapsedTime() {
        m_time.start();
    }

    explicit ElapsedTime(QString const & name, OutputMode mode = Default)
            : m_name(name) {
        m_time.start();
        if (mode != PrintOnlyTime)
            kDebug(36001) << QString("*** (" + name + ")... Starting measuring...").toLatin1().data();
    }

    ~ElapsedTime() {
        uint milliSec = m_time.elapsed();
        uint min = (uint)(milliSec / (1000 * 60));
        milliSec -= (min * 60 * 1000);
        uint sec = (uint)(milliSec / 1000);
        milliSec -= sec * 1000;

        if (m_name.isNull())
            kDebug(36001) << QString("*** Elapsed time: %1 min %2 sec %3 msec").arg(min).arg(sec).arg(milliSec).toLatin1().data();
        else
            kDebug(36001) << QString("*** (%1) Elapsed time: %2 min %3 sec %4 msec").arg(m_name).arg(min).arg(sec).arg(milliSec).toLatin1().data();
    }

private:
    QTime   m_time;
    QString m_name;

#endif // NDEBUG
};

/**
 * This namespace collects enumerations related to
 * pasting operations.
 */
namespace Paste
{
/**
 * The pasted content
 */
enum Mode {
    Normal /** Everything */,
    Text /** Text only */,
    Format /** Format only */,
    NoBorder /** not the borders */,
    Comment /** Comment only */,
    Result /** Result only, no formula */,
    NormalAndTranspose /** */,
    TextAndTranspose /** */,
    FormatAndTranspose /** */,
    NoBorderAndTranspose /** */
};
/**
 * The current cell value treatment.
 */
enum Operation {
    OverWrite /** Overwrite */,
    Add /** Add */,
    Mul /** Multiply */,
    Sub /** Subtract */,
    Div /** Divide */
};
} // namespace Paste

// necessary due to QDock* enums (Werner)
enum MoveTo { Bottom, Left, Top, Right, BottomFirst, NoMovement };
enum MethodOfCalc { SumOfNumber, Min, Max, Average, Count, NoneCalc, CountA };

} // namespace KSpread

#endif
