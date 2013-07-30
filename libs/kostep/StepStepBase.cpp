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


#include "StepStepBase.h"
#include "StepStepBase_p.h"
#include "StepStepLocation.h"
void
StepStepBase::temp ()
{

}
StepStepBase::StepStepBase(QObject *parent):
    QObject(parent),d(new StepStepBasePrivate())
{

}

StepStepBase::~StepStepBase()
{
  if (!d==0)
  {
    delete d;
  }
}
StepStepBase::StepStepBase(const StepStepBase &other): QObject(),
d(new StepStepBasePrivate(other.d->type, other.d->position,other.d->isForeign))
{

}

StepStepBase::StepStepBase (QString type, QObject *parent, int position,
                bool isForeign):
QObject (parent), d(new StepStepBasePrivate(type,position,isForeign))
{

}

StepStepBase& StepStepBase::operator=(const StepStepBase &other)
{
    return *this;

}
bool StepStepBase::operator==( StepStepBase &other)
{
    if (d->operator==(other.d)) {
       return true;
    }
    return false;
}


bool StepStepBase::Foreign()
{
    return d->isForeign;
}
int StepStepBase::Position()
{
    return d->position;
}
void StepStepBase::setForeign(bool foreign)
{
    d->isForeign = foreign;

}
void StepStepBase::setPosition(int position)
{
    d->position = position;

}
QString StepStepBase::stepText()
{
    return d->step;
}

void StepStepBase::setStepText (QString text)
{
    d->step = text;
}

QString StepStepBase::toString()
{
    return d->toString();
}
QString StepStepBase::toXML()
{
   return d->toXML();
}
QString StepStepBase::Type()
{
   return d->type;

}

StepStepLocation StepStepBase::location()
{
   return d->location;
}
void StepStepBase::setLocation(StepStepLocation &location)
{
   d->location = location;

}
