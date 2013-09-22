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
#include "StepDeleteCommand.h"
#include "../StepDeleteSteps.h"

StepDeleteCommand::StepDeleteCommand(QTextCursor caret, StepStepStack *changeStack):StepCommand(caret, changeStack)
{
    if( caret.hasSelection ()) {
        handleHasSelection();
    } else {
        QTextCursor documentTraverser(caret);
        QTextBlock block = documentTraverser.block();
        documentTraverser.movePosition(QTextCursor::NextCharacter);
        QTextBlock block2 = documentTraverser.block();

        if (block == block2) {
            handleDeleteText(caret,caret);
        } else {
            StepDeleteTextStep step;
            StepStepLocation location(caret);
            step.setLocation(location);
            changeStack->push(step);
        }
    }
    changeStack =0;

}

void StepDeleteCommand::handleHasSelection()
{
    QTextCursor documentTraverser(caret);
    QTextCursor start(caret);
    start.setPosition(caret.selectionStart());
    documentTraverser.setPosition(caret.selectionStart());

    //counter for the number of blocks
    int count =0;

    while (documentTraverser.position() != caret.selectionEnd())
    {
        QTextBlock block = documentTraverser.block();
        documentTraverser.movePosition(QTextCursor::NextCharacter);
        QTextBlock block2 = documentTraverser.block();
        if(block != block2) {
            QTextCursor endPosition(documentTraverser);
            endPosition.movePosition(QTextCursor::PreviousCharacter);
            handleDeleteText(start, endPosition);

            start = documentTraverser;
            count++;
        }

    }
    handleDeleteText(start, documentTraverser);
    while (count !=0)
    {
        count--;
        StepDeleteTextBlockStep step;
        QTextCursor startOfCursor(caret);
        startOfCursor.setPosition(caret.selectionStart());
        StepStepLocation location(startOfCursor);
        step.setLocation(location);
        changeStack->push(step);
    }
}

void StepDeleteCommand::handleDeleteText(QTextCursor start, QTextCursor end)
{

    StepDeleteTextStep step;
    StepStepLocation startLocation(start);
    StepStepLocation endLocation(end);
    step.setLocation(startLocation);
    step.setEndLocation(endLocation);
    changeStack->push(step);
}
