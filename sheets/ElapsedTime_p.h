/* This file is part of the KDE project
   Copyright (C) 2012 Philip Van Hoof <philip@codeminded.be>
             (C) 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
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


#ifndef ELAPSED_TIME_P_H
#define ELAPSED_TIME_P_H

#include "SheetsDebug.h"
#include <QTime>

namespace Calligra
{
namespace Sheets
{

class ElapsedTime
{
public:
    enum OutputMode { Default, PrintOnlyTime };

#ifdef NDEBUG

    ElapsedTime() {}
    explicit ElapsedTime(const QString &, OutputMode = Default) {}

#else // NDEBUG

    ElapsedTime() {
        m_time.start();
    }

    explicit ElapsedTime(const QString &name, OutputMode mode = Default)
            : m_name(name) {
        m_time.start();
        if (mode != PrintOnlyTime) {
            debugSheets << QString("*** (" + name + ")... Starting measuring...");
        }
    }

    ~ElapsedTime() {
        uint milliSec = m_time.elapsed();
        uint min = static_cast<uint>(milliSec / (1000 * 60));
        milliSec -= (min * 60 * 1000);
        uint sec = static_cast<uint>(milliSec / 1000);
        milliSec -= sec * 1000;

        if (m_name.isNull())
            debugSheets << QString("*** Elapsed time: %1 min %2 sec %3 msec").arg(min).arg(sec).arg(milliSec);
        else
            debugSheets << QString("*** (%1) Elapsed time: %2 min %3 sec %4 msec").arg(m_name).arg(min).arg(sec).arg(milliSec);
    }

private:
    QTime   m_time;
    QString m_name;

#endif // NDEBUG
};

} // namespace Sheets
} // namespace Calligra

#endif
