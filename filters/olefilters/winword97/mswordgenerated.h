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

    This file is a description of most structures used in the on-disk format
    of Microsoft Word 97 documents. The only structures missing are those
    which are tricky to autogenerate.
*/

#ifndef MSWORDGENERATED_H
#define MSWORDGENERATED_H

#ifndef __GNUC__
#define __attribute__(a)
#pragma pack(1)
#endif

#ifdef __DECCXX
#define __UNAL __unaligned
#else
#define __UNAL
#endif

class MsWordGenerated
{
public:
    typedef char S8;
    typedef unsigned char U8;
    static unsigned read(const U8 *in, __UNAL U8 *out, unsigned count=1);
    static unsigned read(const U8 *in, __UNAL S8 *out, unsigned count=1);

    typedef short S16;
    typedef unsigned short U16;
    static unsigned read(const U8 *in, __UNAL U16 *out, unsigned count=1);
    static unsigned read(const U8 *in, __UNAL S16 *out, unsigned count=1);

    typedef int S32;
    typedef unsigned int U32;
    static unsigned read(const U8 *in, __UNAL U32 *out, unsigned count=1);
    static unsigned read(const U8 *in, __UNAL S32 *out, unsigned count=1);

    typedef U16 XCHAR;
    // Date and Time (internal date format) (DTTM)
    typedef struct DTTM
    {

        // minutes (0-59)
        U16 mint:6;

        // hours (0-23)
        U16 hr:5;

        // days of month (1-31)
        U16 dom:5;

        // months (1-12)
        U16 mon:4;

        // years (1900-2411)-1900
        U16 yr:9;

        // weekday
        //     Sunday=0
        //     Monday=1
        //     Tuesday=2
        //     Wednesday=3
        //     Thursday=4
        //     Friday=5
        //     Saturday=6
        U16 wdy:3;

    } __attribute__ ((packed)) DTTM;
    static unsigned read(const U8 *in, __UNAL DTTM *out, unsigned count=1);

    // Shading Descriptor (SHD)
    typedef struct SHD
    {

        // foreground color (see chp.ico)
        U16 icoFore:5;

        // background color (see chp.ico)
        U16 icoBack:5;

        // shading pattern (see ipat table below)
        //     0 Automatic
        //     1 Solid
        //     2 5 Percent
        //     3 10 Percent
        //     4 20 Percent
        //     5 25 Percent
        //     6 30 Percent
        //     7 40 Percent
        //     8 50 Percent
        //     9 60 Percent
        //     10 70 Percent
        //     11 75 Percent
        //     12 80 Percent
        //     13 90 Percent
        //     14 Dark Horizontal
        //     15 Dark Vertical
        //     16 Dark Forward Diagonal
        //     17 Dark Backward Diagonal
        //     18 Dark Cross
        //     19 Dark Diagonal Cross
        //     20 Horizontal
        //     21 Vertical
        //     22 Forward Diagonal
        //     23 Backward Diagonal
        //     24 Cross
        //     25 Diagonal Cross
        //     35 2.5 Percent
        //     36 7.5 Percent
        //     37 12.5 Percent
        //     38 15 Percent
        //     39 17.5 Percent
        //     40 22.5 Percent
        //     41 27.5 Percent
        //     42 32.5 Percent
        //     43 35 Percent
        //     44 37.5 Percent
        //     45 42.5 Percent
        //     46 45 Percent
        //     47 47.5 Percent
        //     48 52.5 Percent
        //     49 55 Percent
        //     50 57.5 Percent
        //     51 62.5 Percent
        //     52 65 Percent
        //     53 67.5 Percent
        //     54 72.5 Percent
        //     55 77.5 Percent
        //     56 82.5 Percent
        //     57 85 Percent
        //     58 87.5 Percent
        //     59 92.5 Percent
        //     60 95 Percent
        //     61 97.5 Percent
        //     62 97 Percent
        U16 ipat:6;

    } __attribute__ ((packed)) SHD;
    static unsigned read(const U8 *in, __UNAL SHD *out, unsigned count=1);

    // Document Typography Info (DOPTYPOGRAPHY)
    typedef struct DOPTYPOGRAPHY
    {

        // true if we're kerning punctuation
        U16 fKerningPunct:1;

        // Kinsoku method of justification:
        //     0 = always expand
        //     1 = compress punctuation
        //     2 = compress punctuation and kana.
        U16 iJustification:2;

        // Level of Kinsoku:
        //     0 = Level 1
        //     1 = Level 2
        //     2 = Custom
        U16 iLevelOfKinsoku:2;

        // 2-page-on-1 feature is turned on.
        U16 f2on1:1;

        // reserved
        U16 unused0_6:10;

        // length of rgxchFPunct
        U16 cchFollowingPunct;

        // length of rgxchLPunct
        U16 cchLeadingPunct;

        // array of characters that should never appear at the start of a line
        XCHAR rgxchFPunct[101];

        // array of characters that should never appear at the end of a line
        XCHAR rgxchLPunct[51];
    } __attribute__ ((packed)) DOPTYPOGRAPHY;
    static unsigned read(const U8 *in, __UNAL DOPTYPOGRAPHY *out, unsigned count=1);

    // Property Modifier(variant 1) (PRM)
    typedef struct PRM
    {

        // set to 0 for variant 1
        U16 fComplex:1;

        // index to entry into rgsprmPrm
        U16 isprm:7;

        // sprm's operand
        U16 val:8;

    } __attribute__ ((packed)) PRM;
    static unsigned read(const U8 *in, __UNAL PRM *out, unsigned count=1);

    // HOIST STRUCTUREs just above here if needed to avoid forward references.

    // AnnoTation Reference Descriptor (ATRD)
    typedef struct ATRD
    {

        // pascal-style string holding initials of annotation author
        XCHAR xstUsrInitl[10];

        // index into GrpXstAtnOwners
        U16 ibst;

        // unused
        U16 ak:2;

        // unused
        U16 unused22_2:14;

        // unused
        U16 grfbmc;

        // when not -1, this tag identifies the annotation bookmark that
        // locates the range of CPs in the main document which this
        // annotation references.
        U32 lTagBkmk;
    } __attribute__ ((packed)) ATRD;
    static unsigned read(const U8 *in, __UNAL ATRD *out, unsigned count=1);

    // Autonumbered List Data Descriptor (ANLD)
    typedef struct ANLD
    {

        // number format code
        //     0 Arabic numbering
        //     1 Upper case Roman
        //     2 Lower case Roman
        //     3 Upper case Letter
        //     4 Lower case letter
        //     5 Ordinal
        U8 nfc;

        // offset into anld.rgxch that is the limit of the text that will be
        // displayed as the prefix of the autonumber text
        U8 cxchTextBefore;

        // anld.cxchTextBefore will be the beginning offset of the text in
        // the anld.rgxch that will be displayed as the suffix of an
        // autonumber. The sum of anld.cxchTextBefore + anld.cxchTextAfter
        // will be the limit of the autonumber suffix in anld.rgch
        U8 cxchTextAfter;

        // justification code
        //     0 left justify
        //     1 center
        //     2 right justify
        //     3 left and right justify
        U8 jc:2;

        // when ==1, number generated will include previous levels (used for
        // legal numbering)
        U8 fPrev:1;

        // when ==1, number will be displayed using a hanging indent
        U8 fHang:1;

        // when ==1, boldness of number will be determined by anld.fBold.
        U8 fSetBold:1;

        // when ==1, italicness of number will be determined by anld.fItalic
        U8 fSetItalic:1;

        // when ==1, anld.fSmallCaps will determine whether number will be
        // displayed in small caps or not.
        U8 fSetSmallCaps:1;

        // when ==1, anld.fCaps will determine whether number will be
        // displayed capitalized or not
        U8 fSetCaps:1;

        // when ==1, anld.fStrike will determine whether the number will be
        // displayed using strikethrough or not.
        U8 fSetStrike:1;

        // when ==1, anld.kul will determine the underlining state of the
        // autonumber.
        U8 fSetKul:1;

        // when ==1, autonumber will be displayed with a single prefixing
        // space character
        U8 fPrevSpace:1;

        // determines boldness of autonumber when anld.fSetBold == 1.
        U8 fBold:1;

        // determines italicness of autonumber when anld.fSetItalic == 1.
        U8 fItalic:1;

        // determines whether autonumber will be displayed using small caps
        // when anld.fSetSmallCaps == 1.
        U8 fSmallCaps:1;

        // determines whether autonumber will be displayed using caps when
        // anld.fSetCaps == 1.
        U8 fCaps:1;

        // determines whether autonumber will be displayed using caps when
        // anld.fSetStrike == 1.
        U8 fStrike:1;

        // determines whether autonumber will be displayed with underlining
        // when anld.fSetKul == 1.
        U8 kul:3;

        // color of autonumber
        U8 ico:5;

        // font code of autonumber
        U16 ftc;

        // font half point size (or 0=auto)
        U16 hps;

        // starting value (0 to 65535)
        U16 iStartAt;

        // width of prefix text (same as indent)
        U16 dxaIndent;

        // minimum space between number and paragraph
        U16 dxaSpace;

        // number only 1 item per table cell
        U8 fNumber1;

        // number across cells in table rows(instead of down)
        U8 fNumberAcross;

        // restart heading number on section boundary
        U8 fRestartHdn;

        // unused( should be 0)
        U8 fSpareX;

        // characters displayed before/after autonumber
        XCHAR rgxch[32];
    } __attribute__ ((packed)) ANLD;
    static unsigned read(const U8 *in, __UNAL ANLD *out, unsigned count=1);

    // Autonumber Level Descriptor (ANLV)
    typedef struct ANLV
    {

        // number format code
        //     0 Arabic numbering
        //     1 Upper case Roman
        //     2 Lower case Roman
        //     3 Upper case Letter
        //     4 Lower case letter
        //     5 Ordinal
        U8 nfc;

        // offset into anld.rgxch that is the limit of the text that will be
        // displayed as the prefix of the autonumber text
        U8 cxchTextBefore;

        // anld.cxchTextBefore will be the beginning offset of the text in
        // the anld.rgxch that will be displayed as the suffix of an
        // autonumber. The sum of anld.cxchTextBefore + anld.cxchTextAfter
        // will be the limit of the autonumber suffix in anld.rgxch
        U8 cxchTextAfter;

        // justification code
        //     0 left justify
        //     1 center
        //     2 right justify
        //     3 left and right justify
        U8 jc:2;

        // when ==1, number generated will include previous levels (used for
        // legal numbering)
        U8 fPrev:1;

        // when ==1, number will be displayed using a hanging indent
        U8 fHang:1;

        // when ==1, boldness of number will be determined by anld.fBold.
        U8 fSetBold:1;

        // when ==1, italicness of number will be determined by anld.fItalic
        U8 fSetItalic:1;

        // when ==1, anld.fSmallCaps will determine whether number will be
        // displayed in small caps or not.
        U8 fSetSmallCaps:1;

        // when ==1, anld.fCaps will determine whether number will be
        // displayed capitalized or not
        U8 fSetCaps:1;

        // when ==1, anld.fStrike will determine whether the number will be
        // displayed using strikethrough or not.
        U8 fSetStrike:1;

        // when ==1, anld.kul will determine the underlining state of the
        // autonumber.
        U8 fSetKul:1;

        // when ==1, autonumber will be displayed with a single prefixing
        // space character
        U8 fPrevSpace:1;

        // determines boldness of autonumber when anld.fSetBold == 1.
        U8 fBold:1;

        // determines italicness of autonumber when anld.fSetItalic == 1.
        U8 FItalic:1;

        // determines whether autonumber will be displayed using small caps
        // when anld.fSetSmallCaps == 1.
        U8 fSmallCaps:1;

        // determines whether autonumber will be displayed using caps when
        // anld.fSetCaps == 1.
        U8 fCaps:1;

        // determines whether autonumber will be displayed using caps when
        // anld.fSetStrike == 1.
        U8 fStrike:1;

        // determines whether autonumber will be displayed with underlining
        // when anld.fSetKul == 1.
        U8 kul:3;

        // color of autonumber
        U8 ico:5;

        // font code of autonumber
        U16 ftc;

        // font half point size (or 0=auto)
        U16 hps;

        // starting value (0 to 65535)
        U16 iStartAt;

        // width of prefix text (same as indent)
        U16 dxaIndent;

        // minimum space between number and paragraph
        U16 dxaSpace;
    } __attribute__ ((packed)) ANLV;
    static unsigned read(const U8 *in, __UNAL ANLV *out, unsigned count=1);

    // AutoSummary Analysis (ASUMY)
    typedef struct ASUMY
    {

        // AutoSummary level
        U32 lLevel;
    } __attribute__ ((packed)) ASUMY;
    static unsigned read(const U8 *in, __UNAL ASUMY *out, unsigned count=1);

    // AutoSummary Info (ASUMYI)
    typedef struct ASUMYI
    {

        // true iff the ASUMYI is valid
        U16 fValid:1;

        // true iff AutoSummary View is active
        U16 fView:1;

        // Display method for AutoSummary View:
        //     0 = Emphasize in current doc
        //     1 = Reduce doc to summary
        //     2 = Insert into doc
        //     3 = Show in new document
        U16 iViewBy:2;

        // true if we should update File Properties summary information after
        // the next summarization
        U16 fUpdateProps:1;

        // reserved
        U16 unused0_5:11;

        // Dialog summary level
        U16 wDlgLevel;

        // upper bound for lLevel for sentences in this document
        U32 lHighestLevel;

        // show document sentences at or below this level
        U32 lCurrentLevel;
    } __attribute__ ((packed)) ASUMYI;
    static unsigned read(const U8 *in, __UNAL ASUMYI *out, unsigned count=1);

    // Bin Table Entry (BTE)
    typedef struct BTE
    {

        // Page Number for FKP
        U32 pn;
    } __attribute__ ((packed)) BTE;
    static unsigned read(const U8 *in, __UNAL BTE *out, unsigned count=1);

    // BreaK Descriptor (BKD)
    typedef struct BKD
    {

        // except in textbox BKD, index to <b>PGD</b> in <b>plfpgd</b> that
        // describes the page this break is on.
        U16 ipgd;

        // in textbox BKD,
        U16 itxbxs;

        // number of cp's considered for this break; note that the CP's
        // described by cpDepend in this break reside in the next BKD
        U16 dcpDepend;

        //
        U16 icol:8;

        // when 1, this indicates that this is a table break.
        U16 fTableBreak:1;

        // when 1, this indicates that this is a column break.
        U16 fColumnBreak:1;

        // used temporarily while word is running.
        U16 fMarked:1;

        // in textbox BKD, when == 1 indicates cpLim of this textbox is not
        // valid
        U16 fUnk:1;

        // in textbox BKD, when == 1 indicates that text overflows the end of
        // this textbox
        U16 fTextOverflow:1;

    } __attribute__ ((packed)) BKD;
    static unsigned read(const U8 *in, __UNAL BKD *out, unsigned count=1);

    // BooKmark First descriptor (BKF)
    typedef struct BKF
    {

        // index to <b>BKL</b> entry in <b>plcfbkl</b> that describes the
        // ending position of this bookmark in the <b>CP</b> stream.
        U16 ibkl;

        // when bkf.fCol is 1, this is the index to the first column of a
        // table column bookmark.
        U16 itcFirst:7;

        // when 1, this indicates that this bookmark is marking the range of
        // a Macintosh Publisher section.
        U16 fPub:1;

        // when bkf.fCol is 1, this is the index to limit column of a table
        // column bookmark.
        U16 itcLim:7;

        // when 1, this bookmark marks a range of columns in a table
        // specified by [bkf.itcFirst, bkf.itcLim).
        U16 fCol:1;

    } __attribute__ ((packed)) BKF;
    static unsigned read(const U8 *in, __UNAL BKF *out, unsigned count=1);

    // BooKmark Lim descriptor (BKL)
    typedef struct BKL
    {

        // index to <b>BKF</b> entry in <b>plcfbkf</b> that describes the
        // beginning position of this bookmark in the <b>CP</b> stream. If
        // the bkl.ibkf is negative, add on the number of bookmarks recorded
        // in the hplcbkf to the bkl.ibkf to calculate the index to the BKF
        // that corresponds to this entry.
        U16 ibkf;
    } __attribute__ ((packed)) BKL;
    static unsigned read(const U8 *in, __UNAL BKL *out, unsigned count=1);

    // Border Code (BRC)
    typedef struct BRC
    {

        // width of a single line in 1/8 pt, max of 32 pt.
        U16 dptLineWidth:8;

        // border type code:
        //     0 none
        //     1 single
        //     2 thick
        //     3 double
        //     5 hairline
        //     6 dot
        //     7 dash large gap
        //     8 dot dash
        //     9 dot dot dash
        //     10 triple
        //     11 thin-thick small gap
        //     12 thick-thin small gap
        //     13 thin-thick-thin small gap
        //     14 thin-thick medium gap
        //     15 thick-thin medium gap
        //     16 thin-thick-thin medium gap
        //     17 thin-thick large gap
        //     18 thick-thin large gap
        //     19 thin-thick-thin large gap
        //     20 wave
        //     21 double wave
        //     22 dash small gap
        //     23 dash dot stroked
        //     24 emboss 3D
        //     25 engrave 3D
        //     codes 64 - 230 represent border art types and are used only for page borders.
        U16 brcType:8;

        // color code (see chp.ico)
        U16 ico:8;

        // width of space to maintain between border and text within border.
        // Must be 0 when BRC is a substructure of TC. Stored in points.
        U16 dptSpace:5;

        // when 1, border is drawn with shadow. Must be 0 when BRC is a
        // substructure of the TC
        U16 fShadow:1;

        //
        U16 fFrame:1;

        // reserved
        U16 unused2_15:1;

    } __attribute__ ((packed)) BRC;
    static unsigned read(const U8 *in, __UNAL BRC *out, unsigned count=1);

    // Border Code for Windows Word 1.0 (BRC10)
    typedef struct BRC10
    {

        // width of second line of border in pixels
        U16 dxpLine2Width:3;

        // distance to maintain between both lines of border in pixels
        U16 dxpSpaceBetween:3;

        // width of first border line in pixels
        U16 dxpLine1Width:3;

        // width of space to maintain between border and text within border.
        // Must be 0 when BRC is a substructure of the TC.
        U16 dxpSpace:5;

        // when 1, border is drawn with shadow. Must be 0 when BRC10 is a
        // substructure of the TC.
        U16 fShadow:1;

        // reserved
        U16 fSpare:1;

    } __attribute__ ((packed)) BRC10;
    static unsigned read(const U8 *in, __UNAL BRC10 *out, unsigned count=1);

    // Character Properties (CHP)
    typedef struct CHP
    {

        // text is bold when 1 , and not bold when 0.
        U8 fBold:1;

        // italic when 1, not italic when 0
        U8 fItalic:1;

        // when 1, text has been deleted and will be displayed with
        // strikethrough when revision marked text is to be displayed
        U8 fRMarkDel:1;

        // outlined when 1, not outlined when 0
        U8 fOutline:1;

        // used internally by Word
        U8 fFldVanish:1;

        // displayed with small caps when 1, no small caps when 0
        U8 fSmallCaps:1;

        // displayed with caps when 1, no caps when 0
        U8 fCaps:1;

        // when 1, text has "hidden" format, and is not displayed unless
        // fPagHidden is set in the DOP
        U8 fVanish:1;

        // when 1, text is newly typed since the last time revision marks
        // have been accepted and will be displayed with an underline when
        // revision marked text is to be displayed
        U8 fRMark:1;

        // character is a Word special character when 1, not a special
        // character when 0
        U8 fSpec:1;

        // displayed with strikethrough when 1, no strikethrough when 0
        U8 fStrike:1;

        // embedded object when 1, not an embedded object when 0
        U8 fObj:1;

        // character is drawn with a shadow when 1; drawn without shadow when
        // 0
        U8 fShadow:1;

        // character is displayed in lower case when 1. No case
        // transformation is performed when 0. This field may be set to 1
        // only when chp.fSmallCaps is 1.
        U8 fLowerCase:1;

        // when 1, chp.fcPic points to an FFDATA, the data structure binary
        // data used by Word to describe a form field. The bit chp.fData may
        // only be 1 when chp.fSpec is also 1 and the special character in
        // the document stream that has this property is a chPicture (0x01).
        U8 fData:1;

        // when 1, chp.lTagObj specifies a particular object in the object
        // stream that specifies the particular OLE object in the stream that
        // should be displayed when the chPicture fSpec character that is
        // tagged with the fOle2 is encountered. The bit chp.fOle2 may only
        // be 1 when chp.fSpec is also 1 and the special character in the
        // document stream that has this property is a chPicture (0x01).
        U8 fOle2:1;

        // text is embossed when 1 and not embossed when 0
        U16 fEmboss:1;

        // text is engraved when 1 and not engraved when 0
        U16 fImprint:1;

        // displayed with double strikethrough when 1, no double
        // strikethrough when 0
        U16 fDStrike:1;

        //
        U16 fUsePgsuSettings:1;

        // Reserved
        U16 unused2_4:12;

        // Reserved
        U32 unused4;

        // no longer stored
        U16 ftc;

        // (rgftc[0]) font for ASCII text
        U16 ftcAscii;

        // (rgftc[1]) font for Far East text
        U16 ftcFE;

        // (rgftc[2]) font for non-Far East text
        U16 ftcOther;

        // font size in half points
        U16 hps;

        // space following each character in the run expressed in twip units.
        U32 dxaSpace;

        // superscript/subscript indices
        //     0 means no super/subscripting
        //     1 means text in run is superscripted
        //     2 means text in run is subscripted
        U8 iss:3;

        // underline code:
        //     0 none
        //     1 single
        //     2 by word
        //     3 double
        //     4 dotted
        //     5 hidden
        //     6 thick
        //     7 dash
        //     8 dot (not used)
        //     9 dot dash
        //     10 dot dot dash
        //     11 wave
        U8 kul:4;

        // used by Word internally, not stored in file
        U8 fSpecSymbol:1;

        // color of text:
        //     0 Auto
        //     1 Black
        //     2 Blue
        //     3 Cyan
        //     4 Green
        //     5 Magenta
        //     6 Red
        //     7 Yellow
        //     8 White
        //     9 DkBlue
        //     10 DkCyan
        //     11 DkGreen
        //     12 DkMagenta
        //     13 DkRed
        //     14 DkYellow
        //     15 DkGray
        //     16 LtGray
        U8 ico:5;

        // reserved
        U8 unused23_5:1;

        // used by Word internally, not stored in file
        U8 fSysVanish:1;

        // reserved
        U8 hpsPos:1;

        // super/subscript position in half points; positive means text is
        // raised; negative means text is lowered.
        S16 hpScript;

        // LID language identification code (no longer stored here, see rglid
        // below):
        //     0x0400 No Proofing
        //     0x0401 Arabic
        //     0x0402 Bulgarian
        //     0x0403 Catalan
        //     0x0404 Traditional Chinese
        //     0x0804 Simplified Chinese
        //     0x0405 Czech
        //     0x0406 Danish
        //     0x0407 German
        //     0x0807 Swiss German
        //     0x0408 Greek
        //     0x0409 U.S. English
        //     0x0809 U.K. English
        //     0x0c09 Australian English
        //     0x040a Castilian Spanish
        //     0x080a Mexican Spanish
        //     0x040b Finnish
        //     0x040c French
        //     0x080c Belgian French
        //     0x0c0c Canadian French
        //     0x100c Swiss French
        //     0x040d Hebrew
        //     0x040e Hungarian
        //     0x040f Icelandic
        //     0x0410 Italian
        //     0x0810 Swiss Italian
        //     0x0411 Japanese
        //     0x0412 Korean
        //     0x0413 Dutch
        //     0x0813 Belgian Dutch
        //     0x0414 Norwegian - Bokmal
        //     0x0814 Norwegian - Nynorsk
        //     0x0415 Polish
        //     0x0416 Brazilian Portuguese
        //     0x0816 Portuguese
        //     0x0417 Rhaeto-Romanic
        //     0x0418 Romanian
        //     0x0419 Russian
        //     0x041a Croato-Serbian (Latin)
        //     0x081a Serbo-Croatian (Cyrillic)
        //     0x041b Slovak
        //     0x041c Albanian
        //     0x041d Swedish
        //     0x041e Thai
        //     0x041f Turkish
        //     0x0420 Urdu
        //     0x0421 Bahasa
        //     0x0422 Ukrainian
        //     0x0423 Byelorussian
        //     0x0424 Slovenian
        //     0x0425 Estonian
        //     0x0426 Latvian
        //     0x0427 Lithuanian
        //     0x0429 Farsi
        //     0x042D Basque
        //     0x042F Macedonian
        //     0x0436 Afrikaans
        //     0x043E Malaysian
        U16 lid;

        // (rglid[0]) LID language for non-Far East text
        U16 lidDefault;

        // (rglid[1]) LID language for Far East text
        U16 lidFE;

        // not stored in file
        U8 idct;

        // Identifier of Characte type
        //     0 -> shared chars get non-FE props
        //     1 -> shared chars get FE props
        //     (see Appendix C)
        U8 idctHint;

        //
        U16 wCharScale;

        // (fcPic) FC offset in data stream pointing to beginning of a
        // picture when character is a picture character (character is 0x01
        // and chp.fSpec is 1) <p>(fcObj) FC offset in data stream pointing
        // to beginning of a picture when character is an OLE1 object
        // character (character is 0x20 and chp.fSpec is 1, chp.fOle2 is 0)
        // <p>(lTagObj) long word tag that identifies an OLE2 object in the
        // object stream when the character is an OLE2 object character.
        // (character is 0x01 and chp.fSpec is 1, chp.fOle2 is 1)
        U32 fcPic_fcObj_lTagObj;

        // index to author IDs stored in hsttbfRMark. used when text in run
        // was newly typed when revision marking was enabled
        U16 ibstRMark;

        // index to author IDs stored in hsttbfRMark. used when text in run
        // was deleted when revision marking was enabled
        U16 ibstRMarkDel;

        // Date/time at which this run of text was entered/modified by the
        // author. (Only recorded when revision marking is on.)
        DTTM dttmRMark;

        // Date/time at which this run of text was deleted by the author.
        // (Only recorded when revision marking is on.)
        DTTM dttmRMarkDel;

        //
        U16 unused52;

        // index to character style descriptor in the stylesheet that tags
        // this run of text When istd is istdNormalChar (10 decimal),
        // characters in run are not affected by a character style. If
        // chp.istd contains any other value, chpx of the specified character
        // style are applied to CHP for this run before any other exceptional
        // properties are applied.
        U16 istd;

        // when chp.fSpec is 1 and the character recorded for the run in the
        // document stream is chSymbol (0x28), chp.ftcSym identifies the font
        // code of the symbol font that will be used to display the symbol
        // character recorded in chp.xchSym. chp.ftcSym is an index into the
        // rgffn structure.
        U16 ftcSym;

        // when chp.fSpec is 1 and the character recorded for the run in the
        // document stream is chSymbol (0x28), the character stored
        // chp.xchSym will be displayed using the font specified in
        // chp.ftcSym.
        XCHAR xchSym[1];

        // an index to strings displayed as reasons for actions taken by
        // Word's AutoFormat code
        U16 idslRMReason;

        // an index to strings displayed as reasons for actions taken by
        // Word's AutoFormat code
        U16 idslReasonDel;

        // hyphenation rule
        //     0 No hyphenation
        //     1 Normal hyphenation
        //     2 Add letter before hyphen
        //     3 Change letter before hyphen
        //     4 Delete letter before hyphen
        //     5 Change letter after hyphen
        //     6 Delete letter before the hyphen and change the letter preceding the deleted character
        U8 ysr;

        // the character that will be used to add or change a letter when
        // chp.ysr is 2,3, 5 or 6
        U8 chYsr;

        //
        U16 cpg;

        // kerning distance for characters in run recorded in half points
        U16 hpsKern;

        // highlight color (see chp.ico)
        U16 icoHighlight:5;

        // when 1, characters are highlighted with color specified by
        // chp.icoHighlight.
        U16 fHighlight:1;

        //
        U16 kcd:3;

        // used internally by Word
        U16 fNavHighlight:1;

        //
        U16 fChsDiff:1;

        //
        U16 fMacChs:1;

        //
        U16 fFtcAsciSym:1;

        // when 1, properties have been changed with revision marking on
        U16 fPropMark;

        // index to author IDs stored in hsttbfRMark. used when properties
        // have been changed when revision marking was enabled
        U16 ibstPropRMark;

        // Date/time at which properties of this were changed for this run of
        // text by the author. (Only recorded when revision marking is on.)
        DTTM dttmPropRMark;

        // text animation:
        //     0 no animation
        //     1 Las Vegas lights
        //     2 background blink
        //     3 sparkle text
        //     4 marching ants
        //     5 marchine red ants
        //     6 shimmer
        U8 sfxtText;

        // reserved
        U8 unused81;

        // reserved
        U8 unused82;

        // reserved
        U16 unused83;

        // reserved
        U16 unused85;

        // reserved
        U32 unused87;

        // (Only valid for ListNum fields). When 1, the number for a ListNum
        // field is being tracked in xstDispFldRMark -- if that number is
        // different from the current value, the number has changed.
        U8 fDispFldRMark;

        // Index to author IDs stored in hsttbfRMark. used when ListNum field
        // numbering has been changed when revision marking was enabled
        U16 ibstDispFldRMark;

        // The date for the ListNum field number change
        U32 dttmDispFldRMark;

        // The string value of the ListNum field when revision mark tracking
        // began
        XCHAR xstDispFldRMark[16];

        // shading
        SHD shd;

        // border
        BRC brc;
    } __attribute__ ((packed)) CHP;
    static unsigned read(const U8 *in, __UNAL CHP *out, unsigned count=1);

    // Character Property Exceptions (CHPX)
//    typedef struct CHPX
//    {
//
//        // count of bytes of following data in CHPX.
//        U8 cb;
//
//        // a list of the sprms that encode the differences between CHP for a
//        // run of text and the CHP generated by the paragraph and character
//        // styles that tag the run.
//        U8[cb] grpprl;
//    } __attribute__ ((packed)) CHPX;
//    static unsigned read(const U8 *in, __UNAL CHPX *out, unsigned count=1);

    // Drop Cap Specifier(DCS)
    typedef struct DCS
    {

        // 0
        U8 fdct:3;

        // 0
        U8 unused0_3:5;

        //
        U16 unused1;
    } __attribute__ ((packed)) DCS;
    static unsigned read(const U8 *in, __UNAL DCS *out, unsigned count=1);

    // Drawing Object Grid (DOGRID)
    typedef struct DOGRID
    {

        // x-coordinate of the upper left-hand corner of the grid
        U16 xaGrid;

        // y-coordinate of the upper left-hand corner of the grid
        U16 yaGrid;

        // width of each grid square
        U16 dxaGrid;

        // height of each grid square
        U16 dyaGrid;

        // the number of grid squares (in the y direction) between each
        // gridline drawn on the screen. 0 means don't display any gridlines
        // in the y direction.
        U16 dyGridDisplay:7;

        // suppress display of gridlines
        U16 fTurnItOff:1;

        // the number of grid squares (in the x direction) between each
        // gridline drawn on the screen. 0 means don't display any gridlines
        // in the y direction.
        U16 dxGridDisplay:7;

        // if true, the grid will start at the left and top margins and
        // ignore xaGrid and yaGrid.
        U16 fFollowMargins:1;

    } __attribute__ ((packed)) DOGRID;
    static unsigned read(const U8 *in, __UNAL DOGRID *out, unsigned count=1);

    // Document Properties (DOP)
    typedef struct DOP
    {

        // 1 when facing pages should be printed.
        //     Default 0.
        U16 fFacingPages:1;

        // 1 when widow control is in effect. 0 when widow control disabled.
        //     Default 1.
        U16 fWidowControl:1;

        // 1 when doc is a main doc for Print Merge Helper, 0 when not;
        // default=0
        U16 fPMHMainDoc:1;

        // Default line suppression storage; 0= form letter line suppression;
        // 1= no line suppression; default=0. No longer used.
        U16 grfSuppression:2;

        // footnote position code&nbsp;
        //     &nbsp; <p>0 print as endnotes
        //     1 print at bottom of page
        //     2 print immediately beneath text
        //     Default 1.
        U16 fpc:2;

        // unused. Default 0.
        U16 unused0_7:1;

        // No longer used. Default 0.
        U16 grpfIhdt:8;

        // restart index for footnotes&nbsp;
        //     &nbsp; <p>0 don't restart note numbering
        //     1 restart for each section
        //     2 restart for each page
        //     Default 0.
        U16 rncFtn:2;

        // initial footnote number for document. Default 1.
        U16 nFtn:14;

        // when 1, indicates that information in the hplcpad should be
        // refreshed since outline has been dirtied
        U8 fOutlineDirtySave:1;

        // reserved
        U8 unused4_1:7;

        // when 1, Word believes all pictures recorded in the document were
        // created on a Macintosh
        U8 fOnlyMacPics:1;

        // when 1, Word believes all pictures recorded in the document were
        // created in Windows
        U8 fOnlyWinPics:1;

        // when 1, document was created as a print merge labels document
        U8 fLabelDoc:1;

        // when 1, Word is allowed to hyphenate words that are capitalized.
        // When 0, capitalized may not be hyphenated
        U8 fHyphCapitals:1;

        // when 1, Word will hyphenate newly typed text as a background task
        U8 fAutoHyphen:1;

        //
        U8 fFormNoFields:1;

        // when 1, Word will merge styles from its template
        U8 fLinkStyles:1;

        // when 1, Word will mark revisions as the document is edited
        U8 fRevMarking:1;

        // always make backup when document saved when 1.
        U8 fBackup:1;

        // when 1, the results of the last Word Count execution (as recorded
        // in several DOP fields) are still exactly correct.
        U8 fExactCWords:1;

        // when 1, hidden document contents are displayed.
        U8 fPagHidden:1;

        // when 1, field results are displayed, when 0 field codes are
        // displayed.
        U8 fPagResults:1;

        // when 1, annotations are locked for editing
        U8 fLockAtn:1;

        // swap margins on left/right pages when 1.
        U8 fMirrorMargins:1;

        // reserved
        U8 unused6_6;

        // when 1, use TrueType fonts by default (flag obeyed only when doc
        // was created by WinWord 2.x)
        U8 fDfltTrueType:1;

        // when 1, file created with SUPPRESSTOPSPACING=YES in win.ini. (flag
        // obeyed only when doc was created by WinWord 2.x).
        U8 fPagSuppressTopSpacing:1;

        // when 1, document is protected from edit operations
        U8 fProtEnabled:1;

        // when 1, restrict selections to occur only within form fields
        U8 fDispFormFldSel:1;

        // when 1, show revision markings on screen
        U8 fRMView:1;

        // when 1, print revision marks when document is printed
        U8 fRMPrint:1;

        // reserved
        U8 unused7_5;

        // when 1, the current revision marking state is locked
        U8 fLockRev:1;

        // when 1, document contains embedded TrueType fonts
        U8 fEmbedFonts:1;

        // compatibility option: when 1, don't add automatic tab stops for
        // hanging indent
        U16 copts_fNoTabForInd:1;

        // compatibility option: when 1, don't add extra space for raised or
        // lowered characters
        U16 copts_fNoSpaceRaiseLower:1;

        // compatibility option: when 1, suppress the paragraph Space Before
        // and Space After options after a page break
        U16 copts_fSuppressSpbfAfterPageBreak:1;

        // compatibility option: when 1, wrap trailing spaces at the end of a
        // line to the next line
        U16 copts_fWrapTrailSpaces:1;

        // compatibility option: when 1, print colors as black on non-color
        // printers
        U16 copts_fMapPrintTextColor:1;

        // compatibility option: when 1, don't balance columns for Continuous
        // Section starts
        U16 copts_fNoColumnBalance:1;

        //
        U16 copts_fConvMailMergeEsc:1;

        // compatibility option: when 1, suppress extra line spacing at top
        // of page
        U16 copts_fSupressTopSpacing:1;

        // compatibility option: when 1, combine table borders like Word 5.x
        // for the Macintosh
        U16 copts_fOrigWordTableRules:1;

        // compatibility option: when 1, don't blank area between metafile
        // pictures
        U16 copts_fTransparentMetafiles:1;

        // compatibility option: when 1, show hard page or column breaks in
        // frames
        U16 copts_fShowBreaksInFrames:1;

        // compatibility option: when 1, swap left and right pages on odd
        // facing pages
        U16 copts_fSwapBordersFacingPgs:1;

        // reserved
        U16 unused8_12:4;

        // default tab width. Default 720 twips.
        U16 dxaTab;

        //
        U16 wSpare;

        // width of hyphenation hot zone measured in twips
        U16 dxaHotZ;

        // number of lines allowed to have consecutive hyphens
        U16 cConsecHypLim;

        // reserved
        U16 wSpare2;

        // date and time document was created
        DTTM dttmCreated;

        // date and time document was last revised
        DTTM dttmRevised;

        // date and time document was last printed
        DTTM dttmLastPrint;

        // number of times document has been revised since its creation
        U16 nRevision;

        // time document was last edited
        U32 tmEdited;

        // count of words tallied by last Word Count execution
        U32 cWords;

        // count of characters tallied by last Word Count execution
        U32 cCh;

        // count of pages tallied by last Word Count execution
        U16 cPg;

        // count of paragraphs tallied by last Word Count execution
        U32 cParas;

        // restart endnote number code&nbsp;
        //     0 don't restart endnote numbering
        //     1 restart for each section
        //     2 restart for each page
        U16 rncEdn:2;

        // beginning endnote number
        U16 nEdn:14;

        // endnote position code&nbsp;
        //     0 display endnotes at end of section
        //     3 display endnotes at end of document
        U16 epc:2;

        // number format code for auto footnotes&nbsp;
        //     0 Arabic
        //     1 Upper case Roman
        //     2 Lower case Roman
        //     3 Upper case Letter
        //     4 Lower case Letter
        U16 nfcFtnRef:4;

        // number format code for auto endnotes&nbsp;
        //     0 Arabic
        //     1 Upper case Roman
        //     2 Lower case Roman
        //     3 Upper case Letter
        //     4 Lower case Letter
        U16 nfcEdnRef:4;

        // only print data inside of form fields
        U16 fPrintFormData:1;

        // only save document data that is inside of a form field.
        U16 fSaveFormData:1;

        // shade form fields
        U16 fShadeFormData:1;

        // reserved
        U16 unused54_13:2;

        // when 1, include footnotes and endnotes in word count
        U16 fWCFtnEdn:1;

        // count of lines tallied by last Word Count operation
        U32 cLines;

        // count of words in footnotes and endnotes tallied by last Word
        // Count operation
        U32 cWordsFtnEnd;

        // count of characters in footnotes and endnotes tallied by last Word
        // Count operation
        U32 cChFtnEdn;

        // count of pages in footnotes and endnotes tallied by last Word
        // Count operation
        U16 cPgFtnEdn;

        // count of paragraphs in footnotes and endnotes tallied by last Word
        // Count operation
        U32 cParasFtnEdn;

        // count of paragraphs in footnotes and endnotes tallied by last Word
        // Count operation
        U32 cLinesFtnEdn;

        // document protection password key, only valid if dop.fProtEnabled,
        // dop.fLockAtn or dop.fLockRev are 1.
        U32 lKeyProtDoc;

        // document view kind&nbsp;
        //     0 Normal view
        //     1 Outline view
        //     2 Page View
        U16 wvkSaved:3;

        // zoom percentage
        U16 wScaleSaved:9;

        // zoom type&nbsp;
        //     0 None
        //     1 Full page
        //     2 Page width
        U16 zkSaved:2;

        // This is a vertical document (Word 6/95 only)
        U16 fRotateFontW6:1;

        // Gutter position for this doc: 0 => side; 1 => top
        U16 iGutterPos:1;

    } __attribute__ ((packed)) DOP;
    static unsigned read(const U8 *in, __UNAL DOP *out, unsigned count=1);

    // Document Properties with nFIB > 102 (DOP102)
    typedef struct DOP102
    {
        // (see above)
        U32 fNoTabForInd:1;

        // (see above)
        U32 fNoSpaceRaiseLower:1;

        // (see above)
        U32 fSupressSpbfAfterPageBreak:1;

        // (see above)
        U32 fWrapTrailSpaces:1;

        // (see above)
        U32 fMapPrintTextColor:1;

        // (see above)
        U32 fNoColumnBalance:1;

        // (see above)
        U32 fConvMailMergeEsc:1;

        // (see above)
        U32 fSupressTopSpacing:1;

        // (see above)
        U32 fOrigWordTableRules:1;

        // (see above)
        U32 fTransparentMetafiles:1;

        // (see above)
        U32 fShowBreaksInFrames:1;

        // (see above)
        U32 fSwapBordersFacingPgs:1;

        // (reserved)
        U32 unused84_12:4;

        // Suppress extra line spacing at top of page like MacWord5.x
        U32 fSuppressTopSpacingMac5:1;

        // Expand/Condense by whole number of points.
        U32 fTruncDxaExpand:1;

        // Print body text before header/footer
        U32 fPrintBodyBeforeHdr:1;

        // Don't add leading (extra space) between rows of text
        U32 fNoLeading:1;

        // (reserved)
        U32 unused84_20:1;

        // Use larger small caps like MacWord 5.x
        U32 fMWSmallCaps:1;

        // (reserved)
        U32 unused84_22:10;

    } __attribute__ ((packed)) DOP102;
    static unsigned read(const U8 *in, __UNAL DOP102 *out, unsigned count=1);

    // Document Properties with nFIB > 105 (DOP105)
    typedef struct DOP105
    {
        // Autoformat Document Type: 0 for normal. 1 for letter, and 2 for
        // email.
        U16 adt;

        // see DOPTYPOGRAPHY
        DOPTYPOGRAPHY doptypography;

        // see DOGRID
        DOGRID dogrid;

        // Always set to zero when writing files
        U16 reserved:1;

        // Which outline levels are showing in outline view (0 => heading 1
        // only, 4 => headings 1 through 5, 9 => all levels showing)
        U16 lvl:4;

        // Doc has been completely grammar checked
        U16 fGramAllDone:1;

        // No grammer errors exist in doc
        U16 fGramAllClean:1;

        // if you are doing font embedding, you should only embed the
        // characters in the font that are used in the document
        U16 fSubsetFonts:1;

        // Hide the version created for autoversion
        U16 fHideLastVersion:1;

        // This file is based upon an HTML file
        U16 fHtmlDoc:1;

        // Always set to zero when writing files
        U16 unused410_11:1;

        // Snap table and page borders to page border
        U16 fSnapBorder:1;

        // Place header inside page border
        U16 fIncludeHeader:1;

        // Place footer inside page border
        U16 fIncludeFooter:1;

        // Are we in online view
        U16 fForcePageSizePag:1;

        // Are we auto-promoting fonts to >= hpsZoonFontPag?
        U16 fMinFontSizePag:1;

        // versioning is turned on
        U16 fHaveVersions:1;

        // autoversioning is enabled
        U16 fAutoVersion:1;

        // Always set to zero when writing files
        U16 unused412_2:14;

        // Autosummary info
        ASUMYI asumyi;

        // Count of characters with spaces
        U32 cChWS;

        // Count of characters with spaces in footnotes and endnotes
        U32 cChWSFtnEdn;

        //
        U32 grfDocEvents;

        // Have we prompted for virus protection on this doc?
        U32 fVirusPrompted:1;

        // If prompted, load safely for this doc?
        U32 fVirusLoadSafe:1;

        // Random session key to sign above bits for a Word session.
        U32 KeyVirusSession30:30;

        // Spare
        U8 Spare[30];

        // Always set to zero when writing files
        U32 unused472;

        // Always set to zero when writing files
        U32 unused476;

        // Count of double byte characters
        U32 cDBC;

        // Count od double byte characters in footnotes and endnotes
        U32 cDBCFtnEdn;

        // Always set to zero when writing files
        U32 unused488;

        // number format code for auto footnote references&nbsp;
        //     0 Arabic
        //     1 Upper case Roman
        //     2 Lower case Roman
        //     3 Upper case Letter
        //     4 Lower case Letter
        U16 nfcFtnRef;

        // number format code for auto endnote references&nbsp;
        //     0 Arabic&nbsp; <div CLASS="tt">1 Upper case Roman</div>  <p>
        //     2 Lower case Roman
        //     3 Upper case Letter
        //     4 Lower case Letter
        U16 nfcEdnRef;

        // minimum font size if fMinFontSizePag is true
        U16 hpsZoonFontPag;

        // height of the window in online view during last repag
        U16 dywDispPag;
    } __attribute__ ((packed)) DOP105;
    static unsigned read(const U8 *in, __UNAL DOP105 *out, unsigned count=1);

    // Field Descriptor (FLD)
    typedef struct FLD
    {

        // type of field boundary the FLD describes:
        //     19 field begin mark
        //     20 field separator mark
        //     21 field end mark
        U8 ch:5;

        // reserved
        U8 unused0_5:3;

        // fld.ch == 19 (field begin mark) -> U8 field type (see flt table
        // below). <p>fld.ch == 21 (field end mark) ->
        //     fDiffer:1 - ignored for saved file
        //     fZombieEmbed:1 - 1 when result still believes this field is an EMBED or LINK field.
        //     fResultDirty:1 -&nbsp; when user has edited or formatted the result. == 0 otherwise.
        //     fResultEdited:1 - 1 when user has inserted text into or deleted text from the result.
        //     fLocked:1 - 1 when field is locked from recalc.
        //     fPrivateResult:1 - 1 whenever the result of the field is never to be shown.
        //     fNested:1 - 1 when field is nested within another field.
        //     fHasSep:1 - 1 when field has a field separator.
        U8 flt;
    } __attribute__ ((packed)) FLD;
    static unsigned read(const U8 *in, __UNAL FLD *out, unsigned count=1);

    // File Shape Address (FSPA)
    typedef struct FSPA
    {

        // Shape Identifier. Used in conjunction with the office art data
        // (found via <b>fcDggInfo</b> in the <b>FIB</b>) to find the actual
        // data for this shape.
        U32 spid;

        // xa left of rectangle enclosing shape relative to the origin of the
        // shape
        U32 xaLeft;

        // ya top of rectangle enclosing shape relative to the origin of the
        // shape
        U32 yaTop;

        // xa right of rectangle enclosing shape relative to the origin of
        // the shape
        U32 xaRight;

        // ya bottom of the rectangle enclosing shape relative to the origin
        // of the shape
        U32 yaBottom;

        // 1 in the undo doc when shape is from the header doc, 0 otherwise
        // (undefined when not in the undo doc)
        U16 fHdr:1;

        // x position of shape relative to anchor CP
        //     0 relative to page margin
        //     1 relative to top of page
        //     2 relative to text (column for horizontal text; paragraph for vertical text)
        //     3 reserved for future use
        U16 bx:2;

        // y position of shape relative to anchor CP
        //     0 relative to page margin
        //     1 relative to top of page
        //     2 relative to text (paragraph for horizontal text; column for vertical text)
        U16 by:2;

        // text wrapping mode
        //     0 like 2, but doesn't require absolute object
        //     1 no text next to shape
        //     2 wrap around absolute object
        //     3 wrap as if no object present
        //     4 wrap tightly around object
        //     5 wrap tightly, but allow holes
        //     6-15 reserved for future use
        U16 wr:4;

        // text wrapping mode type (valid only for wrapping modes 2 and 4
        //     0 wrap both sides
        //     1 wrap only on left
        //     2 wrap only on right
        //     3 wrap only on largest side
        U16 wrk:4;

        // when set, temporarily overrides bx, by, forcing the xaLeft,
        // xaRight, yaTop, and yaBottom fields to all be page relative.
        U16 fRcaSimple:1;

        // 1 shape is below text
        //     0 shape is above text
        U16 fBelowText:1;

        // 1 anchor is locked
        //     0 anchor is not locked
        U16 fAnchorLock:1;

        // count of textboxes in shape (undo doc only)
        U32 cTxbx;
    } __attribute__ ((packed)) FSPA;
    static unsigned read(const U8 *in, __UNAL FSPA *out, unsigned count=1);

    // Font Family Name (FFN)
//    typedef struct FFN
//    {
//
//        // total length of FFN - 1.
//        U8 cbFfnM1;
//
//        // pitch request
//        U8 prq:2;
//
//        // when 1, font is a TrueType font
//        U8 fTrueType:1;
//
//        // reserved
//        U8 unused1_3:1;
//
//        // font family id
//        U8 ff:3;
//
//        // reserved
//        U8 unused1_7:1;
//
//        // base weight of font
//        U16 wWeight;
//
//        // character set identifier
//        U8 chs;
//
//        // index into ffn.szFfn to the name of the alternate font
//        U8 ixchSzAlt;
//
//        // ? This is supposed to be of type PANOSE.
//        U8 panose[10];
//
//        // ? This is supposed to be of type FONTSIGNATURE.
//        U8 fs[24];
//
//        // zero terminated string that records name of font. Possibly
//        // followed by a second xsz which records the name of an alternate
//        // font to use if the first named font does not exist on this system.
//        // Maximal size of xszFfn is 65 characters.
//        U8 xszFfn[0];
//    } __attribute__ ((packed)) FFN;
//    static unsigned read(const U8 *in, __UNAL FFN *out, unsigned count=1);

    // File Information Block (FIB)
    typedef struct FIB
    {

        // (fibh) FIBH Beginning of the FIB header magic number
        U16 wIdent;

        // FIB version written. This will be >= 101 for all Word 6.0 for
        // Windows and after documents.
        U16 nFib;

        // product version written by
        U16 nProduct;

        // language stamp -- localized version
        //     In pre-WinWord 2.0 files this value was the nLocale. If value is &lt; 999, then it is the nLocale, otherwise it is the lid.
        U16 lid;

        //
        U16 pnNext;

        // Set if this document is a template
        U16 fDot:1;

        // Set if this document is a glossary
        U16 fGlsy:1;

        // when 1, file is in <b>complex, fast-saved format.</b>
        U16 fComplex:1;

        // set if file contains 1 or more pictures
        U16 fHasPic:1;

        // count of times file was quicksaved
        U16 cQuickSaves:4;

        // Set if file is encrypted
        U16 fEncrypted:1;

        // When 0, this fib refers to the table stream named "0Table", when
        // 1, this fib refers to the table stream named "1Table". Normally, a
        // file will have only one table stream, but under unusual
        // circumstances a file may have table streams with both names. In
        // that case, this flag must be used to decide which table stream is
        // valid.
        U16 fWhichTblStm:1;

        // Set when user has recommended that file be read read-only
        U16 fReadOnlyRecommended:1;

        // Set when file owner has made the file write reserved
        U16 fWriteReservation:1;

        // Set when using extended character set in file
        U16 fExtChar:1;

        // REVIEW
        U16 fLoadOverride:1;

        // REVIEW
        U16 fFarEast:1;

        // REVIEW
        U16 fCrypto:1;

        // This file format it compatible with readers that understand nFib
        // at or above this value.
        U16 nFibBack;

        // File encrypted key, only valid if fEncrypted.
        U32 lKey;

        // environment in which file was created
        //     0 created by Win Word
        //     1 created by Mac Word
        U8 envr;

        // when 1, this file was last saved in the Mac environment
        U8 fMac:1;

        //
        U8 fEmptySpecial:1;

        //
        U8 fLoadOverridePage:1;

        //
        U8 fFutureSavedUndo:1;

        //
        U8 fWord97Saved:1;

        //
        U8 fSpare0:3;

        // Default extended character set id for text in document stream.
        // (overridden by chp.chse)
        //     0 by default characters in doc stream should be interpreted using the ANSI character set used by Windows
        //     256 characters in doc stream should be interpreted using the Macintosh character set.
        U16 chs;

        // Default extended character set id for text in internal data
        // structures
        //     0 by default characters stored in internal data structures should be interpreted using the ANSI character set used by Windows
        //     256 characters stored in internal data structures should be interpreted using the Macintosh character set.
        U16 chsTables;

        // file offset of first character of text. In <b>non-complex
        // files</b> a <b>CP</b> can be transformed into an <b>FC</b> by the
        // following transformation:
        //     fc = cp + fib.fcMin.
        U32 fcMin;

        // file offset of last character of text in document text stream + 1
        U32 fcMac;

        // Count of fields in the array of "shorts"
        U16 csw;

        // [Beginning of the array of shorts, rgls]
        //     Unique number Identifying the File's creator 0x6A62 is the creator ID for Word and is reserved. Other creators should choose a different value.
        U16 wMagicCreated;

        // identifies the File's last modifier
        U16 wMagicRevised;

        // private data
        U16 wMagicCreatedPrivate;

        // private data
        U16 wMagicRevisedPrivate;

        // not used
        U16 pnFbpChpFirst_W6;

        // not used
        U16 pnChpFirst_W6;

        // not used
        U16 cpnBteChp_W6;

        // not used
        U16 pnFbpPapFirst_W6;

        // not used
        U16 pnPapFirst_W6;

        // not used
        U16 cpnBtePap_W6;

        // not used
        U16 pnFbpLvcFirst_W6;

        // not used
        U16 pnLvcFirst_W6;

        // not used
        U16 cpnBteLvc_W6;

        // Language id if document was written by Far East version of Word
        // (i.e. FIB.fFarEast is on)
        U16 lidFE;

        // Number of fields in the array of longs
        U16 clw;

        // [Beginning of the array of longs, rglw]
        //     file offset of last byte written to file + 1.
        U32 cbMac;

        // contains the build date of the creator. 10695 indicates the
        // creator program was compiled on Jan 6, 1995
        U32 lProductCreated;

        // contains the build date of the File's last modifier
        U32 lProductRevised;

        // length of main document text stream
        U32 ccpText;

        // length of footnote subdocument text stream
        U32 ccpFtn;

        // length of header subdocument text stream
        U32 ccpHdd;

        // length of macro subdocument text stream, which should now always
        // be 0.
        U32 ccpMcr;

        // length of annotation subdocument text stream
        U32 ccpAtn;

        // length of endnote subdocument text stream
        U32 ccpEdn;

        // length of textbox subdocument text stream
        U32 ccpTxbx;

        // length of header textbox subdocument text stream.
        U32 ccpHdrTxbx;

        // when there was insufficient memory for Word to expand the plcfbte
        // at save time, the plcfbte is written to the file in a linked list
        // of 512-byte pieces starting with this pn
        U32 pnFbpChpFirst;

        // the page number of the lowest numbered page in the document that
        // records CHPX FKP information
        U32 pnChpFirst;

        // count of CHPX FKPs recorded in file. In non-complex files if the
        // number of entries in the plcfbteChpx is less than this, the
        // plcfbteChpx is incomplete.
        U32 cpnBteChp;

        // when there was insufficient memory for Word to expand the plcfbte
        // at save time, the plcfbte is written to the file in a linked list
        // of 512-byte pieces starting with this pn
        U32 pnFbpPapFirst;

        // the page number of the lowest numbered page in the document that
        // records PAPX FKP information
        U32 pnPapFirst;

        // count of PAPX FKPs recorded in file. In non-complex files if the
        // number of entries in the plcfbtePapx is less than this, the
        // plcfbtePapx is incomplete.
        U32 cpnBtePap;

        // when there was insufficient memory for Word to expand the plcfbte
        // at save time, the plcfbte is written to the file in a linked list
        // of 512-byte pieces starting with this pn
        U32 pnFbpLvcFirst;

        // the page number of the lowest numbered page in the document that
        // records LVC FKP information
        U32 pnLvcFirst;

        // count of LVC FKPs recorded in file. In non-complex files if the
        // number of entries in the plcfbtePapx is less than this, the
        // plcfbtePapx is incomplete.
        U32 cpnBteLvc;

        //
        U32 fcIslandFirst;

        //
        U32 fcIslandLim;

        // Number of fields in the array of FC/LCB pairs.
        U16 cfclcb;

        // [Beginning of array of FC/LCB pairs, rgfclcb]
        //     file offset of original allocation for STSH in table stream. During fast save Word will attempt to reuse this allocation if STSH is small enough to fit.
        U32 fcStshfOrig;

        // count of bytes of original STSH allocation
        U32 lcbStshfOrig;

        // offset of STSH in table stream.
        U32 fcStshf;

        // count of bytes of current STSH allocation
        U32 lcbStshf;

        // offset in table stream of footnote reference PLCF of FRD
        // structures. CPs in PLC are relative to main document text stream
        // and give location of footnote references.
        U32 fcPlcffndRef;

        // count of bytes of footnote reference PLC== 0 if no footnotes
        // defined in document.
        U32 lcbPlcffndRef;

        // offset in table stream of footnote text PLC. CPs in PLC are
        // relative to footnote subdocument text stream and give location of
        // beginnings of footnote text for corresponding references recorded
        // in plcffndRef. No structure is stored in this plc. There will just
        // be <b>n+1</b> <b>FC</b> entries in this PLC when there are
        // <b>n</b> footnotes
        U32 fcPlcffndTxt;

        // count of bytes of footnote text PLC. == 0 if no footnotes defined
        // in document
        U32 lcbPlcffndTxt;

        // offset in table stream of annotation reference ATRD PLC. The CPs
        // recorded in this PLC give the offset of annotation references in
        // the main document.
        U32 fcPlcfandRef;

        // count of bytes of annotation reference PLC.
        U32 lcbPlcfandRef;

        // offset in table stream of annotation text PLC. The Cps recorded in
        // this PLC give the offset of the annotation text in the annotation
        // sub document corresponding to the references stored in the
        // plcfandRef. There is a 1 to 1 correspondence between entries
        // recorded in the plcfandTxt and the plcfandRef. No structure is
        // stored in this PLC.
        U32 fcPlcfandTxt;

        // count of bytes of the annotation text PLC
        U32 lcbPlcfandTxt;

        // offset in table stream of section descriptor SED PLC. CPs in PLC
        // are relative to main document.
        U32 fcPlcfsed;

        // count of bytes of section descriptor PLC.
        U32 lcbPlcfsed;

        // no longer used
        U32 fcPlcpad;

        // no longer used
        U32 lcbPlcpad;

        // offset in table stream of PHE PLC of paragraph heights. CPs in PLC
        // are relative to main document text stream. Only written for files
        // in <b>complex</b> format. Should not be written by third party
        // creators of Word files.
        U32 fcPlcfphe;

        // count of bytes of paragraph height PLC. ==0 when file is
        // <b>non-complex</b>.
        U32 lcbPlcfphe;

        // offset in table stream of glossary string table. This table
        // consists of Pascal style strings (strings stored prefixed with a
        // length byte) concatenated one after another.
        U32 fcSttbfglsy;

        // count of bytes of glossary string table. == 0 for non-glossary
        // documents.!=0 for glossary documents.
        U32 lcbSttbfglsy;

        // offset in table stream of glossary PLC. CPs in PLC are relative to
        // main document and mark the beginnings of glossary entries and are
        // in 1-1 correspondence with entries of sttbfglsy. No structure is
        // stored in this PLC. There will be <b>n+1</b> <b>FC</b> entries in
        // this PLC when there are <b>n</b> glossary entries.
        U32 fcPlcfglsy;

        // count of bytes of glossary PLC.== 0 for non-glossary documents.!=0
        // for glossary documents.
        U32 lcbPlcfglsy;

        // byte offset in table stream of header HDD PLC. CPs are relative to
        // header subdocument and mark the beginnings of individual headers
        // in the header subdocument. No structure is stored in this PLC.
        // There will be <b>n+1FC</b> entries in this PLC when there are
        // <b>n</b> headers stored for the document.
        U32 fcPlcfhdd;

        // count of bytes of header PLC.
        //     == 0 if document contains no headers
        U32 lcbPlcfhdd;

        // offset in table stream of character property bin table.PLC. FCs in
        // PLC are file offsets in the main stream. Describes text of main
        // document and all subdocuments.
        U32 fcPlcfbteChpx;

        // count of bytes of character property bin table PLC.
        U32 lcbPlcfbteChpx;

        // offset in table stream of paragraph property bin table.PLC. FCs in
        // PLC are file offsets in the main stream. Describes text of main
        // document and all subdocuments.
        U32 fcPlcfbtePapx;

        // count of bytes of paragraph property bin table PLC
        U32 lcbPlcfbtePapx;

        // offset in table stream of PLC reserved for private use. The
        // <b>SEA</b> is 6 bytes long.
        U32 fcPlcfsea;

        // count of bytes of private use PLC.
        U32 lcbPlcfsea;

        // offset in table stream of font information STTBF. The sttbfffn is
        // a STTBF where is string is actually an FFN structure. The
        // <b>n</b>th entry in the STTBF describes the font that will be
        // displayed when the chp.ftc for text is equal to <b>n.</b> See the
        // FFN file structure definition.
        U32 fcSttbfffn;

        // count of bytes in sttbfffn.
        U32 lcbSttbfffn;

        // offset in table stream to the FLD PLC of field positions in the
        // main document. The CPs point to the beginning CP of a field, the
        // CP of field separator character inside a field and the ending CP
        // of the field. A field may be nested within another field. 20
        // levels of field nesting are allowed.
        U32 fcPlcffldMom;

        // count of bytes in plcffldMom
        U32 lcbPlcffldMom;

        // offset in table stream to the FLD PLC of field positions in the
        // header subdocument.
        U32 fcPlcffldHdr;

        // count of bytes in plcffldHdr
        U32 lcbPlcffldHdr;

        // offset in table stream to the FLD PLC of field positions in the
        // footnote subdocument.
        U32 fcPlcffldFtn;

        // count of bytes in plcffldFtn
        U32 lcbPlcffldFtn;

        // offset in table stream to the FLD PLC of field positions in the
        // annotation subdocument.
        U32 fcPlcffldAtn;

        // count of bytes in plcffldAtn
        U32 lcbPlcffldAtn;

        // no longer used
        U32 fcPlcffldMcr;

        // no longer used
        U32 lcbPlcffldMcr;

        // offset in table stream of the STTBF that records bookmark names in
        // the main document
        U32 fcSttbfbkmk;

        //
        U32 lcbSttbfbkmk;

        // offset in table stream of the PLCF that records the beginning CP
        // offsets of bookmarks in the main document. See BKF structure
        // definition
        U32 fcPlcfbkf;

        //
        U32 lcbPlcfbkf;

        // offset in table stream of the PLCF that records the ending CP
        // offsets of bookmarks recorded in the main document. No structure
        // is stored in this PLCF.
        U32 fcPlcfbkl;

        //
        U32 lcbPlcfbkl;

        // offset in table stream of the macro commands. These commands are
        // private and undocumented.
        U32 fcCmds;

        // undocument size of undocument structure not documented above
        U32 lcbCmds;

        // no longer used
        U32 fcPlcmcr;

        //
        U32 lcbPlcmcr;

        // no longer used
        U32 fcSttbfmcr;

        //
        U32 lcbSttbfmcr;

        // offset in table stream of the printer driver information (names of
        // drivers, port, etc.)
        U32 fcPrDrvr;

        // count of bytes of the printer driver information (names of
        // drivers, port, etc.)
        U32 lcbPrDrvr;

        // offset in table stream of the print environment in portrait mode.
        U32 fcPrEnvPort;

        // count of bytes of the print environment in portrait mode.
        U32 lcbPrEnvPort;

        // offset in table stream of the print environment in landscape mode.
        U32 fcPrEnvLand;

        // count of bytes of the print environment in landscape mode.
        U32 lcbPrEnvLand;

        // offset in table stream of <b>W</b>indow <b>S</b>ave <b>S</b>tate
        // data structure. <b>WSS</b> contains dimensions of document's main
        // text window and the last selection made by Word user.
        U32 fcWss;

        // count of bytes of WSS. ==0 if unable to store the window state.
        // Should not be written by third party creators of Word files.
        U32 lcbWss;

        // offset in table stream of document property data structure.
        U32 fcDop;

        // count of bytes of document properties.
        U32 lcbDop;

        // offset in table stream of STTBF of associated strings. The strings
        // in this table specify document summary info and the paths to
        // special documents related to this document. See documentation of
        // the STTBFASSOC.
        U32 fcSttbfAssoc;

        //
        U32 lcbSttbfAssoc;

        // offset in table stream of beginning of information for
        // <b>complex</b> files. Consists of an encoding of all of the
        // <b>prm</b>s quoted by the document followed by the <b>plcpcd</b>
        // (piece table) for the document.
        U32 fcClx;

        // count of bytes of complex file information == 0 if file is
        // <b>non-complex</b>.
        U32 lcbClx;

        // not used
        U32 fcPlcfpgdFtn;

        //
        U32 lcbPlcfpgdFtn;

        // offset in table stream of the name of the original file.
        // fcAutosaveSource and cbAutosaveSource should both be 0 if autosave
        // is off.
        U32 fcAutosaveSource;

        // count of bytes of the name of the original file.
        U32 lcbAutosaveSource;

        // offset in table stream of group of strings recording the names of
        // the owners of annotations stored in the document
        U32 fcGrpXstAtnOwners;

        // count of bytes of the group of strings
        U32 lcbGrpXstAtnOwners;

        // offset in table stream of the sttbf that records names of
        // bookmarks for the annotation subdocument
        U32 fcSttbfAtnbkmk;

        // length in bytes of the sttbf that records names of bookmarks for
        // the annotation subdocument
        U32 lcbSttbfAtnbkmk;

        // no longer used
        U32 fcPlcdoaMom;

        //
        U32 lcbPlcdoaMom;

        // no longer used
        U32 fcPlcdoaHdr;

        //
        U32 lcbPlcdoaHdr;

        // offset in table stream of the <b>FSPA</b> PLC for main document.
        // == 0 if document has no office art objects.
        U32 fcPlcspaMom;

        // length in bytes of the <b>FSPA</b> PLC of the main document.
        U32 lcbPlcspaMom;

        // offset in table stream of the <b>FSPA</b> PLC for header document.
        // == 0 if document has no office art objects.
        U32 fcPlcspaHdr;

        // length in bytes of the <b>FSPA</b> PLC of the header document.
        U32 lcbPlcspaHdr;

        // offset in table stream of BKF (bookmark first) PLC of the
        // annotation subdocument
        U32 fcPlcfAtnbkf;

        // length in bytes of BKF (bookmark first) PLC of the annotation
        // subdocument
        U32 lcbPlcfAtnbkf;

        // offset in table stream of BKL (bookmark last) PLC of the
        // annotation subdocument
        U32 fcPlcfAtnbkl;

        // length in bytes of PLC marking the CP limits of the annotation
        // bookmarks. No structure is stored in this PLC.
        U32 lcbPlcfAtnbkl;

        // offset in table stream of PMS (Print Merge State) information
        // block. This contains the current state of a print merge operation
        U32 fcPms;

        // length in bytes of PMS. ==0 if no current print merge state.
        // Should not be written by third party creators of Word files.
        U32 lcbPms;

        // offset in table stream of form field Sttbf which contains strings
        // used in form field dropdown controls
        U32 fcFormFldSttbs;

        // length in bytes of form field Sttbf
        U32 lcbFormFldSttbs;

        // offset in table stream of endnote reference PLCF of FRD
        // structures. CPs in PLCF are relative to main document text stream
        // and give location of endnote references.
        U32 fcPlcfendRef;

        //
        U32 lcbPlcfendRef;

        // offset in table stream of PlcfendRef which points to endnote text
        // in the endnote document stream which corresponds with the
        // plcfendRef. No structure is stored in this PLC.
        U32 fcPlcfendTxt;

        //
        U32 lcbPlcfendTxt;

        // offset in table stream to FLD PLCF of field positions in the
        // endnote subdoc
        U32 fcPlcffldEdn;

        //
        U32 lcbPlcffldEdn;

        // not used
        U32 fcPlcfpgdEdn;

        //
        U32 lcbPlcfpgdEdn;

        // offset in table stream of the office art object table data. The
        // format of office art object table data is found in a separate
        // document.
        U32 fcDggInfo;

        // length in bytes of the office art object table data
        U32 lcbDggInfo;

        // offset in table stream to STTBF that records the author
        // abbreviations for authors who have made revisions in the document.
        U32 fcSttbfRMark;

        //
        U32 lcbSttbfRMark;

        // offset in table stream to STTBF that records caption titles used
        // in the document.
        U32 fcSttbCaption;

        //
        U32 lcbSttbCaption;

        // offset in table stream to the STTBF that records the object names
        // and indices into the caption STTBF for objects which get auto
        // captions.
        U32 fcSttbAutoCaption;

        //
        U32 lcbSttbAutoCaption;

        // offset in table stream to WKB PLCF that describes the boundaries
        // of contributing documents in a master document
        U32 fcPlcfwkb;

        //
        U32 lcbPlcfwkb;

        // offset in table stream of PLCF (of SPLS structures) that records
        // spell check state
        U32 fcPlcfspl;

        //
        U32 lcbPlcfspl;

        // offset in table stream of PLCF that records the beginning CP in
        // the text box subdoc of the text of individual text box entries. No
        // structure is stored in this PLCF
        U32 fcPlcftxbxTxt;

        //
        U32 lcbPlcftxbxTxt;

        // offset in table stream of the FLD PLCF that records field
        // boundaries recorded in the textbox subdoc.
        U32 fcPlcffldTxbx;

        //
        U32 lcbPlcffldTxbx;

        // offset in table stream of PLCF that records the beginning CP in
        // the header text box subdoc of the text of individual header text
        // box entries. No structure is stored in this PLC.
        U32 fcPlcfhdrtxbxTxt;

        //
        U32 lcbPlcfhdrtxbxTxt;

        // offset in table stream of the FLD PLCF that records field
        // boundaries recorded in the header textbox subdoc.
        U32 fcPlcffldHdrTxbx;

        //
        U32 lcbPlcffldHdrTxbx;

        // Macro User storage
        U32 fcStwUser;

        //
        U32 lcbStwUser;

        // offset in table stream of embedded true type font data.
        U32 fcSttbttmbd;

        //
        U32 cbSttbttmbd;

        //
        U32 fcUnused;

        //
        U32 lcbUnused;

        // (FCPGD, beginning of array of fcPgd / fcBkd pairs rgpgdbkd).
        //     offset in table stream of the PLF that records the page descriptors for the main text of the doc.
        U32 fcPgdMother;

        //
        U32 lcbPgdMother;

        // offset in table stream of the PLCF that records the break
        // descriptors for the main text of the doc.
        U32 fcBkdMother;

        //
        U32 lcbBkdMother;

        // offset in table stream of the PLF that records the page
        // descriptors for the footnote text of the doc.
        U32 fcPgdFtn;

        //
        U32 lcbPgdFtn;

        // offset in table stream of the PLCF that records the break
        // descriptors for the footnote text of the doc.
        U32 fcBkdFtn;

        //
        U32 lcbBkdFtn;

        // offset in table stream of the PLF that records the page
        // descriptors for the endnote text of the doc.
        U32 fcPgdEdn;

        //
        U32 lcbPgdEdn;

        // offset in table stream of the PLCF that records the break
        // descriptors for the endnote text of the doc.
        U32 fcBkdEdn;

        //
        U32 lcbBkdEdn;

        // offset in table stream of the STTBF containing field keywords.
        // This is only used in a small number of the international versions
        // of word. This field is no longer written to the file for nFib >=
        // 167.
        U32 fcSttbfIntlFld;

        // Always 0 for nFib >= 167.
        U32 lcbSttbfIntlFld;

        // offset in table stream of a mailer routing slip.
        U32 fcRouteSlip;

        //
        U32 lcbRouteSlip;

        // offset in table stream of STTBF recording the names of the users
        // who have saved this document alternating with the save locations.
        U32 fcSttbSavedBy;

        //
        U32 lcbSttbSavedBy;

        // offset in table stream of STTBF recording filenames of documents
        // which are referenced by this document.
        U32 fcSttbFnm;

        //
        U32 lcbSttbFnm;

        // offset in the table stream of list format information.
        U32 fcPlcfLst;

        //
        U32 lcbPlcfLst;

        // offset in the table stream of list format override information.
        U32 fcPlfLfo;

        //
        U32 lcbPlfLfo;

        // offset in the table stream of the textbox break table (a PLCF of
        // BKDs) for the main document
        U32 fcPlcftxbxBkd;

        //
        U32 lcbPlcftxbxBkd;

        // offset in the table stream of the textbox break table (a PLCF of
        // BKDs) for the header subdocument
        U32 fcPlcftxbxHdrBkd;

        //
        U32 lcbPlcftxbxHdrBkd;

        // offset in main stream of undocumented undo / versioning data
        U32 fcDocUndo;

        //
        U32 lcbDocUndo;

        // offset in main stream of undocumented undo / versioning data
        U32 fcRgbuse;

        //
        U32 lcbRgbuse;

        // offset in main stream of undocumented undo / versioning data
        U32 fcUsp;

        //
        U32 lcbUsp;

        // offset in table stream of undocumented undo / versioning data
        U32 fcUskf;

        //
        U32 lcbUskf;

        // offset in table stream of undocumented undo / versioning data
        U32 fcPlcupcRgbuse;

        //
        U32 lcbPlcupcRgbuse;

        // offset in table stream of undocumented undo / versioning data
        U32 fcPlcupcUsp;

        //
        U32 lcbPlcupcUsp;

        // offset in table stream of string table of style names for glossary
        // entries
        U32 fcSttbGlsyStyle;

        //
        U32 lcbSttbGlsyStyle;

        // offset in table stream of undocumented grammar options PL
        U32 fcPlgosl;

        //
        U32 lcbPlgosl;

        // offset in table stream of undocumented ocx data
        U32 fcPlcocx;

        //
        U32 lcbPlcocx;

        // offset in table stream of character property bin table.PLC. FCs in
        // PLC are file offsets. Describes text of main document and all
        // subdocuments.
        U32 fcPlcfbteLvc;

        //
        U32 lcbPlcfbteLvc;

        // (a.k.a FILETIME ftModified)
        U32 dwLowDateTime;

        //
        U32 dwHighDateTime;

        // offset in table stream of LVC PLCF
        U32 fcPlcflvc;

        // size of LVC PLCF, ==0 for <b>non-complex</b> files
        U32 lcbPlcflvc;

        // offset in table stream of autosummary ASUMY PLCF.
        U32 fcPlcasumy;

        //
        U32 lcbPlcasumy;

        // offset in table stream of PLCF (of SPLS structures) which records
        // grammar check state
        U32 fcPlcfgram;

        //
        U32 lcbPlcfgram;

        // offset in table stream of list names string table
        U32 fcSttbListNames;

        //
        U32 lcbSttbListNames;

        // offset in table stream of undocumented undo / versioning data
        U32 fcSttbfUssr;

        //
        U32 lcbSttbfUssr;
    } __attribute__ ((packed)) FIB;
    static unsigned read(const U8 *in, __UNAL FIB *out, unsigned count=1);

    // File Information FC/LCB pair (FIBFCLCB)
    typedef struct FIBFCLCB
    {

        // File position where data begins.
        U32 fc;

        // Size of data. Ignore fc if lcb is zero.
        U32 lcb;
    } __attribute__ ((packed)) FIBFCLCB;
    static unsigned read(const U8 *in, __UNAL FIBFCLCB *out, unsigned count=1);

    // Footnote Reference Descriptor (FRD)
    typedef struct FRD
    {

        // if > 0, the note is an automatically numbered note, otherwise it
        // has a custom mark
        U16 nAuto;
    } __attribute__ ((packed)) FRD;
    static unsigned read(const U8 *in, __UNAL FRD *out, unsigned count=1);

    // Formatted Disk Page for CHPXs (CHPXFKP)
//    typedef struct CHPXFKP
//    {
//
//        // Each <b>FC</b> is the limit <b>FC</b> of a run of exception text.
//        FC rgfc[];
//
//        // an array of bytes where each byte is the word offset of a<b>
//        // CHPX</b>. If the byte stored is 0, there is no difference between
//        // run's character properties and the style's character properties.
//        U8 rgb[];
//
//        // As new runs/paragraphs are recorded in the <b>FKP</b>, unused
//        // space is reduced by 5 if CHPX is already recorded and is reduced
//        // by 5+sizeof(CHPX) if property is not already recorded.
//        U8 unusedSpace[];
//
//        // grpchpx consists of all of the <b>CHPX</b>s stored in <b>FKP</b>
//        // concatenated end to end. Each <b>CHPX</b> is prefixed with a count
//        // of bytes which records its length.
//        U8 grpchpx[];
//
//        // count of runs for <b>CHPX FKP,</b>
//        U8 crun;
//    } __attribute__ ((packed)) CHPXFKP;
//    static unsigned read(const U8 *in, __UNAL CHPXFKP *out, unsigned count=1);

    // Formatted Disk Page for PAPXs (PAPXFKP)
//    typedef struct PAPXFKP
//    {
//
//        // Each <b>FC</b> is the limit <b>FC</b> of a paragraph (i.e. points
//        // to the next character past an end of paragraph mark). There will
//        // be fkp.crun+1 recorded in the FKP.
//        FC[fkp.crun+1] rgfc;
//
//        // an array of the BX data structure. The <b>ith</b> BX entry in the
//        // array describes the paragraph beginning at fkp.rgfc[i]. The BX is
//        // a 13 byte data structure. The first byte of each BX is the word
//        // offset of the <b>PAPX</b> recorded for the paragraph corresponding
//        // to this BX. <b>.</b>. If the byte stored is 0, this represents a 1
//        // line paragraph 15 pixels high with Normal style (stc == 0) whose
//        // column width is 7980 dxas. The last 12 bytes of the BX is a PHE
//        // structure which stores the current paragraph height for the
//        // paragraph corresponding to the BX. If a plcfphe has an entry that
//        // maps to the FC for this paragraph, that entry's PHE overrides the
//        // PHE stored in the FKP.11*fkp.crun+4 unused space. As new
//        // runs/paragraphs are recorded in the <b>FKP</b>, unused space is
//        // reduced by 17 if CHPX/PAPX is already recorded and is reduced by
//        // 17+sizeof(PAPX) if property is not already recorded.
//        BX[fkp.crun] rgbx;
//
//        // grppapx consists of all of the <b>PAPX</b>s stored in <b>FKP</b>
//        // concatenated end to end. Each <b>PAPX</b> begins with a count of
//        // words which records its length padded to a word boundary.
//        U8 grppapx[];
//
//        // count of paragraphs for <b>PAPX FKP.</b>
//        U8 crun;
//    } __attribute__ ((packed)) PAPXFKP;
//    static unsigned read(const U8 *in, __UNAL PAPXFKP *out, unsigned count=1);

    // List LeVeL (on File) (LVLF)
    typedef struct LVLF
    {

        // start at value for this list level
        U32 iStartAt;

        // number format code (see anld.nfc for a list of options)
        U8 nfc;

        // alignment (left, right, or centered) of the paragraph number.
        U8 jc:2;

        // true (==1) if the level turns all inherited numbers to arabic,
        // false if it preserves their number format code (nfc)
        U8 fLegal:1;

        // true if the level's number sequence is not restarted by higher
        // (more significant) levels in the list
        U8 fNoRestart:1;

        // Word 6 compatibility option: equivalent to anld.fPrev (see ANLD)
        U8 fPrev:1;

        // Word 6 compatibility option: equivalent to anld.fPrevSpace (see
        // ANLD)
        U8 fPrevSpace:1;

        // true if this level was from a converted Word 6 document. If it is
        // true, all of the Word 6 compability options become valid;
        // otherwise they are ignored.
        U8 fWord6:1;

        // unused.
        U8 unused5_7:1;

        // contains the character offsets into the LVL's XST of the inherited
        // numbers of previous levels. This array should be zero terminated
        // unless it is full (all 9 levels full). The XST contains place
        // holders for any paragraph numbers contained in the text of the
        // number, and the place holder contains the ilvl of the inherited
        // number, so lvl.xst[lvl.rgbxchNums[0]] == the level of the first
        // inherited number in this level.
        U8 rgbxchNums[9];

        // the type of character following the number text for the paragraph:
        // 0 == tab, 1 == space, 2 == nothing.
        U8 ixchFollow;

        // Word 6 compatibility option: equivalent to anld.dxaSpace (see ANLD)
        U32 dxaSpace;

        // Word 6 compatibility optino: equivalent to anld.dxaIndent (see
        // ANLD)
        U32 dxaIndent;

        // length, in bytes, of the LVL's grpprlChpx
        U8 cbGrpprlChpx;

        // length, in bytes, of the LVL's grpprlPapx
        U8 cbGrpprlPapx;

        // reserved
        U16 reserved;
    } __attribute__ ((packed)) LVLF;
    static unsigned read(const U8 *in, __UNAL LVLF *out, unsigned count=1);

    // Line Spacing Descriptor (LSPD)
    typedef struct LSPD
    {

        // see description of sprmPDyaLine for description of the meaning of
        // dyaLine
        U16 dyaLine;

        // see description of sprmPDyaLine in the Sprm Definitions section
        // for description of the meaning of dyaLine and fMultLinespace
        // fields.
        U16 fMultLinespace;
    } __attribute__ ((packed)) LSPD;
    static unsigned read(const U8 *in, __UNAL LSPD *out, unsigned count=1);

    // LiST Data (on File) (LSTF)
    typedef struct LSTF
    {

        // Unique List ID
        U32 lsid;

        // Unique template code
        U32 tplc;

        // Array of shorts containing the istd's linked to each level of the
        // list, or istdNil (4095) if no style is linked.
        U16 rgistd[9];

        // true if this is a simple (one-level) list; false if this is a
        // multilevel (nine-level) list.
        U8 fSimpleList:1;

        // Word 6 compatibility option: true if the list should start
        // numbering over at the beginning of each section
        U8 fRestartHdn:1;

        // reserved
        U8 unsigned26_2:6;

        // reserved
        U8 reserved;
    } __attribute__ ((packed)) LSTF;
    static unsigned read(const U8 *in, __UNAL LSTF *out, unsigned count=1);

    // List Format Override (LFO)
    typedef struct LFO
    {

        // List ID of corresponding LSTF (see LSTF)
        U32 lsid;

        // reserved
        U32 unused4;

        // reserved
        U32 unused8;

        // count of levels whose format is overridden (see LFOLVL)
        U8 clfolvl;

        // reserved
        U8 reserved[3];
    } __attribute__ ((packed)) LFO;
    static unsigned read(const U8 *in, __UNAL LFO *out, unsigned count=1);

    // List Format Override for a single LeVeL (LFOLVL)
    typedef struct LFOLVL
    {

        // start-at value if fFormatting == false and fStartAt == true. (if
        // fFormatting == true, the start-at is stored in the LVL)
        U32 iStartAt;

        // the level to be overridden
        U8 ilvl:4;

        // true if the start-at value is overridden
        U8 fStartAt:1;

        // true if the formatting is overriden (in which case the LFOLVL
        // should contain a pointer to a LVL)
        U8 fFormatting:1;

        // reserved
        U8 unsigned4_6:2;

        // reserved
        U8 reserved[3];
    } __attribute__ ((packed)) LFOLVL;
    static unsigned read(const U8 *in, __UNAL LFOLVL *out, unsigned count=1);

    // Outline LiST Data (OLST)
    typedef struct OLST
    {

        // an array of 9 ANLV structures describing how heading numbers
        // should be displayed for each of Word's 9 outline heading levels
        ANLV rganlv[9];

        // when ==1, restart heading on section break
        U8 fRestartHdr;

        // reserved
        U8 fSpareOlst2;

        // reserved
        U8 fSpareOlst3;

        // reserved
        U8 fSpareOlst4;

        // text before/after number
        XCHAR rgxch[32];
    } __attribute__ ((packed)) OLST;
    static unsigned read(const U8 *in, __UNAL OLST *out, unsigned count=1);

    // Number Revision Mark Data (NUMRM)
    typedef struct NUMRM
    {

        // True if this paragraph was numbered when revision mark tracking
        // was turned on
        U8 fNumRM;

        //
        U8 unused1;

        // index to author IDs stored in hsttbfRMark for the paragraph number
        // change
        U16 ibstNumRM;

        // Date of the paragraph number change
        DTTM dttmNumRM;

        // Index into NUMRM.xst of the locations of paragraph number place
        // holders for each level (see LVL.rgxchNums)
        U8 rgbxchNums[9];

        // Number Format Code for the paragraph number place holders for each
        // level (see LVL.nfc)
        U8 rgnfc[9];

        //
        U16 unused26;

        // Numerical value for each level place holder in NUMRM.xst.
        U32 PNBR[9];

        // The text string for the paragraph number, containing level place
        // holders
        XCHAR xst[32];
    } __attribute__ ((packed)) NUMRM;
    static unsigned read(const U8 *in, __UNAL NUMRM *out, unsigned count=1);

    // Page Descriptor (PGD)
    typedef struct PGD
    {

        // 1 only when footnote is continued from previous page
        U16 fContinue:1;

        // 1 when page is dirty (i.e. pagination cannot be trusted)
        U16 fUnk:1;

        // 1 when right hand side page
        U16 fRight:1;

        // 1 when page number must be reset to 1.
        U16 fPgnRestart:1;

        // 1 when section break forced page to be empty.
        U16 fEmptyPage:1;

        // 1 when page contains nothing but footnotes
        U16 fAllFtn:1;

        // unused
        U16 unused0_6:1;

        // table breaks have been calculated for this page.
        U16 fTableBreaks:1;

        // used temporarily while word is running.
        U16 fMarked:1;

        // column breaks have been calculated for this page.
        U16 fColumnBreaks:1;

        // page had a table header at the end
        U16 fTableHeader:1;

        // page has never been valid since created, must recalculate the
        // bounds of this page. If this is the last page, this PGD may really
        // represent many pages.
        U16 fNewPage:1;

        // section break code
        U16 bkc:4;

        // line number of first line, -1 if no line numbering
        U16 lnn;

        // page number as printed
        U16 pgn;

        //
        U32 dym;
    } __attribute__ ((packed)) PGD;
    static unsigned read(const U8 *in, __UNAL PGD *out, unsigned count=1);

    // Paragraph Height (PHE)
    typedef struct PHE
    {

        // reserved
        U16 fSpare:1;

        // PHE entry is invalid when == 1
        U16 fUnk:1;

        // when 1, total height of paragraph is known but lines in paragraph
        // have different heights.
        U16 fDiffLines:1;

        // reserved
        U16 unused0_3:5;

        // when fDiffLines is 0 is number of lines in paragraph
        U16 clMac:8;

        // reserved
        U16 unused2;

        // width of lines in paragraph
        U32 dxaCol;

        // when fDiffLines is 0, is height of every line in paragraph in
        // pixels (dymLine)
        //     when fDiffLines is 1, is the total height in pixels of the paragraph (dymHeight)
        U32 dym;
    } __attribute__ ((packed)) PHE;
    static unsigned read(const U8 *in, __UNAL PHE *out, unsigned count=1);

    // Paragraph Properties (PAP)
    typedef struct PAP
    {

        // index to style descriptor . This is an index to an STD in the
        // <b>STSH</b> structure
        U16 istd;

        // justification code
        //     0 left justify
        //     1 center
        //     2 right justify
        //     3 left and right justify
        U8 jc;

        // keep entire paragraph on one page if possible
        U8 fKeep;

        // keep paragraph on same page with next paragraph if possible
        U8 fKeepFollow;

        // start this paragraph on new page
        U8 fPageBreakBefore;

        //
        U8 fBrLnAbove:1;

        //
        U8 fBrLnBelow:1;

        // reserved
        U8 fUnused:2;

        // vertical position code. Specifies coordinate frame to use when
        // paragraphs are absolutely positioned.
        //     0 vertical position coordinates are relative to margin
        //     1 coordinates are relative to page
        //     2 coordinates are relative to text. This means: relative to where the next non-APO text would have been placed if this APO did not exist.
        U8 pcVert:2;

        // horizontal position code. Specifies coordinate frame to use when
        // paragraphs are absolutely positioned.
        //     0 horiz. position coordinates are relative to column.
        //     1 coordinates are relative to margin
        //     2 coordinates are relative to page
        U8 pcHorz:2;

        // /* the brcp and brcl fields have been superseded by the newly
        // defined brcLeft, brcTop, etc. fields. They remain in the PAP for
        // compatibility with MacWord 3.0 */
        //     rectangle border codes
        //     0 none
        //     1 border above
        //     2 border below
        //     15 box around
        //     16 bar to left of paragraph
        U8 brcp;

        // border line style
        //     0 single
        //     1 thick
        //     2 double
        //     3 shadow
        U8 brcl;

        // reserved
        U8 unused9;

        // when non-zero, list level for this paragraph
        U8 ilvl;

        // no line numbering for this paragraph. (makes this an exception to
        // the section property of line numbering)
        U8 fNoLnn;

        // when non-zero, (1-based) index into the pllfo identifying the list
        // to which the paragraph belongs
        U16 ilfo;

        // no longer used
        U8 nLvlAnm;

        // reserved
        U8 unused15;

        // when 1, paragraph is a side by side paragraph
        U8 fSideBySide;

        // reserved
        U8 unused17;

        // when 0, text in paragraph may be auto hyphenated.
        U8 fNoAutoHyph;

        // when 1, Word will prevent widowed lines in this paragraph from
        // being placed at the beginning of a page
        U8 fWidowControl;

        // indent from right margin (signed).
        U32 dxaRight;

        // indent from left margin (signed)
        U32 dxaLeft;

        // first line indent; signed number relative to dxaLeft
        U32 dxaLeft1;

        // line spacing descriptor
        LSPD lspd;

        // vertical spacing before paragraph (unsigned)
        U32 dyaBefore;

        // vertical spacing after paragraph (unsigned)
        U32 dyaAfter;

        // height of current paragraph.
        PHE phe;

        //
        U8 fCrLf;

        //
        U8 fUsePgsuSettings;

        //
        U8 fAdjustRight;

        // reserved
        U8 unused59;

        // when 1, apply kinsoku rules when performing line wrapping
        U8 fKinsoku;

        // when 1, perform word wrap
        U8 fWordWrap;

        // when 1, apply overflow punctuation rules when performing line
        // wrapping
        U8 fOverflowPunct;

        // when 1, perform top line punctuation processing
        U8 fTopLinePunct;

        // when 1, auto space FE and alphabetic characters
        U8 fAutoSpaceDE;

        // when 1, auto space FE and numeric characters
        U8 fAtuoSpaceDN;

        // font alignment
        //     0 Hanging
        //     1 Centered
        //     2 Roman
        //     3 Variable
        //     4 Auto
        U16 wAlignFont;

        //
        U16 fVertical:1;

        //
        U16 fBackward:1;

        //
        U16 fRotateFont:1;

        // reserved
        U16 unused68_3:13;

        // reserved
        U16 unused70;

        // when 1, paragraph is contained in a table row
        U8 fInTable;

        // when 1, paragraph consists only of the row mark special character
        // and marks the end of a table row.
        U8 fTtp;

        // Wrap Code for absolute objects
        U8 wr;

        // when 1, paragraph may not be edited
        U8 fLocked;

        // TAP used internally by Word
        U32 ptap;

        // when positive, is the horizontal distance from the reference frame
        // specified by pap.pcHorz. 0 means paragraph is positioned at the
        // left with respect to the reference frame specified by pcHorz.
        // Certain negative values have special meaning:
        //     -4 paragraph centered horizontally within reference frame
        //     -8 paragraph adjusted right within reference frame
        //     -12 paragraph placed immediately inside of reference frame
        //     -16 paragraph placed immediately outside of reference frame
        U32 dxaAbs;

        // when positive, is the vertical distance from the reference frame
        // specified by pap.pcVert. 0 means paragraph's y-position is
        // unconstrained. Certain negative values have special meaning:
        //     -4 paragraph is placed at top of reference frame
        //     -8 paragraph is centered vertically within reference frame
        //     -12 paragraph is placed at bottom of reference frame.
        U32 dyaAbs;

        // when not == 0, paragraph is constrained to be dxaWidth wide,
        // independent of current margin or column settings.
        U32 dxaWidth;

        // specification for border above paragraph
        BRC brcTop;

        // specification for border to the left of paragraph
        BRC brcLeft;

        // specification for border below paragraph
        BRC brcBottom;

        // specification for border to the right of paragraph
        BRC brcRight;

        // specification of border to place between conforming paragraphs.
        // Two paragraphs conform when both have borders, their brcLeft and
        // brcRight matches, their widths are the same, they both belong to
        // tables or both do not, and have the same absolute positioning
        // props.
        BRC brcBetween;

        // specification of border to place on outside of text when facing
        // pages are to be displayed.
        BRC brcBar;

        // horizontal distance to be maintained between an absolutely
        // positioned paragraph and any non-absolute positioned text
        U32 dxaFromText;

        // vertical distance to be maintained between an absolutely
        // positioned paragraph and any non-absolute positioned text
        U32 dyaFromText;

        // height of abs obj; 0 == Auto
        U16 dyaHeight:15;

        // 0 = Exact, 1 = At Least
        U16 fMinHeight:1;

        // shading
        SHD shd;

        // drop cap specifier (see DCS definition)
        DCS dcs;

        //
        U8 lvl;

        //
        U8 fNumRMIns;

        // autonumber list descriptor (see ANLD definition)
        ANLD anld;

        // when 1, properties have been changed with revision marking on
        U16 fPropRMark;

        // index to author IDs stored in hsttbfRMark. used when properties
        // have been changed when revision marking was enabled
        U16 ibstPropRMark;

        // Date/time at which properties of this were changed for this run of
        // text by the author. (Only recorded when revision marking is on.)
        DTTM dttmPropRMark;

        // paragraph numbering revision mark data (see NUMRM)
        NUMRM numrm;

        // number of tabs stops defined for paragraph. Must be >= 0 and &lt;=
        // 64.
        U16 itbdMac;

        // array of positions of itbdMac tab stops. itbdMax == 64
        U16 rgdxaTab[64];

        // array of itbdMac tab descriptors
        U16 rgtbd[64];
    } __attribute__ ((packed)) PAP;
    static unsigned read(const U8 *in, __UNAL PAP *out, unsigned count=1);

    // Paragraph Property Exceptions (PAPX)
//    typedef struct PAPX
//    {
//
//        // count of bytes of following data in PAPX. The first byte of a PAPX
//        // is a count of bytes when a PAPX is stored in a STSH. Count of
//        // bytes is used because only paragraph sprms are stored in a STSH
//        // PAPX.
//        U8 cb;
//
//        // count of words for this byte and the following data in PAPX. The
//        // first byte of a PAPX is a count of words when PAPX is stored in an
//        // FKP. If this value is 0, it is a 'pad' byte and the count is
//        // stored in the following byte, Count of words is used because PAPX
//        // in an FKP can contain paragraph <b>and</b> table sprms.
//        U8 cw;
//
//        // if previous byte is 0, this is the count of words of following
//        // data in PAPX (not including this and previous 'pad' byte)
//        U8 (cw);
//
//        // index to style descriptor of the style from which the paragraph
//        // inherits its paragraph and character properties
//        U16 istd;
//
//        // a list of the sprms that encode the differences between PAP for a
//        // paragraph and the PAP for the style used. When a paragraph bound
//        // is also the end of a table row, the PAPX also contains a list of
//        // table sprms which express the difference of table row's TAP from
//        // an empty TAP that has been cleared to zeros. The table sprms are
//        // recorded in the list after all of the paragraph sprms. See Sprms
//        // definitions for list of sprms that are used in PAPXs.
//        character array grpprl;
//    } __attribute__ ((packed)) PAPX;
//    static unsigned read(const U8 *in, __UNAL PAPX *out, unsigned count=1);

    // Picture Descriptor (on File) (PICF)
    typedef struct PICF
    {

        // number of bytes in the PIC structure plus size of following
        // picture data which may be a Window's metafile, a bitmap, or the
        // filename of a TIFF file. In the case of a Macintosh PICT picture,
        // this includes the size of the PIC, the standard "x" metafile, and
        // the Macintosh PICT data. See Appendix B for more information.
        U32 lcb;

        // number of bytes in the PIC (to allow for future expansion).
        U16 cbHeader;

        //
        U16 mfp_mm;

        //
        U16 mfp_xExt;

        //
        U16 mfp_yExt;

        //
        U16 mfp_hMF;
    } __attribute__ ((packed)) PICF;
    static unsigned read(const U8 *in, __UNAL PICF *out, unsigned count=1);

    // Piece Descriptor (PCD)
    typedef struct PCD
    {

        // when 1, means that piece contains no end of paragraph marks.
        U16 fNoParaLast:1;

        // used internally by Word
        U16 fPaphNil:1;

        // used internally by Word
        U16 fCopied:1;

        //
        U16 unused0_3:5;

        // used internally by Word
        U16 fn:8;

        // file offset of beginning of piece. The size of the <b>ith</b>
        // piece can be determined by subtracting rgcp[<b>i</b>] of the
        // containing <b>plcfpcd</b> from its rgcp[<b>i+1</b>].
        U32 fc;

        // contains either a single sprm or else an index number of the
        // grpprl which contains the sprms that modify the properties of the
        // piece.
        PRM prm;
    } __attribute__ ((packed)) PCD;
    static unsigned read(const U8 *in, __UNAL PCD *out, unsigned count=1);

    // Plex of CPs stored in File (PLCF)
//    typedef struct PLCF
//    {
//
//        // contains either a single sprm or else an index number of the
//        // grpprl which contains the sprms that modify the properties of the
//        // piece.
//        FC rgfc[];
//
//        // contains either a single sprm or else an index number of the
//        // grpprl which contains the sprms that modify the properties of the
//        // piece.
//        U8 rgstruct[];
//    } __attribute__ ((packed)) PLCF;
//    static unsigned read(const U8 *in, __UNAL PLCF *out, unsigned count=1);

    // Property Modifier(variant 2) (PRM2)
    typedef struct PRM2
    {

        // set to 1 for variant 2
        U16 fComplex:1;

        // index to a <b>grpprl</b> stored in <b>CLX</b> portion of file.
        U16 igrpprl:15;

    } __attribute__ ((packed)) PRM2;
    static unsigned read(const U8 *in, __UNAL PRM2 *out, unsigned count=1);

    // Routing Slip (RS)
    typedef struct RS
    {

        // when 1, document has been routed to at least one recipient.
        U16 fRouted;

        // when 1, document should be routed to the originator after it has
        // been routed to all recipients.
        U16 fReturnOrig;

        // when 1, a status message is sent to the originator each time the
        // document is forwarded to a recipient on the routing list.
        U16 fTrackStatus;

        // unused( should be 0)
        U16 fDirty;

        // document protection while routing
        //     0 recipients can make changes to the document and all changes are untracked.
        //     1 recipients can add annotations and make changes to the document. Any changes are tracked by revision marks, and revision marking cannot be turned off.
        //     2 recipients can only add annotations to the document.
        //     3 recipients can enter information only in form fields.
        U16 nProtect;

        // index of the current recipient.
        U16 iStage;

        // when 0, document is routed to each recipient in turn. when 1,
        // document is routed to all recipients simultaneously.
        U16 delOption;

        // count of recipients.
        U16 cRecip;
    } __attribute__ ((packed)) RS;
    static unsigned read(const U8 *in, __UNAL RS *out, unsigned count=1);

    // Routing Recipient (RR)
    typedef struct RR
    {

        // count of bytes of private system data
        U16 cb;

        // count of bytes in recipient string (including null terminator).
        U16 cbSzRecip;
    } __attribute__ ((packed)) RR;
    static unsigned read(const U8 *in, __UNAL RR *out, unsigned count=1);

    // Section Descriptor (SED)
    typedef struct SED
    {

        // used internally by Word
        U16 fn;

        // file offset in main stream to beginning of SEPX stored for
        // section. If sed.fcSepx == 0xFFFFFFFF, the section properties for
        // the section are equal to the standard SEP (see SEP definition).
        U32 fcSepx;

        // used internally by Word
        U16 fnMpr;

        // points to offset in FC space of main stream where the Macintosh
        // Print Record for a document created on a Mac will be stored
        U32 fcMpr;
    } __attribute__ ((packed)) SED;
    static unsigned read(const U8 *in, __UNAL SED *out, unsigned count=1);

    // Section Properties (SEP)
    typedef struct SEP
    {

        // break code:
        //     0 No break
        //     1 New column
        //     2 New page
        //     3 Even page
        //     4 Odd page
        U8 bkc;

        // set to 1 when a title page is to be displayed
        U8 fTitlePage;

        // only for Mac compatibility, used only during open, when 1,
        // sep.dxaPgn and sep.dyaPgn are valid page number locations
        U8 fAutoPgn;

        // page number format code:
        //     0 Arabic
        //     1 Roman (upper case)
        //     2 Roman (lower case)
        //     3 Letter (upper case)
        //     4 Letter (lower case)
        U8 nfcPgn;

        // set to 1, when a section in a locked document is unlocked
        U8 fUnlocked;

        // chapter number separator for page numbers
        U8 cnsPgn;

        // set to 1 when page numbering should be restarted at the beginning
        // of this section
        U8 fPgnRestart;

        // when 1, footnotes placed at end of section. When 0, footnotes are
        // placed at bottom of page.
        U8 fEndNote;

        // line numbering code:
        //     0 Per page
        //     1 Restart
        //     2 Continue
        U8 lnc;

        // specification of which headers and footers are included in this
        // section. See explanation in Headers and Footers topic. No longer
        // used.
        U8 grpfIhdt;

        // if 0, no line numbering, otherwise this is the line number modulus
        // (e.g. if nLnnMod is 5, line numbers appear on line 5, 10, etc.)
        U16 nLnnMod;

        // distance of
        U32 dxaLnn;

        // when fAutoPgn ==1, gives the x position of auto page number on
        // page in twips (for Mac compatibility only)
        U16 dxaPgn;

        // when fAutoPgn ==1, gives the y position of auto page number on
        // page in twips (for Mac compatibility only)
        U16 dyaPgn;

        // when ==1, draw vertical lines between columns
        U8 fLBetween;

        // vertical justification code
        //     0 top justified
        //     1 centered
        //     2 fully justified vertically
        //     3 bottom justified
        U8 vjc;

        // bin number supplied from windows printer driver indicating which
        // bin the first page of section will be printed.
        U16 dmBinFirst;

        // bin number supplied from windows printer driver indicating which
        // bin the pages other than the first page of section will be printed.
        U16 dmBinOther;

        // dmPaper code for form selected by user
        U16 dmPaperReq;

        // top page border
        BRC brcTop;

        // left page border
        BRC brcLeft;

        // bottom page border
        BRC brcBottom;

        // right page border
        BRC brcRight;

        // when 1, properties have been changed with revision marking on
        U16 fPropRMark;

        // index to author IDs stored in hsttbfRMark. used when properties
        // have been changed when revision marking was enabled
        U16 ibstPropRMark;

        // Date/time at which properties of this were changed for this run of
        // text by the author. (Only recorded when revision marking is on.)
        DTTM dttmPropRMark;

        //
        U32 dxtCharSpace;

        //
        U32 dyaLinePitch;

        //
        U16 clm;

        // reserved
        U16 unused62;

        // orientation of pages in that section. set to 0 when portrait, 1
        // when landscape
        U8 dmOrientPage;

        // heading number level for page number
        U8 iHeadingPgn;

        // user specified starting page number.
        U16 pgnStart;

        // beginning line number for section
        U16 lnnMin;

        //
        U16 wTextFlow;

        // reserved
        U16 unused72;

        // page border properties
        U16 pgbProp;

        // page border applies to:
        //     0 all pages in this section
        //     1 first page in this section
        //     2 all pages in this section but first
        //     3 whole document (all sections)
        U16 pgbApplyTo:3;

        // page border depth:
        //     0 in front
        //     1 in back
        U16 pgbPageDepth:2;

        // page border offset from:
        //     0 offset from text
        //     1 offset from edge of page
        U16 pgbOffsetFrom:3;

        // default value is 12240 twipswidth of page
        U32 xaPage;

        // default value is 15840 twipsheight of page
        U32 yaPage;

        // used internally by Word
        U32 xaPageNUp;

        // used internally by Word
        U32 yaPageNUp;

        // default value is 1800 twipsleft margin
        U32 dxaLeft;

        // default value is 1800 twipsright margin
        U32 dxaRight;

        // default value is 1440 twipstop margin
        U32 dyaTop;

        // default value is 1440 twipsbottom margin
        U32 dyaBottom;

        // default value is 0 twips gutter width
        U32 dzaGutter;

        // y position of top header measured from top edge of page.
        U32 dyaHdrTop;

        // y position of bottom header measured from top edge of page.
        U32 dyaHdrBottom;

        // number of columns in section - 1.
        U16 ccolM1;

        // when == 1, columns are evenly spaced. Default value is 1.
        U8 fEvenlySpaced;

        // reserved
        U8 unused123;

        // distance that will be maintained between columns
        U32 dxaColumns;

        // array of 89 longs that determine bounds of irregular width columns
        U32 rgdxaColumnWidthSpacing[89];

        // used internally by Word
        U32 dxaColumnWidth;

        //
        U8 dmOrientFirst;

        // used internally by Word
        U8 fLayout;

        // reserved
        U16 unused490;

        // multilevel autonumbering list data (see OLST definition)
        OLST olstAnm;
    } __attribute__ ((packed)) SEP;
    static unsigned read(const U8 *in, __UNAL SEP *out, unsigned count=1);

    // Section Property Exceptions (SEPX)
//    typedef struct SEPX
//    {
//
//        // count of bytes in remainder of <b>SEPX.</b>
//        U16 cb;
//
//        // list of sprms that encodes the differences between the properties
//        // of a section and Word's default section properties.
//        U8 grpprl[0];
//    } __attribute__ ((packed)) SEPX;
//    static unsigned read(const U8 *in, __UNAL SEPX *out, unsigned count=1);

    // STyleSHeet Information (STSHI)
    typedef struct STSHI
    {

        // Count of styles in stylesheet
        U16 cstd;

        // Length of STD Base as stored in a file
        U16 cbSTDBaseInFile;

        // Are built-in stylenames stored?
        U16 fStdStylenamesWritten:1;

        // Spare flags
        U16 unused4_2:15;

        // Max sti known when this file was written
        U16 stiMaxWhenSaved;

        // How many fixed-index istds are there?
        U16 istdMaxFixedWhenSaved;

        // Current version of built-in stylenames
        U16 nVerBuiltInNamesWhenSaved;

        // ftc used by StandardChpStsh for this document
        U16 rgftcStandardChpStsh[3];
    } __attribute__ ((packed)) STSHI;
    static unsigned read(const U8 *in, __UNAL STSHI *out, unsigned count=1);

    // STyle Definition (STD)
//    typedef struct STD
//    {
//
//        // invariant style identifier
//        U16 sti:12;
//
//        // spare field for any temporary use, always reset back to zero!
//        U16 fScratch:1;
//
//        // PHEs of all text with this style are wrong
//        U16 fInvalHeight:1;
//
//        // UPEs have been generated
//        U16 fHasUpe:1;
//
//        // std has been mass-copied; if unused at save time, style should be
//        // deleted
//        U16 fMassCopy:1;
//
//        // style type code
//        U16 sgc:4;
//
//        // base style
//        U16 istdBase:12;
//
//        // # of UPXs (and UPEs)
//        U16 cupx:4;
//
//        // next style
//        U16 istdNext:12;
//
//        // offset to end of upx's, start of upe's
//        U16 bchUpe;
//
//        // auto redefine style when appropriate
//        U16 fAutoRedef:1;
//
//        // hidden from UI?
//        U16 fHidden:1;
//
//        // unused bits
//        U16 unused8_3:14;
//
//        // sub-names are separated by chDelimStyle
//        XCHAR xstzName[2];
//
//        //
//        U8 grupx[];
//
//        // the UPEs are not stored on the file; they are a cache of the
//        // based-on chain
//        U8 grupe[];
//    } __attribute__ ((packed)) STD;
//    static unsigned read(const U8 *in, __UNAL STD *out, unsigned count=1);

    // Tab Descriptor (TBD)
    typedef struct TBD
    {

        // justification code
        //     0 left tab
        //     1 centered tab
        //     2 right tab
        //     3 decimal tab
        //     4 bar
        U8 jc:3;

        // tab leader code
        //     0 no leader
        //     1 dotted leader
        //     2 hyphenated leader
        //     3 single line leader
        //     4 heavy line leader
        U8 tlc:3;

        // reserved
        U8 unused0_6:2;

    } __attribute__ ((packed)) TBD;
    static unsigned read(const U8 *in, __UNAL TBD *out, unsigned count=1);

    // Table Cell Descriptors (TC)
    typedef struct TC
    {

        // set to 1 when cell is first cell of a range of cells that have
        // been merged. When a cell is merged, the display areas of the
        // merged cells are consolidated and the text within the cells is
        // interpreted as belonging to one text stream for purposes of
        // calculating line breaks.
        U16 fFirstMerged:1;

        // set to 1 when cell has been merged with preceding cell.
        U16 fMerged:1;

        // set to 1 when cell has vertical text flow
        U16 fVertical:1;

        // for a vertical table cell, text flow is bottom to top when 1 and
        // is bottom to top when 0.
        U16 fBackward:1;

        // set to 1 when cell has rotated characters (i.e. uses @font)
        U16 fRotateFont:1;

        // set to 1 when cell is vertically merged with the cell(s) above
        // and/or below. When cells are vertically merged, the display area
        // of the merged cells are consolidated. The consolidated area is
        // used to display the contents of the first vertically merged cell
        // (the cell with fVertRestart set to 1), and all other vertically
        // merged cells (those with fVertRestart set to 0) must be empty.
        // Cells can only be merged vertically if their left and right
        // boundaries are (nearly) identical (i.e. if corresponding entries
        // in rgdxaCenter of the table rows differ by at most 3).
        U16 fVertMerge:1;

        // set to 1 when the cell is the first of a set of vertically merged
        // cells. The contents of a cell with fVertStart set to 1 are
        // displayed in the consolidated area belonging to the entire set of
        // vertically merged cells. Vertically merged cells with fVertRestart
        // set to 0 must be empty.
        U16 fVertRestart:1;

        // specifies the alignment of the cell contents relative to text flow
        // (e.g. in a cell with bottom to top text flow and bottom vertical
        // alignment, the text is shifted horizontally to match the cell's
        // right boundary):
        //     0 top
        //     1 center
        //     2 bottom
        U16 vertAlign:2;

        // reserved
        U16 fUnused:7;

        // reserved
        U16 wUnused;

        // BRC[cbrcTc] rgbrc: notational convenience for referring to brcTop,
        // brcLeft, etc. fields.
        //     specification of the top border of a table cell
        BRC brcTop;

        // specification of left border of table row
        BRC brcLeft;

        // specification of bottom border of table row
        BRC brcBottom;

        // specification of right border of table row.
        BRC brcRight;
    } __attribute__ ((packed)) TC;
    static unsigned read(const U8 *in, __UNAL TC *out, unsigned count=1);

    // Table Autoformat Look sPecifier (TLP)
    typedef struct TLP
    {

        // index to Word's table of table looks:
        //     0 (none)
        //     1 Simple 1
        //     2 Simple 2
        //     3 Simple 3
        //     4 Classic 1
        //     5 Classic 2
        //     6 Classic 3
        //     7 Classic 4
        //     8 Colorful 1
        //     9 Colorful 2
        //     10 Colorful 3
        //     11 Columns 1
        //     12 Columns 2
        //     13 Columns 3
        //     14 Columns 4
        //     15 Columns 5
        //     16 Grid 1
        //     17 Grid 2
        //     18 Grid 3
        //     19 Grid 4
        //     20 Grid 5
        //     21 Grid 6
        //     22 Grid 7
        //     23 Grid 8
        //     24 List 1
        //     25 List 2
        //     26 List 3
        //     27 List 4
        //     28 List 5
        //     29 List 6
        //     30 List 7
        //     31 List 8
        //     32 3D Effects 1
        //     33 3D Effects 2
        //     34 3D Effects 3
        //     35 Contemporary
        //     36 Elegant
        //     37 Professional
        //     38 Subtle1
        //     39 Subtle2
        U16 itl;

        // when ==1, use the border properties from the selected table look
        U16 fBorders:1;

        // when ==1, use the shading properties from the selected table look
        U16 fShading:1;

        // when ==1, use the font from the selected table look
        U16 fFont:1;

        // when ==1, use the color from the selected table look
        U16 fColor:1;

        // when ==1, do best fit from the selected table look
        U16 fBestFit:1;

        // when ==1, apply properties from the selected table look to the
        // header rows in the table
        U16 fHdrRows:1;

        // when ==1, apply properties from the selected table look to the
        // last row in the table
        U16 fLastRow:1;

        // when ==1, apply properties from the selected table look to the
        // header columns of the table
        U16 fHdrCols:1;

        // when ==1, apply properties from the selected table look to the
        // last column of the table
        U16 fLastCol:1;

    } __attribute__ ((packed)) TLP;
    static unsigned read(const U8 *in, __UNAL TLP *out, unsigned count=1);

    // Table Properties (TAP)
    typedef struct TAP
    {

        // justification code. specifies how table row should be justified
        // within its column.
        //     0 left justify
        //     1 center
        //     2 right justify
        U16 jc;

        // measures half of the white space that will be maintained between
        // text in adjacent columns of a table row. A dxaGapHalf width of
        // white space will be maintained on both sides of a column boundary.
        U32 dxaGapHalf;

        // when greater than 0. guarantees that the height of the table will
        // be at least dyaRowHeight high. When less than 0, guarantees that
        // the height of the table will be exactly absolute value of
        // dyaRowHeight high. When 0, table will be given a height large
        // enough to represent all of the text in all of the cells of the
        // table. Cells with vertical text flow make no contribution to the
        // computation of the height of rows with auto or at least height.
        // Neither do vertically merged cells, except in the last row of the
        // vertical merge. If an auto height row consists entirely of cells
        // which have vertical text direction or are vertically merged, and
        // the row does not contain the last cell in any vertical cell merge,
        // then the row is given height equal to that of the end of cell mark
        // in the first cell.
        U32 dyaRowHeight;

        // when 1, table row may not be split across page bounds
        U8 fCantSplit;

        // when 1, table row is to be used as the header of the table
        U8 fTableHeader;

        // table look specifier (see TLP definition)
        TLP tlp;

        // reserved for future use
        U32 lwHTMLProps;

        // used internally by Word
        U16 fCaFull:1;

        // used internally by Word
        U16 fFirstRow:1;

        // used internally by Word
        U16 fLastRow:1;

        // used internally by Word
        U16 fOutline:1;

        // reserved
        U16 unused20_12:12;

        // count of cells defined for this row. ItcMac must be >= 0 and less
        // than or equal to 64.
        U16 itcMac;

        // used internally by Word
        U32 dxaAdjust;

        // used internally by Word
        U32 dxaScale;

        // used internally by Word
        U32 dxsInch;

        // rgdxaCenter[0] is the left boundary of cell 0 measured relative to
        // margin.. rgdxaCenter[tap.itcMac - 1] is left boundary of last
        // cell. rgdxaCenter[tap.itcMac] is right boundary of last cell.
        S16 rgdxaCenter[65];

        // used internally by Word
        U16 rgdxaCenterPrint[65];

        // array of table cell descriptors
        TC rgtc[64];

        // array of cell shades
        SHD rgshd[64];

        // array of border defaults for cells
        BRC rgbrcTable[6];
    } __attribute__ ((packed)) TAP;
    static unsigned read(const U8 *in, __UNAL TAP *out, unsigned count=1);

    // TeXtBoX Story (FTXBXS)
    typedef struct FTXBXS
    {

        // when not fReusable, counts the number of textboxes in this story
        // chain
        U32 cTxbx;

        // when fReusable, the index of the next in the linked list of
        // reusable FTXBXSs
        U32 iNextReuse;

        // if fReusable, counts the number of reusable FTXBXSs follow this
        // one in the linked list
        U32 cReusable;

        // this FTXBXS is not currently in use
        U16 fReusable;

        // Shape Identifier (see FSPA) for first Office Shape in textbox
        // chain.
        U32 lid;

        //
        U32 txidUndo;
    } __attribute__ ((packed)) FTXBXS;
    static unsigned read(const U8 *in, __UNAL FTXBXS *out, unsigned count=1);

    // WorK Book (WKB)
    typedef struct WKB
    {

        //
        U16 fn;

        //
        U16 grfwkb;

        //
        U16 lvl;

        //
        U16 fnpt:4;

        //
        U16 fnpd:12;

        // unused
        U32 doc;
    } __attribute__ ((packed)) WKB;
    static unsigned read(const U8 *in, __UNAL WKB *out, unsigned count=1);
};

#ifndef __GNUC__
#pragma pack()
#endif
#endif
