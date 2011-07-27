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


QString PresentationViewPortShape::toString()
{
   return m_path;
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

void PresentationViewPortShape::setAdjMatrix(QVector< QVector< int > >* matrix)
{
    //m_adjMatrix = matrix;
}

QVector< QVector< int > > PresentationViewPortShape::adjMatrix()
{
    return m_adjMatrix;
}
/*
QString PresentationViewPortShape::shapeId() const
{
    return PresentationViewPortShapeId;
}
*/
void PresentationViewPortShape::paint(QPainter& painter, const KoViewConverter& converter)
{
    applyConversion( painter, converter );
        
    painter.setPen(QPen(QColor(Qt::black), 1, Qt::DashLine,
                     Qt::FlatCap, Qt::MiterJoin)); 
      
    painter.drawPath(createShapePath(outline().boundingRect().size()));
    //painter.drawPath(m_pathShape);
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

QList< QPointF > PresentationViewPortShape::listOfPoints()
{
    return m_pointsOfShape;
}

//TODO Remove shearing
//TODO remove other functions not applicable to this shape
QPainterPath PresentationViewPortShape::createShapePath(const QSizeF& size) 
{
    createListOfPoints(size);
  
    QPainterPath viewPortPath;
    m_path.clear();
 
    //creating the path
    for(int row = 0; row < m_noOfPoints; row++){
      if(row == 0 || row == 4){
      viewPortPath.moveTo(m_pointsOfShape.at(row));
      
      m_path.append("M");
      m_path.append(QString("%1").arg(m_pointsOfShape.at(row).x()));
      m_path.append(" ");
      m_path.append(QString("%1").arg(m_pointsOfShape.at(row).y()));
       }
      for(int col = row + 1; col < m_noOfPoints ; col++){
	if(m_adjMatrix.at(row).at(col)){
	  viewPortPath.lineTo(m_pointsOfShape.at(col));
	
	  m_path.append("L");
	  m_path.append(QString("%1").arg(m_pointsOfShape.at(col).x()));
	  m_path.append(" ");
	  m_path.append(QString("%1").arg(m_pointsOfShape.at(col).y()));
	}
      }
    }
    m_path.append("Z");

    return viewPortPath;
}

QSizeF PresentationViewPortShape::size() const
{
    //return createShapePath().boundingRect().size();
    //return KoShape::size();
    return (outline().boundingRect().size());
}


void PresentationViewPortShape::paintComponent(QPainter &painter, const KoViewConverter &converter)
{
    Q_UNUSED(converter);
    Q_UNUSED(painter);    
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


