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
  #include "Frame.h"

  Frame::Frame() {
    setDefaultValues();
  }

  Frame::~Frame() {
    }
    
  void Frame::setDefaultValues()
  {
  //setTitle("Frame Title");
  //*refId = "Shape #1";
  m_sequence = 1;
  m_zoomFactor = 1; // How is zoom factor used in the javascript? % or factor?
  //*transitionStyle = "linear";
  m_timeMs = 100;

  }

    void Frame::setTitle(const QString &title) 
    {
      *(this->m_title) = title;
    }
    void Frame::setRefId(QString *refId)
    {
      this->m_refId = refId;
    }
    void Frame::setSequence(int sequence) 
    {
      this->m_sequence = sequence;
    }
    void Frame::setZoomPercent(qreal zoomFactor) 
    {
      this->m_zoomFactor = zoomFactor;
    }
    void Frame::setTransitionStyle(QString *transitionStyle) 
    {
      this->m_transitionStyle = transitionStyle;
    }
    void Frame::setTransitionDuration(qreal timeMs) 
    {
      this->m_timeMs = timeMs;
    }
    
    QString* Frame::title() const
    {
      return m_title;
    }
    QString* Frame::refId() const
    {
      return m_refId;
    }
    int Frame::sequence() const
    {
      return m_sequence;
    }
    qreal Frame::zoomPercent() const
    {
      return m_zoomFactor;
      
    }
    QString* Frame::transitionStyle() const 
    {
      return m_transitionStyle;
    }
    qreal Frame::transitionDuration() const 
    {
      return m_timeMs;
    }
    