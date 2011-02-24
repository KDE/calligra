// This file contains generated testing code. We do some basic tests
// like writing and reading from/to streams. Of course these tests are
// neither complete nor very advanced, so watch out :)

#include <word97_generated.h>
#include <olestream.h>
#include <iostream>
#include <stdlib.h>   // rand(), srand()

#include <time.h>   // time()

using namespace wvWare;
using namespace Word97;

int main(int, char**) {

    // First we have to create some infrastructure
    system("rm word97_test.doc &> /dev/null");
    OLEStorage storage("word97_test.doc");
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
    std::cout << "Testing the Word97 structures..." << std::endl;
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

    // Begin of writing test for DOPTYPOGRAPHY
    std::cout << "Testing writing for DOPTYPOGRAPHY: ";
    DOPTYPOGRAPHY doptypography1;
    // Initilaize the struct with random data
    tmp=sizeof(DOPTYPOGRAPHY)/sizeof(int);
    ptr=reinterpret_cast<int*>( &doptypography1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(DOPTYPOGRAPHY) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(doptypography1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for DOPTYPOGRAPHY

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

    // Begin of writing test for ASUMY
    std::cout << "Testing writing for ASUMY: ";
    ASUMY asumy1;
    // Initilaize the struct with random data
    tmp=sizeof(ASUMY)/sizeof(int);
    ptr=reinterpret_cast<int*>( &asumy1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(ASUMY) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(asumy1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for ASUMY

    // Begin of writing test for ASUMYI
    std::cout << "Testing writing for ASUMYI: ";
    ASUMYI asumyi1;
    // Initilaize the struct with random data
    tmp=sizeof(ASUMYI)/sizeof(int);
    ptr=reinterpret_cast<int*>( &asumyi1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(ASUMYI) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(asumyi1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for ASUMYI

    // Begin of writing test for ATRD
    std::cout << "Testing writing for ATRD: ";
    ATRD atrd1;
    // Initilaize the struct with random data
    tmp=sizeof(ATRD)/sizeof(int);
    ptr=reinterpret_cast<int*>( &atrd1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(ATRD) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(atrd1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for ATRD

    // Begin of writing test for BKD
    std::cout << "Testing writing for BKD: ";
    BKD bkd1;
    // Initilaize the struct with random data
    tmp=sizeof(BKD)/sizeof(int);
    ptr=reinterpret_cast<int*>( &bkd1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(BKD) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(bkd1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for BKD

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

    // Begin of writing test for DOGRID
    std::cout << "Testing writing for DOGRID: ";
    DOGRID dogrid1;
    // Initilaize the struct with random data
    tmp=sizeof(DOGRID)/sizeof(int);
    ptr=reinterpret_cast<int*>( &dogrid1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(DOGRID) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(dogrid1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for DOGRID

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

    // Begin of writing test for FIBFCLCB
    std::cout << "Testing writing for FIBFCLCB: ";
    FIBFCLCB fibfclcb1;
    // Initilaize the struct with random data
    tmp=sizeof(FIBFCLCB)/sizeof(int);
    ptr=reinterpret_cast<int*>( &fibfclcb1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(FIBFCLCB) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(fibfclcb1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for FIBFCLCB

    // Begin of writing test for FRD
    std::cout << "Testing writing for FRD: ";
    FRD frd1;
    // Initilaize the struct with random data
    tmp=sizeof(FRD)/sizeof(int);
    ptr=reinterpret_cast<int*>( &frd1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(FRD) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(frd1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for FRD

    // Begin of writing test for FSPA
    std::cout << "Testing writing for FSPA: ";
    FSPA fspa1;
    // Initilaize the struct with random data
    tmp=sizeof(FSPA)/sizeof(int);
    ptr=reinterpret_cast<int*>( &fspa1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(FSPA) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(fspa1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for FSPA

    // Begin of writing test for FTXBXS
    std::cout << "Testing writing for FTXBXS: ";
    FTXBXS ftxbxs1;
    // Initilaize the struct with random data
    tmp=sizeof(FTXBXS)/sizeof(int);
    ptr=reinterpret_cast<int*>( &ftxbxs1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(FTXBXS) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(ftxbxs1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for FTXBXS

    // Begin of writing test for LFO
    std::cout << "Testing writing for LFO: ";
    LFO lfo1;
    // Initilaize the struct with random data
    tmp=sizeof(LFO)/sizeof(int);
    ptr=reinterpret_cast<int*>( &lfo1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(LFO) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(lfo1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for LFO

    // Begin of writing test for LFOLVL
    std::cout << "Testing writing for LFOLVL: ";
    LFOLVL lfolvl1;
    // Initilaize the struct with random data
    tmp=sizeof(LFOLVL)/sizeof(int);
    ptr=reinterpret_cast<int*>( &lfolvl1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(LFOLVL) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(lfolvl1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for LFOLVL

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

    // Begin of writing test for LSTF
    std::cout << "Testing writing for LSTF: ";
    LSTF lstf1;
    // Initilaize the struct with random data
    tmp=sizeof(LSTF)/sizeof(int);
    ptr=reinterpret_cast<int*>( &lstf1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(LSTF) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(lstf1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for LSTF

    // Begin of writing test for LVLF
    std::cout << "Testing writing for LVLF: ";
    LVLF lvlf1;
    // Initilaize the struct with random data
    tmp=sizeof(LVLF)/sizeof(int);
    ptr=reinterpret_cast<int*>( &lvlf1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(LVLF) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(lvlf1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for LVLF

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

    // Begin of writing test for NUMRM
    std::cout << "Testing writing for NUMRM: ";
    NUMRM numrm1;
    // Initilaize the struct with random data
    tmp=sizeof(NUMRM)/sizeof(int);
    ptr=reinterpret_cast<int*>( &numrm1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(NUMRM) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(numrm1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    numrm1.dump();
    // End of writing test for NUMRM

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

    // Begin of writing test for PHE2
    std::cout << "Testing writing for PHE2: ";
    PHE2 phe21;
    // Initilaize the struct with random data
    tmp=sizeof(PHE2)/sizeof(int);
    ptr=reinterpret_cast<int*>( &phe21 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(PHE2) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(phe21.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for PHE2

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

    // Begin of writing test for RR
    std::cout << "Testing writing for RR: ";
    RR rr1;
    // Initilaize the struct with random data
    tmp=sizeof(RR)/sizeof(int);
    ptr=reinterpret_cast<int*>( &rr1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(RR) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(rr1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for RR

    // Begin of writing test for RS
    std::cout << "Testing writing for RS: ";
    RS rs1;
    // Initilaize the struct with random data
    tmp=sizeof(RS)/sizeof(int);
    ptr=reinterpret_cast<int*>( &rs1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(RS) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(rs1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for RS

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

    // Begin of writing test for WKB
    std::cout << "Testing writing for WKB: ";
    WKB wkb1;
    // Initilaize the struct with random data
    tmp=sizeof(WKB)/sizeof(int);
    ptr=reinterpret_cast<int*>( &wkb1 );
    for(int _i=0; _i<tmp; ++_i)
        *ptr++=rand();
    *ptr |= rand() & (0x00ffffff >> (((sizeof(int)-1)-(sizeof(WKB) % sizeof(int)))*8));  // yay! :)
    // and write it out...
    if(wkb1.write(writer, false))
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of writing test for WKB


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

    // Begin of reading test for DOPTYPOGRAPHY
    std::cout << "Testing reading for DOPTYPOGRAPHY: ";
    DOPTYPOGRAPHY doptypography2;
    // Read the data from the stream
    if(!doptypography2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    DOPTYPOGRAPHY doptypography3(doptypography2);
    if(doptypography1==doptypography2 && doptypography2==doptypography3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for DOPTYPOGRAPHY

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

    // Begin of reading test for ASUMY
    std::cout << "Testing reading for ASUMY: ";
    ASUMY asumy2;
    // Read the data from the stream
    if(!asumy2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    ASUMY asumy3(asumy2);
    if(asumy1==asumy2 && asumy2==asumy3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for ASUMY

    // Begin of reading test for ASUMYI
    std::cout << "Testing reading for ASUMYI: ";
    ASUMYI asumyi2;
    // Read the data from the stream
    if(!asumyi2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    ASUMYI asumyi3(asumyi2);
    if(asumyi1==asumyi2 && asumyi2==asumyi3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for ASUMYI

    // Begin of reading test for ATRD
    std::cout << "Testing reading for ATRD: ";
    ATRD atrd2;
    // Read the data from the stream
    if(!atrd2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    ATRD atrd3(atrd2);
    if(atrd1==atrd2 && atrd2==atrd3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for ATRD

    // Begin of reading test for BKD
    std::cout << "Testing reading for BKD: ";
    BKD bkd2;
    // Read the data from the stream
    if(!bkd2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    BKD bkd3(bkd2);
    if(bkd1==bkd2 && bkd2==bkd3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for BKD

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

    // Begin of reading test for DOGRID
    std::cout << "Testing reading for DOGRID: ";
    DOGRID dogrid2;
    // Read the data from the stream
    if(!dogrid2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    DOGRID dogrid3(dogrid2);
    if(dogrid1==dogrid2 && dogrid2==dogrid3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for DOGRID

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

    // Begin of reading test for FIBFCLCB
    std::cout << "Testing reading for FIBFCLCB: ";
    FIBFCLCB fibfclcb2;
    // Read the data from the stream
    if(!fibfclcb2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    FIBFCLCB fibfclcb3(fibfclcb2);
    if(fibfclcb1==fibfclcb2 && fibfclcb2==fibfclcb3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for FIBFCLCB

    // Begin of reading test for FRD
    std::cout << "Testing reading for FRD: ";
    FRD frd2;
    // Read the data from the stream
    if(!frd2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    FRD frd3(frd2);
    if(frd1==frd2 && frd2==frd3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for FRD

    // Begin of reading test for FSPA
    std::cout << "Testing reading for FSPA: ";
    FSPA fspa2;
    // Read the data from the stream
    if(!fspa2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    FSPA fspa3(fspa2);
    if(fspa1==fspa2 && fspa2==fspa3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for FSPA

    // Begin of reading test for FTXBXS
    std::cout << "Testing reading for FTXBXS: ";
    FTXBXS ftxbxs2;
    // Read the data from the stream
    if(!ftxbxs2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    FTXBXS ftxbxs3(ftxbxs2);
    if(ftxbxs1==ftxbxs2 && ftxbxs2==ftxbxs3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for FTXBXS

    // Begin of reading test for LFO
    std::cout << "Testing reading for LFO: ";
    LFO lfo2;
    // Read the data from the stream
    if(!lfo2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    LFO lfo3(lfo2);
    if(lfo1==lfo2 && lfo2==lfo3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for LFO

    // Begin of reading test for LFOLVL
    std::cout << "Testing reading for LFOLVL: ";
    LFOLVL lfolvl2;
    // Read the data from the stream
    if(!lfolvl2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    LFOLVL lfolvl3(lfolvl2);
    if(lfolvl1==lfolvl2 && lfolvl2==lfolvl3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for LFOLVL

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

    // Begin of reading test for LSTF
    std::cout << "Testing reading for LSTF: ";
    LSTF lstf2;
    // Read the data from the stream
    if(!lstf2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    LSTF lstf3(lstf2);
    if(lstf1==lstf2 && lstf2==lstf3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for LSTF

    // Begin of reading test for LVLF
    std::cout << "Testing reading for LVLF: ";
    LVLF lvlf2;
    // Read the data from the stream
    if(!lvlf2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    LVLF lvlf3(lvlf2);
    if(lvlf1==lvlf2 && lvlf2==lvlf3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for LVLF

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

    // Begin of reading test for NUMRM
    std::cout << "Testing reading for NUMRM: ";
    NUMRM numrm2;
    // Read the data from the stream
    if(!numrm2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    NUMRM numrm3(numrm2);
    if(numrm1==numrm2 && numrm2==numrm3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for NUMRM

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

    // Begin of reading test for PHE2
    std::cout << "Testing reading for PHE2: ";
    PHE2 phe22;
    // Read the data from the stream
    if(!phe22.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    PHE2 phe23(phe22);
    if(phe21==phe22 && phe22==phe23)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for PHE2

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

    // Begin of reading test for RR
    std::cout << "Testing reading for RR: ";
    RR rr2;
    // Read the data from the stream
    if(!rr2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    RR rr3(rr2);
    if(rr1==rr2 && rr2==rr3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for RR

    // Begin of reading test for RS
    std::cout << "Testing reading for RS: ";
    RS rs2;
    // Read the data from the stream
    if(!rs2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    RS rs3(rs2);
    if(rs1==rs2 && rs2==rs3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for RS

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

    // Begin of reading test for WKB
    std::cout << "Testing reading for WKB: ";
    WKB wkb2;
    // Read the data from the stream
    if(!wkb2.read(reader, false))
        std::cout << "Failed. " << std::endl;
    // Test the copy CTOR
    WKB wkb3(wkb2);
    if(wkb1==wkb2 && wkb2==wkb3)
        std::cout << "Passed." << std::endl;
    else
        std::cout << "Failed." << std::endl;
    // End of reading test for WKB


    if(position!=reader->tell())
        std::cout << "Error: Different amount of bytes read/written!" << std::endl;
    delete reader;

    std::cout << "Done." << std::endl;
    // Clean up
    storage.close();
}
