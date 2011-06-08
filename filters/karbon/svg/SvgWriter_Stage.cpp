#include "SvgWriter_Stage.h"
#include "SvgWriter_generic.h"
#include "SvgAnimationData.h"
#include "KoShape.h"

SvgWriter_Stage::SvgWriter_Stage(const QList<KoShapeLayer*> &layers, const QSizeF& pageSize): SvgWriter_generic(layers, pageSize)
{
   QString frames;
   m_frames = new QTextStream(&frames, QIODevice::ReadWrite);
          
  saveScript();
SvgWriter_generic::addAppData(frames);
  
}


SvgWriter_Stage::SvgWriter_Stage(const QList< KoShape* >& toplevelShapes, const QSizeF& pageSize): SvgWriter_generic(toplevelShapes, pageSize)
{
  
   QString frames;
   m_frames = new QTextStream(&frames, QIODevice::ReadWrite);
          
  saveScript();
SvgWriter_generic::addAppData(frames);
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
          
      }
      
      //This function will eventually be removed.
      //Only used for dummy data.
      void SvgWriter_Stage::forTesting(KoShape * shape)
      {
        
      //First save Frame properties to a shape. This is for testing only.
        SvgAnimationData  *obj = new SvgAnimationData();
        Frame *frame = new Frame(); //Default properties set 
        /*QString refId = getID(shape);
        refId.chop(1);
        refId.remove(0, 5);
        
        frame->setRefId(refId);
        */
        obj->addNewFrame(shape, frame);
        
        }

      void SvgWriter_Stage::saveFrame(Frame * frame)
      {
       
        *m_frames << "<desc>" << "Frame info." << "</desc>" << endl;
       // SvgWriter.printIndentation(m_frames, ++m_indent);
        *m_frames << "<calligra:frame" << endl;
       
        //SvgWriter.printIndentation(m_frames, ++m_indent);
        *m_frames << "calligra:" << "title=\"" << frame->title() << "\"" << endl;
        //SvgWriter.printIndentation(m_frames, m_indent);
        *m_frames << "calligra:" << "refid=\"" << frame->refId() << "\"" << endl;
        //SvgWriter.printIndentation(m_frames, m_indent);
        *m_frames << "calligra:" << "transition-profile=\"" << frame->transitionProfile() << "\"" << endl;
        //SvgWriter.printIndentation(m_frames, m_indent);
        *m_frames << "calligra:" << "hide=\"" << frame->isHide() << "\"" << endl;
        //SvgWriter.printIndentation(m_frames, m_indent);
        *m_frames << "calligra:" << "clip=\"" << frame->isClip() << "\"" << endl;
        //SvgWriter.printIndentation(m_frames, m_indent);
        *m_frames << "calligra:" << "timeout-enable=\"" << frame->isEnableTimeout() << "\"" << endl;
        //SvgWriter.printIndentation(m_frames, m_indent);
        *m_frames << "calligra:" << "sequence=\"" << frame->sequence() << "\"" << endl;
        //SvgWriter.printIndentation(m_frames, m_indent);
        *m_frames << "calligra:" << "transition-zoom-percent=\"" << frame->zoomPercent() << "\"" << endl;
        //SvgWriter.printIndentation(m_frames, m_indent);
        *m_frames << "calligra:" << "timeout-ms=\"" << frame->timeout() << "\"" << endl;
        //SvgWriter.printIndentation(m_frames, m_indent);
        *m_frames << "calligra:" << "transition-duration-ms=\"" << frame->transitionDuration() << "\"" << endl;
        
        
        *m_frames << "/>" << endl;
      }

 void SvgWriter_Stage::saveScript()
      {
      m_script  = "Here comes the javascript.";
      
//      SvgWriter.printIndentation(m_body, m_indent--);
      
      *m_frames << "<script>" << m_script << "</script>" << endl;
      }
     

     
    

      