// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2004 Thorsten Zachmann <zachmann@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef EFFECTHANDLER_H
#define EFFECTHANDLER_H

#include <qpixmap.h>
#include <qptrlist.h>
#include <qstring.h>

#include "global.h"
#include "presstep.h"

class QPaintDevice;
class KPrObject;
class KPrView;

class KPrEffectHandler
{
public:
  /**
   * Creates an EffectsHandler for the effect step step and the effect sub step subStep.
   * step    the effect step for which the effect should be displayed
   * subStep the sub effect step for which the effect should be displayed
   * back    set to true if the effect step was reached by going backwards
   * dst     the paint device on which the Effects will be shown
   * src     this pixmap holds the pixmap before any effects are started
   * objects all objects of the page
   * view    pointer to the active KPrView
   */
  KPrEffectHandler( PresStep step, bool back, QPaintDevice *dst, QPixmap *src, const QPtrList<KPrObject> &objects, KPrView *view, int _presSpeed );

  /**
   * Delete the KPrEffectHandler.
   */
  ~KPrEffectHandler();

  /**
   * doEffect handle the next step of the object effect.
   * Returns true if all effects in the current step are finished.
   * In automatic presentation mode:
   * On the first step it stops the automatic presentation timer.
   * The automatic presentation timer will be restared when all
   * effects are done.
   */
  bool doEffect();

  /**
   * Ends the effect for the current step by displaying all objects
   * on their final position.
   * In automatic presentation mode:
   * Restarts the automatic presentation timer.
   */
  void finish();
protected:
  /**
   * Calls the right appear effect method for the object object.
   * Returns true if the effect for the object is finished.
   */
  bool doAppearEffectStep( KPrObject *object );

  /**
   * Calls the right disappear effect method for the object object.
   * Returns true if the effect for the object is finished.
   */
  bool doDisappearEffectStep( KPrObject *object );

  /**
   * Effect None
   * The object just appears on its position
   * Returns allways true as the object is in is final position.
   */
  bool appearNone( KPrObject *object );

  /**
   * Effect Come From Left
   * The object comes in from the left to its position
   * Returns true if the object is on its final position.
   */
  bool appearComeLeft( KPrObject *object );

  /**
   * Effect Come From Top
   * The object comes in from the top to its position
   * Returns true if the object is on its final position.
   */
  bool appearComeTop( KPrObject *object );

  /**
   * Effect Come From Right
   * The object comes in from the right to its position
   * Returns true if the object is on its final position.
   */
  bool appearComeRight( KPrObject *object );

  /**
   * Effect Come From Bottom
   * The object comes in from the bottom to its position
   * Returns true if the object is on its final position.
   */
  bool appearComeBottom( KPrObject *object );

  /**
   * Effect Come From Left/Top
   * The object comes in from the top left corner to its position
   * Returns true if the object is on its final position.
   */
  bool appearComeLeftTop( KPrObject *object );

  /**
   * Effect Come From Left/Buttom
   * The object comes in from the bottom left corner to its position
   * Returns true if the object is on its final position.
   */
  bool appearComeLeftBottom( KPrObject *object );

  /**
   * Effect Come From Right/Top
   * The object comes in from the top right corner to its position
   * Returns true if the object is on its final position.
   */
  bool appearComeRightTop( KPrObject *object );

  /**
   * Effect Come From Right/Buttom
   * The object comes in from the bottom right corner to its position
   * Returns true if the object is on its final position.
   */
  bool appearComeRightBottom( KPrObject *object );

  /**
   * Effect Wipe From Left
   * Returns true if the object is fully shown.
   */
  bool appearWipeLeft( KPrObject *object );

  /**
   * Effect Wipe From Right
   * Returns true if the object is fully shown.
   */
  bool appearWipeRight( KPrObject *object );

  /**
   * Effect Wipe From Top
   * Returns true if the object is fully shown.
   */
  bool appearWipeTop( KPrObject *object );

  /**
   * Effect Wipe From Bottom
   * Returns true if the object is fully shown.
   */
  bool appearWipeBottom( KPrObject *object );

  /**
   * Effect Disappear To Left
   * The object moves from it's position to the left out of the screen
   * Returns true if the object is out of the screen.
   */
  bool disappearGoLeft( KPrObject *object );

  /**
   * Effect Disappear To Right
   * The object moves from it's position to the right out of the screen
   * Returns true if the object is out of the screen.
   */
  bool disappearGoRight( KPrObject *object );

  /**
   * Effect Disappear To Top
   * The object moves from it's position to the top out of the screen
   * Returns true if the object is out of the screen.
   */
  bool disappearGoTop( KPrObject *object );

  /**
   * Effect Disappear To Bottom
   * The object moves from it's position to the bottom out of the screen
   * Returns true if the object is out of the screen.
   */
  bool disappearGoBottom( KPrObject *object );

  /**
   * Effect Disappear To Left/Top
   * The object moves from it's position to the top left out of the screen
   * Returns true if the object is out of the screen.
   */
  bool disappearGoLeftTop( KPrObject *object );

  /**
   * Effect Disappear To Left/Bottom
   * The object moves from it's position to the bottom left out of the screen
   * Returns true if the object is out of the screen.
   */
  bool disappearGoLeftBottom( KPrObject *object );

  /**
   * Effect Disappear To Right/Top
   * The object moves from it's position to the top right out of the screen
   * Returns true if the object is out of the screen.
   */
  bool disappearGoRightTop( KPrObject *object );

  /**
   * Effect Disappear To Right/Bottom
   * The object moves from it's position to the bottom right out of the screen
   * Returns true if the object is out of the screen.
   */
  bool disappearGoRightBottom( KPrObject *object );

  /**
   * Effect Wipe From Left
   * Returns true if the object is fully shown.
   */
  bool disappearWipeLeft( KPrObject *object );

  /**
   * Effect Wipe From Right
   * Returns true if the object is fully shown.
   */
  bool disappearWipeRight( KPrObject *object );

  /**
   * Effect Wipe From Top
   * Returns true if the object is fully shown.
   */
  bool disappearWipeTop( KPrObject *object );

  /**
   * Effect Wipe From Bottom
   * Returns true if the object is fully shown.
   */
  bool disappearWipeBottom( KPrObject *object );

  /**
   * Draws the object object to the pixmap screen with the offset defined
   * in x and y. If clipRect is set the only the clipRect will be painted.
   */
  void drawObject( KPrObject *object, int x, int y, QPixmap *screen, QRect *clipRect = 0 );

  /**
   * The internal step of the effect. This is used to calculate the position
   * of the object.
   */
  int m_effectStep;

  /**
   * The effect step which should be animated.
   */
  PresStep m_step;

  /**
   * True when the effect step was reached by going backwards
   */
  bool m_back;

  /**
   * Paint device on which the effects will be displayed
   */
  QPaintDevice *m_dst;

  /**
   * Pixmap used as source for displaying on.
   * m_src contains all objects which are allready on the
   * final position.
   */
  QPixmap *m_src;

  /**
   * Is used for printing all objects before bitblt to m_dst
   */
  QPixmap m_paint;

  /**
   * List of all objects of the current page
   */
  QPtrList<KPrObject> m_objects;

  /**
   * List of all objects which appear in the current step.
   * If a object has reached its final position it is removed from
   * this list
   */
  QPtrList<KPrObject> m_appearEffectObjects;

  /**
   * List of all objects which disappear in the current step.
   * If a object has disappeared it is removed from this list
   */
  QPtrList<KPrObject> m_disappearEffectObjects;

  /// the active KPrView
  KPrView *m_view;

  /**
   * The width of a step.
   */
  int m_stepWidth;

  /**
   * The height of a step.
   */
  int m_stepHeight;

  /**
   * The rects which have to be bitblt
   */
  QPtrList<QRect> m_repaintRects;

  /**
   * The rects which have to be bitblt as the objects
   * are no longer there.
   */
  QPtrList<QRect> m_lastRepaintRects;

  /**
   * The filename of the sound file which should be played during the effect.
   */
  QString m_soundEffect;

  /**
   * Used the set the autopresentation timer.
   */
  int m_objectTimer;
};


#endif /* EFFECTHANDLER_H */
