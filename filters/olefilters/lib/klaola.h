/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <wtrobin@carinthia.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

// KLaola is the class which is used to decode the OLE 2 streams.

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
    bool deadDir;    // true, if the dir is a "dead end"
};


class KLaola {

public:
    KLaola(const myFile &file);               // see myfile.h!
    ~KLaola();

    const bool isOk() {return ok;}

    const QList<OLENode> parseRootDir();
    const QList<OLENode> parseCurrentDir();

    // Wade through the "file system"
    const bool enterDir(const long &handle);
    const bool leaveDir();
    const QArray<long> currentPath() const;

    const OLEInfo streamInfo(const long &handle);
    const myFile stream(const long &handle);    // Note: data - 512 byte blocks, but
                                                // length is set correctly :)
                                                // Make sure that you delete [] the data!
    const QArray<long> find(const QString &name, const bool onlyCurrentDir=false);

    void testIt();                     // dump some info (similar to "lls"
                                       // of the LAOLA-project)

private:
    KLaola(const KLaola &);
    const KLaola &operator=(const KLaola &);

    struct OLETree {
        long handle;
        short subtree;
    };

    const bool parseHeader();
    void readBigBlockDepot();
    void readSmallBlockDepot();
    void readSmallBlockFile();
    void readRootList();
    void readPPSEntry(const long &pos, const long &handle);
    void createTree(const long &handle, const short &index);
    const unsigned char *readBBStream(const long &start, const bool setmaxSblock=false);
    const unsigned char *readSBStream(const long &start);

    inline const long nextBigBlock(const long &pos);
    inline const long nextSmallBlock(const long &pos);
    inline const unsigned short read16(const long &i);
    inline const unsigned long read32(const long &i);

    QList<QList<OLETree> > treeList;
    QList<OLEInfo> ppsList;
    QArray<long> path;

    bool ok;        // is the file OK?

    unsigned char *data;
    unsigned char *bigBlockDepot;
    unsigned char *smallBlockDepot;
    unsigned char *smallBlockFile;

    unsigned long maxblock;          // maximum number of big-blocks
    unsigned long maxSblock;         //                   small-blocks
    unsigned long num_of_bbd_blocks; // number of big block depot blocks
    unsigned long root_startblock;   // Root chain's first big block
    unsigned long sbd_startblock;    // small block depot's first big block
    unsigned long *bbd_list;  //array of num_of_bbd_blocks big block numbers
};
#endif // KLAOLA_H
