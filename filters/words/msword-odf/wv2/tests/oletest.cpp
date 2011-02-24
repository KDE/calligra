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

#include <olestorage.h>
#include <olestream.h>
#include <utilities.h>
#ifdef HAVE_UNISTD
#include <unistd.h>
#endif
#include <stdlib.h>
#include <time.h>

#include <test.h>

using namespace wvWare;

// Uncomment that line to check whether libole2 correctly handles
// files containing only so called small blocks
//#define SMALL_BLOCKS_ONLY 1

// Some basic test for the storage system. Please don't change the oletest.doc
// file as some tests depend on exacly that file (e.g. directory names)
int main( int, char** )
{
    std::cerr << "Testing the OLE storage and the streams..." << std::endl;
    std::cerr << "###############################################################################" << std::endl;
    std::cerr << "Test 1: Open the sample file: ";
    OLEStorage storage( "testole.doc" );
    test( storage.open( OLEStorage::ReadOnly ) );
    std::cerr << "###############################################################################" << std::endl;

    std::cerr << "Test2: List the contents of the root directory:" << std::endl;
    std::list<std::string> dir = storage.listDirectory();
    test( storage.isValid() );

    std::list<std::string>::const_iterator it = dir.begin();
    std::list<std::string>::const_iterator end = dir.end();
    for ( ; it != end; ++it )
        std::cerr << "   - " << *it << std::endl;
    std::cerr << "###############################################################################" << std::endl;

    std::cerr << "Test3: Stat a stream: skipped" << std::endl;
    /*
    MsOleStat s = storage.stat( "WordDocument" );
    test( storage.lastError() == MS_OLE_ERR_OK && s.type == MsOleStreamT &&
          s.size != static_cast<unsigned int>( -1 ), "### Error in Test 3: " + int2string( storage.lastError() ), "Passed." );
    */

    std::cerr << "Test4: Stat a storage (directory): skipped" << std::endl;
    /*
    s = storage.stat( "ObjectPool" );
    test( storage.lastError() == MS_OLE_ERR_OK && s.type == MsOleStorageT &&
          s.size != static_cast<unsigned int>( -1 ) , "### Error in Test 4: " + int2string( storage.lastError() ), "Passed." );
    */

    std::cerr << "Test5: Enter a directory: ";
    test( storage.enterDirectory( "ObjectPool" ) );
    std::cerr << "###############################################################################" << std::endl;

    std::cerr << "Test6: List the contents of the current directory:" << std::endl;
    dir = storage.listDirectory();
    test( storage.isValid() );
    it = dir.begin();
    end = dir.end();
    for ( ; it != end; ++it )
        std::cerr << "   - " << *it << std::endl;
    std::cerr << "###############################################################################" << std::endl;

    std::cerr << "Test7: Leave a directory." << std::endl;
    storage.leaveDirectory();
    storage.leaveDirectory();  // we can do that as often as we want to
    storage.leaveDirectory();  // as we will never leave '/'
    std::cerr << "###############################################################################" << std::endl;

    std::cerr << "Test8: List the contents of the current directory (should be the root dir):" << std::endl;
    dir = storage.listDirectory();
    test( storage.isValid() );
    it = dir.begin();
    end = dir.end();
    for ( ; it != end; ++it )
        std::cerr << "   - " << *it << std::endl;
    std::cerr << "###############################################################################" << std::endl;

    // go somewhere (for testing purpose)
    storage.enterDirectory( "ObjectPool" );
    std::cerr << "Test9: Set a few custom paths:" << std::endl;
    std::cerr << "   Trying '/ObjectPool/_1020966487': ";
    test( storage.setPath("/ObjectPool/_1020966487") );
    std::cerr << "   Trying '/ObjectPool/_1020966487': ";
    test( storage.setPath("/ObjectPool/_1020966487") );
    std::cerr << "   Trying 'ObjectPool/_1020966487': ";
    test( storage.setPath("ObjectPool/_1020966487") );
    std::cerr << "   Trying 'ObjectPool/_1020966487/': ";
    test( storage.setPath("ObjectPool/_1020966487/") );
    std::cerr << "   Trying '/ObjectPool/_1020966487/': ";
    test( storage.setPath("/ObjectPool/_1020966487/") );
    std::cerr << "   Trying '/ObjectPool_1020966487': ";
    test( !storage.setPath("/ObjectPool_1020966487") );
    std::cerr << "   Trying '/': ";
    test( storage.setPath("/") && storage.path()=="/" );
    std::cerr << "###############################################################################" << std::endl;

    std::cerr << "Test10: Open a stream for reading: ";
    OLEStreamReader* reader = storage.createStreamReader( "1Table" );
    test( reader && reader->isValid() );

    std::cerr << "Test11: Check whether we are at 0 right after opening: ";
    test( reader->tell() == 0 );

    std::cerr << "Test12: Check the size of the stream: ";
    test( reader->size() == 0x20a6 );

    std::cerr << "Test13: Seek in the stream: ";
    reader->seek( 0x100 );
    test( reader->seek( 1, G_SEEK_CUR ) && reader->tell() == 0x101 );
    reader->push(); // save the current pos (0x101)

    std::cerr << "Test14: More seeking: ";
    test( reader->seek( -2, G_SEEK_CUR ) && reader->tell() == 0xff );
    reader->push(); // save the current pos (0xff)

    std::cerr << "Test15: Even more seeking: ";
    test( reader->seek( -2, G_SEEK_END ) && reader->tell() == 0x20a4 );

    std::cerr << "Test16: Checking the stack: ";
    test( reader->pop() && reader->tell() == 0xff );

    std::cerr << "Test17: Checking the stack again: ";
    test( reader->pop() && reader->tell() == 0x101 );

    // clean up the current stream
    delete reader;
    reader = 0;

    std::cerr << "Test18: Open another stream for reading: ";
    reader = storage.createStreamReader( "WordDocument" );
    test( reader && reader->isValid() );

    std::cerr << "Test19: Read the magic 16 Bit value: ";
    test( reader->readU16() == 0xa5ec );

    std::cerr << "Test20: Check the nFib: ";
    test( reader->readU16() == 193 );

    std::cerr << "Test21: Check the byte reading: ";
    reader->seek( 2 );
    test( reader->readU8() == 0xc1 );

    std::cerr << "Test22: Check the word reading: ";
    reader->seek( 0 );
    test( reader->readU32() == 0x00c1a5ec );

    // Get rid of our test document
    // The warning we get is the intended behavior :)
    storage.close();

    // Open a new document for write tests (make sure
    // it doesn't exist)
    system( "rm test123456.doc &> /dev/null" );
    storage.setName( "test123456.doc" );
    test( storage.open( OLEStorage::WriteOnly ) );

    std::cerr << "Test23: Open a stream for writing (I): ";
    OLEStreamWriter* writer = storage.createStreamWriter( "Check1" );
    test( writer && writer->isValid() );

    writer->write( static_cast<U32>( 0x11111111 ) );
    writer->write( static_cast<U32>( 0x11111111 ) );
    writer->write( static_cast<U32>( 0x11111111 ) );
    writer->write( static_cast<U32>( 0x11111111 ) );
    writer->write( static_cast<U32>( 0x11111111 ) );
    writer->write( static_cast<U32>( 0x11111111 ) );
    writer->write( static_cast<U32>( 0x11111111 ) );
    writer->write( static_cast<U32>( 0x11111111 ) );
    writer->write( static_cast<U32>( 0x11111111 ) );

    // write it to disk...
    delete writer;
    writer = 0;

    std::cerr << "Test24: Open a stream for writing (II): ";
    writer = storage.createStreamWriter( "Check2" );
    test( writer && writer->isValid() );

    writer->write( static_cast<U8>( 200 ) );
    writer->write( static_cast<S8>( 123 ) );
    writer->write( static_cast<U16>( 42420 ) );
    writer->write( static_cast<S16>( -21420 ) );
    writer->write( static_cast<U32>( 0xdeadbeef ) );
    writer->write( static_cast<S32>( 0x7fffffff ) );

    // write it to disk...
    delete writer;
    writer = 0;

#ifndef SMALL_BLOCKS_ONLY
    std::cerr << "Test25: Open a stream for writing (III): ";
    writer = storage.createStreamWriter( "Check3" );
    test( writer && writer->isValid() );

    for ( int i = 0; i < 10000; ++i ) {
        writer->write( static_cast<S32>( 0x22222222 ) );
        writer->write( static_cast<S32>( 0x22222222 ) );
        writer->write( static_cast<S32>( 0x22222222 ) );
        writer->write( static_cast<S32>( 0x22222222 ) );
        writer->write( static_cast<S32>( 0x22222222 ) );
    }

    delete writer;
    writer = 0;
#endif

    std::cerr << "Test26: Open a stream for writing (IV): ";
    writer = storage.createStreamWriter( "Check4" );
    test( writer && writer->isValid() );

    srand( time( 0 ) );
    U8 buffer1[ 256 ];
    for ( int i = 0; i< 256; ++i )
        buffer1[ i ] = rand() % 256;

    writer->write( buffer1, 256 );

    delete writer;
    writer = 0;

    storage.close();

    std::cerr << "Test27: Read back the created file: ";
    test( storage.open( OLEStorage::ReadOnly ) && storage.isValid() );

    std::cerr << "Test28: Open the stream I for reading: ";
    reader = storage.createStreamReader( "Check1" );
    test( reader && reader->isValid() );

    std::cerr << "Test29: Check the size of the stream I: ";
    test( reader->size() == 36 );

    delete reader;
    reader = 0;

    std::cerr << "Test30: Open the stream II for reading: ";
    reader = storage.createStreamReader( "Check2" );
    test( reader && reader->isValid() );

    std::cerr << "Test31: Check the size of the stream II: ";
    test( reader->size() == 14 );

    std::cerr << "Test32: Check the contents of the stream II: ";
    test( reader->readU8() == 200 && reader->readS8() == 123 && reader->readU16() == 42420 &&
        reader->readS16() == -21420 && reader->readU32() == 0xdeadbeef && reader->readS32() == 0x7fffffff );

    delete reader;
    reader = 0;

    std::cerr << "Test33: Open the stream IV for reading: ";
    reader = storage.createStreamReader( "Check4" );
    test( reader && reader->isValid() );

    std::cerr << "Test34: Read back and compare the content with the expected values: ";
    U8 buffer2[ 256 ];
    reader->read( buffer2, 256 );
    bool success = true;
    for ( int i = 0; i < 256; ++i )
        if ( buffer1[ i ] != buffer2[ i ] ) {
            success = false;
            break;
        }
    test( success );

    storage.close(); // missing delete reader; intendet, doesn't result in memleaks

    std::cerr << "Done." << std::endl;
    return 0;
}
