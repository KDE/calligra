/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef PPTSTYLE_H
#define PPTSTYLE_H

#include "generated/simpleParser.h"

class PptTextPFRun {
    quint16 level_;
    const MSO::TextPFException* pfs[6];
public:
    PptTextPFRun(const MSO::DocumentContainer* d,
                 const MSO::MasterOrSlideContainer* m,
                 quint32 textType);
    PptTextPFRun(const MSO::DocumentContainer* d = 0,
                 const MSO::MasterOrSlideContainer* m = 0,
                 const MSO::TextContainer* tc = 0,
                 quint32 start = 0);

    quint16 level() const { return level_; }
    //const MSO::TextPFException* pf() const { return pfs[0]; }

    bool fHasBullet() const;
    bool fBulletHasFont() const;
    bool fBulletHasColor() const;
    bool fBulletHasSize() const;
    qint16 bulletChar() const;
    quint16 bulletFontRef() const;
    qint16 bulletSize() const;
    MSO::ColorIndexStruct bulletColor() const;
    quint16 textAlignment() const;
    qint16 lineSpacing() const;
    qint16 spaceBefore() const;
    qint16 spaceAfter() const;
    quint16 leftMargin() const;
    quint16 indent() const;
    quint16 defaultTabSize() const;
    MSO::TabStops tabStops() const;
    quint16 fontAlign() const;
    bool charWrap() const;
    bool wordWrap() const;
    bool overflow() const;
    quint16 textDirection() const;
};


class PptTextCFRun {
    quint16 level_;
    const MSO::TextCFException* cfs[6];
public:
    PptTextCFRun() :level_(0)
    {
        *cfs = 0;
    }
    PptTextCFRun(const MSO::DocumentContainer* d,
                 const MSO::MasterOrSlideContainer* m,
                 const MSO::TextContainer& tc,
                 quint16 level,
                 quint32 start);

    const MSO::TextCFException* cf() const { return cfs[0]; }

    bool bold() const;
    bool italic() const;
    bool underline() const;
    bool shadow() const;
    bool fehint() const;
    bool kumi() const;
    bool emboss() const;
    quint8 pp9rt() const;
    quint16 fontRef() const;
    quint16 oldEAFontRef() const;
    quint16 ansiFontRef() const;
    quint16 symbolFontRef() const;
    quint16 fontSize() const;
    MSO::ColorIndexStruct color() const;
    qint16 position() const;
};

#endif
