/* FemtoZip - superlightweight C++ module to create a compressed ZIP archive
   Copyright (C) 2005-2006 Ariya Hidayat <ariya@kde.org>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   * Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   * Neither the name of the authors nor the names of its contributors may be
     used to endorse or promote products derived from this software without
     specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
   THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef USE_GSF_OUTPUT

#include "FemtoZip.hxx"

#include <stdlib.h>  // malloc, free, realoc
#include <stdio.h>   // printf (debug mode)
#include <string.h>  // memcpy, memset, strlen
#include <time.h>    // time, localtime, time_t

// uses small (16 entries) CRC table instead of the large (256 entries) one
// very useful if memory requirement is critical
#define FEMTOZIP_SMALLTABLE

#ifdef DEBUG
#define FEMTOZIP_DEBUG
#endif

#ifdef FEMTOZIP_DEBUG
#define FZ_DEBUG(M) printf M
#else
#define FZ_DEBUG(M)
#endif

// see http://www.w3.org/TR/PNG-CRCAppendix.html on info regarding CRC-32

#ifdef FEMTOZIP_SMALLTABLE
static unsigned long crc_table[16] = {
	0x00000000L, 0x1db71064L, 0x3b6e20c8L, 0x26d930acL,
	0x76dc4190L, 0x6b6b51f4L, 0x4db26158L, 0x5005713cL,
	0xedb88320L, 0xf00f9344L, 0xd6d6a3e8L, 0xcb61b38cL,
	0x9b64c2b0L, 0x86d3d2d4L, 0xa00ae278L, 0xbdbdf21cL };
#else
static unsigned long crc_table[256] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
	0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
	0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
	0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
	0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
	0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
	0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
	0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
	0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
	0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
	0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
	0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
	0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
	0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
	0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
	0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
	0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
	0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
	0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
	0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
	0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
	0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
	0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
	0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
	0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
	0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
	0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
	0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
	0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
	0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
	0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
	0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
	0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
	0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
	0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
	0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
	0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
	0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
	0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
	0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
	0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
	0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
	0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
	0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
	0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
	0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
	0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
	0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
	0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
	0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
	0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
	0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL };
#endif


#ifdef FEMTOZIP_DEBUG
static void hexdump(unsigned long len, const void* data)
{
	const unsigned char* ptr = (const unsigned char*) data;
	printf("%ld bytes:\n", len);
	while(len)
	{
		unsigned long j = (len > 16) ? 16 : len;
		for(unsigned i = 0; i < 16; i++)
	        if( i < j ) printf("%02x ", ptr[i] );
			else printf("   ");
		printf("    ");
		for(unsigned i = 0; i < 16; i++ )
			if( i < j ) printf("%c", (ptr[i] > 31) ? ptr[i] : '.');
			else printf("   ");
		printf("\n");
		len -= j;
		ptr += j;
	}
	printf("\n");
}
#endif


// see http://www.concentric.net/~Ttwang/tech/inthash.htm
static inline unsigned short hash(unsigned short key, unsigned short hashSize)
{
	key += ~(key << 15);
	key ^=  (key >> 10);
	key +=  (key << 3);
	key ^=  (key >> 6);
	key += ~(key << 11);
	key ^=  (key >> 16);
	return key % hashSize;
}


// entry record is stored as linked list

class FemtoZipEntry
{
public:
	char* name;
	int compressionLevel;
	unsigned long uncompressedSize;
	unsigned long compressedSize;
	unsigned long headerPos;
	unsigned short timeStamp;
	unsigned short dateStamp;
	unsigned long crc32;
	FemtoZipEntry* next;

	FemtoZipEntry(const char* n, int cl):
	compressionLevel(cl), uncompressedSize(0), compressedSize(0),
	headerPos(0), timeStamp(0), dateStamp(0),
	crc32(0xffffffffL),   // pre-condition, all bits are '1'
	next(0)
	{
		name = strdup(n);
	}

	~FemtoZipEntry()
	{
		free(name);
	}
};

class FemtoZipPrivate
{
public:
	int errorCode;
	FILE* fhandle;
	FemtoZipEntry* entryList;
	FemtoZipEntry* currentEntry;
	unsigned char buffer[46];

	FemtoZipPrivate(): errorCode(FemtoZip::NoError), fhandle(0),
	entryList(0), currentEntry(0)
	{
	}

	void createZip(const char* zipfile)
	{
		FZ_DEBUG(("Creating ZIP: %s\n", zipfile));

		fhandle = fopen(zipfile, "wb");
		if(!fhandle)
			errorCode = FemtoZip::ErrorCreateZip;
	}

	unsigned long updateCRC(unsigned long crc, const void *data, unsigned long len)
	{
		const unsigned char* buf = (const unsigned char*) data;

		for(unsigned long i = 0; i < len; i++)
#ifdef FEMTOZIP_SMALLTABLE
		{
			crc ^= buf[i];
			crc = crc_table[crc & 0x0f] ^ (crc >> 4);
			crc = crc_table[crc & 0x0f] ^ (crc >> 4);
		}
#else
			crc = crc_table[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
#endif
		return crc;
	}

	// write central directory, by walking the linked list
	// also: free the entry one by one
	void closeZip()
	{
		// last entry not closed
		if(currentEntry)
		{
			FZ_DEBUG(("automatically closing last entry %s", currentEntry->name));
			closeEntry();
		}

		unsigned long centralDirPos = ftell(fhandle);
		unsigned short entryCount = 0;
		FemtoZipEntry* entry = entryList;
		while(entry && (errorCode == FemtoZip::NoError))
		{
			FZ_DEBUG(("central directory: entry for %s\n", entry->name));
			size_t namelen = strlen(entry->name);

			/*
			central directory record: total size >= 46 bytes

			offset  size   description
			---------------------------------------
			  0      4    signature: 'P','K',1,2
			  4      2    version made by
			  6      2    version needed to extract
			  8      2    general purpose bit flag
			 10      2    compression method 0=store 8=deflate
			 12      2    last mod file time
			 14      2    last mod file date
			 16      4    crc-32
			 20      4    compressed size
			 24      4    uncompressed size
			 28      2    file name length
			 30      2    extra field length
			 32      2    file comment length
			 34      2    disk number start
			 36      2    internal file attributes
			 38      4    external file attributes
			 42      4    relative offset of local header
			*/

			buffer[0] = 0x50;
			buffer[1] = 0x4b;
			buffer[2] = 0x01;
			buffer[3] = 0x02;
			buffer[4] = 0x14;
			buffer[5] = 0;
			buffer[6] = 0x0a;
			buffer[7] = 0;
			buffer[8] = 0;
			buffer[9] = 0;
			buffer[10] = (entry->compressionLevel==0) ? 0 : 8;
			buffer[11] = 0x0;
			buffer[12] = entry->timeStamp & 0xff;
			buffer[13] = (entry->timeStamp>>8) & 0xff;
			buffer[14] = entry->dateStamp & 0xff;
			buffer[15] = (entry->dateStamp>>8) & 0xff;
			buffer[16] = entry->crc32 & 0xff;
			buffer[17] = (entry->crc32>>8) & 0xff;
			buffer[18] = (entry->crc32>>16) & 0xff;
			buffer[19] = (entry->crc32>>24) & 0xff;
			buffer[20] = entry->compressedSize & 0xff;
			buffer[21] = (entry->compressedSize>>8) & 0xff;
			buffer[22] = (entry->compressedSize>>16) & 0xff;
			buffer[23] = (entry->compressedSize>>24) & 0xff;
			buffer[24] = entry->uncompressedSize & 0xff;
			buffer[25] = (entry->uncompressedSize>>8) & 0xff;
			buffer[26] = (entry->uncompressedSize>>16) & 0xff;
			buffer[27] = (entry->uncompressedSize>>24) & 0xff;
			buffer[28] = namelen & 0xff;
			buffer[29] = (namelen>>8) & 0xff;
			buffer[30] = 0;
			buffer[31] = 0;
			buffer[32] = 0;
			buffer[33] = 0;
			buffer[34] = 0;
			buffer[35] = 0;
			buffer[36] = 0;
			buffer[37] = 0;
			buffer[38] = 0;
			buffer[39] = 0;
			buffer[40] = 0;
			buffer[41] = 0;
			buffer[42] = entry->headerPos & 0xff;
			buffer[43] = (entry->headerPos>>8) & 0xff;
			buffer[44] = (entry->headerPos>>16) & 0xff;
			buffer[45] = (entry->headerPos>>24) & 0xff;

			if(fwrite(buffer, 1, 46, fhandle) != 46)
				errorCode = FemtoZip::ErrorWriteData;
			else
			{
				if(fwrite(entry->name, 1, namelen, fhandle) != namelen)
					errorCode = FemtoZip::ErrorWriteData;
			}

			FemtoZipEntry* next_entry = entry->next;
			delete entry;
			entry = next_entry;
			entryCount++;
		}
		unsigned long centralDirSize = ftell(fhandle) - centralDirPos;

		/*
		end of central directory record: total size 22 bytes

		offset  size   description
		---------------------------------------
		  0      4    signature: 'P','K',5,6
		  4      2    number of this disk
		  6      2    number of this disk with the start of the central directory
		  8      2    total number of entries in the central directory on this disk
		 10      2    total number of entries in the central directory
		 12      4    size of the central directory
		 16      4    offset of start of central directory with respect to the starting disk number
		 20      2    comment length
		 22     var.  comment field
		*/

		buffer[0] = 'P';
		buffer[1] = 'K';
		buffer[2] = 5;
		buffer[3] = 6;
		buffer[4] = 0x00;
		buffer[5] = 0x00;
		buffer[6] = 0x00;
		buffer[7] = 0x00;
		buffer[8] = entryCount & 0xff;
		buffer[9] = (entryCount >> 8) & 0xff;
		buffer[10] = entryCount & 0xff;
		buffer[11] = (entryCount >> 8) & 0xff;
		buffer[12] = centralDirSize & 0xff;
		buffer[13] = (centralDirSize>>8) & 0xff;
		buffer[14] = (centralDirSize>>16) & 0xff;
		buffer[15] = (centralDirSize>>24) & 0xff;
		buffer[16] = centralDirPos & 0xff;
		buffer[17] = (centralDirPos>>8) & 0xff;
		buffer[18] = (centralDirPos>>16) & 0xff;
		buffer[19] = (centralDirPos>>24) & 0xff;
		buffer[20] = 0x00;
		buffer[21] = 0x00;
		fwrite(buffer, 1, 22, fhandle);

		// that's all, we're done !
		fclose(fhandle);
		fhandle = 0;
	}

	void writeLocalHeader(FemtoZipEntry* entry)
	{
		if(!entry) return;

		size_t namelen = strlen(entry->name);

		/*
		local file header: total size 22 bytes

		offset  size   description
		---------------------------------------
		  0      4    signature: 'P','K',3,4
		  4      2    version needed to extract
		  6      2    general purpose bit flag
		  8      2    compression method 0=store 8=deflate
		 10      2    last mod file time
		 12      2    last mod file date
		 14      4    crc-32
		 18      4    compressed size
		 22      4    uncompressed size
		 26      2    file name length
		 28      2    extra field length
		 30     var.  file name (NOT null terminated)
		*/

		buffer[0] = 'P';
		buffer[1] = 'K';
		buffer[2] = 3;
		buffer[3] = 4;
		buffer[4] = 0x0a;
		buffer[5] = 0;
		buffer[6] = 0;
		buffer[7] = 0;
		buffer[8] = (currentEntry->compressionLevel==0) ? 0 : 8;
		buffer[9] = 0;
		buffer[10] = currentEntry->timeStamp & 0xff;
		buffer[11] = (currentEntry->timeStamp>>8) & 0xff;
		buffer[12] = currentEntry->dateStamp & 0xff;
		buffer[13] = (currentEntry->dateStamp>>8) & 0xff;
		buffer[14] = currentEntry->crc32 & 0xff;
		buffer[15] = (currentEntry->crc32>>8) & 0xff;
		buffer[16] = (currentEntry->crc32>>16) & 0xff;
		buffer[17] = (currentEntry->crc32>>24) & 0xff;
		buffer[18] = currentEntry->compressedSize & 0xff;
		buffer[19] = (currentEntry->compressedSize>>8) & 0xff;
		buffer[20] = (currentEntry->compressedSize>>16) & 0xff;
		buffer[21] = (currentEntry->compressedSize>>24) & 0xff;
		buffer[22] = currentEntry->uncompressedSize & 0xff;
		buffer[23] = (currentEntry->uncompressedSize>>8) & 0xff;
		buffer[24] = (currentEntry->uncompressedSize>>16) & 0xff;
		buffer[25] = (currentEntry->uncompressedSize>>24) & 0xff;
		buffer[26] = namelen & 0xff;
		buffer[27] = (namelen>>8) & 0xff;
		buffer[28] = 0;
		buffer[29] = 0;

		if(fwrite(buffer, 1, 30, fhandle) != 30)
			errorCode = FemtoZip::ErrorWriteData;
		else
		{
			if(fwrite(entry->name, 1, namelen, fhandle) != namelen)
				errorCode = FemtoZip::ErrorWriteData;
		}
	}


	void createEntry(const char* name, int compressionLevel)
	{
		if(errorCode != FemtoZip::NoError)
			return;

		if(fhandle == 0)
		{
			FZ_DEBUG(("createEntry error: file already closed"));
			return;
		}

		// somebody forget to close previous entry
		if(currentEntry != 0)
		{
			FZ_DEBUG(("createEntry warning: previous entry is not manually closed"));
			closeEntry();
		}

		// sanity check
		if(compressionLevel < 0)
			compressionLevel = 0;

		// create new entry and append it to the linked list
		currentEntry = new FemtoZipEntry(name, compressionLevel);
		if(entryList != 0)
		{
			FemtoZipEntry* e = entryList;
			while(e->next)
				e = e->next;
			e->next = currentEntry;
		}
		else
			entryList = currentEntry;

		// anything to do with time stamp
		time_t currentEpoch;
		time(&currentEpoch);
		struct tm* currentTime = localtime(&currentEpoch);

		currentEntry->timeStamp =
			((currentTime->tm_hour & 31) << 11) | // 5-bit hours
			((currentTime->tm_min & 63) << 5) |   // 6-bit minutes
			((currentTime->tm_sec & 31)*2);       // 5-bit seconds*2

		if(currentTime->tm_year >= 80)
			// note: year is using 1980 as reference
			currentEntry->dateStamp =
				(((currentTime->tm_year-80) & 127) << 9) |  // 7-bit years
				(((currentTime->tm_mon+1) & 15) << 5) |     // 4-bit months
				(currentTime->tm_mday & 31);                // 5-bit days
		else
			// before 1980, can't be represented so set to Jan 1, 1980
			currentEntry->dateStamp = (1 << 5) | 1;

		// because we need to "fix" the header in closeEntry()
		currentEntry->headerPos = ftell(fhandle);

		writeLocalHeader(currentEntry);
	}

	void closeEntry()
	{
		if(errorCode != FemtoZip::NoError)
			return;

		if(fhandle == 0)
		{
			FZ_DEBUG(("closeEntry error: file already closed"));
			return;
		}

		if(!currentEntry)
		{
			FZ_DEBUG(("closeEntry error: no entry is opened"));
			return;
		}

		currentEntry->compressedSize = currentEntry->uncompressedSize;

		// post-condition for the CRC-32, i.e. one's complement
		currentEntry->crc32 ^= 0xffffffffL;

		// we need to come back again here later on
		long filepos = ftell(fhandle);

		// "fix" the local header in the right position
		fseek(fhandle, currentEntry->headerPos, SEEK_SET);
		writeLocalHeader(currentEntry);

		// back to where we left
		fseek(fhandle, filepos, SEEK_SET);

		// that's all for this one
		currentEntry = 0;
	}

	void writeData(unsigned long len, const void* data)
	{
		if(errorCode != FemtoZip::NoError)
			return;

		if(fhandle == 0)
		{
			FZ_DEBUG(("writeData error: file already closed"));
			return;
		}

		if(!currentEntry)
		{
			FZ_DEBUG(("writeData error: no entry is opened"));
			return;
		}

		currentEntry->crc32 = updateCRC(currentEntry->crc32, data, len);
		currentEntry->uncompressedSize += len;

		// method: Store
		if(fwrite(data, 1, len, fhandle) != len)
			errorCode = FemtoZip::ErrorWriteData;
	}
};





FemtoZip::FemtoZip(const char* zipfile)
{
	d = new FemtoZipPrivate;
	d->createZip(zipfile);
}

FemtoZip::~FemtoZip()
{
	d->closeZip();
	delete d;
}

void FemtoZip::createEntry(const char* name, int compressionLevel)
{
	d->createEntry(name, compressionLevel);
}

void FemtoZip::writeString(const char* str)
{
	d->writeData(strlen(str), str);
}

void FemtoZip::closeEntry()
{
	d->closeEntry();
}

int FemtoZip::errorCode() const
{
	return d->errorCode;
}

#endif
