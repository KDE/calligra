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

#include "StepStepBase_p.h"
#include "StepStepLocation.h"

StepStepBasePrivate::StepStepBasePrivate():type("Base"),position(0),isForeign(false)
{
}

StepStepBasePrivate::~StepStepBasePrivate ()
{

}

StepStepBasePrivate::StepStepBasePrivate (QString type, int position, bool isForeign):
    type(type), position(position), isForeign(isForeign)
{

}


bool StepStepBasePrivate::operator==(StepStepBasePrivate *other)
{
    if (other->isForeign == isForeign &&
      other->position == position &&
      other->stepText == stepText &&
      other->type == type
    ) {
    return true;
    }
    else {
    return false;
    }
}

QString StepStepBasePrivate::toString()
{
    QString string = type + " " + stepText + " " + QString::number(position);
    return string;
}

QString StepStepBasePrivate::toXML()
{
    //Should not be called, a derivative class should overload the toXML method
    qDebug("Not supposed to be here");
    QString string = "";
    return string;
}
