#include "PresentationViewPortShape.h"
#include <KoPathPoint.h>
#include <qpainter.h>


PresentationViewPortShape::PresentationViewPortShape()
{
  qDebug() << "PresentationViewPortShape created";
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
    applyConversion( painter, converter );
     
}

QPainterPath PresentationViewPortShape::createShapePath() const
{
    QPainterPath viewPortPath;
    
    viewPortPath.moveTo(0.0, 100.0);
    viewPortPath.lineTo(20.0, 100.0);
    viewPortPath.lineTo(0.0, 20.0);
    viewPortPath.moveTo(0.0, 20.0);
    viewPortPath.lineTo(20.0, 20.0);
    viewPortPath.moveTo(80.0, 20.0);
    viewPortPath.lineTo(100.0, 20.0);
    viewPortPath.moveTo(100.0, 20.0);
    viewPortPath.lineTo(100.0, 100.0);
    viewPortPath.moveTo(100.0, 100.0);
    viewPortPath.lineTo(80.0, 100.0);
    viewPortPath.closeSubpath();
  
    return viewPortPath;
}


void PresentationViewPortShape::paintComponent(QPainter &painter, const KoViewConverter &converter)
{
    Q_UNUSED(converter);
    
    QPainterPath viewPortpath;
    viewPortpath = createShapePath();
    
    painter.drawPath(viewPortpath);
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


