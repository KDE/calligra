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

using namespace MSO;

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
getTextMasterStyleLevel(const TextMasterStyleAtom* ms, quint16 level)
{
    if (!ms) return 0;
    const TextMasterStyleLevel *l = 0;
    if (ms->rh.recInstance < 5) {
        switch (level) {
        case 0: if (ms->lstLvl1) l = ms->lstLvl1.data();break;
        case 1: if (ms->lstLvl2) l = ms->lstLvl2.data();break;
        case 2: if (ms->lstLvl3) l = ms->lstLvl3.data();break;
        case 3: if (ms->lstLvl4) l = ms->lstLvl4.data();break;
        case 4: if (ms->lstLvl5) l = ms->lstLvl5.data();break;
        }
    } else {
        if (ms->cLevels > 0 && level == ms->lstLvl1level) {
            l =  ms->lstLvl1.data();
        } else if (ms->cLevels > 1 && level == ms->lstLvl2level) {
            l =  ms->lstLvl2.data();
        } else if (ms->cLevels > 2 && level == ms->lstLvl3level) {
            l =  ms->lstLvl3.data();
        } else if (ms->cLevels > 3 && level == ms->lstLvl4level) {
            l =  ms->lstLvl4.data();
        } else if (ms->cLevels > 4 && level == ms->lstLvl5level) {
            l =  ms->lstLvl5.data();
        }
    }
    return l;
}
template <class Run>
const Run* getRun(const QList<Run> &runs, quint32 start)
{
    int i = 0;
    quint32 end = 0;
    const Run* run = 0;
    while (i < runs.size()) {
        end += runs[i].count;
        if (end > start) {
            run = &runs[i];
            break;
        }
        i++;
    }
    return run;
}
const TextPFRun* getPFRun(const TextContainer* tc, quint32 start)
{
    if (tc && tc->style) {
        return getRun<TextPFRun>(tc->style->rgTextPFRun, start);
    }
    return 0;
}
const TextCFRun* getCFRun(const TextContainer* tc, quint32 start)
{
    if (tc && tc->style) {
        return getRun<TextCFRun>(tc->style->rgTextCFRun, start);
    }
    return 0;
}
const TextPFException* getLevelPF(const MasterOrSlideContainer* m,
                                  quint32 textType, quint16 level)
{
    const TextMasterStyleAtom* ms = getTextMasterStyleAtom(m, textType);
    const TextMasterStyleLevel* ml = getTextMasterStyleLevel(ms, level);
    return (ml) ?&ml->pf :0;
}
const TextPFException* getLevelPF(const MasterOrSlideContainer* m,
                                  const TextContainer* tc, quint16 level)
{
    return (tc) ?getLevelPF(m, tc->textHeaderAtom.textType, level) :0;
}
const TextCFException* getLevelCF(const MasterOrSlideContainer* m,
                                  const TextContainer& tc, quint16 level)
{
    quint32 textType = tc.textHeaderAtom.textType;
    const TextMasterStyleAtom* ms = getTextMasterStyleAtom(m, textType);
    const TextMasterStyleLevel* ml = getTextMasterStyleLevel(ms, level);
    return (ml) ?&ml->cf :0;
}
const TextMasterStyleLevel* getBaseLevel(const MasterOrSlideContainer* m,
                                  quint32 textType, quint16 level)
{
    const TextMasterStyleAtom* ms = 0;
    if (textType == 6) {
        // inherit from Tx_TYPE_TITLE
        ms = getTextMasterStyleAtom(m, 0);
    } else if (textType == 4 || textType == 5 || textType == 7
               || textType == 8) {
        // inherit from Tx_TYPE_BODY
        ms = getTextMasterStyleAtom(m, 1);
    }
    return getTextMasterStyleLevel(ms, level);
}
const TextMasterStyleLevel* getBaseLevel(const MasterOrSlideContainer* m,
                                  const TextContainer* tc, quint16 level)
{
    return (tc) ?getBaseLevel(m, tc->textHeaderAtom.textType, level) :0;
}
const TextPFException* getBaseLevelPF(const MasterOrSlideContainer* m,
                                  const TextContainer* tc, quint16 level)
{
    const TextMasterStyleLevel* ml = getBaseLevel(m, tc, level);
    return (ml) ?&ml->pf :0;
}
const TextCFException* getBaseLevelCF(const MasterOrSlideContainer* m,
                                  const TextContainer* tc, quint16 level)
{
    const TextMasterStyleLevel* ml = getBaseLevel(m, tc, level);
    return (ml) ?&ml->cf :0;
}
const TextMasterStyleLevel* getDefaultLevel(const MSO::DocumentContainer* d,
                                            quint16 level)
{
    if (!d) return 0;
    const TextMasterStyleLevel* ml = getTextMasterStyleLevel(
            &d->documentTextInfo.textMasterStyleAtom, level);
    if (!ml) {
        ml = getTextMasterStyleLevel(
                d->documentTextInfo.textMasterStyleAtom2.data(), level);
    }
    return ml;
}
const TextPFException* getDefaultLevelPF(const MSO::DocumentContainer* d,
                                         quint16 level)
{
    const TextMasterStyleLevel* ml = getDefaultLevel(d, level);
    return (ml) ?&ml->pf :0;
}
const TextCFException* getDefaultLevelCF(const MSO::DocumentContainer* d,
                                         quint16 level)
{
    const TextMasterStyleLevel* ml = getDefaultLevel(d, level);
    return (ml) ?&ml->cf :0;
}
const TextPFException* getDefaultPF(const MSO::DocumentContainer* d)
{
    if (d && d->documentTextInfo.textPFDefaultsAtom) {
        return &d->documentTextInfo.textPFDefaultsAtom->pf;
    }
    return 0;
}
const TextCFException* getDefaultCF(const MSO::DocumentContainer* d)
{
    if (d && d->documentTextInfo.textCFDefaultsAtom) {
        return &d->documentTextInfo.textCFDefaultsAtom->cf;
    }
    return 0;
}
template <class Style>
void addStyle(const Style** list, const Style* style)
{
    if (style) {
        while (*list) ++list;
        *list = style;
        *list++;
        *list = 0;
    }
}
}

PptTextPFRun::PptTextPFRun(const MSO::DocumentContainer* d,
             const MSO::MasterOrSlideContainer* m,
             quint32 textType)
{
    level_ = 0;
    *pfs = 0;
    addStyle(pfs, getLevelPF(m, textType, 0));
    addStyle(pfs, getDefaultLevelPF(d, 0));
    addStyle(pfs, getDefaultPF(d));
}

PptTextPFRun::PptTextPFRun(const MSO::DocumentContainer* d,
                           const MasterOrSlideContainer* m,
                           const TextContainer* tc,
                           quint32 start)
{
    const TextPFRun* pfrun = getPFRun(tc, start);
    quint16 level = 0;
    if (pfrun) {
        level = pfrun->indentLevel;
        // [MS-PPT].pdf says there are only 5 levels
        if (level > 4) level = 4;
    }

    *pfs = 0;
    addStyle(pfs, (pfrun) ?&pfrun->pf :0);
    addStyle(pfs, getLevelPF(m, tc, level));
    addStyle(pfs, getBaseLevelPF(m, tc, level));
    addStyle(pfs, getDefaultLevelPF(d, level));
    addStyle(pfs, getDefaultPF(d));

    // the level reported by PptPFRun is 0 when not bullets, i.e. no list is
    // active, 1 is lowest list level, 5 is the highest list level
    level_ = (fHasBullet()) ?level + 1 :0;
}
PptTextCFRun::PptTextCFRun(const MSO::DocumentContainer* d,
                           const MasterOrSlideContainer* m,
                           const TextContainer& tc,
                           quint16 level,
                           quint32 start)
{
    const TextCFRun* cfrun = getCFRun(&tc, start);

    *cfs = 0;
    addStyle(cfs, (cfrun) ?&cfrun->cf :0);
    addStyle(cfs, getLevelCF(m, tc, level));
    addStyle(cfs, getBaseLevelCF(m, &tc, level));
    addStyle(cfs, getDefaultLevelCF(d, level));
    addStyle(cfs, getDefaultCF(d));
}

#define GETTER(TYPE, PARENT, PRE, NAME, TEST, DEFAULT) \
TYPE PptTextPFRun::NAME() const \
{ \
    const MSO::TextPFException* const * p = pfs; \
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
#undef GETTER

#define GETTER(TYPE, PARENT, PRE, NAME, TEST, DEFAULT) \
TYPE PptTextCFRun::NAME() const \
{ \
    const MSO::TextCFException* const * c = cfs; \
    while (*c) { \
        if ((*c)->TEST) { \
            return PRE (*c)->PARENT NAME; \
        } \
        ++c; \
    } \
    return DEFAULT; \
}

//     TYPE             PARENT     PRE NAME           TEST              DEFAULT
GETTER(bool,            fontStyle->,,  bold,          masks.bold,        false)
GETTER(bool,            fontStyle->,,  italic,        masks.italic,      false)
GETTER(bool,            fontStyle->,,  underline,     masks.underline,   false)
GETTER(bool,            fontStyle->,,  shadow,        masks.shadow,      false)
GETTER(bool,            fontStyle->,,  fehint,        masks.fehint,      false)
GETTER(bool,            fontStyle->,,  kumi,          masks.kumi,        false)
GETTER(bool,            fontStyle->,,  emboss,        masks.emboss,      false)
GETTER(quint8,          fontStyle->,,  pp9rt,         fontStyle,             0)
GETTER(quint16,         ,           ,  fontRef,       masks.typeface,        0)
GETTER(quint16,         ,           ,  oldEAFontRef,  masks.oldEATypeface,   0)
GETTER(quint16,         ,           ,  ansiFontRef,   masks.ansiTypeface,    0)
GETTER(quint16,         ,           ,  symbolFontRef, masks.symbolTypeface,  0)
GETTER(quint16,         ,           ,  fontSize,      masks.size,            0)
GETTER(ColorIndexStruct,,           *, color,   masks.color, ColorIndexStruct())
GETTER(qint16,          ,           ,  position,      masks.position,        0)

