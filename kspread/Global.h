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

/* Definition of maximal supported rows - please check kspread_util (columnName) and kspread_cluster also */
#define KS_rowMax 0x7FFF
/* Definition of maximal supported columns - please check kspread_util (columnName) and kspread_cluster also */
#define KS_colMax 0x7FFF

/* Definition of maximal supported columns/rows, which can be merged */
#define KS_spanMax 0xFFF


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
namespace Paste {
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

enum ModifyType {
    MT_NONE = 0,
    MT_MOVE,
    MT_RESIZE_UP,
    MT_RESIZE_DN,
    MT_RESIZE_LF,
    MT_RESIZE_RT,
    MT_RESIZE_LU,
    MT_RESIZE_LD,
    MT_RESIZE_RU,
    MT_RESIZE_RD
};

enum PropValue {
    STATE_ON = 0,
    STATE_OFF = 1,
    STATE_UNDEF = 2
};

//TODO Implement all objects :)
enum ToolEditMode {
    TEM_MOUSE = 0,
    //INS_RECT = 1,
    //INS_ELLIPSE = 2,
    //INS_TEXT = 3,
    //INS_PIE = 4,
    INS_OBJECT = 5,
    //INS_LINE = 6,
    INS_DIAGRAMM = 7,
    //INS_TABLE = 8,
    //INS_FORMULA = 9,
    //INS_AUTOFORM = 10,
    //INS_FREEHAND = 11,
    //INS_POLYLINE = 12,
    //INS_QUADRICBEZIERCURVE = 13,
    //INS_CUBICBEZIERCURVE = 14,
    //INS_POLYGON = 15,
    INS_PICTURE = 16
    //INS_CLIPART = 17,
    //TEM_ROTATE = 18,
    //TEM_ZOOM = 19,
    //INS_CLOSED_FREEHAND = 20,
    //INS_CLOSED_POLYLINE = 21,
    //INS_CLOSED_QUADRICBEZIERCURVE = 22,
    //INS_CLOSED_CUBICBEZIERCURVE = 23
};


} // namespace KSpread

#endif
