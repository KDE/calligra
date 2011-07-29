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

#include "PresentationViewPortShape.h"
#include <KoPathPoint.h>
#include <qpainter.h>
#include <KoShapeBackground.h>
#include <KoXmlReader.h>

PresentationViewPortShape::PresentationViewPortShape() : m_noOfPoints(8)
{
    setShapeId(PresentationViewPortShapeId);
    m_adjMatrix = createAdjMatrix();
    setName("ViewPort");//TODO remove this?
    initializeAnimationProperties();
}


PresentationViewPortShape::~PresentationViewPortShape()
{

}

void PresentationViewPortShape::parseAnimationProperties(const KoXmlElement& e)
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
    

}

void PresentationViewPortShape::initializeAnimationProperties()
{
    //Initializing the map with default values of all attributes
    m_animationAttributes.insert("title", "No title");
    m_animationAttributes.insert("refid", "null");
    m_animationAttributes.insert("transition-profile", "linear");
    m_animationAttributes.insert("hide", "true");
    m_animationAttributes.insert("clip", "true");
    m_animationAttributes.insert("timeout-enable", "false");
    m_animationAttributes.insert("sequence", QString("%1").arg(0));
    m_animationAttributes.insert("transition-zoom-percent", QString("%1").arg(1));
    m_animationAttributes.insert("transition-duration-ms", QString("%1").arg(5000));
    m_animationAttributes.insert("timeout-ms", QString("%1").arg(1000));
}

static void printIndentation(QString& stream, unsigned int indent)
{
    static const QString INDENT("  ");
    for (unsigned int i = 0; i < indent;++i){
        stream.append(INDENT);
    }
}

QString PresentationViewPortShape::toString()
{
    unsigned indent = 1;
    QString stream;
    QString ns("calligra:");
        
    printIndentation(stream, indent++);
    stream.append("<calligra:frame");
    stream.append("\n");
    
    foreach(QString key, m_animationAttributes.keys()){
      printIndentation(stream, indent);
      stream.append(ns).append(key).append("=\"").append(m_animationAttributes.value(key)).append("\"");
      stream.append("\n");
    }
    stream.append("/>\n");
    
    return stream;
}
QVector< QVector < int > > PresentationViewPortShape::createAdjMatrix()
{
  m_noOfPoints = 8;
  QVector< QVector < int > > adjMatrix(0);
  
  QVector< int > intVector(m_noOfPoints);
     
      intVector.fill(0);
      intVector.insert(1, 1);
      adjMatrix.append(intVector);
      
      intVector.fill(0);
      intVector.insert(0, 1);    
      intVector.insert(2, 1);
      adjMatrix.append(intVector);
      
      intVector.fill(0);
      intVector.insert(1, 1);
      intVector.insert(3, 1);
      adjMatrix.append(intVector);
      
      intVector.fill(0);
      intVector.insert(2, 1);
      adjMatrix.append(intVector);
      
      intVector.fill(0);
      intVector.insert(5, 1);
      adjMatrix.append(intVector);
      
      intVector.fill(0);
      intVector.insert(4, 1);
      intVector.insert(6, 1);
      adjMatrix.append(intVector);
      
      intVector.fill(0);
      intVector.insert(5, 1);
      intVector.insert(7, 1);
      adjMatrix.append(intVector);
      
      intVector.fill(0);
      intVector.insert(6, 1);
      adjMatrix.append(intVector);
            
      /*for(int i = 0; i < m_noOfPoints; i++)
	for(int j = 0; j < m_noOfPoints; j++){
	 qDebug() << adjMatrix.at(i).at(j) ;
	}
	qDebug() << endl;*/
  return adjMatrix;
    }

void PresentationViewPortShape::paint(QPainter& painter, const KoViewConverter& converter)
{
    applyConversion( painter, converter );
        
    painter.setPen(QPen(QColor(Qt::black), 1, Qt::DashLine,
                     Qt::FlatCap, Qt::MiterJoin)); 
      
    painter.drawPath(createShapePath(outline().boundingRect().size()));
}

QPainterPath PresentationViewPortShape::outline() const
{
    return KoShape::outline();
    }

void PresentationViewPortShape::createListOfPoints(const QSizeF& size)
{
    qreal unit = 15.0; //TODO change according to 'size' and keep a minimum value
    qreal heightUnit = size.height();
    
    QList<QPointF> pointsOfShape;
    pointsOfShape.append(QPointF(unit, 0.0));
    pointsOfShape.append(QPointF(0.0, 0.0));
    pointsOfShape.append(QPointF(0.0, heightUnit));
    pointsOfShape.append(QPointF(unit, heightUnit));
    pointsOfShape.append(QPointF((size.width() - unit), heightUnit));
    pointsOfShape.append(QPointF(size.width(), heightUnit));
    pointsOfShape.append(QPointF(size.width(), 0.0));
    pointsOfShape.append(QPointF((size.width() - unit), 0.0));
  
    setListOfPoints(pointsOfShape);
        
  }

void PresentationViewPortShape::setListOfPoints(QList< QPointF > points)
{
    m_pointsOfShape = points;
}

//TODO Remove shearing
//TODO remove other functions not applicable to this shape
QPainterPath PresentationViewPortShape::createShapePath(const QSizeF& size) 
{
    createListOfPoints(size);
  
    QPainterPath viewPortPath;

    //creating the path
    for(int row = 0; row < m_noOfPoints; row++){
      if(row == 0 || row == 4){
      viewPortPath.moveTo(m_pointsOfShape.at(row));
       }
      for(int col = row + 1; col < m_noOfPoints ; col++){
	if(m_adjMatrix.at(row).at(col)){
	  viewPortPath.lineTo(m_pointsOfShape.at(col));
	}
      }
    }

    return viewPortPath;
}

QSizeF PresentationViewPortShape::size() const
{
    return KoShape::size();
}

void PresentationViewPortShape::setTitle(const QString& title) 
{  
    m_title = title;
}
        
void PresentationViewPortShape::setRefId(const QString& refId)
{
    m_refId = refId;
}
        
void PresentationViewPortShape::setSequence(int sequence) 
{
    m_sequence = sequence;
}
        
void PresentationViewPortShape::setZoomPercent(int zoomPercent) 
{
    m_transitionZoomPercent = zoomPercent;
}
        
void PresentationViewPortShape::setTransitionProfile(const QString& transProfile) 
{
    m_transitionProfile = transProfile;
}
        
void PresentationViewPortShape::setTransitionDuration(int timeMs) 
{
    m_transitionDurationMs = timeMs;
}
        
void PresentationViewPortShape::setTimeout(int timeoutMs)
{
    m_timeoutMs = timeoutMs;
}

void PresentationViewPortShape::enableTimeout(bool condition)
{
    m_timeoutEnable = condition;
}

void PresentationViewPortShape::setClip(bool condition)
{
    m_clip = condition;
}

void PresentationViewPortShape::setHide(bool condition)
{
    m_hide = condition;
 }

QString PresentationViewPortShape::title() const
{
    return m_title;
}
        
QString PresentationViewPortShape::refId() const
{
    return m_refId;
}
       
QString PresentationViewPortShape::transitionProfile() const
{
    return m_transitionProfile;
}

bool PresentationViewPortShape::isHide() const
{
    return m_hide;
}

bool PresentationViewPortShape::isClip() const
{
    return m_clip;
}

bool PresentationViewPortShape::isEnableTimeout() const
{
    return m_timeoutEnable;
}

int PresentationViewPortShape::sequence() const
{
    return m_sequence;
}
        
int PresentationViewPortShape::zoomPercent() const
{
    return m_transitionZoomPercent;
}
        
int PresentationViewPortShape::timeout() const
{
    return m_timeoutMs;
}

int PresentationViewPortShape::transitionDuration() const 
{
    return m_transitionDurationMs;
}
          
//TODO: What will be done in this?
// Neccessary to write to and from an ODF
bool PresentationViewPortShape::loadOdf(const KoXmlElement& element, KoShapeLoadingContext& context)
{
    return true;
}

void PresentationViewPortShape::saveOdf(KoShapeSavingContext& context) const
{
//TODO
}


