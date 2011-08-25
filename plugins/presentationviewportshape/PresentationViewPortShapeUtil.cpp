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
#include "PresentationViewPortShapeUtil.h"

///////////////Helper functions

QList<QPointF> PresentationViewPortShapeUtil::createListOfPoints(const QSizeF& size)
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
  
    return pointsOfShape;
}

void PresentationViewPortShapeUtil::printIndentation(QString& stream, unsigned int indent)
{
    static const QString INDENT("  ");
    for (unsigned int i = 0; i < indent;++i){
        stream.append(INDENT);
    }
}

QVector< QVector < int > > PresentationViewPortShapeUtil::createAdjMatrix(int noOfPoints)
{
  QVector< QVector < int > > adjMatrix(0);
  
  QVector< int > intVector(noOfPoints);
     
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
/////////////////////
