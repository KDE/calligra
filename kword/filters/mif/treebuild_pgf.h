/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_PGF
#define _TREEBUILD_PGF

#include <list>
#include <string>

class TabStop;
class FTag;
class FPostScriptName;
class FPlatformName;
class FFamily;
class FVar;
class FTsume;
class FWeight;
class FAngle;
class FEncoding;
class FSize;
class FUnderlining;
class FOverline;
class FStrike;
class FChangeBar;
class FOutline;
class FShadow;
class FPairKern;
class FCase;
class FPosition;
class FDX_;
class FDY_;
class FDW_;
class FStretch;
class FLanguage;
class FLocked;
class FSeparation;
class FColor;

class PgfTag
{
public:
	PgfTag( const char* );

private:
	string _value;
};


class PgfUseNextTag
{
public:
	PgfUseNextTag( const char* );

private:
	string _value;
};


class PgfNextTag
{
public:
	PgfNextTag( const char* );

private:
	string _value;
};


class PgfAlignment
{
public:
	PgfAlignment( const char* );

private:
	string _value;
};


class PgfFIndent
{
public:
	PgfFIndent( double );

private:
	double _value;
};


class PgfFIndentOffset
{
public:
	PgfFIndentOffset( double );

private:
	double _value;
};


class PgfFIndentRelative
{
public:
	PgfFIndentRelative( char const* );

private:
	string _value;
};


class PgfLIndent
{
public:
	PgfLIndent( double );

private:
	double _value;
};


class PgfRIndent
{
public:
	PgfRIndent( double );

private:
	double _value;
};


class PgfTopSeparator
{
public:
	PgfTopSeparator( const char* );

private:
	string _value;
};


class PgfTopSepAtIndent
{
public:
	PgfTopSepAtIndent( const char* );

private:
	string _value;
};


class PgfTopSepOffset
{
public:
	PgfTopSepOffset( double );

private:
	double _value;
};


class PgfBotSeparator
{
public:
	PgfBotSeparator( const char* );

private:
	string _value;
};


class PgfBotSepAtIndent
{
public:
	PgfBotSepAtIndent( const char* );

private:
	string _value;
};


class PgfBotSepOffset
{
public:
	PgfBotSepOffset( double );

private:
	double _value;
};


class PgfPlacement
{
public:
	PgfPlacement( const char* );

private:
	string _value;
};


class PgfPlacementStyle
{
public:
	PgfPlacementStyle( const char* );

private:
	string _value;
};


class PgfRunInDefaultPunct
{
public:
	PgfRunInDefaultPunct( const char* );

private:
	string _value;
};


class PgfSpBefore
{
public:
	PgfSpBefore( double );

private:
	double _value;
};


class PgfSpAfter
{
public:
	PgfSpAfter( double );

private:
	double _value;
};


class PgfWithPrev
{
public:
	PgfWithPrev( const char* );

private:
	string _value;
};


class PgfWithNext
{
public:
	PgfWithNext( const char* );

private:
	string _value;
};


class PgfBlockSize
{
public:
	PgfBlockSize( int );

private:
	int _value;
};


class PgfFontElement
{
public:
	enum PgfFontElementType {T_FTag, T_FPostScriptName, T_FPlatformName, 
							 T_FFamily, T_FVar, T_FWeight, T_FAngle, 
							 T_FEncoding, T_FSize,	T_FUnderlining,
							 T_FOverline, T_FStrike, T_FChangeBar,
							 T_FOutline, T_FShadow, T_FPairKern, T_FTsume,	
							 T_FCase, T_FPosition, T_FDX, T_FDY, T_FDW, 
							 T_FStretch, T_FLanguage, T_FLocked, T_FSeparation,
							 T_FColor };

	PgfFontElement( FTag* element );
	PgfFontElement( FPostScriptName* element );
	PgfFontElement( FPlatformName* element );
	PgfFontElement( FFamily* element );
	PgfFontElement( FVar* element );
	PgfFontElement( FTsume* element );
	PgfFontElement( FWeight* element );
	PgfFontElement( FAngle* element );
	PgfFontElement( FEncoding* element );
	PgfFontElement( FSize* element );
	PgfFontElement( FUnderlining* element );
	PgfFontElement( FOverline* element );
	PgfFontElement( FStrike* element );
	PgfFontElement( FChangeBar* element );
	PgfFontElement( FOutline* element );
	PgfFontElement( FShadow* element );
	PgfFontElement( FPairKern* element );
	PgfFontElement( FCase* element );
	PgfFontElement( FPosition* element );
	PgfFontElement( FDX_* element );
	PgfFontElement( FDY_* element );
	PgfFontElement( FDW_* element );
	PgfFontElement( FStretch* element );
	PgfFontElement( FLanguage* element );
	PgfFontElement( FLocked* element );
	PgfFontElement( FSeparation* element );
	PgfFontElement( FColor* element );

private:
	PgfFontElementType _type;

	union {
	FTag* _ftag ;
	FPostScriptName* _fpostscriptname ;
	FPlatformName* _fplatformname ;
	FFamily* _ffamily ;
	FVar* _fvar ;
	FWeight* _fweight ;
	FAngle* _fangle ;
	FEncoding* _fencoding ;
	FSize* _fsize ;
	FUnderlining* _funderlining ;
	FOverline* _foverline ;
	FStrike* _fstrike ;
	FChangeBar* _fchangebar ;
	FOutline* _foutline ;
	FShadow* _fshadow ;
	FPairKern* _fpairkern ;
	FTsume* _ftsume ;
	FCase* _fcase ;
	FPosition* _fposition ;
	FDX_* _fdx ;
	FDY_* _fdy ;
	FDW_* _fdw ;
	FStretch* _fstretch ;
	FLanguage* _flanguage ;
	FLocked* _flocked ;
	FSeparation* _fseparation ;
	FColor* _fcolor ;
	};
};


typedef list<PgfFontElement*> PgfFontElementList;

class PgfFont
{
public:
	PgfFont( PgfFontElementList* elements );

private:
	PgfFontElementList* _elements;
};




class PgfLineSpacing
{
public:
	PgfLineSpacing( const char* );

private:
	string _value;
};


class PgfLeading
{
public:
	PgfLeading( double );

private:
	double _value;
};


class PgfAutoNum
{
public:
	PgfAutoNum( const char* );

private:
	string _value;
};


class PgfNumTabs
{
public:
	PgfNumTabs( int );

private:
	int _value;
};


class PgfNumFormat
{
public:
	PgfNumFormat( const char* );

private:
	string _value;
};


class PgfNumberFont
{
public:
	PgfNumberFont( const char* );

private:
	string _value;
};


class PgfNumAtEnd
{
public:
	PgfNumAtEnd( const char* );

private:
	string _value;
};


class PgfHyphenate
{
public:
	PgfHyphenate( const char* );

private:
	string _value;
};


class HyphenMaxLines
{
public:
	HyphenMaxLines( int );

private:
	int _value;
};


class HyphenMinPrefix
{
public:
	HyphenMinPrefix( int );

private:
	int _value;
};


class HyphenMinWord
{
public:
	HyphenMinWord( int );

private:
	int _value;
};


class HyphenMinSuffix
{
public:
	HyphenMinSuffix( int );

private:
	int _value;
};


class PgfLetterSpace
{
public:
	PgfLetterSpace( const char* );

private:
	string _value;
};


class PgfMinWordSpace
{
public:
	PgfMinWordSpace ( int );

private:
	int _value;
};


class PgfMaxWordSpace
{
public:
	PgfMaxWordSpace( int );

private:
	int _value;
};


class PgfOptWordSpace
{
public:
	PgfOptWordSpace ( int );

private:
	int _value;
};


class PgfLanguage
{
public:
	PgfLanguage( const char* );

private:
	string _value;
};


class PgfCellAlignment
{
public:
	PgfCellAlignment( const char* );

private:
	string _value;
};


class PgfMinJRomanLetterSpace
{
public:
	PgfMinJRomanLetterSpace ( int );

private:
	int _value;
};


class PgfMaxJRomanLetterSpace
{
public:
	PgfMaxJRomanLetterSpace ( int );

private:
	int _value;
};


class PgfOptJRomanLetterSpace
{
public:
	PgfOptJRomanLetterSpace ( int );

private:
	int _value;
};


class PgfMinJLetterSpace
{
public:
	PgfMinJLetterSpace ( int );

private:
	int _value;
};


class PgfMaxJLetterSpace
{
public:
	PgfMaxJLetterSpace ( int );

private:
	int _value;
};


class PgfOptJLetterSpace
{
public:
	PgfOptJLetterSpace ( int );

private:
	int _value;
};


class PgfYakumonoType
{
public:
	PgfYakumonoType ( const char* );

private:
	string _value;
};


class PgfAcrobatLevel
{
public:
	PgfAcrobatLevel( int );

private:
	int _value;
};


class PgfCellMargins
{
public:
	PgfCellMargins( double, double, double, double );

private:
	double _value1;
	double _value2;
	double _value3;
	double _value4;
};

class PgfLocked
{
public:
	PgfLocked ( const char* );

private:
	string _value;
};

class PgfNumString
{
public:
	PgfNumString ( const char* );

private:
	string _value;
};

class PgfCellLMarginFixed
{
public:
	PgfCellLMarginFixed ( const char* );

private:
	string _value;
};


class PgfCellRMarginFixed
{
public:
	PgfCellRMarginFixed ( const char* );

private:
	string _value;
};


class PgfCellTMarginFixed
{
public:
	PgfCellTMarginFixed ( const char* );

private:
	string _value;
};


class PgfCellBMarginFixed
{
public:
	PgfCellBMarginFixed ( const char* );

private:
	string _value;
};



class PgfElement
{
public:
	enum PgfElementType { T_PgfUseNextTag, T_PgfNextTag, T_PgfAlignment, 
						  T_PgfFIndent, T_PgfLIndent, T_PgfRIndent, 
						  T_PgfFIndentOffset, T_PgfFIndentRelative,
						  T_PgfTopSeparator, T_PgfTopSepAtIndent, 
						  T_PgfTopSepOffset, T_PgfBotSeparator, 
						  T_PgfBotSepAtIndent, T_PgfBotSepOffset, 
						  T_PgfPlacement, T_PgfPlacementStyle, 
						  T_PgfRunInDefaultPunct, T_PgfSpBefore, 
						  T_PgfSpAfter, T_PgfWithNext, 
						  T_PgfWithPrev, T_PgfBlockSize, 
						  T_PgfFont, T_PgfLineSpacing, 
						  T_PgfLeading, T_PgfAutoNum, 
						  T_PgfNumTabs, T_PgfNumFormat, 
						  T_PgfNumberFont, T_PgfNumAtEnd, 
						  T_PgfHyphenate, T_HyphenMaxLines, 
						  T_HyphenMinSuffix, T_HyphenMinPrefix, 
						  T_HyphenMinWord, T_PgfLetterSpace, 
						  T_PgfMinWordSpace, T_PgfMaxWordSpace, 
						  T_PgfOptWordSpace, T_PgfCellAlignment, 
						  T_PgfLanguage, T_PgfMinJRomanLetterSpace, 
						  T_PgfMaxJRomanLetterSpace, 
						  T_PgfOptJRomanLetterSpace, 
						  T_PgfMinJLetterSpace, T_PgfMaxJLetterSpace, 
						  T_PgfOptJLetterSpace, T_PgfYakumonoType, 
						  T_PgfAcrobatLevel,
						  T_PgfCellMargins, T_PgfCellLMarginFixed, 
						  T_PgfCellRMarginFixed, T_PgfCellTMarginFixed, 
						  T_PgfCellBMarginFixed, T_PgfLocked, T_TabStop, 
						  T_PgfTag };

	PgfElement( PgfUseNextTag* element );
	PgfElement( PgfNextTag* element );
	PgfElement( PgfAlignment* element );
	PgfElement( PgfFIndent* element );
	PgfElement( PgfFIndentOffset* element );
	PgfElement( PgfFIndentRelative* element );
	PgfElement( PgfLIndent* element );
	PgfElement( PgfRIndent* element );
	PgfElement( PgfTopSeparator* element );
	PgfElement( PgfTopSepAtIndent* element );
	PgfElement( PgfTopSepOffset* element );
	PgfElement( PgfBotSeparator* element );
	PgfElement( PgfBotSepAtIndent* element );
	PgfElement( PgfBotSepOffset* element );
	PgfElement( PgfPlacement* element );
	PgfElement( PgfPlacementStyle* element );
	PgfElement( PgfRunInDefaultPunct* element );
	PgfElement( PgfSpBefore* element );
	PgfElement( PgfSpAfter* element );
	PgfElement( PgfWithNext* element );
	PgfElement( PgfWithPrev* element );
	PgfElement( PgfBlockSize* element );
	PgfElement( PgfFont* element );
	PgfElement( PgfLineSpacing* element );
	PgfElement( PgfLeading* element );
	PgfElement( PgfAutoNum* element );
	PgfElement( PgfNumTabs* element );
	PgfElement( PgfNumFormat* element );
	PgfElement( PgfNumberFont* element );
	PgfElement( PgfNumAtEnd* element );
	PgfElement( PgfHyphenate* element );
	PgfElement( HyphenMaxLines* element );
	PgfElement( HyphenMinSuffix* element );
	PgfElement( HyphenMinPrefix* element );
	PgfElement( HyphenMinWord* element );
	PgfElement( PgfLetterSpace* element );
	PgfElement( PgfMinWordSpace* element );
	PgfElement( PgfMaxWordSpace* element );
	PgfElement( PgfOptWordSpace* element );
	PgfElement( PgfCellAlignment* element );
	PgfElement( PgfLanguage* element );
	PgfElement( PgfMinJRomanLetterSpace* element );
	PgfElement( PgfMaxJRomanLetterSpace* element );
	PgfElement( PgfOptJRomanLetterSpace* element );
	PgfElement( PgfMinJLetterSpace* element );
	PgfElement( PgfMaxJLetterSpace* element );
	PgfElement( PgfOptJLetterSpace* element );
	PgfElement( PgfYakumonoType* element );
	PgfElement( PgfAcrobatLevel* element );
	PgfElement( PgfCellMargins* element );
	PgfElement( PgfCellLMarginFixed* element );
	PgfElement( PgfCellRMarginFixed* element );
	PgfElement( PgfCellTMarginFixed* element );
	PgfElement( PgfCellBMarginFixed* element );
	PgfElement( PgfLocked* element );
	PgfElement( TabStop* element );
	PgfElement( PgfTag* element );

private:
	PgfElementType _type;

	union {
		PgfUseNextTag* _pgfusenexttag;
		PgfNextTag* _pgfnexttag;
		PgfAlignment* _pgfalignment;
		PgfFIndent* _pgffindent;
		PgfFIndentOffset* _pgffindentoffset;
		PgfFIndentRelative* _pgffindentrelative;
		PgfLIndent* _pgflindent;
		PgfRIndent* _pgfrindent;
		PgfTopSeparator* _pgftopseparator;
		PgfTopSepAtIndent* _pgftopsepatindent;
		PgfTopSepOffset* _pgftopsepoffset;
		PgfBotSeparator* _pgfbotseparator;
		PgfBotSepAtIndent* _pgfbotsepatindent;
		PgfBotSepOffset * _pgfbotsepoffset;
		PgfPlacement* _pgfplacement;
		PgfPlacementStyle* _pgfplacementstyle;
		PgfRunInDefaultPunct* _pgfrunindefaultpunct;
		PgfSpBefore* _pgfspbefore;
		PgfSpAfter* _pgfspafter;
		PgfWithNext* _pgfwithnext;
		PgfWithPrev* _pgfwithprev;
		PgfBlockSize* _pgfblocksize;
		PgfFont* _pgffont;
		PgfLineSpacing* _pgflinespacing;
		PgfLeading* _pgfleading;
		PgfAutoNum* _pgfautonum;
		PgfNumTabs* _pgfnumtabs;
		PgfNumFormat* _pgfnumformat;
		PgfNumberFont* _pgfnumberfont;
		PgfNumAtEnd* _pgfnumatend;
		PgfHyphenate* _pgfhyphenate;
		HyphenMaxLines* _hyphenmaxlines;
		HyphenMinSuffix* _hyphenminsuffix;
		HyphenMinPrefix* _hyphenminprefix;
		HyphenMinWord* _hyphenminword;
		PgfLetterSpace* _pgfletterspace;
		PgfMinWordSpace* _pgfminwordspace;
		PgfMaxWordSpace* _pgfmaxwordspace;
		PgfOptWordSpace* _pgfoptwordspace;
		PgfCellAlignment* _pgfcellalignment;
		PgfLanguage* _pgflanguage;
		PgfMinJRomanLetterSpace* _pgfminjromanletterspace;
		PgfMaxJRomanLetterSpace* _pgfmaxjromanletterspace;
		PgfOptJRomanLetterSpace* _pgfoptjromanletterspace;
		PgfMinJLetterSpace* _pgfminjletterspace;
		PgfMaxJLetterSpace* _pgfmaxjletterspace;
		PgfOptJLetterSpace* _pgfoptjletterspace;
		PgfYakumonoType* _pgfyakumonotype;
		PgfAcrobatLevel* _pgfacrobatlevel;
		PgfCellMargins* _pgfcellmargins;
		PgfCellLMarginFixed* _pgfcelllmarginfixed;
		PgfCellRMarginFixed* _pgfcellrmarginfixed;
		PgfCellTMarginFixed* _pgfcelltmarginfixed;
		PgfCellBMarginFixed* _pgfcellbmarginfixed;
		PgfLocked* _pgflocked;
		TabStop* _tabstop;
		PgfTag* _pgftag;
	};
};

typedef list<PgfElement*> PgfElementList;

class Pgf
{
public:
	Pgf( PgfElementList* elements );

private:
	PgfElementList* _elements;
};



#endif
