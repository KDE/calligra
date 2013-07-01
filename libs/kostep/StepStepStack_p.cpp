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

#include "StepStepStack_p.h"
#include "StepStepBase.h"
#include <QtCore/QFile>
#include <QtCore/QVariant>

StepStepStack_p::StepStepStack_p()
{

}

StepStepStack_p::~StepStepStack_p()
{

}

StepStepBase StepStepStack_p::at(int i)
{
    if(!stack.empty() && stack.count() >= i+1)
    {
        return stack.at(i);
    }
    else
    {
        return StepStepBase();
    }

}

StepStepBase StepStepStack_p::pop()
{
    //temporary behavior for the time being
    if(!stack.empty())
    {
        return stack.pop();
    }
    else
    {
        return StepStepBase();
    }

}

StepStepBase StepStepStack_p::top()
{
    if(!stack.empty())
    {
        return  stack.top();
    }
    else
    {
        return StepStepBase();
    }

}

void StepStepStack_p::push(StepStepBase step)
{
    stack.push(step);

}

void StepStepStack_p::serialize(QString Filename)
{
    QFile file(Filename);
    file.open(QIODevice::WriteOnly|QIODevice::Text);

    QString steps = "";
    foreach(StepStepBase ptr, stack)
    {
        steps += ptr.toXML();
    }
    file.write(steps.toLatin1());


}
void StepStepStack_p::deserialize(QString Filename)
{

}
void StepStepStack_p::insertAt(int i, StepStepBase step)
{
    //stub method for now will need to implement Operational Transformation Methods to
    //roll a change forward

}
void StepStepStack_p::removeAt(int i)
{
    //stub method for now will need to implement Operational Transformation Methods to
    //roll a change forward

}
int StepStepStack_p::rowcount()
{
    return stack.count();
}
QVariant StepStepStack_p::data(int i)
{
    if (!stack.empty() && stack.count() >= i+1)
    {
        //temporary measure
        StepStepBase step = stack.at(i);
        return step.toString();
    }
    else
    {
        return QVariant();
    }
}
