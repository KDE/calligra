/* Sidewinder - Portable library for spreadsheet
   Copyright (C) 2003 Ariya Hidayat <ariya@kde.org>

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
   Boston, MA 02111-1307, US
*/

#include "excel.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <map>
#include <stdio.h> // memcpy

#include "pole.h"
#include "sidewinder.h"

using namespace Sidewinder;

static inline unsigned long readU16( const void* p )
{
  const unsigned char* ptr = (const unsigned char*) p;
  return ptr[0]+(ptr[1]<<8);
}

static inline unsigned long readU32( const void* p )
{
  const unsigned char* ptr = (const unsigned char*) p;
  return ptr[0]+(ptr[1]<<8)+(ptr[2]<<16)+(ptr[3]<<24);
}

// FIXME check that double is 64 bits
static inline double readFloat64( const void*p )
{
  const unsigned char* ptr = (const unsigned char*) p;
  double num = 0.0;
  *((unsigned*) &num) = readU32( ptr ); 
  *((unsigned*) &num + 1) = readU32( ptr+4 );
  return num;
}

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
    i = (int)(factor * (*((int*) &rkvalue) >> 2) );
  }
  else
  {
    // FIXME litte vs big endian ?
    isInteger = false;
    *((unsigned*) &f) = 0;  // lower 32 bits = 0
    *((unsigned*) &f + 1) = rkvalue & 0xFFFFFFFC; // bit 0, 1 = 0
    f *= factor;
  }
}

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

static inline unsigned long estring_readU16( const void* p )
{
  const unsigned char* ptr = (const unsigned char*) p;
  return ptr[0]+(ptr[1]<<8);
}

// FIXME use maxsize for sanity check
EString EString::fromUnicodeString( const void* p, unsigned maxsize )
{
  const unsigned char* data = (const unsigned char*) p;
  UString str = UString::null;
  
  unsigned len = estring_readU16( data  );
  unsigned char flag = data[ 2 ];
  
  bool unicode = flag & 0x01;
  bool richText = flag & 0x08;

  unsigned formatRuns = 0;
  
  unsigned offset = 3; // taken for len and flag
  
  unsigned size = unicode ? len*2 : len;
  
  if( richText )
  {
    formatRuns = estring_readU16( data + offset );
    offset += 2;
  }
  
  // find out total bytes used in this string
  size = 3; // length(2) + flag(1)
  size += len; // string data
  if( unicode ) size += len; // because unicode takes 2-bytes char
  if( richText ) size += (2+formatRuns*4);
  
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
    str = UString();
    for( unsigned k=0; k<len; k++ )
    {
      unsigned uchar = estring_readU16( data + offset + k*2 );
      str.append( UString(uchar) );
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
EString EString::fromByteString( const void* p, unsigned maxsize )
{
  const unsigned char* data = (const unsigned char*) p;
  UString str = UString::null;
  
  unsigned len = estring_readU16( data  );
  char* buffer = new char[ len+1 ];
  memcpy( buffer, data + 2, len );
  buffer[ len ] = 0;
  str = UString( buffer );
  
  unsigned size = 2 + len;
  
  EString result;
  result.setUnicode( false );
  result.setRichText( false );
  result.setSize( size );
  result.setStr( str );
  
  return result;
}

// FIXME use maxsize for sanity check
EString EString::fromByteString( const void* p, bool longString, 
  unsigned maxsize )
{
  const unsigned char* data = (const unsigned char*) p;
  UString str = UString::null;
  
  unsigned offset = longString ? 2 : 1;  
  unsigned len = longString ? estring_readU16( data  ): data[0];
  
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
  unsigned formatRuns = 0;
  
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
      unsigned uchar = estring_readU16( data + offset + k*2 );
      str.append( UString(uchar) );
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
//          RecordFactory
//=============================================

Record* RecordFactory::create( unsigned type )
{
  Record* record = 0;
  
  if( type == BOFRecord::id )
    record = new BOFRecord();
    
  else if( type == EOFRecord::id )
    record = new EOFRecord();
    
  if( type == BlankRecord::id )
    record = new BlankRecord();
    
  if( type == BoolErrRecord::id )
    record = new BoolErrRecord();
    
  if( type == BottomMarginRecord::id )
    record = new BottomMarginRecord();
    
  if( type == BoundSheetRecord::id )
    record = new BoundSheetRecord();
    
  if( type == ColInfoRecord::id )
    record = new ColInfoRecord();
    
  if( type == Date1904Record::id )
    record = new Date1904Record();
    
  else if( type == FormatRecord::id )
    record = new FormatRecord();
    
  else if( type == FontRecord::id )
    record = new FontRecord();
    
  else if( type == LabelRecord::id )
    record = new LabelRecord();
    
  else if( type == LabelSSTRecord::id )
    record = new LabelSSTRecord();
    
  if( type == LeftMarginRecord::id )
    record = new LeftMarginRecord();
    
  else if( type == MulBlankRecord::id )
    record = new MulBlankRecord();
    
  else if( type == MulRKRecord::id )
    record = new MulRKRecord();
    
  else if( type == NumberRecord::id )
    record = new NumberRecord();
    
  if( type == RightMarginRecord::id )
    record = new RightMarginRecord();
    
  else if( type == RKRecord::id )
    record = new RKRecord();
    
  else if( type == RStringRecord::id )
    record = new RStringRecord();
    
  else if( type == SSTRecord::id )
    record = new SSTRecord();
  
  else if( type == XFRecord::id )
    record = new XFRecord();
  
  else if( type == TopMarginRecord::id )
    record = new TopMarginRecord();
    
  return record;
};


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

void Record::setPosition( unsigned pos )
{
  stream_position = pos;
}
  
unsigned Record::position() const
{
  return stream_position;
}

void Record::setData( unsigned size, const unsigned char* data )
{
}

void Record::dump( std::ostream& out ) const
{
  // nothing to dump
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

void BOFRecord::setData( unsigned size, const unsigned char* data )
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
    case 0x05  : result = Workbook; break;
    case 0x06  : result = VBModule; break;
    case 0x10  : result = Worksheet; break;
    case 0x20  : result = Chart; break;
    default: break;
  }
  return result;
}

const char* BOFRecord::typeAsString() const
{
  const char *result = "Unknown";
  switch( type() )
  {
    case Workbook  : result = "Workbook"; break;
    case Worksheet : result = "Worksheet"; break;
    case Chart     : result = "Chart"; break;
    case VBModule  : result = "Visual Basic Module"; break;
    default: break;
  }
  return result;
}

void BOFRecord::dump( std::ostream& out ) const
{
  out << "BOF" << std::endl;
  out << "  Version : " << std::hex << d->version << " (" << versionAsString() << ")" << std::endl;
  out << "     Type : " << d->type << " (" << typeAsString() << ")" << std::endl;
  out << "    Build : " << d->build << std::endl;
  out << "     Year : " << d->year << std::endl;
  out << "  History : " << d->history << std::endl;
  out << " RVersion : " << d->rversion << std::endl;
}

// ========== BLANK ========== 

const unsigned int BlankRecord::id = 0x0201;

BlankRecord::BlankRecord():
  Record(), CellInfo()
{
}

void BlankRecord::setData( unsigned size, const unsigned char* data )
{
  if( size < 6 ) return;

  setRow( readU16( data ) );
  setColumn( readU16( data+2 ) );
  setXfIndex( readU16( data+4 ) );
}

void BlankRecord::dump( std::ostream& out ) const
{
  out << "BLANK" << std::endl;
  out << "      Row : " << row() << std::endl;
  out << "   Column : " << column() << std::endl;
  out << " XF Index : " << xfIndex() << std::endl;
}

// ========== BOOLERR ==========

const unsigned int BoolErrRecord::id = 0x0205;

class BoolErrRecord::Private
{
public:
  enum { Boolean, Error } type;
  bool value;
  int errorCode;  // raw code: 00=Null, 07=Div0, 0F=Value, ...
};

BoolErrRecord::BoolErrRecord():
  Record(), CellInfo()
{
  d = new BoolErrRecord::Private();
  d->type = Private::Boolean;
  d->value = false;
  d->errorCode = 0;
}

BoolErrRecord::~BoolErrRecord()
{
}

void BoolErrRecord::setData( unsigned size, const unsigned char* data )
{
  if( size != 8 ) return;

  setRow( readU16( data ) );
  setColumn( readU16( data+2 ) );
  setXfIndex( readU16( data+4 ) );

  switch( data[7] )
  {
  case 0 :
    d->type = Private::Boolean;
    d->value = (data[6] != 0);
    break;
  case 1 :
    d->type = Private::Error;
    d->errorCode = data[6];
    break;
  default:
    // bad bad bad
    std::cerr << "Warning: bad BOOLERR record" << std::endl;
    break;
  }
}

bool BoolErrRecord::isBool() const
{
  return d->type == Private::Boolean;
}

bool BoolErrRecord::isError() const
{
  return d->type == Private::Error;
}

bool BoolErrRecord::value() const
{
  return d->value;
}

unsigned BoolErrRecord::errorCode() const
{
  unsigned result = ErrorUnknown;
  switch( d->errorCode )
  {
    case 0x00:  result = ErrorNull; break;
    case 0x07:  result = ErrorDivZero; break;
    case 0x0f:  result = ErrorValue; break;
    case 0x17:  result = ErrorRef; break;
    case 0x1D:  result = ErrorName; break;
    case 0x24:  result = ErrorNum; break;
    case 0x2A:  result = ErrorNA; break;
    default  :  result = ErrorUnknown; break;
  };
  
  return result;
}

const char* BoolErrRecord::errorCodeAsString() const
{
  const char* result = "Unknown";
  switch( errorCode() )
  {
    case ErrorNull    : result = "Intersection of two cell ranges is empty"; break;
    case ErrorDivZero : result = "Division by zero"; break;
    case ErrorValue   : result = "Wrong type of operand"; break;
    case ErrorRef     : result = "Illegal or deleted cell reference"; break;
    case ErrorName    : result = "Wrong function or range name"; break;
    case ErrorNum     : result = "Value range overflow"; break;
    case ErrorNA      : result = "Argument or function not available"; break;
    default: result = "Unknown" ; break;
  }
  return result;
}


void BoolErrRecord::dump( std::ostream& out ) const
{
  out << "BOOLERR" << std::endl;
  out << "   Column : " << column() << std::endl;
  out << "      Row : " << row() << std::endl;
  out << "  XFIndex : " << xfIndex() << std::endl;
  if( isBool() )
  {
    out << "     Type : Bool" << std::endl;
    std::string val = value() ? "True" : "False";
    out << "    Value : " << val << std::endl;
  }
  if( isError() )
  {
    out << "     Type : Error" << std::endl;
    out << "ErrorCode : " << std::hex << d->errorCode << " ";
    out << errorCodeAsString() << std::endl;
  }
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

void BottomMarginRecord::setData( unsigned size, const unsigned char* data )
{
  if( size < 8 ) return;
  setBottomMargin( readFloat64( data ) );
}

void BottomMarginRecord::dump( std::ostream& out ) const
{
  out << "BOTTOMMARGIN" << std::endl;
  out << "   Margin : " << bottomMargin() << std::endl;
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

void BoundSheetRecord::setData( unsigned size, const unsigned char* data )
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
  out << "       Name : " << d->name.ascii() << std::endl;
  out << "       Type : " << d->type << " (" << typeAsString() << ")" << std::endl;
  out << " Visibility : " << d->visibility << " (";
  if( visible() ) out << "Visible"; else out << "Hidden"; out << ")" << std::endl;
  out << "    BOF pos : " << d->bofPosition << std::endl;
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

void ColInfoRecord::setData( unsigned size, const unsigned char* data )
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
  out << "  First Column : " << firstColumn() << std::endl;
  out << "   Last Column : " << lastColumn() << std::endl;
  out << "         Width : " << width() << std::endl;
  out << "      XF Index : " << xfIndex() << std::endl;
  out << "        Hidden : " << ( hidden() ? "Yes" : "No" ) << std::endl;
  out << "     Collapsed : " << ( collapsed() ? "Yes" : "No" ) << std::endl;
  out << " Outline Level : " << outlineLevel() << std::endl;  
}


// ========== DATE1904 ========== 

const unsigned int Date1904Record::id = 0x0022;

class Date1904Record::Private
{
public:
  bool ref1904;
};

Date1904Record::Date1904Record():
  Record()
{
  d = new Date1904Record::Private();
  d->ref1904 = false;
}

Date1904Record::~Date1904Record()
{
  delete d;
}

bool Date1904Record::ref1904() const
{
  return d->ref1904;
}

void Date1904Record::setRef1904( bool r )
{
  d->ref1904 = r;
}

void Date1904Record::setData( unsigned size, const unsigned char* data )
{
  if( size < 2 ) return;
  
  unsigned flag = readU16( data );
  d->ref1904 = flag != 0;
}

void Date1904Record::dump( std::ostream& out ) const
{
  out << "DATE1904" << std::endl;
  out << " 1904 ref : " << (ref1904() ? "Yes" : "No") << std::endl;
}


// ========== DIMENSION ========== 

DimensionRecord::DimensionRecord():
  Record()
{
}

void DimensionRecord::setData( unsigned size, const unsigned char* data )
{
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

void EOFRecord::setData( unsigned size, const unsigned char* data )
{
  // no data associated with EOF record
}

void EOFRecord::dump( std::ostream& out ) const
{
  out << "EOF" << std::endl;
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
  unsigned script;
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
  d->script       = Normal;
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
  d->script       = ef.script();
  d->underline    = ef.underline();
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

unsigned FontRecord::script() const
{
  return d->script;
}

void FontRecord::setScript( unsigned s )
{
  d->script = s;
}

unsigned FontRecord::underline() const
{
  return d->underline;
}

void FontRecord::setUnderline( unsigned u )
{
  d->underline = u;
}


void FontRecord::setData( unsigned size, const unsigned char* data )
{
  if( size < 14 ) return;
  
  setHeight( readU16( data ) );
  unsigned flag = readU16( data+2 );
  setItalic( flag & 2 );
  setStrikeout( flag & 8 );
  setStrikeout( flag & 8 );
  
  setColorIndex( readU16( data+4 ) );
  
  setBoldness( readU16( data+6 ) );
  setScript( readU16( data+8 ) );
  setUnderline( data[10] );
  
  setFontFamily( data[11] );
  setCharacterSet( data[12] );
    
  // FIXME how about excel < 97 ?
  UString fn = EString::fromSheetName( data+14, size-14 ).str();
  setFontName( fn );
}


void FontRecord::dump( std::ostream& out ) const
{
  out << "FONT" << std::endl;
  out << "  Height : " << height() << std::endl;
  out << " Font Name: " << fontName().ascii() << std::endl;
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

void FormatRecord::setData( unsigned size, const unsigned char* data )
{
  if( size < 3 ) return;
  
  setIndex( readU16( data ) );

  // FIXME how about excel < 97 ?
  UString fs = EString::fromUnicodeString( data+2, size-2 ).str();
  setFormatString( fs );
  
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

void LabelRecord::setData( unsigned size, const unsigned char* data )
{
  if( size < 6 ) return;

  setRow( readU16( data ) );
  setColumn( readU16( data+2 ) );
  setXfIndex( readU16( data+4 ) );
  
  UString label = ( version() >= Excel97 ) ?
    EString::fromUnicodeString( data+6, size-6 ).str() :
    EString::fromByteString( data+6, size-6 ).str();
  setLabel( label );
}

void LabelRecord::dump( std::ostream& out ) const
{
  out << "LABEL" << std::endl;
  out << "      Row : " << row() << std::endl;
  out << "   Column : " << column() << std::endl;
  out << " XF Index : " << xfIndex() << std::endl;
  out << "    Label : " << label().ascii() << std::endl;
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

void LabelSSTRecord::setData( unsigned size, const unsigned char* data )
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
  out << "      Row : " << row() << std::endl;
  out << "   Column : " << column() << std::endl;
  out << " XF Index : " << xfIndex() << std::endl;
  out << "SST Index : " << d->sstIndex << std::endl;
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

void LeftMarginRecord::setData( unsigned size, const unsigned char* data )
{
  if( size < 8 ) return;
  setLeftMargin( readFloat64( data ) );
}

void LeftMarginRecord::dump( std::ostream& out ) const
{
  out << "LEFTMARGIN" << std::endl;
  out << "   Margin : " << leftMargin() << std::endl;
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

void MulBlankRecord::setData( unsigned size, const unsigned char* data )
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
  out << "          Row : " << row() << std::endl;
  out << " First Column : " << firstColumn() << std::endl;
  out << "  Last Column : " << lastColumn() << std::endl;
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

void MulRKRecord::setData( unsigned size, const unsigned char* data )
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
  out << "          Row : " << row() << std::endl;
  out << " First Column : " << firstColumn() << std::endl;
  out << "  Last Column : " << lastColumn() << std::endl;
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
void NumberRecord::setData( unsigned size, const unsigned char* data )
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
  out << "      Row : " << row() << std::endl;
  out << " XF Index : " << xfIndex() << std::endl;
  out << "    Value : " << number() << std::endl;
}

// ========== RIGHTMARGIN ========== 

const unsigned int RightMarginRecord::id = 0x0026;

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

void RightMarginRecord::setData( unsigned size, const unsigned char* data )
{
  if( size < 8 ) return;
  setRightMargin( readFloat64( data ) );
}

void RightMarginRecord::dump( std::ostream& out ) const
{
  out << "RIGHTMARGIN" << std::endl;
  out << "   Margin : " << rightMargin() << std::endl;
}

// ========== RK ========== 

const unsigned int RKRecord::id = 0x027e;

class RKRecord::Private
{
public:
  bool integer;
  int i;
  double f;
};

RKRecord::RKRecord():
  Record(), CellInfo()
{
  d = new RKRecord::Private();
  d->integer = true;
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

// FIXME check sizeof(int) is 32
// big vs little endian problem
void RKRecord::setData( unsigned size, const unsigned char* data )
{
  if( size < 10 ) return;

  setRow( readU16( data ) );
  setColumn( readU16( data+2 ) );
  setXfIndex( readU16( data+4 ) );

  int i = 0; double f = 0.0;
  decodeRK( readU32( data+6 ), d->integer, i, f );
  if( d->integer ) setInteger( i );
  else setFloat( f );
}

void RKRecord::dump( std::ostream& out ) const
{
  out << "RK" << std::endl;
  out << "      Row : " << row() << std::endl;
  out << "   Column : " << column() << std::endl;
  out << " XF Index : " << xfIndex() << std::endl;
  out << "    Value : " << asFloat() << std::endl;
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
void RStringRecord::setData( unsigned size, const unsigned char* data )
{
  if( size < 6 ) return;

  setRow( readU16( data ) );
  setColumn( readU16( data+2 ) );
  setXfIndex( readU16( data+4 ) );
  
  // FIXME check Excel97
  UString label = ( version() >= Excel97 ) ?
    EString::fromUnicodeString( data+6, size-6 ).str() :
    EString::fromByteString( data+6, true, size-6 ).str();
  setLabel( label );
}

void RStringRecord::dump( std::ostream& out ) const
{
  out << "RSTRING" << std::endl;
  out << "      Row : " << row() << std::endl;
  out << "   Column : " << column() << std::endl;
  out << " XF Index : " << xfIndex() << std::endl;
  out << "    Label : " << label().ascii() << std::endl;
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

void SSTRecord::setData( unsigned size, const unsigned char* data )
{
  if( size < 8 ) return;
  
  d->total = readU32( data );
  d->count = readU32( data+4 );
  
  unsigned offset = 8;
  d->strings.clear();
  
  //TODO check against size !!
  for( unsigned i = 0; i < d->count; i++ )
  {
    EString es = EString::fromUnicodeString( data+offset );
    d->strings.push_back( es.str() );
    offset += es.size();
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
  out << " Occurences : " << d->total << std::endl;
  out << "      Count : " << count() << std::endl;
  if( count() )
  {
    out << "    Strings : " << std::endl;
    for( unsigned i = 0; i < count(); i++ )
    {
      out << "          " << i+1;
      out << " : " << stringAt( i ).ascii() << std::endl;
    }  
  }
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

void TopMarginRecord::setData( unsigned size, const unsigned char* data )
{
  if( size < 8 ) return;
  setTopMargin( readFloat64( data ) );
}

void TopMarginRecord::dump( std::ostream& out ) const
{
  out << "TOPMARGIN" << std::endl;
  out << "   Margin : " << topMargin() << std::endl;
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
};

XFRecord::XFRecord():  Record()
{
  d = new XFRecord::Private();
  d->fontIndex = 0;
  d->formatIndex = 0;
  d->locked = false;
  d->formulaHidden = false;
  d->parentStyle = 0;
  d->horizontalAlignment = Left;
  d->verticalAlignment = VCentered;
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

void XFRecord::setData( unsigned size, const unsigned char* data )
{
  if( size < 20 ) return;
  
  setFontIndex( readU16( data ) ); 
  setFormatIndex( readU16( data+2 ) );
  
  unsigned protection = readU16( data+4 ) & 7;
  setLocked( protection & 1 );
  setFormulaHidden( protection & 2 );
  
  setParentStyle( readU16( data+4 ) >> 4 );
  
  unsigned align = data[6];
  setHorizontalAlignment( align & 0x07 );
  setVerticalAlignment( align >> 4 );
  
}

void XFRecord::dump( std::ostream& out ) const
{
  out << "XF" << std::endl;
  out << " Hor-Align : " << horizontalAlignmentAsString() << std::endl;
}

//=============================================
//          ExcelReader
//=============================================

class ExcelReader::Private
{
public:

  // the workbook
  Workbook* workbook;

  // active sheet, all cell records will be stored here
  Sheet* activeSheet;
  
  // mapping from BOF pos to actual Sheet
  std::map<unsigned,Sheet*> bofMap;
  
  // shared-string table
  std::vector<UString> stringTable;
  
  // table of format
  std::map<unsigned,FormatRecord> formatTable;
  
  // table of font
  std::vector<FontRecord> fontTable;
  
  // table of Xformat
  std::vector<XFRecord> xfTable;
};

ExcelReader::ExcelReader(): Reader()
{
  d = new ExcelReader::Private();
  d->workbook = 0;
}

ExcelReader::~ExcelReader()
{
  delete d;
}

Workbook* 
ExcelReader::load(
const char* filename )
{
  POLE::Storage storage;
  if( !storage.open( filename ) )
  {
    std::cerr << "Cannot open " << filename << std::endl;
    //return Error::CannotOpen;    
    setResult( CannotOpen );
    return (Workbook*)0;
  }
  
  // TODO check ole correctness

  POLE::Stream* stream;
  stream = storage.stream( "Workbook" ); // Excel 97/2000/XP/2003
  if( !stream ) stream = storage.stream( "Book" ); // Excel 5.0/7.0
  
  if( !stream )
  {
    std::cerr << filename << " is not Excel doc" << std::endl;
    setResult( CannotOpen );
    return (Workbook*)0;
  }

  unsigned long stream_size = stream->size();
  
  // FIXME
  unsigned char buffer[65536];
  
  d->workbook = new Workbook();
  
  // assume
  unsigned version = Sidewinder::Excel97;

  while( stream->tell() < stream_size )
  {
    // get record type and data size
    unsigned long pos = stream->tell();
    unsigned bytes_read = stream->read( buffer, 4 );
    if( bytes_read != 4 ) break;
    
    unsigned long type = readU16( buffer );
    unsigned long size = readU16( buffer + 2 );
    
    // load actual record data
    bytes_read = stream->read( buffer, size );
    if( bytes_read != size ) break;
    
    // skip record type 0, this is just for filler
    if( type == 0 ) continue;
    
    // create the record using the factory
    Record* record = RecordFactory::create( type );

    if( record )
    {
      // setup the record and invoke handler
      record->setVersion( version );
      record->setData( size, buffer );
      record->setPosition( pos );
      handleRecord( record );
      
      // special handling to find Excel version
      BOFRecord* bof = dynamic_cast<BOFRecord*>(record);
      if( bof ) if( bof->type() == BOFRecord::Workbook )
        version = bof->version();

      //record->dump( std::cout );

      delete record;
    }

  }

  delete stream;
  
  storage.close();
  
  setResult( Ok );
  return d->workbook;
}

void ExcelReader::handleRecord( Record* record )
{
  if( !record ) return;

  handleBoundSheet( dynamic_cast<BoundSheetRecord*>( record ) );
  handleBOF( dynamic_cast<BOFRecord*>( record ) );
  handleBoolErr( dynamic_cast<BoolErrRecord*>( record ) );
  handleBlank( dynamic_cast<BlankRecord*>( record ) );
  handleColInfo( dynamic_cast<ColInfoRecord*>( record ) );
  handleFormat( dynamic_cast<FormatRecord*>( record ) );
  handleFont( dynamic_cast<FontRecord*>( record ) );
  handleLabel( dynamic_cast<LabelRecord*>( record ) );
  handleLabelSST( dynamic_cast<LabelSSTRecord*>( record ) );
  handleMulBlank( dynamic_cast<MulBlankRecord*>( record ) );
  handleMulRK( dynamic_cast<MulRKRecord*>( record ) );
  handleNumber( dynamic_cast<NumberRecord*>( record ) );
  handleRK( dynamic_cast<RKRecord*>( record ) );
  handleRString( dynamic_cast<RStringRecord*>( record ) );
  handleSST( dynamic_cast<SSTRecord*>( record ) );
  handleXF( dynamic_cast<XFRecord*>( record ) );
}


// FIXME does the order of sheet follow BOUNDSHEET of BOF(Worksheet) ?
// for now, assume BOUNDSHEET, hence we should create the sheet here
void ExcelReader::handleBoundSheet( BoundSheetRecord* record )
{
  if( !record ) return;
  
  // only care for Worksheet, forget everything else
  if( record->type() == BoundSheetRecord::Worksheet )
  {
    // create a new sheet
    Sheet* sheet = new Sheet( d->workbook );
    sheet->setName( record->sheetName() );
    sheet->setVisible( record->visible() );

    d->workbook->appendSheet( sheet );

    // update bof position map
    unsigned bofPos = record->bofPosition();
    d->bofMap[ bofPos ] = sheet;
  }
}

void ExcelReader::handleBOF( BOFRecord* record )
{
  if( !record ) return;
  
  if( record->type() == BOFRecord::Worksheet )
  {
    // find the sheet and make it active
    // which sheet ? look from from previous BoundSheet
    Sheet* sheet = d->bofMap[ record->position() ];
    if( sheet ) d->activeSheet = sheet;
  }
}

void ExcelReader::handleBoolErr( BoolErrRecord* record )
{
  if( !record ) return;
  
  if( !d->activeSheet ) return;
  
  unsigned column = record->column();
  unsigned row = record->row();
  unsigned xfIndex = record->xfIndex();
  
  Value value;
  if( record->isBool() )
    value.setValue( record->value() );
  else
  {
    switch( record->errorCode() )
    {
      case BoolErrRecord::ErrorNull:    value = Value::errorNULL(); break;
      case BoolErrRecord::ErrorDivZero: value = Value::errorDIV0(); break;
      case BoolErrRecord::ErrorValue:   value = Value::errorVALUE(); break;
      case BoolErrRecord::ErrorRef:     value = Value::errorREF(); break;
      case BoolErrRecord::ErrorName:    value = Value::errorNAME(); break;
      case BoolErrRecord::ErrorNum:     value = Value::errorNUM(); break;
      case BoolErrRecord::ErrorNA:      value = Value::errorNA(); break;
      default: break;
    }
  }
  
  Cell* cell = d->activeSheet->cell( column, row, true );
  if( cell )
  {
    cell->setValue( value );
    cell->setFormat( convertFormat( xfIndex ) );
  }
}

void ExcelReader::handleBlank( BlankRecord* record )
{
  if( !record ) return;
  
  if( !d->activeSheet ) return;
  
  unsigned column = record->column();
  unsigned row = record->row();
  unsigned xfIndex = record->xfIndex();
  
  Cell* cell = d->activeSheet->cell( column, row, true ); 
  if( cell )
  {
    cell->setFormat( convertFormat( xfIndex ) );
  }
}

void ExcelReader::handleColInfo( ColInfoRecord* record )
{
  if( !record ) return;
  
  // FIXME do something here !
}

void ExcelReader::handleDate1904( Date1904Record* record )
{
  if( !record ) return;
  
  // FIXME FIXME what to do ??
  std::cerr << "WARNING: Workbook uses unsupported 1904 Date System " << std::endl;
}

void ExcelReader::handleLabel( LabelRecord* record )
{
  if( !record ) return;
  
  if( !d->activeSheet ) return;
  
  unsigned column = record->column();
  unsigned row = record->row();  
  unsigned xfIndex = record->xfIndex();
  UString label = record->label();
  
  Cell* cell = d->activeSheet->cell( column, row, true );
  if( cell )
  {
    cell->setValue( Value( label ) );
    cell->setFormat( convertFormat( xfIndex ) );
  }
}

void ExcelReader::handleFormat( FormatRecord* record )
{
  if( !record ) return;
  
  d->formatTable[ record->index() ] = *record;
}

void ExcelReader::handleFont( FontRecord* record )
{
  if( !record ) return;
  
  d->fontTable.push_back( *record );

  // font #4 is never used, so add a dummy one
  if( d->fontTable.size() == 4 )
    d->fontTable.push_back( FontRecord() );
  
}

void ExcelReader::handleLabelSST( LabelSSTRecord* record )
{
  if( !record ) return;
  
  if( !d->activeSheet ) return;
  
  unsigned column = record->column();
  unsigned row = record->row();  
  unsigned index = record->sstIndex();
  unsigned xfIndex = record->xfIndex();
  
  UString str;
  if( index < d->stringTable.size() )
    str = d->stringTable[ index ];
   
  Cell* cell = d->activeSheet->cell( column, row, true );
  if( cell )
  {
    cell->setValue( Value( str ) );
    cell->setFormat( convertFormat( xfIndex) );
  }
}

void ExcelReader::handleMulBlank( MulBlankRecord* record )
{
  if( !record ) return;

  if( !d->activeSheet ) return;
  
  unsigned firstColumn = record->firstColumn();
  unsigned lastColumn = record->lastColumn();
  unsigned row = record->row();
  
  for( unsigned column = firstColumn; column <= lastColumn; column++ )
  {
    Cell* cell = d->activeSheet->cell( column, row, true );
    if( cell )
    {
      cell->setFormat( convertFormat( record->xfIndex( column - firstColumn ) ) );
    }
  }
}

void ExcelReader::handleMulRK( MulRKRecord* record )
{
  if( !record ) return;
  
  if( !d->activeSheet ) return;
  
  unsigned firstColumn = record->firstColumn();
  unsigned lastColumn = record->lastColumn();
  unsigned row = record->row();
  
  for( unsigned column = firstColumn; column <= lastColumn; column++ )
  {
    Cell* cell = d->activeSheet->cell( column, row, true );    
    if( cell )
    {
      unsigned i = column - firstColumn;
      Value value;
      if( record->isInteger( i ) )
        value.setValue( record->asInteger( i ) );
      else
        value.setValue( record->asFloat( i ) );
      cell->setValue( value );
      cell->setFormat( convertFormat( record->xfIndex( column-firstColumn ) ) );
    }
  }
}

void ExcelReader::handleNumber( NumberRecord* record )
{
  if( !record ) return;
  
  if( !d->activeSheet ) return;
  
  unsigned column = record->column();
  unsigned row = record->row();
  unsigned xfIndex = record->xfIndex();
  double number = record->number();
  
  Cell* cell = d->activeSheet->cell( column, row, true );
  if( cell )
  {
    cell->setValue( Value( number ) );
    cell->setFormat( convertFormat( xfIndex) );
  }
}



void ExcelReader::handleRK( RKRecord* record )
{
  if( !record ) return;
  
  if( !d->activeSheet ) return;
  
  unsigned column = record->column();
  unsigned row = record->row();
  unsigned xfIndex = record->xfIndex();
  
  Value value;
  if( record->isInteger() )
    value.setValue( record->asInteger() );
  else
    value.setValue( record->asFloat() );
  
  Cell* cell = d->activeSheet->cell( column, row, true );
  if( cell )
  {
    cell->setValue( value );
    cell->setFormat( convertFormat( xfIndex) );
  }
}

void ExcelReader::handleRString( RStringRecord* record )
{
  if( !record ) return;
  
  if( !d->activeSheet ) return;
  
  unsigned column = record->column();
  unsigned row = record->row();  
  unsigned xfIndex = record->xfIndex();
  UString label = record->label();
  
  Cell* cell = d->activeSheet->cell( column, row, true );
  if( cell )
  {
    cell->setValue( Value( label ) );
    cell->setFormat( convertFormat( xfIndex) );
  }
}


void ExcelReader::handleSST( SSTRecord* record )
{
  if( !record ) return;
  
  d->stringTable.clear();
  for( unsigned i = 0; i < record->count();i++ )
  {
    UString str = record->stringAt( i );
    d->stringTable.push_back( str );
  }
  
}

// big task: convert Excel XFormat into Sidewinder::Format
Format ExcelReader::convertFormat( unsigned xfIndex )
{
  Format format;
  
  if( xfIndex >= d->xfTable.size() ) return format;
  
  XFRecord xf = d->xfTable[ xfIndex ];  
  unsigned fontIndex = xf.fontIndex();
  if( fontIndex < d->fontTable.size() )
  {
    FontRecord fr = d->fontTable[ fontIndex ];
    format.font().setFontSize( fr.height() / 20.0 );
    format.font().setFontFamily( fr.fontName() );
    format.font().setBold( fr.boldness() > 500 );
    format.font().setItalic( fr.italic() );
    format.font().setStrikeout( fr.strikeout() );    
    format.font().setSubscript( fr.script() == FontRecord::Subscript );
    format.font().setSuperscript( fr.script() == FontRecord::Superscript );
    format.font().setUnderline( fr.underline() != FontRecord::None );
  }

  switch( xf.horizontalAlignment() )
  {
    case XFRecord::Left:     
      format.alignment().setAlignX( Format::Left ); break;
    case XFRecord::Right:    
      format.alignment().setAlignX( Format::Right ); break;
    case XFRecord::Centered: 
      format.alignment().setAlignX( Format::Center ); break;
    default: break;
    // FIXME still unsupported: Repeat, Justified, Filled, Distributed
  };

  return format;
}


void ExcelReader::handleXF( XFRecord* record )
{
  if( !record ) return;
  
  d->xfTable.push_back( *record );  
}


