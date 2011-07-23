#include "PresentationViewPortShape.h"
#include <KoPathPoint.h>
#include <qpainter.h>
#include <KoShapeBackground.h>


PresentationViewPortShape::PresentationViewPortShape()
{
 setShapeId(PresentationViewPortShapeId);
//TODO: Initialise a basic  [ ]

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
    applyConversion( painter, converter );
        
    painter.setPen(QPen(QColor(Qt::black), 1, Qt::DashLine,
                     Qt::FlatCap, Qt::MiterJoin)); 
    painter.setBrush(QColor(122, 163, 39));// Needed?
    
    painter.drawPath(createShapePath(outline().boundingRect().size()));
    }

QPainterPath PresentationViewPortShape::outline() const
{
    return KoShape::outline();
    }

//TODO re-factor code
//make a function with specific points to be joined to make this path
//check if QPainterPath has it
//TODO Remove shearing
//TODO remove other functions not applicable to this shape
QPainterPath PresentationViewPortShape::createShapePath(const QSizeF& size) const
{
    QPainterPath viewPortPath;
    qreal xCoOrdinate = 0.0;
    qreal yCoOrdinate = 0.0;
    
    qreal unit = 15.0; //TODO change according to 'size' and keep a minimum value
    qreal heightUnit = size.height();
    
    viewPortPath.moveTo(xCoOrdinate, yCoOrdinate);
    xCoOrdinate += unit;
    
    viewPortPath.lineTo(xCoOrdinate, yCoOrdinate);
    
    viewPortPath.closeSubpath();
    xCoOrdinate = 0.0;
    yCoOrdinate = 0.0;
    
    yCoOrdinate += heightUnit;
    viewPortPath.lineTo(xCoOrdinate, yCoOrdinate);
    viewPortPath.moveTo(xCoOrdinate, yCoOrdinate);
    
    xCoOrdinate += unit;
    
    viewPortPath.lineTo(xCoOrdinate, yCoOrdinate); // Left bracket complete
        
    xCoOrdinate += (size.width() - (2 * unit)); //Leave space between the 2 brackets
    viewPortPath.moveTo(xCoOrdinate, yCoOrdinate);
        
    xCoOrdinate += unit;
    viewPortPath.lineTo(xCoOrdinate, yCoOrdinate);
    
    viewPortPath.moveTo(xCoOrdinate, yCoOrdinate);
    
    yCoOrdinate -= heightUnit;
    viewPortPath.lineTo(xCoOrdinate, yCoOrdinate);
    
    viewPortPath.moveTo(xCoOrdinate, yCoOrdinate);
    
    xCoOrdinate -= unit;
    viewPortPath.lineTo(xCoOrdinate, yCoOrdinate);
    
    return viewPortPath;
}

QSizeF PresentationViewPortShape::size() const
{
    //return createShapePath().boundingRect().size();
    return KoShape::size();
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


