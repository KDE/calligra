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
 * along with this library; see thehttp://www.cplusplus.com/doc/tutorial/typecasting/ file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "PresentationViewPortShape.h"

#include <qpainter.h>
#include <KoShapeBackground.h>
#include <KoShapeSavingContext.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <SvgLoadingContext.h>
#include <SvgUtil.h>
#include <SvgStyleWriter.h>
#include <SvgCustomSavingContext.h>

#include "PresentationViewPortShapeUtil.h"


const QString PresentationViewPortShape::title("title");
const QString PresentationViewPortShape::refid("refid");
const QString PresentationViewPortShape::transitionProfile("transition-profile");
const QString PresentationViewPortShape::transitionZoomPercent("transition-zoom-percent");
const QString PresentationViewPortShape::transitionDurationMs("transition-duration-ms");
const QString PresentationViewPortShape::timeoutEnable("timeout-enable");
const QString PresentationViewPortShape::timeoutMs("timeout-ms");
const QString PresentationViewPortShape::hide("hide");
const QString PresentationViewPortShape::clip("clip");
const QString PresentationViewPortShape::sequence("sequence");


PresentationViewPortShape::PresentationViewPortShape() : m_ns("sozi")
{
    setShapeId(PresentationViewPortShapeId);
    setName("ViewPort");//TODO remove this?
    initializeAnimationProperties();
    initializeTransitionProfiles();
}


PresentationViewPortShape::~PresentationViewPortShape()
{

}

QString PresentationViewPortShape::attribute(const QString& attrName)
{
    return m_animationAttributes[attrName];
}

bool PresentationViewPortShape::setAttribute(const QString& attrName, const QString& attrValue)
{
    if(m_animationAttributes.contains(attrName))
    {
      m_animationAttributes[attrName] = attrValue;
      return true;
    }
    else
    {
      qDebug() << "Attribute not found.";
      return false;
    }
}

void PresentationViewPortShape::parseAnimationProperties(const KoXmlElement& e)
{
    foreach(QString key, m_animationAttributes.keys()){
      if(e.hasAttribute(key))
      	m_animationAttributes.insert(key, "sozi:" + e.attribute(key));
	}
}

void PresentationViewPortShape::initializeTransitionProfiles()
{
  int count = -1;
    m_transitionProfiles.insert("linear", ++count);
    m_transitionProfiles.insert("accelerate", ++count);
    m_transitionProfiles.insert("strong-accelerate", ++count);
    m_transitionProfiles.insert("decelerate", ++count);
    m_transitionProfiles.insert("strong-decelerate", ++count);
    m_transitionProfiles.insert("accelerate-decelerate", ++count);
    m_transitionProfiles.insert("strong accelerate-decelerate", ++count);
    m_transitionProfiles.insert("decelerate-accelerate", ++count);
    m_transitionProfiles.insert("strong decelerate-accelerate", ++count);
}

int PresentationViewPortShape::transitionProfileIndex(const QString& profile)
{
    return m_transitionProfiles[profile];
}

void PresentationViewPortShape::initializeAnimationProperties()
{
    //Initializing the map with default values of all attributes
    m_animationAttributes.insert(PresentationViewPortShape::title, "No title");
    m_animationAttributes.insert(PresentationViewPortShape::refid, "null");
    m_animationAttributes.insert(PresentationViewPortShape::transitionProfile, "linear");
    m_animationAttributes.insert(PresentationViewPortShape::hide, "true");
    m_animationAttributes.insert(PresentationViewPortShape::clip, "false");
    m_animationAttributes.insert(PresentationViewPortShape::timeoutEnable, "false");
    m_animationAttributes.insert(PresentationViewPortShape::sequence, QString("%1").arg(0));
    m_animationAttributes.insert(PresentationViewPortShape::transitionZoomPercent, QString("%1").arg(1));
    m_animationAttributes.insert(PresentationViewPortShape::transitionDurationMs, QString("%1").arg(5000));
    m_animationAttributes.insert(PresentationViewPortShape::timeoutMs, QString("%1").arg(1000));
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
    applyConversion(painter, converter);
        
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


bool PresentationViewPortShape::saveSvg(SvgSavingContext &context)
{
    //setRefId(context.getID(this)); Redundant?
    if(!setAttribute("refid", context.getID(this)))
      return false;//??
    
    context.shapeWriter().startElement("rect");
    context.shapeWriter().addAttribute("sozi:viewport", "yes");
    context.shapeWriter().addAttribute("id", attribute("refid"));
    context.shapeWriter().addAttribute("transform", SvgUtil::transformToString(transformation()));

    context.shapeWriter().addAttribute("style", "fill:none");
    
    const QSizeF size = this->size();
    context.shapeWriter().addAttributePt("width", size.width());
    context.shapeWriter().addAttributePt("height", size.height());

    context.shapeWriter().endElement();
    
    //Save animation attributes
    saveAnimationAttributes(context);
    
    return true;
}

bool PresentationViewPortShape::saveAnimationAttributes(SvgSavingContext& savingContext)
{
    SvgCustomSavingContext *context = dynamic_cast<SvgCustomSavingContext*>(&savingContext);
    context->animationPropertiesWriter().startElement("sozi:frame");
    context->animationPropertiesWriter().addAttribute("sozi:title", m_animationAttributes[PresentationViewPortShape::title]);
    context->animationPropertiesWriter().addAttribute("sozi:refid", m_animationAttributes[PresentationViewPortShape::refid]);
    context->animationPropertiesWriter().addAttribute("sozi:transition-profile", m_animationAttributes[PresentationViewPortShape::transitionProfile]);
    context->animationPropertiesWriter().addAttribute("sozi:hide", m_animationAttributes[PresentationViewPortShape::hide]);
    context->animationPropertiesWriter().addAttribute("sozi:clip", m_animationAttributes[PresentationViewPortShape::clip]);
    context->animationPropertiesWriter().addAttribute("sozi:timeout-enable", m_animationAttributes[PresentationViewPortShape::timeoutEnable]);
    context->animationPropertiesWriter().addAttribute("sozi:timeout-ms", m_animationAttributes[PresentationViewPortShape::timeoutMs]);
    context->animationPropertiesWriter().addAttribute("sozi:transition-zoom-percent", m_animationAttributes[PresentationViewPortShape::transitionZoomPercent]);
    context->animationPropertiesWriter().addAttribute("sozi:transition-duration-ms", m_animationAttributes[PresentationViewPortShape::transitionDurationMs]);
    context->animationPropertiesWriter().addAttribute("sozi:sequence", m_animationAttributes[PresentationViewPortShape::sequence]);
    context->animationPropertiesWriter().endElement();
   
    return true;
}

bool PresentationViewPortShape::loadSvg(const KoXmlElement &element, SvgLoadingContext &context)
{
  
  if(element.hasAttribute("sozi:viewport") && (element.attribute("sozi:viewport") == "yes"))//FIXME:Is hasattribute check required at all?
  {  
  //qDebug() << "PVPShape found with rect id = " << element.attribute("id") << endl; 
    const qreal x = SvgUtil::parseUnitX(context.currentGC(), element.attribute("x"));
    const qreal y = SvgUtil::parseUnitY(context.currentGC(), element.attribute("y"));
    const qreal w = SvgUtil::parseUnitX(context.currentGC(), element.attribute("width"));
    const qreal h = SvgUtil::parseUnitY(context.currentGC(), element.attribute("height"));
    
    setSize(QSizeF(w, h));
    setPosition(QPointF(x, y));
    if (w == 0.0 || h == 0.0)
        setVisible(false);

    return true;
  }
  else if(element.localName() == "sozi:frame"){
    PresentationViewPortShape *shape = dynamic_cast<PresentationViewPortShape*>(context.shapeById(element.attribute("id")));
    shape->parseAnimationProperties(element);
    return false;   
  }
  else
    return false;
}
