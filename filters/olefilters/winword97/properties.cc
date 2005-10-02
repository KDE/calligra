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

#include <properties.h>

#define CHP_FLAG(flag) \
case sprmCF##flag: \
    MsWordGenerated::read(in + bytes, &tmp); \
    m_chp.f##flag = tmp == 1; \
    break

#define PAP_FLAG(flag) \
case sprmPF##flag: \
    MsWordGenerated::read(in + bytes, &tmp); \
    m_pap.f##flag = tmp == 1; \
    break

#define TAP_FLAG(flag) \
case sprmTF##flag: \
    MsWordGenerated::read(in + bytes, &tmp); \
    m_tap.f##flag = tmp == 1; \
    break

// Create a paragraph with default properties.
Properties::Properties(MsWord &document) :
    m_document(document)
{
    memset(&m_pap, 0, sizeof(m_pap));
    m_pap.fWidowControl = 1;
    m_pap.lspd.fMultLinespace = 1;
    m_pap.lspd.dyaLine = 240;
    m_pap.lvl = 9;
    memset(&m_chp, 0, sizeof(m_chp));
    m_chp.hps = 20;
    m_chp.fcPic_fcObj_lTagObj = (MsWord::U32)-1;
    m_chp.istd = 10;
    m_chp.lidDefault = 0x400;
    m_chp.lidFE = 0x400;
    m_chp.wCharScale = 100;
    m_chp.fUsePgsuSettings = (MsWord::U16)-1;
    memset(&m_tap, 0, sizeof(m_tap));
}

Properties::Properties(const Properties &original) :
    m_document(original.m_document)
{
    m_pap = original.m_pap;
    m_chp = original.m_chp;
    m_tap = original.m_tap;
}

Properties::~Properties()
{
}

// An array of SPRMs (grpprl).
void Properties::apply(const MsWord::U8 *grpprl, unsigned count)
{
    MsWord::U16 opcodeValue;
    struct
    {
        MsWord::U16 ispmd: 9;
        MsWord::U16 fSpec: 1;
        MsWord::U16 sgc: 3;
        MsWord::U16 spra: 3;
    } opcodeBits;
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
            bytes += MsWordGenerated::read(in + bytes, &opcodeValue);
        }
        else
        {
            MsWord::U8 sprm;

            // Convert sprm to new format.

            bytes += MsWordGenerated::read(in + bytes, &sprm);
            switch (sprm)
            {
            case 0:
                opcodeValue = sprmNoop;
                break;
            case 2:
                opcodeValue = sprmPIstd;
                break;
            case 3:
                opcodeValue = sprmPIstdPermute;
                break;
            case 4:
                opcodeValue = sprmPIncLvl;
                break;
            case 5:
                opcodeValue = sprmPJc;
                break;
            case 6:
                opcodeValue = sprmPFSideBySide;
                break;
            case 7:
                opcodeValue = sprmPFKeep;
                break;
            case 8:
                opcodeValue = sprmPFKeepFollow;
                break;
            case 9:
                opcodeValue = sprmPFPageBreakBefore;
                break;
            case 10:
                opcodeValue = sprmPBrcl;
                break;
            case 11:
                opcodeValue = sprmPBrcp;
                break;
            case 12:
                opcodeValue = sprmPAnld;
                break;
            case 13:
                opcodeValue = sprmPIlvl;
                break;
            case 14:
                opcodeValue = sprmPFNoLineNumb;
                break;
            case 15:
                opcodeValue = sprmPChgTabsPapx;
                break;
            case 16:
                opcodeValue = sprmPDxaRight;
                break;
            case 17:
                opcodeValue = sprmPDxaLeft;
                break;
            case 18:
                opcodeValue = sprmPNest;
                break;
            case 19:
                opcodeValue = sprmPDxaLeft1;
                break;
            case 20:
                opcodeValue = sprmPDyaLine;
                break;
            case 21:
                opcodeValue = sprmPDyaBefore;
                break;
            case 22:
                opcodeValue = sprmPDyaAfter;
                break;
            case 23:
                opcodeValue = sprmPChgTabs;
                break;
            case 24:
                opcodeValue = sprmPFInTable;
                break;
            case 25:
                opcodeValue = sprmPFTtp;
                break;
            case 26:
                opcodeValue = sprmPDxaAbs;
                break;
            case 27:
                opcodeValue = sprmPDyaAbs;
                break;
            case 28:
                opcodeValue = sprmPDxaWidth;
                break;
            case 29:
                opcodeValue = sprmPPc;
                break;
            case 30:
                opcodeValue = sprmPBrcTop10;
                break;
            case 31:
                opcodeValue = sprmPBrcLeft10;
                break;
            case 32:
                opcodeValue = sprmPBrcBottom10;
                break;
            case 33:
                opcodeValue = sprmPBrcRight10;
                break;
            case 34:
                opcodeValue = sprmPBrcBetween10;
                break;
            case 35:
                opcodeValue = sprmPBrcBar10;
                break;
            case 36:
                opcodeValue = sprmPDxaFromText10;
                break;
            case 37:
                opcodeValue = sprmPWr;
                break;
            case 38:
                opcodeValue = sprmPBrcTop;
                break;
            case 39:
                opcodeValue = sprmPBrcLeft;
                break;
            case 40:
                opcodeValue = sprmPBrcBottom;
                break;
            case 41:
                opcodeValue = sprmPBrcRight;
                break;
            case 42:
                opcodeValue = sprmPBrcBetween;
                break;
            case 43:
                opcodeValue = sprmPBrcBar;
                break;
            case 44:
                opcodeValue = sprmPFNoAutoHyph;
                break;
            case 45:
                opcodeValue = sprmPWHeightAbs;
                break;
            case 46:
                opcodeValue = sprmPDcs;
                break;
            case 47:
                opcodeValue = sprmPShd;
                break;
            case 48:
                opcodeValue = sprmPDyaFromText;
                break;
            case 49:
                opcodeValue = sprmPDxaFromText;
                break;
            case 50:
                opcodeValue = sprmPFLocked;
                break;
            case 51:
                opcodeValue = sprmPFWidowControl;
                break;
            case 52:
                opcodeValue = sprmPRuler;
                break;
            case 65:
                opcodeValue = sprmCFRMarkDel;
                break;
            case 66:
                opcodeValue = sprmCFRMark;
                break;
            case 67:
                opcodeValue = sprmCFFldVanish;
                break;
            case 68:
                opcodeValue = sprmCPicLocation;
                break;
            case 69:
                opcodeValue = sprmCIbstRMark;
                break;
            case 70:
                opcodeValue = sprmCDttmRMark;
                break;
            case 71:
                opcodeValue = sprmCFData;
                break;
            case 72:
                opcodeValue = sprmCIdslRMark;
                break;
            case 73:
                opcodeValue = sprmCChs;
                break;
            case 74:
                opcodeValue = sprmCSymbol;
                break;
            case 75:
                opcodeValue = sprmCFOle2;
                break;
            case 80:
                opcodeValue = sprmCIstd;
                break;
            case 81:
                opcodeValue = sprmCIstdPermute;
                break;
            case 82:
                opcodeValue = sprmCDefault;
                break;
            case 83:
                opcodeValue = sprmCPlain;
                break;
            case 85:
                opcodeValue = sprmCFBold;
                break;
            case 86:
                opcodeValue = sprmCFItalic;
                break;
            case 87:
                opcodeValue = sprmCFStrike;
                break;
            case 88:
                opcodeValue = sprmCFOutline;
                break;
            case 89:
                opcodeValue = sprmCFShadow;
                break;
            case 90:
                opcodeValue = sprmCFSmallCaps;
                break;
            case 91:
                opcodeValue = sprmCFCaps;
                break;
            case 92:
                opcodeValue = sprmCFVanish;
                break;
            case 93:
                opcodeValue = sprmCRgFtc0;
                break;
            case 94:
                opcodeValue = sprmCKul;
                break;
            case 95:
                opcodeValue = sprmCSizePos;
                break;
            case 96:
                opcodeValue = sprmCDxaSpace;
                break;
            case 97:
                opcodeValue = sprmCLid;
                break;
            case 98:
                opcodeValue = sprmCIco;
                break;
            case 99:
                opcodeValue = sprmCHps;
                break;
            case 100:
                opcodeValue = sprmCHpsInc;
                break;
            case 101:
                opcodeValue = sprmCHpsPos;
                break;
            case 102:
                opcodeValue = sprmCHpsPosAdj;
                break;
            case 103:
                opcodeValue = sprmCMajority;
                break;
            case 104:
                opcodeValue = sprmCIss;
                break;
            case 105:
                opcodeValue = sprmCHpsNew50;
                break;
            case 106:
                opcodeValue = sprmCHpsInc1;
                break;
            case 107:
                opcodeValue = sprmCHpsKern;
                break;
            case 108:
                opcodeValue = sprmCMajority50;
                break;
            case 109:
                opcodeValue = sprmCHpsMul;
                break;
            case 110:
                opcodeValue = sprmCYsri;
                break;
            case 117:
                opcodeValue = sprmCFSpec;
                break;
            case 118:
                opcodeValue = sprmCFObj;
                break;
            case 119:
                opcodeValue = sprmPicBrcl;
                break;
            case 120:
                opcodeValue = sprmPicScale;
                break;
            case 121:
                opcodeValue = sprmPicBrcTop;
                break;
            case 122:
                opcodeValue = sprmPicBrcLeft;
                break;
            case 123:
                opcodeValue = sprmPicBrcBottom;
                break;
            case 124:
                opcodeValue = sprmPicBrcRight;
                break;
            case 131:
                opcodeValue = sprmScnsPgn;
                break;
            case 132:
                opcodeValue = sprmSiHeadingPgn;
                break;
            case 133:
                opcodeValue = sprmSOlstAnm;
                break;
            case 136:
                opcodeValue = sprmSDxaColWidth;
                break;
            case 137:
                opcodeValue = sprmSDxaColSpacing;
                break;
            case 138:
                opcodeValue = sprmSFEvenlySpaced;
                break;
            case 139:
                opcodeValue = sprmSFProtected;
                break;
            case 140:
                opcodeValue = sprmSDmBinFirst;
                break;
            case 141:
                opcodeValue = sprmSDmBinOther;
                break;
            case 142:
                opcodeValue = sprmSBkc;
                break;
            case 143:
                opcodeValue = sprmSFTitlePage;
                break;
            case 144:
                opcodeValue = sprmSCcolumns;
                break;
            case 145:
                opcodeValue = sprmSDxaColumns;
                break;
            case 146:
                opcodeValue = sprmSFAutoPgn;
                break;
            case 147:
                opcodeValue = sprmSNfcPgn;
                break;
            case 148:
                opcodeValue = sprmSDyaPgn;
                break;
            case 149:
                opcodeValue = sprmSDxaPgn;
                break;
            case 150:
                opcodeValue = sprmSFPgnRestart;
                break;
            case 151:
                opcodeValue = sprmSFEndnote;
                break;
            case 152:
                opcodeValue = sprmSLnc;
                break;
            case 153:
                opcodeValue = sprmSGprfIhdt;
                break;
            case 154:
                opcodeValue = sprmSNLnnMod;
                break;
            case 155:
                opcodeValue = sprmSDxaLnn;
                break;
            case 156:
                opcodeValue = sprmSDyaHdrTop;
                break;
            case 157:
                opcodeValue = sprmSDyaHdrBottom;
                break;
            case 158:
                opcodeValue = sprmSLBetween;
                break;
            case 159:
                opcodeValue = sprmSVjc;
                break;
            case 160:
                opcodeValue = sprmSLnnMin;
                break;
            case 161:
                opcodeValue = sprmSPgnStart;
                break;
            case 162:
                opcodeValue = sprmSBOrientation;
                break;
            case 163:
                opcodeValue = sprmSBCustomize;
                break;
            case 164:
                opcodeValue = sprmSXaPage;
                break;
            case 165:
                opcodeValue = sprmSYaPage;
                break;
            case 166:
                opcodeValue = sprmSDxaLeft;
                break;
            case 167:
                opcodeValue = sprmSDxaRight;
                break;
            case 168:
                opcodeValue = sprmSDyaTop;
                break;
            case 169:
                opcodeValue = sprmSDyaBottom;
                break;
            case 170:
                opcodeValue = sprmSDzaGutter;
                break;
            case 171:
                opcodeValue = sprmSDmPaperReq;
                break;
            case 182:
                opcodeValue = sprmTJc;
                break;
            case 183:
                opcodeValue = sprmTDxaLeft;
                break;
            case 184:
                opcodeValue = sprmTDxaGapHalf;
                break;
            case 185:
                opcodeValue = sprmTFCantSplit;
                break;
            case 186:
                opcodeValue = sprmTTableHeader;
                break;
            case 187:
                opcodeValue = sprmTTableBorders;
                break;
            case 188:
                opcodeValue = sprmTDefTable10;
                break;
            case 189:
                opcodeValue = sprmTDyaRowHeight;
                break;
            case 190:
                opcodeValue = sprmTDefTable;
                break;
            case 191:
                opcodeValue = sprmTDefTableShd;
                break;
            case 192:
                opcodeValue = sprmTTlp;
                break;
            case 193:
                opcodeValue = sprmTSetBrc;
                break;
            case 194:
                opcodeValue = sprmTInsert;
                break;
            case 195:
                opcodeValue = sprmTDelete;
                break;
            case 196:
                opcodeValue = sprmTDxaCol;
                break;
            case 197:
                opcodeValue = sprmTMerge;
                break;
            case 198:
                opcodeValue = sprmTSplit;
                break;
            case 199:
                opcodeValue = sprmTSetBrc10;
                break;
            case 200:
                opcodeValue = sprmTSetShd;
                break;
            default:
                opcodeValue = sprmNoop;
                break;
            }
        }

        // Convert the opcode into its bitfield equivalent in an endian-safe
        // manner.

        opcodeBits.ispmd = opcodeValue >> 0;
        opcodeBits.fSpec = opcodeValue >> 9;
        opcodeBits.sgc = opcodeValue >> 10;
        opcodeBits.spra = opcodeValue >> 13;
        operandSize = operandSizes[opcodeBits.spra];
        if (!operandSize)
        {
            MsWord::U8 t8;
            MsWord::U16 t16;

            // Get length of variable size operand.

            switch (opcodeValue)
            {
            case sprmPChgTabs:
                bytes += MsWordGenerated::read(in + bytes, &t8);
                operandSize = t8;
                if (operandSize == 255)
                    kdError(MsWord::s_area) << "Properties::apply: cannot parse sprmPChgTabs" << endl;
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
        int i;

        //kdDebug(s_area) << "Properties::apply: opcode:" << opcodeValue << endl;
        switch (opcodeValue)
        {
        case sprmNoop: // 0x0000
            break;
        CHP_FLAG(RMarkDel); // 0x0800
        CHP_FLAG(RMark); // 0x0801
        CHP_FLAG(FldVanish); // 0x0802
        CHP_FLAG(Data); // 0x0806
        CHP_FLAG(Ole2); // 0x080A
        case sprmCFBold: // 0x0835
            MsWordGenerated::read(in + bytes, &tmp);
            // TBD: implement access to base chp for >= 128 case!!!
	    if (tmp < 128)
	        m_chp.fBold = tmp == 1;
	    else
                m_chp.fBold = tmp == 128 ? m_chp.fBold : !m_chp.fBold;
	    break;
        case sprmCFItalic: // 0x0836
            MsWordGenerated::read(in + bytes, &tmp);
            // TBD: implement access to base chp for >= 128 case!!!
            if (tmp < 128)
                m_chp.fItalic = tmp == 1;
            else
                m_chp.fItalic = tmp == 128 ? m_chp.fItalic : !m_chp.fItalic;
            break;
        case sprmCFStrike: // 0x0837
            MsWordGenerated::read(in + bytes, &tmp);
            // TBD: implement access to base chp for >= 128 case!!!
            if (tmp < 128)
                m_chp.fStrike = tmp == 1;
            else
                m_chp.fStrike = tmp == 128 ? m_chp.fStrike : !m_chp.fStrike;
            break;
        case sprmCFOutline: // 0x0838
            MsWordGenerated::read(in + bytes, &tmp);
            // TBD: implement access to base chp for >= 128 case!!!
            if (tmp < 128)
                m_chp.fOutline = tmp == 1;
            else
                m_chp.fOutline = tmp == 128 ? m_chp.fOutline : !m_chp.fOutline;
            break;
        case sprmCFShadow: // 0x0839
            MsWordGenerated::read(in + bytes, &tmp);
            // TBD: implement access to base chp for >= 128 case!!!
            if (tmp < 128)
                m_chp.fShadow = tmp == 1;
            else
                m_chp.fShadow = tmp == 128 ? m_chp.fShadow : !m_chp.fShadow;
            break;
        case sprmCFSmallCaps: // 0x083A
            MsWordGenerated::read(in + bytes, &tmp);
            // TBD: implement access to base chp for >= 128 case!!!
            if (tmp < 128)
                m_chp.fSmallCaps = tmp == 1;
            else
                m_chp.fSmallCaps = tmp == 128 ? m_chp.fSmallCaps : !m_chp.fSmallCaps;
            break;
        case sprmCFCaps: // 0x083B
            MsWordGenerated::read(in + bytes, &tmp);
            // TBD: implement access to base chp for >= 128 case!!!
            if (tmp < 128)
                m_chp.fCaps = tmp == 1;
            else
                m_chp.fCaps = tmp == 128 ? m_chp.fCaps : !m_chp.fCaps;
            break;
        case sprmCFVanish: // 0x083C
            MsWordGenerated::read(in + bytes, &tmp);
            // TBD: implement access to base chp for >= 128 case!!!
            if (tmp < 128)
                m_chp.fVanish = tmp == 1;
            else
                m_chp.fVanish = tmp == 128 ? m_chp.fVanish : !m_chp.fVanish;
            break;
        CHP_FLAG(Imprint); // 0x0854
        CHP_FLAG(Spec); // 0x0855
        CHP_FLAG(Obj); // 0x0856
        CHP_FLAG(UsePgsuSettings); // 0x0868
        case sprmPJc: // 0x2403
            MsWordGenerated::read(in + bytes, &m_pap.jc);
            break;
        PAP_FLAG(SideBySide); // 0x2404
        PAP_FLAG(Keep);
        PAP_FLAG(KeepFollow);
        PAP_FLAG(PageBreakBefore); // 0x2407
        PAP_FLAG(InTable); // 0x2416
        PAP_FLAG(Ttp); // 0x2417
        case sprmPWr: // 0x2423
            MsWordGenerated::read(in + bytes, &m_pap.wr);
            break;
        PAP_FLAG(NoAutoHyph); // 0x242A
        PAP_FLAG(Locked); // 0x2430
        PAP_FLAG(WidowControl); // 0x2431
        PAP_FLAG(Kinsoku); // 0x2433
        PAP_FLAG(WordWrap); // 0x2434
        PAP_FLAG(OverflowPunct); // 0x2435
        PAP_FLAG(TopLinePunct); // 0x2436
        PAP_FLAG(AutoSpaceDE); // 0x2437
        PAP_FLAG(AutoSpaceDN); // 0x2438
        case sprmPIlvl: // 0x260a
            MsWordGenerated::read(in + bytes, &m_pap.ilvl);
            break;
        case sprmPPc: // 0x261B
            // TBD: NYI
            break;
        case sprmPOutLvl: // 0x2640
            MsWordGenerated::read(in + bytes, &m_pap.lvl);
            break;
        case sprmCKul: // 0x2A3E
            MsWordGenerated::read(in + bytes, &tmp);
            m_chp.kul = tmp;
            break;
        case sprmCIco: // 0x2A42
            MsWordGenerated::read(in + bytes, &tmp);
            m_chp.ico = tmp;
            break;
        case sprmCIss: // 0x2A48
            MsWordGenerated::read(in + bytes, &tmp);
            m_chp.iss = tmp;
            break;
        TAP_FLAG(CantSplit); // 0x3403
        case sprmTTableHeader: // 0x3404
            MsWordGenerated::read(in + bytes, &tmp);
            m_tap.fTableHeader = tmp == 1;
            break;
        case sprmPWHeightAbs: // 0x442B
            // TBD: NYI
            break;
        case sprmPIstd: // 0x4600
            MsWordGenerated::read(in + bytes, &m_pap.istd);
            break;
        case sprmPIlfo: // 0x460B
            MsWordGenerated::read(in + bytes, &m_pap.ilfo);
            break;
        case sprmCHpsKern: // 0x484B
            MsWordGenerated::read(in + bytes, &m_chp.hpsKern);
            break;
        case sprmCYsri: // 0x484E
            MsWordGenerated::read(in + bytes, &m_chp.ysr);
            break;
        case sprmCRgLid0: // 0x486D
            MsWordGenerated::read(in + bytes, &m_chp.lidDefault);
            break;
        case sprmCRgLid1: // 0x486E
            MsWordGenerated::read(in + bytes, &m_chp.lidFE);
            break;
        case sprmCIstd: // 0x4A30
            MsWordGenerated::read(in + bytes, &m_chp.istd);
            break;
        case sprmCLid: // 0x4A41
            // TBD: "only used internally never stored".
            MsWordGenerated::read(in + bytes, &m_chp.lid);
            break;
        case sprmCHps: // 0x4A43
            MsWordGenerated::read(in + bytes, &m_chp.hps);
            break;
        case sprmCRgFtc0: // 0x4A4F
            MsWordGenerated::read(in + bytes, &m_chp.ftcAscii);
            break;
        case sprmCRgFtc1: // 0x4A50
            MsWordGenerated::read(in + bytes, &m_chp.ftcFE);
            break;
        case sprmCRgFtc2: // 0x4A51
            MsWordGenerated::read(in + bytes, &m_chp.ftcOther);
            break;
        case sprmPDyaLine: // 0x6412
            // TBD: NYI
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
        case sprmPBrcBetween: // 0x6628
            MsWordGenerated::read(in + bytes, &m_pap.brcBetween);
            break;
        case sprmPBrcBar: // 0x6629
            MsWordGenerated::read(in + bytes, &m_pap.brcBar);
            break;
        case sprmCObjLocation: // 0x680E
            MsWordGenerated::read(in + bytes, &m_chp.fcPic_fcObj_lTagObj);
            break;
        case sprmCPicLocation: // 0x6A03
            MsWordGenerated::read(in + bytes, &m_chp.fcPic_fcObj_lTagObj);
            m_chp.fSpec = 1;
            break;
        case sprmCSymbol: // 0x6A09
            MsWordGenerated::read(in + bytes, &m_chp.ftcSym);
            MsWordGenerated::read(in + bytes + 2, &m_chp.xchSym);
            m_chp.fSpec = 1;
            break;
        case sprmTTlp: // 0x740A
            MsWordGenerated::read(in + bytes, &m_tap.tlp);
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
        case sprmTDyaRowHeight: // 0x9407
            MsWordGenerated::read(in + bytes, &m_tap.dyaRowHeight);
            break;
        case sprmTDxaGapHalf: // 0x9602
            // TBD: NYI
            break;
        case sprmPDyaBefore: // 0xA413
            MsWordGenerated::read(in + bytes, &m_pap.dyaBefore);
            break;
        case sprmPDyaAfter: // 0xA414
            MsWordGenerated::read(in + bytes, &m_pap.dyaAfter);
            break;
        case sprmPChgTabsPapx: // 0xC60D
            // TBD: NYI
            break;
        case sprmPChgTabs: // 0xC615
            // TBD: NYI
            break;
        case sprmPAnld: // 0xC63E
            MsWordGenerated::read(in + bytes, &m_pap.anld);
            break;
        case sprmTTableBorders: // 0xD605
            for (i = 0; i < 6; i++)
                MsWordGenerated::read(in + bytes, &m_tap.rgbrcTable[i]);
            break;
        case sprmTDefTable: // 0xD608

            // Get cell count.

            MsWordGenerated::read(in + bytes, &tmp);
            m_tap.itcMac = tmp;
            tmp = 1;

            // Get cell boundaries and descriptions.

            for (i = 0; i < m_tap.itcMac + 1; i++)
                tmp += MsWordGenerated::read(in + bytes + tmp, &m_tap.rgdxaCenter[i]);
            for (i = 0; i < m_tap.itcMac; i++)
                tmp += MsWordGenerated::read(in + bytes + tmp, &m_tap.rgtc[i]);
            break;
        case sprmTDefTableShd: // 0xD609

            // TBD: this is completely different to the documented algorithm!

            tmp = operandSize/MsWordGenerated::sizeof_SHD;
            for (i = 0; i < tmp; i++)
                MsWordGenerated::read(in + bytes, &m_tap.rgshd[i]);
            break;
        default:
            if (!(m_document.m_fib.nFib > MsWord::s_maxWord6Version) &&
                (bytes == count) &&
                (opcodeValue == sprmNoop))
            {
                // The last byte of a Word6 grpprl can be a zero padding byte.
            }
            else
            {
                kdWarning(MsWord::s_area) << "Properties::apply: unsupported opcode: 0x" <<
                    QString::number(opcodeValue, 16) << endl;
            }
            break;
        }
        bytes += operandSize;
    }
}

// An existing base style.
void Properties::apply(MsWord::U16 style)
{
    unsigned originalStyle;

    // Save the style index.
    originalStyle = m_pap.istd;

    // Copy the given style, then restore the style index.

    // If the level has an unexpected style, default to something sane.
    if (style >= m_document.m_styles.count)
    {
        kdError(MsWord::s_area) << "out of range style: " << style << " last style: " <<
                m_document.m_styles.count << endl;
        style = 0;
    }
    m_pap = m_document.m_styles.data[style]->m_pap;
    m_chp = m_document.m_styles.data[style]->m_chp;
    m_tap = m_document.m_styles.data[style]->m_tap;
    m_pap.istd = originalStyle;
}

// List format.
void Properties::apply(const MsWord::LFO &style)
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

            // Build the base PAP. A list with a base style of 4095 has
            // a null style, default to something sane.

            if (m_pap.istd == 4095)
                m_pap.istd = 0;
            apply(m_pap.istd);

            MsWord::U8 *ptr2 = (MsWord::U8 *)m_document.m_listStyles[i][m_pap.ilvl];
            MsWord::LVLF level;
            MsWord::U16 numberTextLength;
            QString numberText;

            // Apply the LVLF.

            ptr2 += MsWordGenerated::read(ptr2, &level);
            m_pap.anld.nfc = level.nfc;
            m_pap.anld.jc = level.jc;
            m_pap.anld.iStartAt = level.iStartAt;
            kdDebug(MsWord::s_area) << " startAt from list style:" <<
                m_pap.anld.iStartAt << endl;
#if 0
            kdDebug(MsWord::s_area) << " nfc=" << level.nfc << " rgbxchNums[0]=" << level.rgbxchNums[0] << " ixchFollow=" << level.ixchFollow << endl;
            kdDebug(MsWord::s_area) << " maybe the anld has it? rgxch=" << m_pap.anld.rgxch << endl;
            for ( unsigned i = 0 ; i < 9 && m_pap.anld.rgxch[i] ; ++i )
                kdDebug() << i << ": " << m_pap.anld.rgxch[i] << " " << QString(QChar(m_pap.anld.rgxch[i])) << endl;
#endif

            // Apply the variable length parts.

            apply(ptr2, level.cbGrpprlPapx);
            ptr2 += level.cbGrpprlPapx;
            ptr2 += level.cbGrpprlChpx;
            ptr2 += MsWordGenerated::read(ptr2, &numberTextLength);

            MsWordGenerated::XCHAR ch;
            unsigned int outIndex = 0;
            bool found = false;
            // According to the description of the rgbxchNums field,
            // the text from the LVL is of the form: <prefix><place_holder><suffix>
            // where the place holder is a level number, i.e. a char <= 9.
            // No idea what to do with that char, but simply copying this information
            // into the ANLD's rgxch, cxchTextBefore and cxchTextAfter, and skipping
            // the place holder, allows a smooth import of '.' after a numberred list.
            // (David)
            for ( unsigned int i = 0 ; i < numberTextLength ; ++i, ++outIndex ) {
                ptr2 += MsWordGenerated::read(ptr2, &ch);
                m_pap.anld.rgxch[outIndex] = ch;
                if ( ch < 10 ) { /// Found the place holder
                    Q_ASSERT(!found);
                    found = true;
                    m_pap.anld.cxchTextBefore = i; // it's a length, so the last char is i-1 if i>0
                    m_pap.anld.cxchTextAfter = numberTextLength - 1;
                    //kdDebug(s_area) << "Properties::apply found placeholder " << ch << " at position " << i << " cxchTextBefore=" << m_pap.anld.cxchTextBefore << " cxchTextAfter=" << m_pap.anld.cxchTextAfter << endl;
                    outIndex--; // We want to remove the place holder - this makes the next iteration overwrite it
                }
            } //for
#if 0 // The old code, which was ignoring numberText completely.
            ptr2 += MsWord::read(m_document.m_fib.lid, ptr2, &numberText, numberTextLength, true, m_document.m_fib.nFib);
            kdDebug() << "Properties::apply numberText=" << numberText << " numberTextLength=" << numberTextLength << endl;
            for ( unsigned i = 0 ; i < numberTextLength ; ++i )
                kdDebug() << i << ": " << numberText[i].unicode() << endl;
#endif
            break;
        }
    }
    if (i == lstfCount)
        kdError(MsWord::s_area) << "Properties::apply: error finding LSTF[" << style.lsid << "]" << endl;
}

// Paragraph property set.

void Properties::apply(const MsWord::PAP &style)
{
    // First apply the paragraph, character and other properties of the
    // paragraph's base style, and then apply the paragraph properties.
    apply(style.istd);
    m_pap = style;

    // Record the style index.
    m_pap.istd = style.istd;
}

// Paragraph property exceptions.

void Properties::apply(const MsWord::PAPXFKP &style)
{
    // First apply the paragraph, character and other properties of the
    // paragraph's base style, and then apply the exception properties.
    apply(style.istd);
    apply(style.ptr, style.count);

    // Record the style index.
    m_pap.istd = style.istd;
}

// Properties height.

void Properties::apply(const MsWord::PHE &layout)
{
    m_pap.phe = layout;
}

// Predefined style from stylesheet.

void Properties::apply(const MsWord::STD &style)
{
    const MsWord::U8 *grpprl;
    MsWord::U16 cbUpx;
    unsigned cupx = style.cupx;

    grpprl = style.grupx;
    if (style.sgc == 1)
    {
        // Align to an even-byte offset *within* the STD. The alignment
        // of the STD was stored in fScratch.

        if (((long)grpprl & 1) ^ style.fScratch)
            grpprl++;
        grpprl += MsWordGenerated::read(grpprl, &cbUpx);
        if (cbUpx)
        {
            // Record the style index.

            grpprl += MsWordGenerated::read(grpprl, &m_pap.istd);

            // Build the base PAP then walk the grpprl.

            apply(m_pap.istd);
            apply(grpprl, cbUpx - 2);
            grpprl += cbUpx - 2;
        }
        cupx--;
    }
    if ((style.sgc == 1) ||
        (style.sgc == 2))
    {
        // Align to an even-byte offset *within* the STD. The alignment
        // of the STD was stored in fScratch.

        if (((long)grpprl & 1) ^ style.fScratch)
            grpprl++;
        grpprl += MsWordGenerated::read(grpprl, &cbUpx);
        if (cbUpx)
        {
            // Apply the grpprl to the base CHP.

            apply(grpprl, cbUpx);
            grpprl += cbUpx;
        }
        cupx--;
    }

    // If things went unexpectedly wrong...

    if (cupx != 0)
        kdError(MsWord::s_area) << "Properties::apply: unexpected cupx: " << style.cupx << endl;
}

// Conversion from compact PRM opcode to real opcode.

MsWord::U16 Properties::getRealOpcode(unsigned shortOpcode, const MsWord::FIB &fib)
{
    static const MsWord::U16 rgsprmPrmWord6[0x80] =
    {
        // 0
        Properties::sprmNoop, Properties::sprmNoop, Properties::sprmPIstd, Properties::sprmPIstdPermute, Properties::sprmPIncLvl, Properties::sprmPJc, Properties::sprmPFSideBySide, Properties::sprmPFKeep,
        Properties::sprmPFKeepFollow, Properties::sprmPFPageBreakBefore, Properties::sprmPBrcl, Properties::sprmPBrcp, Properties::sprmPAnld, Properties::sprmPIlvl, Properties::sprmPFNoLineNumb, Properties::sprmPChgTabsPapx,
        //16
        Properties::sprmPDxaRight, Properties::sprmPDxaLeft, Properties::sprmPNest, Properties::sprmPDxaLeft1, Properties::sprmPDyaLine, Properties::sprmPDyaBefore, Properties::sprmPDyaAfter, Properties::sprmPChgTabs,
        Properties::sprmPFInTable, Properties::sprmPFTtp, Properties::sprmPDxaAbs, Properties::sprmPDyaAbs, Properties::sprmPDxaWidth, Properties::sprmPPc, Properties::sprmPBrcTop10, Properties::sprmPBrcLeft10,
        // 32
        Properties::sprmPBrcBottom10, Properties::sprmPBrcRight10, Properties::sprmPBrcBetween10, Properties::sprmPBrcBar10, Properties::sprmPDxaFromText10, Properties::sprmPWr, Properties::sprmPBrcTop, Properties::sprmPBrcLeft,
        Properties::sprmPBrcBottom, Properties::sprmPBrcRight, Properties::sprmPBrcBetween, Properties::sprmPBrcBar, Properties::sprmPFNoAutoHyph, Properties::sprmPWHeightAbs, Properties::sprmPDcs, Properties::sprmPShd,
        // 48
        Properties::sprmPDyaFromText, Properties::sprmPDxaFromText, Properties::sprmPFLocked, Properties::sprmPFWidowControl, Properties::sprmPRuler, Properties::sprmNoop, Properties::sprmNoop, Properties::sprmNoop,
        Properties::sprmNoop, Properties::sprmNoop, Properties::sprmNoop, Properties::sprmNoop, Properties::sprmNoop, Properties::sprmNoop, Properties::sprmNoop, Properties::sprmNoop,
        // 64
        Properties::sprmNoop, Properties::sprmCFRMarkDel, Properties::sprmCFRMark, Properties::sprmCFFldVanish, Properties::sprmCPicLocation, Properties::sprmCIbstRMark, Properties::sprmCDttmRMark, Properties::sprmCFData,
        Properties::sprmCIdslRMark, Properties::sprmCChs, Properties::sprmCSymbol, Properties::sprmCFOle2, Properties::sprmNoop, Properties::sprmNoop, Properties::sprmNoop, Properties::sprmNoop,
        // 80
        Properties::sprmCIstd, Properties::sprmCIstdPermute, Properties::sprmCDefault, Properties::sprmCPlain, Properties::sprmNoop, Properties::sprmCFBold, Properties::sprmCFItalic, Properties::sprmCFStrike,
        Properties::sprmCFOutline, Properties::sprmCFShadow, Properties::sprmCFSmallCaps, Properties::sprmCFCaps, Properties::sprmCFVanish, Properties::sprmCRgFtc0, Properties::sprmCKul, Properties::sprmCSizePos,
        // 96
        Properties::sprmCDxaSpace, Properties::sprmCLid, Properties::sprmCIco, Properties::sprmCHps, Properties::sprmCHpsInc, Properties::sprmCHpsPos, Properties::sprmCHpsPosAdj, Properties::sprmCMajority,
        Properties::sprmCIss, Properties::sprmCHpsNew50, Properties::sprmCHpsInc1, Properties::sprmCHpsKern, Properties::sprmCMajority50, Properties::sprmCHpsMul, Properties::sprmCYsri, Properties::sprmNoop,
        // 112
        Properties::sprmNoop, Properties::sprmNoop, Properties::sprmNoop, Properties::sprmNoop, Properties::sprmNoop, Properties::sprmCFSpec, Properties::sprmCFObj, Properties::sprmPicBrcl,
        Properties::sprmPicScale, Properties::sprmPicBrcTop, Properties::sprmPicBrcLeft, Properties::sprmPicBrcBottom, Properties::sprmPicBrcRight, Properties::sprmNoop, Properties::sprmNoop, Properties::sprmNoop
    };
    static const MsWord::U16 rgsprmPrmWord8[0x80] =
    {
        sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmPIncLvl, sprmPJc,
        sprmPFSideBySide, sprmPFKeep, sprmPFKeepFollow, sprmPFPageBreakBefore,
        sprmPBrcl, sprmPBrcp, sprmPIlvl, sprmNoop, sprmPFNoLineNumb, sprmNoop,
        sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
        sprmNoop, sprmPFInTable, sprmPFTtp, sprmNoop, sprmNoop, sprmNoop,
        sprmPPc, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
        sprmNoop, sprmPWr, sprmNoop,sprmNoop, sprmNoop, sprmNoop, sprmNoop,
        sprmNoop, sprmPFNoAutoHyph, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
        sprmNoop, sprmPFLocked, sprmPFWidowControl, sprmNoop, sprmPFKinsoku,
        sprmPFWordWrap, sprmPFOverflowPunct, sprmPFTopLinePunct,
        sprmPFAutoSpaceDE, sprmPFAutoSpaceDN, sprmNoop, sprmNoop,
      // TBD: sprmCFStrikeRM does not exist!
      //sprmPISnapBaseLine, sprmNoop, sprmNoop, sprmNoop, sprmCFStrikeRM,
        sprmPISnapBaseLine, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
        sprmCFRMark, sprmCFFldVanish, sprmNoop, sprmNoop, sprmNoop, sprmCFData,
        sprmNoop, sprmNoop, sprmNoop, sprmCFOle2, sprmNoop, sprmCHighlight,
        sprmCFEmboss, sprmCSfxText, sprmNoop, sprmNoop, sprmNoop, sprmCPlain,
        sprmNoop, sprmCFBold, sprmCFItalic, sprmCFStrike, sprmCFOutline,
        sprmCFShadow, sprmCFSmallCaps,sprmCFCaps, sprmCFVanish, sprmNoop,
        sprmCKul, sprmNoop, sprmNoop, sprmNoop, sprmCIco, sprmNoop,
        sprmCHpsInc,sprmNoop, sprmCHpsPosAdj, sprmNoop, sprmCIss, sprmNoop,
        sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop,sprmNoop, sprmNoop,
        sprmNoop, sprmNoop, sprmCFDStrike, sprmCFImprint, sprmCFSpec, sprmCFObj,
        sprmPicBrcl,sprmPOutLvl, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
      // TBD: sprmPPnbrRMarkNot does not exist!`
      //sprmNoop, sprmPPnbrRMarkNot
        sprmNoop, sprmNoop
    };

    // Now get the opcode.
    if (fib.nFib > MsWord::s_maxWord6Version)
    {
        return rgsprmPrmWord8[shortOpcode];
    }
    else
    {
        return rgsprmPrmWord6[shortOpcode];
    }
}
