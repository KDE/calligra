/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Philip Van Hoof <philip@codeminded.be>
             SPDX-FileCopyrightText: 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             SPDX-FileCopyrightText: 2006 Fredrik Edemar <f_edemar@linux.se>
             SPDX-FileCopyrightText: 2005-2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
             SPDX-FileCopyrightText: 2004 Tomas Mecir <mecirt@gmail.com>
             SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2000 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 2000-2006 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1999, 2000 Torben Weis <weis@kde.org>
             SPDX-FileCopyrightText: 1999 Stephan Kulow <coolo@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELAPSED_TIME_P_H
#define ELAPSED_TIME_P_H

#include "SheetsDebug.h"
#include <QElapsedTimer>

namespace Calligra
{
namespace Sheets
{

class ElapsedTime
{
public:
    enum OutputMode { Default, PrintOnlyTime };

#ifdef NDEBUG

    ElapsedTime()
    {
    }
    explicit ElapsedTime(const QString &, OutputMode = Default)
    {
    }

#else // NDEBUG

    ElapsedTime()
    {
        m_time.start();
    }

    explicit ElapsedTime(const QString &name, OutputMode mode = Default)
        : m_name(name)
    {
        m_time.start();
        if (mode != PrintOnlyTime) {
            debugSheets << QString("*** (" + name + ")... Starting measuring...");
        }
    }

    ~ElapsedTime()
    {
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
    QElapsedTimer m_time;
    QString m_name;

#endif // NDEBUG
};

} // namespace Sheets
} // namespace Calligra

#endif
