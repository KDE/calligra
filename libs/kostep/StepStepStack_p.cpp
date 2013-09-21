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
#include "StepStepLocation.h"

#include <QDebug>
#include "StepSteps.h"

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

void StepStepStackPrivate::serialize (StepStepBase & step, QString filename)
{
    QDir directory;
    QString location = directory.homePath() +"/" + filename;
    QFile file(filename);
    file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text);
    QTextStream out(&file);
    out << step.toXML();
    out << endl;
    file.flush();
    file.close();
}

void StepStepStackPrivate::deserialize (QString text)
{
    //remove all whitespace
    text = text.remove("\n");
    text = text.simplified();

    //split the steps based upon the opening of an XML tag
    QStringList stepList = text.split("<");

    //we're going to remove each tag from the list as we go through
    //the deserialization process
    while (!stepList.empty()) {
        //Grab the first tag off of the list and remove it from the list
        QString stepText = stepList.first();
        stepList.removeFirst();

        //if the tag has text inbetween it grab it and
        // remove the end of the XML tag
        QString stepContents;
        if(stepText.indexOf(">") < stepText.length()) {
            stepContents = stepText.mid(stepText.indexOf(">")+1);
        }
        stepText.remove(">");

        //split the tag into it's components based upon spaces
        QStringList stepTextList = stepText.split(" ");

        //Add steps
        if (stepTextList.at(0).toLower() == "add") {
            //Add text steps
            if (stepTextList.at(1).toLower() == "type=\"text\"") {
                //Example Add Text Step
                //<add type="text" s="/1/2" > dogs </add>
                //  0       1          2

                //Create the Add Text Step
                StepAddTextStep *step = new StepAddTextStep(stepContents);
                StepStepLocation stepLocation;
                stepLocation.fromString(stepTextList.at(2));
                step->setLocation(stepLocation);
                stack.push(step);
                step = 0;

                //deletes the closing tag
                stepList.removeFirst();
            }

            //add text paragraph (block) steps
            if (stepTextList.at(1).toLower() == "type=\"paragraph\"") {
                //Example Add Text Block Step
                //<add type="paragraph" s="/1/2" />
                //  0        1            2

                StepAddTextBlockStep *step = new StepAddTextBlockStep();
                StepStepLocation stepLocation;
                stepLocation.fromString(stepTextList.at(2));
                step->setLocation(stepLocation);
                stack.push(step);
                step =0;
            }
                  //delete steps
        } else if (stepTextList.at(0).toLower() == "del") {

            if (stepTextList.at(1).toLower() == "type=\"text\"") {
                //Example Delete Text Step
                //<del type="text" s="/1/2" s="/1/3"/>
                //  0        1         2        3

                StepDeleteTextStep *step = new StepDeleteTextStep();
                StepStepLocation stepLocation;
                stepLocation.fromString(stepTextList.at(2));
                step->setLocation(stepLocation);
                StepStepLocation endLocation;
                endLocation.fromString(stepTextList.at(3));
                step->setEndLocation(endLocation);
                stack.push(step);
                step = 0;
            }

            if (stepTextList.at(1).toLower() == "type=\"paragraph\"") {
                //Example Delete Text Block Step
                //<del type="paragraph" s="/1/2" />
                //  0        1            2

                StepDeleteTextBlockStep *step = new StepDeleteTextBlockStep();
                StepStepLocation stepLocation;
                stepLocation.fromString(stepTextList.at(2));
                step->setLocation(stepLocation);
                stack.push(step);
                step =0;
            }

        }

    }


}
void StepStepStackPrivate::loadFile(QString filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly|QIODevice::Text|QIODevice::Unbuffered);
    QString fileContents = file.readAll();
    file.close();

    fileContents = fileContents.trimmed();
    deserialize(fileContents);

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
