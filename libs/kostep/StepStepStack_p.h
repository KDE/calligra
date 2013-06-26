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

#ifndef STEPSTEPSTACK_P_H
#define STEPSTEPSTACK_P_H

#include <QtCore/QObject>
#include <QtCore/QStack>

class StepStepBase;
class StepStepStack_p :  QObject
{
  Q_OBJECT
public:
    StepStepStack_p();
    ~StepStepStack_p();
    StepStepBase at(int i);
    StepStepBase pop();
    StepStepBase top();
    void push(StepStepBase step);
    void serialize();
    void deserialize();
    void insertAt(int i);
    void removeAt(int i);

private:
    QStack<StepStepBase> stack;
};

#endif // STEPSTEPSTACK_P_H
