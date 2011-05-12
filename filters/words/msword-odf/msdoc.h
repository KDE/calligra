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

enum nFib
{
    Word8nFib = 0x00c1,
    Word2knFib = 0x00D9,
    Word2k2nFib = 0x0101,
    Word2k3nFib = 0x010C,
    Word2k7nFib = 0x0112
};

//Specifies the location from which the offset of a page border is measured.
//[MS-DOC] — v20101219
enum PgbOffsetFrom
{
    pgbFromText = 0x0,  //offset measured from the text
    pgbFromEdge         //offset measured from the edge of the page
};

//Specifies the type of the section break that is being described.
//[MS-DOC] — v20101219
enum SBkcOperand 
{
    bkcContinuous = 0x00, 
    bkcNewColumn,
    bkcNewPage,
    bkcEvenPage,
    bkcOddPage
};

//Specifies the list of numbering formats that can be used for a group of
//automatically numbered objects.  [MS-OSHARED] — v20101219
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

#endif
