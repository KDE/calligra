/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_pgf.h"

Pgf::Pgf(PgfElementList * elements)
{
	_elements = elements;
}


PgfElement::PgfElement(PgfTag * element)
{
 	_type = T_PgfTag;
 	_pgftag = element;
}


PgfElement::PgfElement(PgfUseNextTag * element)
{
	_type = T_PgfUseNextTag;
	_pgfusenexttag = element;
}


PgfElement::PgfElement(PgfNextTag * element)
{
	_type = T_PgfNextTag;
	_pgfnexttag = element;
}


PgfElement::PgfElement(PgfAlignment * element)
{
	_type = T_PgfAlignment;
	_pgfalignment = element;
}


PgfElement::PgfElement(PgfFIndent * element)
{
	_type = T_PgfFIndent;
	_pgffindent = element;
}


PgfElement::PgfElement(PgfFIndentOffset * element)
{
	_type = T_PgfFIndentOffset;
	_pgffindentoffset = element;
}


PgfElement::PgfElement(PgfFIndentRelative * element)
{
	_type = T_PgfFIndentRelative;
	_pgffindentrelative = element;
}


PgfElement::PgfElement(PgfLIndent * element)
{
	_type = T_PgfLIndent;
	_pgflindent = element;
}


PgfElement::PgfElement(PgfRIndent * element)
{
	_type = T_PgfRIndent;
	_pgfrindent = element;
}


PgfElement::PgfElement(PgfTopSeparator * element)
{
	_type = T_PgfTopSeparator;
	_pgftopseparator = element;
}


PgfElement::PgfElement(PgfTopSepAtIndent * element)
{
	_type = T_PgfTopSepAtIndent;
	_pgftopsepatindent = element;
}


PgfElement::PgfElement(PgfTopSepOffset * element)
{
	_type = T_PgfTopSepOffset;
	_pgftopsepoffset = element;
}


PgfElement::PgfElement(PgfBotSeparator * element)
{
	_type = T_PgfBotSeparator;
	_pgfbotseparator = element;
}


PgfElement::PgfElement(PgfBotSepAtIndent * element)
{
	_type = T_PgfBotSepAtIndent;
	_pgfbotsepatindent = element;
}


PgfElement::PgfElement(PgfBotSepOffset * element)
{
	_type = T_PgfBotSepOffset;
	_pgfbotsepoffset = element;
}


PgfElement::PgfElement(PgfPlacement * element)
{
	_type = T_PgfPlacement;
	_pgfplacement = element;
}


PgfElement::PgfElement(PgfPlacementStyle * element)
{
	_type = T_PgfPlacementStyle;
	_pgfplacementstyle = element;
}


PgfElement::PgfElement(PgfRunInDefaultPunct * element)
{
	_type = T_PgfRunInDefaultPunct;
	_pgfrunindefaultpunct = element;
}


PgfElement::PgfElement(PgfSpBefore * element)
{
	_type = T_PgfSpBefore;
	_pgfspbefore = element;
}


PgfElement::PgfElement(PgfSpAfter * element)
{
	_type = T_PgfSpAfter;
	_pgfspafter = element;
}


PgfElement::PgfElement(PgfWithPrev * element)
{
	_type = T_PgfWithPrev;
	_pgfwithprev = element;
}


PgfElement::PgfElement(PgfWithNext * element)
{
	_type = T_PgfWithNext;
	_pgfwithnext = element;
}


PgfElement::PgfElement(PgfBlockSize * element)
{
	_type = T_PgfBlockSize;
	_pgfblocksize = element;
}


PgfElement::PgfElement(PgfFont * element)
{
	_type = T_PgfFont;
	_pgffont = element;
}


PgfElement::PgfElement(PgfLineSpacing * element)
{
	_type = T_PgfLineSpacing;
	_pgflinespacing = element;
}


PgfElement::PgfElement(PgfLeading * element)
{
	_type = T_PgfLeading;
	_pgfleading = element;
}


PgfElement::PgfElement(PgfAutoNum * element)
{
	_type = T_PgfAutoNum;
	_pgfautonum = element;
}


PgfElement::PgfElement(PgfNumTabs * element)
{
	_type = T_PgfNumTabs;
	_pgfnumtabs = element;
}


PgfElement::PgfElement(PgfNumFormat * element)
{
	_type = T_PgfNumFormat;
	_pgfnumformat = element;
}


PgfElement::PgfElement(PgfNumberFont * element)
{
	_type = T_PgfNumberFont;
	_pgfnumberfont = element;
}


PgfElement::PgfElement(PgfNumAtEnd * element)
{
	_type = T_PgfNumAtEnd;
	_pgfnumatend = element;
}


PgfElement::PgfElement(PgfHyphenate * element)
{
	_type = T_PgfHyphenate;
	_pgfhyphenate = element;
}


PgfElement::PgfElement(HyphenMaxLines * element)
{
	_type = T_HyphenMaxLines;
	_hyphenmaxlines = element;
}


PgfElement::PgfElement(HyphenMinPrefix * element)
{
	_type = T_HyphenMinPrefix;
	_hyphenminprefix = element;
}


PgfElement::PgfElement(HyphenMinSuffix * element)
{
	_type = T_HyphenMinSuffix;
	_hyphenminsuffix = element;
}


PgfElement::PgfElement(HyphenMinWord * element)
{
	_type = T_HyphenMinWord;
	_hyphenminword = element;
}


PgfElement::PgfElement(PgfLetterSpace * element)
{
	_type = T_PgfLetterSpace;
	_pgfletterspace = element;
}


PgfElement::PgfElement(PgfMinWordSpace * element)
{
	_type = T_PgfMinWordSpace;
	_pgfminwordspace = element;
}


PgfElement::PgfElement(PgfMaxWordSpace * element)
{
	_type = T_PgfMaxWordSpace;
	_pgfmaxwordspace = element;
}


PgfElement::PgfElement(PgfOptWordSpace * element)
{
	_type = T_PgfOptWordSpace;
	_pgfoptwordspace = element;
}


PgfElement::PgfElement(PgfLanguage * element)
{
	_type = T_PgfLanguage;
	_pgflanguage = element;
}


PgfElement::PgfElement(PgfCellAlignment * element)
{
	_type = T_PgfCellAlignment;
	_pgfcellalignment = element;
}


PgfElement::PgfElement(PgfMinJRomanLetterSpace * element)
{
	_type = T_PgfMinJRomanLetterSpace;
	_pgfminjromanletterspace = element;
}


PgfElement::PgfElement(PgfOptJRomanLetterSpace * element)
{
	_type = T_PgfOptJRomanLetterSpace;
	_pgfoptjromanletterspace = element;
}


PgfElement::PgfElement(PgfMaxJRomanLetterSpace * element)
{
	_type = T_PgfMaxJRomanLetterSpace;
	_pgfmaxjromanletterspace = element;
}


PgfElement::PgfElement(PgfMinJLetterSpace * element)
{
	_type = T_PgfMinJLetterSpace;
	_pgfminjletterspace = element;
}


PgfElement::PgfElement(PgfOptJLetterSpace * element)
{
	_type = T_PgfOptJLetterSpace;
	_pgfoptjletterspace = element;
}


PgfElement::PgfElement(PgfMaxJLetterSpace * element)
{
	_type = T_PgfMaxJLetterSpace;
	_pgfmaxjletterspace = element;
}


PgfElement::PgfElement(PgfYakumonoType * element)
{
	_type = T_PgfYakumonoType;
	_pgfyakumonotype = element;
}


PgfElement::PgfElement(PgfAcrobatLevel * element )
{
	_type = T_PgfAcrobatLevel;
	_pgfacrobatlevel = element;
}


PgfElement::PgfElement(PgfCellMargins * element)
{
	_type = T_PgfCellMargins;
	_pgfcellmargins = element;
}


PgfElement::PgfElement(PgfCellLMarginFixed * element)
{
	_type = T_PgfCellLMarginFixed;
	_pgfcelllmarginfixed = element;
}


PgfElement::PgfElement(PgfCellRMarginFixed * element)
{
	_type = T_PgfCellRMarginFixed;
	_pgfcellrmarginfixed = element;
}


PgfElement::PgfElement(PgfCellTMarginFixed * element)
{
	_type = T_PgfCellTMarginFixed;
	_pgfcelltmarginfixed = element;
}


PgfElement::PgfElement(PgfCellBMarginFixed * element)
{
	_type = T_PgfCellBMarginFixed;
	_pgfcellbmarginfixed = element;
}


PgfElement::PgfElement(PgfLocked * element)
{
	_type = T_PgfLocked;
	_pgflocked = element;
}


PgfElement::PgfElement(TabStop * element)
{
	_type = T_TabStop;
	_tabstop = element;
}


PgfTag::PgfTag(char const * value)
{
	_value = value;
}


PgfNumFormat::PgfNumFormat(char const * value)
{
	_value = value;
}


PgfUseNextTag::PgfUseNextTag(char const * value)
{
	_value = value;
}


PgfNextTag::PgfNextTag(char const * value)
{
	_value = value;
}


PgfAlignment::PgfAlignment(char const * value)
{
	_value = value;
}


PgfFIndent::PgfFIndent(double value)
{
	_value = value;
}


PgfFIndentRelative::PgfFIndentRelative(char const* value)
{
	_value = value;
}


PgfFIndentOffset::PgfFIndentOffset(double value)
{
	_value = value;
}


PgfLIndent::PgfLIndent(double value)
{
	_value = value;
}


PgfRIndent::PgfRIndent(double value)
{
	_value = value;
}


PgfTopSeparator::PgfTopSeparator(char const * value)
{
	_value = value;
}


PgfTopSepAtIndent::PgfTopSepAtIndent(char const * value)
{
	_value = value;
}


PgfTopSepOffset::PgfTopSepOffset(double value)
{
	_value = value;
}


PgfBotSeparator::PgfBotSeparator(char const * value)
{
	_value = value;
}


PgfBotSepAtIndent::PgfBotSepAtIndent(char const * value)
{
	_value = value;
}


PgfBotSepOffset::PgfBotSepOffset(double value)
{
	_value = value;
}


PgfPlacement::PgfPlacement(char const * value)
{
	_value = value;
}


PgfPlacementStyle::PgfPlacementStyle(char const * value)
{
	_value = value;
}


PgfRunInDefaultPunct::PgfRunInDefaultPunct(char const * value)
{
	_value = value;
}


PgfSpBefore::PgfSpBefore(double value)
{
	_value = value;
}


PgfSpAfter::PgfSpAfter(double value)
{
	_value = value;
}


PgfWithPrev::PgfWithPrev(char const * value)
{
	_value = value;
}


PgfWithNext::PgfWithNext(char const * value)
{
	_value = value;
}


PgfBlockSize::PgfBlockSize(int value)
{
	_value = value;
}


PgfFont::PgfFont(PgfFontElementList * elements)
{
	_elements = elements;
}


PgfLineSpacing::PgfLineSpacing(char const * value)
{
	_value = value;
}


PgfLeading::PgfLeading(double value)
{
	_value = value;
}


PgfAutoNum::PgfAutoNum(char const * value)
{
	_value = value;
}


PgfNumTabs::PgfNumTabs(int value)
{
	_value = value;
}


PgfNumberFont::PgfNumberFont(char const * value)
{
	_value = value;
}


PgfNumAtEnd::PgfNumAtEnd(char const * value)
{
	_value = value;
}


PgfHyphenate::PgfHyphenate(char const * value)
{
	_value = value;
}


HyphenMaxLines::HyphenMaxLines(int value)
{
	_value = value;
}


HyphenMinPrefix::HyphenMinPrefix(int value)
{
	_value = value;
}


HyphenMinSuffix::HyphenMinSuffix(int value)
{
	_value = value;
}


HyphenMinWord::HyphenMinWord(int value)
{
	_value = value;
}


PgfLetterSpace::PgfLetterSpace(char const * value)
{
	_value = value;
}


PgfMinWordSpace::PgfMinWordSpace(int value)
{
	_value = value;
}


PgfMaxWordSpace::PgfMaxWordSpace(int value)
{
	_value = value;
}


PgfOptWordSpace::PgfOptWordSpace(int value)
{
	_value = value;
}


PgfLanguage::PgfLanguage(char const * value)
{
	_value = value;
}


PgfCellAlignment::PgfCellAlignment(char const * value)
{
	_value = value;
}


PgfCellMargins::PgfCellMargins(double value1, double value2, double value3, double value4)
{
	_value1 = value1;
	_value2 = value2;
	_value3 = value3;
	_value4 = value4;
}


PgfCellLMarginFixed::PgfCellLMarginFixed(char const * value)
{
	_value = value;
}


PgfCellRMarginFixed::PgfCellRMarginFixed(char const * value)
{
	_value = value;
}


PgfCellTMarginFixed::PgfCellTMarginFixed(char const * value)
{
	_value = value;
}


PgfCellBMarginFixed::PgfCellBMarginFixed(char const * value)
{
	_value = value;
}


PgfMinJRomanLetterSpace::PgfMinJRomanLetterSpace(int value)
{
	_value = value;
}


PgfOptJRomanLetterSpace::PgfOptJRomanLetterSpace(int value)
{
	_value = value;
}


PgfMaxJRomanLetterSpace::PgfMaxJRomanLetterSpace(int value)
{
	_value = value;
}


PgfMinJLetterSpace::PgfMinJLetterSpace(int value)
{
	_value = value;
}


PgfOptJLetterSpace::PgfOptJLetterSpace(int value)
{
	_value = value;
}


PgfMaxJLetterSpace::PgfMaxJLetterSpace(int value)
{
	_value = value;
}


PgfYakumonoType::PgfYakumonoType(char const * value)
{
	_value = value;
}


PgfAcrobatLevel::PgfAcrobatLevel( int value )
{
	_value = value;
}


PgfLocked::PgfLocked(char const * value)
{
	_value = value;
}


PgfNumString::PgfNumString(char const * value)
{
	_value = value;
}


PgfFontElement::PgfFontElement(FTag * element)
{
	_type = T_FTag;
	_ftag = element;
}


PgfFontElement::PgfFontElement(FPostScriptName * element)
{
	_type = T_FPostScriptName;
	_fpostscriptname = element;
}


PgfFontElement::PgfFontElement(FPlatformName * element)
{
	_type = T_FPlatformName;
	_fplatformname = element;
}


PgfFontElement::PgfFontElement(FFamily * element)
{
	_type = T_FFamily;
	_ffamily = element;
}


PgfFontElement::PgfFontElement(FVar * element)
{
	_type = T_FVar;
	_fvar = element;
}


PgfFontElement::PgfFontElement(FWeight * element)
{
	_type = T_FWeight;
	_fweight = element;
}


PgfFontElement::PgfFontElement(FAngle * element)
{
	_type = T_FAngle;
	_fangle = element;
}


PgfFontElement::PgfFontElement(FEncoding * element)
{
	_type = T_FEncoding;
	_fencoding = element;
}


PgfFontElement::PgfFontElement(FSize * element)
{
	_type = T_FSize;
	_fsize = element;
}


PgfFontElement::PgfFontElement(FUnderlining * element)
{
	_type = T_FUnderlining;
	_funderlining = element;
}


PgfFontElement::PgfFontElement(FOverline * element)
{
	_type = T_FOverline;
	_foverline = element;
}


PgfFontElement::PgfFontElement(FStrike * element)
{
	_type = T_FStrike;
	_fstrike = element;
}


PgfFontElement::PgfFontElement(FChangeBar * element)
{
	_type = T_FChangeBar;
	_fchangebar = element;
}


PgfFontElement::PgfFontElement(FOutline * element)
{
	_type = T_FOutline;
	_foutline = element;
}


PgfFontElement::PgfFontElement(FShadow * element)
{
	_type = T_FShadow;
	_fshadow = element;
}


PgfFontElement::PgfFontElement(FPairKern * element)
{
	_type = T_FPairKern;
	_fpairkern = element;
}


PgfFontElement::PgfFontElement(FTsume * element)
{
	_type = T_FTsume;
	_ftsume = element;
}


PgfFontElement::PgfFontElement(FCase * element)
{
	_type = T_FCase;
	_fcase = element;
}


PgfFontElement::PgfFontElement(FPosition * element)
{
	_type = T_FPosition;
	_fposition = element;
}


PgfFontElement::PgfFontElement(FDX_ * element)
{
	_type = T_FDX;
	_fdx = element;
}


PgfFontElement::PgfFontElement(FDY_ * element)
{
	_type = T_FDY;
	_fdy = element;
}


PgfFontElement::PgfFontElement(FDW_ * element)
{
	_type = T_FDW;
	_fdw = element;
}


PgfFontElement::PgfFontElement(FStretch * element)
{
	_type = T_FStretch;
	_fstretch = element;
}


PgfFontElement::PgfFontElement(FLanguage * element)
{
	_type = T_FLanguage;
	_flanguage = element;
}


PgfFontElement::PgfFontElement(FLocked * element)
{
	_type = T_FLocked;
	_flocked = element;
}


PgfFontElement::PgfFontElement(FSeparation * element)
{
	_type = T_FSeparation;
	_fseparation = element;
}


PgfFontElement::PgfFontElement(FColor * element)
{
	_type = T_FColor;
	_fcolor = element;
}



