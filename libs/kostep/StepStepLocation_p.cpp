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

//Note: All locations start with 1 as opposed to 0 due to the XML standard

StepStepLocationPrivate::StepStepLocationPrivate()
{
}

StepStepLocationPrivate::~StepStepLocationPrivate ()
{

}

void StepStepLocationPrivate::constructor(const QTextCursor &cursor)
{
    //Gets the cursor's current frame and the root frame of the document
    //and passes those to parentFrame() to push the frames of the current
    //position of the cursor in the document onto the location stack
    QTextFrame *frame = cursor.currentFrame();
    QTextFrame *rootFrame = cursor.document()->rootFrame();
    parentFrame(frame, rootFrame);

#if DEBUG
    qDebug() << "After Frames: \n" << toString() << "\n-----------------";
#endif DEBUG

    //Finds the position of the current block inside of the cursors current frame, and then
    //pushes that onto the location stack.
    int i=1;
    QTextFrame::iterator itr;
    for (itr = cursor.currentFrame()->begin(); itr != cursor.currentFrame()->end(); itr++) {
        if (cursor.block() == itr.currentBlock()) {
            location.push(i);
#if DEBUG
            qDebug() << "After Block: \n"<< toString() << "\n-----------------";
#endif DEBUG
            break;
        }
        i++;
    }

    location.push(cursor.positionInBlock()+1);
#if DEBUG
    qDebug() << "After Position: \n" << toString() << "\n----------------- \n\n";
#endif DEBUG
}

QTextCursor StepStepLocationPrivate::convertToQTextCursor(QTextDocument *document)
{
    //create a temporary stack, and pop off the top number as that is the position
    //inside of the block, and then flip the stack
    QStack<int> stack1 = location;
    int position = stack1.pop();
    QStack<int> stack2;
    for(int i=0; i<location.count(); i++) {
      stack2.push(stack1.pop());
    }

    QTextFrame::iterator itr= document->rootFrame()->begin();

    while(!stack2.isEmpty()) {
        //Finds the Frame that is on the top of the flipped stack
        int currentFrameNumber = stack2.pop();
        for(int i=1; i<=currentFrameNumber; i++) {
            itr++;
        }

        //Checks whether the stack is empty, if it is it creates a
        //QTextCursor and moves it to the position inside of the block
        //and then returns it. if not it set the iterator to the
        //the children of the frame we got from the above for loop and
        //the while loop starts again
        if(stack2.isEmpty()) {
            QTextCursor textCursor(itr.currentBlock());
            textCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, position);
            return textCursor;
        } else {
            itr = itr.currentFrame()->begin();
        }
    }

    //This Should not happen
    return QTextCursor();
}

QString StepStepLocationPrivate::toString()
{
    //create a temporary stack and flip it so that
    QStack <int> tempStack = location;
    QStack <int> tempStack2;
    while(!tempStack.isEmpty()) {
        tempStack2.push(tempStack.pop());
    }

    //The format of the location of a step is
    //s="/x/y/z" it can be as short as /x for inserting a block,
    //and /x/y for a text fragment and can go on infinitely.
    //Note: QString::number is required to be wrapped around the int
    //in order for this to not put out garbage.
    QString returnValue = "s=\"";
    while (!tempStack2.isEmpty()) {
        returnValue +="/" + QString::number(tempStack2.pop());
    }

    returnValue+="\"";
    return returnValue;
}

void StepStepLocationPrivate::parentFrame(QTextFrame *frame, QTextFrame *rootFrame)
{
    QStack<QTextFrame*> frameStack;

    //Checks whether the frame is the root frame, if it is then we can just return
    //if not we have to find out which frame it is and so put it on the framestack
    if(frame != rootFrame) {
        frameStack.push(frame);
    } else {
        return;
    }

    //pushes QTextFrames onto the frameStack until it reaches the root frame
    //which returns a 0 when parentFrame is called()
    QTextFrame *frame2 = frame;
    while (frame2->parentFrame() != 0) {
        frame2 = frame2->parentFrame();
        frameStack.push(frame2);
    }

    //Finds the first QTextFrame in frame's ownership hierarchy under the root frame
    //this needs to be separate because we need rootFrame's beginning and ending
    //iterator points.  When we find out what the position of the QTextFrame we push
    //that onto the location stack.
    QTextFrame::iterator itr;
    int i =1;
    for (itr = rootFrame->begin(); itr != rootFrame->end(); itr++) {
        if (itr.currentFrame() == frame2) {
            qDebug() << "i = "<< i;
            location.push(i);
            frameStack.pop();
            break;
        }
        i++;
    }

    //Finds the location of the rest of the QTextFrames and pushes them onto the location stack
    while (!frameStack.isEmpty()) {
        i=1;
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

    return;
}

StepStepLocationPrivate* StepStepLocationPrivate::operator=(const StepStepLocationPrivate &other)
{
    location = other.location;
    return this;
}
