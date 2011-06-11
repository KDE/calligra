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
  #include "SvgAnimationData.h"
  #include "Frame.h"
  #include "KoShape.h"

  SvgAnimationData::SvgAnimationData()
  {
  }

  SvgAnimationData::~SvgAnimationData()
  {
  }

  void SvgAnimationData::setFrame(Frame *frame)
  {
   m_frame = *frame;
  }

  Frame SvgAnimationData::frame()
  {
    return m_frame;
  }

  //Temporary function.
  //Only for testing
  void SvgAnimationData::addNewFrame(KoShape *shape, Frame *frame)
  {
    SvgAnimationData *applicationData = dynamic_cast<SvgAnimationData*>( shape->applicationData() );

    if ( applicationData == 0 ) {
          applicationData = new SvgAnimationData();
    }

    applicationData->setFrame(frame);
    shape->setApplicationData(applicationData);
    
    //'frame' saved as a part of 'shape' now.
    //The SvgWriter will use this 'frame' to extract frame specific attributes.
  }
    
