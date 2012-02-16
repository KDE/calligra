//========================================================================
//
// SplashState.h
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2011 Thomas Freitag <Thomas.Freitag@alfa.de>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#ifndef SPLASHSTATE_H
#define SPLASHSTATE_H

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "SplashTypes.h"

class SplashPattern;
class SplashScreen;
class SplashClip;
class SplashBitmap;

//------------------------------------------------------------------------
// line cap values
//------------------------------------------------------------------------

#define splashLineCapButt       0
#define splashLineCapRound      1
#define splashLineCapProjecting 2

//------------------------------------------------------------------------
// line join values
//------------------------------------------------------------------------

#define splashLineJoinMiter     0
#define splashLineJoinRound     1
#define splashLineJoinBevel     2

//------------------------------------------------------------------------
// SplashState
//------------------------------------------------------------------------

class SplashState {
public:

  // Create a new state object, initialized with default settings.
  SplashState(int width, int height, GBool vectorAntialias,
	      SplashScreenParams *screenParams);
  SplashState(int width, int height, GBool vectorAntialias,
	      SplashScreen *screenA);

  // Copy a state object.
  SplashState *copy() { return new SplashState(this); }

  ~SplashState();

  // Set the stroke pattern.  This does not copy <strokePatternA>.
  void setStrokePattern(SplashPattern *strokePatternA);

  // Set the fill pattern.  This does not copy <fillPatternA>.
  void setFillPattern(SplashPattern *fillPatternA);

  // Set the screen.  This does not copy <screenA>.
  void setScreen(SplashScreen *screenA);

  // Set the line dash pattern.  This copies the <lineDashA> array.
  void setLineDash(SplashCoord *lineDashA, int lineDashLengthA,
		   SplashCoord lineDashPhaseA);

  // Set the soft mask bitmap.
  void setSoftMask(SplashBitmap *softMaskA);

  // Set the overprint parametes.
  void setFillOverprint(GBool fillOverprintA) { fillOverprint = fillOverprintA; }
  void setStrokeOverprint(GBool strokeOverprintA) { strokeOverprint = strokeOverprintA; }
  void setOverprintMode(int overprintModeA) { overprintMode = overprintModeA; }

private:

  SplashState(SplashState *state);

  SplashCoord matrix[6];
  SplashPattern *strokePattern;
  SplashPattern *fillPattern;
  SplashScreen *screen;
  SplashBlendFunc blendFunc;
  SplashCoord strokeAlpha;
  SplashCoord fillAlpha;
  SplashCoord lineWidth;
  int lineCap;
  int lineJoin;
  SplashCoord miterLimit;
  SplashCoord flatness;
  SplashCoord *lineDash;
  int lineDashLength;
  SplashCoord lineDashPhase;
  GBool strokeAdjust;
  SplashClip *clip;
  SplashBitmap *softMask;
  GBool deleteSoftMask;
  GBool inNonIsolatedGroup;
  GBool fillOverprint;
  GBool strokeOverprint;
  int overprintMode;

  SplashState *next;		// used by Splash class

  friend class Splash;
};

#endif
