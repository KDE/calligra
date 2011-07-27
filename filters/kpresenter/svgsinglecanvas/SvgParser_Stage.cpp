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

#include "SvgParser_Stage.h"

#include <KoShape.h>
#include <KoShapeGroup.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>

#include "SvgAnimationData.h"
#include "plugins/presentationviewportshape/PresentationViewPortShape.h"

SvgParser_Stage::SvgParser_Stage(KoResourceManager* documentResourceManager):SvgParser_generic(documentResourceManager)
{
 
    m_appData_tagName = "calligra:frame";
    m_hasAppData = true;
   // m_appData_elementName = PresentationViewPortShapeId;
    
    m_frameList.begin();
    }

SvgParser_Stage::~SvgParser_Stage()
{
}

PresentationViewPortShape* SvgParser_Stage::parseAppData(const KoXmlElement& e, QList< KoShape* > shapes)
{
   Frame *frame = new Frame(e);
   //m_frameList.append(frame);
   return setAppData(frame, shapes);
   }

void SvgParser_Stage::setAppData()
{

}

KoShape* SvgParser_Stage::removeShape()
{
    return temp;
}

PresentationViewPortShape* SvgParser_Stage::setAppData(Frame* frame, QList<KoShape*>& shapeList)
{
  PresentationViewPortShape* temp;
    foreach(KoShape *shape, m_shapes)
    {
      if(shape->name() == frame->refId())
      {
	
	/*SvgAnimationData * appData = new SvgAnimationData();
       
           appData->setFrame(frame);
           shape->setApplicationData(appData);
      */
      
      PresentationViewPortShape* pvpShape = createPVPShape(shape, frame);
      
//      pvpShape->setShapeId(PresentationViewPortShapeId);
      
      //shapeList.append(pvpShape);
    qDebug() << "Removed old shape: " << m_shapes.removeOne(shape);//true
    //TODO shape not removed still
      //temp = shape;
      return pvpShape;
    }
    }
    
    return temp;
} 

PresentationViewPortShape* SvgParser_Stage::createPVPShape(KoShape* shape, Frame * frame)
{
  QString shapeID = PresentationViewPortShapeId;
    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->get(shapeID);
    if (! factory) {
        kWarning(30514) << "Could not find factory for shape id" << shapeID;
        return 0;
    }

    PresentationViewPortShape *pvpShape = static_cast<PresentationViewPortShape*>(factory->createDefaultShape(m_documentResourceManager));
    if (pvpShape){
      qDebug() << "New PVPShape created.";
    }
         pvpShape->setShapeId(PresentationViewPortShapeId);
	 pvpShape->setTransformation(shape->transformation());
	 pvpShape->setBorder(shape->border());
	 pvpShape->setBackground(shape->background());
    
	 SvgAnimationData * appData = new SvgAnimationData();
       
           appData->setFrame(frame);
           pvpShape->setApplicationData(appData);
	   
      
	 return pvpShape;

}
/*
void SvgParser_Stage::setAppData()
{
    
  foreach(KoShape *shape, m_shapes){
      foreach(Frame * frame, m_frameList){
          
        if(shape->name() == frame->refId()){
           SvgAnimationData * appData = new SvgAnimationData();
       
           appData->setFrame(frame);
           shape->setApplicationData(appData);
         }
      }
    }
    
}

/*KoShape* SvgParser_Stage::createAppData(const KoXmlElement& e)
{
  QString shapeID = PresentationViewPortShapeId;
  
  KoShapeFactoryBase *factory = KoShapeRegistry::instance()->get(shapeID);
    if (! factory) {
        kWarning(30514) << "Could not find factory for shape id" << shapeID;
        return 0;
    }

    KoShape *shape = factory->createDefaultShape(m_documentResourceManager);
    if (shape && shape->shapeId().isEmpty())
        shape->setShapeId(factory->id());

    
     // reset tranformation that might come from the default shape
    shape->setTransformation(QTransform());

    // reset border
    KoShapeBorderModel *oldBorder = shape->border();
    shape->setBorder(0);
    delete oldBorder;

    // reset fill
    KoShapeBackground *oldFill = shape->background();
    shape->setBackground(0);
    delete oldFill;

    //creates the basic [ ] shape
    PresentationViewPortShape* PVPshape = static_cast<PresentationViewPortShape*> (shape);
    
    //sets all the properties to draw the shape
    PVPshape->setName(e.attribute("id"));
   
    //TODO FIll?
    return PVPshape;
}*/
    