#define FrameId "Frame"

#include<QString>

class Frame {
  public:
    Frame();
   ~Frame();
   
   void setTitle(QString *title);
   void setRefId(QString *refId); //Data type for ref id of groups/shapes?
   void setSequence(int seq);
   void setZoomPercent(qreal zoomFactor);
   void setTransitionStyle(QString *transitionStyle); //-2,-1,0,1,2 ?
   void setTransitionDuration(qreal timeMs);
   
   QString* getTitle();
   QString* getRefId(); 
   int getSequence();
   qreal getZoomPercent();
   QString* getTransitionStyle(); 
   qreal getTransitionDuration();
   
private:
  QString *title;
  QString *refId;
  QString *transitionStyle;
  
  int sequence;
  
  qreal zoomFactor;
  qreal timeMs;
};