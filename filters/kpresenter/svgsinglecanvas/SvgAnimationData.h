  /* This file is part of the KDE project
  * Copyright (C) 2011 Aakriti Gupta <aakriti.a.gupta@gmail.com>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Library General Public
  * License as published by the Free Software Foundation; either
  * version 2 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Library General Public License for more details.
  *
  * You should have received a copy of the GNU Library General Public License
  * along with this library; see the file COPYING.LIB.  If not, write to
  * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  * Boston, MA 02110-1301, USA.
  */

#ifndef SVGANIMATIONDATA_H
#define SVGANIMATIONDATA_H

#include "Frame.h"
#include <libs/flake/KoShapeApplicationData.h>

class Frame;
class KoShape;

/**
 * Inherits from KoshapeApplicationData
 * To store a Frame object to a KoShape
 */
class SvgAnimationData : public KoShapeApplicationData
{
  public:
    SvgAnimationData();
    ~SvgAnimationData();

    /**
     * Sets the frame properties for the calling shape
     */
    void setFrame(Frame *frame);
    /**
     * Returns the Frame object which contains information for the shape animation.
     */
    Frame frame();
    /**
     * This function adds a new frame to a shape.
     * This function will only be used for tests.
     * Eventually, will be removed from here.
     */
    void addNewFrame(KoShape *shape, Frame *frame);
    
  private:
      /**
       * The Frame object which contains frame attributes and their values
       */
      Frame m_frame;
};

#endif /*SVGANIMATIONDATA_H */
