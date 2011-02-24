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
#include <parser.h>
#include <parserfactory.h>
#include <global.h>

using namespace wvWare;

// A small testcase for the parser (Word97)
int main( int argc, char** argv )
{
    if ( argc > 2 ) {
        std::cerr << "Usage: parsertest_mem [foo.doc]" << std::endl;
        ::exit( 1 );
    }

    std::string file;
    if ( argc == 1 )
        file = "testole.doc";
    else
        file = argv[ 1 ];

    std::cerr << "Testing the parser with " << file << "..." << std::endl;

    FILE *fp;
    if( ( fp = fopen( file.c_str(), "rb" ) ) != 0 )
    {
        fseek( fp, 0, SEEK_END );
        size_t size = ftell( fp );
        fseek( fp, 0, SEEK_SET );
        unsigned char *mem = new unsigned char[ size ];
        if ( fread( mem, 1, size, fp ) == size )
        {
            SharedPtr<Parser> parser( ParserFactory::createParser( mem, size ) );
            std::cerr << "Trying... " << std::endl;
            if ( parser )
                test ( parser->parse() );
            std::cerr << "Done." << std::endl;
        }
        else
            std::cerr << "couldn't read file" << std::endl;
        fclose( fp );
        delete [] mem;
    }
    else
        std::cerr << "couldn't open file" << std::endl;

    return 0;
}
