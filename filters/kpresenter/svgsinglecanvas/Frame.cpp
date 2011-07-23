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
  //static int s = 0;
  //TODO:Use namespace instead of writing 'calligra:' again and again
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
      //setSequence(++s);
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
        
static void printIndentation(QTextStream *stream, unsigned int indent)
//void SvgWriter_generic::printIndentation(QTextStream* stream, unsigned int indent)
{
    static const QString INDENT("  ");
    for (unsigned int i = 0; i < indent;++i)
    *stream << INDENT;
}

void Frame::writeToStream(QTextStream * stream)
{
    unsigned indent = 1;
    //QTextStream * stream;
    
    printIndentation(stream, indent++);
    *stream << "<calligra:frame" << endl;
       
    printIndentation(stream, indent);
    *stream << "calligra:" << "title=\"" << title() << "\"" << endl;
    printIndentation(stream, indent);
    *stream << "calligra:" << "refid=\"" << refId() << "\"" << endl;
    printIndentation(stream, indent);
    *stream << "calligra:" << "transition-profile=\"" << transitionProfile() << "\"" << endl;
    printIndentation(stream, indent);
    *stream << "calligra:" << "hide=\"";
    if(isHide())
      *stream << "true";
    else
     *stream << "false";
    *stream << "\"" << endl;
     
    printIndentation(stream, indent);
    *stream << "calligra:" << "clip=\"";
    if(isClip())
      *stream << "true";
    else
      *stream << "false";
    *stream << "\"" << endl;
    
    printIndentation(stream, indent);
    *stream << "calligra:" << "timeout-enable=\"" << isEnableTimeout() << "\"" << endl;
    printIndentation(stream, indent);
    *stream << "calligra:" << "sequence=\"" << sequence() << "\"" << endl;
    printIndentation(stream, indent);
    *stream << "calligra:" << "transition-zoom-percent=\"" << zoomPercent() << "\"" << endl;
    printIndentation(stream, indent);
    *stream << "calligra:" << "timeout-ms=\"" << timeout() << "\"" << endl;
    printIndentation(stream, indent);
    *stream << "calligra:" << "transition-duration-ms=\"" << transitionDuration() << "\"" << endl;
              
    *stream << "/>" << endl;
     
}
   
void Frame::setDefaultValues()
{
    m_title = "No Title";
    m_refId = "No Ref ID"; 
    m_transitionProfile = "linear";
    m_hide = true;
    m_clip = true;
    m_timeoutEnable = false;

    m_sequence = 0;
    m_transitionZoomPercent = 1;
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
