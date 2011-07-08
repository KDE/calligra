/* This file is part of the KDE project
 * Copyright (C) 2011 Aakriti Gupta<aakriti.a.gupta@gmail.com>
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

#include <KoXmlReader.h>
#include <plugins/pathshapes/rectangle/RectangleShape.h>

#define PresentationViewPortShapeId "PresentationViewPortShape"

class PresentationViewPortShape : public RectangleShape
{
public:
    /**
     * @brief PresentationViewPortShape() - Constructor
     * Calls setDefaultValues() and
     * setRectangleStyle()
     */
   PresentationViewPortShape();
   PresentationViewPortShape(const KoXmlElement& e);
   /**
     * @brief ~PresentationViewPortShape() - Destructor
     */
   ~PresentationViewPortShape();
  
    /// reimplemented
    virtual QString pathShapeId() const;
  
    /**
     * @param stream QTextStream to which all frame data is written in specific format.
     */
    void saveSvg(QTextStream * stream);
    
    /**
     * @brief PresentationViewPortShape(const KoXmlElement& e) - Overloaded constructor
     * Parses frame information from a KoXmlElement,
     * And saves it into this frame.
     */
    void loadSvg(const KoXmlElement& e);
        
    /**
     * Sets the default values for all frame attrbutes
     */
    void setDefaultValues();
        
    void setRectangleStyle();
    /**
     * @brief generates a list of shapes contained within this ViewPort and
     * groups them into a KoShapeGroup.
     * All the ViewPort properties will be applied to this KoShapeGroup
     */
    void updateShapeGroup();
    
    void setTitle(const QString& title);
    void setRefId(const QString& refId); 
    void setTransitionProfile(const QString& transProfile);

    void setHide(bool condition);
    void setClip(bool condition);
    void enableTimeout(bool condition);

    void setSequence(int seq);
    void setZoomPercent(int zoomPercent);
    void setTimeout(int timeoutMs);
    void setTransitionDuration(int timeMs);
      
    QString title() const;
    QString refId() const; 
    QString transitionProfile() const;
      
    bool isHide() const;
    bool isClip() const;
    bool isEnableTimeout() const;
      
    int sequence() const;
    int zoomPercent() const;
    int timeout() const;
    int transitionDuration() const;
     
    /**
     * @return m_attributes 
     */
    QList<QString> attributes() const;
      
  private:
    
    QString m_title;
    QString m_refId;
    QString m_transitionProfile;
      
    bool m_hide;
    bool m_clip;
    bool m_timeoutEnable;

    int m_sequence;
    int m_transitionZoomPercent;
    int m_transitionDurationMs;
    int m_timeoutMs;
    
    /**
     * @brief m_attributes List of all attributes a frame can have
     */
    QList<QString> m_attributes;
  
};
