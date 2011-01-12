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
#include <ustring.h>
#include <string.h>

using namespace wvWare;

// A small testcase for the UString class
int main( int, char** )
{
    std::cerr << "Testing the UString class..." << std::endl;

    std::cerr << "Test 1: isNull/isEmpty: ";
    UString string1;
    test( string1.isNull() && string1.isEmpty() );

    std::cerr << "Test 2: isNull vs. isEmpty: ";
    UString string2("");
    test( !string2.isNull() && string2.isEmpty() );

    std::cerr << "Test 3: isEmpty: ";
    UString string3( "test" );
    test( !string3.isNull() && !string3.isEmpty() );

    std::cerr << "Test 4: Copying a null string: ";
    UString string4( string1 );
    test( string4.isNull() && string4.isEmpty() );

    std::cerr << "Test 5: Copying an empty string: ";
    UString string5( string2 );
    test( !string5.isNull() && string5.isEmpty() );

    std::cerr << "Test 6: Creating and deleting a UConstString on static data: ";
    const int length = 12;
    { // open a new scope level to let the string go out of scope sooner
        // Not 0 terminated!
        UChar data[] = { 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!' };
        UConstString string( data, length );
        test( strcmp( string.string().ascii(), "Hello World!" ) == 0 );
    }

    std::cerr << "Test 7: Checking whether the data gets copied properly: ";
    UString testString;
    UChar *data = new UChar[ length ];
    data[ 0 ] = 'H'; data[ 1 ] = 'e'; data[ 2 ] = 'l'; data[ 3 ] = 'l';
    data[ 4 ] = 'o'; data[ 5 ] = ' '; data[ 6 ] = 'W'; data[ 7 ] = 'o';
    data[ 8 ] = 'r'; data[ 9 ] = 'l'; data[ 10 ] = 'd'; data[ 11 ] = '!';

    { // open a new scope level to let the string go out of scope sooner
        UConstString str( data, length );
        testString = str.string();
    }
    delete [] data;
    test( strcmp( testString.ascii(), "Hello World!" ) == 0 );

    std::cerr << "Test 8: Checking whether modifying a shallow copy works as expected I: ";
    data = new UChar[ length ];
    data[ 0 ] = 'H'; data[ 1 ] = 'e'; data[ 2 ] = 'l'; data[ 3 ] = 'l';
    data[ 4 ] = 'o'; data[ 5 ] = ' '; data[ 6 ] = 'W'; data[ 7 ] = 'o';
    data[ 8 ] = 'r'; data[ 9 ] = 'l'; data[ 10 ] = 'd'; data[ 11 ] = '!';

    { // open a new scope level to let the string go out of scope sooner
        UConstString str( data, length );
        testString = str.string();
        testString[ 0 ] = 'C';
        test( strcmp( str.string().ascii(), "Hello World!" ) == 0 );
    }
    delete [] data;
    std::cerr << "Test 9: Checking whether modifying a shallow copy works as expected II: ";
    test( strcmp( testString.ascii(), "Cello World!" ) == 0 );

    std::cerr << "Done." << std::endl;
    return 0;
}
