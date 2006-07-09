/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef CONVERSION_H
#define CONVERSION_H

#include <qstring.h>
#include <qpair.h>

// Static methods for simple OO<->KWord conversions
// (enums etc.)
// import* means OO->KWord
// export* means KWord->OO
// Only put here the conversions that are specific to KWord.
// Those that can be used in KPresenter<->ooimpress should go to OoUtils.

namespace Conversion
{
    // Convert paragraph alignment value
    QString importAlignment( const QString& );
    QString exportAlignment( const QString& );

    // Convert frame wrapping (run-around)
    // KWord-1.3 knows runaround(int) and runaroundSide(QString)
    QPair<int,QString> importWrapping( const QString& );
    QString exportWrapping( const QPair<int,QString>& runAroundAttribs );

    // Convert overflow behavior ("what to do if the text is bigger than the frame")
    int importOverflowBehavior( const QString& oasisOverflowBehavior );
    QString exportOverflowBehavior( const QString& kwordAutoCreateNewFrame );

    // Convert numbering format (for a list) to a kword counter type
    int importCounterType( const QString& numFormat );

    // Header-footer helpers
    QString headerTypeToFramesetName( const QString& tagName, bool hasEvenOdd );
    int headerTypeToFrameInfo( const QString& tagName, bool hasEvenOdd );
}

#endif
