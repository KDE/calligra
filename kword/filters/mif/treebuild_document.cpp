/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_document.h"

/*********************************************************************
 * This is where the document elements are collected during tree construction.
 ********************************************************************/
DocumentElementList documentelements;


DocumentElement::DocumentElement(DocumentAcrobatBookmarksIncludeTagNames *)
{
}


DocumentElement::DocumentElement(DocumentAcrobatParagraphBookmarks *)
{
}


DocumentElement::DocumentElement(DocumentAutoChBars *)
{
}


DocumentElement::DocumentElement(DocumentBordersOn *)
{
}


DocumentElement::DocumentElement(DocumentChBarColor *)
{
}


DocumentElement::DocumentElement(DocumentChBarGap *)
{
}


DocumentElement::DocumentElement(DocumentChBarPosition *)
{
}


DocumentElement::DocumentElement(DocumentChBarWidth *)
{
}


DocumentElement::DocumentElement(DocumentCurrentView *)
{
}


DocumentElement::DocumentElement(DocumentDisplayOverrides *)
{
}


DocumentElement::DocumentElement(DocumentFNoteAnchorPos *)
{
}


DocumentElement::DocumentElement(DocumentFNoteAnchorPrefix *)
{
}


DocumentElement::DocumentElement(DocumentFNoteAnchorSuffix *)
{
}


DocumentElement::DocumentElement(DocumentFNoteLabels *)
{
}


DocumentElement::DocumentElement(DocumentFNoteMaxH *)
{
}


DocumentElement::DocumentElement(DocumentFNoteNumStyle *)
{
}


DocumentElement::DocumentElement(DocumentFNoteNumberPos *)
{
}


DocumentElement::DocumentElement(DocumentFNoteNumberPrefix *)
{
}


DocumentElement::DocumentElement(DocumentFNoteNumberSuffix *)
{
}


DocumentElement::DocumentElement(DocumentFNoteRestart *)
{
}


DocumentElement::DocumentElement(DocumentFNoteTag *)
{
}


DocumentElement::DocumentElement(DocumentFluid *)
{
}


DocumentElement::DocumentElement(DocumentFluidSideHeads *)
{
}


DocumentElement::DocumentElement(DocumentFrozenPages *)
{
}


DocumentElement::DocumentElement(DocumentFullRulers *)
{
}


DocumentElement::DocumentElement(DocumentGenerateAcrobatInfo *)
{
}


DocumentElement::DocumentElement(DocumentGraphicsOff *)
{
}


DocumentElement::DocumentElement(DocumentGridOn *)
{
}


DocumentElement::DocumentElement(DocumentLanguage *)
{
}


DocumentElement::DocumentElement(DocumentLineBreakChars *)
{
}


DocumentElement::DocumentElement(DocumentLinkBoundariesOn *)
{
}


DocumentElement::DocumentElement(DocumentMathAlphaCharFontFamily *)
{
}


DocumentElement::DocumentElement(DocumentMathCatalog *)
{
}


DocumentElement::DocumentElement(DocumentMathFunctions *)
{
}


DocumentElement::DocumentElement(DocumentMathGreek *)
{
}


DocumentElement::DocumentElement(DocumentMathLargeHoriz *)
{
}


DocumentElement::DocumentElement(DocumentMathLargeIntegral *)
{
}


DocumentElement::DocumentElement(DocumentMathLargeLevel1 *)
{
}


DocumentElement::DocumentElement(DocumentMathLargeLevel2 *)
{
}


DocumentElement::DocumentElement(DocumentMathLargeLevel3 *)
{
}


DocumentElement::DocumentElement(DocumentMathLargeSigma *)
{
}


DocumentElement::DocumentElement(DocumentMathLargeVert *)
{
}


DocumentElement::DocumentElement(DocumentMathMediumHoriz *)
{
}


DocumentElement::DocumentElement(DocumentMathMediumIntegral *)
{
}


DocumentElement::DocumentElement(DocumentMathMediumLevel1 *)
{
}


DocumentElement::DocumentElement(DocumentMathMediumLevel2 *)
{
}


DocumentElement::DocumentElement(DocumentMathMediumLevel3 *)
{
}


DocumentElement::DocumentElement(DocumentMathMediumSigma *)
{
}


DocumentElement::DocumentElement(DocumentMathMediumVert *)
{
}


DocumentElement::DocumentElement(DocumentMathNumbers *)
{
}


DocumentElement::DocumentElement(DocumentMathShowCustom *)
{
}


DocumentElement::DocumentElement(DocumentMathSmallHoriz *)
{
}


DocumentElement::DocumentElement(DocumentMathSmallIntegral *)
{
}


DocumentElement::DocumentElement(DocumentMathSmallLevel1 *)
{
}


DocumentElement::DocumentElement(DocumentMathSmallLevel2 *)
{
}


DocumentElement::DocumentElement(DocumentMathSmallLevel3 *)
{
}


DocumentElement::DocumentElement(DocumentMathSmallSigma *)
{
}


DocumentElement::DocumentElement(DocumentMathSmallVert *)
{
}


DocumentElement::DocumentElement(DocumentMathStrings *)
{
}


DocumentElement::DocumentElement(DocumentMathVariables *)
{
}


DocumentElement::DocumentElement(DocumentMenuBar *)
{
}


DocumentElement::DocumentElement(DocumentNarrowRubiSpaceForJapanese *)
{
}


DocumentElement::DocumentElement(DocumentNarrowRubiSpaceForOther *)
{
}


DocumentElement::DocumentElement(DocumentNextUnique *)
{
}


DocumentElement::DocumentElement(DocumentPageGrid *)
{
}


DocumentElement::DocumentElement(DocumentPageNumStyle *)
{
}


DocumentElement::DocumentElement(DocumentPagePointStyle *)
{
}


DocumentElement::DocumentElement(DocumentPageRounding *)
{
}


DocumentElement::DocumentElement(DocumentPageScrolling *)
{
}


DocumentElement::DocumentElement(DocumentPageSize *)
{
}


DocumentElement::DocumentElement(DocumentParity *)
{
}


DocumentElement::DocumentElement(DocumentPrintSeparations *)
{
}


DocumentElement::DocumentElement(DocumentPrintSkipBlankPages *)
{
}


DocumentElement::DocumentElement(DocumentPunctuationChars *)
{
}


DocumentElement::DocumentElement(DocumentRubiAlignAtLineBounds *)
{
}


DocumentElement::DocumentElement(DocumentRubiOverhang *)
{
}


DocumentElement::DocumentElement(DocumentRubiSize *)
{
}


DocumentElement::DocumentElement(DocumentRulersOn *)
{
}


DocumentElement::DocumentElement(DocumentShowAllConditions *)
{
}


DocumentElement::DocumentElement(DocumentSmallCapsSize *)
{
}


DocumentElement::DocumentElement(DocumentSmallCapsStretch *)
{
}


DocumentElement::DocumentElement(DocumentSmartQuotesOn *)
{
}


DocumentElement::DocumentElement(DocumentSmartSpacesOn *)
{
}


DocumentElement::DocumentElement(DocumentSnapGrid *)
{
}


DocumentElement::DocumentElement(DocumentSnapRotation *)
{
}


DocumentElement::DocumentElement(DocumentStartPage *)
{
}


DocumentElement::DocumentElement(DocumentSubscriptShift *)
{
}


DocumentElement::DocumentElement(DocumentSubscriptSize *)
{
}


DocumentElement::DocumentElement(DocumentSubscriptStretch *)
{
}


DocumentElement::DocumentElement(DocumentSuperscriptShift *)
{
}


DocumentElement::DocumentElement(DocumentSuperscriptSize *)
{
}


DocumentElement::DocumentElement(DocumentSuperscriptStretch *)
{
}


DocumentElement::DocumentElement(DocumentSymbolsOn *)
{
}


DocumentElement::DocumentElement(DocumentTblFNoteAnchorPos *)
{
}


DocumentElement::DocumentElement(DocumentTblFNoteAnchorPrefix *)
{
}


DocumentElement::DocumentElement(DocumentTblFNoteAnchorSuffix *)
{
}


DocumentElement::DocumentElement(DocumentTblFNoteLabels *)
{
}


DocumentElement::DocumentElement(DocumentTblFNoteNumStyle *)
{
}


DocumentElement::DocumentElement(DocumentTblFNoteNumberPos *)
{
}


DocumentElement::DocumentElement(DocumentTblFNoteNumberPrefix *)
{
}


DocumentElement::DocumentElement(DocumentTblFNoteNumberSuffix *)
{
}


DocumentElement::DocumentElement(DocumentTblFNoteTag *)
{
}


DocumentElement::DocumentElement(DocumentTwoSides *)
{
}


DocumentElement::DocumentElement(DocumentUpdateTextInsetsOnOpen *)
{
}


DocumentElement::DocumentElement(DocumentUpdateXRefsOnOpen *)
{
}


DocumentElement::DocumentElement(DocumentViewOnly *)
{
}


DocumentElement::DocumentElement(DocumentViewOnlySelect *)
{
}


DocumentElement::DocumentElement(DocumentViewOnlyWinBorders *)
{
}


DocumentElement::DocumentElement(DocumentViewOnlyWinMenubar *)
{
}


DocumentElement::DocumentElement(DocumentViewOnlyWinPalette *)
{
}


DocumentElement::DocumentElement(DocumentViewOnlyWinPopup *)
{
}


DocumentElement::DocumentElement(DocumentViewOnlyXRef *)
{
}


DocumentElement::DocumentElement(DocumentViewRect *)
{
}


DocumentElement::DocumentElement(DocumentViewScale *)
{
}


DocumentElement::DocumentElement(DocumentVoMenuBar *)
{
}


DocumentElement::DocumentElement(DocumentWideRubiSpaceForJapanese *)
{
}


DocumentElement::DocumentElement(DocumentWideRubiSpaceForOther *)
{
}


DocumentElement::DocumentElement(DocumentWindowRect *)
{
}


DocumentElement::DocumentElement(FNoteStartNum *)
{
}


DocumentAcrobatBookmarksIncludeTagNames::DocumentAcrobatBookmarksIncludeTagNames(char const *)
{
}


DocumentAcrobatParagraphBookmarks::DocumentAcrobatParagraphBookmarks(char const *)
{
}


DocumentAutoChBars::DocumentAutoChBars(char const *)
{
}


DocumentBordersOn::DocumentBordersOn(char const *)
{
}


DocumentChBarColor::DocumentChBarColor(char const *)
{
}


DocumentChBarGap::DocumentChBarGap(double)
{
}


DocumentChBarPosition::DocumentChBarPosition(char const *)
{
}


DocumentChBarWidth::DocumentChBarWidth(double)
{
}


DocumentCurrentView::DocumentCurrentView(int)
{
}


DocumentDisplayOverrides::DocumentDisplayOverrides(char const *)
{
}


DocumentFNoteAnchorPos::DocumentFNoteAnchorPos(char const *)
{
}


DocumentFNoteAnchorPrefix::DocumentFNoteAnchorPrefix(char const *)
{
}


DocumentFNoteAnchorSuffix::DocumentFNoteAnchorSuffix(char const *)
{
}


DocumentFNoteLabels::DocumentFNoteLabels(char const *)
{
}


DocumentFNoteMaxH::DocumentFNoteMaxH(double)
{
}


DocumentFNoteNumStyle::DocumentFNoteNumStyle(char const *)
{
}


DocumentFNoteNumberPos::DocumentFNoteNumberPos(char const *)
{
}


DocumentFNoteNumberPrefix::DocumentFNoteNumberPrefix(char const *)
{
}


DocumentFNoteNumberSuffix::DocumentFNoteNumberSuffix(char const *)
{
}


DocumentFNoteRestart::DocumentFNoteRestart(char const *)
{
}


DocumentFNoteTag::DocumentFNoteTag(char const *)
{
}


DocumentFluid::DocumentFluid(char const *)
{
}


DocumentFluidSideHeads::DocumentFluidSideHeads(char const *)
{
}


DocumentFrozenPages::DocumentFrozenPages(char const *)
{
}


DocumentFullRulers::DocumentFullRulers(char const *)
{
}


DocumentGenerateAcrobatInfo::DocumentGenerateAcrobatInfo(char const *)
{
}


DocumentGraphicsOff::DocumentGraphicsOff(char const *)
{
}


DocumentGridOn::DocumentGridOn(char const *)
{
}


DocumentLanguage::DocumentLanguage(char const *)
{
}


DocumentLineBreakChars::DocumentLineBreakChars(char const *)
{
}


DocumentLinkBoundariesOn::DocumentLinkBoundariesOn(char const *)
{
}


DocumentMathAlphaCharFontFamily::DocumentMathAlphaCharFontFamily(char const *)
{
}


DocumentMathCatalog::DocumentMathCatalog(void)
{
}


DocumentMathFunctions::DocumentMathFunctions(char const *)
{
}


DocumentMathGreek::DocumentMathGreek(char const *)
{
}


DocumentMathLargeHoriz::DocumentMathLargeHoriz(double)
{
}


DocumentMathLargeIntegral::DocumentMathLargeIntegral(double)
{
}


DocumentMathLargeLevel1::DocumentMathLargeLevel1(double)
{
}


DocumentMathLargeLevel2::DocumentMathLargeLevel2(double)
{
}


DocumentMathLargeLevel3::DocumentMathLargeLevel3(double)
{
}


DocumentMathLargeSigma::DocumentMathLargeSigma(double)
{
}


DocumentMathLargeVert::DocumentMathLargeVert(double)
{
}


DocumentMathMediumHoriz::DocumentMathMediumHoriz(double)
{
}


DocumentMathMediumIntegral::DocumentMathMediumIntegral(double)
{
}


DocumentMathMediumLevel1::DocumentMathMediumLevel1(double)
{
}


DocumentMathMediumLevel2::DocumentMathMediumLevel2(double)
{
}


DocumentMathMediumLevel3::DocumentMathMediumLevel3(double)
{
}


DocumentMathMediumSigma::DocumentMathMediumSigma(double)
{
}


DocumentMathMediumVert::DocumentMathMediumVert(double)
{
}


DocumentMathNumbers::DocumentMathNumbers(char const *)
{
}


DocumentMathShowCustom::DocumentMathShowCustom(char const *)
{
}


DocumentMathSmallHoriz::DocumentMathSmallHoriz(double)
{
}


DocumentMathSmallIntegral::DocumentMathSmallIntegral(double)
{
}


DocumentMathSmallLevel1::DocumentMathSmallLevel1(double)
{
}


DocumentMathSmallLevel2::DocumentMathSmallLevel2(double)
{
}


DocumentMathSmallLevel3::DocumentMathSmallLevel3(double)
{
}


DocumentMathSmallSigma::DocumentMathSmallSigma(double)
{
}


DocumentMathSmallVert::DocumentMathSmallVert(double)
{
}


DocumentMathStrings::DocumentMathStrings(char const *)
{
}


DocumentMathVariables::DocumentMathVariables(char const *)
{
}


DocumentMenuBar::DocumentMenuBar(char const *)
{
}


DocumentNarrowRubiSpaceForJapanese::DocumentNarrowRubiSpaceForJapanese(char const *)
{
}


DocumentNarrowRubiSpaceForOther::DocumentNarrowRubiSpaceForOther(char const *)
{
}


DocumentNextUnique::DocumentNextUnique(int)
{
}


DocumentPageGrid::DocumentPageGrid(double)
{
}


DocumentPageNumStyle::DocumentPageNumStyle(char const *)
{
}


DocumentPagePointStyle::DocumentPagePointStyle(char const *)
{
}


DocumentPageRounding::DocumentPageRounding(char const *)
{
}


DocumentPageScrolling::DocumentPageScrolling(int)
{
}


DocumentPageSize::DocumentPageSize(double, double)
{
}


DocumentParity::DocumentParity(char const *)
{
}


DocumentPrintSeparations::DocumentPrintSeparations(char const *)
{
}


DocumentPrintSkipBlankPages::DocumentPrintSkipBlankPages(char const *)
{
}


DocumentPunctuationChars::DocumentPunctuationChars(char const *)
{
}


DocumentRubiAlignAtLineBounds::DocumentRubiAlignAtLineBounds(char const *)
{
}


DocumentRubiOverhang::DocumentRubiOverhang(char const *)
{
}


DocumentRubiSize::DocumentRubiSize(double)
{
}


DocumentRulersOn::DocumentRulersOn(char const *)
{
}


DocumentShowAllConditions::DocumentShowAllConditions(char const *)
{
}


DocumentSmallCapsSize::DocumentSmallCapsSize(double)
{
}


DocumentSmallCapsStretch::DocumentSmallCapsStretch(double)
{
}


DocumentSmartQuotesOn::DocumentSmartQuotesOn(char const *)
{
}


DocumentSmartSpacesOn::DocumentSmartSpacesOn(char const *)
{
}


DocumentSnapGrid::DocumentSnapGrid(double)
{
}


DocumentSnapRotation::DocumentSnapRotation(double)
{
}


DocumentStartPage::DocumentStartPage(int)
{
}


DocumentSubscriptShift::DocumentSubscriptShift(double)
{
}


DocumentSubscriptSize::DocumentSubscriptSize(double)
{
}


DocumentSubscriptStretch::DocumentSubscriptStretch(double)
{
}


DocumentSuperscriptShift::DocumentSuperscriptShift(double)
{
}


DocumentSuperscriptSize::DocumentSuperscriptSize(double)
{
}


DocumentSuperscriptStretch::DocumentSuperscriptStretch(double)
{
}


DocumentSymbolsOn::DocumentSymbolsOn(char const *)
{
}


DocumentTblFNoteAnchorPos::DocumentTblFNoteAnchorPos(char const *)
{
}


DocumentTblFNoteAnchorPrefix::DocumentTblFNoteAnchorPrefix(char const *)
{
}


DocumentTblFNoteAnchorSuffix::DocumentTblFNoteAnchorSuffix(char const *)
{
}


DocumentTblFNoteLabels::DocumentTblFNoteLabels(char const *)
{
}


DocumentTblFNoteNumStyle::DocumentTblFNoteNumStyle(char const *)
{
}


DocumentTblFNoteNumberPos::DocumentTblFNoteNumberPos(char const *)
{
}


DocumentTblFNoteNumberPrefix::DocumentTblFNoteNumberPrefix(char const *)
{
}


DocumentTblFNoteNumberSuffix::DocumentTblFNoteNumberSuffix(char const *)
{
}


DocumentTblFNoteTag::DocumentTblFNoteTag(char const *)
{
}


DocumentTwoSides::DocumentTwoSides(char const *)
{
}


DocumentUpdateTextInsetsOnOpen::DocumentUpdateTextInsetsOnOpen(char const *)
{
}


DocumentUpdateXRefsOnOpen::DocumentUpdateXRefsOnOpen(char const *)
{
}


DocumentViewOnly::DocumentViewOnly(char const *)
{
}


DocumentViewOnlySelect::DocumentViewOnlySelect(char const *)
{
}


DocumentViewOnlyWinBorders::DocumentViewOnlyWinBorders(char const *)
{
}


DocumentViewOnlyWinMenubar::DocumentViewOnlyWinMenubar(char const *)
{
}


DocumentViewOnlyWinPalette::DocumentViewOnlyWinPalette(char const *)
{
}


DocumentViewOnlyWinPopup::DocumentViewOnlyWinPopup(char const *)
{
}


DocumentViewOnlyXRef::DocumentViewOnlyXRef(char const *)
{
}


DocumentViewRect::DocumentViewRect(int, int, int, int)
{
}


DocumentViewScale::DocumentViewScale(double)
{
}


DocumentVoMenuBar::DocumentVoMenuBar(char const *)
{
}


DocumentWideRubiSpaceForJapanese::DocumentWideRubiSpaceForJapanese(char const *)
{
}


DocumentWideRubiSpaceForOther::DocumentWideRubiSpaceForOther(char const *)
{
}


DocumentWindowRect::DocumentWindowRect(int, int, int, int)
{
}


FNoteStartNum::FNoteStartNum(int)
{
}



