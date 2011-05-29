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
  