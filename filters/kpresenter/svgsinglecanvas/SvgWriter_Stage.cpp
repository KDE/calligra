#include "SvgWriter_Stage.h"
//#include "SvgWriter.h"
#include "SvgWriter_generic.h"
#include "SvgAnimationData.h"
#include "KoShape.h"

SvgWriter_Stage::SvgWriter_Stage(const QList<KoShapeLayer*> &layers, const QSizeF& pageSize): SvgWriter_generic(layers, pageSize)
{
  m_hasAppData = true;
  m_frames = new QTextStream(&m_appData, QIODevice::ReadWrite);
          
  saveScript();
  //addAppData(m_framesString);
  }


SvgWriter_Stage::SvgWriter_Stage(const QList< KoShape* >& toplevelShapes, const QSizeF& pageSize): SvgWriter_generic(toplevelShapes, pageSize)
{
  m_hasAppData = true;
  m_frames = new QTextStream(&m_appData, QIODevice::ReadWrite);
          
  saveScript();
  //addAppData(m_framesString);
}

SvgWriter_Stage::~SvgWriter_Stage()
{

}

void SvgWriter_Stage::saveAppData(KoShape *shape)
{
    forTesting(shape);// Adds a frame object to this shape

    SvgAnimationData * appData = dynamic_cast<SvgAnimationData*>(shape->applicationData());
    Frame *frameObj = new Frame();
    frameObj = &(appData->frame());
          
    saveFrame(frameObj);
    //return m_framesString;
}
      
      //This function will eventually be removed.
      //Only used for dummy data.
void SvgWriter_Stage::forTesting(KoShape * shape)
{
     //First save Frame properties to a shape. This is for testing only.
    SvgAnimationData  *obj = new SvgAnimationData();
    Frame *frame = new Frame(); //Default properties set 
        
    obj->addNewFrame(shape, frame);
}

void SvgWriter_Stage::saveFrame(Frame * frame)
{
    unsigned indent = 1;
        
    printIndentation(m_frames, indent++);
    *m_frames << "<calligra:frame" << endl;
       
     printIndentation(m_frames, indent);
     *m_frames << "calligra:" << "title=\"" << frame->title() << "\"" << endl;
     printIndentation(m_frames, indent);
     *m_frames << "calligra:" << "refid=\"" << frame->refId() << "\"" << endl;
     printIndentation(m_frames, indent);
     *m_frames << "calligra:" << "transition-profile=\"" << frame->transitionProfile() << "\"" << endl;
     printIndentation(m_frames, indent);
     *m_frames << "calligra:" << "hide=\"" << frame->isHide() << "\"" << endl;
     printIndentation(m_frames, indent);
     *m_frames << "calligra:" << "clip=\"" << frame->isClip() << "\"" << endl;
     printIndentation(m_frames, indent);
     *m_frames << "calligra:" << "timeout-enable=\"" << frame->isEnableTimeout() << "\"" << endl;
     printIndentation(m_frames, indent);
     *m_frames << "calligra:" << "sequence=\"" << frame->sequence() << "\"" << endl;
     printIndentation(m_frames, indent);
     *m_frames << "calligra:" << "transition-zoom-percent=\"" << frame->zoomPercent() << "\"" << endl;
     printIndentation(m_frames, indent);
     *m_frames << "calligra:" << "timeout-ms=\"" << frame->timeout() << "\"" << endl;
     printIndentation(m_frames, indent);
     *m_frames << "calligra:" << "transition-duration-ms=\"" << frame->transitionDuration() << "\"" << endl;
               
     *m_frames << "/>" << endl;
}

void SvgWriter_Stage::saveScript()
{
      m_script  = "Here comes the javascript.";
      
      *m_frames << "<script>" << m_script << "</script>" << endl;
}
