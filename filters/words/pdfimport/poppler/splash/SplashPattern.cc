//========================================================================
//
// SplashPattern.cc
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2010, 2011 Thomas Freitag <Thomas.Freitag@alfa.de>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#include <config.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include "SplashMath.h"
#include "SplashScreen.h"
#include "SplashPattern.h"

//------------------------------------------------------------------------
// SplashPattern
//------------------------------------------------------------------------

SplashPattern::SplashPattern() {
}

SplashPattern::~SplashPattern() {
}

//------------------------------------------------------------------------
// SplashSolidColor
//------------------------------------------------------------------------

SplashSolidColor::SplashSolidColor(SplashColorPtr colorA) {
  splashColorCopy(color, colorA);
}

SplashSolidColor::~SplashSolidColor() {
}

GBool SplashSolidColor::getColor(int x, int y, SplashColorPtr c) {
  splashColorCopy(c, color);
  return gTrue;
}

void SplashSolidColor::overprint(GBool op, Guchar aSrc, SplashColorPtr cSrc, 
                                 Guchar aDest, SplashColorPtr cDest, 
                                 SplashColorPtr colorResult) {
  // default for overprint is knockout:
  colorResult[0] = (Guchar)(((aDest - aSrc) * cDest[0] +
                   aSrc * cSrc[0]) / aDest);
  colorResult[1] = (Guchar)(((aDest - aSrc) * cDest[1] +
                   aSrc * cSrc[1]) / aDest);
  colorResult[2] = (Guchar)(((aDest - aSrc) * cDest[2] +
                   aSrc * cSrc[2]) / aDest);
  colorResult[3] = (Guchar)(((aDest - aSrc) * cDest[3] +
                   aSrc * cSrc[3]) / aDest);
}
