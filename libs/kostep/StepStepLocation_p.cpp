/*
    kostep -- handles changetracking using operational transformation for calligra
    Copyright (C) 2013  Luke Wolf <Lukewolf101010@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "StepStepLocation_p.h"
#include <QtGui/QTextBlock>
#include <QtGui/QTextDocument>
#include <QtCore/QQueue>

StepStepLocation_p::StepStepLocation_p(QObject *parent) :
    QObject(parent)
{
}

void StepStepLocation_p::constructor(QTextCursor cursor)
{
  QTextFrame* frame = cursor.currentFrame();
  getParentFrame(frame);
  QTextBlock Temp= cursor.block();
  QTextBlock* block = &Temp;
  int i=0;
  foreach(QObject* ptr, frame->children())
  {
    if(ptr == block)
    {
      location.push(i);
      break;
    }
    i++;
  }
  location.push(cursor.positionInBlock());

}
QTextCursor StepStepLocation_p::convertToQTextCursor()
{
  QList<int> locationq = location.toList();
  //temporary
  return QTextCursor();
}

QString StepStepLocation_p::ToString()
{
  QString returnValue = "s=";
  foreach(int ptr, location)
  {
    returnValue +="/" + ptr;
  }
  return returnValue;

}
int StepStepLocation_p::getParentFrame(QTextFrame* frame)
{
  QTextFrame* parentFrame = frame->parentFrame();
  if(parentFrame != parentFrame->document()->rootFrame())  {
    getParentFrame(frame->parentFrame());
  }
  else  {
    QList<QObject*> children = parentFrame->children();
    int i=0;
    foreach(QObject* ptr, children)
    {
      if(ptr == dynamic_cast<QObject*>(frame)){
	location.push(i);
	return i;
      }
      i++;
    }
  }
  //shouldn't happen
  return 0;
}

