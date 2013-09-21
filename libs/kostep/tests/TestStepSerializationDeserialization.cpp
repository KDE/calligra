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

void TestStepSerializationDeserialization::deserializeAddSteps()
{
    stack->deserialize("<add type=\"text\" s=\"/1/5\"> text </add>");
    qDebug() << stack->toString();
    stack->deserialize("<add type=\"paragraph\" s=\"/2/5\" />");
    qDebug() << stack->toString();
    stack->deserialize("<add type=\"paragraph\" s=\"/2/5\" /><add type=\"paragraph\" s=\"/2/5\" /><add type=\"paragraph\" s=\"/2/5\" />"
                       +(QString)"<add type=\"paragraph\" s=\"/2/5\" /><add type=\"paragraph\" s=\"/2/5\" /><add type=\"paragraph\" s=\"/2/5\" />");
    qDebug() << stack->toString();

}
void TestStepSerializationDeserialization::deserializeDelSteps()
{
    stack->deserialize("<del type=\"text\" s=\"/1/5\" />");
    qDebug() << stack->toString();
    stack->deserialize("<del type=\"paragraph\" s=\"/2/5\" />");
    qDebug() << stack->toString();

    stack->deserialize("<del type=\"paragraph\" s=\"/2/5\" /><del type=\"paragraph\" s=\"/2/5\" />" +
                       (QString)"<del type=\"paragraph\" s=\"/2/5\" /><del type=\"paragraph\" s=\"/2/5\" /><del type=\"paragraph\" s=\"/2/5\" />" +
            (QString)"<del type=\"paragraph\" s=\"/2/5\" /><del type=\"paragraph\" s=\"/2/5\" /><del type=\"paragraph\" s=\"/2/5\" />");
    qDebug() << stack->toString();
}
