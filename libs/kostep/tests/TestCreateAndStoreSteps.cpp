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
#include "../StepAddTextBlockStep.h"
#include "../StepDeleteTextBlockStep.h"
#include "../StepDeleteTextStep.h"
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
  step = new StepStepBase();
  stack = new StepStepStack();
}

void TestCreateAndStoreSteps::cleanup ()
{
  // Called after every testfunction

}

void TestCreateAndStoreSteps::CreateSteps ()
{
  qDebug() << "Creating Add Text step";
  step = new StepAddTextStep();
  delete step;
  qDebug() << "Creating Add TextBlock step";
  step = new StepAddTextBlockStep();
  delete step;
  qDebug() << "Creating Delete TextBlock step";
  step = new StepDeleteTextBlockStep();
  delete step;
  qDebug() << "Create Delete Text step";
  step = new StepDeleteTextStep();
  delete step;
  qDebug() << "Created and Deleted All Steps Successfully";
  step =0;

}

void TestCreateAndStoreSteps::TestStack ()
{
  if(step==0)
  {
    qDebug() << "Creating Step";
    step = new StepStepBase();
  }

  qDebug() << "pushing onto stack";

  stack->push(*step);
  qDebug() << "in Test";
  stack->pop();
  qDebug() << "stack popped";

}

#include "TestCreateAndStoreSteps.moc"
