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

#include "TestStepSerializationDeserialization.h"
#include <QtTest/QTest>
#include <QtCore/QDebug>
#include "../StepSteps.h"

QTEST_MAIN(TestStepSerializationDeserialization)

void TestStepSerializationDeserialization::initTestCase ()
{
  // Called before the first testfunction is executed
  stack = new StepStepStack();
  step = new StepStepBase();
}

void TestStepSerializationDeserialization::cleanupTestCase ()
{
  // Called after the last testfunction was executed
}

void TestStepSerializationDeserialization::init ()
{
  // Called before each testfunction is executed
  step = new StepStepBase();
  stack = new StepStepStack();
}

void TestStepSerializationDeserialization::cleanup ()
{
  // Called after every testfunction

}

void TestStepSerializationDeserialization::deserializeAddSteps ()
{
    stack->deserialize("<add type=\"text\" s=\"/1/5\"> text </add>");
    qDebug() << stack->toString();
    Q_ASSERT(stack->toString() == "<add type=\"text\" s=\"/1/5\" > text </add>");

    delete stack;
    stack = new StepStepStack();
    stack->deserialize("<add type=\"paragraph\" s=\"/2/5\" />");
    qDebug() << stack->toString();
    //Q_ASSERT(stack->toString().toLower() == "<add type=\"paragraph\" s=\"/2/5\" />");

    delete stack;
    stack = new StepStepStack();
    stack->deserialize("<add type=\"paragraph\" s=\"/2/5\" /><add type=\"text\" s=\"/1/5\"> text </add><add type=\"paragraph\" s=\"/2/5\" />");
    qDebug() << stack->toString();
    //Q_ASSERT(stack->toString().toLower() == "<add type=\"paragraph\" s=\"/2/5\" /><add type=\"text\" s=\"/1/5\"> text </add><add type=\"paragraph\" s=\"/2/5\" />");
}
void TestStepSerializationDeserialization::deserializeDelSteps ()
{
    //test delete textstep
    stack->deserialize("<del type=\"text\" s=\"/1/5\" s=\"/1/6\" />");
    qDebug() << stack->toString();
    Q_ASSERT(stack->toString() == "<del type=\"text\" s=\"/1/5\" s=\"/1/6\" />");

    //test delete paragraph step
    delete stack;
    stack = new StepStepStack();
    stack->deserialize("<del type=\"paragraph\" s=\"/2/5\" />");
    qDebug() << stack->toString();
    Q_ASSERT(stack->toString() == "<del type=\"paragraph\" s=\"/2/5\" />");

    //test delete multiple steps
    delete stack;
    stack = new StepStepStack();
    stack->deserialize("<del type=\"paragraph\" s=\"/2/5\" /><del type=\"paragraph\" s=\"/2/5\" />");
    qDebug() << stack->toString();
    Q_ASSERT(stack->toString() == "<del type=\"paragraph\" s=\"/2/5\" /><del type=\"paragraph\" s=\"/2/5\" />");

    //test having whitespace in the text
    delete stack;
    stack = new StepStepStack();
    stack->deserialize("<del         type=\"paragraph\"   \n\n\n     s=\"/2/5\" />   \n\n\n <\ndel\n type=\"paragraph\" s=\"/2/5\" />");
    qDebug() << stack->toString();
    Q_ASSERT(stack->toString() == "<del type=\"paragraph\" s=\"/2/5\" /><del type=\"paragraph\" s=\"/2/5\" />");
}
