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
#include <qfile.h>
#include <qstrlist.h>
#include <qlist.h>
#include <kdebug.h>
#include "myfile.h"


struct OLEInfo {
	short handle;       // An internal number :)
	char name[32];      // Name of the Stream
	short nameSize;     // Size of the Name
	char type;          // Type of pps 
	long prev;          // Last pps
	long next;          // Next pps
	long dir;           // Dir pps
	long ts1s;          // Timestamp 1, seconds
	long ts1d;          // Timestamp 1, days
	long ts2s;          // Timestamp 2, seconds
	long ts2d;          // Timestamp 2, days
	long sb;            // Starting Block
	long size;          // Size of Property	
};

struct OLEStream {
	long size;          // Size of the stream
	char *stream;       // The Data. Note: This is a potential
	                    // memory-leak! Please ensure that you
	                    // delete [] stream; stream=0L; somewhere!
};


class KLaola {

public:
	KLaola(myFile _file);
	~KLaola();

	QStrList streamNames();            // all the Stream-Names, TODO
	OLEInfo streamInfo(QString name);  // fetch info for a specific Stream
	                                   // this info is from the property
	                                   // storage area, TODO
	OLEStream stream(QString name);    // get the Stream, TODO
 
private:
	bool parseHeader();
	void readBigBlockDepot();
	void readSmallBlockDepot();
	void readSmallBlockFile();
	void parseNames();             // TODO
	void readRootList();           // TODO
	
	unsigned char *readBBStream(long start);
	
	inline unsigned short read16(int i);
	inline unsigned long read32(int i);

	QStrList names;
	QList<OLEInfo> streamInfoList;
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
