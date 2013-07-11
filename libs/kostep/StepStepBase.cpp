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
StepStepBase::StepStepBase(QObject *parent):
    QObject(parent),d(new StepStepBase_p())
{

}

StepStepBase::~StepStepBase()
{
  delete d;
}
StepStepBase::StepStepBase(const StepStepBase& other): QObject()
{

}
StepStepBase& StepStepBase::operator=(const StepStepBase& other)
{
  return *this;

}
bool StepStepBase::operator==( StepStepBase& other)
{
  if (d->operator==(other.d)) {
    return true;
  }
  return false;
}


bool StepStepBase::Foreign()
{
  return d->Foreign();
}
int StepStepBase::Position()
{
  return d->Position();
}
void StepStepBase::setForeign(bool foreign)
{
  d->setForeign(foreign);

}
void StepStepBase::setPosition(int position)
{
  d->setPosition(position);

}
QString StepStepBase::Step()
{
  return d->Step();
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
  return d->Type();

}

StepStepLocation StepStepBase::Location()
{
  return d->Location();
}
void StepStepBase::setLocation(StepStepLocation location)
{
  d->setLocation(location);

}
