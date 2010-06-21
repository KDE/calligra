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

#include "parserfactory.h"
#include "parser95.h"
#include "parser97.h"
#include "olestream.h"
#include <iostream>
#include "wvlog.h"
#include <stdio.h>

using namespace wvWare;

namespace
{
    void diagnose( const unsigned char* const buffer )
    {
        // Check if it's a Word 3, 4, or 5 file
        if ( buffer[0] == 0x31 && buffer[1] == 0xbe &&
             buffer[2] == 0x00 && buffer[3] == 0x00 )
            std::cerr << "This is a Word 3, 4, or 5 file. Right now we don't handle these versions.\n"
                      << "Please send us the file, maybe we will implement it later on." << endl;
        else if ( buffer[0] == 0xdb && buffer[1] == 0xa5 &&
                  buffer[2] == 0x2d && buffer[3] == 0x00 )
            std::cerr << "This is a Word 2 document. Right now we don't handle this version." << endl
                      << "Please send us the file, maybe we will implement it later on." << endl;
        else
            std::cerr << "That doesn't seem to be a Word document." << endl;
    }

    SharedPtr<Parser> setupParser( OLEStorage* storage )
    {
        // Is it called WordDocument in all versions?
        OLEStreamReader* wordDocument = storage->createStreamReader( "WordDocument" );
        if ( !wordDocument || !wordDocument->isValid() ) {
            std::cerr << "Error: No 'WordDocument' stream found. Are you sure this is a Word document?" << endl;
            delete wordDocument;
            delete storage;
            return 0;
        }

        U16 magic = wordDocument->readU16();
        if ( magic != 0xa5ec && magic != 0xa5dc )
            wvlog << "+++ Attention: Strange magic number: " << magic << endl;

        U16 nFib = wordDocument->readU16();
        wvlog << "nFib = 0x" << hex << nFib << endl;
        wordDocument->seek( 0 );  // rewind the stream

        if ( nFib < 101 ) {
            std::cerr << "+++ Don't know how to handle nFib=" << nFib << endl;
            delete wordDocument;
            delete storage;
            return 0;
        }
        else if ( nFib == 101 ) {
            wvlog << "Word 6 document found" << endl;
            return new Parser95( storage, wordDocument );
        }
        else if ( nFib == 103 || nFib == 104 ) {
            wvlog << "Word 7 (aka Word 95) document found" << endl;
            return new Parser95( storage, wordDocument );
        }
        else if ( nFib == Word8nFib ) {  // Word8nFib == 193
            wvlog << "Word 8 (aka Word 97) document found" << endl;
            return new Parser97( storage, wordDocument );
        }
        else {
            if ( nFib == 217 ) {
                wvlog << "Looks like document was created with Word 9/Office 2000"
                    << ", trying with the Word 8 parser." << endl;
            }
            else if ( nFib == 257 ) {
                wvlog << "Looks like document was created with Word 10/Office XP"
                    << ", trying with the Word 8 parser." << endl;
            }
            else if ( nFib == 268 ) {
                wvlog << "Looks like document was created with Word 11/Office 2003"
                    << ", trying with the Word 8 parser." << endl;
            }
            else {
            wvlog << "A document newer than Word 8 found"
                  << ", trying with the Word 8 parser" << endl;
            }
            return new Parser97( storage, wordDocument );
        }
    }
}

SharedPtr<Parser> ParserFactory::createParser( const std::string& fileName )
{
    OLEStorage* storage( new OLEStorage( fileName ) );
    if ( !storage->open( OLEStorage::ReadOnly ) || !storage->isValid() ) {
        delete storage;

        FILE* file = fopen( fileName.c_str(), "r" );
        if ( !file ) {
            std::cerr << "Couldn't open " << fileName.c_str() << " for reading." << endl;
            return 0;
        }
        unsigned char buffer[4];
        fread( buffer, 1, 4, file );
        diagnose( buffer );
        fclose( file );
        return 0;
    }

    return setupParser( storage );
}

SharedPtr<Parser> ParserFactory::createParser( const unsigned char* buffer, size_t buflen )
{
    OLEStorage* storage( new OLEStorage( buffer, buflen ) );
    if ( !storage->open( OLEStorage::ReadOnly ) || !storage->isValid() ) {
        delete storage;
        if ( buflen >= 4 )
            diagnose( buffer );
        return 0;
    }
    return setupParser( storage );
}
