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

#ifndef PRESENTATIONVIEWPORTSHAPE_H
#define PRESENTATIONVIEWPORTSHAPE_H

#include <KoShapeContainer.h>
#include <qpainterpath.h>
#include <KoShape.h>
#include <KoPathShape.h>

#define PresentationViewPortShapeId "PresentationViewPortShape"

class KoParameterShape;

class PresentationViewPortShape : public KoShape
{
public:
  /**
   * @brief Constructor
   * @brief initializes a basic PresentationViewPortShape shaped like a [ ]
   */
    PresentationViewPortShape();
    ~PresentationViewPortShape();
    
    /** Reimplemented methods */
    
    virtual void paint(QPainter &painter, const KoViewConverter &converter);

    virtual void saveOdf(KoShapeSavingContext &context) const;

    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);

    virtual QPainterPath outline() const;

    virtual QSizeF size() const;

    //Methods for the frame object
    void initializeAnimationProperties(); //Frame()
    /**
     * Parses frame information from a KoXmlElement,
     * And saves it into this frame.
     */
    void parseAnimationProperties(const KoXmlElement& e); //Frame(e)
        
    /**
     * Sets the default values for all frame attrbutes
     */
    //void setDefaultValues();
    /**
     * @param stream QTextStream to which all frame data is written in specific format.
     */
    //QTextStream* writeToStream();
    QString toString();
    
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
     
     
    
private:     
    void setListOfPoints(QList<QPointF> points);

   /**
     * @return a default path in the shape of '[ ]'
     */
     QPainterPath createShapePath(const QSizeF& size);
     QVector< QVector< int > > createAdjMatrix();
     void createListOfPoints(const QSizeF& size);

     QList<QPointF> m_pointsOfShape;
     int m_noOfPoints;
     QVector< QVector<int> > m_adjMatrix;
     
    //Animation properties
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
        
     QMap<QString, QString> m_animationAttributes;
  
};
#endif
