/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_font.h"
#include "unitconv.h"

FTag::FTag(char const *value)
{
	_value = value;
}


FPostScriptName::FPostScriptName(char const *value)
{
	_value = value;
}


FPlatformName::FPlatformName(char const *value)
{
	_value = value;
}


FFamily::FFamily(char const *value)
{
	_value = value;
}


FVar::FVar(char const *value)
{
	_value = value;
}


FWeight::FWeight(char const *value)
{
	_value = value;
}


FAngle::FAngle(char const *value)
{
	_value = value;
}


FEncoding::FEncoding(char const *value)
{
	_value = value;
}


FSize::FSize(double value, const char* unit)
{
	_value = unitconversion( value, unit );
}


FUnderlining::FUnderlining(char const *value)
{
	_value = value;
}


FOverline::FOverline(char const *value)
{
	_value = value;
}


FStrike::FStrike(char const *value)
{
	_value = value;
}


FChangeBar::FChangeBar(char const *value)
{
	_value = value;
}


FOutline::FOutline(char const *value)
{
	_value = value;
}


FShadow::FShadow(char const *value)
{
	_value = value;
}


FPairKern::FPairKern(char const *value)
{
	_value = value;
}


FTsume::FTsume(char const *value)
{
	_value = value;
}


FCase::FCase(char const *value)
{
	_value = value;
}


FPosition::FPosition(char const *value)
{
	_value = value;
}


FDX_::FDX_(double value)
{
	_value = value;
}


FDY_::FDY_(double value)
{
	_value = value;
}


FDW_::FDW_(double value)
{
	_value = value;
}


FStretch::FStretch(double value)
{
	_value = value;
}


FLanguage::FLanguage(char const *value)
{
	_value = value;
}


FLocked::FLocked(char const *value)
{
	_value = value;
}


FSeparation::FSeparation(int value)
{
	_value = value;
}


FColor::FColor(char const *value)
{
	_value = value;
}


Font::Font(FontElementList *elements)
{
	_elements = elements;
}


FontElement::FontElement(FTag * element)
{
	_type = T_FTag;
	_ftag = element;
}


FontElement::FontElement(FPostScriptName * element)
{
	_type = T_FPostScriptName;
	_fpostscriptname = element;
}


FontElement::FontElement(FPlatformName * element)
{
	_type = T_FPlatformName;
	_fplatformname = element;
}


FontElement::FontElement(FFamily * element)
{
	_type = T_FFamily;
	_ffamily = element;
}


FontElement::FontElement(FVar * element)
{
	_type = T_FVar;
	_fvar = element;
}


FontElement::FontElement(FWeight * element)
{
	_type = T_FWeight;
	_fweight = element;
}


FontElement::FontElement(FAngle * element)
{
	_type = T_FAngle;
	_fangle = element;
}


FontElement::FontElement(FEncoding * element)
{
	_type = T_FEncoding;
	_fencoding = element;
}


FontElement::FontElement(FSize * element)
{
	_type = T_FSize;
	_fsize = element;
}


FontElement::FontElement(FUnderlining * element)
{
	_type = T_FUnderlining;
	_funderlining = element;
}


FontElement::FontElement(FOverline * element)
{
	_type = T_FOverline;
	_foverline = element;
}


FontElement::FontElement(FStrike * element)
{
	_type = T_FStrike;
	_fstrike = element;
}


FontElement::FontElement(FChangeBar * element)
{
	_type = T_FChangeBar;
	_fchangebar = element;
}


FontElement::FontElement(FOutline * element)
{
	_type = T_FOutline;
	_foutline = element;
}


FontElement::FontElement(FShadow * element)
{
	_type = T_FShadow;
	_fshadow = element;
}


FontElement::FontElement(FPairKern * element)
{
	_type = T_FPairKern;
	_fpairkern = element;
}


FontElement::FontElement(FTsume * element)
{
	_type = T_FTsume;
	_ftsume = element;
}


FontElement::FontElement(FCase * element)
{
	_type = T_FCase;
	_fcase = element;
}


FontElement::FontElement(FPosition * element)
{
	_type = T_FPosition;
	_fposition = element;
}


FontElement::FontElement(FDX_ * element)
{
	_type = T_FDX;
	_fdx = element;
}


FontElement::FontElement(FDY_ * element)
{
	_type = T_FDY;
	_fdy = element;
}


FontElement::FontElement(FDW_ * element)
{
	_type = T_FDW;
	_fdw = element;
}


FontElement::FontElement(FStretch * element)
{
	_type = T_FStretch;
	_fstretch = element;
}


FontElement::FontElement(FLanguage * element)
{
	_type = T_FLanguage;
	_flanguage = element;
}


FontElement::FontElement(FLocked * element)
{
	_type = T_FLocked;
	_flocked = element;
}


FontElement::FontElement(FSeparation * element)
{
	_type = T_FSeparation;
	_fseparation = element;
}


FontElement::FontElement(FColor * element)
{
	_type = T_FColor;
	_fcolor = element;
}





