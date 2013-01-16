/* This file is part of the Calligra Suite project
   Copyright (C) 2011 Matus Uzak <matus.uzak@ixonos.com>

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
   Boston, MA 02110-1301, USA.
*/
#ifndef MSPPT_H
#define MSPPT_H

/**
 * Specifies the slide layout of a slide.  [MS-PPT] — v20101219
 * P slide- Presentation Slide
 * PS - Placeholder Shape
 */
enum SlideLayoutType
{
    SL_TitleSlide = 0x0,    //0x00  1 title and 1 subtitle PSs.
    SL_TitleBody = 0x1,     //0x01  PSlide or main master slide layout w 1 title and 1 body PS.
    SL_MasterTitle = 0x2,   //0x02  Title master slide layout with 1 title and 1 subtitle PS.
    SL_TitleOnly = 0x07,    //0x07  PSlide layout w 1 title PS.
    SL_TwoColumns,          //0x08  PSlide layout w 1 title and 2 body PSs stacked horizontally.
    SL_TwoRows,             //0x09  PSlide layout w 1 title and 2 body PSs stacked vertically.
    SL_ColumnTwoRows,       //0x0A  PSlide layout w 1 title and 3 body PSs split into 2 columns. Right column:2 rows.
    SL_TwoRowsColumn,       //0x0B  PSlide layout w 1 title and 3 body PSs split into 2 columns. Left column:2 rows.
    SL_TwoColumnsRow = 0x0D,//0x0D  PSlide layout w 1 title and 3 body PSs split into 2 rows. Top row - 2 columns.
    SL_FourObjects,         //0x0E  PSlide layout w 1 title and 4 body PSs.
    SL_BigObject,           //0x0F  PSlide layout w 1 body PS.
    SL_Blank,               //0x10  PSlide layout w no PS.
    SL_VerticalTitleBody,   //0x11  PSlide layout w a vertical title PS on the Right and a body PS on the Left.
    SL_VerticalTwoRows      //0x12  PSlide layout w a vertical title PS (Right) and 2 body PS in 2 columns (Left).
};

/**
 * An enumeration that specifies the type of a placeholder shape.  The meaning
 * of each enumeration value is further specified in the PlaceholderAtom
 * record.  This enumeration is also used to define a slide layout as described
 * in the SlideAtom record.  [MS-PPT] — v20101219
 */
enum PlaceholderEnum
{
    PT_None,                 //0x00  No placeholder shape.
    PT_MasterTitle,          //0x01  Master title text placeholder shape.
    PT_MasterBody,           //0x02  Master body text placeholder shape.
    PT_MasterCenterTitle,    //0x03  Master center title text placeholder shape.
    PT_MasterSubTitle,       //0x04  Master sub-title text placeholder shape.
    PT_MasterNotesSlideImage,//0x05  Master notes slide image placeholder shape.
    PT_MasterNotesBody,      //0x06  Master notes body text placeholder shape.
    PT_MasterDate,           //0x07  Master date placeholder shape.
    PT_MasterSlideNumber,    //0x08  Master slide number placeholder shape.
    PT_MasterFooter,         //0x09  Master footer placeholder shape.
    PT_MasterHeader,         //0x0A  Master header placeholder shape.
    PT_NotesSlideImage,      //0x0B  Notes slide image placeholder shape.
    PT_NotesBody,            //0x0C  Notes body text placeholder shape.
    PT_Title,                //0x0D  Title text placeholder shape.
    PT_Body,                 //0x0E  Body text placeholder shape.
    PT_CenterTitle,          //0x0F  Center title text placeholder shape.
    PT_SubTitle,             //0x10  Sub-title text placeholder shape.
    PT_VerticalTitle,        //0x11  Vertical title text placeholder shape.
    PT_VerticalBody,         //0x12  Vertical body text placeholder shape.
    PT_Object,               //0x13  Object placeholder shape.
    PT_Graph,                //0x14  Graph object placeholder shape.
    PT_Table,                //0x15  Table object placeholder shape.
    PT_ClipArt,              //0x16  Clipart object placeholder shape.
    PT_OrgChart,             //0x17  Organization chart object placeholder shape.
    PT_Media,                //0x18  Media object placeholder shape.
    PT_VerticalObject,       //0x19  Vertical object placeholder shape.
    PT_Picture               //0x1A  Picture object placeholder shape.
};

/**
 * TextAutoNumberSchemeEnum
 * Referenced by: TextAutoNumberScheme
 *
 * An enumeration that specifies the character sequence and delimiters
 * to use for automatic numbering.
 */
enum TextAutoNumberSchemeEnum
{
    ANM_AlphaLcPeriod,     //0x0000  Example: a., b., c., ...Lowercase Latin character followed by a period.
    ANM_AlphaUcPeriod ,    //0x0001  Example: A., B., C., ...Uppercase Latin character followed by a period.
    ANM_ArabicParenRight,  //0x0002  Example: 1), 2), 3), ...Arabic numeral followed by a closing parenthesis.
    ANM_ArabicPeriod,      //0x0003  Example: 1., 2., 3., ...Arabic numeral followed by a period.
    ANM_RomanLcParenBoth,  //0x0004  Example: (i), (ii), (iii), ...Lowercase Roman numeral in parentheses.
    ANM_RomanLcParenRight, //0x0005  Example: i), ii), iii), ...Lowercase Roman numeral and a closing parenthesis.
    ANM_RomanLcPeriod,     //0x0006  Example: i., ii., iii., ...Lowercase Roman numeral followed by a period.
    ANM_RomanUcPeriod ,    //0x0007  Example: I., II., III., ...Uppercase Roman numeral followed by a period.
    ANM_AlphaLcParenBoth,  //0x0008  Example: (a), (b), (c), ...Lowercase alphabetic character in parentheses.
    ANM_AlphaLcParenRight, //0x0009  Example: a), b), c), ...Lowercase alphabetic character followed by a closing
    ANM_AlphaUcParenBoth,  //0x000A  Example: (A), (B), (C), ...Uppercase alphabetic character in parentheses.
    ANM_AlphaUcParenRight, //0x000B  Example: A), B), C), ...Uppercase alphabetic character followed by a closing
    ANM_ArabicParenBoth,   //0x000C  Example: (1), (2), (3), ...Arabic numeral enclosed in parentheses.
    ANM_ArabicPlain,       //0x000D  Example: 1, 2, 3, ...Arabic numeral.
    ANM_RomanUcParenBoth,  //0x000E  Example: (I), (II), (III), ...Uppercase Roman numeral in parentheses.
    ANM_RomanUcParenRight, //0x000F  Example: I), II), III), ...Uppercase Roman numeral and a closing parenthesis.
    //Future
};

/**
 * Referenced by: InteractiveInfoAtom
 *
 * An enumeration that specifies an action that can be performed when
 * interacting with an object during a slide show.
 */
enum InteractiveInfoActionEnum
{
    II_NoAction,
    II_MacroAction,
    II_RunProgramAction,
    II_JumpAction,
    II_HyperlinkAction,
    II_OLEAction,
    II_MediaAction,
    II_CustomShowAction
};

/**
 * Referenced by: InteractiveInfoAtom
 *
 * An enumeration that specifies how the action of a hyperlink is
 * interpreted. All locations are relative to the currently-displayed
 * presentation slide in the slide show.
 */
enum LinkToEnum
{
    LT_NextSlide,
    LT_PreviousSlide,
    LT_FirstSlide,
    LT_LastSlide,
    LT_CustomShow = 0x06,
    LT_SlideNumber,
    LT_Url,
    LT_OtherPresentation,
    LT_OtherFile,
    LT_Nil = 0xFF
};

#endif
