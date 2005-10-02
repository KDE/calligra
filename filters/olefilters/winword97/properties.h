/*
    Copyright (C) 2000, S.R.Haque <shaheedhaque@hotmail.com>.
    This file is part of the KDE project

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

DESCRIPTION

    This file implements an abstraction for properties in Microsoft
    Word documents. In other words, it is an abstraction for the PAP/CHP/TAP
    structures.
*/

#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <msword.h>

class Properties
{
public:

    // Create an object with default properties.

    Properties(MsWord &document);
    Properties(const Properties &original);
    ~Properties();

    // Modify the properties with style information from various sources...
    //
    // An array of SPRMs (grpprl).
    // An existing base style.
    // List format.
    // Paragraph property set.
    // Paragraph property exceptions.
    // Paragraph height.
    // Predefined style from stylesheet.

    void apply(const MsWord::U8 *grpprl, unsigned count);
    void apply(MsWord::U16 style);
    void apply(const MsWord::LFO &style);
    void apply(const MsWord::PAP &style);
    void apply(const MsWord::PAPXFKP &style);
    void apply(const MsWord::PHE &layout);
    void apply(const MsWord::STD &style);

    // Conversion from compact PRM opcode to real opcode.

    static MsWord::U16 getRealOpcode(unsigned shortOpcode, const MsWord::FIB &fib);

    // Get the properties.

    const MsWord::PAP *getPap(void) const { return &m_pap; }
    const MsWord::CHP *getChp(void) const { return &m_chp; }
    const MsWord::TAP *getTap(void) const { return &m_tap; }
private:
    friend class MsWord;

    MsWord &m_document;
    MsWord::PAP m_pap;

    // The character properties.

    MsWord::CHP m_chp;

    // Any table properties.

    MsWord::TAP m_tap;

    // Encodings of all Word97 sprms.

    typedef enum
    {
        sprmNoop = 0x0000,
        sprmCFRMarkDel = 0x0800,
        sprmCFRMark = 0x0801,
        sprmCFFldVanish = 0x0802,
        sprmCFData = 0x0806,
        sprmCFOle2 = 0x080A,
        sprmCFBold = 0x0835,
        sprmCFItalic = 0x0836,
        sprmCFStrike = 0x0837,
        sprmCFOutline = 0x0838,
        sprmCFShadow = 0x0839,
        sprmCFSmallCaps = 0x083A,
        sprmCFCaps = 0x083B,
        sprmCFVanish = 0x083C,
        sprmCFImprint = 0x0854,
        sprmCFSpec = 0x0855,
        sprmCFObj = 0x0856,
        sprmCFEmboss = 0x0858,
        sprmCFBiDi = 0x085A,
        sprmCFDiacColor = 0x085B,
        sprmCFBoldBi = 0x085C,
        sprmCFItalicBi = 0x085D,
        sprmCFUsePgsuSettings = 0x0868,
        sprmPJc = 0x2403,
        sprmPFSideBySide = 0x2404,
        sprmPFKeep = 0x2405,
        sprmPFKeepFollow = 0x2406,
        sprmPFPageBreakBefore = 0x2407,
        sprmPBrcl = 0x2408,
        sprmPBrcp = 0x2409,
        sprmPFNoLineNumb = 0x240C,
        sprmPFInTable = 0x2416,
        sprmPFTtp = 0x2417,
        sprmPWr = 0x2423,
        sprmPFNoAutoHyph = 0x242A,
        sprmPFLocked = 0x2430,
        sprmPFWidowControl = 0x2431,
        sprmPFKinsoku = 0x2433,
        sprmPFWordWrap = 0x2434,
        sprmPFOverflowPunct = 0x2435,
        sprmPFTopLinePunct = 0x2436,
        sprmPFAutoSpaceDE = 0x2437,
        sprmPFAutoSpaceDN = 0x2438,
        sprmPISnapBaseLine = 0x243B,
        sprmPFBiDi = 0x2441,
        sprmPFNumRMIns = 0x2443,
        sprmPCrLf = 0x2444,
        sprmPFUsePgsuSettings = 0x2447,
        sprmPFAdjustRight = 0x2448,
        sprmPIncLvl = 0x2602,
        sprmPIlvl = 0x260A,
        sprmPPc = 0x261B,
        sprmPOutLvl = 0x2640,
        sprmCSfxText = 0x2859,
        sprmCIdctHint = 0x286F,
        sprmCHighlight = 0x2A0C,
        sprmCFFtcAsciSymb = 0x2A10,
        sprmCDefault = 0x2A32,
        sprmCPlain = 0x2A33,
        sprmCKcd = 0x2A34,
        sprmCKul = 0x2A3E,
        sprmCIco = 0x2A42,
        sprmCHpsInc = 0x2A44,
        sprmCHpsPosAdj = 0x2A46,
        sprmCIss = 0x2A48,
        sprmCFDStrike = 0x2A53,
        sprmPicBrcl = 0x2E00,
        sprmScnsPgn = 0x3000,
        sprmSiHeadingPgn = 0x3001,
        sprmSFEvenlySpaced = 0x3005,
        sprmSFProtected = 0x3006,
        sprmSBkc = 0x3009,
        sprmSFTitlePage = 0x300A,
        sprmSFAutoPgn = 0x300D,
        sprmSNfcPgn = 0x300E,
        sprmSFPgnRestart = 0x3011,
        sprmSFEndnote = 0x3012,
        sprmSLnc = 0x3013,
        sprmSGprfIhdt = 0x3014,
        sprmSLBetween = 0x3019,
        sprmSVjc = 0x301A,
        sprmSBOrientation = 0x301D,
        sprmSBCustomize = 0x301E,
        sprmSFBiDi = 0x3228,
        sprmSFFacingCol = 0x3229,
        sprmSFRTLGutter = 0x322A,
        sprmTFCantSplit = 0x3403,
        sprmTTableHeader = 0x3404,
        sprmPWHeightAbs = 0x442B,
        sprmPDcs = 0x442C,
        sprmPShd = 0x442D,
        sprmPWAlignFont = 0x4439,
        sprmPFrameTextFlow = 0x443A,
        sprmPIstd = 0x4600,
        sprmPIlfo = 0x460B,
        sprmPNest = 0x4610,
        sprmPBrcTop10 = 0x461C,
        sprmPBrcLeft10 = 0x461D,
        sprmPBrcBottom10 = 0x461E,
        sprmPBrcRight10 = 0x461F,
        sprmPBrcBetween10 = 0x4620,
        sprmPBrcBar10 = 0x4621,
        sprmPDxaFromText10 = 0x4622,
        sprmCIbstRMark = 0x4804,
        sprmCIdslRMark = 0x4807,
        sprmCIdCharType = 0x480B,
        sprmCHpsPos = 0x4845,
        sprmCHpsKern = 0x484B,
        sprmCYsri = 0x484E,
        sprmCCharScale = 0x4852,
        sprmCLidBi = 0x485F,
        sprmCIbstRMarkDel = 0x4863,
        sprmCShd = 0x4866,
        sprmCIdslRMarkDel = 0x4867,
        sprmCCpg = 0x486B,
        sprmCRgLid0 = 0x486D,
        sprmCRgLid1 = 0x486E,
        sprmCIstd = 0x4A30,
        sprmCFtcDefault = 0x4A3D,
        sprmCLid = 0x4A41,
        sprmCHps = 0x4A43,
        sprmCHpsMul = 0x4A4D,
        sprmCRgFtc0 = 0x4A4F,
        sprmCRgFtc1 = 0x4A50,
        sprmCRgFtc2 = 0x4A51,
        sprmCFtcBi = 0x4A5E,
        sprmCIcoBi = 0x4A60,
        sprmCHpsBi = 0x4A61,
        sprmSDmBinFirst = 0x5007,
        sprmSDmBinOther = 0x5008,
        sprmSCcolumns = 0x500B,
        sprmSNLnnMod = 0x5015,
        sprmSLnnMin = 0x501B,
        sprmSPgnStart = 0x501C,
        sprmSDmPaperReq = 0x5026,
        sprmSClm = 0x5032,
        sprmSTextFlow = 0x5033,
        sprmSPgbProp = 0x522F,
        sprmTJc = 0x5400,
        sprmTFBiDi = 0x560B,
        sprmTDelete = 0x5622,
        sprmTMerge = 0x5624,
        sprmTSplit = 0x5625,
        sprmPDyaLine = 0x6412,
        sprmPBrcTop = 0x6424,
        sprmPBrcLeft = 0x6425,
        sprmPBrcBottom = 0x6426,
        sprmPBrcRight = 0x6427,
        sprmPBrcBetween = 0x6428,
        sprmPBrcBar = 0x6629,
        sprmPHugePapx = 0x6645,
        sprmCDttmRMark = 0x6805,
        sprmCObjLocation = 0x680E,
        sprmCDttmRMarkDel = 0x6864,
        sprmCBrc = 0x6865,
        sprmCPicLocation = 0x6A03,
        sprmCSymbol = 0x6A09,
        sprmPicBrcTop = 0x6C02,
        sprmPicBrcLeft = 0x6C03,
        sprmPicBrcBottom = 0x6C04,
        sprmPicBrcRight = 0x6C05,
        sprmSBrcTop = 0x702B,
        sprmSBrcLeft = 0x702C,
        sprmSBrcBottom = 0x702D,
        sprmSBrcRight = 0x702E,
        sprmSDxtCharSpace = 0x7030,
        sprmTTlp = 0x740A,
        sprmTHTMLProps = 0x740C,
        sprmTInsert = 0x7621,
        sprmTDxaCol = 0x7623,
        sprmTSetShd = 0x7627,
        sprmTSetShdOdd = 0x7628,
        sprmTTextFlow = 0x7629,
        sprmPDxaRight = 0x840E,
        sprmPDxaLeft = 0x840F,
        sprmPDxaLeft1 = 0x8411,
        sprmPDxaAbs = 0x8418,
        sprmPDyaAbs = 0x8419,
        sprmPDxaWidth = 0x841A,
        sprmPDyaFromText = 0x842E,
        sprmPDxaFromText = 0x842F,
        sprmCDxaSpace = 0x8840,
        sprmSDxaColumns = 0x900C,
        sprmSDxaLnn = 0x9016,
        sprmSDyaTop = 0x9023,
        sprmSDyaBottom = 0x9024,
        sprmSDyaLinePitch = 0x9031,
        sprmTDyaRowHeight = 0x9407,
        sprmTDxaLeft = 0x9601,
        sprmTDxaGapHalf = 0x9602,
        sprmPDyaBefore = 0xA413,
        sprmPDyaAfter = 0xA414,
        sprmSDyaPgn = 0xB00F,
        sprmSDxaPgn = 0xB010,
        sprmSDyaHdrTop = 0xB017,
        sprmSDyaHdrBottom = 0xB018,
        sprmSXaPage = 0xB01F,
        sprmSYaPage = 0xB020,
        sprmSDxaLeft = 0xB021,
        sprmSDxaRight = 0xB022,
        sprmSDzaGutter = 0xB025,
        sprmPIstdPermute = 0xC601,
        sprmPChgTabsPapx = 0xC60D,
        sprmPChgTabs = 0xC615,
        sprmPRuler = 0xC632,
        sprmPAnld = 0xC63E,
        sprmPPropRMark = 0xC63F,
        sprmPNumRM = 0xC645,
        sprmCIstdPermute = 0xCA31,
        sprmCMajority = 0xCA47,
        sprmCHpsNew50 = 0xCA49,
        sprmCHpsInc1 = 0xCA4A,
        sprmCMajority50 = 0xCA4C,
        sprmCPropRMark = 0xCA57,
        sprmCDispFldRMark = 0xCA62,
        sprmPicScale = 0xCE01,
        sprmSOlstAnm = 0xD202,
        sprmSPropRMark = 0xD227,
        sprmTTableBorders = 0xD605,
        sprmTDefTable10 = 0xD606,
        sprmTDefTable = 0xD608,
        sprmTDefTableShd = 0xD609,
        sprmTSetBrc = 0xD620,
        sprmTSetBrc10 = 0xD626,
        sprmTDiagLine = 0xD62A,
        sprmTVertMerge = 0xD62B,
        sprmTVertAlign = 0xD62C,
        sprmCChs = 0xEA08,
        sprmCSizePos = 0xEA3F,
        sprmSDxaColWidth = 0xF203,
        sprmSDxaColSpacing = 0xF204
    } opcodes;
};
#endif // PARAGRAPH_H
