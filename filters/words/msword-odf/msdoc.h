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
    Word7nFib = 0x0067,  //also 0x0068
    Word8nFib = 0x00c1,  //also 0x00c0 and 0x00c2
    Word2knFib = 0x00D9,
    Word2k2nFib = 0x0101,
    Word2k3nFib = 0x010C,
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

#endif
