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

      Frame::Frame() 
      {
        setDefaultValues();
      }

      Frame::~Frame() 
      {
        }
        
      void Frame::setDefaultValues()
      {
      m_title = "No Title";
      m_refId = "No Ref ID"; // What should this be?
      m_transitionProfile = "linear";

      m_hide = false;
      m_clip = true;
      m_timeoutEnable = false;

      m_sequence = 0;
      m_transitionZoomPercent = 100; // How is zoom factor used in the javascript? % or factor?
      m_transitionDurationMs = 1000; 
      m_timeoutMs = 5000;

      }

        void Frame::setTitle(const QString& title) 
        {
          m_title = title;
        }
        
        void Frame::setRefId(const QString& refId)
        {
          m_refId = refId;
        }
        
        void Frame::setSequence(int sequence) 
        {
          m_sequence = sequence;
        }
        
        void Frame::setZoomPercent(int zoomPercent) 
        {
          m_transitionZoomPercent = zoomPercent;
        }
        
        void Frame::setTransitionProfile(const QString& transProfile) 
        {
          m_transitionProfile = transProfile;
        }
        
        void Frame::setTransitionDuration(int timeMs) 
        {
          m_transitionDurationMs = timeMs;
        }
        
  void Frame::setTimeout(int timeoutMs)
  {
  m_timeoutMs = timeoutMs;
  }

  void Frame::enableTimeout(bool condition)
  {
  m_timeoutEnable = condition;
  }

  void Frame::setClip(bool condition)
  {
  m_clip = condition;
  }

  void Frame::setHide(bool condition)
  {
  m_hide = condition;
  }

        QString Frame::title() const
        {
          return m_title;
        }
        
        QString Frame::refId() const
        {
          return m_refId;
        }
        
  QString Frame::transitionProfile() const
  {
  return m_transitionProfile;
  }

  bool Frame::isHide() const
  {
  return m_hide;
  }

  bool Frame::isClip() const
  {
  return m_clip;
  }

  bool Frame::isEnableTimeout() const
  {
  return m_timeoutEnable;
  }

        int Frame::sequence() const
        {
          return m_sequence;
        }
        
        int Frame::zoomPercent() const
        {
          return m_transitionZoomPercent;
          }
        
  int Frame::timeout() const
  {
  return m_timeoutMs;
  }

        
        int Frame::transitionDuration() const 
        {
          return m_transitionDurationMs;
        }
          
