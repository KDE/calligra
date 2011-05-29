#include "Frame.h"

Frame::Frame() {
}

Frame::~Frame() {
   }
   
   void Frame::setTitle(QString *title) {
     this->title = title;
   }
   void Frame::setRefId(QString *refId){
     this->refId = refId;
   }
   void Frame::setSequence(int sequence) {
     this->sequence = sequence;
   }
   void Frame::setZoomPercent(qreal zoomFactor) {
     this->zoomFactor = zoomFactor;
   }
   void Frame::setTransitionStyle(QString *transitionStyle) {
     this->transitionStyle = transitionStyle;
   }
   void Frame::setTransitionDuration(qreal timeMs) {
     this->timeMs = timeMs;
   }
   
   QString* Frame::getTitle() {
     return title;
   }
   QString* Frame::getRefId() {
     return refId;
   }
   int Frame::getSequence() {
     return sequence;
   }
   qreal Frame::getZoomPercent() {
     return zoomFactor;
     
  }
   QString* Frame::getTransitionStyle() {
     return transitionStyle;
   }
   qreal Frame::getTransitionDuration() {
     return timeMs;
   }
  