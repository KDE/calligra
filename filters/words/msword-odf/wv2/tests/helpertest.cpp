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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <test.h>
#include <olestream.h>
#include <word_helper.h>
#include <word97_helper.h>
#include <word97_generated.h>

using namespace wvWare;

// A small testcase for the word97_helper stuff
int main( int, char** )
{
    std::cerr << "Testing some auxilliary classes..." << std::endl;

    OLEStorage storage( std::string( "testole.doc" ) );
    test( storage.open( OLEStorage::ReadOnly ) );

    OLEStreamReader* document = storage.createStreamReader( "WordDocument" );
    test( document && document->isValid(), "Error: Couldn't open the WordDocument stream" );
    //document->dumpStream( "document.stream" );

    Word97::FIB fib( document, true );

    OLEStreamReader* table = storage.createStreamReader( fib.fWhichTblStm ? "1Table" : "0Table" );
    test( table && table->isValid(), "Error: Couldn't open the Table stream" );

    // Test STTBFASSOC reading from memory
    std::cerr << "Test 1: Read the STTBFASSOC from memory: ";
    table->seek( fib.fcSttbfAssoc );
    //table->dumpStream( "table.stream" );
    U8* data = new U8[ fib.lcbSttbfAssoc ];
    table->read( data, fib.lcbSttbfAssoc );
    STTBF sttbf( 0x0400, data );
    delete [] data;
    test( sttbf.count() == 18 );

    for ( UString s = sttbf.firstString(); !s.isNull(); s = sttbf.nextString() )
        std::cerr << "String: '" << s.ascii() << "'" << std::endl;

    // Test STTBFASSOC reading from a stream
    std::cerr << "Test 2: Read the STTBFASSOC from a stream: ";
    table->seek( fib.fcSttbfAssoc );
    STTBF sttbf2( 0x0400, table );

    UString s1 = sttbf.firstString(), s2 = sttbf2.firstString();
    bool failed = false;
    for ( ; !s1.isNull() && !s2.isNull(); s1 = sttbf.nextString(), s2 = sttbf2.nextString() ) {
        if ( s1 != s2 ) {
            failed = true;
            break;
        }
    }
    test( !failed );

    // Testing PLF reading/iterating
    std::cerr << "Test 3a: Reading a PLF (LFO): ";
    table->seek( fib.fcPlfLfo );
    PLF<Word97::LFO> plf( table );
    test( plf.count() == 10 ); // we have some padding data at the end, so the size is screwed

    std::cerr << "Test 3b: Reading a PLF (LSTF, \"short count\" PLF): ";
    table->seek( fib.fcPlcfLst );
    PLF<Word97::LSTF, true> plf1( table );
    test( plf1.count() * Word97::LSTF::sizeOf + 2 == fib.lcbPlcfLst );

    std::cerr << "Test 3c: Trying to match the list IDs: ";
    // Check whether all list ids match, then the file is okay (and we read it correctly)
    // Yes, I know that this is O(n^2), but hey, this is a test case for 10 elements ;)
    bool success = true;
    bool found = false;
    for ( const Word97::LSTF* lstf = plf1.first(); lstf != 0; lstf = plf1.next() ) {
        found = false;
        for ( const Word97::LFO* lfo = plf.first(); lfo != 0; lfo = plf.next() )
            if ( lstf->lsid == lfo->lsid ) {
                found = true;
                break;
            }
        if ( !found ) {
            success = false;
            break;
        }
    }
    test( success );

    table->seek( fib.fcPlcfbtePapx );
    std::cerr << "Test 4: Reading a PLCF: " << std::endl;
    //std::cerr << "Size: " << fib.lcbPlcfbtePapx << std::endl;
    PLCF<Word97::BTE> plcf( fib.lcbPlcfbtePapx, table );

    std::cerr << "Test 4a: Checking the size of the PLCF: ";
    test( plcf.count() == 6 );

    PLCFIterator<Word97::BTE> it( plcf );
    for ( ; it.current(); ++it ) {
        std::cerr << "Item: " << std::endl;
        std::cerr << "   start: " << it.currentStart() << std::endl;
        std::cerr << "   lim: " << it.currentLim() << std::endl;
        std::cerr << "   value: " << it.current()->pn << std::endl;
    }

    // Test the FKP template
    it.toFirst();   // rewind the iterator ;)
    std::cerr << "Test 5: Reading a FKP: " << std::endl;
    document->seek( it.current()->pn << 9, G_SEEK_SET );
    FKP< BX<Word97::PHE> > fkp( document, false );
    std::cerr << "crun: " << fkp.crun() << std::endl;
    FKPIterator< BX<Word97::PHE> > it2( fkp );
    for ( int i = 0; !it2.atEnd(); ++it2, ++i ) {
        std::cerr << "Item(" << i << "): " << std::endl;
        std::cerr << "   start: " << it2.currentStart() << std::endl;
        std::cerr << "   lim: " << it2.currentLim() << std::endl;
        if ( it2.current() )
            std::cerr << "   plain entry" << std::endl;
        else
            std::cerr << "   null entry" << std::endl;
    }

    std::cerr << "sizeof(PRM): " << sizeof(Word97::PRM) << std::endl;
    std::cerr << "sizeof(PRM2): " << sizeof(Word97::PRM2) << std::endl;
    std::cerr << "sizeof(PCD): " << sizeof(Word97::PCD) << std::endl;
    std::cerr << "sizeof(DTTM): " << sizeof(Word97::DTTM) << std::endl;
    std::cerr << "sizeof(BTE): " << sizeof(Word97::BTE) << std::endl;
    std::cerr << "sizeof(PHE): " << sizeof(Word97::PHE) << std::endl;
    std::cerr << "sizeof(BX): " << sizeof(BX<Word97::PHE>) << std::endl;

    delete document;
    delete table;
    std::cerr << "Done." << std::endl;
    return 0;
}
