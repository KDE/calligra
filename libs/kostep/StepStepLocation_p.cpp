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


void StepStepLocationPrivate::constructor(QTextCursor cursor)
{
  QTextFrame* frame = cursor.currentFrame();
  ParentFrame(frame);
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
  QString returnValue = "s=";
  foreach(int ptr, location)
  {
    returnValue +="/" + ptr;
  }
  return returnValue;

}
int StepStepLocationPrivate::ParentFrame(QTextFrame* frame)
{
  QTextFrame* parentFrame = frame->parentFrame();
  if(parentFrame != parentFrame->document()->rootFrame())  {
    ParentFrame(frame->parentFrame());
  }
  else  {
    QTextFrame::iterator itr;
    int i=0;
    for(itr = parentFrame->begin(); itr != parentFrame->end(); itr++)
    {
      if(itr.currentFrame() == frame)
      {
	location.push(i);
	break;
      }
    }
  }
  //shouldn't happen
  return 0;
}

StepStepLocationPrivate* StepStepLocationPrivate::operator=(StepStepLocationPrivate location)
{
  this->location = location.location;
  return this;

}

