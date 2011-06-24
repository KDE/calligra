#include "SvgParser_Stage.h"
#include "SvgAnimationData.h"
#include <KoShape.h>
#include <KoShapeGroup.h>

SvgParser_Stage::SvgParser_Stage(KoResourceManager* documentResourceManager):SvgParser(documentResourceManager)
{
    setAppDataTag();
    NS = new QString("calligra:");
    //m_frame = new Frame();
    m_frameList.begin();
    m_frame = new Frame();
    
    m_attributes = m_frame->attributes();
}

SvgParser_Stage::~SvgParser_Stage()
{
}

void SvgParser_Stage::parseAppData(const KoXmlElement& e)
{
   Frame *frame = new Frame(e);
   m_frameList.append(m_frame);
   }

void SvgParser_Stage::setAppDataTag()
{
    m_appData_tagName = "calligra:frame";
    m_hasAppData = true;
}

void SvgParser_Stage::createAppData()
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
