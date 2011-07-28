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

PresentationViewPortShape::PresentationViewPortShape() : m_noOfPoints(8)
{
    setShapeId(PresentationViewPortShapeId);
    m_adjMatrix = createAdjMatrix();
    setName("ViewPort");
    }


PresentationViewPortShape::~PresentationViewPortShape()
{

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
    return (outline().boundingRect().size());
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


