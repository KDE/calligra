/*
 * KLoala
 *
 * based on "LAOLA file system"
 *          "Structured Storage"
 *          (c) 1996, 1997 by martin schwartz@cs.tu-berlin.de
 */

#include "klaola.h"
#include <iostream>

KLaola::KLaola(myFile _file) {
	
	smallBlockDepot=0L;
	bigBlockDepot=0L;
	smallBlockFile=0L;

	if( (_file.length % 0x200) != 0 ) {
		kdebug(KDEBUG_ERROR, 31000, "Invalid file size!");
		exit(-1);
	}
	
	data=_file.data;
	maxblock = _file.length / 0x200 - 2;
	
	if(!parseHeader())
		exit(-1);
	readBigBlockDepot();
	readSmallBlockDepot();
	readSmallBlockFile();
	readRootList();
	parseNames();
}

KLaola::~KLaola() {

	if(bigBlockDepot) {
		delete [] bigBlockDepot;
		bigBlockDepot=0L;
	}	
	if(smallBlockDepot) {
		delete [] smallBlockDepot;
		smallBlockDepot=0L;
	}
	if(smallBlockFile) {
		delete [] smallBlockFile;
		smallBlockFile=0L;
	}
}

QStrList KLaola::streamNames() {
	return names;
}

OLEInfo KLaola::streamInfo(QString name) {
	OLEInfo tmp;
	return tmp;
}

OLEStream KLaola::stream(QString name) {
	OLEStream tmp;
	return tmp;
}

bool KLaola::parseHeader() {

	if(qstrncmp((const char*)data,"\xd0\xcf\x11\xe0\xa1\xb1\x1a\xe1",8 )!=0) {
		kdebug(KDEBUG_ERROR, 31000, 
			   "Invalid file format (unexpected id in header)!");
		return false;
	}

	num_of_bbd_blocks=read32(0x2c);
	root_startblock=read32(0x30);
	sbd_startblock=read32(0x3c);

	for(unsigned int i=0;i<num_of_bbd_blocks;++i)
		bbd_list[i]=read32(0x4c+i);

	return true;
}

void KLaola::readBigBlockDepot() {

	bigBlockDepot=new unsigned char[0x200*num_of_bbd_blocks];
	
	for(unsigned int i=0;i<num_of_bbd_blocks;++i) 
		memcpy(&bigBlockDepot[i*0x200], &data[(bbd_list[i]+1)*0x200], 0x200);
}

void KLaola::readSmallBlockDepot() {
	smallBlockDepot=readBBStream(sbd_startblock);
}

void KLaola::readSmallBlockFile() {	
	smallBlockFile=readBBStream( read32( (root_startblock+1)*0x200 + 0x74) );
}

void KLaola::parseNames() {
}

void KLaola::readRootList() {
}

unsigned char *KLaola::readBBStream(long start) {

	long i=0, x, tmp;
	unsigned char *p=0;
	QString foo;

	tmp=start;

	kdebug(KDEBUG_INFO, 31000, "in");
	foo.setNum(tmp);
	kdebug(KDEBUG_INFO, 31000, (const char*)foo);
	
	while(tmp!=-2 && tmp>=0 && tmp<=(long)maxblock) {
		++i;
		x=tmp*4;
		tmp=(bigBlockDepot[x+3] << 24) +
			(bigBlockDepot[x+2] << 16) +
			(bigBlockDepot[x+1] << 8) +
			bigBlockDepot[x];
		foo.setNum(tmp);
		kdebug(KDEBUG_INFO, 31000, (const char*)foo);
	}

	if(i!=0) {
		p=new unsigned char[i*0x200];
		
		i=0;
		tmp=start;
		while(tmp!=-2 && tmp>=0 && tmp<=(long)maxblock) {
			memcpy(&p[i*0x200], &data[(tmp+1)*0x200], 0x200);
			x=tmp*4;
			tmp=(bigBlockDepot[x+3] << 24) +
				(bigBlockDepot[x+2] << 16) +
				(bigBlockDepot[x+1] << 8) +
				bigBlockDepot[x];
			++i;
		}
	}
	kdebug(KDEBUG_INFO, 31000, "out");
	return p;
}

inline unsigned short KLaola::read16(int i) {
	return ( (data[i+1] << 8) + data[i] );
}

inline unsigned long KLaola::read32(int i) {
	return ( (read16(i+2) << 16) + read16(i) );
}
