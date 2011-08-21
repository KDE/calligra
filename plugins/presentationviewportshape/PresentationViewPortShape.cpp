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

#include <qpainter.h>
#include <KoShapeBackground.h>
#include <KoXmlReader.h>
#include "PresentationViewPortShapeUtil.h"

PresentationViewPortShape::PresentationViewPortShape() : m_ns("calligra")
{
    setShapeId(PresentationViewPortShapeId);
    setName("ViewPort");//TODO remove this?
    initializeAnimationProperties();
}


PresentationViewPortShape::~PresentationViewPortShape()
{

}

void PresentationViewPortShape::setSequence(int s)
{
    m_animationAttributes["sequence"] = QString("%1").arg(s);
    //qDebug() << "animation attribute sequence set to " << s;
}

void PresentationViewPortShape::setDuration(int duration)
{
    m_animationAttributes["transition-duration-ms"] = QString("%1").arg(duration);
}

void PresentationViewPortShape::setTransitionProfile(const QString& profile)
{
    m_animationAttributes["transition-profile"] = profile;
}

void PresentationViewPortShape::setZoom(int zoom)
{
    m_animationAttributes["transition-zoom-percent"] = QString("%1").arg(zoom);
}

int PresentationViewPortShape::sequence()
{
    return m_animationAttributes["sequence"].toInt();
}

void PresentationViewPortShape::parseAnimationProperties(const KoXmlElement& e)
{
    foreach(QString key, m_animationAttributes){
      if(e.hasAttribute(m_ns + ":" + key))
        m_animationAttributes.insert(key, e.attribute(m_ns + key));
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
    m_animationAttributes.insert("timeout-enable", "0");
    m_animationAttributes.insert("sequence", QString("%1").arg(0));
    m_animationAttributes.insert("transition-zoom-percent", QString("%1").arg(1));
    m_animationAttributes.insert("transition-duration-ms", QString("%1").arg(5000));
    m_animationAttributes.insert("timeout-ms", QString("%1").arg(1000));
}


void PresentationViewPortShape::setRefId(const QString& refId)
{
    m_animationAttributes["refid"] = refId;
}

QString PresentationViewPortShape::toString()
{
    unsigned indent = 1;
    QString stream;
            
   PresentationViewPortShapeUtil::printIndentation(stream, indent++);
    stream.append("<calligra:frame");
    stream.append("\n");
    
    foreach(QString key, m_animationAttributes.keys()){
      PresentationViewPortShapeUtil::printIndentation(stream, indent);
      stream.append(m_ns + ":").append(key).append("=\"").append(m_animationAttributes.value(key)).append("\"");
      stream.append("\n");
    }
    stream.append("/>\n");
    
    return stream;
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

//TODO Remove shearing
//TODO remove other functions not applicable to this shape
QPainterPath PresentationViewPortShape::createShapePath(const QSizeF& size) 
{
    //createListOfPoints(size);
    int noOfPoints = 8;
    QList<QPointF> pointsOfShape = PresentationViewPortShapeUtil::createListOfPoints(size);
    QVector< QVector<int> > adjMatrix = PresentationViewPortShapeUtil::createAdjMatrix(noOfPoints);
    
    
    QPainterPath viewPortPath;

    //creating the path
    for(int row = 0; row < noOfPoints; row++){
      if(row == 0 || row == 4){
      viewPortPath.moveTo(pointsOfShape.at(row));
       }
      for(int col = row + 1; col < noOfPoints ; col++){
	if(adjMatrix.at(row).at(col)){
	  viewPortPath.lineTo(pointsOfShape.at(col));
	}
      }
    }

    return viewPortPath;
}

QSizeF PresentationViewPortShape::size() const
{
    return KoShape::size();
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
