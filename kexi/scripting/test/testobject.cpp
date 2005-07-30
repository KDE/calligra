/***************************************************************************
 * testobject.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#include "testobject.h"

#include <iostream> // for std::out

#include <kdebug.h>

TestObject::TestObject(QObject* parent)
    : QObject(parent, "TestObject")
{
    //kdDebug() << "TestObject::TestObject Constructor." << endl;
    connect(this, SIGNAL(testSignal()), this, SLOT(testSignalSlot()));
}

TestObject::~TestObject()
{
    //kdDebug() << "TestObject::~TestObject Destructor." << endl;
}

void TestObject::testSlot()
{
    kdDebug() << "TestObject::testSlot called" << endl;
    emit testSignal();
    emit testSignalString("This is the emitted TestObject::testSignalString(const QString&)");
}

void TestObject::testSlot2()
{
    kdDebug() << "TestObject::testSlot2 called" << endl;
}

void TestObject::testSignalSlot()
{
    kdDebug() << "TestObject::testSignalSlot called" << endl;
}

void TestObject::stdoutSlot(const QString& s)
{
    kdDebug() << "<stdout> " << s << endl;
    //std::cout << "<stdout> " << s.latin1() << std::endl;
}

void TestObject::stderrSlot(const QString& s)
{
    kdDebug() << "<stderr> " << s << endl;
    //std::cout << "<stderr> " << s.latin1() << std::endl;
}

//#include "testobject.moc"
