/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_document.h"
#include "unitconv.h"

/*********************************************************************
 * This is where the document elements are collected during tree construction.
 ********************************************************************/
DocumentElementList documentelements;


DocumentElement::DocumentElement(DocumentAcrobatBookmarksIncludeTagNames * element)
{
	_type = T_DocumentAcrobatBookmarksIncludeTagNames;
	_documentacrobatbookmarksincludetagnames = element;
}


DocumentElement::DocumentElement(DocumentAcrobatParagraphBookmarks * element)
{
	_type = T_DocumentAcrobatParagraphBookmarks;
	_documentacrobatparagraphbookmarks = element;
}


DocumentElement::DocumentElement(DocumentAutoChBars * element)
{
	_type = T_DocumentAutoChBars;
	_documentautochbars = element;
}


DocumentElement::DocumentElement(DocumentBordersOn * element)
{
	_type = T_DocumentBordersOn;
	_documentborderson = element;
}


DocumentElement::DocumentElement(DocumentChBarColor * element)
{
	_type = T_DocumentChBarColor;
	_documentchbarcolor = element;
}


DocumentElement::DocumentElement(DocumentChBarGap * element)
{
	_type = T_DocumentChBarGap;
	_documentchbargap = element;
}


DocumentElement::DocumentElement(DocumentChBarPosition * element)
{
	_type = T_DocumentChBarPosition;
	_documentchbarposition = element;
}


DocumentElement::DocumentElement(DocumentChBarWidth * element)
{
	_type = T_DocumentChBarWidth;
	_documentchbarwidth = element;
}


DocumentElement::DocumentElement(DocumentCurrentView * element)
{
	_type = T_DocumentCurrentView;
	_documentcurrentview = element;
}


DocumentElement::DocumentElement(DocumentDisplayOverrides * element)
{
	_type = T_DocumentDisplayOverrides;
	_documentdisplayoverrides = element;
}


DocumentElement::DocumentElement(DocumentFNoteAnchorPos * element)
{
	_type = T_DocumentFNoteAnchorPos;
	_documentfnoteanchorpos = element;
}


DocumentElement::DocumentElement(DocumentFNoteAnchorPrefix * element)
{
	_type = T_DocumentFNoteAnchorPrefix;
	_documentfnoteanchorprefix = element;
}


DocumentElement::DocumentElement(DocumentFNoteAnchorSuffix * element)
{
	_type = T_DocumentFNoteAnchorSuffix;
	_documentfnoteanchorsuffix = element;
}


DocumentElement::DocumentElement(DocumentFNoteLabels * element)
{
	_type = T_DocumentFNoteLabels;
	_documentfnotelabels = element;
}


DocumentElement::DocumentElement(DocumentFNoteMaxH * element)
{
	_type = T_DocumentFNoteMaxH;
	_documentfnotemaxh = element;
}


DocumentElement::DocumentElement(DocumentFNoteNumStyle * element)
{
	_type = T_DocumentFNoteNumStyle;
	_documentfnotenumstyle = element;
}


DocumentElement::DocumentElement(DocumentFNoteNumberPos * element)
{
	_type = T_DocumentFNoteNumberPos;
	_documentfnotenumberpos = element;
}


DocumentElement::DocumentElement(DocumentFNoteNumberPrefix * element)
{
	_type = T_DocumentFNoteNumberPrefix;
	_documentfnotenumberprefix = element;
}


DocumentElement::DocumentElement(DocumentFNoteNumberSuffix * element)
{
	_type = T_DocumentFNoteNumberSuffix;
	_documentfnotenumbersuffix = element;
}


DocumentElement::DocumentElement(DocumentFNoteRestart * element)
{
	_type = T_DocumentFNoteRestart;
	_documentfnoterestart = element;
}


DocumentElement::DocumentElement(DocumentFNoteTag * element)
{
	_type = T_DocumentFNoteTag;
	_documentfnotetag = element;
}


DocumentElement::DocumentElement(DocumentFluid * element)
{
	_type = T_DocumentFluid;
	_documentfluid = element;
}


DocumentElement::DocumentElement(DocumentFluidSideHeads * element)
{
	_type = T_DocumentFluidSideHeads;
	_documentfluidsideheads = element;
}


DocumentElement::DocumentElement(DocumentFrozenPages * element)
{
	_type = T_DocumentFrozenPages;
	_documentfrozenpages = element;
}


DocumentElement::DocumentElement(DocumentFullRulers * element)
{
	_type = T_DocumentFullRulers;
	_documentfullrulers = element;
}


DocumentElement::DocumentElement(DocumentGenerateAcrobatInfo * element)
{
	_type = T_DocumentGenerateAcrobatInfo;
	_documentgenerateacrobatinfo = element;
}


DocumentElement::DocumentElement(DocumentGraphicsOff * element)
{
	_type = T_DocumentGraphicsOff;
	_documentgraphicsoff = element;
}


DocumentElement::DocumentElement(DocumentGridOn * element)
{
	_type = T_DocumentGridOn;
	_documentgridon = element;
}


DocumentElement::DocumentElement(DocumentLanguage * element)
{
	_type = T_DocumentLanguage;
	_documentlanguage = element;
}


DocumentElement::DocumentElement(DocumentLineBreakChars * element)
{
	_type = T_DocumentLineBreakChars;
	_documentlinebreakchars = element;
}


DocumentElement::DocumentElement(DocumentLinkBoundariesOn * element)
{
	_type = T_DocumentLinkBoundariesOn;
	_documentlinkboundarieson = element;
}


DocumentElement::DocumentElement(DocumentMathAlphaCharFontFamily * element)
{
	_type = T_DocumentMathAlphaCharFontFamily;
	_documentmathalphacharfontfamily = element;
}


DocumentElement::DocumentElement(DocumentMathCatalog * element)
{
	_type = T_DocumentMathCatalog;
	_documentmathcatalog = element;
}


DocumentElement::DocumentElement(DocumentMathFunctions * element)
{
	_type = T_DocumentMathFunctions;
	_documentmathfunctions = element;
}


DocumentElement::DocumentElement(DocumentMathGreek * element)
{
	_type = T_DocumentMathGreek;
	_documentmathgreek = element;
}


DocumentElement::DocumentElement(DocumentMathLargeHoriz * element)
{
	_type = T_DocumentMathLargeHoriz;
	_documentmathlargehoriz = element;
}


DocumentElement::DocumentElement(DocumentMathLargeIntegral * element)
{
	_type = T_DocumentMathLargeIntegral;
	_documentmathlargeintegral = element;
}


DocumentElement::DocumentElement(DocumentMathLargeLevel1 * element)
{
	_type = T_DocumentMathLargeLevel1;
	_documentmathlargelevel1 = element;
}


DocumentElement::DocumentElement(DocumentMathLargeLevel2 * element)
{
	_type = T_DocumentMathLargeLevel2;
	_documentmathlargelevel2 = element;
}


DocumentElement::DocumentElement(DocumentMathLargeLevel3 * element)
{
	_type = T_DocumentMathLargeLevel3;
	_documentmathlargelevel3 = element;
}


DocumentElement::DocumentElement(DocumentMathLargeSigma * element)
{
	_type = T_DocumentMathLargeSigma;
	_documentmathlargesigma = element;
}


DocumentElement::DocumentElement(DocumentMathLargeVert * element)
{
	_type = T_DocumentMathLargeVert;
	_documentmathlargevert = element;
}


DocumentElement::DocumentElement(DocumentMathMediumHoriz * element)
{
	_type = T_DocumentMathMediumHoriz;
	_documentmathmediumhoriz = element;
}


DocumentElement::DocumentElement(DocumentMathMediumIntegral * element)
{
	_type = T_DocumentMathMediumIntegral;
	_documentmathmediumintegral = element;
}


DocumentElement::DocumentElement(DocumentMathMediumLevel1 * element)
{
	_type = T_DocumentMathMediumLevel1;
	_documentmathmediumlevel1 = element;
}


DocumentElement::DocumentElement(DocumentMathMediumLevel2 * element)
{
	_type = T_DocumentMathMediumLevel2;
	_documentmathmediumlevel2 = element;
}


DocumentElement::DocumentElement(DocumentMathMediumLevel3 * element)
{
	_type = T_DocumentMathMediumLevel3;
	_documentmathmediumlevel3 = element;
}


DocumentElement::DocumentElement(DocumentMathMediumSigma * element)
{
	_type = T_DocumentMathMediumSigma;
	_documentmathmediumsigma = element;
}


DocumentElement::DocumentElement(DocumentMathMediumVert * element)
{
	_type = T_DocumentMathMediumVert;
	_documentmathmediumvert = element;
}


DocumentElement::DocumentElement(DocumentMathNumbers * element)
{
	_type = T_DocumentMathNumbers;
	_documentmathnumbers = element;
}


DocumentElement::DocumentElement(DocumentMathShowCustom * element)
{
	_type = T_DocumentMathShowCustom;
	_documentmathshowcustom = element;
}


DocumentElement::DocumentElement(DocumentMathSmallHoriz * element)
{
	_type = T_DocumentMathSmallHoriz;
	_documentmathsmallhoriz = element;
}


DocumentElement::DocumentElement(DocumentMathSmallIntegral * element)
{
	_type = T_DocumentMathSmallIntegral;
	_documentmathsmallintegral = element;
}


DocumentElement::DocumentElement(DocumentMathSmallLevel1 * element)
{
	_type = T_DocumentMathSmallLevel1;
	_documentmathsmalllevel1 = element;
}


DocumentElement::DocumentElement(DocumentMathSmallLevel2 * element)
{
	_type = T_DocumentMathSmallLevel2;
	_documentmathsmalllevel2 = element;
}


DocumentElement::DocumentElement(DocumentMathSmallLevel3 * element)
{
	_type = T_DocumentMathSmallLevel3;
	_documentmathsmalllevel3 = element;
}


DocumentElement::DocumentElement(DocumentMathSmallSigma * element)
{
	_type = T_DocumentMathSmallSigma;
	_documentmathsmallsigma = element;
}


DocumentElement::DocumentElement(DocumentMathSmallVert * element)
{
	_type = T_DocumentMathSmallVert;
	_documentmathsmallvert = element;
}


DocumentElement::DocumentElement(DocumentMathStrings * element)
{
	_type = T_DocumentMathStrings;
	_documentmathstrings = element;
}


DocumentElement::DocumentElement(DocumentMathVariables * element)
{
	_type = T_DocumentMathVariables;
	_documentmathvariables = element;
}


DocumentElement::DocumentElement(DocumentMenuBar * element)
{
	_type = T_DocumentMenuBar;
	_documentmenubar = element;
}


DocumentElement::DocumentElement(DocumentNarrowRubiSpaceForJapanese * element)
{
	_type = T_DocumentNarrowRubiSpaceForJapanese;
	_documentnarrowrubispaceforjapanese = element;
}


DocumentElement::DocumentElement(DocumentNarrowRubiSpaceForOther * element)
{
	_type = T_DocumentNarrowRubiSpaceForOther;
	_documentnarrowrubispaceforother = element;
}


DocumentElement::DocumentElement(DocumentNextUnique * element)
{
	_type = T_DocumentNextUnique;
	_documentnextunique = element;
}


DocumentElement::DocumentElement(DocumentPageGrid * element)
{
	_type = T_DocumentPageGrid;
	_documentpagegrid = element;
}


DocumentElement::DocumentElement(DocumentPageNumStyle * element)
{
	_type = T_DocumentPageNumStyle;
	_documentpagenumstyle = element;
}


DocumentElement::DocumentElement(DocumentPagePointStyle * element)
{
	_type = T_DocumentPagePointStyle;
	_documentpagepointstyle = element;
}


DocumentElement::DocumentElement(DocumentPageRounding * element)
{
	_type = T_DocumentPageRounding;
	_documentpagerounding = element;
}


DocumentElement::DocumentElement(DocumentPageScrolling * element)
{
	_type = T_DocumentPageScrolling;
	_documentpagescrolling = element;
}


DocumentElement::DocumentElement(DocumentPageSize * element)
{
	_type = T_DocumentPageSize;
	_documentpagesize = element;
}


DocumentElement::DocumentElement(DocumentParity * element)
{
	_type = T_DocumentParity;
	_documentparity = element;
}


DocumentElement::DocumentElement(DocumentPrintSeparations * element)
{
	_type = T_DocumentPrintSeparations;
	_documentprintseparations = element;
}


DocumentElement::DocumentElement(DocumentPrintSkipBlankPages * element)
{
	_type = T_DocumentPrintSkipBlankPages;
	_documentprintskipblankpages = element;
}


DocumentElement::DocumentElement(DocumentPunctuationChars * element)
{
	_type = T_DocumentPunctuationChars;
	_documentpunctuationchars = element;
}


DocumentElement::DocumentElement(DocumentRubiAlignAtLineBounds * element)
{
	_type = T_DocumentRubiAlignAtLineBounds;
	_documentrubialignatlinebounds = element;
}


DocumentElement::DocumentElement(DocumentRubiOverhang * element)
{
	_type = T_DocumentRubiOverhang;
	_documentrubioverhang = element;
}


DocumentElement::DocumentElement(DocumentRubiSize * element)
{
	_type = T_DocumentRubiSize;
	_documentrubisize = element;
}


DocumentElement::DocumentElement(DocumentRulersOn * element)
{
	_type = T_DocumentRulersOn;
	_documentrulerson = element;
}


DocumentElement::DocumentElement(DocumentShowAllConditions * element)
{
	_type = T_DocumentShowAllConditions;
	_documentshowallconditions = element;
}


DocumentElement::DocumentElement(DocumentSmallCapsSize * element)
{
	_type = T_DocumentSmallCapsSize;
	_documentsmallcapssize = element;
}


DocumentElement::DocumentElement(DocumentSmallCapsStretch * element)
{
	_type = T_DocumentSmallCapsStretch;
	_documentsmallcapsstretch = element;
}


DocumentElement::DocumentElement(DocumentSmartQuotesOn * element)
{
	_type = T_DocumentSmartQuotesOn;
	_documentsmartquoteson = element;
}


DocumentElement::DocumentElement(DocumentSmartSpacesOn * element)
{
	_type = T_DocumentSmartSpacesOn;
	_documentsmartspaceson = element;
}


DocumentElement::DocumentElement(DocumentSnapGrid * element)
{
	_type = T_DocumentSnapGrid;
	_documentsnapgrid = element;
}


DocumentElement::DocumentElement(DocumentSnapRotation * element)
{
	_type = T_DocumentSnapRotation;
	_documentsnaprotation = element;
}


DocumentElement::DocumentElement(DocumentStartPage * element)
{
	_type = T_DocumentStartPage;
	_documentstartpage = element;
}


DocumentElement::DocumentElement(DocumentSubscriptShift * element)
{
	_type = T_DocumentSubscriptShift;
	_documentsubscriptshift = element;
}


DocumentElement::DocumentElement(DocumentSubscriptSize * element)
{
	_type = T_DocumentSubscriptSize;
	_documentsubscriptsize = element;
}


DocumentElement::DocumentElement(DocumentSubscriptStretch * element)
{
	_type = T_DocumentSubscriptStretch;
	_documentsubscriptstretch = element;
}


DocumentElement::DocumentElement(DocumentSuperscriptShift * element)
{
	_type = T_DocumentSuperscriptShift;
	_documentsuperscriptshift = element;
}


DocumentElement::DocumentElement(DocumentSuperscriptSize * element)
{
	_type = T_DocumentSuperscriptSize;
	_documentsuperscriptsize = element;
}


DocumentElement::DocumentElement(DocumentSuperscriptStretch * element)
{
	_type = T_DocumentSuperscriptStretch;
	_documentsuperscriptstretch = element;
}


DocumentElement::DocumentElement(DocumentSymbolsOn * element)
{
	_type = T_DocumentSymbolsOn;
	_documentsymbolson = element;
}


DocumentElement::DocumentElement(DocumentTblFNoteAnchorPos * element)
{
	_type = T_DocumentTblFNoteAnchorPos;
	_documenttblfnoteanchorpos = element;
}


DocumentElement::DocumentElement(DocumentTblFNoteAnchorPrefix * element)
{
	_type = T_DocumentTblFNoteAnchorPrefix;
	_documenttblfnoteanchorprefix = element;
}


DocumentElement::DocumentElement(DocumentTblFNoteAnchorSuffix * element)
{
	_type = T_DocumentTblFNoteAnchorSuffix;
	_documenttblfnoteanchorsuffix = element;
}


DocumentElement::DocumentElement(DocumentTblFNoteLabels * element)
{
	_type = T_DocumentTblFNoteLabels;
	_documenttblfnotelabels = element;
}


DocumentElement::DocumentElement(DocumentTblFNoteNumStyle * element)
{
	_type = T_DocumentTblFNoteNumStyle;
	_documenttblfnotenumstyle = element;
}


DocumentElement::DocumentElement(DocumentTblFNoteNumberPos * element)
{
	_type = T_DocumentTblFNoteNumberPos;
	_documenttblfnotenumberpos = element;
}


DocumentElement::DocumentElement(DocumentTblFNoteNumberPrefix * element)
{
	_type = T_DocumentTblFNoteNumberPrefix;
	_documenttblfnotenumberprefix = element;
}


DocumentElement::DocumentElement(DocumentTblFNoteNumberSuffix * element)
{
	_type = T_DocumentFNoteNumberSuffix;
	_documenttblfnotenumbersuffix = element;
}


DocumentElement::DocumentElement(DocumentTblFNoteTag * element)
{
	_type = T_DocumentTblFNoteTag;
	_documenttblfnotetag = element;
}


DocumentElement::DocumentElement(DocumentTwoSides * element)
{
	_type = T_DocumentTwoSides;
	_documenttwosides = element;
}


DocumentElement::DocumentElement(DocumentUpdateTextInsetsOnOpen * element)
{
	_type = T_DocumentUpdateTextInsetsOnOpen;
	_documentupdatetextinsetsonopen = element;
}


DocumentElement::DocumentElement(DocumentUpdateXRefsOnOpen * element)
{
	_type = T_DocumentUpdateXRefsOnOpen;
	_documentupdatexrefsonopen = element;
}


DocumentElement::DocumentElement(DocumentViewOnly * element)
{
	_type = T_DocumentViewOnly;
	_documentviewonly = element;
}


DocumentElement::DocumentElement(DocumentViewOnlySelect * element)
{
	_type = T_DocumentViewOnlySelect;
	_documentviewonlyselect = element;
}


DocumentElement::DocumentElement(DocumentViewOnlyWinBorders * element)
{
	_type = T_DocumentViewOnlyWinBorders;
	_documentviewonlywinborders = element;
}


DocumentElement::DocumentElement(DocumentViewOnlyWinMenubar * element)
{
	_type = T_DocumentViewOnlyWinMenubar;
	_documentviewonlywinmenubar = element;
}


DocumentElement::DocumentElement(DocumentViewOnlyWinPalette * element)
{
	_type = T_DocumentViewOnlyWinPalette;
	_documentviewonlywinpalette = element;
}


DocumentElement::DocumentElement(DocumentViewOnlyWinPopup * element)
{
	_type = T_DocumentViewOnlyWinPopup;
	_documentviewonlywinpopup = element;
}


DocumentElement::DocumentElement(DocumentViewOnlyXRef * element)
{
	_type = T_DocumentViewOnlyXRef;
	_documentviewonlyxref = element;
}


DocumentElement::DocumentElement(DocumentViewRect * element)
{
	_type = T_DocumentViewRect;
	_documentviewrect = element;
}


DocumentElement::DocumentElement(DocumentViewScale * element)
{
	_type = T_DocumentViewScale;
	_documentviewscale = element;
}


DocumentElement::DocumentElement(DocumentVoMenuBar * element)
{
	_type = T_DocumentVoMenuBar;
	_documentvomenubar = element;
}


DocumentElement::DocumentElement(DocumentWideRubiSpaceForJapanese * element)
{
	_type = T_DocumentWideRubiSpaceForJapanese;
	_documentwiderubispaceforjapanese = element;
}


DocumentElement::DocumentElement(DocumentWideRubiSpaceForOther * element)
{
	_type = T_DocumentWideRubiSpaceForOther;
	_documentwiderubispaceforother = element;
}


DocumentElement::DocumentElement(DocumentWindowRect * element)
{
	_type = T_DocumentWindowRect;
	_documentwindowrect = element;
}


DocumentElement::DocumentElement(FNoteStartNum * element)
{
	_type = T_FNoteStartNum;
	_fnotestartnum = element;
}


DocumentAcrobatBookmarksIncludeTagNames::DocumentAcrobatBookmarksIncludeTagNames(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentAcrobatParagraphBookmarks::DocumentAcrobatParagraphBookmarks(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentAutoChBars::DocumentAutoChBars(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentBordersOn::DocumentBordersOn(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentChBarColor::DocumentChBarColor(char const * value)
{
	_value = value;
}


DocumentChBarGap::DocumentChBarGap( double value, const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentChBarPosition::DocumentChBarPosition(char const * value)
{
	_value = value;
}


DocumentChBarWidth::DocumentChBarWidth( double value, const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentCurrentView::DocumentCurrentView( int value )
{
	_value = value;
}


DocumentDisplayOverrides::DocumentDisplayOverrides(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentFNoteAnchorPos::DocumentFNoteAnchorPos(char const * value)
{
	_value = value;
}


DocumentFNoteAnchorPrefix::DocumentFNoteAnchorPrefix(char const * value)
{
	_value = value;
}


DocumentFNoteAnchorSuffix::DocumentFNoteAnchorSuffix(char const * value)
{
	_value = value;
}


DocumentFNoteLabels::DocumentFNoteLabels(char const * value)
{
	_value = value;
}


DocumentFNoteMaxH::DocumentFNoteMaxH( double value, const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentFNoteNumStyle::DocumentFNoteNumStyle(char const * value)
{
	_value = value;
}


DocumentFNoteNumberPos::DocumentFNoteNumberPos(char const * value)
{
	_value = value;
}


DocumentFNoteNumberPrefix::DocumentFNoteNumberPrefix(char const * value)
{
	_value = value;
}


DocumentFNoteNumberSuffix::DocumentFNoteNumberSuffix(char const * value)
{
	_value = value;
}


DocumentFNoteRestart::DocumentFNoteRestart(char const * value)
{
	_value = value;
}


DocumentFNoteTag::DocumentFNoteTag(char const * value)
{
	_value = value;
}


DocumentFluid::DocumentFluid(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentFluidSideHeads::DocumentFluidSideHeads(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentFrozenPages::DocumentFrozenPages(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentFullRulers::DocumentFullRulers(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentGenerateAcrobatInfo::DocumentGenerateAcrobatInfo(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentGraphicsOff::DocumentGraphicsOff(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentGridOn::DocumentGridOn(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentLanguage::DocumentLanguage(char const * value)
{
	_value = value;
}


DocumentLineBreakChars::DocumentLineBreakChars(char const * value)
{
	_value = value;
}


DocumentLinkBoundariesOn::DocumentLinkBoundariesOn(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentMathAlphaCharFontFamily::DocumentMathAlphaCharFontFamily(char const * value)
{
	_value = value;
}


DocumentMathCatalog::DocumentMathCatalog(void)
{
}


DocumentMathFunctions::DocumentMathFunctions(char const * value)
{
	_value = value;
}


DocumentMathGreek::DocumentMathGreek(char const * value)
{
	_value = value;
}


DocumentMathLargeHoriz::DocumentMathLargeHoriz( double value, 
												const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathLargeIntegral::DocumentMathLargeIntegral( double value, 
													  const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathLargeLevel1::DocumentMathLargeLevel1( double value, 
												  const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathLargeLevel2::DocumentMathLargeLevel2( double value,
												  const char* unit)
{
	_value = unitconversion( value, unit );
}


DocumentMathLargeLevel3::DocumentMathLargeLevel3( double value, 
												  const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathLargeSigma::DocumentMathLargeSigma( double value, 
												const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathLargeVert::DocumentMathLargeVert( double value, const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathMediumHoriz::DocumentMathMediumHoriz( double value, 
												  const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathMediumIntegral::DocumentMathMediumIntegral( double value, 
														const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathMediumLevel1::DocumentMathMediumLevel1( double value, 
													const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathMediumLevel2::DocumentMathMediumLevel2( double value, 
													const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathMediumLevel3::DocumentMathMediumLevel3( double value, 
													const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathMediumSigma::DocumentMathMediumSigma( double value, 
												  const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathMediumVert::DocumentMathMediumVert( double value, 
												const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathNumbers::DocumentMathNumbers(char const * value)
{
	_value = value;
}


DocumentMathShowCustom::DocumentMathShowCustom(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentMathSmallHoriz::DocumentMathSmallHoriz( double value, 
												const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathSmallIntegral::DocumentMathSmallIntegral( double value, 
													  const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathSmallLevel1::DocumentMathSmallLevel1( double value, 
												  const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathSmallLevel2::DocumentMathSmallLevel2( double value, 
												  const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathSmallLevel3::DocumentMathSmallLevel3( double value, 
												  const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathSmallSigma::DocumentMathSmallSigma( double value, 
												const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathSmallVert::DocumentMathSmallVert( double value, 
											  const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentMathStrings::DocumentMathStrings(char const * value)
{
	_value = value;
}


DocumentMathVariables::DocumentMathVariables(char const * value)
{
	_value = value;
}


DocumentMenuBar::DocumentMenuBar(char const * value)
{
	_value = value;
}


DocumentNarrowRubiSpaceForJapanese::DocumentNarrowRubiSpaceForJapanese(char const * value)
{
	_value = value;
}


DocumentNarrowRubiSpaceForOther::DocumentNarrowRubiSpaceForOther(char const * value)
{
	_value = value;
}


DocumentNextUnique::DocumentNextUnique( int value )
{
	_value = value;
}


DocumentPageGrid::DocumentPageGrid( double value, const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentPageNumStyle::DocumentPageNumStyle(char const * value)
{
	_value = value;
}


DocumentPagePointStyle::DocumentPagePointStyle(char const * value)
{
	_value = value;
}


DocumentPageRounding::DocumentPageRounding(char const * value)
{
	_value = value;
}


DocumentPageScrolling::DocumentPageScrolling( int value )
{
	_type = DPS_INT;
	_intvalue = value;
}

DocumentPageSize::DocumentPageSize( double width, const char* unitw, 
									double height, const char* unith )
{
	_width = unitconversion( width, unitw );
	_height = unitconversion( height, unith );
}


DocumentParity::DocumentParity(char const * value)
{
	_value = value;
}


DocumentPrintSeparations::DocumentPrintSeparations(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentPrintSkipBlankPages::DocumentPrintSkipBlankPages(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentPunctuationChars::DocumentPunctuationChars(char const * value)
{
	_value = value;
}


DocumentRubiAlignAtLineBounds::DocumentRubiAlignAtLineBounds(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentRubiOverhang::DocumentRubiOverhang(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentRubiSize::DocumentRubiSize( double value )
{
	_value = value;
}


DocumentRulersOn::DocumentRulersOn(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentShowAllConditions::DocumentShowAllConditions(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentSmallCapsSize::DocumentSmallCapsSize( double value )
{
	_value = value;
}


DocumentSmallCapsStretch::DocumentSmallCapsStretch( double value )
{
	_value = value;
}


DocumentSmartQuotesOn::DocumentSmartQuotesOn(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentSmartSpacesOn::DocumentSmartSpacesOn(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentSnapGrid::DocumentSnapGrid( double value, const char* unit )
{
	_value = unitconversion( value, unit );
}


DocumentSnapRotation::DocumentSnapRotation( double value )
{
	_value = value;
}


DocumentStartPage::DocumentStartPage( int value )
{
	_value = value;
}


DocumentSubscriptShift::DocumentSubscriptShift( double value )
{
	_value = value;
}


DocumentSubscriptSize::DocumentSubscriptSize( double value )
{
	_value = value;
}


DocumentSubscriptStretch::DocumentSubscriptStretch( double value )
{
	_value = value;
}


DocumentSuperscriptShift::DocumentSuperscriptShift( double value )
{
	_value = value;
}


DocumentSuperscriptSize::DocumentSuperscriptSize( double value )
{
	_value = value;
}


DocumentSuperscriptStretch::DocumentSuperscriptStretch( double value )
{
	_value = value;
}


DocumentSymbolsOn::DocumentSymbolsOn(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentTblFNoteAnchorPos::DocumentTblFNoteAnchorPos(char const * value)
{
	_value = value;
}


DocumentTblFNoteAnchorPrefix::DocumentTblFNoteAnchorPrefix(char const * value)
{
	_value = value;
}


DocumentTblFNoteAnchorSuffix::DocumentTblFNoteAnchorSuffix(char const * value)
{
	_value = value;
}


DocumentTblFNoteLabels::DocumentTblFNoteLabels(char const * value)
{
	_value = value;
}


DocumentTblFNoteNumStyle::DocumentTblFNoteNumStyle(char const * value)
{
	_value = value;
}


DocumentTblFNoteNumberPos::DocumentTblFNoteNumberPos(char const * value)
{
	_value = value;
}


DocumentTblFNoteNumberPrefix::DocumentTblFNoteNumberPrefix(char const * value)
{
	_value = value;
}


DocumentTblFNoteNumberSuffix::DocumentTblFNoteNumberSuffix(char const * value)
{
	_value = value;
}


DocumentTblFNoteTag::DocumentTblFNoteTag(char const * value)
{
	_value = value;
}


DocumentTwoSides::DocumentTwoSides(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentUpdateTextInsetsOnOpen::DocumentUpdateTextInsetsOnOpen(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentUpdateXRefsOnOpen::DocumentUpdateXRefsOnOpen(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentViewOnly::DocumentViewOnly(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentViewOnlySelect::DocumentViewOnlySelect(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentViewOnlyWinBorders::DocumentViewOnlyWinBorders(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentViewOnlyWinMenubar::DocumentViewOnlyWinMenubar(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentViewOnlyWinPalette::DocumentViewOnlyWinPalette(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentViewOnlyWinPopup::DocumentViewOnlyWinPopup(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentViewOnlyXRef::DocumentViewOnlyXRef(char const * value)
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}


DocumentViewRect::DocumentViewRect(int x, int y, int width, int height)
{
	_x = x;
	_y = y;
	_width = width;
	_height = height;
}


DocumentViewScale::DocumentViewScale( double value )
{
	_value = value;
}


DocumentVoMenuBar::DocumentVoMenuBar(char const * value)
{
	_value = value;
}


DocumentWideRubiSpaceForJapanese::DocumentWideRubiSpaceForJapanese(char const * value)
{
	_value = value;
}


DocumentWideRubiSpaceForOther::DocumentWideRubiSpaceForOther(char const * value)
{
	_value = value;
}


DocumentWindowRect::DocumentWindowRect(int x, int y, int width, int height)
{
	_x = x;
	_y = y;
	_width = width;
	_height = height;
}


FNoteStartNum::FNoteStartNum( int value )
{
	_value = value;
}



