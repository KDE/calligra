// This file contains generated testing code. We do some basic tests
// like writing and reading from/to streams. Of course these tests are
// neither complete nor very advanced, so watch out :)

#include <word95_generated.h>
#include <olestream.h>
#include <iostream>
#include <stdlib.h>   // rand(), srand()

#include <time.h>   // time()

using namespace wvWare;
using namespace Word95;

int main(int, char**) {

    // First we have to create some infrastructure
    system("rm word95_test.doc &> /dev/null");
    OLEStorage storage("word95_test.doc");
    if(!storage.open(OLEStorage::WriteOnly)) {
        std::cout << "Error: Couldn't open the storage!" << std::endl;
        ::exit(1);
    }

    OLEStreamWriter *writer=storage.createStreamWriter("TestStream");
    if(!writer || !writer->isValid()) {
        std::cout << "Error: Couldn't open a stream for writing!" << std::endl;
        ::exit(1);
    }

    // Initialize the random number generator
    srand( time( 0 ) );

    // Some "global" variables...
    int *ptr=0;  // used to "initialize" the structs
    int tmp;
    std::cout << "Testing the Word95 structures..." << std::endl;
    // Begin of writing test for DTTM
    std::cout << "Testing writing for DTTM: ";
    DTTM dttm1;
    // Initilaize the struct with random data
    tmp=sizeof(DTTM)/sizeof(int);
    ptr=reinterpret_cast<int*>( &dttm1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(DTTM) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(dttm1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    dttm1.dump();
    // End of writing test for DTTM

    // Begin of writing test for PRM2
    std::cout << "Testing writing for PRM2: ";
    PRM2 prm21;
    // Initilaize the struct with random data
    tmp=sizeof(PRM2)/sizeof(int);
    ptr=reinterpret_cast<int*>( &prm21 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(PRM2) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(prm21.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for PRM2

    // Begin of writing test for PRM
    std::cout << "Testing writing for PRM: ";
    PRM prm1;
    // Initilaize the struct with random data
    tmp=sizeof(PRM)/sizeof(int);
    ptr=reinterpret_cast<int*>( &prm1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(PRM) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(prm1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for PRM

    // Begin of writing test for SHD
    std::cout << "Testing writing for SHD: ";
    SHD shd1;
    // Initilaize the struct with random data
    tmp=sizeof(SHD)/sizeof(int);
    ptr=reinterpret_cast<int*>( &shd1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(SHD) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(shd1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    shd1.dump();
    // End of writing test for SHD

    // Begin of writing test for PHE
    std::cout << "Testing writing for PHE: ";
    PHE phe1;
    // Initilaize the struct with random data
    tmp=sizeof(PHE)/sizeof(int);
    ptr=reinterpret_cast<int*>( &phe1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(PHE) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(phe1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    phe1.dump();
    // End of writing test for PHE

    // Begin of writing test for BRC
    std::cout << "Testing writing for BRC: ";
    BRC brc1;
    // Initilaize the struct with random data
    tmp=sizeof(BRC)/sizeof(int);
    ptr=reinterpret_cast<int*>( &brc1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(BRC) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(brc1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    brc1.dump();
    // End of writing test for BRC

    // Begin of writing test for TLP
    std::cout << "Testing writing for TLP: ";
    TLP tlp1;
    // Initilaize the struct with random data
    tmp=sizeof(TLP)/sizeof(int);
    ptr=reinterpret_cast<int*>( &tlp1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(TLP) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(tlp1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    tlp1.dump();
    // End of writing test for TLP

    // Begin of writing test for TC
    std::cout << "Testing writing for TC: ";
    TC tc1;
    // Initilaize the struct with random data
    tmp=sizeof(TC)/sizeof(int);
    ptr=reinterpret_cast<int*>( &tc1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(TC) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(tc1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    tc1.dump();
    // End of writing test for TC

    // Begin of writing test for DPHEAD
    std::cout << "Testing writing for DPHEAD: ";
    DPHEAD dphead1;
    // Initilaize the struct with random data
    tmp=sizeof(DPHEAD)/sizeof(int);
    ptr=reinterpret_cast<int*>( &dphead1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(DPHEAD) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(dphead1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for DPHEAD

    // Begin of writing test for DPTXBX
    std::cout << "Testing writing for DPTXBX: ";
    DPTXBX dptxbx1;
    // Initilaize the struct with random data
    tmp=sizeof(DPTXBX)/sizeof(int);
    ptr=reinterpret_cast<int*>( &dptxbx1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(DPTXBX) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(dptxbx1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for DPTXBX

    std::cout << "Testing writing for DPPOLYLINE:" << std::endl;
    std::cout << "  Sorry, testing dynamic structures isn't implemented,"
              << " yet." << std::endl;
    // Begin of writing test for ANLD
    std::cout << "Testing writing for ANLD: ";
    ANLD anld1;
    // Initilaize the struct with random data
    tmp=sizeof(ANLD)/sizeof(int);
    ptr=reinterpret_cast<int*>( &anld1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(ANLD) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(anld1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    anld1.dump();
    // End of writing test for ANLD

    // Begin of writing test for ANLV
    std::cout << "Testing writing for ANLV: ";
    ANLV anlv1;
    // Initilaize the struct with random data
    tmp=sizeof(ANLV)/sizeof(int);
    ptr=reinterpret_cast<int*>( &anlv1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(ANLV) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(anlv1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    anlv1.dump();
    // End of writing test for ANLV

    // Begin of writing test for BKF
    std::cout << "Testing writing for BKF: ";
    BKF bkf1;
    // Initilaize the struct with random data
    tmp=sizeof(BKF)/sizeof(int);
    ptr=reinterpret_cast<int*>( &bkf1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(BKF) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(bkf1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for BKF

    // Begin of writing test for BKL
    std::cout << "Testing writing for BKL: ";
    BKL bkl1;
    // Initilaize the struct with random data
    tmp=sizeof(BKL)/sizeof(int);
    ptr=reinterpret_cast<int*>( &bkl1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(BKL) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(bkl1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for BKL

    // Begin of writing test for BRC10
    std::cout << "Testing writing for BRC10: ";
    BRC10 brc101;
    // Initilaize the struct with random data
    tmp=sizeof(BRC10)/sizeof(int);
    ptr=reinterpret_cast<int*>( &brc101 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(BRC10) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(brc101.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for BRC10

    // Begin of writing test for BTE
    std::cout << "Testing writing for BTE: ";
    BTE bte1;
    // Initilaize the struct with random data
    tmp=sizeof(BTE)/sizeof(int);
    ptr=reinterpret_cast<int*>( &bte1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(BTE) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(bte1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for BTE

    // Begin of writing test for CHP
    std::cout << "Testing writing for CHP: ";
    CHP chp1;
    // Initilaize the struct with random data
    tmp=sizeof(CHP)/sizeof(int);
    ptr=reinterpret_cast<int*>( &chp1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(CHP) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(chp1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    chp1.dump();
    // End of writing test for CHP

    // Begin of writing test for DCS
    std::cout << "Testing writing for DCS: ";
    DCS dcs1;
    // Initilaize the struct with random data
    tmp=sizeof(DCS)/sizeof(int);
    ptr=reinterpret_cast<int*>( &dcs1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(DCS) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(dcs1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    dcs1.dump();
    // End of writing test for DCS

    // Begin of writing test for DO
    std::cout << "Testing writing for DO: ";
    DO do1;
    // Initilaize the struct with random data
    tmp=sizeof(DO)/sizeof(int);
    ptr=reinterpret_cast<int*>( &do1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(DO) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(do1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for DO

    // Begin of writing test for DOP
    std::cout << "Testing writing for DOP: ";
    DOP dop1;
    // Initilaize the struct with random data
    tmp=sizeof(DOP)/sizeof(int);
    ptr=reinterpret_cast<int*>( &dop1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(DOP) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(dop1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for DOP

    // Begin of writing test for DPARC
    std::cout << "Testing writing for DPARC: ";
    DPARC dparc1;
    // Initilaize the struct with random data
    tmp=sizeof(DPARC)/sizeof(int);
    ptr=reinterpret_cast<int*>( &dparc1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(DPARC) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(dparc1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for DPARC

    // Begin of writing test for DPELLIPSE
    std::cout << "Testing writing for DPELLIPSE: ";
    DPELLIPSE dpellipse1;
    // Initilaize the struct with random data
    tmp=sizeof(DPELLIPSE)/sizeof(int);
    ptr=reinterpret_cast<int*>( &dpellipse1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(DPELLIPSE) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(dpellipse1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for DPELLIPSE

    // Begin of writing test for DPLINE
    std::cout << "Testing writing for DPLINE: ";
    DPLINE dpline1;
    // Initilaize the struct with random data
    tmp=sizeof(DPLINE)/sizeof(int);
    ptr=reinterpret_cast<int*>( &dpline1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(DPLINE) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(dpline1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for DPLINE

    // Begin of writing test for DPRECT
    std::cout << "Testing writing for DPRECT: ";
    DPRECT dprect1;
    // Initilaize the struct with random data
    tmp=sizeof(DPRECT)/sizeof(int);
    ptr=reinterpret_cast<int*>( &dprect1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(DPRECT) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(dprect1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for DPRECT

    // Begin of writing test for DPSAMPLE
    std::cout << "Testing writing for DPSAMPLE: ";
    DPSAMPLE dpsample1;
    // Initilaize the struct with random data
    tmp=sizeof(DPSAMPLE)/sizeof(int);
    ptr=reinterpret_cast<int*>( &dpsample1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(DPSAMPLE) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(dpsample1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for DPSAMPLE

    // Begin of writing test for FDOA
    std::cout << "Testing writing for FDOA: ";
    FDOA fdoa1;
    // Initilaize the struct with random data
    tmp=sizeof(FDOA)/sizeof(int);
    ptr=reinterpret_cast<int*>( &fdoa1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(FDOA) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(fdoa1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for FDOA

    // Begin of writing test for FIB
    std::cout << "Testing writing for FIB: ";
    FIB fib1;
    // Initilaize the struct with random data
    tmp=sizeof(FIB)/sizeof(int);
    ptr=reinterpret_cast<int*>( &fib1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(FIB) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(fib1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for FIB

    // Begin of writing test for LSPD
    std::cout << "Testing writing for LSPD: ";
    LSPD lspd1;
    // Initilaize the struct with random data
    tmp=sizeof(LSPD)/sizeof(int);
    ptr=reinterpret_cast<int*>( &lspd1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(LSPD) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(lspd1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    lspd1.dump();
    // End of writing test for LSPD

    // Begin of writing test for METAFILEPICT
    std::cout << "Testing writing for METAFILEPICT: ";
    METAFILEPICT metafilepict1;
    // Initilaize the struct with random data
    tmp=sizeof(METAFILEPICT)/sizeof(int);
    ptr=reinterpret_cast<int*>( &metafilepict1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(METAFILEPICT) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(metafilepict1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    metafilepict1.dump();
    // End of writing test for METAFILEPICT

    // Begin of writing test for OBJHEADER
    std::cout << "Testing writing for OBJHEADER: ";
    OBJHEADER objheader1;
    // Initilaize the struct with random data
    tmp=sizeof(OBJHEADER)/sizeof(int);
    ptr=reinterpret_cast<int*>( &objheader1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(OBJHEADER) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(objheader1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for OBJHEADER

    // Begin of writing test for OLST
    std::cout << "Testing writing for OLST: ";
    OLST olst1;
    // Initilaize the struct with random data
    tmp=sizeof(OLST)/sizeof(int);
    ptr=reinterpret_cast<int*>( &olst1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(OLST) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(olst1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    olst1.dump();
    // End of writing test for OLST

    // Begin of writing test for PCD
    std::cout << "Testing writing for PCD: ";
    PCD pcd1;
    // Initilaize the struct with random data
    tmp=sizeof(PCD)/sizeof(int);
    ptr=reinterpret_cast<int*>( &pcd1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(PCD) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(pcd1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for PCD

    // Begin of writing test for PGD
    std::cout << "Testing writing for PGD: ";
    PGD pgd1;
    // Initilaize the struct with random data
    tmp=sizeof(PGD)/sizeof(int);
    ptr=reinterpret_cast<int*>( &pgd1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(PGD) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(pgd1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for PGD

    // Begin of writing test for PICF
    std::cout << "Testing writing for PICF: ";
    PICF picf1;
    // Initilaize the struct with random data
    tmp=sizeof(PICF)/sizeof(int);
    ptr=reinterpret_cast<int*>( &picf1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(PICF) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(picf1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    picf1.dump();
    // End of writing test for PICF

    // Begin of writing test for SED
    std::cout << "Testing writing for SED: ";
    SED sed1;
    // Initilaize the struct with random data
    tmp=sizeof(SED)/sizeof(int);
    ptr=reinterpret_cast<int*>( &sed1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(SED) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(sed1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for SED

    std::cout << "Testing writing for SEPX:" << std::endl;
    std::cout << "  Sorry, testing dynamic structures isn't implemented,"
              << " yet." << std::endl;
    // Begin of writing test for STSHI
    std::cout << "Testing writing for STSHI: ";
    STSHI stshi1;
    // Initilaize the struct with random data
    tmp=sizeof(STSHI)/sizeof(int);
    ptr=reinterpret_cast<int*>( &stshi1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(STSHI) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(stshi1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for STSHI


    // Okay, close the stream writer and open it for reading...
    int position=writer->tell();  // store the position for a check
    delete writer;
    storage.close();
    if(!storage.open(OLEStorage::ReadOnly)) {
        std::cout << "Error: Couldn't open the storage!" << std::endl;
        ::exit(1);
    }
    OLEStreamReader *reader=storage.createStreamReader("TestStream");
    if(!reader || !reader->isValid()) {
        std::cout << "Error: Couldn't open a stream for reading!" << std::endl;
        ::exit(1);
    }

    // Begin of reading test for DTTM
    std::cout << "Testing reading for DTTM: ";
    DTTM dttm2;
    // Read the data from the stream
    if(!dttm2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    DTTM dttm3(dttm2);
    if(dttm1==dttm2 && dttm2==dttm3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for DTTM

    // Begin of reading test for PRM2
    std::cout << "Testing reading for PRM2: ";
    PRM2 prm22;
    // Read the data from the stream
    if(!prm22.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    PRM2 prm23(prm22);
    if(prm21==prm22 && prm22==prm23)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for PRM2

    // Begin of reading test for PRM
    std::cout << "Testing reading for PRM: ";
    PRM prm2;
    // Read the data from the stream
    if(!prm2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    PRM prm3(prm2);
    if(prm1==prm2 && prm2==prm3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for PRM

    // Begin of reading test for SHD
    std::cout << "Testing reading for SHD: ";
    SHD shd2;
    // Read the data from the stream
    if(!shd2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    SHD shd3(shd2);
    if(shd1==shd2 && shd2==shd3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for SHD

    // Begin of reading test for PHE
    std::cout << "Testing reading for PHE: ";
    PHE phe2;
    // Read the data from the stream
    if(!phe2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    PHE phe3(phe2);
    if(phe1==phe2 && phe2==phe3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for PHE

    // Begin of reading test for BRC
    std::cout << "Testing reading for BRC: ";
    BRC brc2;
    // Read the data from the stream
    if(!brc2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    BRC brc3(brc2);
    if(brc1==brc2 && brc2==brc3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for BRC

    // Begin of reading test for TLP
    std::cout << "Testing reading for TLP: ";
    TLP tlp2;
    // Read the data from the stream
    if(!tlp2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    TLP tlp3(tlp2);
    if(tlp1==tlp2 && tlp2==tlp3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for TLP

    // Begin of reading test for TC
    std::cout << "Testing reading for TC: ";
    TC tc2;
    // Read the data from the stream
    if(!tc2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    TC tc3(tc2);
    if(tc1==tc2 && tc2==tc3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for TC

    // Begin of reading test for DPHEAD
    std::cout << "Testing reading for DPHEAD: ";
    DPHEAD dphead2;
    // Read the data from the stream
    if(!dphead2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    DPHEAD dphead3(dphead2);
    if(dphead1==dphead2 && dphead2==dphead3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for DPHEAD

    // Begin of reading test for DPTXBX
    std::cout << "Testing reading for DPTXBX: ";
    DPTXBX dptxbx2;
    // Read the data from the stream
    if(!dptxbx2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    DPTXBX dptxbx3(dptxbx2);
    if(dptxbx1==dptxbx2 && dptxbx2==dptxbx3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for DPTXBX

    std::cout << "Testing reading for DPPOLYLINE:" << std::endl;
    std::cout << "  Sorry, testing dynamic structures isn't implemented,"
              << " yet." << std::endl;
    // Begin of reading test for ANLD
    std::cout << "Testing reading for ANLD: ";
    ANLD anld2;
    // Read the data from the stream
    if(!anld2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    ANLD anld3(anld2);
    if(anld1==anld2 && anld2==anld3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for ANLD

    // Begin of reading test for ANLV
    std::cout << "Testing reading for ANLV: ";
    ANLV anlv2;
    // Read the data from the stream
    if(!anlv2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    ANLV anlv3(anlv2);
    if(anlv1==anlv2 && anlv2==anlv3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for ANLV

    // Begin of reading test for BKF
    std::cout << "Testing reading for BKF: ";
    BKF bkf2;
    // Read the data from the stream
    if(!bkf2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    BKF bkf3(bkf2);
    if(bkf1==bkf2 && bkf2==bkf3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for BKF

    // Begin of reading test for BKL
    std::cout << "Testing reading for BKL: ";
    BKL bkl2;
    // Read the data from the stream
    if(!bkl2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    BKL bkl3(bkl2);
    if(bkl1==bkl2 && bkl2==bkl3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for BKL

    // Begin of reading test for BRC10
    std::cout << "Testing reading for BRC10: ";
    BRC10 brc102;
    // Read the data from the stream
    if(!brc102.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    BRC10 brc103(brc102);
    if(brc101==brc102 && brc102==brc103)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for BRC10

    // Begin of reading test for BTE
    std::cout << "Testing reading for BTE: ";
    BTE bte2;
    // Read the data from the stream
    if(!bte2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    BTE bte3(bte2);
    if(bte1==bte2 && bte2==bte3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for BTE

    // Begin of reading test for CHP
    std::cout << "Testing reading for CHP: ";
    CHP chp2;
    // Read the data from the stream
    if(!chp2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    CHP chp3(chp2);
    if(chp1==chp2 && chp2==chp3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for CHP

    // Begin of reading test for DCS
    std::cout << "Testing reading for DCS: ";
    DCS dcs2;
    // Read the data from the stream
    if(!dcs2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    DCS dcs3(dcs2);
    if(dcs1==dcs2 && dcs2==dcs3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for DCS

    // Begin of reading test for DO
    std::cout << "Testing reading for DO: ";
    DO do2;
    // Read the data from the stream
    if(!do2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    DO do3(do2);
    if(do1==do2 && do2==do3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for DO

    // Begin of reading test for DOP
    std::cout << "Testing reading for DOP: ";
    DOP dop2;
    // Read the data from the stream
    if(!dop2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    DOP dop3(dop2);
    if(dop1==dop2 && dop2==dop3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for DOP

    // Begin of reading test for DPARC
    std::cout << "Testing reading for DPARC: ";
    DPARC dparc2;
    // Read the data from the stream
    if(!dparc2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    DPARC dparc3(dparc2);
    if(dparc1==dparc2 && dparc2==dparc3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for DPARC

    // Begin of reading test for DPELLIPSE
    std::cout << "Testing reading for DPELLIPSE: ";
    DPELLIPSE dpellipse2;
    // Read the data from the stream
    if(!dpellipse2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    DPELLIPSE dpellipse3(dpellipse2);
    if(dpellipse1==dpellipse2 && dpellipse2==dpellipse3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for DPELLIPSE

    // Begin of reading test for DPLINE
    std::cout << "Testing reading for DPLINE: ";
    DPLINE dpline2;
    // Read the data from the stream
    if(!dpline2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    DPLINE dpline3(dpline2);
    if(dpline1==dpline2 && dpline2==dpline3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for DPLINE

    // Begin of reading test for DPRECT
    std::cout << "Testing reading for DPRECT: ";
    DPRECT dprect2;
    // Read the data from the stream
    if(!dprect2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    DPRECT dprect3(dprect2);
    if(dprect1==dprect2 && dprect2==dprect3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for DPRECT

    // Begin of reading test for DPSAMPLE
    std::cout << "Testing reading for DPSAMPLE: ";
    DPSAMPLE dpsample2;
    // Read the data from the stream
    if(!dpsample2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    DPSAMPLE dpsample3(dpsample2);
    if(dpsample1==dpsample2 && dpsample2==dpsample3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for DPSAMPLE

    // Begin of reading test for FDOA
    std::cout << "Testing reading for FDOA: ";
    FDOA fdoa2;
    // Read the data from the stream
    if(!fdoa2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    FDOA fdoa3(fdoa2);
    if(fdoa1==fdoa2 && fdoa2==fdoa3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for FDOA

    // Begin of reading test for FIB
    std::cout << "Testing reading for FIB: ";
    FIB fib2;
    // Read the data from the stream
    if(!fib2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    FIB fib3(fib2);
    if(fib1==fib2 && fib2==fib3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for FIB

    // Begin of reading test for LSPD
    std::cout << "Testing reading for LSPD: ";
    LSPD lspd2;
    // Read the data from the stream
    if(!lspd2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    LSPD lspd3(lspd2);
    if(lspd1==lspd2 && lspd2==lspd3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for LSPD

    // Begin of reading test for METAFILEPICT
    std::cout << "Testing reading for METAFILEPICT: ";
    METAFILEPICT metafilepict2;
    // Read the data from the stream
    if(!metafilepict2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    METAFILEPICT metafilepict3(metafilepict2);
    if(metafilepict1==metafilepict2 && metafilepict2==metafilepict3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for METAFILEPICT

    // Begin of reading test for OBJHEADER
    std::cout << "Testing reading for OBJHEADER: ";
    OBJHEADER objheader2;
    // Read the data from the stream
    if(!objheader2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    OBJHEADER objheader3(objheader2);
    if(objheader1==objheader2 && objheader2==objheader3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for OBJHEADER

    // Begin of reading test for OLST
    std::cout << "Testing reading for OLST: ";
    OLST olst2;
    // Read the data from the stream
    if(!olst2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    OLST olst3(olst2);
    if(olst1==olst2 && olst2==olst3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for OLST

    // Begin of reading test for PCD
    std::cout << "Testing reading for PCD: ";
    PCD pcd2;
    // Read the data from the stream
    if(!pcd2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    PCD pcd3(pcd2);
    if(pcd1==pcd2 && pcd2==pcd3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for PCD

    // Begin of reading test for PGD
    std::cout << "Testing reading for PGD: ";
    PGD pgd2;
    // Read the data from the stream
    if(!pgd2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    PGD pgd3(pgd2);
    if(pgd1==pgd2 && pgd2==pgd3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for PGD

    // Begin of reading test for PICF
    std::cout << "Testing reading for PICF: ";
    PICF picf2;
    // Read the data from the stream
    if(!picf2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    PICF picf3(picf2);
    if(picf1==picf2 && picf2==picf3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for PICF

    // Begin of reading test for SED
    std::cout << "Testing reading for SED: ";
    SED sed2;
    // Read the data from the stream
    if(!sed2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    SED sed3(sed2);
    if(sed1==sed2 && sed2==sed3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for SED

    std::cout << "Testing reading for SEPX:" << std::endl;
    std::cout << "  Sorry, testing dynamic structures isn't implemented,"
              << " yet." << std::endl;
    // Begin of reading test for STSHI
    std::cout << "Testing reading for STSHI: ";
    STSHI stshi2;
    // Read the data from the stream
    if(!stshi2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    STSHI stshi3(stshi2);
    if(stshi1==stshi2 && stshi2==stshi3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for STSHI


    if(position!=reader->tell())
        std::cout << "Error: Different amount of bytes read/written!" << std::endl;
    delete reader;

    std::cout << "Done." << std::endl;
    // Clean up
    storage.close();
}
