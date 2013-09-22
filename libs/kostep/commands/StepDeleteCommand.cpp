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

StepDeleteCommand::StepDeleteCommand(QTextCursor caret, StepStepStack &changeStack):StepCommand(caret, changeStack)
{
    if( caret.hasSelection ()) {
        handleHasSelection();
    } else {
        StepDeleteTextStep step;
        StepStepLocation location(caret);

        step.setLocation(location);
        changeStack.push(step);
    }

}

void StepDeleteCommand::handleHasSelection()
{
    QTextCursor documentTraverser(caret);
    QTextCursor start(caret);
    documentTraverser.setPosition(caret.selectionStart());
    while (documentTraverser.position() != caret.selectionEnd())
    {
        QTextBlock block = documentTraverser.block();
        documentTraverser.movePosition(QTextCursor::NextCharacter);
        QTextBlock block2 = documentTraverser.block();
        if(block != block2) {
            handleDeleteText(start, documentTraverser);
            start = documentTraverser;

            StepDeleteTextBlockStep step;
            StepStepLocation location(documentTraverser);
            step.setLocation(location);
            changeStack.push(step);
        }

    }
    handleDeleteText(start, documentTraverser);
}

void StepDeleteCommand::handleDeleteText(QTextCursor start, QTextCursor end)
{
    end.movePosition(QTextCursor::PreviousCharacter);
    StepDeleteTextStep step;
    StepStepLocation startLocation(start);
    StepStepLocation endLocation(end);
    step.setLocation(startLocation);
    step.setEndLocation(endLocation);
    changeStack.push(step);
}
