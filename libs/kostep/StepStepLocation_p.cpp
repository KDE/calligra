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
#include <QDebug>

StepStepLocationPrivate::StepStepLocationPrivate()
{
}

StepStepLocationPrivate::~StepStepLocationPrivate ()
{

}

void StepStepLocationPrivate::constructor(const QTextCursor &cursor)
{
    QTextFrame *frame = cursor.currentFrame();
    QTextFrame *rootFrame = cursor.document()->rootFrame();

    parentFrame(frame, rootFrame);

    qDebug() << "After Frames: \n" << toString() << "\n-----------------";
    int i=0;

    QTextFrame::iterator itr;
    for (itr = cursor.currentFrame()->begin(); itr != cursor.currentFrame()->end(); itr++) {
        if (cursor.block() == itr.currentBlock()) {
            location.push(i);
            if (cursor.currentTable()) {
                //QTextTable table = cursor.currentTable();
                //QTextTable::iterator itr;

                //cursor.movePosition();

            }
            if(cursor.currentList()) {
            }
            qDebug() << "After Block: \n"<< toString() << "\n-----------------";
            break;
        }
        i++;
    }
    location.push(cursor.positionInBlock());
    qDebug() << "After Position" << toString() << "\n----------------- \n\n";
}

QTextCursor StepStepLocationPrivate::convertToQTextCursor(QTextDocument *document)
{
    //flip the stack
    QStack<int> stack1 = location;
    QStack<int> stack2;
    for(int i=0; i<location.count(); i++) {
      stack2.push(stack1.pop());
    }
    QTextFrame::iterator itr= document->rootFrame()->begin();

    while(!stack2.isEmpty()) {
        int i = stack2.pop();
        for(int x=0; x<=i; x++) {
            itr++;
        }

        if(stack2.isEmpty()) {
          QTextCursor textCursor(itr.currentBlock());
          return textCursor;
        } else {
            itr = itr.currentFrame()->begin();
        }
    }

    //temporary
    return QTextCursor();
}

QString StepStepLocationPrivate::toString()
{
    QString returnValue = "s=\"";
    QVector<int>::const_iterator itr;
    for (itr = location.constBegin(); itr != location.constEnd(); itr++) {
        returnValue +="/" + *itr;
    }

    returnValue+="\"";
    return returnValue;
}

void StepStepLocationPrivate::parentFrame(QTextFrame *frame, QTextFrame *rootFrame)
{
    QStack<QTextFrame*> frameStack;

    if(frame != rootFrame) {
        frameStack.push(frame);
    } else {
        return;
    }

    QTextFrame *frame2 = frame;
    while (frame2->parentFrame() != 0) {
        frame2 = frame2->parentFrame();
        frameStack.push(frame2);
    }

    QTextFrame::iterator itr;
    int i =0;
    for (itr = rootFrame->begin(); itr != rootFrame->end(); itr++) {
        if (itr.currentFrame() == frame2) {
            qDebug() << "i = "<< i;
            location.push(i);
            frameStack.pop();
            break;
        }
        i++;
    }

    while (!frameStack.isEmpty()) {
        i=0;
        for ( itr= frame2->begin(); itr != frame2->end(); itr++) {
            if(itr.currentFrame() == frameStack.top()) {
                qDebug() << "i = "<< i;
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
    return;
}

StepStepLocationPrivate* StepStepLocationPrivate::operator=(const StepStepLocationPrivate &other)
{
    location = other.location;
    return this;

}
