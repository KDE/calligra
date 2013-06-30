/*
    <one line to give the library's name and an idea of what it does.>
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


#ifndef STEPSTEPBASE_H
#define STEPSTEPBASE_H
#include <QtCore/QObject>
#include <QtCore/QString>

class StepStepBase_p;
class StepStepLocation;

class StepStepBase : public QObject
{
    Q_OBJECT

    //Basic Properties of a step, allows for usage in QML
    Q_PROPERTY(bool Foreign READ Foreign WRITE setForeign)
    Q_PROPERTY(int Position READ Position WRITE setPosition)
    Q_PROPERTY(QString Step READ Step)
    Q_PROPERTY(QString Type READ Type)

public:
    explicit StepStepBase(QObject *parent =0);
    StepStepBase(const StepStepBase& other);

    virtual ~StepStepBase();
    virtual StepStepBase& operator=(const StepStepBase& other);
    virtual bool operator==(StepStepBase& other);

    //String Representation of a Step
    virtual QString toString();
    //XML Representation of a Step for use in serialization
    virtual QString toXML();

    //The Foreign Property gives indication whether the step came
    //from a source besides the user, say for instance if the user
    //is collaboratively editing with another user and thus receiving
    //steps from them
    virtual bool Foreign();
    virtual void setForeign(bool foreign);

    //The Position Property gives the position in characters from the
    //beginning of the text  which is then used when doing Operational
    //Transformation to allow derivative operations
    int Position();
    virtual void setPosition(int position);

    //The Location Property gives a structured location within the
    //document that can be translated to and from a QTextCursor
    //Potentially deprecates Position()
    //StepStepLocation Location();
    //virtual void setLocation(StepStepLocation location);


    //The Step Property holds the text of the change so say I've got the word
    //Dog and I make it Dogs, Step would hold an "s"
    //NOTE: May be later moved out into a StepTextStep class which textual changes
    //will derive from
    virtual QString Step();

    //The Type Property Returns the Type of Operation that the step is as a QString
    //mostly useful for equality testing and serialization/deserialization
    virtual QString Type();

private:

  StepStepBase_p *d;

};

#endif // STEPSTEPBASE_H
