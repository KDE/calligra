/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2013  Luke Wolf <email>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "StepStepStack.h"
#include "StepStepStack_p.h"
#include "StepStepBase.h"
#include <QtCore/QString>

StepStepStack::StepStepStack(QObject *parent):
    d(new StepStepStack_p())
{

}

StepStepStack::StepStepStack(const StepStepStack& other):QAbstractListModel(0)
{

}

StepStepStack::~StepStepStack()
{

}

StepStepStack& StepStepStack::operator=(const StepStepStack& other)
{
  //temporary
  return *this;

}

bool StepStepStack::operator==(const StepStepStack& other)
{
  //temporary
  return true;

}

StepStepBase StepStepStack::at(int i)
{
  return d->at(i);
}

StepStepBase StepStepStack::pop()
{
  return d->pop();
}
StepStepBase StepStepStack::top()
{
  return d->top();
}


void StepStepStack::deserialize(QString Filename)
{
    d->deserialize(Filename);
}
void StepStepStack::insertAt(int i, StepStepBase step)
{
    d->insertAt(i,step);
}

void StepStepStack::push(StepStepBase step)
{
    d->push(step);

}
void StepStepStack::removeAt(int i)
{
    d->removeAt(i);
}
void StepStepStack::serialize(QString Filename)
{
    d->serialize(Filename);
}

int StepStepStack::rowCount(const QModelIndex &parent) const
{
    return d->rowcount();
}

QVariant StepStepStack::data(const QModelIndex &index, int role) const
{
    int i = index.row();
    return d->data(i);
}
