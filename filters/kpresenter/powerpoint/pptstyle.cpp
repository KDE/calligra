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

#include "pptstyle.h"

using namespace PPT;

namespace {

const TextMasterStyleAtom*
getTextMasterStyleAtom(const MasterOrSlideContainer* m, quint16 texttype)
{
    if (!m) return 0;
    const MainMasterContainer* mm = m->anon.get<MainMasterContainer>();
    if (!mm) return 0;
    const TextMasterStyleAtom* textstyle = 0;
    foreach (const TextMasterStyleAtom&ma, mm->rgTextMasterStyle) {
        if (ma.rh.recInstance == texttype) {
            textstyle = &ma;
        }
    }
    return textstyle;
}
const TextMasterStyleLevel *
getTextMasterStyleLevel(const MasterOrSlideContainer* m, quint16 type, quint16 level)
{
    const TextMasterStyleAtom* masterStyle = getTextMasterStyleAtom(m, type);
    if (!masterStyle) return 0;
    const TextMasterStyleLevel *l = 0;
    switch (level) {
    case 0: if (masterStyle->lstLvl1) l = masterStyle->lstLvl1.data();break;
    case 1: if (masterStyle->lstLvl2) l = masterStyle->lstLvl2.data();break;
    case 2: if (masterStyle->lstLvl3) l = masterStyle->lstLvl3.data();break;
    case 3: if (masterStyle->lstLvl4) l = masterStyle->lstLvl4.data();break;
    case 4: if (masterStyle->lstLvl5) l = masterStyle->lstLvl5.data();break;
    }
    return l;
}
const TextPFRun* getPFRun(const TextContainer& tc, quint32 start)
{
    // find the textpfexception that belongs to this line
    const TextPFRun* pf = 0;
    if (tc.style) {
        const QList<TextPFRun> &pfs = tc.style->rgTextPFRun;
        int i = 0;
        quint32 pfend = 0;
        while (i < pfs.size()) {
            pfend += pfs[i].count;
            if (pfend > start) {
                pf = &pfs[i];
                break;
            }
            i++;
        }
    }
    return pf;
}
const TextPFException* getLinePF(const TextContainer& tc, quint32 start)
{
    // find the textpfexception that belongs to this line
    const TextPFRun* pf = 0;
    if (tc.style) {
        const QList<TextPFRun> &pfs = tc.style->rgTextPFRun;
        int i = 0;
        quint32 pfend = 0;
        while (i < pfs.size()) {
            pfend += pfs[i].count;
            if (pfend > start) {
                pf = &pfs[i];
                break;
            }
            i++;
        }
    }
    return (pf) ?&pf->pf :0;
}

const TextPFException* getLevelPF(const MasterOrSlideContainer* m,
                                  const TextContainer& tc, quint16 level)
{
    quint32 textType = tc.textHeaderAtom.textType;
    const TextMasterStyleLevel* ms = getTextMasterStyleLevel(m, textType, level);
    return (ms) ?&ms->pf :0;
}

const TextPFException* getBaseLevelPF(const MasterOrSlideContainer* m,
                                  const TextContainer& tc, quint16 level)
{
    quint32 textType = tc.textHeaderAtom.textType;
    const TextMasterStyleLevel* ms = 0;
    if (textType == 6) {
        // inherit from Tx_TYPE_TITLE
        ms = getTextMasterStyleLevel(m, 0, level);
    } else if (textType == 4 || textType == 5 || textType == 7
               || textType == 8) {
        // inherit from Tx_TYPE_BODY
        ms = getTextMasterStyleLevel(m, 1, level);
    }
    return (ms) ?&ms->pf :0;
}

}

PptTextPFRun::PptTextPFRun(const MasterOrSlideContainer* m,
                               const TextContainer& tc,
                               quint32 start)
{
    const TextPFRun* pfrun = getPFRun(tc, start);
    quint16 level = 0;
    if (pfrun) {
        level = pfrun->indentLevel;
        // [MS-PPT].pdf says there are only 5 levels
        if (level > 4) level = 4;
    }

    const PPT::TextPFException** p = pfs;
    const PPT::TextPFException* pfe = getLinePF(tc, start);
    if (pfe) {
        *p = pfe;
        ++p;
    }
    pfe = getLevelPF(m, tc, level);
    if (pfe) {
        *p = pfe;
        ++p;
    }
    pfe = getBaseLevelPF(m, tc, level);
    if (pfe) {
        *p = pfe;
        ++p;
    }
    *p = 0;

    // the level reported by PptPFRun is 0 when not bullets, i.e. no list is
    // active, 1 is lowest list level, 5 is the highest list level
    level_ = (fHasBullet()) ?level + 1 :0;
}

#define GETTER(TYPE, PARENT, PRE, NAME, TEST, DEFAULT) \
TYPE PptTextPFRun::NAME() const \
{ \
    const PPT::TextPFException* const * p = pfs; \
    while (*p) { \
        if ((*p)->masks.TEST) { \
            return PRE (*p)->PARENT NAME; \
        } \
        ++p; \
    } \
    return DEFAULT; \
}

//     TYPE      PARENT       PRE NAME             TEST            DEFAULT
GETTER(bool,     bulletFlags->,,  fHasBullet,      hasBullet,      false)
GETTER(bool,     bulletFlags->,,  fBulletHasFont,  bulletHasFont,  false)
GETTER(bool,     bulletFlags->,,  fBulletHasColor, bulletHasColor, false)
GETTER(bool,     bulletFlags->,,  fBulletHasSize,  bulletHasSize,  false)
GETTER(qint16,   ,             ,  bulletChar,      bulletChar,     0)
GETTER(quint16,  ,             ,  bulletFontRef,   bulletFont,     0)
GETTER(qint16,   ,             ,  bulletSize,      bulletSize,     0)
GETTER(ColorIndexStruct,,      *, bulletColor,   bulletColor,ColorIndexStruct())
GETTER(quint16,  ,             ,  textAlignment,   align,          0)
GETTER(qint16,   ,             ,  lineSpacing,     lineSpacing,    0)
GETTER(qint16,   ,             ,  spaceBefore,     spaceBefore,    0)
GETTER(qint16,   ,             ,  spaceAfter,      spaceAfter,     0)
GETTER(quint16,  ,             ,  leftMargin,      leftMargin,     0)
GETTER(quint16,  ,             ,  indent,          indent,         0)
GETTER(quint16,  ,             ,  defaultTabSize,  defaultTabSize, 0)
GETTER(TabStops, ,             *, tabStops,        tabStops,       0)
GETTER(quint16,  ,             ,  fontAlign,       fontAlign,      0)
GETTER(bool,     wrapFlags->,  ,  charWrap,        wordWrap,       false)
GETTER(bool,     wrapFlags->,  ,  wordWrap,        wordWrap,       false)
GETTER(bool,     wrapFlags->,  ,  overflow,        overflow,       false)
GETTER(quint16,  ,             ,  textDirection,   textDirection,  0)

