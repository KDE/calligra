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
#include <QtCore/QTextStream>
#include <QtCore/QVariant>
#include <QDir>

#include <QDebug>

StepStepStackPrivate::StepStepStackPrivate()
{
}

StepStepStackPrivate::~StepStepStackPrivate()
{
}

StepStepBase StepStepStackPrivate::at(int i)
{
    if(!stack.empty() && (stack.count() >= i+1)) {
        return stack.at(i);
    } else {
        return StepStepBase();
    }

}

const StepStepBase & StepStepStackPrivate::pop()
{
    //temporary behavior for the time being
    if(!stack.empty()) {
    StepStepBase* step = stack.pop();
        return *step;
    } else {
#if DEBUG
      qDebug("Stack popped when Empty");
#endif
        return new StepStepBase();
    }

}

StepStepBase StepStepStackPrivate::top()
{
    if(!stack.empty()) {
        return  stack.top();
    } else {
        return StepStepBase();
    }

}

void StepStepStackPrivate::push(StepStepBase & step)
{
#if DEBUG
    qDebug("Pushing onto actual stack");
#endif
    StepStepBase* ptr = &step;
    stack.push(ptr);
#if DEBUG
    qDebug("Pushed");
#endif
    //serialize(step, "changes.xml");

}

void StepStepStackPrivate::serialize(QString Filename)
{
    QFile file(Filename);
    file.open(QIODevice::WriteOnly|QIODevice::Text);

    QString steps = "";
    foreach(StepStepBase ptr, stack) {
        steps += ptr.toXML();
    }
    file.write(steps.toLatin1());
    file.close();
}

void StepStepStackPrivate::serialize (StepStepBase & step, QString Filename)
{
    QDir directory;
    QString Location = directory.homePath() +"/" + Filename;
    QFile file(Filename);
    file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text);
    QTextStream out(&file);
    out << step.toXML();
    out << endl;
    file.flush();
    file.close();
}

void StepStepStackPrivate::deserialize(QString Filename)
{

}
void StepStepStackPrivate::insertAt(int i, StepStepBase step)
{
    //stub method for now will need to implement Operational Transformation Methods to
    //roll a change forward

}
void StepStepStackPrivate::removeAt(int i)
{
    //stub method for now will need to implement Operational Transformation Methods to
    //roll a change forward

}
int StepStepStackPrivate::rowcount()
{
    return stack.count();
}
QVariant StepStepStackPrivate::data(int i)
{
    if (!stack.empty() && stack.count() >= i+1) {
        //temporary measure
        StepStepBase step = stack.at(i);
        return step.toString();
    } else {
        return QVariant();
    }
}
