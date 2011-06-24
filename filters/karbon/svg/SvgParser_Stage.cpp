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
    /*foreach(QString attribute, m_attributes){
      QString attr = *NS;
      if(e.hasAttribute(attr.append(attribute))){
        m_frame->setTitle(e.attribute(attr));  
        }
    */
    //static int s = 0;    
    if(e.hasAttribute("calligra:title")) {
      m_frame->setTitle(e.attribute("calligra:title"));
      }

    if(e.hasAttribute("calligra:refid")) {
      m_frame->setRefId(e.attribute("calligra:refid"));
      }

    if(e.hasAttribute("calligra:clip")) {
      m_frame->setClip((e.attribute("calligra:clip") == "true"));
      }

    if(e.hasAttribute("calligra:hide")) {
      m_frame->setHide((e.attribute("calligra:hide") == "true"));
      }

    if(e.hasAttribute("calligra:sequence")) {
      m_frame->setSequence(e.attribute("calligra:sequence").toInt());
      //m_frame->setSequence(++s);
      }
     
     if(e.hasAttribute("calligra:timeout-ms")) {
      m_frame->setTimeout(e.attribute("calligra:timeout-ms").toInt());
      }
    
    if(e.hasAttribute("calligra:timeout-enable")) {
      m_frame->enableTimeout(e.attribute("calligra:timeout-enable").toInt());
      }
    
    if(e.hasAttribute("calligra:transition-profile")) {
      m_frame->setTransitionProfile(e.attribute("calligra:transition-profile"));
      }
    
    if(e.hasAttribute("calligra:transition-duration-ms")) {
      m_frame->setTransitionDuration(e.attribute("calligra:transition-duration-ms").toInt());
      }
      
    if(e.hasAttribute("calligra:transition-zoom-percent")) {
      m_frame->setZoomPercent(e.attribute("calligra:transition-zoom-percent").toInt());
      }
    
    m_frameList.append(m_frame);
    
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
