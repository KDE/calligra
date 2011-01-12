/* This file is part of the wvWare 2 project
   Copyright (C) 2001-2003 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02111-1307, USA.
*/

#include <test.h>
#include <sharedptr.h>

using namespace wvWare;

struct Foo : public Shared
{
    Foo() : bar( 42 ), baz( 42.42 ) { std::cerr << "Creating Foo" << std::endl; }
    ~Foo() { std::cerr << "Destroying Foo" << std::endl; }

    int bar;
    double baz;
};

SharedPtr<const Foo> createFoo()
{
    return SharedPtr<const Foo>( new Foo() );
}

void testMe()
{
    SharedPtr<const Foo> foo = createFoo();
    std::cerr << "Test 1: Passing around the object: ";
    test( foo->bar == 42 && foo.count() == 1 );

    std::cerr << "Test 2: Copying the \"pointer\": ";
    SharedPtr<const Foo> foo2( foo );
    test( foo2->bar == 42 && foo2.count() == 2 && foo->bar == 42 && foo.count() == 2 );

    std::cerr << "Going out of scope..." << std::endl;
}

// A small testcase for the SharedPtr template
int main( int, char** )
{
    std::cerr << "Testing the SharedPtr template..." << std::endl;
    testMe();
    std::cerr << "Done." << std::endl;
    return 0;
}
