/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "CmxImport.h"
#include "GGroup.h"
#include "GPolyline.h"
#include "GOval.h"
#include "GPolygon.h"
#include "GText.h"
#include <limits.h>
#include <math.h>
#include <algorithm>

#include <qfile.h>

// typedefs for CMX

typedef unsigned char Byte;
typedef double DOUBLE;
typedef Q_UINT16 UINT16;
typedef Q_UINT32 UINT32;
typedef Q_INT32 INT32;

struct CmxHeader {
  Byte Id[32];
  Byte OS[16];
  Byte ByteOrder[4];
  Byte CoordSize[2];
  Byte Major[4];
  Byte Minor[4];
  UINT16 Unit;
  DOUBLE Factor;
  UINT32 IOption;
  UINT32 IForeignKey;
  UINT32 ICapability;
  INT32 IndexSection;
  INT32 InfoSection;
  INT32 IBBLeft;
  INT32 IBBTop;
  INT32 IBBRight;
  INT32 IBBBottom;
  INT32 IThumbnail;
  INT32 ITally;
  Byte Reserved[64];
};

struct MasterIndexHeader {
  Byte Id[4];
  Byte Unused[4];
  INT16 MasterID;
  INT16 Size;
  INT16 RecordCount;
};

struct PageIndexHeader {
  Byte Id[4];
  Byte Unused[4];
  INT16 RecordCount;
};

#define Cmx_BeginPage  9
#define Cmx_EndPage   10
#define Cmx_PolyCurve 67
 
CmxImport::CmxImport () {
}

CmxImport::~CmxImport () {
}

bool CmxImport::setup (GDocument* , const char* /*format*/) {
  return true;
}

bool CmxImport::importFromFile (GDocument *doc) {
  char riffHeader[19];
  struct CmxHeader cmxHeader;

  QFile file (inputFileName ());
  if (! file.open (IO_ReadOnly))
    return false;

  QDataStream strm (&file);
  strm.readRawBytes (riffHeader, 20);
  if (strncmp (riffHeader, "RIFF", 4) != 0 ||
      strncmp (&riffHeader[8], "CMX1", 4) != 0) {
    file.close ();
    return false;
  }

  cout << "CMX file format identified !!" << endl;
  strm.readRawBytes ((char *) &cmxHeader, 32+16+4+2+4+4);
  cout << "id = ";
  for (int i = 0; i < 32; i++)
    cout << cmxHeader.Id[i];
  cout << endl;
  cout << "byteorder = " 
       << cmxHeader.ByteOrder[0] /*<< cmxHeader.ByteOrder[1]*/ << endl;
  cout << "coordsize = " 
       << cmxHeader.CoordSize[0] << cmxHeader.CoordSize[1] << endl;
  strm.setByteOrder ((cmxHeader.ByteOrder[0] == '2' ? 
		      QDataStream::LittleEndian :
		      QDataStream::BigEndian));
  cout << strm.byteOrder () << endl;
  strm >> cmxHeader.Unit; 
  strm >> cmxHeader.Factor;
  cout << "unit = " << cmxHeader.Unit << endl;
  cout << "factor = " << cmxHeader.Factor << endl;
  strm >> cmxHeader.IOption 
       >> cmxHeader.IForeignKey
       >> cmxHeader.ICapability
       >> cmxHeader.IndexSection
       >> cmxHeader.InfoSection
       >> cmxHeader.IBBLeft >> cmxHeader.IBBTop
       >> cmxHeader.IBBRight >> cmxHeader.IBBBottom
       >> cmxHeader.IThumbnail 
       >> cmxHeader.ITally;
  cout << "offset to Index Section = " << cmxHeader.IndexSection << endl;
  cout << unit2pt (cmxHeader.IBBLeft) << ", " << unit2pt (cmxHeader.IBBTop)
       << " - " << unit2pt (cmxHeader.IBBRight) << ", " 
       << unit2pt (cmxHeader.IBBBottom) << endl;
  cout << "instructions = " << cmxHeader.ITally << endl;

  file.at (cmxHeader.IndexSection);
  struct MasterIndexHeader midxHeader;
  strm.readRawBytes ((char *) &midxHeader, 4+4);
  if (strncmp ((const char *) midxHeader.Id, "ixmr", 4) != 0) {
    file.close ();
    return false;
  }
  strm >> midxHeader.MasterID
       >> midxHeader.Size
       >> midxHeader.RecordCount;
  cout << "RecordCount = " << midxHeader.RecordCount 
       << " (" << midxHeader.MasterID << ", " << midxHeader.Size 
       << ")" << endl;
  for (int i = 0; i < midxHeader.RecordCount; i++) {
    INT16 recordID;
    INT32 offset;
    strm >> recordID >> offset;
    cout << "Record: " << recordID << " (" << offset << ")" << endl;
    if (recordID == 2) {
      file.at (offset);
      if (readPageIndex (strm, doc)) {
	file.close ();
	return false;
      }
    }
  }
  file.close ();
  return true;
}

bool CmxImport::readPageIndex (QDataStream& strm, GDocument *doc) {
  struct PageIndexHeader pidxHeader;
  strm.readRawBytes ((char *) &pidxHeader, 4+4);
  if (strncmp ((const char *) pidxHeader.Id, "ixpg", 4) != 0)
    return false;
  strm >> pidxHeader.RecordCount;
  cout << "RecordCount = " << pidxHeader.RecordCount << endl;
  for (int i = 0; i < pidxHeader.RecordCount; i++) {
    INT16 Size;
    INT32 PageOffset, LayerTableOffset, ThumbnailOffset, RefListOffset;
    strm >> Size >> PageOffset 
	 >> LayerTableOffset >> ThumbnailOffset
	 >> RefListOffset;
    skipBytes (strm, Size - sizeof (INT32) * 4);
    cout << "PageOffset: " << PageOffset << endl;
    QFile *file = (QFile *) strm.device ();
    file->at (PageOffset);
    if (! readPageSection (strm, doc))
      return false;
  }
  return true;
}

bool CmxImport::readPageSection (QDataStream& strm, GDocument *doc) {
  Byte Header[8];
  bool endOfPage = false;
  INT16 InstrSize, InstrCode;

  strm.readRawBytes ((char *) Header, 8);
  if (strncmp ((const char *) Header, "page", 4) != 0)
    return false;

  while (! endOfPage) {
    int len = sizeof (INT16) * 2;
    strm >> InstrSize >> InstrCode;
    switch (InstrCode) {
    case Cmx_BeginPage:
      cout << "Begin of Page !" << endl;
      break;
    case Cmx_EndPage:
      cout << "End of Page !" << endl;
      endOfPage = true;
      break;
    case Cmx_PolyCurve:
      len += readPolyCurve (strm, doc);
      break;
    default:
      cout << "Instruction = " << InstrCode << endl;
      break;
    }
    skipBytes (strm, InstrSize - len);
  }
  return true;
}

void CmxImport::skipBytes (QDataStream& strm, int len) {
  Q_INT8 dummy;
  for (int i = 0; i < len; i++)
    strm >> dummy;
}

int CmxImport::readPolyCurve (QDataStream& strm, GDocument *doc) {
  Byte TagID;
  UINT16 TagSize;
  int numBytes = 0;
  int i;

  cout << "parse PolyCurve" << endl;
  // Rendering Attributes
  strm >> TagID >> TagSize;
  cout << "TagID (1) = " << (int) TagID << endl;
  // ignore the rendering attributes now
  skipBytes (strm, TagSize - 3);
  numBytes += TagSize;

  // Point List
  strm >> TagID >> TagSize;
  cout << "TagID (2) = " << (int) TagID << endl;
  INT16 Count;
  strm >> Count;
  cout << "Count = " << Count << endl;
  for (i = 0; i < Count; i++) {
    INT32 x, y;
    float xp, yp;
    strm >> x >> y;
    xp = unit2pt (x);
    yp = unit2pt (y);
    cout << "  #" << i << ": " << xp << ", " << yp << endl;
  }
  for (i = 0; i < Count; i++) {
    Byte node;
    strm >> node;
    cout << "  #" << i << ": " << (int) node << endl;
  }
  //  skipBytes (strm, TagSize - 5);
  numBytes += TagSize;

  // Bounding Box
  strm >> TagID >> TagSize;
  cout << "TagID (3) = " << (int) TagID << endl;
  skipBytes (strm, TagSize - 3);
  numBytes += TagSize;

  // End Tag
  strm >> TagID >> TagSize;
  cout << "EndTag (255) = " << (int) TagID << endl;
  numBytes += TagSize;

  return numBytes;
}

float CmxImport::unit2pt (float v) {
  return v * 72.0 / 254000.0;
} 

