/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_FONT
#define _TREEBUILD_FONT

#include <list>
#include <string>

class FTag
{
public:
	FTag( const char* );

private:
	string _value;
};


class FPostScriptName
{
public:
	FPostScriptName( const char* );

private:
	string _value;
};


class FFamily
{
public:
	FFamily ( const char* );
		
private:
	string _value;
};


class FPlatformName
{
public:
	FPlatformName ( const char* );

private:
	string _value;
};


class FVar
{
public:
	FVar( const char* );

private:
	string _value;
};


class FWeight
{
public:
	FWeight ( const char* );

private:
	string _value;
};


class FAngle
{
public:
	FAngle ( const char* );

private:
	string _value;
};


class FEncoding
{
public:
	FEncoding ( const char* );

private:
	string _value;
};


class FSize
{
public:
	FSize ( double );

private:
	double _value;
};


class FUnderlining
{
public:
	FUnderlining( const char* );

private:
	string _value;
};


class FOverline
{
public:
	FOverline( const char* );

private:
	string _value;
};


class FStrike
{
public:
	FStrike( const char* );

private:
	string _value;
};


class FChangeBar
{
public:
	FChangeBar( const char* );

private:
	string _value;
};


class FOutline
{
public:
	FOutline( const char* );

private:
	string _value;
};


class FShadow
{
public:
	FShadow( const char* );

private:
	string _value;
};


class FPairKern
{
public:
	FPairKern( const char* );

private:
	string _value;
};


class FTsume
{
public:
	FTsume( const char* );

private:
	string _value;
};


class FCase
{
public:
	FCase( const char* );

private:
	string _value;
};


class FPosition
{
public:
	FPosition( const char* );

private:
	string _value;
};


class FDX_
{
public:
	FDX_( double );

private:
	double _value;
};


class FDY_
{
public:
	FDY_( double );

private:
	double _value;
};


class FDW_
{
public:
	FDW_( double );

private:
	double _value;
};


class FStretch
{
public:
	FStretch( double );

private:
	double _value;
};


class FLanguage
{
public:
	FLanguage( const char* );

private:
	string _value;
};


class FLocked
{
public:
	FLocked( const char* );

private:
	string _value;
};


class FSeparation
{
public:
	FSeparation( int );

private:
	int _value;
};


class FColor
{
public:
	FColor( const char* );

private:
	string _value;
};


class FontElement
{
public:
	enum FontElementType {T_FTag, T_FPostScriptName, T_FPlatformName, 
						  T_FFamily, T_FVar, T_FWeight, T_FAngle, 
						  T_FEncoding, T_FSize,	T_FUnderlining,
						  T_FOverline, T_FStrike, T_FChangeBar,
						  T_FOutline, T_FShadow, T_FPairKern, T_FTsume,	
						  T_FCase, T_FPosition, T_FDX, T_FDY, T_FDW, 
						  T_FStretch, T_FLanguage, T_FLocked, T_FSeparation,
						  T_FColor };

	FontElement( FTag* element );
	FontElement( FPostScriptName* element );
	FontElement( FPlatformName* element );
	FontElement( FFamily* element );
	FontElement( FVar* element );
	FontElement( FWeight* element );
	FontElement( FAngle* element );
	FontElement( FEncoding* element );
	FontElement( FSize* element );
	FontElement( FUnderlining* element );
	FontElement( FOverline* element );
	FontElement( FStrike* element );
	FontElement( FChangeBar* element );
	FontElement( FOutline* element );
	FontElement( FShadow* element );
	FontElement( FPairKern* element );
	FontElement( FTsume* element );
	FontElement( FCase* element );
	FontElement( FPosition* element );
	FontElement( FDX_* element );
	FontElement( FDY_* element );
	FontElement( FDW_* element );
	FontElement( FStretch* element );
	FontElement( FLanguage* element );
	FontElement( FLocked* element );
	FontElement( FSeparation* element );
	FontElement( FColor* element );

private:
	FontElementType _type;

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

typedef list<FontElement*> FontElementList;

class Font
{
public:
	Font( FontElementList* );

private:
	FontElementList* _elements;
};




#endif
