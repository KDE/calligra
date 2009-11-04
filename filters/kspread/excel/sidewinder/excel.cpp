/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003-2005 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA
*/

#include "excel.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <stdlib.h>
#include <string.h>

#include "pole.h"
#include "swinder.h"
#include "globalssubstreamhandler.h"
#include "worksheetsubstreamhandler.h"
#include "utils.h"

// Use anonymous namespace to cover following functions
namespace{

// RK value is special encoded integer or floating-point
// see any documentation of Excel file format for detail description
static inline void decodeRK( unsigned rkvalue, bool& isInteger,
  int& i, double& f )
{
  double factor = (rkvalue & 0x01) ? 0.01 : 1;
  if( rkvalue & 0x02 )
  {
    // FIXME check that int is 32 bits ?
    isInteger = true;
    i = *((int*) &rkvalue) >> 2;
    if (rkvalue & 0x01)
    {
      if (i%100 == 0)
      {
        i /= 100;
      } else {
        isInteger = false;
        f = i * 0.01;
      }
    }
  }
  else
  {
    // TODO ensure double takes 8 bytes
    isInteger = false;
    unsigned char* s = (unsigned char*) &rkvalue;
    unsigned char* r = (unsigned char*) &f;
    if( Swinder::isLittleEndian() )
    {
      r[0] = r[1] = r[2] = r[3] = 0;
      r[4] = s[0] & 0xfc;
      r[5] = s[1]; r[6] = s[2];  r[7] = s[3];
    }
    else
    {
      r[0] = r[1] = r[2] = r[3] = 0;
      r[4] = s[0] & 0xfc;
      r[5] = s[1]; r[6] = s[2];  r[7] = s[3];
    }
    memcpy( &f, r, 8 );
    f *= factor;
  }
}

}

namespace Swinder
{
static inline std::ostream& operator<<( std::ostream& s, Swinder::UString ustring )
{
  char* str = ustring.ascii();
  s << str;
  return s;
}

}

using namespace Swinder;

//=============================================
//          EString
//=============================================


class EString::Private
{
public:
  bool unicode;
  bool richText;
  UString str;
  unsigned size;
};

EString::EString()
{
  d = new EString::Private();
  d->unicode  = false;
  d->richText = false;
  d->str      = UString::null;
  d->size     = 0;
}

EString::EString( const EString& es )
{
  d = new EString::Private();
  operator=( es );
}

EString& EString::operator=( const EString& es )
{
  d->unicode  = es.d->unicode;
  d->richText = es.d->richText;
  d->size     = es.d->size;
  d->str      = es.d->str;
  return *this;
}

EString::~EString()
{
  delete d;
}

bool EString::unicode() const
{
  return d->unicode;
}

void EString::setUnicode( bool u )
{
  d->unicode = u;
}

bool EString::richText() const
{
  return d->richText;
}

void EString::setRichText( bool r )
{
  d->richText = r;
}

UString EString::str() const
{
  return d->str;
}

void EString::setStr( const UString& str )
{
  d->str = str;
}

unsigned EString::size() const
{
  return d->size;
}

void EString::setSize( unsigned s )
{
  d->size = s;
}

// FIXME use maxsize for sanity check
EString EString::fromUnicodeString( const void* p, bool longString, unsigned /* maxsize */, unsigned continuePosition )
{
  const unsigned char* data = (const unsigned char*) p;
  UString str = UString::null;

  unsigned offset = longString ? 2 : 1;
  unsigned len = longString ? readU16( data  ): data[0];
  unsigned char flag = data[ offset ];
  offset++; // for flag (1 byte)

  bool unicode = flag & 0x01;
  bool asianPhonetics = flag & 0x04;
  bool richText = flag & 0x08;
  unsigned formatRuns = 0;
  unsigned asianPhoneticsSize = 0;

  if( richText )
  {
    formatRuns = readU16( data + offset );
    offset += 2;
  }

  if( asianPhonetics )
  {
    asianPhoneticsSize = readU32( data + offset );
    offset += 4;
  }

  // find out total bytes used in this string
  unsigned size = offset;
  if( richText ) size += (formatRuns*4);
  if( asianPhonetics ) size += asianPhoneticsSize;

  str = UString();
  for( unsigned k=0; k<len; k++ )
  {
    unsigned uchar;
    if( unicode ) {
      uchar = readU16( data + offset );
      offset += 2;
      size += 2;
    } else {
      uchar = data[offset++];
      size++;
    }
    str.append( UString(UChar(uchar)) );
    if( offset == continuePosition && k < len-1 )
    {
      unicode = data[offset] & 1;
      size++;
      offset++;
    }
  }

  EString result;
  result.setUnicode( unicode );
  result.setRichText( richText );
  result.setSize( size );
  result.setStr( str );

  return result;
}

// FIXME use maxsize for sanity check
EString EString::fromByteString( const void* p, bool longString,
  unsigned /* maxsize */ )
{
  const unsigned char* data = (const unsigned char*) p;
  UString str = UString::null;

  unsigned offset = longString ? 2 : 1;
  unsigned len = longString ? readU16( data  ): data[0];

  char* buffer = new char[ len+1 ];
  memcpy( buffer, data + offset, len );
  buffer[ len ] = 0;
  str = UString( buffer );
  delete[] buffer;

  unsigned size = offset + len;

  EString result;
  result.setUnicode( false );
  result.setRichText( false );
  result.setSize( size );
  result.setStr( str );

  return result;
}



// why different ? see BoundSheetRecord
EString EString::fromSheetName( const void* p, unsigned datasize )
{
  const unsigned char* data = (const unsigned char*) p;
  UString str = UString::null;

  bool richText = false;
  // unsigned formatRuns = 0;

  unsigned len = data[0];
  unsigned flag = data[1];
  bool unicode = flag & 1;

  if( len > datasize-2 ) len = datasize-2;
  if( len == 0 ) return EString();

  unsigned offset = 2;

  if( !unicode )
  {
    char* buffer = new char[ len+1 ];
    memcpy( buffer, data + offset, len );
    buffer[ len ] = 0;
    str = UString( buffer );
    delete[] buffer;
  }
  else
  {
    for( unsigned k=0; k<len; k++ )
    {
      unsigned uchar = readU16( data + offset + k*2 );
      str.append( UString(UChar(uchar)) );
    }
  }

  EString result;
  result.setUnicode( unicode );
  result.setRichText( richText );
  result.setSize( datasize );
  result.setStr( str );

  return result;
}

//=============================================
//          CellInfo
//=============================================

class CellInfo::Private
{
public:
  unsigned row;
  unsigned column;
  unsigned xfIndex;
};

CellInfo::CellInfo()
{
  info = new CellInfo::Private();
  info->row = 0;
  info->column = 0;
  info->xfIndex = 0;
}

CellInfo::~CellInfo()
{
  delete info;
}

unsigned CellInfo::row() const
{
  return info->row;
}

void CellInfo::setRow( unsigned r )
{
  info->row = r;
}

unsigned CellInfo::column() const
{
  return info->column;
}

void CellInfo::setColumn( unsigned c )
{
  info->column = c;
}

unsigned CellInfo::xfIndex() const
{
  return info->xfIndex;
}

void CellInfo::setXfIndex( unsigned i )
{
  info->xfIndex = i;
}

//=============================================
//          ColumnSpanInfo
//=============================================

class ColumnSpanInfo::Private
{
public:
  unsigned firstColumn;
  unsigned lastColumn;
};

ColumnSpanInfo::ColumnSpanInfo()
{
  spaninfo = new ColumnSpanInfo::Private();
  spaninfo->firstColumn = 0;
  spaninfo->lastColumn = 0;
}

ColumnSpanInfo::~ColumnSpanInfo()
{
  delete spaninfo;
}

unsigned ColumnSpanInfo::firstColumn() const
{
  return spaninfo->firstColumn;
}

void ColumnSpanInfo::setFirstColumn( unsigned c )
{
  spaninfo->firstColumn = c;
}

unsigned ColumnSpanInfo::lastColumn() const
{
  return spaninfo->lastColumn;
}

void ColumnSpanInfo::setLastColumn( unsigned c )
{
  spaninfo->lastColumn = c;
}

// ========== base record ==========

const unsigned int Record::id = 0; // invalid of-course

Record::Record()
{
  stream_position = 0;
  ver = Excel97;
}

Record::~Record()
{
}

Record* Record::create( unsigned type )
{
  if( type ==BOFRecord::id)
    return new BOFRecord();

  else if( type ==EOFRecord::id)
    return new EOFRecord();

  else if( type ==BackupRecord::id)
    return new BackupRecord();

  else if( type ==BlankRecord::id)
    return new BlankRecord();

  else if( type ==BoolErrRecord::id)
    return new BoolErrRecord();

  else if( type ==BottomMarginRecord::id)
    return new BottomMarginRecord();

  else if( type ==BoundSheetRecord::id)
    return new BoundSheetRecord();

  else if( type ==CalcModeRecord::id)
    return new CalcModeRecord();

  else if( type ==ColInfoRecord::id)
    return new ColInfoRecord();

  else if( type ==DataTableRecord::id)
    return new DataTableRecord();

  else if( type ==DateModeRecord::id)
    return new DateModeRecord();

  else if( type ==DimensionRecord::id)
    return new DimensionRecord();

  else if( type ==ExternBookRecord::id)
    return new ExternBookRecord();

  else if( type ==ExternNameRecord::id)
    return new ExternNameRecord();

  else if( type ==ExternSheetRecord::id)
    return new ExternSheetRecord();

  else if( type ==FilepassRecord::id)
    return new FilepassRecord();

  else if( type ==FontRecord::id)
    return new FontRecord();

  else if( type ==FooterRecord::id)
    return new FooterRecord();

  else if( type ==FormatRecord::id)
    return new FormatRecord();

  else if( type ==FormulaRecord::id)
    return new FormulaRecord();

  else if( type ==HeaderRecord::id)
    return new HeaderRecord();

  else if( type ==LabelRecord::id)
    return new LabelRecord();

  else if( type ==LabelSSTRecord::id)
    return new LabelSSTRecord();

  else if( type ==LeftMarginRecord::id)
    return new LeftMarginRecord();

  else if( type ==MergedCellsRecord::id)
    return new MergedCellsRecord();

  else if( type ==MulBlankRecord::id)
    return new MulBlankRecord();

  else if( type ==MulRKRecord::id)
    return new MulRKRecord();

  else if( type ==NameRecord::id)
    return new NameRecord();

  else if( type ==NumberRecord::id)
    return new NumberRecord();

  else if( type ==PaletteRecord::id)
    return new PaletteRecord();

  else if( type ==RightMarginRecord::id)
    return new RightMarginRecord();

  else if( type ==RKRecord::id)
    return new RKRecord();

  else if( type ==RowRecord::id)
    return new RowRecord();

  else if( type ==RStringRecord::id)
    return new RStringRecord();

  else if( type ==SharedFormulaRecord::id)
    return new SharedFormulaRecord();

  else if( type ==SSTRecord::id)
    return new SSTRecord();

  else if( type ==StringRecord::id)
    return new StringRecord();

  else if( type ==XFRecord::id)
    return new XFRecord();

  else if( type ==TopMarginRecord::id)
    return new TopMarginRecord();

  return 0;
}

void Record::setPosition( unsigned pos )
{
  stream_position = pos;
}

unsigned Record::position() const
{
  return stream_position;
}

void Record::setData( unsigned, const unsigned char*, const unsigned int* )
{
}

void Record::dump( std::ostream& ) const
{
  // nothing to dump
}

// ========== BACKUP ==========

const unsigned int BackupRecord::id = 0x0040;

class BackupRecord::Private
{
public:
  bool backup;
};

BackupRecord::BackupRecord():
  Record()
{
  d = new BackupRecord::Private();
  d->backup = false;
}

BackupRecord::~BackupRecord()
{
  delete d;
}

bool BackupRecord::backup() const
{
  return d->backup;
}

void BackupRecord::setBackup( bool b )
{
  d->backup = b;
}

void BackupRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 2 ) return;

  unsigned flag = readU16( data );
  d->backup = flag != 0;
}

void BackupRecord::dump( std::ostream& out ) const
{
  out << "BACKUP" << std::endl;
  out << "     Backup on save : " << (backup() ? "Yes" : "No") << std::endl;
}

// ========== BLANK ==========

const unsigned int BlankRecord::id = 0x0201;

BlankRecord::BlankRecord():
  Record(), CellInfo()
{
}

void BlankRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 6 ) return;

  setRow( readU16( data ) );
  setColumn( readU16( data+2 ) );
  setXfIndex( readU16( data+4 ) );
}

void BlankRecord::dump( std::ostream& out ) const
{
  out << "BLANK" << std::endl;
  out << "                Row : " << row() << std::endl;
  out << "             Column : " << column() << std::endl;
  out << "           XF Index : " << xfIndex() << std::endl;
}


// ========== BOF ==========

const unsigned int BOFRecord::id = 0x0809;

// helper class for BOFRecord
class BOFRecord::Private
{
public:
  unsigned version;  // 0x0500=Excel95, 0x0600=Excel97, and so on
  unsigned type;
  unsigned build;
  unsigned year;
  unsigned history;
  unsigned rversion;
};

// constructor of BOFRecord
BOFRecord::BOFRecord():
  Record()
{
  d = new BOFRecord::Private();
  d->version  = 0x600; // BIFF8;
  d->type     = 0;
  d->build    = 0;
  d->year     = 0;
  d->history  = 0;
  d->rversion = 0;
}

// destructor of BOFRecord
BOFRecord::~BOFRecord()
{
  delete d;
}

void BOFRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 4 ) return;

  d->version  = readU16( data );
  d->type     = readU16( data+2 );
  if( size > 6 )
  {
    d->build    = readU16( data+4 );
    d->year     = readU16( data+6);
    if( size > 12 )
    {
      d->history  = readU32( data+8 );
      d->rversion = readU32( data+12 );
    }
  }
}

unsigned BOFRecord::version() const
{
  unsigned ver = UnknownExcel;
  switch( d->version )
  {
    case 0x0500 : ver = Excel95; break;
    case 0x0600 : ver = Excel97; break;
    default: break;
  }
  return ver;
}

const char* BOFRecord::versionAsString() const
{
  const char *result = "Unknown";
  switch( version() )
  {
    case Excel95 : result = "Excel95"; break;
    case Excel97 : result = "Excel97"; break;
    default: break;
  }
  return result;
}

unsigned BOFRecord::type() const
{
  unsigned result = UnknownType;
  switch( d->type )
  {
    case 0x005  : result = Workbook; break;
    case 0x006  : result = VBModule; break;
    case 0x010  : result = Worksheet; break;
    case 0x020  : result = Chart; break;
    case 0x040  : result = MacroSheet; break;
    case 0x100  : result = Workspace; break;
    default: break;
  }
  return result;
}

const char* BOFRecord::typeAsString() const
{
  const char *result = "Unknown";
  switch( type() )
  {
    case Workbook   : result = "Workbook"; break;
    case VBModule   : result = "Visual Basic Module"; break;
    case Worksheet  : result = "Worksheet"; break;
    case Chart      : result = "Chart"; break;
    case MacroSheet : result = "Macro Sheet"; break;
    case Workspace  : result = "Workspace File"; break;
    default: break;
  }
  return result;
}

void BOFRecord::dump( std::ostream& out ) const
{
  out << "BOF" << std::endl;
  out << "            Version : 0x" << std::hex << d->version << " (" << versionAsString() << ")" << std::endl;
  out << "               Type : 0x" << d->type << " (" << typeAsString() << ")" << std::endl;
  out << "              Build : 0x" << d->build << std::endl;
  out << "               Year : " << std::dec << d->year << std::endl;
  out << "            History : 0x" << std::hex << d->history << std::endl;
  out << "           RVersion : 0x" << d->rversion << std::endl;
  out << std::dec;
}

// ========== BOOLERR ==========

const unsigned int BoolErrRecord::id = 0x0205;

class BoolErrRecord::Private
{
public:
  Value value;
};

BoolErrRecord::BoolErrRecord():
  Record(), CellInfo()
{
  d = new BoolErrRecord::Private();
  d->value = Value( false );
}

BoolErrRecord::~BoolErrRecord()
{
  delete d;
}

void BoolErrRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size != 8 ) return;

  setRow( readU16( data ) );
  setColumn( readU16( data+2 ) );
  setXfIndex( readU16( data+4 ) );

  switch( data[7] )
  {
  case 0 :
    d->value = Value( data[6] ? true : false );
    break;
  case 1 :
    d->value = errorAsValue( data[6] );
    break;
  default:
    // bad bad bad
    std::cerr << "Warning: bad BOOLERR record" << std::endl;
    break;
  }
}

Value BoolErrRecord::value() const
{
  return d->value;
}

void BoolErrRecord::dump( std::ostream& out ) const
{
  out << "BOOLERR" << std::endl;
  out << "             Column : " << column() << std::endl;
  out << "                Row : " << row() << std::endl;
  out << "            XFIndex : " << xfIndex() << std::endl;
  out << "              Value : " << value() << std::endl;
}

// ========== BOTTOMMARGIN ==========

const unsigned int BottomMarginRecord::id = 0x0029;

class BottomMarginRecord::Private
{
public:
  double bottomMargin;
};

BottomMarginRecord::BottomMarginRecord():
  Record()
{
  d = new BottomMarginRecord::Private();
  d->bottomMargin = 1.0;
}

BottomMarginRecord::~BottomMarginRecord()
{
  delete d;
}

double BottomMarginRecord::bottomMargin() const
{
  return d->bottomMargin;
}

void BottomMarginRecord::setBottomMargin( double m )
{
  d->bottomMargin = m;
}

void BottomMarginRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 8 ) return;
  setBottomMargin( readFloat64( data ) );
}

void BottomMarginRecord::dump( std::ostream& out ) const
{
  out << "BOTTOMMARGIN" << std::endl;
  out << "      Bottom Margin : " << bottomMargin() << " inches" << std::endl;
}


// ========== BOUNDSHEET ==========

const unsigned int BoundSheetRecord::id = 0x0085;

// helper class for BoundSheetRecord
class BoundSheetRecord::Private
{
public:
  unsigned type;  // 0=Worksheet, 2=Chart, 6=VB Module
  unsigned visibility; // 0=visible, 1=hidden, 2=strong hidden
  UString name;
  unsigned bofPosition;
};

BoundSheetRecord::BoundSheetRecord():
  Record()
{
  d = new BoundSheetRecord::Private();
  d->type = 0;
  d->visibility = 0;
  d->name = "Sheet";
}

void BoundSheetRecord::setType( unsigned t )
{
  switch( t )
  {
    case Worksheet: d->type = 0; break;
    case Chart:     d->type = 2; break;
    case VBModule:  d->type = 6; break;
    default: d->type = 0; break; // fallback
  };
}

unsigned BoundSheetRecord::type() const
{
  unsigned t = Worksheet;
  switch( d->type )
  {
    case 0: t = Worksheet; break;
    case 2: t = Chart; break;
    case 6: t = VBModule; break;
    default: break;
  };
  return t;
}

const char* BoundSheetRecord::typeAsString() const
{
  const char *result = "Unknown";
  switch( type() )
  {
    case Worksheet: result = "Worksheet"; break;
    case Chart:     result = "Chart"; break;
    case VBModule:  result = "Visual Basic Module"; break;
    default: break;
  }
  return result;
}

void BoundSheetRecord::setVisible( bool v )
{
  d->visibility = v ? 0 : 1;
}

bool BoundSheetRecord::visible() const
{
  return d->visibility == 0;
}

void BoundSheetRecord::setSheetName( const UString& n )
{
  d->name = n;
}

UString BoundSheetRecord::sheetName() const
{
  return d->name;
}

void BoundSheetRecord::setBofPosition( unsigned pos )
{
  d->bofPosition = pos;
}

unsigned BoundSheetRecord::bofPosition() const
{
  return d->bofPosition;
}

BoundSheetRecord::~BoundSheetRecord()
{
  delete d;
}

void BoundSheetRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 6 ) return;

  d->bofPosition = readU32( data );
  d->visibility = data[4];
  d->type = data[5];

  /* FIXME: it turned out that sheet name is not normal unicode string
     where the first two bytes specifies string length, but instead
     only the first specifies it.
     the next byte could be correctly interpreted as flag.
   */

  UString name = ( version() >= Excel97 ) ?
    EString::fromSheetName( data+6, size-6 ).str() :
    EString::fromByteString( data+6, false, size-6 ).str();
  setSheetName( name );
}

void BoundSheetRecord::dump( std::ostream& out ) const
{
  out << "BOUNDSHEET" << std::endl;
  out << "               Name : " << d->name << std::endl;
  out << "               Type : " << d->type << " (" << typeAsString() << ")" << std::endl;
  out << "         Visibility : " << d->visibility << " (";
  if( visible() ) out << "Visible"; else out << "Hidden"; out << ")" << std::endl;
  out << "            BOF pos : " << d->bofPosition << std::endl;
}

// ========== CALCMODE ==========

const unsigned int CalcModeRecord::id = 0x000d;

class CalcModeRecord::Private
{
public:
  bool autoCalc;
};

CalcModeRecord::CalcModeRecord():
  Record()
{
  d = new CalcModeRecord::Private();
  d->autoCalc = false;
}

CalcModeRecord::~CalcModeRecord()
{
  delete d;
}

bool CalcModeRecord::autoCalc() const
{
  return d->autoCalc;
}

void CalcModeRecord::setAutoCalc( bool b )
{
  d->autoCalc = b;
}

void CalcModeRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 2 ) return;

  unsigned flag = readU16( data );
  d->autoCalc = flag != 0;
}

void CalcModeRecord::dump( std::ostream& out ) const
{
  out << "CALCMODE" << std::endl;
  out << "          Auto Calc : " << (autoCalc() ? "Yes" : "No") << std::endl;
}

// ========== COLINFO ==========

const unsigned int ColInfoRecord::id = 0x007d;

class ColInfoRecord::Private
{
public:
  unsigned width;
  unsigned xfIndex;
  bool hidden;
  bool collapsed;
  unsigned outlineLevel;
};

ColInfoRecord::ColInfoRecord():
  Record(), ColumnSpanInfo()
{
  d = new ColInfoRecord::Private();
  d->width        = 2340;
  d->xfIndex      = 0;
  d->hidden       = false;
  d->collapsed    = false;
  d->outlineLevel = 0;
}

ColInfoRecord::~ColInfoRecord()
{
  delete d;
}

// FIXME how to find the real width (in pt/mm/inch) ?
unsigned ColInfoRecord::width() const
{
  return d->width;
}

void ColInfoRecord::setWidth( unsigned w )
{
  d->width = w;
}

unsigned ColInfoRecord::xfIndex() const
{
  return d->xfIndex;
}

void ColInfoRecord::setXfIndex( unsigned i )
{
  d->xfIndex = i;
}

bool ColInfoRecord::hidden() const
{
  return d->hidden;
}

void ColInfoRecord::setHidden( bool h )
{
  d->hidden = h;
}

bool ColInfoRecord::collapsed() const
{
  return d->collapsed;
}

void ColInfoRecord::setCollapsed( bool c )
{
  d->collapsed = c;
}

unsigned ColInfoRecord::outlineLevel() const
{
  return d->outlineLevel;
}

void ColInfoRecord::setOutlineLevel( unsigned l )
{
  d->outlineLevel = l;
}

void ColInfoRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 10 ) return;

  setFirstColumn( readU16( data ) );
  setLastColumn( readU16( data+2 ) );
  setWidth( readU16( data+4 ) );
  setXfIndex( readU16( data+6 ) );

  unsigned options = readU16( data+8 );
  setHidden ( options & 1 );
  setCollapsed ( options & 0x1000 );
  setOutlineLevel( ( options >> 8 ) & 7 );
}

void ColInfoRecord::dump( std::ostream& out ) const
{
  out << "COLINFO" << std::endl;
  out << "       First Column : " << firstColumn() << std::endl;
  out << "        Last Column : " << lastColumn() << std::endl;
  out << "              Width : " << width() << std::endl;
  out << "           XF Index : " << xfIndex() << std::endl;
  out << "             Hidden : " << ( hidden() ? "Yes" : "No" ) << std::endl;
  out << "          Collapsed : " << ( collapsed() ? "Yes" : "No" ) << std::endl;
  out << "      Outline Level : " << outlineLevel() << std::endl;
}

// ========== DATATABLE ==========

const unsigned int DataTableRecord::id = 0x0236;

class DataTableRecord::Private
{
public:
  unsigned firstRow;
  unsigned lastRow;
  unsigned firstColumn;
  unsigned lastColumn;
  Direction direction;

  unsigned inputRow1, inputCol1;
  unsigned inputRow2, inputCol2;
};

DataTableRecord::DataTableRecord():
  Record()
{
  d = new DataTableRecord::Private();
  d->firstRow = d->lastRow = d->firstColumn = d->lastColumn = 0;
  d->inputRow1 = d->inputRow2 = d->inputCol1 = d->inputCol2 = 0;
  d->direction = Input2D;
}

DataTableRecord::~DataTableRecord()
{
  delete d;
}

DataTableRecord::DataTableRecord( const DataTableRecord& record )
 : Record()
{
  d = new DataTableRecord::Private();
  *this = record;
}

DataTableRecord& DataTableRecord::operator=( const DataTableRecord& record )
{
  *d = *record.d;
  return *this;
}

unsigned DataTableRecord::firstRow() const
{
  return d->firstRow;
}

unsigned DataTableRecord::firstColumn() const
{
  return d->firstColumn;
}

DataTableRecord::Direction DataTableRecord::direction() const
{
  return d->direction;
}

unsigned DataTableRecord::inputRow1() const
{
  return d->inputRow1;
}

unsigned DataTableRecord::inputRow2() const
{
  return d->inputRow2;
}

unsigned DataTableRecord::inputColumn1() const
{
  return d->inputCol1;
}

unsigned DataTableRecord::inputColumn2() const
{
  return d->inputCol2;
}

void DataTableRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 12 ) return;
  if( version() != Excel97 ) return;

  d->firstRow = readU16( data );
  d->lastRow = readU16( data+2 );
  d->firstColumn = data[4];
  d->lastColumn = data[5];

  d->inputRow1 = readU16( data + 8 );
  d->inputCol1 = readU16( data + 10 );
  if( size >= 16 )
  {
    d->inputRow2 = readU16( data + 12 );
    d->inputCol2 = readU16( data + 14 );
  }

  unsigned options = readU16( data+6 );
  if( options & 0x0008 )
    d->direction = Input2D;
  else if( options & 0x0004 )
    d->direction = InputRow;
  else
    d->direction = InputColumn;
}

void DataTableRecord::dump( std::ostream& out ) const
{
  out << "DATATABLE" << std::endl;
  out << "       First Column : " << d->firstColumn << std::endl;
  out << "        Last Column : " << d->lastColumn << std::endl;
  out << "          First Row : " << d->firstRow << std::endl;
  out << "           Last Row : " << d->lastRow << std::endl;
  out << "          Direction : " << ( d->direction == Input2D ? "2D" : d->direction == InputColumn ? "Column" : "Row" ) << std::endl;
  out << "        Input row 1 : " << d->inputRow1 << std::endl;
  out << "     Input column 1 : " << d->inputCol1 << std::endl;
  out << "        Input row 2 : " << d->inputRow2 << std::endl;
  out << "     Input column 2 : " << d->inputCol2 << std::endl;
}

// ========== DATEMODE ==========

const unsigned int DateModeRecord::id = 0x0022;

class DateModeRecord::Private
{
public:
  bool base1904;
};

DateModeRecord::DateModeRecord():
  Record()
{
  d = new DateModeRecord::Private();
  d->base1904 = false;
}

DateModeRecord::~DateModeRecord()
{
  delete d;
}

bool DateModeRecord::base1904() const
{
  return d->base1904;
}

void DateModeRecord::setBase1904( bool r )
{
  d->base1904 = r;
}

void DateModeRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 2 ) return;

  unsigned flag = readU16( data );
  d->base1904 = flag != 0;
}

void DateModeRecord::dump( std::ostream& out ) const
{
  out << "DATEMODE" << std::endl;
  out << "          1904 base : " << (base1904() ? "Yes" : "No") << std::endl;
}


// ========== DIMENSION ==========

const unsigned int DimensionRecord::id = 0x0200;

class DimensionRecord::Private
{
public:
  unsigned firstRow;
  unsigned lastRow;
  unsigned firstColumn;
  unsigned lastColumn;
};

DimensionRecord::DimensionRecord():
  Record()
{
  d = new DimensionRecord::Private;
  d->firstRow    = 0;
  d->lastRow     = 0;
  d->firstColumn = 0;
  d->lastColumn  = 0;
}

DimensionRecord::~DimensionRecord()
{
  delete d;
}

unsigned DimensionRecord::firstRow() const
{
  return d->firstRow;
}

void DimensionRecord::setFirstRow( unsigned r )
{
  d->firstRow = r;
}

unsigned DimensionRecord::lastRow() const
{
  return d->lastRow;
}

void DimensionRecord::setLastRow( unsigned r )
{
  d->lastRow = r;
}

unsigned DimensionRecord::firstColumn() const
{
  return d->firstColumn;
}

void DimensionRecord::setFirstColumn( unsigned r )
{
  d->firstColumn = r;
}

unsigned DimensionRecord::lastColumn() const
{
  return d->lastColumn;
}

void DimensionRecord::setLastColumn( unsigned r )
{
  d->lastColumn = r;
}

void DimensionRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 14 ) return;

  setFirstRow( readU32( data ) );
  setLastRow( readU32( data+4 ) - 1 );
  setFirstColumn( readU16( data + 8 ) );
  setLastColumn( readU16( data + 10 ) - 1 );
}

void DimensionRecord::dump( std::ostream& out ) const
{
  out << "DIMENSION" << std::endl;
  out << "          First Row : " << firstRow() << std::endl;
  out << "           Last Row : " << lastRow() << std::endl;
  out << "       First Column : " << firstColumn() << std::endl;
  out << "        Last Column : " << lastColumn() << std::endl;
}

// ========== EOF ==========

const unsigned int EOFRecord::id = 0x000a;

EOFRecord::EOFRecord():
  Record()
{
}

EOFRecord::~EOFRecord()
{
}

void EOFRecord::setData( unsigned,  const unsigned char*, const unsigned int* )
{
  // no data associated with EOF record
}

void EOFRecord::dump( std::ostream& out ) const
{
  out << "EOF" << std::endl;
}

// ========== EXTERNBOOK ==========

const unsigned int ExternBookRecord::id = 0x01ae;

class ExternBookRecord::Private
{
public:
  unsigned sheetCount;
  UString name;
};


ExternBookRecord::ExternBookRecord()
{
  d = new Private;
  d->sheetCount = 0;
}

ExternBookRecord::~ExternBookRecord()
{
  delete d;
}

UString ExternBookRecord::bookName() const
{
  return d->name;
}

void ExternBookRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 4 ) return;

  d->sheetCount = readU16( data );
  if( data[2] == 0x01 && data[3] == 0x04 )
  {
    d->name = UString("\004");
  }
  else if( data[2] == 0x01 && data[3] == ':' )
  {
    d->name = UString(":");
  }
  else
  {
    d->name = EString::fromUnicodeString( data+2, true, size-2 ).str();
  }
}

void ExternBookRecord::dump( std::ostream& out ) const
{
  out << "EXTERNBOOK" << std::endl;
  out << "        Sheet count : " << d->sheetCount << std::endl;
  out << "               Name : " << d->name << std::endl;
}

// ========== EXTERNNAME ==========

const unsigned int ExternNameRecord::id = 0x0023;

class ExternNameRecord::Private
{
public:
  unsigned optionFlags;
  unsigned sheetIndex;   // one-based, not zero-based
  UString externName;
};


ExternNameRecord::ExternNameRecord()
{
  d = new Private;
  d->optionFlags = 0;
  d->sheetIndex = 0;
}

ExternNameRecord::~ExternNameRecord()
{
  delete d;
}

void ExternNameRecord::setSheetIndex( unsigned sheetIndex )
{
  d->sheetIndex = sheetIndex;
}

unsigned ExternNameRecord::sheetIndex() const
{
  return d->sheetIndex;
}

void ExternNameRecord::setExternName( const UString& name )
{
  d->externName = name;
}

UString ExternNameRecord::externName() const
{
  return d->externName;
}

void ExternNameRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 6 ) return;

  if ( version() == Excel97 )
  {
    d->optionFlags = readU16( data );
    d->sheetIndex = readU16( data+2 );
    d->externName = EString::fromUnicodeString( data+6, false, size ).str();
  }

  if ( version() == Excel95 )
  {
    d->optionFlags = 0;
    d->sheetIndex = 0;
    d->externName = EString::fromByteString( data+6, false, size ).str();
  }
}

void ExternNameRecord::dump( std::ostream& /*out*/ ) const
{
}

// ========== EXTERNSHEET ==========

const unsigned int ExternSheetRecord::id = 0x0017;

class ExternSheetRecord::Private
{
public:
  unsigned refCount;
  std::vector<unsigned> books;
  std::vector<std::pair<unsigned, unsigned> > sheets;
};


ExternSheetRecord::ExternSheetRecord()
{
  d = new Private;
  d->refCount = 0;
}

ExternSheetRecord::~ExternSheetRecord()
{
  delete d;
}

unsigned ExternSheetRecord::refCount() const
{
  return d->refCount;
}

unsigned ExternSheetRecord::bookRef( unsigned refIndex ) const
{
  return d->books[ refIndex ];
}

unsigned ExternSheetRecord::firstSheetRef( unsigned refIndex ) const
{
  return d->sheets[ refIndex ].first;
}

unsigned ExternSheetRecord::lastSheetRef( unsigned refIndex ) const
{
  return d->sheets[ refIndex ].second;
}

void ExternSheetRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 2 ) return;
  if( version() != Excel97 ) return;

  d->refCount = std::max(0, std::min<int>( (int(size)-2) / 6, readU16( data ) ) );
  d->books.resize(d->refCount);
  d->sheets.resize(d->refCount);

  for( unsigned i = 0; i < d->refCount; i++ )
  {
    d->books[i]           = readU16( data + 2 + 6*i + 0 );
    d->sheets[i].first    = readU16( data + 2 + 6*i + 2 );
    d->sheets[i].second   = readU16( data + 2 + 6*i + 4 );
  }
}

void ExternSheetRecord::dump( std::ostream& out ) const
{
  out << "EXTERNSHEET" << std::endl;
  out << "          Ref count : " << d->refCount << std::endl;
  for( unsigned i = 0; i < d->refCount; i++ )
  {
    out << "            Ref #" << std::setw(2) << i << " : Book #" << d->books[i] << ", Sheets #" << d->sheets[i].first << " - #" << d->sheets[i].second << std::endl;
  }
}

// ========== FILEPASS ==========

const unsigned int FilepassRecord::id = 0x002f;

FilepassRecord::FilepassRecord():
  Record()
{
}

FilepassRecord::~FilepassRecord()
{
}

void FilepassRecord::setData( unsigned,  const unsigned char*, const unsigned int* )
{
  // TODO
}

void FilepassRecord::dump( std::ostream& out ) const
{
  out << "FILEPASS" << std::endl;
}

// ========== FONT ==========

const unsigned int FontRecord::id = 0x0031;

class FontRecord::Private
{
public:
  unsigned height;
  UString fontName;
  unsigned fontFamily;
  unsigned characterSet;
  unsigned colorIndex;
  unsigned boldness;
  bool italic;
  bool strikeout;
  unsigned escapement;
  unsigned underline;
};

FontRecord::FontRecord():  Record()
{
  d = new FontRecord::Private;
  d->height       = 11;
  d->fontName     = "Arial";
  d->fontFamily   = 0;
  d->characterSet = 0;
  d->colorIndex   = 0;
  d->boldness     = 400;
  d->italic       = false;
  d->strikeout    = false;
  d->escapement   = Normal;
  d->underline    = None;
}

FontRecord::~FontRecord()
{
  delete d;
}

FontRecord::FontRecord( const FontRecord& ef ):  Record()
{
  d = new FontRecord::Private;
  operator=( ef );
}

FontRecord& FontRecord::operator=( const FontRecord& ef )
{
  d->height       = ef.height();
  d->fontName     = ef.fontName();
  d->fontFamily   = ef.fontFamily();
  d->characterSet = ef.characterSet();
  d->boldness     = ef.boldness();
  d->italic       = ef.italic();
  d->strikeout    = ef.strikeout();
  d->escapement   = ef.escapement();
  d->underline    = ef.underline();
  d->colorIndex   = ef.colorIndex();
  return *this;
}

unsigned FontRecord::height() const
{
  return d->height;
}

void FontRecord::setHeight( unsigned h )
{
  d->height = h;
}

UString FontRecord::fontName() const
{
  return d->fontName;
}

void FontRecord::setFontName( const UString& fn )
{
  d->fontName = fn;
}

unsigned FontRecord::fontFamily() const
{
  return d->fontFamily;
}

void FontRecord::setFontFamily( unsigned f )
{
  d->fontFamily = f;
}

unsigned FontRecord::characterSet() const
{
  return d->characterSet;
}

void FontRecord::setCharacterSet( unsigned cs )
{
  d->characterSet = cs;
}

unsigned FontRecord::colorIndex() const
{
  return d->colorIndex;
}

void FontRecord::setColorIndex( unsigned ci )
{
  d->colorIndex = ci;
}

unsigned FontRecord::boldness() const
{
  return d->boldness;
}

void FontRecord::setBoldness( unsigned b )
{
  d->boldness = b;
}

bool FontRecord::italic() const
{
  return d->italic;
}

void FontRecord::setItalic( bool i )
{
  d->italic = i;
}

bool FontRecord::strikeout() const
{
  return d->strikeout;
}

void FontRecord::setStrikeout( bool s )
{
  d->strikeout = s;
}

unsigned FontRecord::escapement() const
{
  return d->escapement;
}

void FontRecord::setEscapement( unsigned s )
{
  d->escapement = s;
}

unsigned FontRecord::underline() const
{
  return d->underline;
}

void FontRecord::setUnderline( unsigned u )
{
  d->underline = u;
}


void FontRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 14 ) return;

  setHeight( readU16( data ) );
  unsigned flag = readU16( data+2 );
  setItalic( flag & 2 );
  setStrikeout( flag & 8 );
  setStrikeout( flag & 8 );

  setColorIndex( readU16( data+4 ) );

  setBoldness( readU16( data+6 ) );
  setEscapement( readU16( data+8 ) );
  setUnderline( data[10] );

  setFontFamily( data[11] );
  setCharacterSet( data[12] );

  UString fn = ( version() >= Excel97 ) ?
    EString::fromSheetName( data+14, size-14 ).str() :
    EString::fromByteString( data+14, false, size-14 ).str();
  setFontName( fn );
}


void FontRecord::dump( std::ostream& out ) const
{
  out << "FONT" << std::endl;
  out << "             Height : " << height() << " twips" << std::endl;
  out << "          Font Name : " << fontName() << std::endl;
  out << "        Color Index : " << colorIndex() << std::endl;
  out << "           Boldness : " << boldness() << std::endl;
  out << "             Italic : " << (italic()?"Yes":"No") << std::endl;
  out << "          Strikeout : " << (strikeout()?"Yes":"No") << std::endl;
  out << "         Escapement : ";
  switch( escapement() )
  {
    case Normal: out << "Normal" << std::endl; break;
    case Subscript: out << "Subscript" << std::endl; break;
    case Superscript: out << "Superscript" << std::endl; break;
    default: out << "Unknown " << escapement() << std::endl; break;
  };
}

// ========== FOOTER ==========

const unsigned int FooterRecord::id = 0x0015;

class FooterRecord::Private
{
public:
  UString footer;
};

FooterRecord::FooterRecord():
  Record()
{
  d = new FooterRecord::Private();
}

FooterRecord::~FooterRecord()
{
  delete d;
}

UString FooterRecord::footer() const
{
  return d->footer;
}

void FooterRecord::setFooter( const UString& footer )
{
  d->footer = footer;
}

void FooterRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 2 ) return;

  UString footer = ( version() >= Excel97 ) ?
    EString::fromUnicodeString( data, true, size ).str() :
    EString::fromByteString( data, false, size ).str();
  setFooter( footer );
}

void FooterRecord::dump( std::ostream& out ) const
{
  out << "FOOTER" << std::endl;
  out << "             Footer : " << footer() << std::endl;
}

// ========== FORMAT ==========

const unsigned int FormatRecord::id = 0x041e;

class FormatRecord::Private
{
public:
  unsigned index;
  UString formatString;
};

FormatRecord::FormatRecord():
  Record()
{
  d = new FormatRecord::Private;
  d->index = 0;
  d->formatString = "General";
}

FormatRecord::~FormatRecord()
{
  delete d;
}

FormatRecord::FormatRecord( const FormatRecord& fr ):
  Record()
{
  d = new FormatRecord::Private;
  operator=( fr );
}

FormatRecord& FormatRecord::operator=( const FormatRecord& fr )
{
  d->index = fr.index();
  d->formatString = fr.formatString();
  return *this;
}

unsigned FormatRecord::index() const
{
  return d->index;
}

void FormatRecord::setIndex( unsigned i )
{
  d->index = i;
}

UString FormatRecord::formatString() const
{
  return d->formatString;
}

void FormatRecord::setFormatString( const UString& fs )
{
  d->formatString = fs;
}

void FormatRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 3 ) return;

  setIndex( readU16( data ) );

  UString fs = ( version() >= Excel97 ) ?
    EString::fromUnicodeString( data+2, true, size-2 ).str() :
    EString::fromByteString( data+2, false, size-2 ).str();
  setFormatString( fs );
}

void FormatRecord::dump( std::ostream& out ) const
{
  out << "FORMAT" << std::endl;
  out << "             Index  : " << index() << std::endl;
  out << "      Format String : " << formatString() << std::endl;
}


// ========== FORMULA ==========

const unsigned int FormulaRecord::id = 0x0006;

class FormulaRecord::Private
{
public:
  Value result;
  FormulaTokens tokens;
};

FormulaRecord::FormulaRecord():
  Record()
{
  d = new FormulaRecord::Private();
}

FormulaRecord::~FormulaRecord()
{
  delete d;
}

Value FormulaRecord::result() const
{
  return d->result;
}

void FormulaRecord::setResult( const Value& r )
{
  d->result = r;
}

FormulaTokens FormulaRecord::tokens() const
{
  return d->tokens;
}

void FormulaRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 20 ) return;

  setRow( readU16( data ) );
  setColumn( readU16( data+2 ) );
  setXfIndex( readU16( data+4 ) );

  if( readU16( data+12 ) != 0xffff )
  {
    // Floating-point
    setResult( Value( readFloat64( data+6 ) ) );
  }
  else
  {
    switch( data[6] )
    {
      case 0: // string, real value in subsequent string record
        setResult( Value( Value::String ) );
        break;
      case 1: // boolean
        setResult( Value( data[8] ? true : false ) );
        break;
      case 2: // error code
        setResult( errorAsValue( data[8] ) );
        break;
      case 3: // empty
        setResult( Value::empty() );
        break;
      default: // fallback
        setResult( Value::empty() );
        break;
    };
  }

  unsigned formula_len = readU16( data+20 );

  // reconstruct all tokens
  d->tokens.clear();
  for( unsigned j = 22; j < size; )
  {
    unsigned ptg = data[j++];
    ptg = ((ptg & 0x40) ? (ptg | 0x20) : ptg) & 0x3F;
    FormulaToken token( ptg );
    token.setVersion( version() );

    if( token.id() == FormulaToken::String )
    {
      // find bytes taken to represent the string
      EString estr = (version()==Excel97) ?
        EString::fromUnicodeString( data+j, false, formula_len ) :
        EString::fromByteString( data+j, false, formula_len );
      token.setData( estr.size(), data+j );
      j += estr.size();
    }
    else
    {
      // normal, fixed-size token
      if( token.size() > 0 )
      {
        token.setData( token.size(), data+j );
        j += token.size();
      }
    }

    d->tokens.push_back( token );
  }
}

void FormulaRecord::dump( std::ostream& out ) const
{
  out << "FORMULA" << std::endl;
  out << "                Row : " << row() << std::endl;
  out << "             Column : " << column() << std::endl;
  out << "           XF Index : " << xfIndex() << std::endl;
  out << "             Result : " << result() << std::endl;

  FormulaTokens ts = tokens();
  out << "             Tokens : " << ts.size() << std::endl;
  for( unsigned i = 0; i < ts.size(); i++ )
    out << "                       " << ts[i]  << std::endl;

}


// SHAREDFMLA

const unsigned int SharedFormulaRecord::id = 0x04BC;

class SharedFormulaRecord::Private
{
public:
  // range
  int numCells;
  FormulaTokens tokens;
};

SharedFormulaRecord::SharedFormulaRecord():
  Record()
{
  d = new SharedFormulaRecord::Private();
}

SharedFormulaRecord::~SharedFormulaRecord()
{
  delete d;
}

FormulaTokens SharedFormulaRecord::tokens() const
{
  return d->tokens;
}

void SharedFormulaRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 8 ) return;

  // maybe read range
  d->numCells = data[7];

  unsigned formula_len = readU16( data+8 );

  // reconstruct all tokens
  d->tokens.clear();
  for( unsigned j = 10; j < size; )
  {
    unsigned ptg = data[j++];
    ptg = ((ptg & 0x40) ? (ptg | 0x20) : ptg) & 0x3F;
    FormulaToken token( ptg );
    token.setVersion( version() );

    if( token.id() == FormulaToken::String )
    {
      // find bytes taken to represent the string
      EString estr = (version()==Excel97) ?
        EString::fromUnicodeString( data+j, false, formula_len ) :
        EString::fromByteString( data+j, false, formula_len );
      token.setData( estr.size(), data+j );
      j += estr.size();
    }
    else
    {
      // normal, fixed-size token
      if( token.size() > 1 )
      {
        token.setData( token.size(), data+j );
        j += token.size();
      }
    }

    d->tokens.push_back( token );
  }
}

void SharedFormulaRecord::dump( std::ostream& out ) const
{
  out << "SHAREDFMLA" << std::endl;
  // range
  out << "          Num cells : " << d->numCells << std::endl;

  FormulaTokens ts = tokens();
  out << "             Tokens : " << ts.size() << std::endl;
  for( unsigned i = 0; i < ts.size(); i++ )
    out << "                       " << ts[i]  << std::endl;

}

// ========== LABEL ==========

const unsigned int LabelRecord::id = 0x0204;

class LabelRecord::Private
{
public:
  UString label;
};

LabelRecord::LabelRecord():
  Record(), CellInfo()
{
  d = new LabelRecord::Private();
  d->label = UString::null;
}

LabelRecord::~LabelRecord()
{
  delete d;
}

UString LabelRecord::label() const
{
  return d->label;
}

void LabelRecord::setLabel( const UString& l )
{
  d->label = l;
}

void LabelRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 6 ) return;

  setRow( readU16( data ) );
  setColumn( readU16( data+2 ) );
  setXfIndex( readU16( data+4 ) );

  UString label = ( version() >= Excel97 ) ?
    EString::fromUnicodeString( data+6, true, size-6 ).str() :
    EString::fromByteString( data+6, true, size-6 ).str();
  setLabel( label );
}

void LabelRecord::dump( std::ostream& out ) const
{
  out << "LABEL" << std::endl;
  out << "                Row : " << row() << std::endl;
  out << "             Column : " << column() << std::endl;
  out << "           XF Index : " << xfIndex() << std::endl;
  out << "              Label : " << label() << std::endl;
}

// ========== HEADER ==========

const unsigned int HeaderRecord::id = 0x0014;

class HeaderRecord::Private
{
public:
  UString header;
};

HeaderRecord::HeaderRecord():
  Record()
{
  d = new HeaderRecord::Private();
}

HeaderRecord::~HeaderRecord()
{
  delete d;
}

UString HeaderRecord::header() const
{
  return d->header;
}

void HeaderRecord::setHeader( const UString& header )
{
  d->header = header;
}

void HeaderRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 2 ) return;

  UString header = ( version() >= Excel97 ) ?
    EString::fromUnicodeString( data, true, size ).str() :
    EString::fromByteString( data, false, size ).str();
  setHeader( header );
}

void HeaderRecord::dump( std::ostream& out ) const
{
  out << "HEADER" << std::endl;
  out << "              Header: " << header() << std::endl;
}

// ========== LABELSST ==========

const unsigned int LabelSSTRecord::id = 0x00fd;

class LabelSSTRecord::Private
{
public:
  unsigned sstIndex;
};

LabelSSTRecord::LabelSSTRecord():
  Record(), CellInfo()
{
  d = new LabelSSTRecord::Private();
  d->sstIndex = 0;
}

LabelSSTRecord::~LabelSSTRecord()
{
  delete d;
}

unsigned LabelSSTRecord::sstIndex() const
{
  return d->sstIndex;
}

void LabelSSTRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 10 ) return;

  setRow( readU16( data ) );
  setColumn( readU16( data+2 ) );
  setXfIndex( readU16( data+4 ) );

  d->sstIndex = readU32( data+6 );
}

void LabelSSTRecord::dump( std::ostream& out ) const
{
  out << "LABELSST" << std::endl;
  out << "                Row : " << row() << std::endl;
  out << "             Column : " << column() << std::endl;
  out << "           XF Index : " << xfIndex() << std::endl;
  out << "          SST Index : " << d->sstIndex << std::endl;
}

// ========== LEFTMARGIN ==========

const unsigned int LeftMarginRecord::id = 0x0026;

class LeftMarginRecord::Private
{
public:
  double leftMargin;
};

LeftMarginRecord::LeftMarginRecord():
  Record()
{
  d = new LeftMarginRecord::Private();
  d->leftMargin = 1.0;
}

LeftMarginRecord::~LeftMarginRecord()
{
  delete d;
}

double LeftMarginRecord::leftMargin() const
{
  return d->leftMargin;
}

void LeftMarginRecord::setLeftMargin( double m )
{
  d->leftMargin = m;
}

void LeftMarginRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 8 ) return;
  setLeftMargin( readFloat64( data ) );
}

void LeftMarginRecord::dump( std::ostream& out ) const
{
  out << "LEFTMARGIN" << std::endl;
  out << "        Left Margin : " << leftMargin() << " inches" << std::endl;
}

// ========== MERGEDCELLS ==========

const unsigned int MergedCellsRecord::id = 0x00e5;

class MergedInfo
{
public:
  unsigned firstRow, lastRow, firstColumn, lastColumn;
};

class MergedCellsRecord::Private
{
public:
  std::vector<MergedInfo> mergedCells;
};

MergedCellsRecord::MergedCellsRecord():
  Record()
{
  d = new MergedCellsRecord::Private();
}

MergedCellsRecord::~MergedCellsRecord()
{
  delete d;
}

unsigned MergedCellsRecord::count() const
{
  return d->mergedCells.size();
}

unsigned MergedCellsRecord::firstRow( unsigned i ) const
{
  if( i >= d->mergedCells.size() ) return 0;
  MergedInfo info = d->mergedCells[ i ];
  return info.firstRow;
}

unsigned MergedCellsRecord::lastRow( unsigned i ) const
{
  if( i >= d->mergedCells.size() ) return 0;
  MergedInfo info = d->mergedCells[ i ];
  return info.lastRow;
}

unsigned MergedCellsRecord::firstColumn( unsigned i ) const
{
  if( i >= d->mergedCells.size() ) return 0;
  MergedInfo info = d->mergedCells[ i ];
  return info.firstColumn;
}

unsigned MergedCellsRecord::lastColumn( unsigned i ) const
{
  if( i >= d->mergedCells.size() ) return 0;
  MergedInfo info = d->mergedCells[ i ];
  return info.lastColumn;
}

void MergedCellsRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 2 ) return;

  unsigned num = readU16( data );

  // sanity check
  if( size < 2 + num*4 ) return;

  unsigned p = 2;
  for( unsigned i = 0; i < num; i++ )
  {
    MergedInfo info;
    info.firstRow = readU16( data + p );
    info.lastRow = readU16( data + p + 2 );
    info.firstColumn = readU16( data + p + 4 );
    info.lastColumn = readU16( data + p + 6 );
    p += 8;
    d->mergedCells.push_back( info );
  }
}

void MergedCellsRecord::dump( std::ostream& out ) const
{
  out << "MERGEDCELLS" << std::endl;
  out << "              Count : " << count() << std::endl;
  for( unsigned c = 0; c < count(); c++ )
  {
    out << "     Merged Cell #" << c << " : ";
    out << "Column " << firstColumn(c) << "-" << lastColumn(c);
    out << "   Row " << firstRow(c) << "-" << lastRow(c);
    out << std::endl;
  }
}

// ========== MULBLANK ==========

const unsigned int MulBlankRecord::id = 0x00be;

class MulBlankRecord::Private
{
public:
  std::vector<unsigned> xfIndexes;
};

MulBlankRecord::MulBlankRecord():
  Record(), CellInfo(), ColumnSpanInfo()
{
  d = new MulBlankRecord::Private();
}

MulBlankRecord::~MulBlankRecord()
{
  delete d;
}

void MulBlankRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 6 ) return;

  setRow( readU16( data ) );

  setFirstColumn( readU16( data+2 ) );
  setLastColumn( readU16( data+size-2 ) );

  d->xfIndexes.clear();
  for( unsigned i = 4; i < size-2; i+= 2 )
    d->xfIndexes.push_back( readU16( data+i ) );

  // FIXME sentinel !
}

unsigned MulBlankRecord::xfIndex( unsigned i ) const
{
  if( i >= d->xfIndexes.size() ) return 0;
  return d->xfIndexes[ i ];
}

void MulBlankRecord::dump( std::ostream& out ) const
{
  out << "MULBLANK" << std::endl;
  out << "                Row : " << row() << std::endl;
  out << "       First Column : " << firstColumn() << std::endl;
  out << "        Last Column : " << lastColumn() << std::endl;
}

// ========== MULRK ==========

const unsigned int MulRKRecord::id = 0x00bd;

class MulRKRecord::Private
{
public:
  std::vector<unsigned> xfIndexes;
  std::vector<bool> isIntegers;
  std::vector<int> intValues;
  std::vector<double> floatValues;
  std::vector<unsigned> rkValues;
};

MulRKRecord::MulRKRecord():
  Record(), CellInfo(), ColumnSpanInfo()
{
  d = new MulRKRecord::Private();
}

MulRKRecord::~MulRKRecord()
{
  delete d;
}

unsigned MulRKRecord::xfIndex( unsigned i ) const
{
  if( i >= d->xfIndexes.size() ) return 0;
  return d->xfIndexes[ i ];
}

bool MulRKRecord::isInteger( unsigned i ) const
{
  if( i >= d->isIntegers.size() ) return true;
  return d->isIntegers[ i ];
}

int MulRKRecord::asInteger( unsigned i ) const
{
  if( i >= d->intValues.size() ) return 0;
  return d->intValues[ i ];
}

double MulRKRecord::asFloat( unsigned i ) const
{
  if( i >= d->floatValues.size() ) return 0.0;
  return d->floatValues[ i ];
}

unsigned MulRKRecord::encodedRK( unsigned i ) const
{
  if( i >= d->rkValues.size() ) return 0;
  return d->rkValues[ i ];
}

void MulRKRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 6 ) return;

  setRow( readU16( data ) );

  setFirstColumn( readU16( data+2 ) );
  setLastColumn( readU16( data+size-2 ) );

  d->xfIndexes.clear();
  d->isIntegers.clear();
  d->intValues.clear();
  d->floatValues.clear();
  for( unsigned i = 4; i < size-2; i+= 6 )
  {
    d->xfIndexes.push_back( readU16( data+i ) );
    unsigned rk = readU32( data+i+2 );
    d->rkValues.push_back( rk );
    bool isInteger = true; int iv = 0; double fv = 0.0;
    decodeRK( rk, isInteger, iv, fv );

    d->isIntegers.push_back( isInteger );
    d->intValues.push_back( isInteger ? iv : (int)fv );
    d->floatValues.push_back( !isInteger ? fv : (double)iv );
  }

  // FIXME sentinel !
}

void MulRKRecord::dump( std::ostream& out ) const
{
  out << "MULRK" << std::endl;
  out << "                Row : " << row() << std::endl;
  out << "       First Column : " << firstColumn() << std::endl;
  out << "        Last Column : " << lastColumn() << std::endl;
  for( unsigned c = firstColumn(); c <= lastColumn(); c++ )
  {
    out << "          Column  " << c << " : " << asFloat( c-firstColumn() );
    out << "  Encoded: " << std::hex << encodedRK( c-firstColumn() );
    out << std::endl;
  }
}

// ========== NAME ==========

const unsigned int NameRecord::id = 0x0018;

class NameRecord::Private
{
public:
  unsigned optionFlags;
  UString definedName;
};


NameRecord::NameRecord()
{
  d = new Private;
  d->optionFlags = 0;
}

NameRecord::~NameRecord()
{
  delete d;
}

void NameRecord::setDefinedName( const UString& name )
{
  d->definedName = name;
}

UString NameRecord::definedName() const
{
  return d->definedName;
}

void NameRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 14 ) return;

  d->optionFlags = readU16( data );
  unsigned len = data[3];

  if ( version() == Excel95 )
  {
    char* buffer = new char[ len+1 ];
    memcpy( buffer, data + 14, len );
    buffer[ len ] = 0;
    d->definedName = UString( buffer );
    delete[] buffer;
  }

  if ( version() == Excel97 )
  {
    UString str = UString();
    for( unsigned k=0; k<len; k++ )
    {
      unsigned uchar = readU16( data + 14 + k*2 );
      str.append( UString(uchar) );
    }
    d->definedName = str;
  }
}

void NameRecord::dump( std::ostream& /*out*/ ) const
{
}

// ========== Number ==========

const unsigned int NumberRecord::id = 0x0203;

class NumberRecord::Private
{
public:
  double number;
};

NumberRecord::NumberRecord():
  Record(), CellInfo()
{
  d = new NumberRecord::Private();
  d->number = 0.0;
}

NumberRecord::~NumberRecord()
{
  delete d;
}

double NumberRecord::number() const
{
  return d->number;
}

void NumberRecord::setNumber( double f )
{
  d->number = f;
}

// FIXME check that sizeof(double) is 64
void NumberRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 14 ) return;

  setRow( readU16( data ) );
  setColumn( readU16( data+2 ) );
  setXfIndex( readU16( data+4 ) );
  setNumber( readFloat64( data+6 ) );
}

void NumberRecord::dump( std::ostream& out ) const
{
  out << "NUMBER" << std::endl;
  out << "                Row : " << row() << std::endl;
  out << "             Column : " << column() << std::endl;
  out << "           XF Index : " << xfIndex() << std::endl;
  out << "              Value : " << number() << std::endl;
}

// ========== PALETTE ==========

const unsigned int PaletteRecord::id = 0x0092;

class PaletteRecord::Private
{
public:
  std::vector<Color> colors;
};

PaletteRecord::PaletteRecord():
  Record()
{
  d = new PaletteRecord::Private();
}

PaletteRecord::~PaletteRecord()
{
  delete d;
}

Color PaletteRecord::color( unsigned i ) const
{
  return d->colors[ i ];
}

unsigned PaletteRecord::count() const
{
  return d->colors.size();
}

void PaletteRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 14 ) return;

  unsigned num = readU16( data );

  unsigned p = 2;
  for( unsigned i = 0; i < num; i++, p+=4 )
  {
    unsigned red = data[ p ];
    unsigned green = data[ p+1 ];
    unsigned blue = data[ p+2 ];
    d->colors.push_back( Color( red, green, blue ) );
  }
}

void PaletteRecord::dump( std::ostream& out ) const
{
  out << "PALETTE" << std::endl;
  out << "             Count : " << count() << std::endl;
  for( unsigned i = 0; i < count(); i++ )
  {
    out << "         Color #" << std::setw(2) << i << " : ";
    Color c = color( i );
    out << "R:" << std::setw(3) << c.red;
    out << "   G:" << std::setw(3) << c.green;
    out << "   B:" << std::setw(3) << c.blue << std::endl;
  }
}

// ========== RIGHTMARGIN ==========

const unsigned int RightMarginRecord::id = 0x0027;

class RightMarginRecord::Private
{
public:
  double rightMargin;
};

RightMarginRecord::RightMarginRecord():
  Record()
{
  d = new RightMarginRecord::Private();
  d->rightMargin = 1.0;
}

RightMarginRecord::~RightMarginRecord()
{
  delete d;
}

double RightMarginRecord::rightMargin() const
{
  return d->rightMargin;
}

void RightMarginRecord::setRightMargin( double m )
{
  d->rightMargin = m;
}

void RightMarginRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 8 ) return;
  setRightMargin( readFloat64( data ) );
}

void RightMarginRecord::dump( std::ostream& out ) const
{
  out << "RIGHTMARGIN" << std::endl;
  out << "       Right Margin : " << rightMargin() << " inches " << std::endl;
}

// ========== RK ==========

const unsigned int RKRecord::id = 0x027e;

class RKRecord::Private
{
public:
  bool integer;
  unsigned rk;
  int i;
  double f;
};

RKRecord::RKRecord():
  Record(), CellInfo()
{
  d = new RKRecord::Private();
  d->integer = true;
  d->rk = 0;
  d->i = 0;
  d->f = 0.0;
}

RKRecord::~RKRecord()
{
  delete d;
}

bool RKRecord::isInteger() const
{
  return d->integer;
}

bool RKRecord::isFloat() const
{
  return !d->integer;
}

int RKRecord::asInteger() const
{
  if( d->integer )
    return d->i;
  else
    return (int)d->f;
}

double RKRecord::asFloat() const
{
  if( !d->integer )
    return d->f;
  else
    return (double)d->i;
}

void RKRecord::setInteger( int i )
{
  d->integer = true;
  d->i = i;
  d->f = (double)i;
}

void RKRecord::setFloat( double f )
{
  d->integer = false;
  d->i = (int)f;
  d->f = f;
}

unsigned RKRecord::encodedRK() const
{
  return d->rk;
}

// FIXME check sizeof(int) is 32
// big vs little endian problem
void RKRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 10 ) return;

  setRow( readU16( data ) );
  setColumn( readU16( data+2 ) );
  setXfIndex( readU16( data+4 ) );

  int i = 0; double f = 0.0;
  d->rk = readU32( data+6 );
  decodeRK( d->rk, d->integer, i, f );
  if( d->integer ) setInteger( i );
  else setFloat( f );
}

void RKRecord::dump( std::ostream& out ) const
{
  out << "RK" << std::endl;
  out << "                Row : " << row() << std::endl;
  out << "             Column : " << column() << std::endl;
  out << "           XF Index : " << xfIndex() << std::endl;
  out << "              Value : " << asFloat() << std::endl;
  out << "         Encoded RK : 0x" << std::hex << encodedRK() << std::endl;
  out << std::dec;
}

// ========== Row ==========

const unsigned int RowRecord::id = 0x0208;

class RowRecord::Private
{
public:
  unsigned row;
  unsigned height;
  unsigned xfIndex;
  bool hidden;
};

RowRecord::RowRecord():
  Record(), ColumnSpanInfo()
{
  d = new RowRecord::Private();
  d->row     = 0;
  d->height  = 50;
  d->xfIndex = 0;
  d->hidden  = false;
}

RowRecord::~RowRecord()
{
  delete d;
}

unsigned RowRecord::row() const
{
  return d->row;
}

void RowRecord::setRow( unsigned r )
{
  d->row = r;
}

unsigned RowRecord::height() const
{
  return d->height;
}

void RowRecord::setHeight( unsigned h )
{
  d->height = h;
}

unsigned RowRecord::xfIndex() const
{
  return d->xfIndex;
}

void RowRecord::setXfIndex( unsigned i )
{
  d->xfIndex = i;
}

bool RowRecord::hidden() const
{
  return d->hidden;
}

void RowRecord::setHidden( bool h )
{
  d->hidden = h;
}

void RowRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 16 ) return;

  setRow( readU16( data ) );
  setFirstColumn( readU16( data+2 ) );
  setLastColumn( readU16( data+4 ) );
  setHeight( readU16( data+6 ) & 0x7fff );
  setXfIndex( readU16( data+14 ) & 0xfff );

  unsigned options = readU16( data+12 );
  setHidden ( options & 0x20 );
}

void RowRecord::dump( std::ostream& out ) const
{
  out << "ROW" << std::endl;
  out << "                Row : " << row() << std::endl;
  out << "       First Column : " << firstColumn() << std::endl;
  out << "        Last Column : " << lastColumn() << std::endl;
  out << "             Height : " << height() << std::endl;
  out << "           XF Index : " << xfIndex() << std::endl;
  out << "             Hidden : " << ( hidden() ? "Yes" : "No" ) << std::endl;
}

// ========== RSTRING ==========

const unsigned int RStringRecord::id = 0x00d6;

class RStringRecord::Private
{
public:
  UString label;
};

RStringRecord::RStringRecord():
  Record(), CellInfo()
{
  d = new RStringRecord::Private();
  d->label = UString::null;
}

RStringRecord::~RStringRecord()
{
  delete d;
}

UString RStringRecord::label() const
{
  return d->label;
}

void RStringRecord::setLabel( const UString& l )
{
  d->label = l;
}

// FIXME formatting runs ? in EString perhaps ?
void RStringRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 6 ) return;

  setRow( readU16( data ) );
  setColumn( readU16( data+2 ) );
  setXfIndex( readU16( data+4 ) );

  // FIXME check Excel97
  UString label = ( version() >= Excel97 ) ?
    EString::fromUnicodeString( data+6, true, size-6 ).str() :
    EString::fromByteString( data+6, true, size-6 ).str();
  setLabel( label );
}

void RStringRecord::dump( std::ostream& out ) const
{
  out << "RSTRING" << std::endl;
  out << "                Row : " << row() << std::endl;
  out << "             Column : " << column() << std::endl;
  out << "           XF Index : " << xfIndex() << std::endl;
  out << "              Label : " << label() << std::endl;
}

// ========== SST ==========

const unsigned int SSTRecord::id = 0x00fc;

class SSTRecord::Private
{
public:
  unsigned total;
  unsigned count;
  std::vector<UString> strings;
};

SSTRecord::SSTRecord():
  Record()
{
  d = new SSTRecord::Private();
  d->total = 0;
  d->count = 0;
}

SSTRecord::~SSTRecord()
{
  delete d;
}

UString sstrecord_get_plain_string( const unsigned char* data, unsigned length )
{
  char* buffer = new char[ length+1 ];
  memcpy( buffer, data, length );
  buffer[ length ] = 0;
  UString str = UString( buffer );
  delete[] buffer;
  return str;
}

void SSTRecord::setData( unsigned size, const unsigned char* data, const unsigned int* continuePositions )
{
  if( size < 8 ) return;

  d->total = readU32( data );
  d->count = readU32( data+4 );

  unsigned offset = 8;
  unsigned int nextContinuePosIdx = 0;
  unsigned int nextContinuePos = continuePositions[0];

  d->strings.clear();
  for( unsigned i = 0; i < d->count; i++ )
  {
    // check against size
    if (offset >= size) {
      std::cerr << "Warning: reached end of SST record, but not all strings have been read!" << std::endl;
      break;
    }

    EString es = EString::fromUnicodeString( data+offset, true, size - offset, nextContinuePos - offset );
    d->strings.push_back( es.str() );
    offset += es.size();
    while( nextContinuePos < offset ) nextContinuePos = continuePositions[++nextContinuePosIdx];
  }


  // sanity check, adjust to safer condition
  if( d->count < d->strings.size() )
  {
    std::cerr << "Warning: mismatch number of string in SST record!" << std::endl;
    d->count = d->strings.size();
  }
}

unsigned SSTRecord::count() const
{
  return d->count;
}

// why not just string() ? to avoid easy confusion with std::string
UString SSTRecord::stringAt( unsigned index ) const
{
  if( index >= count()) return UString::null;
  return d->strings[ index ];
}

void SSTRecord::dump( std::ostream& out ) const
{
  out << "SST" << std::endl;
  out << "         Occurrences : " << d->total << std::endl;
  out << "              Count : " << count() << std::endl;
  for( unsigned i = 0; i < count(); i++ )
    out << "         String #" << std::setw(2) << i << " : " <<
    stringAt( i ) << std::endl;
}

// ========== STRING ==========

const unsigned int StringRecord::id = 0x0207;

class StringRecord::Private
{
public:
  UString string;
};

StringRecord::StringRecord():
  Record()
{
  d = new StringRecord::Private();
}

StringRecord::~StringRecord()
{
  delete d;
}

void StringRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 3 ) return;

  //  TODO simple string for BIFF7

  EString es = EString::fromUnicodeString( data, true, size );
  d->string = es.str();
}

UString StringRecord::ustring() const
{
  return d->string;
}

Value StringRecord::value() const
{
  return Value( d->string );
}

void StringRecord::dump( std::ostream& out ) const
{
  out << "STRING" << std::endl;
  out << "             String : " << ustring() << std::endl;
}


// ========== TOPMARGIN ==========

const unsigned int TopMarginRecord::id = 0x0028;

class TopMarginRecord::Private
{
public:
  double topMargin;
};

TopMarginRecord::TopMarginRecord():
  Record()
{
  d = new TopMarginRecord::Private();
  d->topMargin = 1.0;
}

TopMarginRecord::~TopMarginRecord()
{
  delete d;
}

double TopMarginRecord::topMargin() const
{
  return d->topMargin;
}

void TopMarginRecord::setTopMargin( double m )
{
  d->topMargin = m;
}

void TopMarginRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  if( size < 8 ) return;
  setTopMargin( readFloat64( data ) );
}

void TopMarginRecord::dump( std::ostream& out ) const
{
  out << "TOPMARGIN" << std::endl;
  out << "         Top Margin : " << topMargin() << " inches " << std::endl;
}

// ========== XF ==========

const unsigned int XFRecord::id = 0x00e0;

class XFRecord::Private
{
public:
  unsigned fontIndex;
  unsigned formatIndex;
  bool locked;
  bool formulaHidden;
  unsigned parentStyle;
  unsigned horizontalAlignment;
  unsigned verticalAlignment;
  bool textWrap;
  unsigned rotationAngle;
  bool stackedLetters;
  unsigned indentLevel;
  bool shrinkContent;
  unsigned leftBorderStyle;
  unsigned leftBorderColor;
  unsigned rightBorderStyle;
  unsigned rightBorderColor;
  unsigned topBorderStyle;
  unsigned topBorderColor;
  unsigned bottomBorderStyle;
  unsigned bottomBorderColor;
  bool diagonalTopLeft;
  bool diagonalBottomLeft;
  unsigned diagonalStyle;
  unsigned diagonalColor;
  unsigned fillPattern;
  unsigned patternForeColor;
  unsigned patternBackColor;
};

XFRecord::XFRecord():  Record()
{
  d = new XFRecord::Private();
  d->fontIndex           = 0;
  d->formatIndex         = 0;
  d->locked              = false;
  d->formulaHidden       = false;
  d->parentStyle         = 0;
  d->horizontalAlignment = Left;
  d->verticalAlignment   = VCentered;
  d->textWrap            = false;
  d->rotationAngle       = 0;
  d->stackedLetters      = 0;
  d->indentLevel         = 0;
  d->shrinkContent       = 0;
  d->leftBorderStyle     = 0;
  d->leftBorderColor     = 0;
  d->rightBorderStyle    = 0;
  d->rightBorderColor    = 0;
  d->topBorderStyle      = 0;
  d->topBorderColor      = 0;
  d->bottomBorderStyle   = 0;
  d->bottomBorderColor   = 0;
  d->diagonalTopLeft     = false;
  d->diagonalBottomLeft  = false;
  d->diagonalStyle       = 0;
  d->diagonalColor       = 0;
  d->fillPattern         = 0;
  d->patternForeColor    = 0;
  d->patternBackColor    = 0;
}

XFRecord::~XFRecord()
{
  delete d;
}

XFRecord::XFRecord( const XFRecord& xf ):  Record()
{
  d = new XFRecord::Private();
  operator=( xf );
}

XFRecord& XFRecord::operator=( const XFRecord& xf )
{
  d->fontIndex           = xf.fontIndex();
  d->formatIndex         = xf.formatIndex();
  d->locked              = xf.locked();
  d->formulaHidden       = xf.formulaHidden();
  d->parentStyle         = xf.parentStyle();
  d->horizontalAlignment = xf.horizontalAlignment();
  d->verticalAlignment   = xf.verticalAlignment();
  d->textWrap            = xf.textWrap();
  d->rotationAngle       = xf.rotationAngle();
  d->stackedLetters      = xf.stackedLetters();
  d->indentLevel         = xf.indentLevel();
  d->shrinkContent       = xf.shrinkContent();
  d->leftBorderStyle     = xf.leftBorderStyle();
  d->leftBorderColor     = xf.leftBorderColor();
  d->rightBorderStyle    = xf.rightBorderStyle();
  d->rightBorderColor    = xf.rightBorderColor();
  d->topBorderStyle      = xf.topBorderStyle();
  d->topBorderColor      = xf.topBorderColor();
  d->bottomBorderStyle   = xf.bottomBorderStyle();
  d->bottomBorderColor   = xf.bottomBorderColor();
  d->diagonalTopLeft     = xf.diagonalTopLeft();
  d->diagonalBottomLeft  = xf.diagonalBottomLeft();
  d->diagonalStyle       = xf.diagonalStyle();
  d->diagonalColor       = xf.diagonalColor();
  d->fillPattern         = xf.fillPattern();
  d->patternForeColor    = xf.patternForeColor();
  d->patternBackColor    = xf.patternBackColor();
  return *this;
}

unsigned XFRecord::fontIndex() const
{
  return d->fontIndex;
}

void XFRecord::setFontIndex( unsigned fi )
{
  d->fontIndex = fi;
}

unsigned XFRecord::formatIndex() const
{
  return d->formatIndex;
}

void XFRecord::setFormatIndex( unsigned fi )
{
  d->formatIndex = fi;
}

bool XFRecord::locked() const
{
  return d->locked;
}

void XFRecord::setLocked( bool l )
{
  d->locked = l;
}

bool XFRecord::formulaHidden() const
{
  return d->formulaHidden;
}

void XFRecord::setFormulaHidden( bool f )
{
  d->formulaHidden = f;
}

unsigned XFRecord::parentStyle() const
{
  return d->parentStyle;
}

void XFRecord::setParentStyle( unsigned p )
{
  d->parentStyle = p;
}

unsigned XFRecord::horizontalAlignment() const
{
  return d->horizontalAlignment;
}

void XFRecord::setHorizontalAlignment( unsigned ha )
{
  d->horizontalAlignment = ha;
}

const char* XFRecord::horizontalAlignmentAsString() const
{
  const char *result = "Unknown";
  switch( horizontalAlignment() )
  {
    case General:   result = "General"; break;
    case Left:      result = "Left"; break;
    case Centered:  result = "Centered"; break;
    case Right:     result = "Right"; break;
    case Justified: result = "Justified"; break;
    case Filled:    result = "Filled"; break;
    default: break;
  }
  return result;
}

unsigned XFRecord::verticalAlignment() const
{
  return d->verticalAlignment;
}

void XFRecord::setVerticalAlignment( unsigned va )
{
  d->verticalAlignment = va;
}

const char* XFRecord::verticalAlignmentAsString() const
{
  const char *result = "Unknown";
  switch( verticalAlignment() )
  {
    case Top:          result = "Top"; break;
    case VCentered:    result = "Centered"; break;
    case Bottom:       result = "Bottom"; break;
    case VJustified:   result = "Justified"; break;
    case VDistributed: result = "Distributed"; break;
    default: break;
  }
  return result;
}

bool XFRecord::textWrap() const
{
  return d->textWrap;
}

void XFRecord::setTextWrap( bool wrap )
{
  d->textWrap = wrap;
}

unsigned XFRecord::rotationAngle() const
{
  return d->rotationAngle;
}

void XFRecord::setRotationAngle( unsigned angle )
{
  d->rotationAngle = angle;
}

bool XFRecord::stackedLetters() const
{
  return d->stackedLetters;
}

void XFRecord::setStackedLetters( bool stacked )
{
  d->stackedLetters = stacked;
}

unsigned XFRecord::indentLevel() const
{
  return d->indentLevel;
}

void XFRecord::setIndentLevel( unsigned i )
{
  d->indentLevel = i;
}

bool XFRecord::shrinkContent() const
{
  return d->shrinkContent;
}

void XFRecord::setShrinkContent( bool s )
{
  d->shrinkContent = s;
}

unsigned XFRecord::leftBorderStyle() const
{
  return d->leftBorderStyle;
}

void XFRecord::setLeftBorderStyle( unsigned style )
{
  d->leftBorderStyle = style;
}

unsigned XFRecord::leftBorderColor() const
{
  return d->leftBorderColor;
}

void XFRecord::setLeftBorderColor( unsigned color )
{
  d->leftBorderColor = color;
}

unsigned XFRecord::rightBorderStyle() const
{
  return d->rightBorderStyle;
}

void XFRecord::setRightBorderStyle( unsigned style )
{
  d->rightBorderStyle = style;
}

unsigned XFRecord::rightBorderColor() const
{
  return d->rightBorderColor;
}

void XFRecord::setRightBorderColor( unsigned color )
{
  d->rightBorderColor = color;
}

unsigned XFRecord::topBorderStyle() const
{
  return d->topBorderStyle;
}

void XFRecord::setTopBorderStyle( unsigned style )
{
  d->topBorderStyle = style;
}

unsigned XFRecord::topBorderColor() const
{
  return d->topBorderColor;
}

void XFRecord::setTopBorderColor( unsigned color )
{
  d->topBorderColor = color;
}

unsigned XFRecord::bottomBorderStyle() const
{
  return d->bottomBorderStyle;
}

void XFRecord::setBottomBorderStyle( unsigned style )
{
  d->bottomBorderStyle = style;
}

unsigned XFRecord::bottomBorderColor() const
{
  return d->bottomBorderColor;
}

void XFRecord::setBottomBorderColor( unsigned color )
{
  d->bottomBorderColor = color;
}

bool XFRecord::diagonalTopLeft() const
{
  return d->diagonalTopLeft;
}

void XFRecord::setDiagonalTopLeft( bool dd )
{
  d->diagonalTopLeft = dd;
}

bool XFRecord::diagonalBottomLeft() const
{
  return d->diagonalBottomLeft;
}

void XFRecord::setDiagonalBottomLeft( bool dd )
{
  d->diagonalBottomLeft = dd;
}

unsigned XFRecord::diagonalStyle() const
{
  return d->diagonalStyle;
}

void XFRecord::setDiagonalStyle( unsigned style )
{
  d->diagonalStyle = style;
}

unsigned XFRecord::diagonalColor() const
{
  return d->diagonalColor;
}

void XFRecord::setDiagonalColor( unsigned color )
{
  d->diagonalColor = color;
}

unsigned XFRecord::fillPattern() const
{
  return d->fillPattern;
}

void XFRecord::setFillPattern( unsigned pattern )
{
  d->fillPattern = pattern;
}

unsigned XFRecord::patternForeColor() const
{
  return d->patternForeColor;
}

void XFRecord::setPatternForeColor( unsigned color )
{
  d->patternForeColor = color;
}

unsigned XFRecord::patternBackColor() const
{
  return d->patternBackColor;
}

void XFRecord::setPatternBackColor( unsigned color )
{
  d->patternBackColor = color;
}

void XFRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
  unsigned recordSize = ( version() == Excel97 ) ? 20: 16;
  if( size < recordSize ) return;

  setFontIndex( readU16( data ) );
  setFormatIndex( readU16( data+2 ) );

  unsigned protection = readU16( data+4 ) & 7;
  setLocked( protection & 1 );
  setFormulaHidden( protection & 2 );

  setParentStyle( readU16( data+4 ) >> 4 );

  unsigned align = data[6];
  setHorizontalAlignment( align & 0x07 );
  setVerticalAlignment( align >> 4 );
  setTextWrap( align & 0x08 );

  unsigned angle = data[7];
  setRotationAngle( ( angle != 255 ) ? ( angle & 0x7f ) : 0 );
  setStackedLetters( angle == 255 );

  if( version() == Excel97 )
  {
    unsigned options = data[8];
//     unsigned attributes = data[9];

    setIndentLevel( options & 0x0f );
    setShrinkContent( options & 0x10 );

    unsigned linestyle = readU16( data + 10 );
    unsigned color1 = readU16( data + 12 );
    // unsigned color2 = readU16( data + 14 );
    unsigned flag = readU16( data + 16 );
    unsigned fill = readU16( data + 18 );

    setLeftBorderStyle( linestyle & 0xf );
    setRightBorderStyle( ( linestyle >> 4 ) & 0xf );
    setTopBorderStyle( ( linestyle >> 8 ) & 0xf );
    setBottomBorderStyle( ( linestyle >> 12 ) & 0xf );

    setLeftBorderColor( color1 & 0x7f );
    setRightBorderColor( ( color1 >> 7 ) & 0x7f );
    setTopBorderColor( color1 & 0x7f );
    setBottomBorderColor( ( color1 >> 7 ) & 0x7f );

    setDiagonalTopLeft( color1 & 0x40 );
    setDiagonalBottomLeft( color1 & 0x40 );
    setDiagonalStyle( ( flag >> 4 ) & 0x1e  );
    setDiagonalColor( ( ( flag & 0x1f ) << 2 ) + (  ( color1 >> 14 ) & 3 ));

    setFillPattern( ( flag >> 10 ) & 0x3f );
    setPatternForeColor( fill & 0x7f );
    setPatternBackColor( ( fill >> 7 ) & 0x7f );
  }
  else
  {
    unsigned data1 = readU32( data + 8 );
    unsigned data2 = readU32( data + 12 );

    setPatternForeColor( data1 & 0x7f );
    setPatternBackColor( ( data1 >> 7 ) & 0x7f );
    setFillPattern( ( data1 >> 16 ) & 0x3f );

    setBottomBorderStyle( ( data1 >> 22 ) & 0x07 );
    setBottomBorderColor( ( data1 >> 25 ) & 0x7f );

    setTopBorderStyle( data2 & 0x07 );
    setLeftBorderStyle( ( data2 >> 3 ) & 0x07 );
    setRightBorderStyle( ( data2 >> 6 ) & 0x07 );

    setTopBorderColor( ( data2 >> 9 ) & 0x7f );
    setLeftBorderColor( ( data2 >> 16 ) & 0x7f );
    setRightBorderColor( ( data2 >> 23 ) & 0x7f );
  }
}

void XFRecord::dump( std::ostream& out ) const
{
  out << "XF" << std::endl;
  out << "       Parent Style : " << parentStyle() << std::endl;
  out << "         Font Index : " << fontIndex() << std::endl;
  out << "       Format Index : " << formatIndex() << std::endl;
  out << "             Locked : " << (locked()?"Yes":"No") << std::endl;
  out << " Formula Visibility : " << (formulaHidden()?"Hidden":"Visible") << std::endl;
  out << "   Horizontal Align : " << horizontalAlignmentAsString() << std::endl;
  out << "     Vertical Align : " << verticalAlignmentAsString() << std::endl;
  out << "          Text Wrap : " << ( textWrap() ? "yes" : "no" ) << std::endl;
  out << "          Rotation  : " << rotationAngle() << std::endl;
  out << "    Stacked Letters : " << ( stackedLetters() ? "yes" : "no" ) << std::endl;
  out << "       Indent Level : " << indentLevel() << std::endl;
  out << "      Shrink To Fit : " << ( shrinkContent() ? "yes" : "no" ) << std::endl;
  out << "        Left Border : Style " << leftBorderStyle();
  out << " Color: " << leftBorderColor() << std::endl;
  out << "       Right Border : Style " << rightBorderStyle();
  out << " Color: " << rightBorderColor() << std::endl;
  out << "         Top Border : Style " << topBorderStyle();
  out << " Color: " << topBorderColor() << std::endl;
  out << "      Bottom Border : Style " << bottomBorderStyle();
  out << " Color: " << bottomBorderColor() << std::endl;
  out << "     Diagonal Lines : " ;
  if ( diagonalTopLeft() ) out << "TopLeft ";
  if ( diagonalBottomLeft() ) out << "BottomLeft ";
  out << "Style " << diagonalStyle() << " Color: " << diagonalColor() << std::endl;
  out << "       Fill Pattern : " << fillPattern() << std::endl;
  out << "         Fill Color : Fore " << patternForeColor() << " Back: "
  << patternBackColor() << std::endl;
}

//=============================================
//          ExcelReader
//=============================================

class ExcelReader::Private
{
public:

  // the workbook
  Workbook* workbook;

  GlobalsSubStreamHandler* globals;

  std::vector<SubStreamHandler*> handlerStack;

  // active sheet, all cell records will be stored here
  Sheet* activeSheet;

  // for FORMULA+STRING record pair
  Cell* formulaCell;

  // mapping from font index to Swinder::FormatFont
  std::map<unsigned,FormatFont> fontCache;

  // mapping from cell position to shared formulas
  std::map<std::pair<unsigned, unsigned>, FormulaTokens> sharedFormulas;

  // for FORMULA+SHAREDFMLA record pair
  Cell* lastFormulaCell;

  // mapping from cell position to data tables
  std::map<std::pair<unsigned, unsigned>, DataTableRecord> dataTables;
};

ExcelReader::ExcelReader()
{
  d = new ExcelReader::Private();

  d->workbook    = 0;
  d->activeSheet = 0;
  d->formulaCell = 0;
  d->lastFormulaCell = 0;
  d->globals = 0;
}

ExcelReader::~ExcelReader()
{
  delete d;
}

bool ExcelReader::load( Workbook* workbook, const char* filename )
{
  POLE::Storage storage( filename );
  if( !storage.open() )
  {
    std::cerr << "Cannot open " << filename << std::endl;
    return false;
  }

#ifdef SWINDER_XLS2RAW
  std::list<std::string> entries = storage.entries();
  std::cout << "Streams:" << std::endl;
  for (std::list<std::string>::iterator it = entries.begin(); it != entries.end(); ++it)  {
    std::cout << "    /" << *it << std::endl;
  }
#endif

  unsigned streamVersion = Swinder::Excel97;
  POLE::Stream* stream;
  stream = new POLE::Stream( &storage, "/Workbook" );
  if( stream->fail() )
  {
    delete stream;
    stream = new POLE::Stream( &storage, "/Book" );
    streamVersion = Swinder::Excel95;
  }

  if( stream->fail() )
  {
    std::cerr << filename << " is not Excel workbook" << std::endl;
    delete stream;
    return false;
  }

  unsigned long stream_size = stream->size();

  unsigned int buffer_size = 65536;		// current size of the buffer
  unsigned char *buffer = (unsigned char *) malloc(buffer_size);
  unsigned char small_buffer[128];	// small, fixed size buffer
  unsigned int continuePositionsSize = 128; // size of array for continue positions
  unsigned int *continuePositions = (unsigned int *) malloc(continuePositionsSize * sizeof(int));

  workbook->clear();
  d->workbook = workbook;
  d->globals = new GlobalsSubStreamHandler( workbook, streamVersion );
  d->handlerStack.clear();

  while( stream->tell() < stream_size )
  {

    // this is set by FILEPASS record
    // subsequent records will need to be decrypted
    // since we do not support it yet, we have to bail out
    if(d->globals->passwordProtected())
    {
      d->workbook->setPasswordProtected( true );
      break;
    }

    // get record type and data size
    unsigned long pos = stream->tell();
    unsigned bytes_read = stream->read( buffer, 4 );
    if( bytes_read != 4 ) break;

    unsigned long type = readU16( buffer );
    unsigned long size = readU16( buffer + 2 );
    unsigned int continuePositionsCount = 0;

    // verify buffer is large enough to hold the record data
    if (size > buffer_size) {
        buffer = (unsigned char *) realloc(buffer, size);
        buffer_size = size;
    }

    // load actual record data
    bytes_read = stream->read( buffer, size );
    if( bytes_read != size ) break;

    // save current position in stream, to be able to restore the position later on
    unsigned long saved_pos;
    // repeatedly check if the next record is type 0x3C, a continuation record
    unsigned long next_type; // the type of the next record
    do {
        saved_pos = stream->tell();

        bytes_read = stream->read( small_buffer, 4 );
        if (bytes_read != 4) break;

        next_type = readU16( small_buffer );
        unsigned long next_size = readU16( small_buffer + 2 );

        if (next_type == 0x3C) {
            // type of next record is 0x3C, so go ahead and append the contents of the next record to the buffer
            continuePositions[continuePositionsCount++] = size;
            if (continuePositionsCount >= continuePositionsSize) {
                continuePositionsSize *= 2;
                continuePositions = (unsigned int *) realloc(continuePositions, continuePositionsSize * sizeof(int));
            }

            // first verify the buffer is large enough to hold all the data
            if ( (size + next_size) > buffer_size) {
                buffer = (unsigned char *) realloc(buffer, size + next_size);
                buffer_size = size + next_size;
            }

            // next read the data of the record
            bytes_read = stream->read( buffer + size, next_size );
            if (bytes_read != next_size) {
                std::cout << "ERROR!" << std::endl;
                break;
            }

            // and finally update size
            size += next_size;
        }
    } while (next_type == 0x3C);

    // append total size as last continue position
    continuePositions[continuePositionsCount] = size;

    // restore position in stream to the beginning of the next record
    stream->seek( saved_pos );

    // skip record type 0, this is just for filler
    if( type == 0 ) continue;

    // create the record using the factory
    Record* record = Record::create( type );

    if( record )
    {
      // setup the record and invoke handler
      record->setVersion( d->globals->version() );
      record->setData( size, buffer, continuePositions );
      record->setPosition( pos );

      if( record->rtti() == BOFRecord::id )
        handleRecord( record );
      if( !d->handlerStack.empty() && d->handlerStack.back() )
        d->handlerStack.back()->handleRecord( record );
      if( record->rtti() == EOFRecord::id )
        handleRecord( record );

#ifdef SWINDER_XLS2RAW
      std::cout << "Record 0x";
      std::cout << std::setfill('0') << std::setw(4) << std::hex << record->rtti();
      std::cout << " (";
      std::cout << std::dec;
      std::cout << record->rtti() << ") ";
      record->dump( std::cout );
      std::cout << std::endl;
#endif

      delete record;
    }

#ifdef SWINDER_XLS2RAW
    if( !record )
    {
      std::cout << "Record 0x";
      std::cout << std::setfill('0') << std::setw(4) << std::hex << type;
      std::cout << std::dec;
      std::cout << " (" << type << ")";
      std::cout << std::endl;
      std::cout << std::endl;
    }
#endif

  }

  free(buffer);

  delete stream;

  storage.close();

  return true;
}

void ExcelReader::handleRecord( Record* record )
{
  if( !record ) return;

  unsigned type = record->rtti();
  switch( type )
  {
    case BOFRecord::id:
      handleBOF( static_cast<BOFRecord*>( record ) ); break;
    case EOFRecord::id:
      handleEOF( static_cast<EOFRecord*>( record ) ); break;
    default:
      break;
  }
}

void ExcelReader::handleBOF( BOFRecord* record )
{
  if( !record ) return;

  if( record->type() == BOFRecord::Workbook )
  {
    d->handlerStack.push_back(d->globals);
  }
  else if( record->type() == BOFRecord::Worksheet )
  {
    // find the sheet and make it active
    // which sheet ? look from from previous BoundSheet
    Sheet* sheet = d->globals->sheetFromPosition( record->position() );
    if( sheet ) d->activeSheet = sheet;
    d->handlerStack.push_back( new WorksheetSubStreamHandler( sheet, d->globals ) );
  }
  else
  {
    d->handlerStack.push_back( 0 );
  }
}

void ExcelReader::handleEOF( EOFRecord* record )
{
  if( !record ) return;
  if( d->handlerStack.empty() ) return;

  SubStreamHandler* handler = d->handlerStack.back();
  d->handlerStack.pop_back();
  if (handler != d->globals) delete handler;
}

#ifdef SWINDER_XLS2RAW

#include <iostream>

int main( int argc, char ** argv )
{
  if( argc < 2 )
  {
    std::cout << "Usage: xls2raw filename" << std::endl;
    return 0;
  }

  char* filename = argv[1];
  std::cout << "Checking " << filename << std::endl;

  Workbook* workbook = new Workbook();
  ExcelReader* reader = new ExcelReader();
  reader->load( workbook, filename );
  delete reader;
  delete workbook;

  return 0;
}

#endif  // XLS2RAW
