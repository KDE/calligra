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

#include "mso/simpleParser.h"

class PptTextPFRun {
    quint16 level_;
    const PPT::TextPFException* pfs[5];
public:
    PptTextPFRun() :level_(0)
    {
        *pfs = 0;
    }
    PptTextPFRun(const PPT::DocumentContainer* d,
                 const PPT::MasterOrSlideContainer* m,
                 const PPT::TextContainer& tc,
                 quint32 start);

    quint16 level() const { return level_; }
    const PPT::TextPFException* pf() const { return pfs[0]; }

    bool fHasBullet() const;
    bool fBulletHasFont() const;
    bool fBulletHasColor() const;
    bool fBulletHasSize() const;
    qint16 bulletChar() const;
    quint16 bulletFontRef() const;
    qint16 bulletSize() const;
    PPT::ColorIndexStruct bulletColor() const;
    quint16 textAlignment() const;
    qint16 lineSpacing() const;
    qint16 spaceBefore() const;
    qint16 spaceAfter() const;
    quint16 leftMargin() const;
    quint16 indent() const;
    quint16 defaultTabSize() const;
    PPT::TabStops tabStops() const;
    quint16 fontAlign() const;
    bool charWrap() const;
    bool wordWrap() const;
    bool overflow() const;
    quint16 textDirection() const;
};


class PptTextCFRun {
    quint16 level_;
    const PPT::TextCFException* cfs[5];
public:
    PptTextCFRun() :level_(0)
    {
        *cfs = 0;
    }
    PptTextCFRun(const PPT::DocumentContainer* d,
                 const PPT::MasterOrSlideContainer* m,
                 const PPT::TextContainer& tc,
                 quint16 level,
                 quint32 start);

    const PPT::TextCFException* cf() const { return cfs[0]; }

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
    PPT::ColorIndexStruct color() const;
    qint16 position() const;
};

#endif
