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
}

SvgParser_Stage::~SvgParser_Stage()
{
}

void SvgParser_Stage::parseAppData(const KoXmlElement& e)
{
    //Frame * frame = new Frame();
    Frame * temp_frame = new Frame();
  
    if(e.hasAttribute("calligra:title")){
        temp_frame->setTitle("Parsed title");  
        }
    
    if(e.hasAttribute("calligra:refid")) {
      temp_frame->setRefId(e.attribute("calligra:refid"));
      }

    m_frameList.append(temp_frame);
   // temp_frame->~Frame();
}

void SvgParser_Stage::setAppDataTag()
{
    m_appData_tagName = "calligra:frame";
    m_hasAppData = true;
}

void SvgParser_Stage::createAppData()
{
    //Frame temp = *(m_frameList.at(0));
    
    QString refId;// = frame->refId();
    
    //const KoShapeGroup *noKey = new KoShapeGroup();
  
    foreach(KoShape *shape, m_shapes){
      foreach(Frame * frame, m_frameList){
        refId = frame->refId();
        if(shape->name() == refId){
         qDebug() << "refid: " << refId <<"matched with shape name: " << shape->name() << endl;
    
         SvgAnimationData * appData = new SvgAnimationData();
          appData->setFrame(frame);
          shape->setApplicationData(appData);
     //appData->~SvgAnimationData();
        //if(shape->applicationData() == 0)
         // qDebug() <<"Couldn't add app data to shape";
         }
      }
    }
}
