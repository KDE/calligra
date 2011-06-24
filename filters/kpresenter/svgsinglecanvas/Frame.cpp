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

Frame::Frame(const KoXmlElement& e)
{
    if(e.hasAttribute("calligra:title")) {
      setTitle(e.attribute("calligra:title"));
      }

    if(e.hasAttribute("calligra:refid")) {
      setRefId(e.attribute("calligra:refid"));
      }

    if(e.hasAttribute("calligra:clip")) {
      setClip((e.attribute("calligra:clip") == "true"));
      }

    if(e.hasAttribute("calligra:hide")) {
      setHide((e.attribute("calligra:hide") == "true"));
      }

    if(e.hasAttribute("calligra:sequence")) {
      setSequence(e.attribute("calligra:sequence").toInt());
      //m_frame->setSequence(++s);
      }
     
     if(e.hasAttribute("calligra:timeout-ms")) {
      setTimeout(e.attribute("calligra:timeout-ms").toInt());
      }
    
    if(e.hasAttribute("calligra:timeout-enable")) {
      enableTimeout(e.attribute("calligra:timeout-enable").toInt());
      }
    
    if(e.hasAttribute("calligra:transition-profile")) {
      setTransitionProfile(e.attribute("calligra:transition-profile"));
      }
    
    if(e.hasAttribute("calligra:transition-duration-ms")) {
      setTransitionDuration(e.attribute("calligra:transition-duration-ms").toInt());
      }
      
    if(e.hasAttribute("calligra:transition-zoom-percent")) {
      setZoomPercent(e.attribute("calligra:transition-zoom-percent").toInt());
      }
    
    Frame();
}

      Frame::Frame() 
      {
        m_attributes.append("title");
        m_attributes.append("refid");
        m_attributes.append("transition-profile");
        m_attributes.append("hide");
        m_attributes.append("clip");
        m_attributes.append("timeout-enable");
        m_attributes.append("sequence");
        m_attributes.append("transition-zoom-percent");
        m_attributes.append("timeout-ms");
        m_attributes.append("transition-duration-ms");
        
        setDefaultValues();
      }

      Frame::~Frame() 
      {
        }
   
/*bool Frame::setAttribute(const QString& attrName, const QString& value)
{
    if(m_attrrValues.contains(attrName)){
      m_attrrValues[m_attrrValues.key(attrName)] = value;
    }
}

bool Frame::setAttribute(const QString& attrName, const bool& value)
{
    
    if(m_attrrValues.contains(attrName)){
      m_attrrValues[m_attrrValues.key(attrName)] = QString.value;
    }
}

bool Frame::setAttribute(const QString& attrName, const int& value)
{
    if(m_attrrValues.contains(attrName)){
      m_attrrValues[m_attrrValues.key(attrName)] = QString.setNum(value);
    }

}*/

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
          
QList< QString > Frame::attributes() const
{
    return m_attributes;
}
