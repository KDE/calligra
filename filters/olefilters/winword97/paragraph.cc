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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

DESCRIPTION

    This file implements an abstraction for paragraph properties in Microsoft
    Word documents. In other words, it is an abstraction for the PAP structure.
*/

#include <paragraph.h>

// Create a paragraph with default properties.
Paragraph::Paragraph(MsWord &document) :
    m_document(document)
{
    memset(&m_pap, 0, sizeof(m_pap));
    m_pap.fWidowControl = 1;
    m_pap.lspd.fMultLinespace = 1;
    m_pap.lspd.dyaLine = 240;
    m_pap.lvl = 9;
}

Paragraph::~Paragraph()
{
}

// An array of SPRMs (grpprl) with an optional TAP.
void Paragraph::apply(const MsWord::U8 *grpprl, unsigned count, MsWord::TAP *tap)
{
    // Encodings of all Word97 sprms.

    typedef enum
    {
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
    union
    {
        MsWord::U16 value;
        struct
        {
            MsWord::U16 ispmd: 9;
            MsWord::U16 fSpec: 1;
            MsWord::U16 sgc: 3;
            MsWord::U16 spra: 3;
        } bits;
    } opcode;
    unsigned operandSize;
    unsigned operandSizes[8] =
    {
        1,
        1,
        2,
        4,
        2,
        2,
        0,
        3
    };
    const MsWord::U8 *in = grpprl;
    unsigned bytes = 0;

    // Walk the grpprl.

    while (bytes < count)
    {
        if (m_document.m_fib.nFib > MsWord::s_maxWord6Version)
        {
            bytes += MsWordGenerated::read(in + bytes, &opcode.value);
        }
        else
        {
            MsWord::U8 sprm;

            // Convert sprm to new format.

            bytes += MsWordGenerated::read(in + bytes, &sprm);
            switch (sprm)
            {
            case 2:
                opcode.value = sprmPIstd;
                break;
            case 3:
                opcode.value = sprmPIstdPermute;
                break;
            case 4:
                opcode.value = sprmPIncLvl;
                break;
            case 5:
                opcode.value = sprmPJc;
                break;
            case 6:
                opcode.value = sprmPFSideBySide;
                break;
            case 7:
                opcode.value = sprmPFKeep;
                break;
            case 8:
                opcode.value = sprmPFKeepFollow;
                break;
            case 9:
                opcode.value = sprmPFPageBreakBefore;
                break;
            case 10:
                opcode.value = sprmPBrcl;
                break;
            case 11:
                opcode.value = sprmPBrcp;
                break;
            case 12:
                opcode.value = sprmPAnld;
                break;
            case 13:
                opcode.value = sprmPIlvl;
                break;
            case 14:
                opcode.value = sprmPFNoLineNumb;
                break;
            case 15:
                opcode.value = sprmPChgTabsPapx;
                break;
            case 16:
                opcode.value = sprmPDxaRight;
                break;
            case 17:
                opcode.value = sprmPDxaLeft;
                break;
            case 18:
                opcode.value = sprmPNest;
                break;
            case 19:
                opcode.value = sprmPDxaLeft1;
                break;
            case 20:
                opcode.value = sprmPDyaLine;
                break;
            case 21:
                opcode.value = sprmPDyaBefore;
                break;
            case 22:
                opcode.value = sprmPDyaAfter;
                break;
            case 23:
                opcode.value = sprmPChgTabs;
                break;
            case 24:
                opcode.value = sprmPFInTable;
                break;
            case 25:
                opcode.value = sprmPFTtp;
                break;
            case 26:
                opcode.value = sprmPDxaAbs;
                break;
            case 27:
                opcode.value = sprmPDyaAbs;
                break;
            case 28:
                opcode.value = sprmPDxaWidth;
                break;
            case 29:
                opcode.value = sprmPPc;
                break;
            case 30:
                opcode.value = sprmPBrcTop10;
                break;
            case 31:
                opcode.value = sprmPBrcLeft10;
                break;
            case 32:
                opcode.value = sprmPBrcBottom10;
                break;
            case 33:
                opcode.value = sprmPBrcRight10;
                break;
            case 34:
                opcode.value = sprmPBrcBetween10;
                break;
            case 35:
                opcode.value = sprmPBrcBar10;
                break;
            case 36:
                opcode.value = sprmPDxaFromText10;
                break;
            case 37:
                opcode.value = sprmPWr;
                break;
            case 38:
                opcode.value = sprmPBrcTop;
                break;
            case 39:
                opcode.value = sprmPBrcLeft;
                break;
            case 40:
                opcode.value = sprmPBrcBottom;
                break;
            case 41:
                opcode.value = sprmPBrcRight;
                break;
            case 42:
                opcode.value = sprmPBrcBetween;
                break;
            case 43:
                opcode.value = sprmPBrcBar;
                break;
            case 44:
                opcode.value = sprmPFNoAutoHyph;
                break;
            case 45:
                opcode.value = sprmPWHeightAbs;
                break;
            case 46:
                opcode.value = sprmPDcs;
                break;
            case 47:
                opcode.value = sprmPShd;
                break;
            case 48:
                opcode.value = sprmPDyaFromText;
                break;
            case 49:
                opcode.value = sprmPDxaFromText;
                break;
            case 50:
                opcode.value = sprmPFLocked;
                break;
            case 51:
                opcode.value = sprmPFWidowControl;
                break;
            case 52:
                opcode.value = sprmPRuler;
                break;
            case 65:
                opcode.value = sprmCFRMarkDel;
                break;
            case 66:
                opcode.value = sprmCFRMark;
                break;
            case 67:
                opcode.value = sprmCFFldVanish;
                break;
            case 68:
                opcode.value = sprmCPicLocation;
                break;
            case 69:
                opcode.value = sprmCIbstRMark;
                break;
            case 70:
                opcode.value = sprmCDttmRMark;
                break;
            case 71:
                opcode.value = sprmCFData;
                break;
            case 72:
                opcode.value = sprmCIdslRMark;
                break;
            case 73:
                opcode.value = sprmCChs;
                break;
            case 74:
                opcode.value = sprmCSymbol;
                break;
            case 75:
                opcode.value = sprmCFOle2;
                break;
            case 80:
                opcode.value = sprmCIstd;
                break;
            case 81:
                opcode.value = sprmCIstdPermute;
                break;
            case 82:
                opcode.value = sprmCDefault;
                break;
            case 83:
                opcode.value = sprmCPlain;
                break;
            case 85:
                opcode.value = sprmCFBold;
                break;
            case 86:
                opcode.value = sprmCFItalic;
                break;
            case 87:
                opcode.value = sprmCFStrike;
                break;
            case 88:
                opcode.value = sprmCFOutline;
                break;
            case 89:
                opcode.value = sprmCFShadow;
                break;
            case 90:
                opcode.value = sprmCFSmallCaps;
                break;
            case 91:
                opcode.value = sprmCFCaps;
                break;
            case 92:
                opcode.value = sprmCFVanish;
                break;
            case 93:
                opcode.value = sprmCRgFtc0;
                break;
            case 94:
                opcode.value = sprmCKul;
                break;
            case 95:
                opcode.value = sprmCSizePos;
                break;
            case 96:
                opcode.value = sprmCDxaSpace;
                break;
            case 97:
                opcode.value = sprmCLid;
                break;
            case 98:
                opcode.value = sprmCIco;
                break;
            case 99:
                opcode.value = sprmCHps;
                break;
            case 100:
                opcode.value = sprmCHpsInc;
                break;
            case 101:
                opcode.value = sprmCHpsPos;
                break;
            case 102:
                opcode.value = sprmCHpsPosAdj;
                break;
            case 103:
                opcode.value = sprmCMajority;
                break;
            case 104:
                opcode.value = sprmCIss;
                break;
            case 105:
                opcode.value = sprmCHpsNew50;
                break;
            case 106:
                opcode.value = sprmCHpsInc1;
                break;
            case 107:
                opcode.value = sprmCHpsKern;
                break;
            case 108:
                opcode.value = sprmCMajority50;
                break;
            case 109:
                opcode.value = sprmCHpsMul;
                break;
            case 110:
                opcode.value = sprmCYsri;
                break;
            case 117:
                opcode.value = sprmCFSpec;
                break;
            case 118:
                opcode.value = sprmCFObj;
                break;
            case 119:
                opcode.value = sprmPicBrcl;
                break;
            case 120:
                opcode.value = sprmPicScale;
                break;
            case 121:
                opcode.value = sprmPicBrcTop;
                break;
            case 122:
                opcode.value = sprmPicBrcLeft;
                break;
            case 123:
                opcode.value = sprmPicBrcBottom;
                break;
            case 124:
                opcode.value = sprmPicBrcRight;
                break;
            case 131:
                opcode.value = sprmScnsPgn;
                break;
            case 132:
                opcode.value = sprmSiHeadingPgn;
                break;
            case 133:
                opcode.value = sprmSOlstAnm;
                break;
            case 136:
                opcode.value = sprmSDxaColWidth;
                break;
            case 137:
                opcode.value = sprmSDxaColSpacing;
                break;
            case 138:
                opcode.value = sprmSFEvenlySpaced;
                break;
            case 139:
                opcode.value = sprmSFProtected;
                break;
            case 140:
                opcode.value = sprmSDmBinFirst;
                break;
            case 141:
                opcode.value = sprmSDmBinOther;
                break;
            case 142:
                opcode.value = sprmSBkc;
                break;
            case 143:
                opcode.value = sprmSFTitlePage;
                break;
            case 144:
                opcode.value = sprmSCcolumns;
                break;
            case 145:
                opcode.value = sprmSDxaColumns;
                break;
            case 146:
                opcode.value = sprmSFAutoPgn;
                break;
            case 147:
                opcode.value = sprmSNfcPgn;
                break;
            case 148:
                opcode.value = sprmSDyaPgn;
                break;
            case 149:
                opcode.value = sprmSDxaPgn;
                break;
            case 150:
                opcode.value = sprmSFPgnRestart;
                break;
            case 151:
                opcode.value = sprmSFEndnote;
                break;
            case 152:
                opcode.value = sprmSLnc;
                break;
            case 153:
                opcode.value = sprmSGprfIhdt;
                break;
            case 154:
                opcode.value = sprmSNLnnMod;
                break;
            case 155:
                opcode.value = sprmSDxaLnn;
                break;
            case 156:
                opcode.value = sprmSDyaHdrTop;
                break;
            case 157:
                opcode.value = sprmSDyaHdrBottom;
                break;
            case 158:
                opcode.value = sprmSLBetween;
                break;
            case 159:
                opcode.value = sprmSVjc;
                break;
            case 160:
                opcode.value = sprmSLnnMin;
                break;
            case 161:
                opcode.value = sprmSPgnStart;
                break;
            case 162:
                opcode.value = sprmSBOrientation;
                break;
            case 163:
                opcode.value = sprmSBCustomize;
                break;
            case 164:
                opcode.value = sprmSXaPage;
                break;
            case 165:
                opcode.value = sprmSYaPage;
                break;
            case 166:
                opcode.value = sprmSDxaLeft;
                break;
            case 167:
                opcode.value = sprmSDxaRight;
                break;
            case 168:
                opcode.value = sprmSDyaTop;
                break;
            case 169:
                opcode.value = sprmSDyaBottom;
                break;
            case 170:
                opcode.value = sprmSDzaGutter;
                break;
            case 171:
                opcode.value = sprmSDmPaperReq;
                break;
            case 182:
                opcode.value = sprmTJc;
                break;
            case 183:
                opcode.value = sprmTDxaLeft;
                break;
            case 184:
                opcode.value = sprmTDxaGapHalf;
                break;
            case 185:
                opcode.value = sprmTFCantSplit;
                break;
            case 186:
                opcode.value = sprmTTableHeader;
                break;
            case 187:
                opcode.value = sprmTTableBorders;
                break;
            case 188:
                opcode.value = sprmTDefTable10;
                break;
            case 189:
                opcode.value = sprmTDyaRowHeight;
                break;
            case 190:
                opcode.value = sprmTDefTable;
                break;
            case 191:
                opcode.value = sprmTDefTableShd;
                break;
            case 192:
                opcode.value = sprmTTlp;
                break;
            case 193:
                opcode.value = sprmTSetBrc;
                break;
            case 194:
                opcode.value = sprmTInsert;
                break;
            case 195:
                opcode.value = sprmTDelete;
                break;
            case 196:
                opcode.value = sprmTDxaCol;
                break;
            case 197:
                opcode.value = sprmTMerge;
                break;
            case 198:
                opcode.value = sprmTSplit;
                break;
            case 199:
                opcode.value = sprmTSetBrc10;
                break;
            case 200:
                opcode.value = sprmTSetShd;
                break;
            default:
                opcode.value = sprm;
            }
        }
        operandSize = operandSizes[opcode.bits.spra];
        if (!operandSize)
        {
            MsWord::U8 t8;
            MsWord::U16 t16;

            // Get length of variable size operand.

            switch (opcode.value)
            {
            case sprmPChgTabs:
                bytes += MsWordGenerated::read(in + bytes, &t8);
                operandSize = t8;
                if (operandSize == 255)
                    kdError(MsWord::s_area) << "Paragraph::apply: cannot parse sprmPChgTabs" << endl;
                break;
            case sprmTDefTable10:
            case sprmTDefTable:
                bytes += MsWordGenerated::read(in + bytes, &t16);
                operandSize = t16 - 1;
                break;
            default:
                bytes += MsWordGenerated::read(in + bytes, &t8);
                operandSize = t8;
                break;
            }
        }

        // Apply known opcodes.

        MsWord::U8 tmp;

        //kdDebug(s_area) << "Paragraph::apply: opcode:" << opcode.value << endl;
        switch (opcode.value)
        {
        case sprmPJc: // 0x2403
            MsWordGenerated::read(in + bytes, &m_pap.jc);
            break;
        case sprmPFSideBySide: // 0x2404
            MsWordGenerated::read(in + bytes, &tmp);
            m_pap.fSideBySide = tmp;
            break;
        case sprmPFKeep:
            MsWordGenerated::read(in + bytes, &tmp);
            m_pap.fKeep = tmp;
            break;
        case sprmPFKeepFollow:
            MsWordGenerated::read(in + bytes, &tmp);
            m_pap.fKeepFollow = tmp;
            break;
        case sprmPFPageBreakBefore: // 0x2407
            MsWordGenerated::read(in + bytes, &tmp);
            m_pap.fPageBreakBefore = tmp;
            break;
        case sprmPFInTable: // 0x2416
            MsWordGenerated::read(in + bytes, &tmp);
            m_pap.fInTable = tmp;
            break;
        case sprmPFTtp:
            MsWordGenerated::read(in + bytes, &tmp);
            m_pap.fTtp = tmp;
            break;
        case sprmPIlvl: // 0x260a
            MsWordGenerated::read(in + bytes, &m_pap.ilvl);
            break;
        case sprmPOutLvl: // 0x2640
            MsWordGenerated::read(in + bytes, &m_pap.lvl);
            break;
        case sprmPIstd: // 0x4600
            MsWordGenerated::read(in + bytes, &m_pap.istd);
            break;
        case sprmPIlfo: // 0x460B
            MsWordGenerated::read(in + bytes, &m_pap.ilfo);
            break;
        case sprmPBrcTop: // 0x6424
            MsWordGenerated::read(in + bytes, &m_pap.brcTop);
            break;
        case sprmPBrcLeft: // 0x6425
            MsWordGenerated::read(in + bytes, &m_pap.brcLeft);
            break;
        case sprmPBrcBottom: // 0x6426
            MsWordGenerated::read(in + bytes, &m_pap.brcBottom);
            break;
        case sprmPBrcRight: // 0x6427
            MsWordGenerated::read(in + bytes, &m_pap.brcRight);
            break;
        case sprmPDxaRight: // 0x840E
            MsWordGenerated::read(in + bytes, &m_pap.dxaRight);
            break;
        case sprmPDxaLeft: // 0x840F
            MsWordGenerated::read(in + bytes, &m_pap.dxaLeft);
            break;
        case sprmPDxaLeft1: // 0x8411
            MsWordGenerated::read(in + bytes, &m_pap.dxaLeft1);
            break;
        case sprmPDxaAbs: // 0x8418
            MsWordGenerated::read(in + bytes, &m_pap.dxaAbs);
            break;
        case sprmPDyaAbs: // 0x8419
            MsWordGenerated::read(in + bytes, &m_pap.dyaAbs);
            break;
        case sprmPDxaWidth: // 0x841A
            MsWordGenerated::read(in + bytes, &m_pap.dxaWidth);
            break;
        case sprmPDyaBefore: // 0xA413
            MsWordGenerated::read(in + bytes, &m_pap.dyaBefore);
            break;
        case sprmPDyaAfter: // 0xA414
            MsWordGenerated::read(in + bytes, &m_pap.dyaAfter);
            break;

        // TAP-specific stuff...

        case sprmTTableBorders:
            if (tap)
            {
                MsWordGenerated::read(in + bytes, &tap->rgbrcTable[0], 6);
            }
            break;
        case sprmTDefTable:
            if (tap)
            {
                // Get cell count.

                MsWordGenerated::read(in + bytes, &tmp);
                tap->itcMac = tmp;
                tmp = 1;

                // Get cell boundaries and descriptions.

                tmp += MsWordGenerated::read(in + bytes + tmp, (MsWord::U16 *)&tap->rgdxaCenter[0], tap->itcMac + 1);
                tmp += MsWordGenerated::read(in + bytes + tmp, &tap->rgtc[0], tap->itcMac);
            }
            else
            {
                kdError(MsWord::s_area) << "Paragraph::apply: cannot apply sprmTDefTable without a TAP" << endl;
            }
            break;
        default:
            if (!(m_document.m_fib.nFib > MsWord::s_maxWord6Version) &&
                (bytes == count) &&
                (opcode.value == 0))
            {
                // The last byte of a Word6 grpprl can be a zero padding byte.
            }
            else
            {
                kdWarning(MsWord::s_area) << "Paragraph::apply: unsupported opcode:" << opcode.value << endl;
            }
            break;
        }
        bytes += operandSize;
    }
}

// An existing base style.
void Paragraph::apply(MsWord::U16 style)
{
    unsigned originalStyle;

    // Save the style index.

    originalStyle = m_pap.istd;

    // Copy the given style, then restore the style index.

    m_pap = m_document.m_styles[style]->m_pap;
    m_pap.istd = originalStyle;
}

// List format.
void Paragraph::apply(MsWord::LFO &style, bool useFormatting, bool useStartAt)
{
    const MsWord::U8 *ptr = m_document.m_tableStream + m_document.m_fib.fcPlcfLst; //lcbPlcfLst.
    MsWord::U16 lstfCount;
    MsWord::LSTF data;
    int i;

    // Find the number of LSTFs.

    ptr += MsWordGenerated::read(ptr, &lstfCount);

    // Walk the LSTFs.

    for (i = 0; i < lstfCount; i++)
    {
        ptr += MsWordGenerated::read(ptr, &data);
        if (data.lsid == style.lsid)
        {
            // Record the style index.

            m_pap.istd = data.rgistd[m_pap.ilvl];

            // Build the base PAP if required.

            if (m_pap.istd != 4095)
                apply(m_pap.istd);

            MsWord::U8 *ptr2 = (MsWord::U8 *)m_document.m_listStyles[i][m_pap.ilvl];
            MsWord::LVLF level;
            MsWord::U16 numberTextLength;
            QString numberText;

            // Apply the LVLF.

            ptr2 += MsWordGenerated::read(ptr2, &level);
            m_pap.anld.nfc = level.nfc;
            m_pap.anld.jc = level.jc;
            if (useStartAt)
            {
                // Apply the startAt.

                m_pap.anld.iStartAt = level.iStartAt;
            }

            // Apply the variable length parts.

            if (useFormatting)
            {
                // Apply the grpprl.

                apply(ptr2, level.cbGrpprlPapx);
            }
            ptr2 += level.cbGrpprlPapx;
            ptr2 += level.cbGrpprlChpx;
            ptr2 += MsWordGenerated::read(ptr2, &numberTextLength);
            ptr2 += MsWord::read(ptr2, &numberText, numberTextLength, true);
            break;
        }
    }
    if (i == lstfCount)
        kdError(MsWord::s_area) << "Paragraph::apply: error finding LSTF[" << style.lsid << "]" << endl;
}

// Property exceptions.

void Paragraph::apply(MsWord::PAPXFKP &style)
{
    // Record the style index.

    m_pap.istd = style.istd;

    // Build the base PAP then walk the grpprl.

    apply(style.istd);
    apply(style.grpprl, style.grpprlBytes);
}

// Paragraph height.

void Paragraph::apply(MsWord::PHE &layout)
{
    m_pap.phe = layout;
}

// Predefined style from stylesheet.

void Paragraph::apply(MsWord::STD &style)
{
    if (style.sgc != 1)
    {
        kdError(MsWord::s_area) << "Paragraph::apply: not a paragraph style: " << style.sgc << endl;
        return;
    }

    const MsWord::U8 *grpprl;
    MsWord::U16 cbUpx;

    // Align to an even-byte position.

    grpprl = style.grupx;
    if ((int)grpprl & 1)
        grpprl++;
    grpprl += MsWordGenerated::read(grpprl, &cbUpx);

    // Record the style index.

    grpprl += MsWordGenerated::read(grpprl, &m_pap.istd);

    // Build the base PAP then walk the grpprl.

    apply(m_pap.istd);
    apply(grpprl, cbUpx - 2);
}
