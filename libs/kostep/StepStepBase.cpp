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


#include "StepStepBase.h"

StepStepBase::StepStepBase()
{

}

StepStepBase::StepStepBase(const StepStepBase& other)
{

}

StepStepBase::~StepStepBase()
{

}

StepStepBase& StepStepBase::operator=(const StepStepBase& other)
{
    return *this;
}

bool StepStepBase::operator==(const StepStepBase& other) const
{
///TODO: return ...;
  return true;
}

bool StepStepBase::Foreign()
{
  return false;
}
int StepStepBase::Position()
{
  return 0;
}
void StepStepBase::setForeign(bool foreign)
{

}
void StepStepBase::setPosition(int position)
{

}
QString StepStepBase::Step()
{
  QString string = (QString)"";
  return string;
}
QString StepStepBase::toString()
{
  QString string = (QString)"";
  return string;
}
QString StepStepBase::toXML()
{
  QString string = (QString)"";
  return string;
}
QString StepStepBase::Type()
{
  QString string = (QString)"";
  return string;

}


