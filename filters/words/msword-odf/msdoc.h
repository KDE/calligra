/* This file is part of the Calligra Suite project
   Copyright (C) 2011 Matus Uzak <matus.uzak@ixonos.com>

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

#ifndef MSDOC_H
#define MSDOC_H

/**
 * Standard sti codes - invariant identifiers for built-in styles, which MUST
 * remain the same (i.e. don't renumber them, or old files will be messed up.)
 * NOTE: sti and istd are the same for Normal and level styles
 *
 * If you want to define a new built-in style:
 *   1) Decide if you really need one--it will exist in all future versions!
 *   2) Add a new sti below.  You can take the first available slot.
 *   3) Change stiMax, and stiPapMax or stiChpMax
 *   4) Add entry to _dnsti, and the two ids's in strman.pp
 *   5) Add case in GetDefaultUpdForSti
 *   6) Change cstiMaxBuiltinDependents if necessary
 *
 * If you want to change the definition of a built-in style
 *   1) In order to make WinWord 2 documents that use the style look like they
 *      did in WinWord 2, add a case in GetDefaultUpdForSti to handle fOldDef.
 *      This definition will be used when converting WinWord 2 stylesheets.
 *   2) If you change the name of a built-in style, increment nVerBuiltInNames
 *
 * SRC: wv2/src/generator/generator_wword8.htm
 */

#define stiNormal      0     // 0x0000  Default paragraph style

#define stiLev1        1     // 0x0001
#define stiLev2        2     // 0x0002
#define stiLev3        3     // 0x0003
#define stiLev4        4     // 0x0004
#define stiLev5        5     // 0x0005
#define stiLev6        6     // 0x0006
#define stiLev7        7     // 0x0007
#define stiLev8        8     // 0x0008
#define stiLev9        9     // 0x0009
#define stiLevFirst    stiLev1
#define stiLevLast     stiLev9

#define stiIndex1      10    // 0x000A
#define stiIndex2      11    // 0x000B
#define stiIndex3      12    // 0x000C
#define stiIndex4      13    // 0x000D
#define stiIndex5      14    // 0x000E
#define stiIndex6      15    // 0x000F
#define stiIndex7      16    // 0x0010
#define stiIndex8      17    // 0x0011
#define stiIndex9      18    // 0x0012
#define stiIndexFirst  stiIndex1
#define stiIndexLast   stiIndex9

#define stiToc1        19    // 0x0013
#define stiToc2        20    // 0x0014
#define stiToc3        21    // 0x0015
#define stiToc4        22    // 0x0016
#define stiToc5        23    // 0x0017
#define stiToc6        24    // 0x0018
#define stiToc7        25    // 0x0019
#define stiToc8        26    // 0x001A
#define stiToc9        27    // 0x001B
#define stiTocFirst    stiToc1
#define stiTocLast     stiToc9

#define stiNormIndent  28    // 0x001C
#define stiFtnText     29    // 0x001D
#define stiAtnText     30    // 0x001E
#define stiHeader      31    // 0x001F
#define stiFooter      32    // 0x0020
#define stiIndexHeading 33   // 0x0021
#define stiCaption     34    // 0x0022
#define stiToCaption   35    // 0x0023
#define stiEnvAddr     36    // 0x0024
#define stiEnvRet      37    // 0x0025
#define stiFtnRef      38    // 0x0026  char style
#define stiAtnRef      39    // 0x0027  char style
#define stiLnn         40    // 0x0028  char style
#define stiPgn         41    // 0x0029  char style
#define stiEdnRef      42    // 0x002A  char style
#define stiEdnText     43    // 0x002B
#define stiToa         44    // 0x002C
#define stiMacro       45    // 0x002D
#define stiToaHeading  46    // 0x002E
#define stiList        47    // 0x002F
#define stiListBullet  48    // 0x0030
#define stiListNumber  49    // 0x0031
#define stiList2       50    // 0x0032
#define stiList3       51    // 0x0033
#define stiList4       52    // 0x0034
#define stiList5       53    // 0x0035
#define stiListBullet2 54    // 0x0036
#define stiListBullet3 55    // 0x0037
#define stiListBullet4 56    // 0x0038
#define stiListBullet5 57    // 0x0039
#define stiListNumber2 58    // 0x003A
#define stiListNumber3 59    // 0x003B
#define stiListNumber4 60    // 0x003C
#define stiListNumber5 61    // 0x003D
#define stiTitle       62    // 0x003E
#define stiClosing     63    // 0x003F
#define stiSignature   64    // 0x0040
#define stiNormalChar  65    // 0x0041  Default char style
#define stiBodyText    66    // 0x0042
#define stiBodyText2   67    // 0x0043
#define stiListCont    68    // 0x0044
#define stiListCont2   69    // 0x0045
#define stiListCont3   70    // 0x0046
#define stiListCont4   71    // 0x0047
#define stiListCont5   72    // 0x0048
#define stiMsgHeader   73    // 0x0049
#define stiSubtitle    74    // 0x004A
#define stiSalutation  75    // 0x004B
#define stiDate        76    // 0X004C
#define stiBodyText1I  77    // 0x004D
#define stiBodyText1I2 78    // 0x004E
#define stiNoteHeading 79    // 0x004F
/* #define stiBodyText2   80    // 0x0050 */
#define stiBodyText3   81    // 0x0051
#define stiBodyTextInd2 82   // 0x0052
#define stiBodyTextInd3 83   // 0x0053
#define stiBlockQuote  84    // 0x0054
#define stiHyperlink   85    // 0x0055  char style
#define stiHyperlinkFollowed 86 // 0x0056   char style
#define stiStrong      87    // 0x0057  char style
#define stiEmphasis    88    // 0x0058  char style
#define stiNavPane     89    // 0x0059  char style
#define stiPlainText   90    // 0x005A
#define stiMax         91    // number of defined sti's

#define stiUser      0x0ffe  // user styles are distinguished by name
#define stiNil       0x0fff  // max for 12 bits

/**
 * Specifies the version number of the file format used.  Superseded by
 * FibRgCswNew.nFibNew if it is present.
 */
enum nFib
{
    Word2nFib = 0x0063,
    Word6nFib = 0x0065,
    Word7nFib = 0x0067,   //also 0x0068
    Word8nFib = 0x00c1,
    Word8nFib0 = 0x00c0,
    Word8nFib2 = 0x00c2,
    Word2knFib = 0x00D9,
    Word2k2nFib = 0x0101,
    Word2k3nFib = 0x010c, //also 0x10b
    Word2k7nFib = 0x0112
};

/**
 * Specifies the location from which the offset of a page border is measured.
 * [MS-DOC] — v20101219
 */
enum PgbOffsetFrom
{
    pgbFromText = 0x0,  //offset measured from the text
    pgbFromEdge         //offset measured from the edge of the page
};

/**
 * An unsigned integer that specifies the type of border.  Values that are
 * larger than 0x1B are not valid unless they describe a page border, in which
 * case they can be a value in the range of 0x40 to 0xE3, inclusive.
 * [MS-DOC] — v20101219
 */
enum BrcType
{
    BorderNone,                  // 0x00  No border.
    BorderSingle,                // 0x01  A single line.
    BorderDouble = 0x03,         // 0x03  A double line.
    BorderThin = 0x05,           // 0x05  A thin single solid line.
    BorderDotted,                // 0x06  A dotted border.
    BorderDashed,                // 0x07  A dashed border with large gaps between the dashes.
    BorderDotDash,               // 0x08  A border of alternating dots and dashes.
    BorderDotDotDash,            // 0x09  A border of alternating sets of two dots and one dash.
    BorderTriple,                // 0x0A  A triple line border.
    BorderThinThickSmallGap,     // 0x0B  A thin outer border and a thick inner border with a small gap between them.
    BorderThickThinSmallGap,     // 0x0C  A thin outer border and thick inner border with a small gap between them.
    BorderThinThickThinSmallGap, // 0x0D  A thin outer border, a thick middle border, and a thin inner border with a small gap between them.
    BorderThinThickMediumGap,    // 0x0E  A thin outer border and a thick inner border with a medium gap between them.
    BorderThickThinMediumGap,    // 0x0F  A thin outer border and a thick inner border and a medium gap between them.
    BorderThinThickThinMediumGap,// 0x10  A thin outer border, a thick middle border, and a thin inner border with a medium gaps between them.
    BorderThinThickLargeGap,     // 0x11  A thick outer border and a thin inner border with a large gap between them.
    BorderThickThinLargeGap,     // 0x12  A thin outer border and a thick inner border with a large gap between them.
    BorderThinThickThinLargeGap, // 0x13  A thin outer border, a thick middle border, and a thin inner border with large gaps between them.
    BorderWave,                  // 0x14  A single wavy line.
    BorderDoubleWave,            // 0x15  A double wavy line.
    BorderDashSmallGap,          // 0x16  A dashed border with small gaps between the dashes.
    BorderDashDotStroked,        // 0x17  A border consisting of alternating groups of 5 and 1 thin diagonal lines.
    BorderThreeDEmboss,          // 0x18  A thin light gray outer border, a thick medium gray middle border, and a thin black inner border with no gaps between them.
    BorderThreeDEngrave,         // 0x19  A thin black outer border, a thick medium gray middle border, and a thin light gray inner border with no gaps between them.
    BorderOutset,                // 0x1A  A thin light gray outer border and a thin medium gray inner border with a large gap between them.
    BorderInset,                 // 0x1B  A thin medium gray outer border and a thin light gray inner border with a large gap between them.
                                 // 0x40 - 0xE3  An image border.
    BorderNil = 0xFF             // 0xFF  This MUST be ignored.
};

/**
 * Specifies the type of the section break that is being described.
 * [MS-DOC] — v20101219
 */
enum SBkcOperand
{
    bkcContinuous = 0x00,
    bkcNewColumn,
    bkcNewPage,
    bkcEvenPage,
    bkcOddPage
};

/**
 * The Ipat enumeration is an index to a shading pattern.  [MS-DOC] — v20101219
 */
enum IPAT
{
    ipatAuto,             //0x0000  Clear, ST_Shd: clear
    ipatSolid,            //0x0001  Solid, ST_Shd: solid
    ipatPct5,             //0x0002  5%,    ST_Shd: pct5
    ipatPct10,            //0x0003  10%,   ST_Shd: pct10
    ipatPct20,            //0x0004  20%,   ST_Shd: pct20
    ipatPct25,            //0x0005  25%,   ST_Shd: pct25
    ipatPct30,            //0x0006  30%,   ST_Shd: pct30
    ipatPct40,            //0x0007  40%,   ST_Shd: pct40
    ipatPct50,            //0x0008  50%,   ST_Shd: pct50
    ipatPct60,            //0x0009  60%,   ST_Shd: pct60
    ipatPct70,            //0x000A  70%,   ST_Shd: pct70
    ipatPct75,            //0x000B  75%,   ST_Shd: pct75
    ipatPct80,            //0x000C  80%,   ST_Shd: pct80
    ipatPct90,            //0x000D  90%,   ST_Shd: pct90
    ipatDkHorizontal,     //0x000E  Horizontal Stripe,            ST_Shd: horzStripe
    ipatDkVertical,       //0x000F  Vertical Stripe,              ST_Shd: vertStripe
    ipatDkForeDiag,       //0x0010  Reverse Diagonal Stripe,      ST_Shd: reverseDiagStripe
    ipatDkBackDiag,       //0x0011  Diagonal Stripe,              ST_Shd: diagStripe
    ipatDkCross,          //0x0012  Horizontal Cross,             ST_Shd: horzCross
    ipatDkDiagCross,      //0x0013  Diagonal Cross,               ST_Shd: diagCross
    ipatHorizontal,       //0x0014  Thin Horizontal Stripe,       ST_Shd: thinHorzStripe
    ipatVertical,         //0x0015  Thin Vertical Stripe,         ST_Shd: thinVertStripe
    ipatForeDiag,         //0x0016  Thin Reverse Diagonal Stripe, ST_Shd: thinReverseDiagStripe
    ipatBackDiag,         //0x0017  Thin Diagonal Stripe,         ST_Shd: thinDiagStripe
    ipatCross,            //0x0018  Thin Horizontal Cross,        ST_Shd: thinHorzCross
    ipatDiagCross,        //0x0019  Thin Diagonal Cross,          ST_Shd: thinDiagCross
    ipatPctNew2 = 0x0023, //0x0023  2.5% fill pattern
    ipatPctNew7,          //0x0024  7.5% fill pattern
    ipatPctNew12,         //0x0025  12.5%,               ST_Shd: pct12
    ipatPctNew15,         //0x0026  15%,                 ST_Shd: pct15
    ipatPctNew17,         //0x0027  17.5% fill pattern
    ipatPctNew22,         //0x0028  22.5% fill pattern
    ipatPctNew27,         //0x0029  27.5% fill pattern
    ipatPctNew32,         //0x002A  32.5% fill pattern
    ipatPctNew35,         //0x002B  35%,                 ST_Shd: pct35
    ipatPctNew37,         //0x002C  37.5%,               ST_Shd: pct37
    ipatPctNew42,         //0x002D  42.5% fill pattern
    ipatPctNew45,         //0x002E  45%,                 ST_Shd: pct45
    ipatPctNew47,         //0x002F  47.5% fill pattern
    ipatPctNew52,         //0x0030  52.5% fill pattern
    ipatPctNew55,         //0x0031  55%,                 ST_Shd: pct55
    ipatPctNew57,         //0x0032  57.5% fill pattern
    ipatPctNew62,         //0x0033  62.5%,               ST_Shd: pct62
    ipatPctNew65,         //0x0034  65%,                 ST_Shd: pct65
    ipatPctNew67,         //0x0035  67.5% fill pattern
    ipatPctNew72,         //0x0036  72.5% fill pattern
    ipatPctNew77,         //0x0037  77.5% fill pattern
    ipatPctNew82,         //0x0038  82.5% fill pattern
    ipatPctNew85,         //0x0039  85%,                 ST_Shd: pct85
    ipatPctNew87,         //0x003A  87.5%,               ST_Shd: pct87
    ipatPctNew92,         //0x003B  92.5% fill pattern
    ipatPctNew95,         //0x003C  95%,                 ST_Shd: pct95
    ipatPctNew97,         //0x003D  97.5% fill pattern
    ipatNil = 0xFFFF      //0xFFFF  Nil,                 ST_Shd: nil
};

/**
 * The Kul enumeration specifies the style of underlining for text.  [MS-DOC] —
 * v20101219
 */
enum KUL
{
    kulNone,                //0x00 No underlining.
    kulSingle,              //0x01 Normal single underline.
    kulWords,               //0x02 Underline words only.
    kulDouble,              //0x03 Double underline.
    kulDotted,              //0x04 Dotted underline.
    kulThick = 0x06,        //0x06 Heavy underline.
    kulDash,                //0x07 Dashed underline.
    kulDotDash = 0x09,      //0x09 Dot-dash underline.
    kulDotDotDash,          //0x0A Dot-dot-dash underline.
    kulWavy,                //0x0B Wavy underline.
    kulDottedHeavy = 0x14,  //0x14 Heavy dotted underline.
    kulDashHeavy = 0x17,    //0x17 Heavy dashed underline.
    kulDotDashHeavy = 0x19, //0x19 Heavy dot-dash underline.
    kulDotDotDashHeavy,     //0x1A Heavy dot-dot-dash underline.
    kulWavyHeavy,           //0x1B Heavy wavy underline.
    kulDashLong = 0x27,     //0x27 Long-dash underline.
    kulWavyDouble = 0x2B,   //0x2B Wavy double underline.
    kulDashLongHeavy = 0x37 //0x37 Heavy long-dash underline.
};

/**
 * Specifies the list of numbering formats that can be used for a group of
 * automatically numbered objects.  [MS-OSHARED] — v20101219
 */
enum MSONFC
{
    msonfcArabic = 0x00, //decimal
    msonfcUCRoman,       //upperRoman
    msonfcLCRoman,       //lowerRoman
    msonfcUCLetter,      //upperLetter
    msonfcLCLetter,      //lowerLetter
    msonfcOrdinal,       //ordinal
    msonfcCardtext,      //cardinalText
    msonfcOrdtext,       //ordinalText
    msonfcHex,           //hex
    msonfcChiManSty,     //chicago
    msonfcDbNum1,        //ideographDigital
    msonfcDbNum2,        //japaneseCounting
    msonfcAiueo,         //Aiueo
    msonfcIroha,         //Iroha
    msonfcDbChar,        //decimalFullWidth
    msonfcSbChar,        //decimalHalfWidth
    msonfcDbNum3 = 0x10, //japaneseLegal
    msonfcDbNum4,        //japaneseDigitalTenThousand
    msonfcCirclenum,     //decimalEnclosedCircle
    msonfcDArabic,       //decimalFullWidth2
    msonfcDAiueo,        //aiueoFullWidth
    msonfcDIroha,        //irohaFullWidth
    msonfcArabicLZ,      //decimalZero
    msonfcBullet,        //bullet
    msonfcGanada,        //ganada
    msonfcChosung,       //chosung
    msonfcGB1,           //decimalEnclosedFullstop
    msonfcGB2,           //decimalEnclosedParen
    msonfcGB3,           //decimalEnclosedCircleChinese
    msonfcGB4,           //ideographEnclosedCircle
    msonfcZodiac1,       //ideographTraditional
    msonfcZodiac2,       //ideographZodiac
    msonfcZodiac3 = 0x20,//ideographZodiacTraditional
    msonfcTpeDbNum1,     //taiwaneseCounting
    msonfcTpeDbNum2,     //ideographLegalTraditional
    msonfcTpeDbNum3,     //taiwaneseCountingThousand
    msonfcTpeDbNum4,     //taiwaneseDigital
    msonfcChnDbNum1,     //chineseCounting
    msonfcChnDbNum2,     //chineseLegalSimplified
    msonfcChnDbNum3,     //chineseCountingThousand
    msonfcChnDbNum4,     //decimal
    msonfcKorDbNum1,     //koreanDigital
    msonfcKorDbNum2,     //koreanCounting
    msonfcKorDbNum3,     //koreanLegal
    msonfcKorDbNum4,     //koreanDigital2
    msonfcHebrew1,       //hebrew1
    msonfcArabic1,       //arabicAlpha
    msonfcHebrew2,       //hebrew2
    msonfcArabic2 = 0x30,//arabicAbjad
    msonfcHindi1,        //hindiVowels
    msonfcHindi2,        //hindiConsonants
    msonfcHindi3,        //hindiNumbers
    msonfcHindi4,        //hindiCounting
    msonfcThai1,         //thaiLetters
    msonfcThai2,         //thaiNumbers
    msonfcThai3,         //thaiCounting
    msonfcViet1,         //vietnameseCounting
    msonfcNumInDash,     //numberInDash
    msonfcLCRus,         //russianLower
    msonfcUCRus,         //russianUpper
    msonfcNone = 0xFF    //Specifies that the sequence will not display any numbering
};

/**
 * Specifies the Style Type.
 * [ECMA-376]
 */
enum ST_StyleType
{
    sgcUnknown = 0,
    sgcPara = 1,   //Paragraph style
    sgcChp = 2,    //Character style
    sgcTbl = 3,    //Table style
    sgcNmbr = 4    //Numbering style
};

/**
 * Specifies the type of alignment which is applied to the text that
 * is entered at the tab stop.
 */
enum TabJC {
    jcLeft,      //Left justification.
    jcCenter,    //Center justification.
    jcRight,     //Right justification.
    jcDecimal,   //[1]
    jcBar,       //Specifies that the current tab is a bar tab.
    jcList = 0x6 //Specifies that the current tab is a list tab.
};

//specifies the characters that are used to fill in the space which is created
//by a tab that ends at a custom tab stop.
enum TabLC {
    tlcNone,         //No leader.
    tlcDot,          //Dot leader.
    tlcHyphen,       //Dashed leader.
    tlcUnderscore,   //Underscore leader.
    tlcHeavy,        //Same as tlcUnderscore.
    tlcMiddleDot,    //Centered dot leader.
    tlcDefault = 0x7 //Same as tlcNone.
};

/*
  [1] - Specifies that the current tab stop results in a location in
  the document at which all following text is aligned around the first
  decimal separator in the following text runs.  If there is no
  decimal separator, text is aligned around the implicit decimal
  separator after the last digit of the first numeric value that
  appears in the following text.  All text runs before the first
  decimal character appear before the tab stop; all text runs after it
  appear after the tab stop location. */

/*
 * ---------------------------------------------------
 * Following enumeration are not defined in [MS-DOC]
 * ---------------------------------------------------
 */

/**
 * The HALIGN enumeration specifies horizontal alignment.
 */
enum HALIGN
{
    hAlignLeft = 0x0,
    hAlignCenter = 0x1,
    hAlignRight = 0x2
};

/**
 * The VALIGN enumeration specifies vertical alignment.
 */
enum VALIGN
{
    vAlignTop = 0x0,
    vAlignMiddle = 0x1,
    vAlignBottom = 0x2
};

#endif
