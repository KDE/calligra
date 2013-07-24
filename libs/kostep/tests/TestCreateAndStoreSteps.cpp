/*
 *     kostep -- handles changetracking using operational transformation for calligra
 *     Copyright (C) 2013  Luke Wolf <Lukewolf101010@gmail.com>
 *
 *     This library is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU Lesser General Public
 *     License as published by the Free Software Foundation; either
 *     version 2.1 of the License, or (at your option) any later version.
 *
 *     This library is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 *
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this library; if not, write to the Free Software
 *     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "TestCreateAndStoreSteps.h"

#include <QtTest/QTest>
#include <QtCore/QDebug>
#include "../StepStepStack.h"
#include "../StepAddTextStep.h"
#include "../StepStepBase.h"

QTEST_MAIN(TestCreateAndStoreSteps)

void TestCreateAndStoreSteps::initTestCase ()
{
  // Called before the first testfunction is executed
  stack = new StepStepStack();
  step = new StepStepBase();
}

void TestCreateAndStoreSteps::cleanupTestCase ()
{
  // Called after the last testfunction was executed
}

void TestCreateAndStoreSteps::init ()
{
  // Called before each testfunction is executed

}

void TestCreateAndStoreSteps::cleanup ()
{
  // Called after every testfunction
}

void TestCreateAndStoreSteps::CreateSteps ()
{

}

void TestCreateAndStoreSteps::PushStack ()
{
  if(step==0)
  {
    step = new StepStepBase();
  }
  qDebug() << "Creating Step";
  StepStepBase step2;// = new StepAddTextStep();
  qDebug() << "pushing onto stack";
  //StepAddTextStep step2 = step*;
  stack->push(*step);
  qDebug() << "in Test";
  stack->pop();
  qDebug() << "stack popped";

}

void TestCreateAndStoreSteps::PopStack ()
{
  stack->pop();

}

void TestCreateAndStoreSteps::CreateStack ()
{
  StepStepStack* stack2 = new StepStepStack();
  delete stack2;
}

#include "TestCreateAndStoreSteps.moc"
