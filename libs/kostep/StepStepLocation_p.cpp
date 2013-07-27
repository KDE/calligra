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
#include <QtGui/QTextCursor>
#include <QTextTable>
#include <QtCore/QQueue>

StepStepLocationPrivate::StepStepLocationPrivate()
{
}

StepStepLocationPrivate::~StepStepLocationPrivate ()
{

}


void StepStepLocationPrivate::constructor(QTextCursor & cursor)
{
  QTextFrame* frame = cursor.currentFrame();
  QTextFrame* rootFrame = cursor.document()->rootFrame();

  ParentFrame(frame, rootFrame);

  QTextBlock Temp= cursor.block();
  QTextBlock* block = &Temp;

  int i=0;

  QTextFrame::iterator itr;
  for(itr = cursor.currentFrame()->begin(); itr != cursor.currentFrame()->end(); itr++)
  {
    if (cursor.block() == itr.currentBlock())
    {
      location.push(i);
      if (cursor.currentTable())
      {
	//QTextTable table = cursor.currentTable();
	//QTextTable::iterator t_itr;

	//cursor.movePosition();

      }
      if(cursor.currentList())
      {
      }
      break;
    }
    i++;
  }
  location.push(cursor.positionInBlock());
}
QTextCursor StepStepLocationPrivate::convertToQTextCursor(QTextDocument* ptr)
{
  //flip the stack
  QStack<int> Stack1 = location;
  QStack<int> Stack2;
  for(int i=0; i<location.count(); i++)
  {
    Stack2.push(Stack1.pop());
  }
  QTextFrame::iterator itr= ptr->rootFrame()->begin();

  while(!Stack2.isEmpty())
  {
    int i = Stack2.pop();
    for(int x=0; x<=i; x++)
    {
      itr++;
    }

    if(Stack2.isEmpty())
    {
      QTextCursor textCursor(itr.currentBlock());
      return textCursor;
    }
    else
    {
      itr = itr.currentFrame()->begin();
    }
  }

  //temporary
  return QTextCursor();
}

QString StepStepLocationPrivate::ToString()
{
  QString returnValue = "s=\"";
  qDebug("2.5.1");
  foreach(int ptr, location)
  {
    returnValue +="/" + ptr;
  }
  qDebug("2.5.2");
  returnValue+="\"";
  qDebug("2.5.3");
  return returnValue;

}
int StepStepLocationPrivate::ParentFrame(QTextFrame* frame, QTextFrame* rootFrame)
{
  QStack<QTextFrame*> frameStack;

  if(frame != rootFrame)
  {
    frameStack.push(frame);
  }
  else
  {
    location.push(0);
    return 0;
  }

  QTextFrame* frame2 = frame;
  while (frame2->parentFrame() != 0)
  {
    frame2 = frame2->parentFrame();
    frameStack.push(frame2);
  }



  QTextFrame::iterator itr;
  int i =0;
  for(itr = rootFrame->begin(); itr != rootFrame->end(); itr++)
  {
    if(itr.currentFrame() == frame2)
    {
      qDebug("Found first frame");
      location.push(i);
      frameStack.pop();
      break;
    }
    i++;
  }


  while(!frameStack.isEmpty())
  {
    i=0;
    for( itr= frame2->begin(); itr != frame2->end(); itr++)
    {
      QTextFrame* ptr = frameStack.top();
      if(itr.currentFrame() == ptr)
      {
	qDebug("found frame");
	location.push(i);
	frame2 = frameStack.pop();
	break;
      }
      i++;
    }
  }

  /*
  QTextFrame* parentFrame = (frame->parentFrame()!=0)? frame: frame->parentFrame();
  if(frame->parentFrame()!= 0)  {
    qDebug(".5.1");
    ParentFrame(frame->parentFrame());
  }
  else  {
    qDebug(".5.2");
    QTextFrame::iterator itr;
    int i=0;
    for(itr = parentFrame->begin(); itr != parentFrame->end(); itr++)
    {
      if(itr.currentFrame() == frame)
      {
	location.push(i);
	return i;
      }
    }
  }*/
  //shouldn't happen
  return 0;
}

StepStepLocationPrivate* StepStepLocationPrivate::operator=(StepStepLocationPrivate location)
{
  this->location = location.location;
  return this;

}

