/*
 * KLoala
 *
 * based on "LAOLA file system"
 *          "Structured Storage"
 *          (c) 1996, 1997 by martin schwartz@cs.tu-berlin.de
 */

#ifndef KLAOLA_H
#define KLAOLA_H

#include <string.h>
#include <qstring.h>
#include <qarray.h>
#include <qlist.h>
#include <kdebug.h>
#include <myfile.h>

// If you fetch this struct, you will get all the available
// OLE-Info for the corresponding stream.
// Normally you won't use it.
struct OLEInfo {
    long handle;        // PPS entry number
    QString name;       // Name of the stream
    short nameSize;     // Size of the name
    char type;          // Type of pps 
    long prev;          // Last pps
    long next;          // Next pps
    long dir;           // Dir pps
    long ts1s;          // Timestamp 1, seconds
    long ts1d;          // Timestamp 1, days
    long ts2s;          // Timestamp 2, seconds
    long ts2d;          // Timestamp 2, days
    long sb;            // Starting block
    long size;          // Size of property
};

// A little bit of OLE-Information. If you want to navigate through
// the "filesystem" you have to use this struct.
struct OLENode {
    long handle;
    QString name;
    char type;       // 1=Dir, 2=File/Stream, 5=Root Entry
};

// For internal use only!
struct OLETree {
    long handle;
    short subtree;
};


class KLaola {

public:
    KLaola(myFile file);               // see myfile.h!
    ~KLaola();


    QList<OLENode> parseRootDir();
    QList<OLENode> parseCurrentDir();

    bool enterDir(long handle);
    bool leaveDir();
    QArray<long> currentPath();

    OLEInfo streamInfo(long handle);
    QString stream(long handle);       // Note: 512 byte blocks!

    void testIt();                     // dump some info (similar to "lls"
                                       // of the LAOLA-project), TODO
 
private:
    bool parseHeader();
    void readBigBlockDepot();
    void readSmallBlockDepot();
    void readSmallBlockFile();
    void readRootList();
    void readPPSEntry(long pos, long handle);
    void createTree(long handle, short index);
    unsigned char *readBBStream(long start);
    unsigned char *readSBStream(long start);

    inline long nextBigBlock(long pos);
    inline long nextSmallBlock(long pos);
    inline unsigned short read16(int i);
    inline unsigned long read32(int i);

    QList<QList<OLETree> > treeList;
    QList<OLEInfo> ppsList;
    QArray<long> path;

    unsigned char *data;
    unsigned char *bigBlockDepot;
    unsigned char *smallBlockDepot;
    unsigned char *smallBlockFile;

    unsigned long maxblock;          // maximum number of blocks
    unsigned long num_of_bbd_blocks; // number of big block depot blocks
    unsigned long root_startblock;   // Root chain's first big block
    unsigned long sbd_startblock;    // small block depot's first big block
    unsigned long bbd_list[436]; //array of num_of_bbd_blocks big block numbers
};
#endif // KLAOLA_H
