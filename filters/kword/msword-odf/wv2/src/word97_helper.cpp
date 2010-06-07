/* This file is part of the wvWare 2 project
   Copyright (C) 2001-2003 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "word97_helper.h"
#include "word95_helper.h"
#include "word97_generated.h"
#include "word95_generated.h"
#include "convert.h"
#include "olestream.h"
#include "styles.h"
#include "paragraphproperties.h"

#include <string.h> // memcpy
#include <algorithm>
#include <functional> // std::bind2nd for gcc 2.9x

#include "wvlog.h"

namespace wvWare
{

namespace Word97
{

namespace SPRM
{

struct opcodeBits
{
    U16 ispmd:9;
    U16 fSpec:1;
    U16 sgc:3;
    U16 spra:3;
};

// Apart from all the SPRMs in the documentation I added some from the OOo code.
// Additionally there are some xyzFake entries, which are used to get Word 6 sprms
// through the same code :-}
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
    sprmCUndocumented1 = 0x875,
    sprmPJc = 0x2403,
    sprmPJcFE = 0x2461, // Undocumented. According to OOo it's the asian equivalent to sprmPJc
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
    sprmPNLvlAnmFake = 0x25FF,       // Fake entry!
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
    sprmTUndocumented1 = 0x3615,
    sprmPWHeightAbs = 0x442B,
    sprmPDcs = 0x442C,
    sprmPShd80 = 0x442D,
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
    sprmCShd80 = 0x4866,
    sprmCIdslRMarkDel = 0x4867,
    sprmCCpg = 0x486B,
    sprmCRgLid0 = 0x486D,
    sprmCRgLid1 = 0x486E,
    sprmCRgLidUndocumented1 = 0x4873, // According to OOo it's equal to sprmCRgLid0
    sprmCUndocumented2 = 0x4874,
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
    sprmTUndocumented2 = 0x5664,
    sprmPDyaLine = 0x6412,
    sprmPBrcTop = 0x6424,
    sprmPBrcLeft = 0x6425,
    sprmPBrcBottom = 0x6426,
    sprmPBrcRight = 0x6427,
    sprmPBrcBetween = 0x6428,
    sprmPTableProps = 0x646b,
    sprmPBrcBar = 0x6629,
    sprmPHugePapx = 0x6645,
    sprmPHugePapx2 = 0x6646,
    sprmPTableLevelUndoc = 0x6649, // Undocumented. According to OOo it's the table level
    sprmCDttmRMark = 0x6805,
    sprmCObjLocation = 0x680E,
    sprmCDttmRMarkDel = 0x6864,
    sprmCBrc = 0x6865,
    sprmCCv = 0x6870,
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
    sprmPDxaRightFE = 0x845D,  // Undocumented. According to OOo it's the asian equivalent to sprmPDxaRight
    sprmPDxaLeft = 0x840F,
    sprmPDxaLeftFE = 0x845E,  // Undocumented. According to OOo it's the asian equivalent to sprmPDxaLeft
    sprmPDxaLeft1 = 0x8411,
    sprmPDxaLeft1FE = 0x8460,  // Undocumented. According to OOo it's the asian equivalent to sprmPDxaLeft1
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
    sprmPShd = 0xC64D,
    sprmPUndocumented1 = 0xC64E,
    sprmPUndocumented2 = 0xC64F,
    sprmPUndocumented3 = 0xC650,
    sprmPUndocumented4 = 0xC651,
    sprmCIstdPermute = 0xCA31,
    sprmCMajority = 0xCA47,
    sprmCHpsNew50 = 0xCA49,
    sprmCHpsInc1 = 0xCA4A,
    sprmCMajority50 = 0xCA4C,
    sprmCPropRMark = 0xCA57,
    sprmCDispFldRMark = 0xCA62,
    sprmCShd = 0xCA71,
    sprmCFELayout = 0xCA78,
    sprmPicScale = 0xCE01,
    sprmSOlstAnm = 0xD202,
    sprmSPropRMark = 0xD227,
    sprmTTableBorders80 = 0xD605,
    sprmTDefTable10 = 0xD606,
    sprmTDefTable = 0xD608,
    sprmTDefTableShd80 = 0xD609,
    sprmTDefTableShd = 0xD612,
    sprmTTableBorders = 0xD613,
    sprmTDefTableShd2nd = 0xD616,
    sprmTDefTableShd3rd = 0xD60C,
    sprmTBrcTopCv = 0xD61A,
    sprmTBrcLeftCv = 0xD61B,
    sprmTBrcBottomCv = 0xD61C,
    sprmTBrcRightCv = 0xD61D,
    sprmTSetBrc80 = 0xD620,
    sprmTSetBrc10 = 0xD626,
    sprmTDiagLine = 0xD62A,
    sprmTVertMerge = 0xD62B,
    sprmTVertAlign = 0xD62C,
    sprmTSetBrc = 0xD62F,
    sprmTUndocumentedSpacing = 0xD632, // OOo: specific spacing
    sprmTUndocumented8 = 0xD634,
    sprmTUndocumented9 = 0xD660, // "something to do with color" (OOo)
    sprmTCellBrcType = 0xD662,
    sprmCChs = 0xEA08,
    sprmCSizePos = 0xEA3F,
    sprmSDxaColWidth = 0xF203,
    sprmSDxaColSpacing = 0xF204,
    sprmTUndocumented10 = 0xF614,
    sprmTUndocumented11 = 0xF617,
    sprmTUndocumented12 = 0xF618,
    sprmTWidthIndent = 0xF661
} opcodes;

// The length of the SPRM parameter
U16 determineParameterLength( U16 sprm, const U8* in, WordVersion version )
{
    if ( version == Word8 ) {
        static const char operandSizes[ 8 ] = { 1, 1, 2, 4, 2, 2, 0, 3 };

        int index = ( sprm & 0xE000 ) >> 13;
        if ( operandSizes[ index ] != 0 )
            return operandSizes[ index ];
        else {
            // Get length of variable size operand.
            switch ( sprm ) {
                case sprmTDefTable10:
                case sprmTDefTable:
                    return readU16( in ) + 1;
                    break;
                case sprmPChgTabs:
                    if ( *in == 255 ) {
                        U8 itbdDelMax = in[ 1 ];
                        U8 itbdAddMax = in[ 1 + itbdDelMax * 4 ];
                        return 1 + itbdDelMax * 4 + itbdAddMax * 3;
                    }
                    else
                        return *in + 1;
                    break;
                default:
                    return *in + 1;
                    break;
            }
        }
    }
    else { // Word67
        if ( sprm > 255 )
            wvlog << "Error: Trying to get the length of a flaky SPRM (" << sprm << ", 0x" << hex
                  << sprm << dec << ") via the Word 95 method!" << endl;
        return Word95::SPRM::determineParameterLength( static_cast<U8>( sprm ), in );
    }
}

// Apply a <T> grpprl of a given size ("count" bytes long)
// This template function might be a bit sick, but it helps to
// avoid duplicated code, so what ;)
template<class T>
void apply(T* const t,
           S16 ( T::* applySPRM ) ( const U8*, const Style*, const StyleSheet*, OLEStreamReader*, WordVersion ),
           const U8* grpprl, U16 count, const Style* style, const StyleSheet* styleSheet,
           OLEStreamReader* dataStream, WordVersion version )
{
    if ( !grpprl )
        return;

    // We are using an integer here, that we can detect situations where
    // we read beyond the limit due to a buggy spec/impl.
    // The plain U16 would overflow and we'd be trapped in a loop.
    int safeCount = count;

    // walk through the grpprl, applying one sprm after the other
    while ( safeCount > 1 ) {
        S16 result = ( t->*applySPRM )( grpprl, style, styleSheet, dataStream, version );
        if ( result == -1 ) {
            U16 sprm;
            if ( version == Word8 ) {
                sprm = readU16( grpprl );
                grpprl += 2;
#ifdef WV2_DEBUG_SPRMS
                wvlog << "Seems like that's a different SPRM (0x" << hex << sprm << dec << ")... skipping" << endl;
#endif
            }
            else {
                sprm = *grpprl++;
#ifdef WV2_DEBUG_SPRMS
                wvlog << "Seems like that's a different SPRM (" << sprm << ")... skipping" << endl;
#endif
            }

            U16 len = determineParameterLength( sprm, grpprl, version );
            grpprl += len;
            safeCount -= len + ( version == Word8 ? 2 : 1 );
        }
        else {
            grpprl += result;
            safeCount -= result;
        }
    }
    if ( safeCount < 0 )
        wvlog << "Warning: We read past the end of the grpprl, buggy spec?" << endl;
}

U16 unzippedOpCode( U8 isprm )
{
    // Note: Changed sprmCFStrikeRM to sprmCFStrike and sprmPPnbrRMarkNot to sprmNoop
    static const U16 lut[] = {
        sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmPIncLvl, sprmPJc, sprmPFSideBySide,
        sprmPFKeep, sprmPFKeepFollow, sprmPFPageBreakBefore, sprmPBrcl, sprmPBrcp,
        sprmPIlvl, sprmNoop, sprmPFNoLineNumb, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
        sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmPFInTable, sprmPFTtp,
        sprmNoop, sprmNoop, sprmNoop, sprmPPc, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
        sprmNoop, sprmNoop, sprmNoop, sprmPWr, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
        sprmNoop, sprmNoop, sprmPFNoAutoHyph, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
        sprmNoop, sprmPFLocked, sprmPFWidowControl, sprmNoop, sprmPFKinsoku, sprmPFWordWrap,
        sprmPFOverflowPunct, sprmPFTopLinePunct, sprmPFAutoSpaceDE, sprmPFAutoSpaceDN,
        sprmNoop, sprmNoop, sprmPISnapBaseLine, sprmNoop, sprmNoop, sprmNoop, sprmCFStrike,
        sprmCFRMark, sprmCFFldVanish, sprmNoop, sprmNoop, sprmNoop, sprmCFData,
        sprmNoop, sprmNoop, sprmNoop, sprmCFOle2, sprmNoop, sprmCHighlight, sprmCFEmboss,
        sprmCSfxText, sprmNoop, sprmNoop, sprmNoop, sprmCPlain, sprmNoop, sprmCFBold,
        sprmCFItalic, sprmCFStrike, sprmCFOutline, sprmCFShadow, sprmCFSmallCaps,
        sprmCFCaps, sprmCFVanish, sprmNoop, sprmCKul, sprmNoop, sprmNoop, sprmNoop,
        sprmCIco, sprmNoop, sprmCHpsInc, sprmNoop, sprmCHpsPosAdj, sprmNoop, sprmCIss,
        sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
        sprmNoop, sprmNoop, sprmCFDStrike, sprmCFImprint, sprmCFSpec, sprmCFObj,
        sprmPicBrcl, sprmPOutLvl, sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
        sprmNoop };

    return lut[ isprm ];
}

/**
 * Converts a Word 6/7 SPRM to a Word 8 one.
 */
U16 word6toWord8( U8 sprm )
{
    static const U16 lut[] = {
        sprmNoop, sprmNoop, sprmPIstd, sprmPIstdPermute, sprmPIncLvl,
        sprmPJc, sprmPFSideBySide, sprmPFKeep, sprmPFKeepFollow, sprmPFPageBreakBefore,
        sprmPBrcl, sprmPBrcp, sprmPAnld, sprmPNLvlAnmFake, sprmPFNoLineNumb,
        sprmPChgTabsPapx, sprmPDxaRight, sprmPDxaLeft, sprmPNest, sprmPDxaLeft1,
        sprmPDyaLine, sprmPDyaBefore, sprmPDyaAfter, sprmPChgTabs, sprmPFInTable,
        sprmPFTtp, sprmPDxaAbs, sprmPDyaAbs, sprmPDxaWidth, sprmPPc,
        sprmPBrcTop10, sprmPBrcLeft10, sprmPBrcBottom10, sprmPBrcRight10, sprmPBrcBetween10,
        sprmPBrcBar10, sprmPDxaFromText10, sprmPWr, sprmPBrcTop, sprmPBrcLeft,
        sprmPBrcBottom, sprmPBrcRight, sprmPBrcBetween, sprmPBrcBar, sprmPFNoAutoHyph,
        sprmPWHeightAbs, sprmPDcs, sprmPShd80, sprmPDyaFromText, sprmPDxaFromText,
        sprmPFLocked, sprmPFWidowControl, sprmPRuler, sprmNoop, sprmNoop,
        sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
        sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
        sprmCFRMarkDel, sprmCFRMark, sprmCFFldVanish, sprmCPicLocation, sprmCIbstRMark,
        sprmCDttmRMark, sprmCFData, sprmCIdslRMark, sprmCChs, sprmCSymbol,
        sprmCFOle2, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
        sprmCIstd, sprmCIstdPermute, sprmCDefault, sprmCPlain, sprmNoop,
        sprmCFBold, sprmCFItalic, sprmCFStrike, sprmCFOutline, sprmCFShadow,
        sprmCFSmallCaps, sprmCFCaps, sprmCFVanish, sprmCFtcDefault, sprmCKul,
        sprmCSizePos, sprmCDxaSpace, sprmCLid, sprmCIco, sprmCHps,
        sprmCHpsInc, sprmCHpsPos, sprmCHpsPosAdj, sprmCMajority, sprmCIss,
        sprmCHpsNew50, sprmCHpsInc1, sprmCHpsKern, sprmCMajority50, sprmCHpsMul,
        sprmCYsri, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
        sprmNoop, sprmNoop, sprmCFSpec, sprmCFObj, sprmPicBrcl,
        sprmPicScale, sprmPicBrcTop, sprmPicBrcLeft, sprmPicBrcBottom, sprmPicBrcRight,
        sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
        sprmNoop, sprmScnsPgn, sprmSiHeadingPgn, sprmSOlstAnm, sprmNoop,
        sprmNoop, sprmSDxaColWidth, sprmSDxaColSpacing, sprmSFEvenlySpaced, sprmSFProtected,
        sprmSDmBinFirst, sprmSDmBinOther, sprmSBkc, sprmSFTitlePage, sprmSCcolumns,
        sprmSDxaColumns, sprmSFAutoPgn, sprmSNfcPgn, sprmSDyaPgn, sprmSDxaPgn,
        sprmSFPgnRestart, sprmSFEndnote, sprmSLnc, sprmSGprfIhdt, sprmSNLnnMod,
        sprmSDxaLnn, sprmSDyaHdrTop, sprmSDyaHdrBottom, sprmSLBetween, sprmSVjc,
        sprmSLnnMin, sprmSPgnStart, sprmSBOrientation, sprmSBCustomize, sprmSXaPage,
        sprmSYaPage, sprmSDxaLeft, sprmSDxaRight, sprmSDyaTop, sprmSDyaBottom,
        sprmSDzaGutter, sprmSDmPaperReq, sprmNoop, sprmNoop, sprmNoop,
        sprmNoop, sprmNoop, sprmNoop, sprmNoop, sprmNoop,
        sprmNoop, sprmNoop, sprmTJc, sprmTDxaLeft, sprmTDxaGapHalf,
        sprmTFCantSplit, sprmTTableHeader, sprmTTableBorders80, sprmTDefTable10, sprmTDyaRowHeight,
        sprmTDefTable, sprmTDefTableShd80, sprmTTlp, sprmTSetBrc80, sprmTInsert,
        sprmTDelete, sprmTDxaCol, sprmTMerge, sprmTSplit, sprmTSetBrc10,
        sprmTSetShd };

    U16 s;
    if ( sprm > 200 )
        s = sprmNoop;
    else
        s = lut[ sprm ];
    if ( s == sprmNoop )
        wvlog << "Warning: Got a Word 6 " << static_cast<int>( sprm ) << " and return a noop!" << endl;
    return s;
}

} // namespace SPRM


ParagraphProperties* initPAPFromStyle( const U8* exceptions, const StyleSheet* styleSheet, OLEStreamReader* dataStream, WordVersion version )
{
    ParagraphProperties* properties = 0;
    if ( exceptions == 0 ) {
        if ( !styleSheet ) {
            wvlog << "Warning: Couldn't read from the stylesheet." << endl;
            return new ParagraphProperties();
        }
        const Style* normal = styleSheet->styleByID( 0 );  // stiNormal == 0x0000
        if ( normal )
            properties = new ParagraphProperties( normal->paragraphProperties() );
        else
            properties = new ParagraphProperties();
    }
    else {
        int cb = static_cast<int>( *exceptions++ ) << 1;  // Count of words (x2) -> count of bytes
        if ( cb == 0 ) {                    // odd PAPX -> skip the padding byte
            cb = static_cast<int>( *exceptions++ ) << 1;
            cb -= 2;
        }
        else
            cb -= version == Word8 ? 3 : 2;  // Don't ask me, why Word 6/7 only needs -2 bytes

        U16 tmpIstd = readU16( exceptions );
        exceptions += 2;

        const Style* style = 0;
        if ( styleSheet ) {
            style = styleSheet->styleByIndex( tmpIstd );
            if ( style )
                properties = new ParagraphProperties( style->paragraphProperties() );
            else {
                wvlog << "Warning: Couldn't read from the style, just applying the PAPX." << endl;
                properties = new ParagraphProperties();
            }
        }
        else {
            wvlog << "Warning: Couldn't read from the stylesheet, just applying the PAPX." << endl;
            properties = new ParagraphProperties();
        }

        properties->pap().istd = tmpIstd;

        cb = cb < 0 ? 0 : cb;  // safety :-}
        // Note: The caller also has to override the PHE from this
        // PAP with the PHE stored in the BX
        properties->pap().apply( exceptions, cb, style, styleSheet, dataStream, version );
    }
    return properties;
}

Word97::TAP* initTAP( const U8* exceptions, OLEStreamReader* dataStream, WordVersion version )
{
    Word97::TAP* tap = new Word97::TAP;

    if ( exceptions == 0 )
        return tap;

    int cb = static_cast<int>( *exceptions++ ) << 1;  // Count of words (x2) -> count of bytes
    if ( cb == 0 ) {                    // odd PAPX -> skip the padding byte
        cb = static_cast<int>( *exceptions++ ) << 1;
        cb -= 2;
    }
    else
        cb -= 3;
    
    exceptions += 2; // skip the istd
    cb = cb < 0 ? 0 : cb;  // safety :-}
    tap->apply( exceptions, cb, 0, 0, dataStream, version ); // we don't need a style(sheet), do we?
    

    return tap;
}


// Apply a PAP grpprl of a given size ("count" bytes long)
void PAP::apply( const U8* grpprl, U16 count, const Style* style, const StyleSheet* styleSheet,
                 OLEStreamReader* dataStream, WordVersion version )
{
    // A PAP grpprl might contain TAP sprms, we just skip them
    SPRM::apply<PAP>( this, &PAP::applyPAPSPRM, grpprl, count, style, styleSheet, dataStream, version );
}

    U32 icoToRGB(U16 ico)
    {
        switch(ico)
        {
            case 0: //default and we choose black as most paper is white
                return 0xFF000000;
            case 1://black
                return 0x00000000;
            case 2://blue
                return 0x000000FF;
            case 3://cyan
                return 0x0000FFFF;
            case 4://green
                return 0x00008000;
            case 5://magenta
                return 0x00FF00FF;
            case 6://red
                return 0x00FF0000;
            case 7://yellow
                return 0x00FFFF00;
            case 8://white
                return 0x00FFFFFF;
            case 9://dark blue
                return 0x0000008B;
            case 10://dark cyan
                return 0x00008B8B;
            case 11://dark green
                return 0x00006400;
            case 12://dark magenta
                return 0x008B008B;
            case 13://dark red
                return 0x008B0000;
            case 14://dark yellow
                return 0x00808000;
            case 15://dark gray
                return 0x00A9A9A9;
            case 16://light gray
                return 0x00D3D3D3;

            default:
                return 0x000000;
        }
    }

// Helper methods for the more complex sprms
namespace
{
    // Adds the tabs of the sprmPChgTabs* sprms. Pass a pointer to the
    // itbdAddMax and the vector
    // Returns the number of tabs added
    typedef std::vector<Word97::TabDescriptor> TabDescVector;
    U8 addTabs( const U8* ptr, TabDescVector& rgdxaTab )
    {
        //wvlog << "Before adding the tabs: " << (int)rgdxaTab.size() << endl;
        // Remember where the end was
        const TabDescVector::size_type oldSize = rgdxaTab.size();
        // Now append the new ones, we'll then sort the vector using inplace_merge
        const U8 itbdAddMax = *ptr++;
        //wvlog << "                           itbdAddMax=" << (int)itbdAddMax << endl;
        for ( U8 i = 0 ; i < itbdAddMax ; ++i )
        {
            // #### We should probably add a proper constructor to TabDescriptor (Werner)
            TabDescriptor descr;
            descr.dxaTab = readS16( ptr + sizeof( S16 ) * i );
            //wvlog << "                           dxaPos=" << descr.dxaTab << endl;
            descr.tbd = TBD( readU8( ptr + sizeof( S16 ) * itbdAddMax + i ) );
            rgdxaTab.push_back( descr );
        }
        if ( oldSize != 0 && itbdAddMax ) {
            TabDescVector::iterator middle = rgdxaTab.begin();
            middle += oldSize + 1u;
            std::inplace_merge( rgdxaTab.begin(), middle, rgdxaTab.end() );
        }
        TabDescVector::iterator uend = std::unique( rgdxaTab.begin(), rgdxaTab.end() );
        if ( uend != rgdxaTab.end() )
            rgdxaTab.erase( uend, rgdxaTab.end() );
        //wvlog << "After applying sprmPChgTabs(Papx) : " << (int)rgdxaTab.size() << endl;
        return itbdAddMax;
    }

    // A zone where tabstops are forbidden, needed for sprmPChgTabs
    struct Zone
    {
        Zone( const U8* ptr, U8 index, U8 itbdDelMax )
        {
            m_center = readS16( ptr + index * sizeof( S16 ) );
            // A negative value doesn't make sense here, right? Hmmm
            m_plusMinus = std::abs( readS16( ptr + itbdDelMax * sizeof( S16 ) + index * sizeof( S16 ) ) );
        }

        bool contains( S16 position ) const { return m_center - m_plusMinus <= position && m_center + m_plusMinus >= position; }

        S16 m_center;
        S16 m_plusMinus;
    };

    struct InZone : public std::binary_function<TabDescriptor, Zone, bool>
    {
        bool operator()(const TabDescriptor &tab, const Zone& zone) const { return zone.contains( tab.dxaTab ); }
    };

    U16 getSPRM( const U8** ptr, WordVersion version, U16& sprmLength )
    {
        U16 sprm;
        if ( version == Word8 ) {
            sprm = readU16( *ptr );
            ( *ptr ) += 2;
            sprmLength = SPRM::determineParameterLength( sprm, *ptr, version ) + 2;
        }
        else {
            sprm = **ptr;
            ++( *ptr );
            sprmLength = SPRM::determineParameterLength( sprm, *ptr, version ) + 1;
            sprm = SPRM::word6toWord8( sprm );  // "fix" it for the Word 8 switch statements below
        }
        return sprm;
    }

    void readBRC( BRC& brc, const U8* ptr, WordVersion version )
    {
        if ( version == Word8 )
            brc.readPtr( ptr );
        else
            brc = toWord97( Word95::BRC( ptr ) );
    }
}       

// Returns -1 if this wasn't a PAP sprm and it returns the length
// of the applied sprm if it was successful
S16 PAP::applyPAPSPRM( const U8* ptr, const Style* style, const StyleSheet* styleSheet, OLEStreamReader* dataStream, WordVersion version )
{
    U16 sprmLength;
    const U16 sprm( getSPRM( &ptr, version, sprmLength ) );
#ifdef WV2_DEBUG_SPRMS
    wvlog << "got a sprm: 0x" << hex << sprm << dec << endl;
#endif

    // Is it a PAP sprm?
    if ( ( ( sprm & 0x1C00 ) >> 10 ) != 1 ) {
#ifdef WV2_DEBUG_SPRMS
        wvlog << "Warning: You're trying to apply a non PAP sprm to a PAP. Not necessarily bad." << endl;
#endif
        return -1;  // tell the caller to try with something else (e.g. applying a TAP)...
    }
    // which one? ;)
    switch ( sprm ) {
        case SPRM::sprmNoop:
            wvlog << "Huh? Found a sprmNoop..." << endl;
            break;
        case SPRM::sprmPIstd:
            istd = readU16( ptr );
            break;
        case SPRM::sprmPIstdPermute:
        {
            const U8* myPtr = ptr + 3; // cch, fLongg, fSpare
            const U16 istdFirst = readU16( myPtr );
            myPtr += 2;
            const U16 istdLast = readU16( myPtr );
            myPtr += 2;
            if ( istd > istdFirst && istd <= istdLast )
                istd = myPtr[ istd - istdFirst ];
            break;
        }
        case SPRM::sprmPIncLvl:
            // Applies only to list styles, note: SPEC defect, there's no stc anymore
            if ( istd >= 1 && istd <= 9 ) {
                istd += static_cast<S8>( *ptr );
                lvl += static_cast<S8>( *ptr );
            }
            break;
        case SPRM::sprmPJc:
        case SPRM::sprmPJcFE: // asian version, according to OOo (fall-through intended)
            jc = *ptr;
            break;
        case SPRM::sprmPFSideBySide:
            fSideBySide = *ptr == 1;
            break;
        case SPRM::sprmPFKeep:
            fKeep = *ptr == 1;
            break;
        case SPRM::sprmPFKeepFollow:
            fKeepFollow = *ptr == 1;
            break;
        case SPRM::sprmPFPageBreakBefore:
            fPageBreakBefore = *ptr == 1;
            break;
        case SPRM::sprmPBrcl:
            brcl = *ptr;
            break;
        case SPRM::sprmPBrcp:
            brcp = *ptr;
            break;
        case SPRM::sprmPIlvl:
            ilvl = *ptr;
            break;
        case SPRM::sprmPIlfo:
            ilfo = readS16( ptr );
            break;
        case SPRM::sprmPFNoLineNumb:
            fNoLnn = *ptr == 1;
            break;
        case SPRM::sprmPChgTabsPapx:
        {
            const U8* myPtr = ptr;
            U8 cch = *myPtr++;
            U8 itbdDelMax = *myPtr++;
            std::vector<Word97::TabDescriptor>::iterator tabIt = rgdxaTab.begin();
            //wvlog << "Applying sprmPChgTabsPapx. itbdDelMax=" << (int)itbdDelMax << endl;
            // First the 'to be deleted' array
            for ( U8 i = 0 ; i < itbdDelMax ; ++i )
            {
                TabDescriptor testDescr;
                testDescr.dxaTab = readS16( myPtr );
                myPtr += 2;
                // Look for this one, starting at last position (they are sorted)
                tabIt = std::find( tabIt, rgdxaTab.end(), testDescr );
                if ( tabIt != rgdxaTab.end() )
                {
                    tabIt = rgdxaTab.erase( tabIt );
                    itbdMac--;
                }
            }
            U8 itbdAddMax = addTabs( myPtr, rgdxaTab );
            itbdMac += itbdAddMax;

            //wvlog << "After applying sprmPChgTabsPapx : " << (int)rgdxaTab.size() << endl;

            if ( cch != 1 + 2 * itbdDelMax + 1 + 3 * itbdAddMax )
                wvlog << "Offset problem in sprmPChgTabsPapx. cch=" << static_cast<int>( cch ) << " data size=" << 1 + 2 * itbdDelMax + 1 + 3 * itbdAddMax << endl;
            break;
        }
        case SPRM::sprmPDxaRight:
        case SPRM::sprmPDxaRightFE: // asian version, according to OOo (fall-through intended)
            dxaRight = readS16( ptr );
            break;
        case SPRM::sprmPDxaLeft:
        case SPRM::sprmPDxaLeftFE: // asian version, according to OOo (fall-through intended)
            dxaLeft = readS16( ptr );
            break;
        case SPRM::sprmPNest:
            dxaLeft += readS16( ptr );
            dxaLeft = dxaLeft < 0 ? 0 : dxaLeft;
            break;
        case SPRM::sprmPDxaLeft1:
        case SPRM::sprmPDxaLeft1FE: // asian version, according to OOo (fall-through intended)
            dxaLeft1 = readS16( ptr );
            break;
        case SPRM::sprmPDyaLine:
            lspd.dyaLine = readS16( ptr );
            lspd.fMultLinespace = readS16( ptr + 2 );
            break;
        case SPRM::sprmPDyaBefore:
            dyaBefore = readU16( ptr );
            break;
        case SPRM::sprmPDyaAfter:
            dyaAfter = readU16( ptr );
            break;
        case SPRM::sprmPChgTabs:
        {
            const U8* myPtr = ptr;
            const U8 cch = *myPtr++;
            const U8 itbdDelMax = *myPtr++;
            // Remove the tabs within the deletion zones
            std::vector<TabDescriptor>::iterator newEnd = rgdxaTab.end();
            for ( U8 i = 0; i < itbdDelMax; ++i )
                newEnd = std::remove_if ( rgdxaTab.begin(), newEnd, std::bind2nd( InZone(), Zone( myPtr, i, itbdDelMax ) ) );
            rgdxaTab.erase( newEnd, rgdxaTab.end() ); // really get rid of them
            itbdMac = rgdxaTab.size();

            // Add the new tabs
            myPtr += itbdDelMax * 4;
            U8 itbdAddMax = addTabs( myPtr, rgdxaTab );
            itbdMac += itbdAddMax;

            if ( cch != 255 && cch != 1 + 4 * itbdDelMax + 1 + 3 * itbdAddMax )
                wvlog << "Offset problem in sprmPChgTabs. cch=" << static_cast<int>( cch ) << " data size=" << 1 + 4 * itbdDelMax + 1 + 3 * itbdAddMax << endl;
            //wvlog << "SPRM::sprmPChgTabs done ### " << rgdxaTab.size() << endl;
            break;
        }
        case SPRM::sprmPFInTable:
            fInTable = *ptr == 1;
            break;
        case SPRM::sprmPFTtp:
            fTtp = *ptr == 1;
            break;
        case SPRM::sprmPDxaAbs:
            dxaAbs = readS16( ptr );
            break;
        case SPRM::sprmPDyaAbs:
            dyaAbs = readS16( ptr );
            break;
        case SPRM::sprmPDxaWidth:
            dxaWidth = readS16( ptr );
            break;
        case SPRM::sprmPPc:
        {
            pcVert = ( *ptr & 0x30 ) >> 4;
            pcHorz = ( *ptr & 0xC0 ) >> 6;
            break;
        }
        case SPRM::sprmPBrcTop10:
            wvlog << "Warning: sprmPBrcTop10 doesn't make sense for Word 8" << endl;
            break;
        case SPRM::sprmPBrcLeft10:
            wvlog << "Warning: sprmPBrcLeft10 doesn't make sense for Word 8" << endl;
            break;
        case SPRM::sprmPBrcBottom10:
            wvlog << "Warning: sprmPBrcBottom10 doesn't make sense for Word 8" << endl;
            break;
        case SPRM::sprmPBrcRight10:
            wvlog << "Warning: sprmPBrcRight10 doesn't make sense for Word 8" << endl;
            break;
        case SPRM::sprmPBrcBetween10:
            wvlog << "Warning: sprmPBrcBetween10 doesn't make sense for Word 8" << endl;
            break;
        case SPRM::sprmPBrcBar10:
            wvlog << "Warning: sprmPBrcBar10 doesn't make sense for Word 8" << endl;
            break;
        case SPRM::sprmPDxaFromText10:
            wvlog << "Warning: sprmPDxaFromText10 doesn't make sense for Word 8" << endl;
            break;
        case SPRM::sprmPWr:
            wr = *ptr;
            break;
        case SPRM::sprmPBrcTop:
            readBRC( brcTop, ptr, version );
            break;
        case SPRM::sprmPBrcLeft:
            readBRC( brcLeft, ptr, version );
            break;
        case SPRM::sprmPBrcBottom:
            readBRC( brcBottom, ptr, version );
            break;
        case SPRM::sprmPBrcRight:
            readBRC( brcRight, ptr, version );
            break;
        case SPRM::sprmPBrcBetween:
            readBRC( brcBetween, ptr, version );
            break;
        case SPRM::sprmPBrcBar:
            readBRC( brcBar, ptr, version );
            break;
        case SPRM::sprmPFNoAutoHyph:
            fNoAutoHyph = *ptr == 1;
            break;
        case SPRM::sprmPWHeightAbs:
        {
            U16 heightAbsTmp = readU16( ptr );
            dyaHeight = ( heightAbsTmp & 0xfffe );
            break;
        }
        case SPRM::sprmPDcs:
            dcs.readPtr( ptr );
            break;
        case SPRM::sprmPShd80:
            shd.readPtr( ptr );
            break;
        case SPRM::sprmPShd:
            shd.readSHDOperandPtr( ptr );       // and read the SHDOperand structure into SHD
            break;
        case SPRM::sprmPDyaFromText:
            dyaFromText = readS16( ptr );
            break;
        case SPRM::sprmPDxaFromText:
            dxaFromText = readS16( ptr );
            break;
        case SPRM::sprmPFLocked:
            fLocked = *ptr == 1;
            break;
        case SPRM::sprmPFWidowControl:
            fWidowControl = *ptr == 1;
            break;
        case SPRM::sprmPRuler:
            wvlog << "Warning: sprmPRuler not implemented" << endl;
            break;
        case SPRM::sprmPFKinsoku:
            fKinsoku = *ptr == 1;
            break;
        case SPRM::sprmPFWordWrap:
            fWordWrap = *ptr == 1;
            break;
        case SPRM::sprmPFOverflowPunct:
            fOverflowPunct = *ptr == 1;
            break;
        case SPRM::sprmPFTopLinePunct:
            fTopLinePunct = *ptr == 1;
            break;
        case SPRM::sprmPFAutoSpaceDE:
            fAutoSpaceDE = *ptr == 1;
            break;
        case SPRM::sprmPFAutoSpaceDN:
            fAutoSpaceDN = *ptr == 1;
            break;
        case SPRM::sprmPWAlignFont:
            wAlignFont = readU16( ptr );
            break;
        case SPRM::sprmPFrameTextFlow:
            wvlog << "Warning: sprmPFrameTextFlow not implemented" << endl;
            break;
        case SPRM::sprmPISnapBaseLine:
            wvlog << "Warning: sprmPISnapBaseLine is obsolete" << endl;
            break;
        case SPRM::sprmPAnld:
            if ( version == Word8 )
                anld.readPtr( ptr + 1 ); // variable length, skip length byte
            else
                anld = toWord97( Word95::ANLD( ptr + 1 ) );
            break;
        case SPRM::sprmPPropRMark:
            fPropRMark = *( ptr + 1 );
            ibstPropRMark = readS16( ptr + 2 );
            dttmPropRMark.readPtr( ptr + 4 );
            break;
        case SPRM::sprmPOutLvl:
            lvl = readS8( ptr );
            break;
        case SPRM::sprmPFBiDi:
            fBiDi = readS8( ptr );
            break;
        case SPRM::sprmPFNumRMIns:
            fNumRMIns = *ptr == 1;
            break;
        case SPRM::sprmPCrLf:
            fCrLf = *ptr;
            break;
        case SPRM::sprmPNumRM:
            numrm.readPtr( ptr + 1 );
            break;
        case SPRM::sprmPHugePapx:
        case SPRM::sprmPHugePapx2:
        {
            if ( dataStream ) {
                dataStream->push();
                dataStream->seek( readU32( ptr ), G_SEEK_SET );
                const U16 count( dataStream->readU16() );
                U8* grpprl = new U8[ count ];
                dataStream->read( grpprl, count );
                dataStream->pop();

                apply( grpprl, count, style, styleSheet, dataStream, version );
                delete [] grpprl;
            }
            else
                wvlog << "Error: sprmPHugePapx found, but no data stream!" << endl;
            break;
        }
        case SPRM::sprmPFUsePgsuSettings:
            fUsePgsuSettings = *ptr == 1;
            break;
        case SPRM::sprmPFAdjustRight:
            fAdjustRight = *ptr == 1;
            break;
        case SPRM::sprmPNLvlAnmFake:
            nLvlAnm = *ptr;
            break;
        case SPRM::sprmPTableLevelUndoc:
            // No idea if we have to use that one... for Word 2000 or newer it's there
            if ( readU32( ptr ) != 1 )
                wvlog << "++++++++++++++++ Table level=" << readU32( ptr ) << endl;
            break;
        case SPRM::sprmPUndocumented1:
        case SPRM::sprmPUndocumented2:
        case SPRM::sprmPUndocumented3:
        case SPRM::sprmPUndocumented4:
            break;
        default:
            wvlog << "Huh? None of the defined sprms matches 0x" << hex << sprm << dec << "... trying to skip anyway" << endl;
            break;
    }
    return static_cast<S16>( sprmLength );  // length of the SPRM
}

// Apply a CHP grpprl of a given size ("count" bytes long)
void CHP::apply( const U8* grpprl, U16 count, const Style* paragraphStyle, const StyleSheet* styleSheet,
                 OLEStreamReader* dataStream, WordVersion version )
{
    // There should be only CHP sprms in the grpprl we get
    SPRM::apply<CHP>( this, &CHP::applyCHPSPRM, grpprl, count, paragraphStyle, styleSheet, dataStream, version );
}

void CHP::applyExceptions( const U8* exceptions, const Style* paragraphStyle, const StyleSheet* styleSheet,
                           OLEStreamReader* dataStream, WordVersion version )
{
    if ( exceptions == 0 )
        return;
    U8 cb = *exceptions;
    ++exceptions;
    apply( exceptions, cb, paragraphStyle, styleSheet, dataStream, version );
}

// Helper functions for more complex sprms
namespace
{
    const Word97::CHP* determineCHP( U16 istd, const Style* paragraphStyle, const StyleSheet* styleSheet )
    {
        const Word97::CHP* chp( 0 );
        if ( istd == 10 && paragraphStyle )
            chp = &paragraphStyle->chp();
        else if ( istd != 10 && styleSheet ) {
            const Style* style( styleSheet->styleByIndex( istd ) );
            chp = style != 0 && style->type() == Style::sgcChp ? &style->chp() : 0;
        }
        else
            wvlog << "Warning: sprmCFxyz couldn't find a style" << endl;
        return chp;
    }
}

// Returns -1 if this wasn't a CHP sprm and it returns the length
// of the applied sprm if it was successful
S16 CHP::applyCHPSPRM( const U8* ptr, const Style* paragraphStyle, const StyleSheet* styleSheet, OLEStreamReader* dataStream, WordVersion version )
{
    U16 sprmLength;
    const U16 sprm( getSPRM( &ptr, version, sprmLength ) );
#ifdef WV2_DEBUG_SPRMS
    wvlog << "got a sprm: 0x" << hex << sprm << dec << endl;
#endif

    // Is it a CHP sprm?
    if ( ( ( sprm & 0x1C00 ) >> 10 ) != 2 ) {
#ifdef WV2_DEBUG_SPRMS
        wvlog << "Warning: You're trying to apply a non CHP sprm to a CHP. Not necessarily bad." << endl;
#endif
        return -1;
    }
    // which one? ;)
    switch ( sprm ) {
        case SPRM::sprmNoop:
            wvlog << "Huh? Found a sprmNoop..." << endl;
            break;
        case SPRM::sprmCFRMarkDel:
            fRMarkDel = *ptr == 1;
            break;
        case SPRM::sprmCFRMark:
            fRMark = *ptr == 1;
            break;
        case SPRM::sprmCFFldVanish:
            fFldVanish = *ptr == 1;
            break;
        case SPRM::sprmCPicLocation:
            fSpec = 1;
            fcPic_fcObj_lTagObj = readS32( ptr + ( version == Word8 ? 0 : 1 ) );
            break;
        case SPRM::sprmCIbstRMark:
            ibstRMark = readS16( ptr );
            break;
        case SPRM::sprmCDttmRMark:
            dttmRMark.readPtr( ptr );
            break;
        case SPRM::sprmCFData:
            fData = *ptr == 1;
            break;
        case SPRM::sprmCIdslRMark:
            idslRMReason = readS16( ptr );
            break;
        case SPRM::sprmCChs:
            fChsDiff = *ptr == 1;
            chse = readU16( ptr + 1 );
            break;
        case SPRM::sprmCSymbol:
            // First the length byte...
            ftcSym = readS16( ptr + 1 );
            if ( version == Word8 )
                xchSym = readS16( ptr + 3 );
            else
                xchSym = *( ptr + 3 );
#ifdef WV2_DEBUG_SPRMS
            wvlog << "sprmCSymbol: ftcSym=" << ftcSym << " xchSym=" << xchSym << endl;
#endif
            fSpec = 1;
            break;
        case SPRM::sprmCFOle2:
            fOle2 = *ptr == 1;
            break;
        case SPRM::sprmCIdCharType:
            wvlog << "Warning: sprmCIdCharType doesn't make sense for Word 8" << endl;
            break;
        case SPRM::sprmCHighlight:
            icoHighlight = *ptr;
            fHighlight = icoHighlight != 0 ? 1 : 0;
            break;
        case SPRM::sprmCObjLocation:
            fcPic_fcObj_lTagObj = readU32( ptr );
            break;
        case SPRM::sprmCFFtcAsciSymb:
            fFtcAsciSym = *ptr == 1;
            break;
        case SPRM::sprmCIstd:
        {
            wvlog << "######################## old character style = " << istd << endl;
            istd = readS16( ptr );
            if ( styleSheet ) {
                wvlog << "Trying to change the character style to " << istd << endl;
                const Style* style = styleSheet->styleByIndex( istd );
                if ( style && style->type() == Style::sgcChp ) {
                    wvlog << "got a character style!" << endl;
                    const UPECHPX& upechpx( style->upechpx() );
                    apply( upechpx.grpprl, upechpx.cb, paragraphStyle, styleSheet, dataStream, version );
                }
                else
                    wvlog << "Warning: Couldn't find the character style with istd " << istd << endl;
            }
            else
                wvlog << "Warning: Tried to change the character style, but the stylesheet was 0" << endl;
            break;
        }
        case SPRM::sprmCIstdPermute:
        {
            const U8* myPtr = ptr + 3; // cch, fLongg, fSpare
            const U16 istdFirst = readU16( myPtr );
            myPtr += 2;
            const U16 istdLast = readU16( myPtr );
            myPtr += 2;
            if ( istd > istdFirst && istd <= istdLast )
                istd = myPtr[ istd - istdFirst ];
            break;
        }
        case SPRM::sprmCDefault:
            fBold = false;
            fItalic = false;
            fOutline = false;
            fStrike = false;
            fShadow = false;
            fSmallCaps = false;
            fCaps = false;
            fVanish = false;
            kul = 0;
            cv = 0;
            break;
        case SPRM::sprmCPlain:
        {
            bool fSpecBackup = fSpec;
            if ( paragraphStyle )
                *this = paragraphStyle->chp();
            fSpec = fSpecBackup;
            break;
        }
        case SPRM::sprmCKcd:
            kcd = *ptr;
            break;
        case SPRM::sprmCFBold:              // handling of bold style property
            if ( *ptr < 128 ) {             // if ToggleOperand == 1, the text is BOLD
                fBold = *ptr == 1;
            } else {                        // if ToggleOperand >= 128
                const Word97::CHP* chp( determineCHP( istd, paragraphStyle, styleSheet ) ); // get CHP by the istd and parent (paragraph)
                if ( *ptr == 128 && chp ) {
                    fBold = chp->fBold;     // for ToggleOperand == 128, the BOLD property of this equals the BOLD property of parent (paragraph)
                } else if ( *ptr == 129 && chp ) {
                    fBold = !chp->fBold;    // for ToggleOperand == 129, the BOLD property of this is oposite to BOLD property of parent (paragraph)
                } else if ( !chp ) {
                    fBold = 1;              // if we failed to get chp by istd and parent AND we should change the BOLD property, just make it BOLD
                }
            }
            break;
        case SPRM::sprmCFItalic:
            if ( *ptr < 128 )
                fItalic = *ptr == 1;
            else {
                //see comments on SPRM::sprmCFBold above
                const Word97::CHP* chp( determineCHP( istd, paragraphStyle, styleSheet ) );
                if (*ptr == 128 && chp) {
                    fItalic = chp->fItalic;
                } else if (*ptr == 129 && chp) {
                    fItalic = !chp->fItalic;
                } else if ( !chp ) {
                    fItalic = 1;
                }
            }
            break;
        case SPRM::sprmCFStrike:
            wvlog << "sprmCFStrike -- fStrike = " << static_cast<int>( fStrike ) << " *ptr = " << static_cast<int>( *ptr ) << endl;
            if (*ptr < 128)
                fStrike = *ptr == 1;
            else {
                const Word97::CHP* chp(determineCHP( istd, paragraphStyle, styleSheet));
                if ( chp )
                    wvlog << "chp->fStrike = " << static_cast<int>( chp->fStrike ) << endl;
                if (*ptr == 128 && chp) {
                    fStrike = chp->fStrike;
                } else if (*ptr == 129 && chp) {
                    fStrike = !chp->fStrike;
                } else if (!chp) {
                    fStrike = 1;
                }
            }
            wvlog << "sprmCFStrike -- fStrike (changed) = " << static_cast<int>( fStrike ) << endl;
            break;
        case SPRM::sprmCFOutline:
            if (*ptr < 128)
                fOutline = *ptr == 1;
            else {
                const Word97::CHP* chp(determineCHP(istd, paragraphStyle, styleSheet));
                if (*ptr == 128 && chp) {
                    fOutline = chp->fOutline;
                } else if (*ptr == 129 && chp) {
                    fOutline = !chp->fOutline;
                } else if (!chp) {
                    fOutline = 1;
                }
            }
            break;
        case SPRM::sprmCFShadow:
            if (*ptr < 128)
                fShadow = *ptr == 1;
            else {
                const Word97::CHP* chp(determineCHP(istd, paragraphStyle, styleSheet));
                if (*ptr == 128 && chp) {
                    fShadow = chp->fShadow;
                } else if (*ptr == 129 && chp) {
                    fShadow = !chp->fShadow;
                } else if (!chp) {
                    fShadow = 1;
                }
            }
            break;
        case SPRM::sprmCFSmallCaps:
            if (*ptr < 128)
                fSmallCaps = *ptr == 1;
            else {
                const Word97::CHP* chp(determineCHP(istd, paragraphStyle, styleSheet));
                if (*ptr == 128 && chp) {
                    fSmallCaps = chp->fSmallCaps;
                } else if (*ptr == 129 && chp) {
                    fSmallCaps = !chp->fSmallCaps;
                } else if (!chp) {
                    fSmallCaps = 1;
                }
            }
            break;
        case SPRM::sprmCFCaps:
            if ( *ptr < 128 )
                fCaps = *ptr == 1;
            else {
                const Word97::CHP* chp( determineCHP( istd, paragraphStyle, styleSheet ) );
                if (*ptr == 128 && chp) {
                    fCaps = chp->fCaps;
                } else if (*ptr == 129 && chp) {
                    fCaps = !chp->fCaps;
                } else if (!chp) {
                    fCaps = 1;
                }
            }
            break;
        case SPRM::sprmCFVanish:
            if ( *ptr < 128 )
                fVanish = *ptr == 1;
            else {
                const Word97::CHP* chp( determineCHP( istd, paragraphStyle, styleSheet ) );
                if (*ptr == 128 && chp) {
                    fVanish = chp->fVanish;
                } else if (*ptr == 129 && chp) {
                    fVanish = !chp->fVanish;
                } else if (!chp) {
                    fVanish = 1;
                }
            }
            break;
        case SPRM::sprmCFtcDefault:
            // We are abusing this SPRM for Word 6 purposes (sprmCFtc, 93)
            //wvlog << "Error: sprmCFtcDefault only used internally in MS Word" << endl;
            ftcAscii = ftcFE = ftcOther = ftc = readS16( ptr );
            break;
        case SPRM::sprmCKul:
            kul = *ptr;
            break;
        case SPRM::sprmCSizePos:
            // The hps sprms would be quite hard to implement in a sane way
            wvlog << "Warning: sprmCSizePos not implemented" << endl;
            break;
        case SPRM::sprmCDxaSpace:
            dxaSpace = readS16( ptr );
            break;
        case SPRM::sprmCLid:
            // We are abusing this SPRM for Word 6 purposes (sprmCLid, 97)
            lidDefault = lidFE = lid = readU16( ptr );
            //wvlog << "Error: sprmCLid only used internally in MS Word" << endl;
            break;
        case SPRM::sprmCIco: {
            U16 ico = *ptr;
            cv=Word97::icoToRGB(ico);
            break;
        }
        case SPRM::sprmCCv: {
            U8 r,g,b,k;

            r=readU8(ptr);
            ptr+=sizeof(U8);
            g=readU8(ptr);
            ptr+=sizeof(U8);
            b=readU8(ptr);
            ptr+=sizeof(U8);
            k=readU8(ptr);
            ptr+=sizeof(U8);
            cv=(k<<24)|(r<<16)|(g<<8)|(b);
            break;
        }
        case SPRM::sprmCHps:
            hps = readU16( ptr );
            break;
        case SPRM::sprmCHpsInc:
            // The hps sprms would be quite hard to implement in a sane way
            wvlog << "Warning: sprmCHpsInc not implemented" << endl;
            break;
        case SPRM::sprmCHpsPos:
            hpsPos = readS16( ptr );
            break;
        case SPRM::sprmCHpsPosAdj:
            // The hps sprms would be quite hard to implement in a sane way
            wvlog << "Warning: sprmCHpsPosAdj not implemented" << endl;
            break;
        case SPRM::sprmCMajority:
        case SPRM::sprmCMajority50: // same as sprmCMajority
        {
            CHP tmpChp;
            tmpChp.ftc = 4; // the rest is default, looks a bit strange
            tmpChp.apply( ptr + 1, *ptr, paragraphStyle, styleSheet, dataStream, version );
            if ( paragraphStyle ) {
                const CHP& pstyle( paragraphStyle->chp() );
                if ( tmpChp.fBold == fBold )
                    fBold = pstyle.fBold;
                if ( tmpChp.fItalic == fItalic )
                    fItalic = pstyle.fItalic;
                if ( tmpChp.fStrike == fStrike )
                    fStrike = pstyle.fStrike;
                if ( tmpChp.fOutline == fOutline )
                    fOutline = pstyle.fOutline;
                if ( tmpChp.fShadow == fShadow )
                    fShadow = pstyle.fShadow;
                if ( tmpChp.fSmallCaps == fSmallCaps )
                    fSmallCaps = pstyle.fSmallCaps;
                if ( tmpChp.fCaps == fCaps )
                    fCaps = pstyle.fCaps;
                if ( tmpChp.ftc == ftc )
                    ftc = pstyle.ftc;
                if ( tmpChp.hps == hps )
                    hps = pstyle.hps;
                if ( tmpChp.hpsPos == hpsPos )
                    hpsPos = pstyle.hpsPos;
                if ( tmpChp.kul == kul )
                    kul = pstyle.kul;
                if ( tmpChp.dxaSpace == dxaSpace ) // qpsSpace???
                    dxaSpace = pstyle.dxaSpace;
                if ( tmpChp.cv == cv )
                    cv = pstyle.cv;
            }
            else
                wvlog << "Warning: sprmCMajority couldn't find a style" << endl;
            break;
        }
        case SPRM::sprmCIss:
            iss = *ptr;
            break;
        case SPRM::sprmCHpsNew50:
            if ( *ptr != 2 )
                wvlog << "Warning: sprmCHpsNew50 has a different length than 2" << endl;
            else
                hps = readU16( ptr + 1 );
            break;
        case SPRM::sprmCHpsInc1:
            // The hps sprms would be quite hard to implement in a sane way
            wvlog << "Warning: sprmCHpsInc1 not implemented" << endl;
            break;
        case SPRM::sprmCHpsKern:
            hpsKern = readU16( ptr );
            break;
        case SPRM::sprmCHpsMul:
            hps = static_cast<int>( static_cast<double>( hps ) * ( 1.0 + static_cast<double>( readS16( ptr ) ) / 100.0 ) );
            break;
        case SPRM::sprmCYsri:
            // Undocumented, no idea if that's implemented correctly
            ysr = *ptr;
            chYsr = *( ptr + 1 );
            break;
        case SPRM::sprmCRgFtc0:
            ftcAscii = readS16( ptr );
            break;
        case SPRM::sprmCRgFtc1:
            ftcFE = readS16( ptr );
            break;
        case SPRM::sprmCRgFtc2:
            ftcOther = readS16( ptr );
            break;
        case SPRM::sprmCCharScale:
            wCharScale = readU16( ptr ); // undocumented, but should be okay
            break;
        case SPRM::sprmCFDStrike:
            fDStrike = *ptr == 1;
            break;
        case SPRM::sprmCFImprint:
            if ( *ptr < 128 )
                fImprint = *ptr == 1;
            else if ( *ptr == 128 && paragraphStyle )
                fImprint = paragraphStyle->chp().fImprint;
            else if ( *ptr == 129 && paragraphStyle )
                fImprint = !( paragraphStyle->chp().fImprint );
            else
                wvlog << "Warning: sprmCFImprint couldn't find a style" << endl;
            break;
        case SPRM::sprmCFSpec:
            fSpec = *ptr == 1;
            break;
        case SPRM::sprmCFObj:
            fObj = *ptr == 1;
            break;
        case SPRM::sprmCPropRMark:
            if ( *ptr != 7 )
                wvlog << "Error: sprmCPropRMark has an unexpected size" << endl;
            fPropMark = *( ptr + 1 ) == 1;
            ibstPropRMark = readS16( ptr + 2 );
            dttmPropRMark.readPtr( ptr + 4 );
            break;
        case SPRM::sprmCFEmboss:
            if ( *ptr < 128 )
                fEmboss = *ptr == 1;
            else if ( *ptr == 128 && paragraphStyle )
                fEmboss = paragraphStyle->chp().fEmboss;
            else if ( *ptr == 129 && paragraphStyle )
                fEmboss = !( paragraphStyle->chp().fEmboss );
            else
                wvlog << "Warning: sprmCFEmboss couldn't find a style" << endl;
            break;
        case SPRM::sprmCSfxText:
            sfxtText = *ptr;
            break;
        // All the BiDi flags below aren't documented. The question is whether we should
        // add some BiDi versions of e.g. fBold and interpret these sprms here like plain
        // sprmCFBold. For now I just ignore them, as the only user of wv2 is KWord, and
        // KWord is intelligent enough to support BiDi "the right way." (Werner)
        case SPRM::sprmCFBiDi:
            // ###### Undocumented
            //wvlog << "Warning: sprmCFBiDi not implemented" << endl;
            break;
        case SPRM::sprmCFDiacColor:
            // ###### Undocumented
            //wvlog << "Warning: sprmCFDiacColor not implemented" << endl;
            break;
        case SPRM::sprmCFBoldBi:
            // ###### Undocumented
            //wvlog << "Warning: sprmCFBoldBi not implemented" << endl;
            break;
        case SPRM::sprmCFItalicBi:
            // ###### Undocumented
            //wvlog << "Warning: sprmCFItalicBi not implemented" << endl;
            break;
        case SPRM::sprmCFtcBi:
            // ###### Undocumented
            //wvlog << "Warning: sprmCFtcBi not implemented" << endl;
            break;
        case SPRM::sprmCLidBi:
            // OOo does something with that flag.
            wvlog << "Warning: sprmCLidBi not implemented (no documentation available)" << endl;
            break;
        case SPRM::sprmCIcoBi:
            // ###### Undocumented
            //wvlog << "Warning: sprmCIcoBi not implemented" << endl;
            break;
        case SPRM::sprmCHpsBi:
            // OOo does something with that flag.
            wvlog << "Warning: sprmCHpsBi not implemented (no documentation available)" << endl;
            break;
        case SPRM::sprmCDispFldRMark:
        {
            if ( *ptr != 39 )
                wvlog << "Warning: sprmCDispFldRMark has a different length than 39" << endl;
            else {
                fDispFldRMark = *( ptr + 1 ) == 1;
                ibstDispFldRMark = readS16( ptr + 2 );
                dttmPropRMark.readPtr( ptr + 4 );
                for ( int i = 0; i < 16; ++i )
                    xstDispFldRMark[ i ] = readU16( ptr + 8 + i * sizeof( U16 ) );
            }
            break;
        }
        case SPRM::sprmCShd:
            ptr++;
            shd.read90Ptr( ptr );
            break;
        case SPRM::sprmCIbstRMarkDel:
            ibstRMarkDel = readS16( ptr );
            break;
        case SPRM::sprmCDttmRMarkDel:
            dttmRMarkDel.readPtr( ptr );
            break;
        case SPRM::sprmCBrc:
            readBRC( brc, ptr, version );
            break;
        case SPRM::sprmCShd80:
            shd.readPtr( ptr );
            break;
        case SPRM::sprmCIdslRMarkDel:
            idslRMReasonDel = readS16( ptr );
            break;
        case SPRM::sprmCFUsePgsuSettings:
            fUsePgsuSettings = *ptr == 1;
            break;
        case SPRM::sprmCCpg:
            // Undocumented, no idea what this variable is for. I changed it to chse in
            // the spec as it made more sense. (Werner)
            //wvlog << "Warning: sprmCCpg not implemented" << endl;
            break;
        case SPRM::sprmCRgLid0:
        case SPRM::sprmCRgLidUndocumented1: // according to OOo a dup. of sprmCRgLid0
            lidDefault = readU16( ptr );
            break;
        case SPRM::sprmCRgLid1:
            lidFE = readU16( ptr );
            break;
        case SPRM::sprmCIdctHint:
            idct = *ptr;
            break;
        // Fall-through intended
        case SPRM::sprmCUndocumented1:
        case SPRM::sprmCUndocumented2:
            break;  // They are not documented but they are skipped correctly
        case SPRM::sprmCFELayout:
        {
            //skipping cb
            U16 ufel = readU16( ptr + sizeof(U8) );
            fTNY = ufel;
            //skip to compress;
            ufel >>= 12;
            fTNYCompress = ufel;
            break;
        }
        default:
            wvlog << "Huh? None of the defined sprms matches 0x" << hex << sprm << dec << "... trying to skip anyway" << endl;
            break;
    }
    return static_cast<S16>( sprmLength );  // length of the SPRM
}


// Apply a PICF grpprl of a given size ("count" bytes long)
void PICF::apply( const U8* grpprl, U16 count, const Style* style, const StyleSheet* styleSheet,
                  OLEStreamReader* dataStream, WordVersion version )
{
    // There should be only PICF sprms in the grpprl we get
    SPRM::apply<PICF>( this, &PICF::applyPICFSPRM, grpprl, count, style, styleSheet, dataStream, version );
}

void PICF::applyExceptions(const U8* /*exceptions*/, const StyleSheet* /*stylesheet*/, OLEStreamReader* /*dataStream*/, WordVersion /*version*/ )
{
    // ### CHECK: Do we need that at all?
}

// Returns -1 if this wasn't a PICF sprm and it returns the length
// of the applied sprm if it was successful
S16 PICF::applyPICFSPRM( const U8* ptr, const Style* /*style*/, const StyleSheet* /*styleSheet*/,
                         OLEStreamReader* /*dataStream*/, WordVersion version )
{
    U16 sprmLength;
    const U16 sprm( getSPRM( &ptr, version, sprmLength ) );

    // Is it a PICF sprm?
    if ( ( ( sprm & 0x1C00 ) >> 10 ) != 3 ) {
        wvlog << "Warning: You're trying to apply a non PICF sprm to a PICF." << endl;
        return -1;
    }
    // which one? ;)
    switch ( sprm ) {
        case SPRM::sprmNoop:
            wvlog << "Huh? Found a sprmNoop..." << endl;
            break;
        case SPRM::sprmPicBrcl:
            brcl = *ptr;
            break;
        case SPRM::sprmPicScale:
            if ( *ptr != 12 )
                wvlog << "Warning: sprmPicScale has a different size (" << static_cast<int>( *ptr )
                      << ") than expected (12)." << endl;
            mx = readU16( ptr + 1 );
            my = readU16( ptr + 3 );
            dxaCropLeft = readU16( ptr + 5 );
            dyaCropTop = readU16( ptr + 7 );
            dxaCropRight = readU16( ptr + 9 );
            dyaCropBottom = readU16( ptr + 11 );
            break;
        case SPRM::sprmPicBrcTop:
            readBRC( brcTop, ptr, version );
            break;
        case SPRM::sprmPicBrcLeft:
            readBRC( brcLeft, ptr, version );
            break;
        case SPRM::sprmPicBrcBottom:
            readBRC( brcBottom, ptr, version );
            break;
        case SPRM::sprmPicBrcRight:
            readBRC( brcRight, ptr, version );
            break;
        default:
            wvlog << "Huh? None of the defined sprms matches 0x" << hex << sprm << dec << "... trying to skip anyway" << endl;
            break;
    }
    return static_cast<S16>( sprmLength );  // length of the SPRM
}


// Apply a SEP grpprl of a given size ("count" bytes long)
void SEP::apply( const U8* grpprl, U16 count, const Style* style, const StyleSheet* styleSheet,
                 OLEStreamReader* dataStream, WordVersion version )
{
    // There should be only SEP sprms in the grpprl we get
    SPRM::apply<SEP>( this, &SEP::applySEPSPRM, grpprl, count, style, styleSheet, dataStream, version );
}

void SEP::applyExceptions( const U8* exceptions, const StyleSheet* styleSheet, OLEStreamReader* dataStream, WordVersion version )
{
    if ( exceptions == 0 )
        return;
    U16 cb = readU16( exceptions );
    exceptions += 2;
    apply( exceptions, cb, 0, styleSheet, dataStream, version );
}

// Returns -1 if this wasn't a SEP sprm and it returns the length
// of the applied sprm if it was successful
S16 SEP::applySEPSPRM( const U8* ptr, const Style* /*style*/, const StyleSheet* /*styleSheet*/, OLEStreamReader* /*dataStream*/, WordVersion version )
{
    U16 sprmLength;
    const U16 sprm( getSPRM( &ptr, version, sprmLength ) );

    // Is it a SEP sprm?
    if ( ( ( sprm & 0x1C00 ) >> 10 ) != 4 ) {
        wvlog << "Warning: You're trying to apply a non SEP sprm to a SEP." << endl;
        return -1;
    }
    // which one? ;)
    switch ( sprm ) {
        case SPRM::sprmNoop:
            wvlog << "Huh? Found a sprmNoop..." << endl;
            break;
        case SPRM::sprmScnsPgn:
            cnsPgn = *ptr;
            break;
        case SPRM::sprmSiHeadingPgn:
            iHeadingPgn = *ptr;
            break;
        case SPRM::sprmSOlstAnm:
            if ( version == Word8 )
                olstAnm.readPtr( ptr + 1 ); // variable length, skip length byte
            else
                olstAnm = toWord97( Word95::OLST( ptr + 1 ) );
            break;
        case SPRM::sprmSDxaColWidth:
            wvlog << "Warning: sprmSDxaColWidth not implemented" << endl;
            break;
        case SPRM::sprmSDxaColSpacing:
            wvlog << "Warning: sprmSDxaColSpacing not implemented" << endl;
            break;
        case SPRM::sprmSFEvenlySpaced:
            fEvenlySpaced = *ptr == 1;
            break;
        case SPRM::sprmSFProtected:
            fUnlocked = *ptr == 1;
            break;
        case SPRM::sprmSDmBinFirst:
            dmBinFirst = readU16( ptr );
            break;
        case SPRM::sprmSDmBinOther:
            dmBinOther = readU16( ptr );
            break;
        case SPRM::sprmSBkc:
            bkc = *ptr;
            break;
        case SPRM::sprmSFTitlePage:
            fTitlePage = *ptr == 1;
            break;
        case SPRM::sprmSCcolumns:
            ccolM1 = readS16( ptr );
            break;
        case SPRM::sprmSDxaColumns:
            dxaColumns = readS16( ptr );
            break;
        case SPRM::sprmSFAutoPgn:
            fAutoPgn = *ptr == 1;
            break;
        case SPRM::sprmSNfcPgn:
            nfcPgn = *ptr;
            break;
        case SPRM::sprmSDyaPgn:
            dyaPgn = readS16( ptr );
            break;
        case SPRM::sprmSDxaPgn:
            dxaPgn = readS16( ptr );
            break;
        case SPRM::sprmSFPgnRestart:
            fPgnRestart = *ptr == 1;
            break;
        case SPRM::sprmSFEndnote:
            fEndNote = *ptr == 1;
            break;
        case SPRM::sprmSLnc:
            lnc = *ptr;
            break;
        case SPRM::sprmSGprfIhdt:
            grpfIhdt = *ptr;
            break;
        case SPRM::sprmSNLnnMod:
            nLnnMod = readU16( ptr );
            break;
        case SPRM::sprmSDxaLnn:
            dxaLnn = readS16( ptr );
            break;
        case SPRM::sprmSDyaHdrTop:
            dyaHdrTop = readU16( ptr );
            break;
        case SPRM::sprmSDyaHdrBottom:
            dyaHdrBottom = readU16( ptr );
            break;
        case SPRM::sprmSLBetween:
            fLBetween = *ptr == 1;
            break;
        case SPRM::sprmSVjc:
            vjc = *ptr;
            break;
        case SPRM::sprmSLnnMin:
            lnnMin = readS16( ptr );
            break;
        case SPRM::sprmSPgnStart:
            pgnStart = readU16( ptr );
            break;
        case SPRM::sprmSBOrientation:
            dmOrientPage = *ptr;
            break;
        case SPRM::sprmSBCustomize:
            wvlog << "Warning: sprmSBCustomize not implemented" << endl;
            break;
        case SPRM::sprmSXaPage:
            xaPage = readU16( ptr );
            break;
        case SPRM::sprmSYaPage:
            yaPage = readU16( ptr );
            break;
        case SPRM::sprmSDxaLeft:
            dxaLeft = readU16( ptr );
            break;
        case SPRM::sprmSDxaRight:
            dxaRight = readU16( ptr );
            break;
        case SPRM::sprmSDyaTop:
            dyaTop = readU16( ptr );
            break;
        case SPRM::sprmSDyaBottom:
            dyaBottom = readU16( ptr );
            break;
        case SPRM::sprmSDzaGutter:
            dzaGutter = readU16( ptr );
            break;
        case SPRM::sprmSDmPaperReq:
            dmPaperReq = readU16( ptr );
            break;
        case SPRM::sprmSPropRMark:
            fPropRMark = *( ptr + 1 );
            ibstPropRMark = readS16( ptr + 2 );
            dttmPropRMark.readPtr( ptr + 4 );
            break;
        case SPRM::sprmSFBiDi:
            wvlog << "Warning: sprmSFBiDi not implemented" << endl;
            break;
        case SPRM::sprmSFFacingCol:
            wvlog << "Warning: sprmSFFacingCol not implemented" << endl;
            break;
        case SPRM::sprmSFRTLGutter:
            wvlog << "Warning: sprmSFRTLGutter not implemented" << endl;
            break;
        case SPRM::sprmSBrcTop:
            readBRC( brcTop, ptr, version );
            break;
        case SPRM::sprmSBrcLeft:
            readBRC( brcLeft, ptr, version );
            break;
        case SPRM::sprmSBrcBottom:
            readBRC( brcBottom, ptr, version );
            break;
        case SPRM::sprmSBrcRight:
            readBRC( brcRight, ptr, version );
            break;
        case SPRM::sprmSPgbProp:
        {
            U16 pgbProp = readU16( ptr );
            pgbApplyTo = pgbProp;
            pgbProp >>= 3;
            pgbPageDepth = pgbProp;
            pgbProp >>= 2;
            pgbOffsetFrom = pgbProp;
            pgbProp >>= 3;
            unused74_8 = pgbProp;
            break;
        }
        case SPRM::sprmSDxtCharSpace:
            dxtCharSpace = readS32( ptr );
            break;
        case SPRM::sprmSDyaLinePitch:
            dyaLinePitch = readS32( ptr );
            break;
        case SPRM::sprmSClm:
            clm = readU16( ptr );
            break;
        case SPRM::sprmSTextFlow:
            wTextFlow = readU16( ptr );
            break;
        default:
            wvlog << "Huh? None of the defined sprms matches 0x" << hex << sprm << dec << "... trying to skip anyway" << endl;
            break;
    }
    return static_cast<S16>( sprmLength );  // length of the SPRM
}


// Apply a TAP grpprl (or at least the TAP properties of a PAP/TAP grpprl)
// of a given size ("count" bytes long)
void TAP::apply( const U8* grpprl, U16 count, const Style* style, const StyleSheet* styleSheet,
                 OLEStreamReader* dataStream, WordVersion version )
{
    // There should be mostly TAP sprms in the grpprl we get, and we
    // have to ignore the remaining PAP sprms, just what the template does
    SPRM::apply<TAP>( this, &TAP::applyTAPSPRM, grpprl, count, style, styleSheet, dataStream, version );
}

void TAP::applyExceptions( const U8* /*exceptions*/, const StyleSheet* /*stylesheet*/, OLEStreamReader* /*dataStream*/, WordVersion /*version*/ )
{
    // ### TODO -- is that needed at all?
}

namespace
{
    void cropIndices( U8& itcFirst, U8& itcLim, U8 size )
    {
        if ( itcFirst >= size ) {
            wvlog << "Warning: itcFirst out of bounds" << endl;
            itcFirst = size - 1;
        }
        if ( itcLim > size ) {
            wvlog << "Warning: itcLim out of bounds" << endl;
            itcLim = size;
        }
    }
}

// Returns -1 if this wasn't a TAP sprm and it returns the length
// of the applied sprm if it was successful
S16 TAP::applyTAPSPRM( const U8* ptr, const Style* style, const StyleSheet* styleSheet, OLEStreamReader* dataStream, WordVersion version )
{
    U16 sprmLength;
    const U16 sprm( getSPRM( &ptr, version, sprmLength ) );

#ifdef WV2_DEBUG_SPRMS
    wvlog << "sprm: 0x" << hex << sprm << dec << endl;
    wvlog << "sprmLength: " << sprmLength << endl;
#endif
    // Is it a TAP sprm? Not really an error if it's none, as all TAP sprms live
    // inside PAP grpprls
    if ( ( ( sprm & 0x1C00 ) >> 10 ) != 5 && 
         (sprm != SPRM::sprmPHugePapx) &&
         (sprm != SPRM::sprmPHugePapx2) &&
         (sprm != SPRM::sprmPTableProps) )
    {
#ifdef WV2_DEBUG_SPRMS
        wvlog << "Warning: You're trying to apply a non TAP sprm to a TAP. Not necessarily bad." 
              << endl;
#endif
        return -1;
    }
    // which one? ;)
    switch ( sprm ) {
    case SPRM::sprmNoop:
        wvlog << "Huh? Found a sprmNoop..." << endl;
        break;
    case SPRM::sprmTJc:
        jc = readS16( ptr );
        break;
    case SPRM::sprmTDxaLeft:
    {
//         const S16 dxaNew = readS16( ptr ) - ( rgdxaCenter[ 0 ] + dxaGapHalf );
//         std::transform( rgdxaCenter.begin(), rgdxaCenter.end(), rgdxaCenter.begin(), 
//                         std::bind1st( std::plus<S16>(), dxaNew ) );

        //NOTE: calculation of the horizontal origin moved to TableHandler
        dxaLeft = readS16( ptr );
#ifdef WV2_DEBUG_SPRMS
        wvlog << "dxaLeft: " << dxaLeft << endl;
#endif
        break;
    }
    case SPRM::sprmTDxaGapHalf:
    {
//         const S16 dxaGapHalfNew = readS16( ptr );
//         if ( !rgdxaCenter.empty() ) {
//             rgdxaCenter[ 0 ] += dxaGapHalf - dxaGapHalfNew;
//         }

        //NOTE: calculation of the horizontal origin moved to TableHandler
        dxaGapHalf = readS16( ptr );
#ifdef WV2_DEBUG_SPRMS
        wvlog << "dxaGapHalf: " << dxaGapHalf << endl;
#endif
        break;
    }
    case SPRM::sprmTFCantSplit:
        fCantSplit = *ptr == 1;
        break;
    case SPRM::sprmTTableHeader:
        fTableHeader = *ptr == 1;
        break;
    case SPRM::sprmTTableBorders80:
    {
        const U8 inc = version == Word8 ? Word97::BRC::sizeOf97 : Word95::BRC::sizeOf;
        for ( int i = 0; i < 6; ++i )
        {
            // skip the leading size byte
            readBRC( rgbrcTable[ i ], ptr + 1 + i * inc, version );
        }
        break;
    }
    case SPRM::sprmTTableBorders:
    {
        const U8 inc = version == Word8 ? Word97::BRC::sizeOf : Word95::BRC::sizeOf;
        for ( int i = 0; i < 6; ++i )
            // skip the leading size byte
            rgbrcTable[ i ].read90Ptr( ptr + 1 + i * inc );
        break;
    }
    case SPRM::sprmTDefTable10:
        wvlog << "Warning: sprmTDefTable10 is obsolete" << endl;
        break;
    case SPRM::sprmTDyaRowHeight:
        dyaRowHeight = readS16( ptr );
        break;
    case SPRM::sprmTDefTable:
    {
        if ( itcMac != 0 ) {
            wvlog << "Bug: Assumption about sprmTDefTable not true" << endl;
            break;
        }
        int remainingLength = readU16( ptr ) - 1;
        itcMac = *( ptr + 2 );

        remainingLength -= 2 * ( itcMac + 1 );
        if ( remainingLength < 0 ) {
            wvlog << "Bug: Not even enough space for the dxas!" << endl;
            break;
        }
        const U8* myPtr = ptr + 3;
        const U8* myLim = ptr + 3 + 2 * ( itcMac + 1 );
        while ( myPtr < myLim ) {
            rgdxaCenter.push_back( readS16( myPtr ) );
            myPtr += 2;
        }
        wvlog << "rgdxaCenter[0]: " << rgdxaCenter[0] << endl;

        const int tcSize = version == Word8 ? Word97::TC::sizeOf : Word95::TC::sizeOf;
        myLim = myPtr + ( remainingLength / tcSize ) * tcSize;
        while ( myPtr < myLim ) {
            if ( version == Word8 ) {
                rgtc.push_back( TC( myPtr ) );
            }
            else {
                rgtc.push_back( toWord97( Word95::TC( myPtr ) ) );
            }
            myPtr += tcSize;
        }
        if ( rgtc.size() < static_cast<std::vector<S16>::size_type>( itcMac ) ) {
            rgtc.insert( rgtc.end(), itcMac - rgtc.size(), TC() );
        }
        //default shading
        rgshd.insert( rgshd.begin(), itcMac, SHD() );
        break;
    }
    case SPRM::sprmTDefTableShd80:
    {
        const U8* myPtr = ptr + 1;
        const U8* myLim = ptr + 1 + *ptr;
        rgshd.clear();
        while ( myPtr < myLim ) {
            rgshd.push_back( SHD( myPtr ) );
            myPtr += SHD::sizeOf;
        }
        if ( rgshd.size() < static_cast<std::vector<S16>::size_type>( itcMac ) ) {
            rgshd.insert( rgshd.end(), itcMac - rgshd.size(), SHD() );
        }
        break;
    }
    case SPRM::sprmTDefTableShd:
    {
        const U8* myPtr = ptr + 1;
        const U8* myLim = ptr + 1 + *ptr;
        rgshd.clear();
        while ( myPtr < myLim ) {
            SHD shd;
            shd.read90Ptr(myPtr);
            rgshd.push_back( shd );
            myPtr += SHD::sizeOf;
        }
        if ( rgshd.size() < static_cast<std::vector<S16>::size_type>( itcMac ) ) {
            rgshd.insert( rgshd.end(), itcMac - rgshd.size(), SHD() );
        }
        break;
    }
    case SPRM::sprmTDefTableShd2nd:
    {
        break;
    }
    case SPRM::sprmTDefTableShd3rd:
    {
        break;
    }
    case SPRM::sprmTTlp:
        tlp.readPtr( ptr );
        break;
    case SPRM::sprmTFBiDi:
        wvlog << "Warning: sprmTFBiDi not implemented" << endl;
        break;
    case SPRM::sprmTHTMLProps:
        wvlog << "Warning: sprmTHTMLProps not implemented" << endl;
        break;
    case SPRM::sprmTSetBrc80:
    {
        const U8* myPtr( version == Word8 ? ptr + 1 : ptr );  // variable size byte for Word 8!
        U8 itcFirst = *myPtr;
        U8 itcLim = *( myPtr + 1 );
        cropIndices( itcFirst, itcLim, rgtc.size() );
        const U8 flags = *( myPtr + 2 );
        BRC brc;
        readBRC( brc, myPtr + 3, version );

        for ( ; itcFirst < itcLim; ++itcFirst ) {
            if ( flags & 0x01 )
                rgtc[ itcFirst ].brcTop = brc;
            if ( flags & 0x02 )
                rgtc[ itcFirst ].brcLeft = brc;
            if ( flags & 0x04 )
                rgtc[ itcFirst ].brcBottom = brc;
            if ( flags & 0x08 )
                rgtc[ itcFirst ].brcRight = brc;
        }
        break;
    }
    case SPRM::sprmTInsert:
    {
        const U8 itcFirst = *ptr;
        const U8 ctc = *( ptr + 1 );
        const U16 dxaCol = readU16( ptr + 2 );

        //NOTE: don't process before sprmTDefTable
        if (itcMac) {
            // Sanity check
            if ( static_cast<std::vector<S16>::size_type>( itcMac ) + 1 != rgdxaCenter.size() ) {
                wvlog << "Bug: Somehow itcMac and the rgdxaCenter.size() aren't in sync anymore!" << endl;
                itcMac = rgdxaCenter.size() - 1;
            }

            if ( itcMac < itcFirst ) {
                // Shaky, no idea why we would have to subtract ctc here?  The
                // implementation below is a guess from me, but it looks like
                // this never happens in real documents.
                wvlog << "Warning: sprmTInsert: Debug me ########################################" << endl;

                S16 runningDxaCol = 0;
                if ( !rgdxaCenter.empty() ) {
                    runningDxaCol = rgdxaCenter.back();
                }

                for ( ; itcMac < itcFirst; ++itcMac ) {
                    // If the index is bigger than our current array we just
                    // fill the hole with dummy cells (dxaCol wide, default TC)
                    // as suggested in the documentation
                    runningDxaCol += dxaCol;
                    rgdxaCenter.push_back( runningDxaCol );
                    rgtc.push_back( TC() );
                }
            }
            else {
                S16 runningDxaCol;
                if ( itcFirst > 0 ) {
                    runningDxaCol = rgdxaCenter[ itcFirst - 1 ] + dxaCol;
                }
                // preserve the position of the table row 
                else { 
                    runningDxaCol = rgdxaCenter[ 0 ];
                }
                for ( int i = 0; i < ctc; ++i ) {
                    std::vector<S16>::iterator dxaIt = rgdxaCenter.begin() + itcFirst + i;
                    rgdxaCenter.insert( dxaIt, runningDxaCol );
                    runningDxaCol += dxaCol;
                }

                rgtc.insert( rgtc.begin() + itcFirst, ctc, TC() );
                rgshd.insert( rgshd.begin() + itcFirst, ctc, SHD() );
                itcMac += ctc;

                // Adjust all successive items (+= ctc * dxaCol)
                std::transform( rgdxaCenter.begin() + itcFirst + ctc, rgdxaCenter.end(),
                                rgdxaCenter.begin() + itcFirst + ctc, 
                                std::bind1st( std::plus<S16>(), ctc * dxaCol ) );
            }
        }
        break;
    }
    case SPRM::sprmTDelete:
    {
        U8 itcFirst = *ptr;
        U8 itcLim = *( ptr + 1 );
        cropIndices( itcFirst, itcLim, rgdxaCenter.size() );

        rgdxaCenter.erase( rgdxaCenter.begin() + itcFirst, rgdxaCenter.begin() + itcLim );
        rgtc.erase( rgtc.begin() + itcFirst, rgtc.begin() + itcLim );
        rgshd.erase( rgshd.begin() + itcFirst, rgshd.begin() + itcLim );

        itcMac -= itcLim - itcFirst;
        break;
    }
    case SPRM::sprmTDxaCol:
    {
        U8 itcFirst = *ptr;
        U8 itcLim = *( ptr + 1 );
        const S16 dxaCol = readS16( ptr + 2 );

        //NOTE: don't process before sprmTDefTable
        if (itcMac) {
            cropIndices( itcFirst, itcLim, rgdxaCenter.size() );
            S16 shift = 0;
            for ( ; itcFirst < itcLim; ++itcFirst ) {
                shift += rgdxaCenter[ itcFirst + 1 ] - rgdxaCenter[ itcFirst ] - dxaCol;
                rgdxaCenter[ itcFirst + 1 ] = rgdxaCenter[ itcFirst ] + dxaCol;
            }
            // Adjust all the following columns
            ++itcFirst;
            std::transform( rgdxaCenter.begin() + itcFirst, rgdxaCenter.end(),
                            rgdxaCenter.begin() + itcFirst, 
                            std::bind2nd( std::minus<S16>(), shift ) );
        }
        break;
    }
    case SPRM::sprmTMerge:
    {
        wvlog << "Debug me (sprmTMerge) #########################################################" << endl;
        U8 itcFirst = *ptr;
        U8 itcLim = *( ptr + 1 );
        cropIndices( itcFirst, itcLim, rgtc.size() );

        rgtc[ itcFirst++ ].fFirstMerged = 1;
        for ( ; itcFirst < itcLim; ++itcFirst ) {
            rgtc[ itcFirst ].fMerged = 1;
        }
        break;
    }
    case SPRM::sprmTSplit:
    {
        wvlog << "Debug me (sprmTSplit) #########################################################" << endl;
        U8 itcFirst = *ptr;
        U8 itcLim = *( ptr + 1 );
        cropIndices( itcFirst, itcLim, rgtc.size() );

        std::vector<TC>::iterator it = rgtc.begin() + itcFirst;
        std::vector<TC>::const_iterator end = rgtc.begin() + itcLim;
        for ( ; it != end; ++it ) {
            ( *it ).fFirstMerged = 0;
            ( *it ).fMerged = 0;
        }
        break;
    }
    case SPRM::sprmTSetBrc10:
        wvlog << "Warning: sprmTSetBrc10 doesn't make sense for Word97 structures" << endl;
        break;
    case SPRM::sprmTSetShd:
    {
        U8 itcFirst = *ptr;
        U8 itcLim = *( ptr + 1 );
        cropIndices( itcFirst, itcLim, rgshd.size() );
        const SHD shd( ptr + 2 );

        std::fill_n( rgshd.begin() + itcFirst, itcLim - itcFirst, shd );
        break;
    }
    case SPRM::sprmTSetShdOdd:
    {
        U8 itcFirst = *ptr;
        U8 itcLim = *( ptr + 1 );
        cropIndices( itcFirst, itcLim, rgshd.size() );
        const SHD shd( ptr + 2 );

        for ( ; itcFirst < itcLim; ++itcFirst ) {
            if ( itcFirst & 0x01 ) {
                rgshd[ itcFirst ] = shd;
            }
        }
        break;
    }
    case SPRM::sprmTTextFlow:
        wvlog << "Warning: sprmTTextFlow is undocumented. Please send this document to trobin@kde.org." << endl;
        break;
    case SPRM::sprmTDiagLine:
        wvlog << "Warning: sprmTDiagLine is undocumented. Please send this document to trobin@kde.org." << endl;
        break;
    case SPRM::sprmTVertMerge:
    {
        const U8 index = *ptr;
        const U8 flags = *( ptr + 1 );
        if ( index < rgtc.size() ) {
            rgtc[ index ].fVertMerge = flags;
            rgtc[ index ].fVertRestart = ( flags & 0x02 ) >> 1;
        }
        else {
            wvlog << "Warning: sprmTVertMerge: Out of bounds access" << endl;
        }
        break;
    }
    case SPRM::sprmTVertAlign:
    {
        U8 itcFirst = *ptr;
        U8 itcLim = *( ptr + 1 );
        cropIndices( itcFirst, itcLim, rgtc.size() );
        const U8 vertAlign = *( ptr + 2 );

        for ( ; itcFirst < itcLim; ++itcFirst ) {
            rgtc[ itcFirst ].vertAlign = vertAlign;
        }
        break;
    }
    case SPRM::sprmTSetBrc:
    {
        const U8* myPtr( version == Word8 ? ptr + 1 : ptr );  // variable size byte for Word 8!
        U8 itcFirst = *myPtr;
        U8 itcLim = *( myPtr + 1 );
        cropIndices( itcFirst, itcLim, rgtc.size() );
        const U8 flags = *( myPtr + 2 );
        BRC brc;
        brc.read90Ptr(myPtr + 3 );

        for ( ; itcFirst < itcLim; ++itcFirst ) {
            if ( flags & 0x01 )
                rgtc[ itcFirst ].brcTop = brc;
            if ( flags & 0x02 )
                rgtc[ itcFirst ].brcLeft = brc;
            if ( flags & 0x04 )
                rgtc[ itcFirst ].brcBottom = brc;
            if ( flags & 0x08 )
                rgtc[ itcFirst ].brcRight = brc;
        }
        break;
    }
    case SPRM::sprmTUndocumentedSpacing:
        if ( *ptr == 6 ) {
/*                wvlog << "sprmTUndocumentedSpacing----" << endl;
                const U8 itcFirst( *( ptr + 1 ) );
                for ( int i = 0; i < 6; ++i )
                    wvlog << "    byte " << i << ": " << hex << ( int )*( ptr + i ) << dec << endl;
*/
        }
        else {
            wvlog << "Warning: sprmTUndocumentedSpacing with unusual length=" << 
                  static_cast<int>( *ptr ) << endl;
        }
        break;
    case SPRM::sprmTUndocumented8:
            // ###### TODO
            //wvlog << "sprmTUndocumented8: some undocumented spacing thingy, TODO" << endl;
        break;
    case SPRM::sprmTUndocumented1:
    case SPRM::sprmTUndocumented2:
        break;
    case SPRM::sprmTBrcTopCv:
    {
        const U8* myPtr( version == Word8 ? ptr + 1 : ptr );  // variable size byte for Word 8!
        for (int i=0 ; i < 64 && i < rgtc.size(); ++i ) {
            rgtc[ i ].brcTop.cv = ((*(myPtr + i*4))<<16)  | ((*(myPtr + 1 + i*4))<<8) | (*(myPtr + 2 + i*4)) ;
        }
        break;
    }
    case SPRM::sprmTBrcLeftCv:
    {
        const U8* myPtr( version == Word8 ? ptr + 1 : ptr );  // variable size byte for Word 8!
        for (int i=0 ; i < 64 && i < rgtc.size(); ++i ) {
            rgtc[ i ].brcLeft.cv = ((*(myPtr + i*4))<<16)  | ((*(myPtr + 1 + i*4))<<8) | (*(myPtr + 2 + i*4)) ;
        }
        break;
    }
    case SPRM::sprmTBrcRightCv:
    {
        const U8* myPtr( version == Word8 ? ptr + 1 : ptr );  // variable size byte for Word 8!
        for (int i=0 ; i < 64 && i < rgtc.size(); ++i ) {
            rgtc[ i ].brcRight.cv = ((*(myPtr + i*4))<<16)  | ((*(myPtr + 1 + i*4))<<8) | (*(myPtr + 2 + i*4)) ;
        }
        break;
    }
    case SPRM::sprmTBrcBottomCv:
    {
        const U8* myPtr( version == Word8 ? ptr + 1 : ptr );  // variable size byte for Word 8!
        for (int i=0 ; i < 64 && i < rgtc.size(); ++i ) {
            rgtc[ i ].brcBottom.cv = ((*(myPtr + i*4))<<16)  | ((*(myPtr + 1 + i*4))<<8) | (*(myPtr + 2 + i*4)) ;
        }
        break;
    }
    case SPRM::sprmTUndocumented9:
    case SPRM::sprmTUndocumented10:
    case SPRM::sprmTUndocumented11:
    case SPRM::sprmTUndocumented12:
        break;
    case SPRM::sprmTWidthIndent:
    {
        //MS-DOC, page 362 of 609
        const U8 ftsWidth = *ptr;
        const S16 wWidth = readS16( ptr + 1 );

        if ((ftsWidth == 0x01) || (ftsWidth == 0x13)) {
            wvlog << "Warning: sprmTWidthIndent: unsupported units of measurement"; 
        }  
        //value is expressed in percentage of page/table, etc.
        if (ftsWidth == 2) {
            wvlog << "Warning: sprmTWidthIndent: unsupported units of measurement"; 
        }
        //absolute width measured in twips
        else if (ftsWidth == 3) {
            widthIndent = wWidth;
#ifdef WV2_DEBUG_SPRMS
            wvlog << "widthIndent: " << widthIndent;
#endif
        }
        break; 
    }
    // These are needed in case there are table properties inside the huge papx
    case SPRM::sprmPHugePapx:
    case SPRM::sprmPHugePapx2:
    case SPRM::sprmPTableProps:
    {
        wvlog << "--> Parsing PrcData" << endl;
        if ( dataStream ) {
            dataStream->push();
            dataStream->seek( readU32( ptr ), G_SEEK_SET );

            const U16 count( dataStream->readU16() );
            U8* grpprl = new U8[ count ];

            dataStream->read( grpprl, count );
            dataStream->pop();

            apply( grpprl, count, style, styleSheet, dataStream, version );
            delete [] grpprl;
        }
        else {
            wvlog << "Error: no data stream!" << endl;
        }
        wvlog << "<-- PrcData parsed successfully" << endl;
        break;
    }
    default:
        wvlog << "Huh? None of the defined sprms matches 0x" << hex << sprm << dec << "... trying to skip anyway" << endl;
        break;
    }
    return static_cast<S16>( sprmLength );  // length of the SPRM
}


// Creates a PRM2 from a plain PRM
PRM2 PRM::toPRM2() const
{
    PRM2 prm;
    prm.fComplex = fComplex;
    prm.igrpprl = ( val << 7 ) | isprm;
    return prm;
}

} // namespace Word97
} // namespace wvWare
