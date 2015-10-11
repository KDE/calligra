/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>
   Copyright (C) 2010, 2011 Matus Uzak <matus.uzak@ixonos.com>

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

#include "PptDebug.h"

using namespace MSO;

const TextMasterStyleAtom*
getTextMasterStyleAtom(const MasterOrSlideContainer* m, quint16 textType)
{
    if (!m) return 0;
    const MainMasterContainer* mm = m->anon.get<MainMasterContainer>();
    if (!mm) return 0;
    const TextMasterStyleAtom* textstyle = 0;
    foreach (const TextMasterStyleAtom& ma, mm->rgTextMasterStyle) {
        if (ma.rh.recInstance == textType) {
            textstyle = &ma;
        }
    }
    return textstyle;
}

namespace {

// ************************************************
//  TextPFRun/TextCFRun
// ************************************************
template <class Run>
const Run* getRun(const QList<Run> &runs, quint32 start, quint32& num)
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
    num = end - start;
    return run;
}

const TextPFRun*
getPFRun(const TextContainer* tc, quint32 start)
{
    //TODO: make use of the num variable if required!
    quint32 num;
    if (tc && tc->style) {
        return getRun<TextPFRun>(tc->style->rgTextPFRun, start, num);
    }
    return 0;
}
const TextCFRun*
getCFRun(const TextContainer* tc, const quint32 start, quint32& num)
{
    if (tc && tc->style) {
        return getRun<TextCFRun>(tc->style->rgTextCFRun, start, num);
    }
    return 0;
}
const TextCFException*
getTextCFException(const MSO::TextContainer* tc, const int start)
{
    if (!tc || !tc->style) return 0;
    const QList<TextCFRun> &cfs = tc->style->rgTextCFRun;
    int i = 0;
    int cfend = 0;
    while (i < cfs.size()) {
        cfend += cfs[i].count;
        if (cfend > start) {
            break;
        }
        i++;
    }
    if (i >= cfs.size()) {
        return 0;
    }
    return &cfs[i].cf;
}
// ************************************************
//  Master Style - Level (PF/CF)
// ************************************************

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


const TextPFException* getLevelPF(const MasterOrSlideContainer* m,
                                  const TextContainer* tc, quint16 level)
{
    if (!tc) return 0;
    quint32 textType = tc->textHeaderAtom.textType;
    const TextMasterStyleAtom* ms = getTextMasterStyleAtom(m, textType);
    const TextMasterStyleLevel* ml = getTextMasterStyleLevel(ms, level);
    return (ml) ?&ml->pf :0;
}
const TextCFException* getLevelCF(const MasterOrSlideContainer* m,
                                  const TextContainer* tc, quint16 level)
{
    if (!tc) return 0;
    quint32 textType = tc->textHeaderAtom.textType;
    const TextMasterStyleAtom* ms = getTextMasterStyleAtom(m, textType);
    const TextMasterStyleLevel* ml = getTextMasterStyleLevel(ms, level);
    return (ml) ?&ml->cf :0;
}
// ************************************************
//  Master Style - Base Levels (PF/CF)
// ************************************************

QList<const TextMasterStyleLevel*>
getBaseLevels(const MasterOrSlideContainer* m, const TextContainer* tc, const quint16 level)
{
    QList<const TextMasterStyleLevel*> lst;
    if (!tc) return lst;

    quint32 textType = tc->textHeaderAtom.textType;
    const TextMasterStyleAtom* ms = 0;

    //NOTE: some of the following rules were discussed at the Office File
    //Formats Forum, fine tuning is still required!  Tx_TYPE_OTHER does not
    //inherit, it uses the default values from DocumentContainer.

    switch (textType) {
    case Tx_TYPE_CENTERTITLE:
        ms = getTextMasterStyleAtom(m, Tx_TYPE_TITLE);
        lst.append(getTextMasterStyleLevel(ms, level));
        break;
    case Tx_TYPE_BODY:
        ms = getTextMasterStyleAtom(m, Tx_TYPE_BODY);
        for (int i = level - 1; i >= 0; i--) {
            lst.append(getTextMasterStyleLevel(ms, i));
        }
        break;
    case Tx_TYPE_CENTERBODY:
    case Tx_TYPE_HALFBODY:
    case Tx_TYPE_QUARTERBODY:
        ms = getTextMasterStyleAtom(m, Tx_TYPE_BODY);
        for (int i = level; i >= 0; i--) {
            lst.append(getTextMasterStyleLevel(ms, i));
        }
        break;
    //TODO: Tx_TYPE_NOTES do not inherit at the moment
    default:
        break;
    }

    return lst;
}
QList<const TextPFException*>
getBaseLevelsPF(const MasterOrSlideContainer* m, const TextContainer* tc, quint16 level)
{
    QList<const TextMasterStyleLevel*> mls = getBaseLevels(m, tc, level);
    QList<const TextPFException*> pfs;
    for (int i = 0; i < mls.size(); i++) {
        if (mls[i]) {
            pfs.append(&mls[i]->pf);
        }
    }
    return pfs;
}
QList<const TextCFException*>
getBaseLevelsCF(const MasterOrSlideContainer* m, const TextContainer* tc, quint16 level)
{
    QList<const TextMasterStyleLevel*> mls = getBaseLevels(m, tc, level);
    QList<const TextCFException*> cfs;
    for (int i = 0; i < mls.size(); i++) {
        if (mls[i]) {
            cfs.append(&mls[i]->cf);
        }
    }
    return cfs;
}
// ************************************************
//  Default - Base Levels (PF/CF)
// ************************************************

QList<const TextMasterStyleLevel*>
getDefaultBaseLevels(const MSO::DocumentContainer* d, quint16 level)
{
    QList<const TextMasterStyleLevel*> mls;
    if (!d) return mls;
    while (level > 0) {
        mls.append(getTextMasterStyleLevel(
                      &d->documentTextInfo.textMasterStyleAtom, --level));
    }
    return mls;
}
QList<const TextPFException*>
getDefaultBaseLevelsPF(const MSO::DocumentContainer* d, quint16 level)
{
    QList<const TextMasterStyleLevel*> mls = getDefaultBaseLevels(d, level);
    QList<const TextPFException*> pfs;
    for (int i = 0; i < mls.size(); i++) {
        if (mls[i]) {
            pfs.append(&mls[i]->pf);
        }
    }
    return pfs;
}
QList<const TextCFException*>
getDefaultBaseLevelsCF(const MSO::DocumentContainer* d, quint16 level)
{
    QList<const TextMasterStyleLevel*> mls = getDefaultBaseLevels(d, level);
    QList<const TextCFException*> cfs;
    for (int i = 0; i < mls.size(); i++) {
        if (mls[i]) {
            cfs.append(&mls[i]->cf);
        }
    }
    return cfs;
}
// ************************************************
//  Default - Level (PF/CF)
// ************************************************

const TextMasterStyleLevel*
getDefaultLevel(const MSO::DocumentContainer* d, quint16 level)
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
const TextPFException*
getDefaultLevelPF(const MSO::DocumentContainer* d, quint16 level)
{
    const TextMasterStyleLevel* ml = getDefaultLevel(d, level);
    return (ml) ?&ml->pf :0;
}
const TextCFException*
getDefaultLevelCF(const MSO::DocumentContainer* d, quint16 level)
{
    const TextMasterStyleLevel* ml = getDefaultLevel(d, level);
    return (ml) ?&ml->cf :0;
}

// ************************************************
//  Default (PF/CF)
// ************************************************

const TextPFException*
getDefaultPF(const MSO::DocumentContainer* d)
{
    if (d && d->documentTextInfo.textPFDefaultsAtom) {
        return &d->documentTextInfo.textPFDefaultsAtom->pf;
    }
    return 0;
}
const TextCFException*
getDefaultCF(const MSO::DocumentContainer* d)
{
    if (d && d->documentTextInfo.textCFDefaultsAtom) {
        return &d->documentTextInfo.textCFDefaultsAtom->cf;
    }
    return 0;
}

// ************************************************
//  Master Style 9
// ************************************************
const TextMasterStyle9Atom*
getTextMasterStyle9Atom(const PP9SlideBinaryTagExtension* m, quint16 texttype)
{
    if (!m) return 0;
    const TextMasterStyle9Atom* textstyle = 0;
    foreach (const TextMasterStyle9Atom&ma, m->rgTextMasterStyleAtom) {
        if (ma.rh.recInstance == texttype) {
            textstyle = &ma;
        }
    }
    return textstyle;
}
const TextMasterStyle9Atom*
getTextMasterStyle9Atom(const PP9DocBinaryTagExtension* m, quint16 texttype)
{
    if (!m) return 0;
    const TextMasterStyle9Atom* textstyle = 0;
    foreach (const TextMasterStyle9Atom&ma, m->rgTextMasterStyle9) {
        if (ma.rh.recInstance == texttype) {
            textstyle = &ma;
        }
    }
    return textstyle;
}

const MSO::StyleTextProp9*
getStyleTextProp9(const MSO::DocumentContainer* d, quint32 slideIdRef,
                  quint32 textType, quint8 pp9rt)
{
    const PP9DocBinaryTagExtension* pp9 = getPP<PP9DocBinaryTagExtension>(d);
    if (pp9 && pp9->outlineTextPropsContainer) {
        foreach (const OutlineTextProps9Entry& o,
                 pp9->outlineTextPropsContainer->rgOutlineTextProps9Entry) {
            if (o.outlineTextHeaderAtom.slideIdRef == slideIdRef
                    && o.outlineTextHeaderAtom.txType == textType) {
                // we assume that pp9rt is the index in this array
                if (o.styleTextProp9Atom.rgStyleTextProp9.size() > pp9rt) {
                    return &o.styleTextProp9Atom.rgStyleTextProp9[pp9rt];
                }
            }
        }
    }
    return 0;
}
const MSO::StyleTextProp9*
getStyleTextProp9(const PptOfficeArtClientData* pcd, quint8 pp9rt)
{
    const PP9ShapeBinaryTagExtension* p = 0;
    if (pcd) {
        p = getPP<PP9ShapeBinaryTagExtension>(*pcd);
    }
    if (p && p->styleTextProp9Atom.rgStyleTextProp9.size() > pp9rt) {
        return &p->styleTextProp9Atom.rgStyleTextProp9[pp9rt];
    }
    return 0;
}
const TextPFException9* getPF9(const MSO::DocumentContainer* d,
                               const MSO::SlideListWithTextSubContainerOrAtom* texts,
                               const PptOfficeArtClientData* pcd,
                               const MSO::TextContainer* tc,
                               const int start)
{
    // to find the pf9, the cf has to be obtained which contains a pp9rt
    quint8 pp9rt = 0;
    const TextCFException* cf = getTextCFException(tc, start);
    if (cf && cf->fontStyle) {
        pp9rt = cf->fontStyle->pp9rt;
    }
    const StyleTextProp9* stp9 = 0;
    if (tc) {
        quint32 textType = tc->textHeaderAtom.textType;
        stp9 = getStyleTextProp9(pcd, pp9rt);
        if (!stp9 && texts) {
            stp9 = getStyleTextProp9(d, texts->slidePersistAtom.slideId.slideId,
                                     textType, pp9rt);
        }
    }
    return (stp9) ?&stp9->pf9 :0;
}
const TextMasterStyle9Level* getMaster9Level(const MasterOrSlideContainer* m,
                                              quint32 textType,
                                              quint16 level)
{
    const PP9SlideBinaryTagExtension* pp9 = getPP<PP9SlideBinaryTagExtension>(m);
    if (!pp9) return 0;
    const TextMasterStyle9Atom* ms = getTextMasterStyle9Atom(pp9, textType);
    const TextMasterStyle9Level *l = 0;
    if (!ms) return l;
    switch (level) {
    case 0: if (ms->lstLvl1) l = ms->lstLvl1.data();break;
    case 1: if (ms->lstLvl2) l = ms->lstLvl2.data();break;
    case 2: if (ms->lstLvl3) l = ms->lstLvl3.data();break;
    case 3: if (ms->lstLvl4) l = ms->lstLvl4.data();break;
    case 4: if (ms->lstLvl5) l = ms->lstLvl5.data();break;
    }
    return l;
}
const TextPFException9* getLevelPF9(const MasterOrSlideContainer* m,
                                    quint32 textType,
                                    quint16 level)
{
    const TextMasterStyle9Level* ml = getMaster9Level(m, textType, level);
    return (ml) ?&ml->pf9 :0;
}
const TextPFException9* getLevelPF9(const MasterOrSlideContainer* m,
                                    const TextContainer* tc,
                                    quint16 level)
{
    return (tc) ?getLevelPF9(m, tc->textHeaderAtom.textType, level) :0;
}
const TextMasterStyle9Level* getDefault9Level(const MSO::DocumentContainer* d,
                                              quint32 textType,
                                              quint16 level)
{
    const PP9DocBinaryTagExtension* pp9 = getPP<PP9DocBinaryTagExtension>(d);
    if (!pp9) return 0;
    const TextMasterStyle9Atom* ms = getTextMasterStyle9Atom(pp9, textType);
    if (!ms) return 0;
    const TextMasterStyle9Level *l = 0;
    switch (level) {
    case 0: if (ms->lstLvl1) l = ms->lstLvl1.data();break;
    case 1: if (ms->lstLvl2) l = ms->lstLvl2.data();break;
    case 2: if (ms->lstLvl3) l = ms->lstLvl3.data();break;
    case 3: if (ms->lstLvl4) l = ms->lstLvl4.data();break;
    case 4: if (ms->lstLvl5) l = ms->lstLvl5.data();break;
    }
    return l;
}
const TextPFException9* getDefaultLevelPF9(const MSO::DocumentContainer* d,
                                           quint32 textType,
                                           quint16 level)
{
    const TextMasterStyle9Level* ml = getDefault9Level(d, textType, level);
    return (ml) ?&ml->pf9 :0;
}
const TextPFException9* getDefaultLevelPF9(const MSO::DocumentContainer* d,
                                           const MSO::TextContainer* tc,
                                           quint16 level)
{
    return (tc) ?getDefaultLevelPF9(d, tc->textHeaderAtom.textType, level) :0;
}
const TextPFException9* getDefaultPF9(const MSO::DocumentContainer* d)
{
    const PP9DocBinaryTagExtension* pp9 = getPP<PP9DocBinaryTagExtension>(d);
    return (pp9 && pp9->textDefaultsAtom) ?&pp9->textDefaultsAtom->pf9 :0;
}
// ************************************************
//  LeftMargin + Indent
// ************************************************

qint16 getLeftMargin(const TextRuler* tr, quint16 level)
{
    if (!tr) {
        return -1;
    }
    qint16 m = -1;
    switch (level) {
    case 0: if (tr->fLeftMargin1) m = tr->leftMargin1; break;
    case 1: if (tr->fLeftMargin2) m = tr->leftMargin2; break;
    case 2: if (tr->fLeftMargin3) m = tr->leftMargin3; break;
    case 3: if (tr->fLeftMargin4) m = tr->leftMargin4; break;
    case 4: if (tr->fLeftMargin5) m = tr->leftMargin5; break;
    }
    return m;
}

qint16 getDefaultLeftMargin(const MSO::DocumentContainer* d, quint16 level)
{
    if (d && d->documentTextInfo.defaultRulerAtom) {
        const MSO::TextRuler* tr = &d->documentTextInfo.defaultRulerAtom->defaultTextRuler;
        return getLeftMargin(tr, level);
    }
    return -1;
}

qint16 getIndent(const TextRuler* tr, quint16 level)
{
    if (!tr) {
        return -1;
    }
    qint16 indent = -1;
    switch (level) {
    case 0: if (tr->fIndent1) indent = tr->indent1; break;
    case 1: if (tr->fIndent2) indent = tr->indent2; break;
    case 2: if (tr->fIndent3) indent = tr->indent3; break;
    case 3: if (tr->fIndent4) indent = tr->indent4; break;
    case 4: if (tr->fIndent5) indent = tr->indent5; break;
    }
    return indent;
}

qint16 getDefaultIndent(const MSO::DocumentContainer* d, quint16 level)
{
    if (d && d->documentTextInfo.defaultRulerAtom) {
        const MSO::TextRuler* tr = &d->documentTextInfo.defaultRulerAtom->defaultTextRuler;
        return getIndent(tr, level);
    }
    return -1;
}

} //namespace (anonymous)

PptTextPFRun::PptTextPFRun(const DocumentContainer* d)
    : m_level(0)
    , m_textType(Tx_TYPE_TITLE)
    , m_fHasBullet(false)
{
    //check DocumentContainer/DocumentTextInfoContainer/textPFDefaultsAtom
    pfs.append(getDefaultPF(d));
}

PptTextPFRun::PptTextPFRun(const DocumentContainer* d,
                           const TextMasterStyleLevel* level,
                           const TextMasterStyle9Level* level9,
                           const quint32 textType,
                           const quint16 indentLevel)
    : m_level(indentLevel)
    , m_textType(textType)
    , m_fHasBullet(false)
{
    //TODO: TextRuler required!

    if (level) {
        pfs.append(&level->pf);
    }

    if (level9) {
        pf9s.append(&level9->pf9);
    }
    processPFDefaults(d);
}

PptTextPFRun::PptTextPFRun(const DocumentContainer* d,
                           const MasterOrSlideContainer* m,
                           const SlideListWithTextSubContainerOrAtom* texts,
                           const PptOfficeArtClientData* pcd,
                           const TextContainer* tc,
                           const TextRuler* tr,
                           quint32 start)
    : m_level(0)
    , m_textType(Tx_TYPE_TITLE)
    , m_fHasBullet(false)
{
    const TextPFRun* pfrun = getPFRun(tc, start);

    if (pfrun) {
        pfs.append(&pfrun->pf);
        m_level = pfrun->indentLevel;
        if (m_level > 4) {
            m_level = 4;
        }
    } else {
        debugPpt << "TextPFRun MISSING!";
    }

    //check the main master/title master slide's TextMasterStyleAtom
    pfs.append(getLevelPF(m, tc, m_level));
    pfs.append(getBaseLevelsPF(m, tc, m_level));

    //TODO: test documents required to construct correct pf9s based on the full
    //masters hierarchy.
    pf9s.append(getPF9(d, texts, pcd, tc, start));
    pf9s.append(getLevelPF9(m, tc, m_level));
    pf9s.append(getDefaultLevelPF9(d, tc, m_level));
    pf9s.append(getDefaultPF9(d));

    // the level reported by PptPFRun is 0 when not bullets, i.e. no list is
    // active, 1 is lowest list level, 5 is the highest list level
//     m_level = (level || fHasBullet()) ?level + 1 :0;

    //FIXME: The previous assumption is not true, there are cases when
    //(indentLevel > 0 && fHasBullet == false).  At the moment such run of text
    //is interpreted as content of a paragraph with corresponding indentLevel,
    //but it looks like we should store it as a text:list-item.

    //set the default values of bullet properties
    if (tc) {
        m_textType = tc->textHeaderAtom.textType;
    }

    //Processing either OfficeArtClientTextbox or a SlideListWithTextContainer.
    //The TextRulerAtom is provided only in case of OfficeArtClientTextbox.
    m_leftMargin.append(getLeftMargin(tr, m_level));
    m_indent.append(getIndent(tr, m_level));

    processPFDefaults(d);
}

void PptTextPFRun::processPFDefaults(const DocumentContainer* d)
{
    //check DocumentContainer/DocumentTextInfoContainer/textMasterStyleAtom
    pfs.append(getDefaultLevelPF(d, m_level));
    pfs.append(getDefaultBaseLevelsPF(d, m_level));
    //check DocumentContainer/DocumentTextInfoContainer/textPFDefaultsAtom
    pfs.append(getDefaultPF(d));

    //check DocumentContainer/DocumentTextInfoContainer/DefaultRulerAtom
    m_leftMargin.append(getDefaultLeftMargin(d, m_level));
    m_indent.append(getDefaultIndent(d, m_level));

    //TODO: Fine tuning on test documents required for default values.
    switch (m_textType) {
    case Tx_TYPE_TITLE:
    case Tx_TYPE_CENTERTITLE:
        m_fHasBullet = false;
        break;
    case Tx_TYPE_BODY:
        m_fHasBullet = true;
        break;
    default:
        m_fHasBullet = false;
        break;
    }
}


PptTextCFRun::PptTextCFRun(const DocumentContainer* d)
    : m_level(99)
    , m_cfrun_rm(false)
{
    //check DocumentContainer/DocumentTextInfoContainer/textCFDefaultsAtom
    cfs.append(getDefaultCF(d));
}

PptTextCFRun::PptTextCFRun(const DocumentContainer* d,
                           const TextMasterStyleLevel* level,
                           const TextMasterStyle9Level* /*level9*/,
                           const quint16 indentLevel)
    : m_level(indentLevel)
    , m_cfrun_rm(false)
{
    if (level) {
        cfs.append(&level->cf);
    }
    processCFDefaults(d);
}

PptTextCFRun::PptTextCFRun(const DocumentContainer* d,
                           const MasterOrSlideContainer* m,
                           const TextContainer* tc,
                           quint16 indentLevel)
    : m_level(indentLevel)
    , m_cfrun_rm(false)
{
    //check the main main master slide's TextMasterStyleAtom
    cfs.append(getLevelCF(m, tc, m_level));
    cfs.append(getBaseLevelsCF(m, tc, m_level));
    processCFDefaults(d);
}

void PptTextCFRun::processCFDefaults(const DocumentContainer* d)
{
    //check DocumentContainer/DocumentTextInfoContainer/textMasterStyleAtom
    cfs.append(getDefaultLevelCF(d, m_level));
    cfs.append(getDefaultBaseLevelsCF(d, m_level));
    //check DocumentContainer/DocumentTextInfoContainer/textCFDefaultsAtom
    cfs.append(getDefaultCF(d));
}

int PptTextCFRun::addCurrentCFRun(const TextContainer* tc, quint32 start, quint32& num)
{
    int n = -1;

    //remove pointer to TextCFException structure of the previous run of text
    if (m_cfrun_rm) {
        cfs.removeFirst();
        m_cfrun_rm = false;
    }

    const TextCFRun* cfrun = getCFRun(tc, start, num);
    if (cfrun) {
        cfs.prepend(&cfrun->cf);
        n = cfrun->count;
        num = n - num;
        m_cfrun_rm = true;
    }
    return n;
}

void PptTextCFRun::removeCurrentCFRun()
{
    if (!cfs.isEmpty()) {
        cfs.removeFirst() ;
        m_cfrun_rm = false;
    }
}

#define GETTER(TYPE, PARENT, PRE, NAME, TEST, DEFAULT) \
TYPE PptTextPFRun::NAME() const \
{ \
    for (int i = 0; i < pfs.size(); i++) { \
        if (pfs[i]) { \
            if ((pfs[i])->masks.TEST) {	\
                return PRE (pfs[i])->PARENT NAME; \
            } \
        } \
    } \
    return DEFAULT; \
}
//     TYPE      PARENT       PRE NAME             TEST            DEFAULT
GETTER(bool,     bulletFlags->,,  fHasBullet,      hasBullet,      m_fHasBullet)
GETTER(bool,     bulletFlags->,,  fBulletHasFont,  bulletHasFont,  false)
GETTER(bool,     bulletFlags->,,  fBulletHasColor, bulletHasColor, false)
GETTER(bool,     bulletFlags->,,  fBulletHasSize,  bulletHasSize,  false)
GETTER(qint16,   ,             ,  bulletChar,      bulletChar,     0)
GETTER(quint16,  ,             ,  textAlignment,   align,          0)
GETTER(qint16,   ,             ,  lineSpacing,     lineSpacing,    0)
GETTER(qint16,   ,             ,  spaceBefore,     spaceBefore,    0)
GETTER(qint16,   ,             ,  spaceAfter,      spaceAfter,     0)
GETTER(quint16,  ,             ,  defaultTabSize,  defaultTabSize, 0)
GETTER(TabStops, ,             *, tabStops,        tabStops,       0)
GETTER(quint16,  ,             ,  fontAlign,       fontAlign,      0)
GETTER(bool,     wrapFlags->,  ,  charWrap,        wordWrap,       false)
GETTER(bool,     wrapFlags->,  ,  wordWrap,        wordWrap,       false)
GETTER(bool,     wrapFlags->,  ,  overflow,        overflow,       false)
GETTER(quint16,  ,             ,  textDirection,   textDirection,  0)
#undef GETTER

#define GETTER(TYPE, TR_VAL, NAME, TEST, DEFAULT) \
TYPE PptTextPFRun::NAME() const \
{ \
    if (TR_VAL.size() > 1) { \
        if (TR_VAL[0] > -1) { \
            return TR_VAL[0]; \
        } \
    } \
    for (int i = 0; i < pfs.size(); i++) { \
        if (pfs[i]) { \
            if ((pfs[i])->masks.TEST) { \
                return (pfs[i])->NAME; \
            } \
        } \
    } \
    if (!TR_VAL.isEmpty()) { \
        if (TR_VAL.last() > -1) { \
            return TR_VAL.last(); \
        } \
    } \
    return DEFAULT; \
}
//     TYPE      TEXTRULER_VALUE   NAME             TEST            DEFAULT
GETTER(quint16,  m_leftMargin,     leftMargin,      leftMargin,     0)
GETTER(quint16,  m_indent,         indent,          indent,         0)
#undef GETTER


#define GETTER(TYPE, PRE, NAME, TEST, VALID, DEFAULT) \
TYPE PptTextPFRun::NAME() const \
{ \
    for (int i = 0; i < pfs.size(); i++) { \
        if (pfs[i]) { \
            if ((pfs[i])->masks.TEST) {	\
                if (VALID()) { \
                    return PRE (pfs[i])->NAME; \
                } \
            } \
        } \
    } \
    return DEFAULT; \
}
//     TYPE             PRE NAME             TEST         VALID            DEFAULT
GETTER(quint16,          ,  bulletFontRef,   bulletFont,  fBulletHasFont,  0)
GETTER(qint16,           ,  bulletSize,      bulletSize,  fBulletHasSize,  0)
GETTER(ColorIndexStruct, *, bulletColor,     bulletColor, fBulletHasColor, ColorIndexStruct())
#undef GETTER

#define ANM_Default 3
#define GETTER(TYPE, NAME, PARENT, TEST, DEFAULT)	\
TYPE PptTextPFRun::NAME() const \
{ \
    for (int i = 0; i < pf9s.size(); i++) { \
        if (pf9s[i]) { \
            if (pf9s[i]->masks.TEST) { \
                return pf9s[i]->PARENT NAME; \
            } \
        } \
    } \
    return DEFAULT; \
}

//     TYPE       NAME                    PARENT                    TEST              DEFAULT
GETTER(qint32,    bulletBlipRef,          ,                         bulletBlip,       65535)
GETTER(qint16,    fBulletHasAutoNumber,   ,                         bulletHasScheme,  0)
GETTER(quint16,   scheme,                 bulletAutoNumberScheme->, bulletScheme,     ANM_Default)
GETTER(qint16,    startNum,               bulletAutoNumberScheme->, bulletScheme,     1)
#undef GETTER


//FIXME: Looks like only Tx_TYPE_OTHER is not stored in lists.  Test files are
//required to prove this!

//NOTE: Logic exploited from test documents saved in MS PowerPoint 2003/2007
bool PptTextPFRun::isList() const {
    bool ret = false;
    if ( fHasBullet() ||
         fBulletHasAutoNumber() ||
         (bulletBlipRef() != 65535)
//         (bulletChar() > 0) ||
//         fBulletHasFont() ||
       )
    {
        ret = true;
    }
    return ret;
}

#define GETTER(TYPE, PARENT, PRE, NAME, TEST, DEFAULT) \
TYPE PptTextCFRun::NAME() const \
{ \
    for (int i = 0; i < cfs.size(); i++) { \
        if (cfs[i]) { \
            if ((cfs[i])->TEST) { \
                return PRE (cfs[i])->PARENT NAME; \
            } \
        } \
    } \
    return DEFAULT; \
}

//     TYPE             PARENT      PRE NAME           TEST              DEFAULT
GETTER(bool,            fontStyle->, ,  bold,          masks.bold,        false)
GETTER(bool,            fontStyle->, ,  italic,        masks.italic,      false)
GETTER(bool,            fontStyle->, ,  underline,     masks.underline,   false)
GETTER(bool,            fontStyle->, ,  shadow,        masks.shadow,      false)
GETTER(bool,            fontStyle->, ,  fehint,        masks.fehint,      false)
GETTER(bool,            fontStyle->, ,  kumi,          masks.kumi,        false)
GETTER(bool,            fontStyle->, ,  emboss,        masks.emboss,      false)
GETTER(quint8,          fontStyle->, ,  pp9rt,         fontStyle,             0)
GETTER(quint16,         ,            ,  fontRef,       masks.typeface,        0)
GETTER(quint16,         ,            ,  oldEAFontRef,  masks.oldEATypeface,   0)
GETTER(quint16,         ,            ,  ansiFontRef,   masks.ansiTypeface,    0)
GETTER(quint16,         ,            ,  symbolFontRef, masks.symbolTypeface,  0)
GETTER(quint16,         ,            ,  fontSize,      masks.size,            0)
GETTER(ColorIndexStruct,,            *, color,         masks.color,           ColorIndexStruct())
GETTER(qint16,          ,            ,  position,      masks.position,        0)
#undef GETTER
