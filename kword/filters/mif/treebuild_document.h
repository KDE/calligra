/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_DOCUMENT
#define _TREEBUILD_DOCUMENT

#include <list>
#include <string>

class DocumentAcrobatBookmarksIncludeTagNames
{
public:
	DocumentAcrobatBookmarksIncludeTagNames( const char* );

private:
	bool _value;
};


class DocumentAcrobatParagraphBookmarks
{
public:
	DocumentAcrobatParagraphBookmarks( const char* );

private:
	bool _value;
};


class DocumentAutoChBars
{
public:
	DocumentAutoChBars( const char* );
	
private:
	bool _value;
};


class DocumentBordersOn
{
public:
	DocumentBordersOn( const char* );
	
private:
	bool _value;
};


class DocumentChBarColor
{
public:
	DocumentChBarColor( const char* );
	
private:
	string _value;
};


class DocumentChBarGap
{
public:
	DocumentChBarGap( double, const char* unit );
	
private:
	double _value;
};


class DocumentChBarPosition
{
public:
	DocumentChBarPosition( const char* );
	
private:
	string _value;
};


class DocumentChBarWidth
{
public:
	DocumentChBarWidth( double, const char* unit );
	
private:
	double _value;
};


class DocumentCurrentView
{
public:
	DocumentCurrentView( int );
	
private:
	double _value;
};


class DocumentDisplayOverrides
{
public:
	DocumentDisplayOverrides( const char* );
	
private:
	bool _value;
};


class DocumentFluid
{
public:
	DocumentFluid( const char* );
	
private:
	bool _value;
};


class DocumentFluidSideHeads
{
public:
	DocumentFluidSideHeads( const char* );
	
private:
	bool _value;
};


class DocumentFNoteAnchorPos
{
public:
	DocumentFNoteAnchorPos( const char* );
	
private:
	string _value;
};


class DocumentFNoteAnchorPrefix
{
public:
	DocumentFNoteAnchorPrefix( const char* );
	
private:
	string _value;
};


class DocumentFNoteAnchorSuffix
{
public:
	DocumentFNoteAnchorSuffix( const char* );
	
private:
	string _value;
};


class DocumentFNoteLabels
{
public:
	DocumentFNoteLabels( const char* );
	
private:
	string _value;
};


class DocumentFNoteMaxH
{
public:
	DocumentFNoteMaxH( double, const char* unit );
	
private:
	double _value;
};


class DocumentFNoteNumberPos
{
public:
	DocumentFNoteNumberPos( const char* );
	
private:
	string _value;
};


class DocumentFNoteNumberPrefix
{
public:
	DocumentFNoteNumberPrefix( const char* );
	
private:
	string _value;
};


class DocumentFNoteNumberSuffix
{
public:
	DocumentFNoteNumberSuffix( const char* );
	
private:
	string _value;
};


class DocumentFNoteNumStyle
{
public:
	DocumentFNoteNumStyle( const char* );
	
private:
	string _value;
};


class DocumentFNoteRestart
{
public:
	DocumentFNoteRestart( const char* );
	
private:
	string _value;
};


class DocumentFNoteTag
{
public:
	DocumentFNoteTag( const char* );
	
private:
	string _value;
};


class DocumentFrozenPages
{
public:
	DocumentFrozenPages( const char* );
	
private:
	bool _value;
};


class DocumentFullRulers
{
public:
	DocumentFullRulers( const char* );
	
private:
	bool _value;
};


class DocumentGenerateAcrobatInfo
{
public:
	DocumentGenerateAcrobatInfo( const char* );
	
private:
	bool _value;
};


class DocumentGraphicsOff
{
public:
	DocumentGraphicsOff( const char* );
	
private:
	bool _value;
};


class DocumentGridOn
{
public:
	DocumentGridOn( const char* );
	
private:
	bool _value;
};


class DocumentLanguage
{
public:
	DocumentLanguage( const char* );
	
private:
	string _value;
};


class DocumentLineBreakChars
{
public:
	DocumentLineBreakChars( const char* );
	
private:
	string _value;
};


class DocumentLinkBoundariesOn
{
public:
	DocumentLinkBoundariesOn( const char* );
	
private:
	bool _value;
};


class DocumentMathAlphaCharFontFamily
{
public:
	DocumentMathAlphaCharFontFamily( const char* );
	
private:
	string _value;
};


class DocumentMathCatalog
{
public:
	DocumentMathCatalog();
};


class DocumentMathFunctions
{
public:
	DocumentMathFunctions( const char* );
	
private:
	string _value;
};


class DocumentMathGreek
{
public:
	DocumentMathGreek( const char* );
	
private:
	string _value;
};


class DocumentMathLargeHoriz
{
public:
	DocumentMathLargeHoriz( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathLargeIntegral
{
public:
	DocumentMathLargeIntegral( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathLargeLevel1
{
public:
	DocumentMathLargeLevel1( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathLargeLevel2
{
public:
	DocumentMathLargeLevel2( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathLargeLevel3
{
public:
	DocumentMathLargeLevel3( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathLargeSigma
{
public:
	DocumentMathLargeSigma( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathLargeVert
{
public:
	DocumentMathLargeVert( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathMediumHoriz
{
public:
	DocumentMathMediumHoriz( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathMediumIntegral
{
public:
	DocumentMathMediumIntegral( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathMediumLevel1
{
public:
	DocumentMathMediumLevel1( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathMediumLevel2
{
public:
	DocumentMathMediumLevel2( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathMediumLevel3
{
public:
	DocumentMathMediumLevel3( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathMediumSigma
{
public:
	DocumentMathMediumSigma( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathMediumVert
{
public:
	DocumentMathMediumVert( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathNumbers
{
public:
	DocumentMathNumbers( const char* );
	
private:
	string _value;
};


class DocumentMathShowCustom
{
public:
	DocumentMathShowCustom( const char* );
	
private:
	bool _value;
};


class DocumentMathSmallHoriz
{
public:
	DocumentMathSmallHoriz( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathSmallIntegral
{
public:
	DocumentMathSmallIntegral( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathSmallLevel1
{
public:
	DocumentMathSmallLevel1( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathSmallLevel2
{
public:
	DocumentMathSmallLevel2( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathSmallLevel3
{
public:
	DocumentMathSmallLevel3( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathSmallSigma
{
public:
	DocumentMathSmallSigma( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathSmallVert
{
public:
	DocumentMathSmallVert( double, const char* unit );
	
private:
	double _value;
};


class DocumentMathStrings
{
public:
	DocumentMathStrings( const char* );
	
private:
	string _value;
};


class DocumentMathVariables
{
public:
	DocumentMathVariables( const char* );
	
private:
	string _value;
};


class DocumentMenuBar
{
public:
	DocumentMenuBar( const char* );
	
private:
	string _value;
};


class DocumentNarrowRubiSpaceForJapanese
{
public:
	DocumentNarrowRubiSpaceForJapanese( const char* );
	
private:
	string _value;
};


class DocumentNarrowRubiSpaceForOther
{
public:
	DocumentNarrowRubiSpaceForOther( const char* );
	
private:
	string _value;
};


class DocumentNextUnique
{
public:
	DocumentNextUnique( int );
	
private:
	int _value;
};


class DocumentPageGrid
{
public:
	DocumentPageGrid( double, const char* unit );
	
private:
	double _value;
};


class DocumentPageNumStyle
{
public:
	DocumentPageNumStyle( const char* );
	
private:
	string _value;
};


class DocumentPagePointStyle
{
public:
	DocumentPagePointStyle( const char* );
	
private:
	string _value;
};


class DocumentPageRounding
{
public:
	DocumentPageRounding( const char* );
	
private:
	string _value;
};


class DocumentPageScrolling
{
public:
	DocumentPageScrolling( const char* );
	DocumentPageScrolling( int );

	enum DocumentPageScrollingType { DPS_STRING, DPS_INT };

private:
	DocumentPageScrollingType _type;
	string _stringvalue;
	int _intvalue;
};


class DocumentPageSize
{
public:
	DocumentPageSize( double, const char* unit, double, const char* unit );
	
	double width() const { return _width; }
	double height() const { return _height; }

private:
	double _width;
	double _height;
};


class DocumentParity
{
public:
	DocumentParity( const char* );
	
private:
	string _value;
};


class DocumentPrintSeparations
{
public:
	DocumentPrintSeparations( const char* );
	
private:
	bool _value;
};


class DocumentPrintSkipBlankPages
{
 public:
	DocumentPrintSkipBlankPages( const char* );
	
 private:
	bool _value;
};


class DocumentPunctuationChars
{
public:
	DocumentPunctuationChars( const char* );
	
private:
	string _value;
};


class DocumentRubiAlignAtLineBounds
{
public:
	DocumentRubiAlignAtLineBounds( const char* );
	
private:
	bool _value;
};


class DocumentRubiOverhang
{
public:
	DocumentRubiOverhang( const char* );
	
private:
	bool _value;
};


class DocumentRubiSize
{
public:
	DocumentRubiSize( double );
	
private:
	double _value;
};


class DocumentRulersOn
{
public:
	DocumentRulersOn( const char* );
	
private:
	bool _value;
};


class DocumentShowAllConditions
{
public:
	DocumentShowAllConditions( const char* );
	
private:
	bool _value;
};


class DocumentSmallCapsSize
{
public:
	DocumentSmallCapsSize( double );
	
private:
	double _value;
};


class DocumentSmallCapsStretch
{
public:
	DocumentSmallCapsStretch( double );
	
private:
	double _value;
};


class DocumentSmartQuotesOn
{
public:
	DocumentSmartQuotesOn( const char* );
	
private:
	bool _value;
};


class DocumentSmartSpacesOn
{
public:
	DocumentSmartSpacesOn( const char* );
	
private:
	bool _value;
};


class DocumentSnapGrid
{
public:
	DocumentSnapGrid( double, const char* unit );
	
private:
	double _value;
};


class DocumentSnapRotation
{
public:
	DocumentSnapRotation( double );
	
private:
	double _value;
};


class DocumentStartPage
{
public:
	DocumentStartPage( int );
	
private:
	double _value;
};


class DocumentSubscriptShift
{
public:
	DocumentSubscriptShift( double );
	
private:
	double _value;
};


class DocumentSubscriptSize
{
public:
	DocumentSubscriptSize( double );
	
private:
	double _value;
};


class DocumentSubscriptStretch
{
public:
	DocumentSubscriptStretch( double );
	
private:
	double _value;
};


class DocumentSuperscriptShift
{
public:
	DocumentSuperscriptShift( double );
	
private:
	double _value;
};


class DocumentSuperscriptSize
{
public:
	DocumentSuperscriptSize( double );
	
private:
	double _value;
};


class DocumentSuperscriptStretch
{
public:
	DocumentSuperscriptStretch( double );
	
private:
	double _value;
};


class DocumentSymbolsOn
{
public:
	DocumentSymbolsOn( const char* );
	
private:
	bool _value;
};


class DocumentTblFNoteAnchorPos
{
public:
	DocumentTblFNoteAnchorPos( const char* );
	
private:
	string _value;
};


class DocumentTblFNoteAnchorPrefix
{
public:
	DocumentTblFNoteAnchorPrefix( const char* );
	
private:
	string _value;
};


class DocumentTblFNoteAnchorSuffix
{
public:
	DocumentTblFNoteAnchorSuffix( const char* );
	
private:
	string _value;
};


class DocumentTblFNoteLabels
{
public:
	DocumentTblFNoteLabels( const char* );
	
private:
	string _value;
};


class DocumentTblFNoteNumberPos
{
public:
	DocumentTblFNoteNumberPos( const char* );
	
private:
	string _value;
};


class DocumentTblFNoteNumberPrefix
{
public:
	DocumentTblFNoteNumberPrefix( const char* );
	
private:
	string _value;
};


class DocumentTblFNoteNumberSuffix
{
public:
	DocumentTblFNoteNumberSuffix( const char* );
	
private:
	string _value;
};


class DocumentTblFNoteNumStyle
{
public:
	DocumentTblFNoteNumStyle( const char* );
	
private:
	string _value;
};


class DocumentTblFNoteTag
{
public:
	DocumentTblFNoteTag( const char* );
	
private:
	string _value;
};


class DocumentTwoSides
{
public:
	DocumentTwoSides( const char* );
	
private:
	bool _value;
};


class DocumentUpdateTextInsetsOnOpen
{
public:
	DocumentUpdateTextInsetsOnOpen( const char* );
	
private:
	bool _value;
};


class DocumentUpdateXRefsOnOpen
{
public:
	DocumentUpdateXRefsOnOpen( const char* );
	
private:
	bool _value;
};


class DocumentViewOnly
{
public:
	DocumentViewOnly( const char* );
	
private:
	bool _value;
};


class DocumentViewOnlySelect
{
public:
	DocumentViewOnlySelect( const char* );
	
private:
	bool _value;
};


class DocumentViewOnlyWinBorders
{
public:
	DocumentViewOnlyWinBorders( const char* );
	
private:
	bool _value;
};


class DocumentViewOnlyWinMenubar
{
public:
	DocumentViewOnlyWinMenubar( const char* );
	
private:
	bool _value;
};


class DocumentViewOnlyWinPalette
{
public:
	DocumentViewOnlyWinPalette( const char* );
	
private:
	bool _value;
};


class DocumentViewOnlyWinPopup
{
public:
	DocumentViewOnlyWinPopup( const char* );
	
private:
	bool _value;
};


class DocumentViewOnlyXRef
{
public:
	DocumentViewOnlyXRef( const char* );
	
private:
	bool _value;
};


class DocumentViewRect
{
public:
	DocumentViewRect( int, int, int, int );
	
private:
	int _x;
	int _y;
	int _width;
	int _height;
};


class DocumentViewScale
{
public:
	DocumentViewScale( double );
	
private:
	double _value;
};


class DocumentVoMenuBar
{
public:
	DocumentVoMenuBar( const char* );
	
private:
	string _value;
};


class DocumentWideRubiSpaceForJapanese
{
public:
	DocumentWideRubiSpaceForJapanese( const char* );
	
private:
	string _value;
};


class DocumentWideRubiSpaceForOther
{
public:
	DocumentWideRubiSpaceForOther( const char* );
	
private:
	string _value;
};


class DocumentWindowRect
{
public:
	DocumentWindowRect( int, int, int, int );
	
private:
	int _x;
	int _y;
	int _width;
	int _height;
};


class FNoteStartNum
{
public:
	FNoteStartNum( int );

private:
	int _value;
};


class DocumentElement
{
public:
	enum DocumentElementType {
		T_DocumentAcrobatBookmarksIncludeTagNames,
		T_DocumentAcrobatParagraphBookmarks,
		T_DocumentAutoChBars, T_DocumentBordersOn,
		T_DocumentChBarColor, T_DocumentChBarGap,
		T_DocumentChBarPosition, T_DocumentChBarWidth,
		T_DocumentCurrentView, T_DocumentDisplayOverrides,
		T_DocumentFluid, T_DocumentFluidSideHeads,
		T_DocumentFNoteAnchorPos,
		T_DocumentFNoteAnchorPrefix,
		T_DocumentFNoteAnchorSuffix,
		T_DocumentFNoteLabels,
		T_DocumentFNoteMaxH,
		T_DocumentFNoteNumberPos,
		T_DocumentFNoteNumberPrefix,
		T_DocumentFNoteNumberSuffix,
		T_DocumentFNoteNumStyle,
		T_DocumentFNoteRestart,
		T_DocumentFNoteTag,
		T_DocumentFrozenPages,
		T_DocumentFullRulers,
		T_DocumentGenerateAcrobatInfo,
		T_DocumentGraphicsOff,
		T_DocumentGridOn,
		T_DocumentLanguage,
		T_DocumentLineBreakChars,
		T_DocumentLinkBoundariesOn,
		T_DocumentMathAlphaCharFontFamily,
		T_DocumentMathCatalog,
		T_DocumentMathFunctions,
		T_DocumentMathGreek,
		T_DocumentMathLargeHoriz,
		T_DocumentMathLargeIntegral,
		T_DocumentMathLargeLevel1,
		T_DocumentMathLargeLevel2,
		T_DocumentMathLargeLevel3,
		T_DocumentMathLargeSigma,
		T_DocumentMathLargeVert,
		T_DocumentMathMediumHoriz,
		T_DocumentMathMediumIntegral,
		T_DocumentMathMediumLevel1,
		T_DocumentMathMediumLevel2,
		T_DocumentMathMediumLevel3,
		T_DocumentMathMediumSigma,
		T_DocumentMathMediumVert,
		T_DocumentMathNumbers,
		T_DocumentMathShowCustom,
		T_DocumentMathSmallHoriz,
		T_DocumentMathSmallIntegral,
		T_DocumentMathSmallLevel1,
		T_DocumentMathSmallLevel2,
		T_DocumentMathSmallLevel3,
		T_DocumentMathSmallSigma,
		T_DocumentMathSmallVert,
		T_DocumentMathStrings,
		T_DocumentMathVariables,
		T_DocumentMenuBar,
		T_DocumentNarrowRubiSpaceForJapanese,
		T_DocumentNarrowRubiSpaceForOther,
		T_DocumentNextUnique,
		T_DocumentNoPrintSepColor,
		T_DocumentPageGrid,
		T_DocumentPageNumStyle,
		T_DocumentPagePointStyle,
		T_DocumentPageRounding,
		T_DocumentPageScrolling,
		T_DocumentPageSize,
		T_DocumentParity,
		T_DocumentPrintSeparations,
		T_DocumentPrintSkipBlankPages,
		T_DocumentPunctuationChars,
		T_DocumentRubiAlignAtLineBounds,
		T_DocumentRubiOverhang,
		T_DocumentRubiSize,
		T_DocumentRulersOn,
		T_DocumentShowAllConditions,
		T_DocumentSmallCapsSize,
		T_DocumentSmallCapsStretch,
		T_DocumentSmartQuotesOn,
		T_DocumentSmartSpacesOn,
		T_DocumentSnapGrid,
		T_DocumentSnapRotation,
		T_DocumentStartPage,
		T_DocumentSubscriptShift,
		T_DocumentSubscriptSize,
		T_DocumentSubscriptStretch,
		T_DocumentSuperscriptShift,
		T_DocumentSuperscriptSize,
		T_DocumentSuperscriptStretch,
		T_DocumentSymbolsOn,
		T_DocumentTblFNoteAnchorPos,
		T_DocumentTblFNoteAnchorPrefix,
		T_DocumentTblFNoteAnchorSuffix,
		T_DocumentTblFNoteLabels,
		T_DocumentTblFNoteNumberPos,
		T_DocumentTblFNoteNumberPrefix,
		T_DocumentTblFNoteNumberSuffix,
		T_DocumentTblFNoteNumStyle,
		T_DocumentTblFNoteTag,
		T_DocumentTwoSides,
		T_DocumentUpdateTextInsetsOnOpen,
		T_DocumentUpdateXRefsOnOpen,
		T_DocumentViewOnly,
		T_DocumentViewOnlySelect,
		T_DocumentViewOnlyWinBorders,
		T_DocumentViewOnlyWinMenubar,
		T_DocumentViewOnlyWinPalette,
		T_DocumentViewOnlyWinPopup,
		T_DocumentViewOnlyXRef,
		T_DocumentViewRect,
		T_DocumentViewScale,
		T_DocumentVoMenuBar,
		T_DocumentWideRubiSpaceForJapanese,
		T_DocumentWideRubiSpaceForOther,
		T_DocumentWindowRect,
		T_FNoteStartNum };

	DocumentElement( DocumentAcrobatBookmarksIncludeTagNames* element );
	DocumentElement( DocumentAcrobatParagraphBookmarks* element );
	DocumentElement( DocumentAutoChBars* element );
	DocumentElement( DocumentBordersOn* element );
	DocumentElement( DocumentChBarColor* element );
	DocumentElement( DocumentChBarGap* element );
	DocumentElement( DocumentChBarPosition* element );
	DocumentElement( DocumentChBarWidth* element );
	DocumentElement( DocumentCurrentView* element );
	DocumentElement( DocumentDisplayOverrides* element );
	DocumentElement( DocumentFluid* element );
	DocumentElement( DocumentFluidSideHeads* element );
	DocumentElement( DocumentFNoteAnchorPos* element );
	DocumentElement( DocumentFNoteAnchorPrefix* element );
	DocumentElement( DocumentFNoteAnchorSuffix* element );
	DocumentElement( DocumentFNoteLabels* element );
	DocumentElement( DocumentFNoteMaxH* element );
	DocumentElement( DocumentFNoteNumberPos* element );
	DocumentElement( DocumentFNoteNumberPrefix* element );
	DocumentElement( DocumentFNoteNumberSuffix* element );
	DocumentElement( DocumentFNoteNumStyle* element );
	DocumentElement( DocumentFNoteRestart* element );
	DocumentElement( DocumentFNoteTag* element );
	DocumentElement( DocumentFrozenPages* element );
	DocumentElement( DocumentFullRulers* element );
	DocumentElement( DocumentGenerateAcrobatInfo* element );
	DocumentElement( DocumentGraphicsOff* element );
	DocumentElement( DocumentGridOn* element );
	DocumentElement( DocumentLanguage* element );
	DocumentElement( DocumentLineBreakChars* element );
	DocumentElement( DocumentLinkBoundariesOn* element );
	DocumentElement( DocumentMathAlphaCharFontFamily* element );
	DocumentElement( DocumentMathCatalog* element );
	DocumentElement( DocumentMathFunctions* element );
	DocumentElement( DocumentMathGreek* element );
	DocumentElement( DocumentMathLargeHoriz* element );
	DocumentElement( DocumentMathLargeIntegral* element );
	DocumentElement( DocumentMathLargeLevel1* element );
	DocumentElement( DocumentMathLargeLevel2* element );
	DocumentElement( DocumentMathLargeLevel3* element );
	DocumentElement( DocumentMathLargeSigma* element );
	DocumentElement( DocumentMathLargeVert* element );
	DocumentElement( DocumentMathMediumHoriz* element );
	DocumentElement( DocumentMathMediumIntegral* element );
	DocumentElement( DocumentMathMediumLevel1* element );
	DocumentElement( DocumentMathMediumLevel2* element );
	DocumentElement( DocumentMathMediumLevel3* element );
	DocumentElement( DocumentMathMediumSigma* element );
	DocumentElement( DocumentMathMediumVert* element );
	DocumentElement( DocumentMathNumbers* element );
	DocumentElement( DocumentMathShowCustom* element );
	DocumentElement( DocumentMathSmallHoriz* element );
	DocumentElement( DocumentMathSmallIntegral* element );
	DocumentElement( DocumentMathSmallLevel1* element );
	DocumentElement( DocumentMathSmallLevel2* element );
	DocumentElement( DocumentMathSmallLevel3* element );
	DocumentElement( DocumentMathSmallSigma* element );
	DocumentElement( DocumentMathSmallVert* element );
	DocumentElement( DocumentMathStrings* element );
	DocumentElement( DocumentMathVariables* element );
	DocumentElement( DocumentMenuBar* element );
	DocumentElement( DocumentNarrowRubiSpaceForJapanese* element );
	DocumentElement( DocumentNarrowRubiSpaceForOther* element );
	DocumentElement( DocumentNextUnique* element );
	DocumentElement( DocumentPageGrid* element );
	DocumentElement( DocumentPageNumStyle* element );
	DocumentElement( DocumentPagePointStyle* element );
	DocumentElement( DocumentPageRounding* element );
	DocumentElement( DocumentPageScrolling* element );
	DocumentElement( DocumentPageSize* element );
	DocumentElement( DocumentParity* element );
	DocumentElement( DocumentPrintSeparations* element );
	DocumentElement( DocumentPrintSkipBlankPages* element );
	DocumentElement( DocumentPunctuationChars* element );
	DocumentElement( DocumentRubiAlignAtLineBounds* element );
	DocumentElement( DocumentRubiOverhang* element );
	DocumentElement( DocumentRubiSize* element );
	DocumentElement( DocumentRulersOn* element );
	DocumentElement( DocumentShowAllConditions* element );
	DocumentElement( DocumentSmallCapsSize* element );
	DocumentElement( DocumentSmallCapsStretch* element );
	DocumentElement( DocumentSmartQuotesOn* element );
	DocumentElement( DocumentSmartSpacesOn* element );
	DocumentElement( DocumentSnapGrid* element );
	DocumentElement( DocumentSnapRotation* element );
	DocumentElement( DocumentStartPage* element );
	DocumentElement( DocumentSubscriptShift* element );
	DocumentElement( DocumentSubscriptSize* element );
	DocumentElement( DocumentSubscriptStretch* element );
	DocumentElement( DocumentSuperscriptShift* element );
	DocumentElement( DocumentSuperscriptSize* element );
	DocumentElement( DocumentSuperscriptStretch* element );
	DocumentElement( DocumentSymbolsOn* element );
	DocumentElement( DocumentTblFNoteAnchorPos* element );
	DocumentElement( DocumentTblFNoteAnchorPrefix* element );
	DocumentElement( DocumentTblFNoteAnchorSuffix* element );
	DocumentElement( DocumentTblFNoteLabels* element );
	DocumentElement( DocumentTblFNoteNumberPos* element );
	DocumentElement( DocumentTblFNoteNumberPrefix* element );
	DocumentElement( DocumentTblFNoteNumberSuffix* element );
	DocumentElement( DocumentTblFNoteNumStyle* element );
	DocumentElement( DocumentTblFNoteTag* element );
	DocumentElement( DocumentTwoSides* element );
	DocumentElement( DocumentUpdateTextInsetsOnOpen* element );
	DocumentElement( DocumentUpdateXRefsOnOpen* element );
	DocumentElement( DocumentViewOnly* element );
	DocumentElement( DocumentViewOnlySelect* element );
	DocumentElement( DocumentViewOnlyWinBorders* element );
	DocumentElement( DocumentViewOnlyWinMenubar* element );
	DocumentElement( DocumentViewOnlyWinPalette* element );
	DocumentElement( DocumentViewOnlyWinPopup* element );
	DocumentElement( DocumentViewOnlyXRef* element );
	DocumentElement( DocumentViewRect* element );
	DocumentElement( DocumentViewScale* element );
	DocumentElement( DocumentVoMenuBar* element );
	DocumentElement( DocumentWideRubiSpaceForJapanese* element );
	DocumentElement( DocumentWideRubiSpaceForOther* element );
	DocumentElement( DocumentWindowRect* element );
	DocumentElement( FNoteStartNum* element );

 	DocumentElementType type() const { return _type; }
	DocumentPageSize* pageSize() const { return _type == T_DocumentPageSize ?
											_documentpagesize : 0; }

private:
 	DocumentElementType _type;
 	union {
		DocumentAcrobatBookmarksIncludeTagNames* _documentacrobatbookmarksincludetagnames;
		DocumentAcrobatParagraphBookmarks* _documentacrobatparagraphbookmarks;
		DocumentAutoChBars* _documentautochbars;
		DocumentBordersOn* _documentborderson;
		DocumentChBarColor* _documentchbarcolor;
		DocumentChBarGap* _documentchbargap;
		DocumentChBarPosition* _documentchbarposition;
		DocumentChBarWidth* _documentchbarwidth;
		DocumentCurrentView* _documentcurrentview;
		DocumentDisplayOverrides* _documentdisplayoverrides;
		DocumentFluid* _documentfluid;
		DocumentFluidSideHeads* _documentfluidsideheads;
		DocumentFNoteAnchorPos* _documentfnoteanchorpos;
		DocumentFNoteAnchorPrefix* _documentfnoteanchorprefix;
		DocumentFNoteAnchorSuffix* _documentfnoteanchorsuffix;
		DocumentFNoteLabels* _documentfnotelabels;
		DocumentFNoteMaxH* _documentfnotemaxh;
		DocumentFNoteNumberPos* _documentfnotenumberpos;
		DocumentFNoteNumberPrefix* _documentfnotenumberprefix;
		DocumentFNoteNumberSuffix* _documentfnotenumbersuffix;
		DocumentFNoteNumStyle* _documentfnotenumstyle;
		DocumentFNoteRestart* _documentfnoterestart;
		DocumentFNoteTag* _documentfnotetag;
		DocumentFrozenPages* _documentfrozenpages;
		DocumentFullRulers* _documentfullrulers;
		DocumentGenerateAcrobatInfo* _documentgenerateacrobatinfo;
		DocumentGraphicsOff* _documentgraphicsoff;
		DocumentGridOn* _documentgridon;
		DocumentLanguage* _documentlanguage;
		DocumentLineBreakChars* _documentlinebreakchars;
		DocumentLinkBoundariesOn* _documentlinkboundarieson;
		DocumentMathAlphaCharFontFamily* _documentmathalphacharfontfamily;
		DocumentMathCatalog* _documentmathcatalog;
		DocumentMathFunctions* _documentmathfunctions;
		DocumentMathGreek* _documentmathgreek;
		DocumentMathLargeHoriz* _documentmathlargehoriz;
		DocumentMathLargeIntegral* _documentmathlargeintegral;
		DocumentMathLargeLevel1* _documentmathlargelevel1;
		DocumentMathLargeLevel2* _documentmathlargelevel2;
		DocumentMathLargeLevel3* _documentmathlargelevel3;
		DocumentMathLargeSigma* _documentmathlargesigma;
		DocumentMathLargeVert* _documentmathlargevert;
		DocumentMathMediumHoriz* _documentmathmediumhoriz;
		DocumentMathMediumIntegral* _documentmathmediumintegral;
		DocumentMathMediumLevel1* _documentmathmediumlevel1;
		DocumentMathMediumLevel2* _documentmathmediumlevel2;
		DocumentMathMediumLevel3* _documentmathmediumlevel3;
		DocumentMathMediumSigma* _documentmathmediumsigma;
		DocumentMathMediumVert* _documentmathmediumvert;
		DocumentMathNumbers* _documentmathnumbers;
		DocumentMathShowCustom* _documentmathshowcustom;
		DocumentMathSmallHoriz* _documentmathsmallhoriz;
		DocumentMathSmallIntegral* _documentmathsmallintegral;
		DocumentMathSmallLevel1* _documentmathsmalllevel1;
		DocumentMathSmallLevel2* _documentmathsmalllevel2;
		DocumentMathSmallLevel3* _documentmathsmalllevel3;
		DocumentMathSmallSigma* _documentmathsmallsigma;
		DocumentMathSmallVert* _documentmathsmallvert;
		DocumentMathStrings* _documentmathstrings;
		DocumentMathVariables* _documentmathvariables;
		DocumentMenuBar* _documentmenubar;
		DocumentNarrowRubiSpaceForJapanese* _documentnarrowrubispaceforjapanese;
		DocumentNarrowRubiSpaceForOther* _documentnarrowrubispaceforother;
		DocumentNextUnique* _documentnextunique;
		DocumentPageGrid* _documentpagegrid;
		DocumentPageNumStyle* _documentpagenumstyle;
		DocumentPagePointStyle* _documentpagepointstyle;
		DocumentPageRounding* _documentpagerounding;
		DocumentPageScrolling* _documentpagescrolling;
		DocumentPageSize* _documentpagesize;
		DocumentParity* _documentparity;
		DocumentPrintSeparations* _documentprintseparations;
		DocumentPrintSkipBlankPages* _documentprintskipblankpages;
		DocumentPunctuationChars* _documentpunctuationchars;
		DocumentRubiAlignAtLineBounds* _documentrubialignatlinebounds;
		DocumentRubiOverhang* _documentrubioverhang;
		DocumentRubiSize* _documentrubisize;
		DocumentRulersOn* _documentrulerson;
		DocumentShowAllConditions* _documentshowallconditions;
		DocumentSmallCapsSize* _documentsmallcapssize;
		DocumentSmallCapsStretch* _documentsmallcapsstretch;
		DocumentSmartQuotesOn* _documentsmartquoteson;
		DocumentSmartSpacesOn* _documentsmartspaceson;
		DocumentSnapGrid* _documentsnapgrid;
		DocumentSnapRotation* _documentsnaprotation;
		DocumentStartPage* _documentstartpage;
		DocumentSubscriptShift* _documentsubscriptshift;
		DocumentSubscriptSize* _documentsubscriptsize;
		DocumentSubscriptStretch* _documentsubscriptstretch;
		DocumentSuperscriptShift* _documentsuperscriptshift;
		DocumentSuperscriptSize* _documentsuperscriptsize;
		DocumentSuperscriptStretch* _documentsuperscriptstretch;
		DocumentSymbolsOn* _documentsymbolson;
		DocumentTblFNoteAnchorPos* _documenttblfnoteanchorpos;
		DocumentTblFNoteAnchorPrefix* _documenttblfnoteanchorprefix;
		DocumentTblFNoteAnchorSuffix* _documenttblfnoteanchorsuffix;
		DocumentTblFNoteLabels* _documenttblfnotelabels;
		DocumentTblFNoteNumberPos* _documenttblfnotenumberpos;
		DocumentTblFNoteNumberPrefix* _documenttblfnotenumberprefix;
		DocumentTblFNoteNumberSuffix* _documenttblfnotenumbersuffix;
		DocumentTblFNoteNumStyle* _documenttblfnotenumstyle;
		DocumentTblFNoteTag* _documenttblfnotetag;
		DocumentTwoSides* _documenttwosides;
		DocumentUpdateTextInsetsOnOpen* _documentupdatetextinsetsonopen;
		DocumentUpdateXRefsOnOpen* _documentupdatexrefsonopen;
		DocumentViewOnly* _documentviewonly;
		DocumentViewOnlySelect* _documentviewonlyselect;
		DocumentViewOnlyWinBorders* _documentviewonlywinborders;
		DocumentViewOnlyWinMenubar* _documentviewonlywinmenubar;
		DocumentViewOnlyWinPalette* _documentviewonlywinpalette;
		DocumentViewOnlyWinPopup* _documentviewonlywinpopup;
		DocumentViewOnlyXRef* _documentviewonlyxref;
		DocumentViewRect* _documentviewrect;
		DocumentViewScale* _documentviewscale;
		DocumentVoMenuBar* _documentvomenubar;
		DocumentWideRubiSpaceForJapanese* _documentwiderubispaceforjapanese;
		DocumentWideRubiSpaceForOther* _documentwiderubispaceforother;
		DocumentWindowRect* _documentwindowrect;
		FNoteStartNum* _fnotestartnum;
 	};
};

typedef list<DocumentElement*> DocumentElementList;

extern DocumentElementList documentelements;

#endif
