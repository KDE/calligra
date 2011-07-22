#include "PresentationViewPortShape.h"
#include <KoPathPoint.h>
#include <qpainter.h>
#include <KoShapeBackground.h>


PresentationViewPortShape::PresentationViewPortShape()
{
 // qDebug() << "PresentationViewPortShape created";
  setShapeId(PresentationViewPortShapeId);
//TODO: Initialise a basic  [ ]
QPainterPath viewPortPath = createShapePath();

//setShapeId to PVPS id
}

PresentationViewPortShape::~PresentationViewPortShape()
{

}

QString PresentationViewPortShape::pathShapeId() const
{
    return PresentationViewPortShapeId;
}

void PresentationViewPortShape::paint(QPainter& painter, const KoViewConverter& converter)
{
    //QPainterPath viewPortpath;
  
     //viewPortpath = createShapePath();
    applyConversion( painter, converter );
    //viewPortpath.setFillRule(Qt::OddEvenFill);
    
    if(background()) {
      qDebug() << "background() == true"; 
      background()->paint(painter, outline());
    }

    painter.setPen(QPen(QColor(79, 106, 25), 1, Qt::SolidLine,
                     Qt::FlatCap, Qt::MiterJoin));
    painter.setBrush(QColor(122, 163, 39));
    
    painter.drawPath(createShapePath());
    }

QPainterPath PresentationViewPortShape::outline() const
{
    //return KoShape::outline();
    return createShapePath();
}

QPainterPath PresentationViewPortShape::createShapePath() const
{
    QPainterPath viewPortPath;
    qreal xCoOrdinate = 0.0;
    qreal yCoOrdinate = 0.0;
    
    qreal unit = 50.0;
    
    viewPortPath.setFillRule(Qt::OddEvenFill);
    
    viewPortPath.moveTo(xCoOrdinate, yCoOrdinate);
    xCoOrdinate += unit;
    
    viewPortPath.lineTo(xCoOrdinate, yCoOrdinate);
    
    viewPortPath.closeSubpath();
    xCoOrdinate = 0.0;
    yCoOrdinate = 0.0;
    
    yCoOrdinate += unit * 2;
    viewPortPath.lineTo(xCoOrdinate, yCoOrdinate);
    viewPortPath.moveTo(xCoOrdinate, yCoOrdinate);
    
    xCoOrdinate += unit;
    
    viewPortPath.lineTo(xCoOrdinate, yCoOrdinate);
    //25, 50
    
    xCoOrdinate += unit * 2;
    viewPortPath.moveTo(xCoOrdinate, yCoOrdinate);
    //75, 50
    
    xCoOrdinate += unit;
    viewPortPath.lineTo(xCoOrdinate, yCoOrdinate);
    //100 50
    viewPortPath.moveTo(xCoOrdinate, yCoOrdinate);
    //100 50
    yCoOrdinate -= unit * 2;
    viewPortPath.lineTo(xCoOrdinate, yCoOrdinate);
    //100 0
    viewPortPath.moveTo(xCoOrdinate, yCoOrdinate);
    //100 0
    xCoOrdinate -= unit;
    viewPortPath.lineTo(xCoOrdinate, yCoOrdinate);
    //75 0
    return viewPortPath;
}

QSizeF PresentationViewPortShape::size() const
{
    return outline().boundingRect().size();
}


void PresentationViewPortShape::paintComponent(QPainter &painter, const KoViewConverter &converter)
{
    Q_UNUSED(converter);
    Q_UNUSED(painter);
    
   }

void PresentationViewPortShape::update() const
{
    KoShape::update();
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


