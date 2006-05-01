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
#include <stdio.h> // memcpy

#include "pole.h"
#include "swinder.h"

// Use anonymous namespace to cover following functions
namespace{

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

typedef double& data_64;
inline void convert_64 (data_64 convert)
{
  register unsigned char temp;
  register unsigned int u_int_temp;
  temp = ((unsigned char*)&convert)[0];
  ((unsigned char*)&convert)[0] = ((unsigned char*)&convert)[3];
  ((unsigned char*)&convert)[3] = temp;
  temp = ((unsigned char*)&convert)[1];
  ((unsigned char*)&convert)[1] = ((unsigned char*)&convert)[2];
  ((unsigned char*)&convert)[2] = temp;
  temp = ((unsigned char*)&convert)[4];
  ((unsigned char*)&convert)[4] = ((unsigned char*)&convert)[7];
  ((unsigned char*)&convert)[7] = temp;
  temp = ((unsigned char*)&convert)[5];
  ((unsigned char*)&convert)[5] = ((unsigned char*)&convert)[6];
  ((unsigned char*)&convert)[6] = temp;
       
  u_int_temp = ((unsigned int *)&convert)[0];
  ((unsigned int *)&convert)[0] = ((unsigned int *)&convert)[1];
  ((unsigned int *)&convert)[1] = u_int_temp;
}

inline bool isLittleEndian(void)
{
  long i = 0x44332211;
  unsigned char* a = (unsigned char*) &i;
  return ( *a == 0x11 );
}


// FIXME check that double is 64 bits
static inline double readFloat64( const void*p )
{
  const double* ptr = (const double*) p;
  double num = 0.0;
  num = *ptr;
 
  if( !isLittleEndian() )
    convert_64( num );
    
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
    // TODO ensure double takes 8 bytes
    isInteger = false;
    unsigned char* s = (unsigned char*) &rkvalue;
    unsigned char* r = (unsigned char*) &f;
    if( isLittleEndian() )
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
std::ostream& operator<<( std::ostream& s, Swinder::UString ustring )
{
  char* str = ustring.ascii();
  s << str;
  return s;
}

}

using namespace Swinder;

static Value errorAsValue( int errorCode )
{
  Value result( Value::Error );
  
  switch( errorCode )
  {
    case 0x00: result = Value::errorNULL();  break;
    case 0x07: result = Value::errorDIV0();  break;
    case 0x0f: result = Value::errorVALUE(); break;
    case 0x17: result = Value::errorREF();   break;
    case 0x1d: result = Value::errorNAME();  break;
    case 0x24: result = Value::errorNUM();   break;
    case 0x2A: result = Value::errorNA();    break;
    default: break;
  };
  
  return result;
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

// FIXME use maxsize for sanity check
EString EString::fromUnicodeString( const void* p, bool longString, unsigned /* maxsize */ )
{
  const unsigned char* data = (const unsigned char*) p;
  UString str = UString::null;
  
  unsigned offset = longString ? 2 : 1;  
  unsigned len = longString ? readU16( data  ): data[0];
  unsigned char flag = data[ offset ];
  offset++; // for flag (1 byte)
  
  bool unicode = flag & 0x01;
  bool richText = flag & 0x08;
  unsigned formatRuns = 0;
  
  if( richText )
  {
    formatRuns = readU16( data + offset );
    offset += 2;
  }
  
  // find out total bytes used in this string
  unsigned size = offset + len; // string data
  if( unicode ) size += len; // because unicode takes 2-bytes char
  if( richText ) size += (formatRuns*4);
  
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
      unsigned uchar = readU16( data + offset + k*2 );
      str.append( UString(UChar(uchar)) );
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
//          FormulaToken
//=============================================

class FormulaToken::Private
{
public:
  unsigned ver;
  unsigned id;
  std::vector<unsigned char> data;
};

FormulaToken::FormulaToken()
{
  d = new Private;
  d->ver = Excel97;
  d->id = Unused;
}

FormulaToken::FormulaToken( unsigned t )
{
  d = new Private;
  d->ver = Excel97;
  d->id = t;
}

FormulaToken::FormulaToken( const FormulaToken& token )
{
  d = new Private;
  d->ver = token.d->ver;
  d->id = token.id();
  
  d->data.resize( token.d->data.size() );
  for( unsigned i = 0; i < d->data.size(); i++ )
    d->data[i] = token.d->data[i];
}

FormulaToken::~FormulaToken()
{
  delete d;
}

unsigned FormulaToken::version() const
{
  return d->ver;
}

void FormulaToken::setVersion( unsigned v )
{
  d->ver = v;
}

unsigned FormulaToken::id() const
{
  return d->id;
}

const char* FormulaToken::idAsString() const
{
  const char* s = 0;
  
  switch( d->id )
  {
    case Matrix:       s = "Matrix"; break;
    case Table:        s = "Table"; break;
    case Add:          s = "Add"; break;
    case Sub:          s = "Sub"; break;
    case Mul:          s = "Mul"; break;
    case Div:          s = "Div"; break;
    case Power:        s = "Power"; break;
    case Concat:       s = "Concat"; break;
    case LT:           s = "LT"; break;
    case LE:           s = "LE"; break;
    case EQ:           s = "EQ"; break;
    case GE:           s = "GE"; break;
    case GT:           s = "GT"; break;
    case NE:           s = "NE"; break;
    case Intersect:    s = "Intersect"; break;
    case List:         s = "List"; break;
    case Range:        s = "Range"; break;
    case UPlus:        s = "UPlus"; break;
    case UMinus:       s = "UMinus"; break;
    case Percent:      s = "Percent"; break;
    case Paren:        s = "Paren"; break;
    case String:       s = "String"; break;
    case MissArg:      s = "MissArg"; break;
    case ErrorCode:    s = "ErrorCode"; break;
    case Bool:         s = "Bool"; break;
    case Integer:      s = "Integer"; break;
    case Array:        s = "Array"; break;
    case Function:     s = "Function"; break;
    case FunctionVar:  s = "FunctionVar"; break;
    case Name:         s = "Name"; break;
    case Ref:          s = "Ref"; break;
    case RefErr:       s = "RefErr"; break;
    case RefN:         s = "RefN"; break;
    case Area:         s = "Area"; break;
    case AreaErr:      s = "AreaErr"; break;
    case AreaN:        s = "AreaN"; break;
    case NameX:        s = "NameX"; break;
    case Ref3d:        s = "Ref3d"; break;
    case RefErr3d:     s = "RefErr3d"; break;
    case Float:        s = "Float"; break;
    case Area3d:       s = "Area3d"; break;
    case AreaErr3d:    s = "AreaErr3d"; break;
    default:           s = "Unknown"; break;
  };
  
  return s;
}


unsigned FormulaToken::size() const
{
  unsigned s = 0; // on most cases no data
  
  switch( d->id )
  {
    case Add: 
    case Sub: 
    case Mul: 
    case Div:
    case Power:
    case Concat:
    case LT:
    case LE:
    case EQ:
    case GE:
    case GT:
    case NE:
    case Intersect:
    case List:
    case Range:
    case UPlus:
    case UMinus:
    case Percent:
    case Paren:
    case MissArg:
      s = 0; break;

    case Attr:
      s = 3; break;
      
    case ErrorCode:
    case Bool:
      s = 1; break;  
    
    case Integer:
      s = 2; break;
      
    case Array:
      s = 7; break;
    
    case Function:
      s = 2;
      break;
    
    case FunctionVar:
      s = 3;
      break;
    
    case Matrix:
    case Table:  
      s = (d->ver == Excel97) ? 4 : 3;
      break;
      
    case Name:
      s = (d->ver == Excel97) ? 4 : 14;
      break;
    
    case Ref:
    case RefErr:
    case RefN:
      s = (d->ver == Excel97) ? 4 : 3;
      break;
    
    case Area:
    case AreaErr:
    case AreaN:
      s = (d->ver == Excel97) ? 8 : 6;
      break;
    
    case NameX:
      s = (d->ver == Excel97) ? 6 : 24;
      break;
    
    case Ref3d:
    case RefErr3d:
      s = (d->ver == Excel97) ? 6 : 27;
      break;
      
    case Float:
      s = 8; break;  
      
    case Area3d:
    case AreaErr3d:
      s = (d->ver == Excel97) ? 10 : 20;
      break;  
      
    default:
      // WARNING this is unhandled case
      break;
  };
  
  return s;
}

void FormulaToken::setData( unsigned size, const unsigned char* data )
{
  d->data.resize( size );
  for( unsigned i = 0; i < size; i++ )
    d->data[i] = data[i];
}

Value FormulaToken::value() const
{
  Value result;

  unsigned char* buf;
  buf = new unsigned char[d->data.size()];
  for( unsigned k=0; k<d->data.size(); k++ )
    buf[k] = d->data[k];

  // FIXME sanity check: verify size of data  
  switch( d->id )
  {
    case ErrorCode:
      result = errorAsValue( buf[0] );
      break;
    
    case Bool:    
      result = Value( buf[0]!=0 );
      break;
      
    case Integer:
      result = Value( (int)readU16( buf ) );
      break;
      
    case Float:  
      result = Value( readFloat64( buf ) );
      break;
     
    case String:
      {
        EString estr = (version()==Excel97) ? 
          EString::fromUnicodeString( buf, false, d->data.size() ) :
          EString::fromByteString( buf, false, d->data.size() );
        result = Value( estr.str() );  
      }
      break;  
      
    default: break;  
  }
  
  delete [] buf;
  
  return result;  
}

unsigned FormulaToken::functionIndex() const
{
  // FIXME check data size
  unsigned index = 0;
  unsigned char buf[2];

  if( d->id == Function )
  {
    buf[0] = d->data[0];
    buf[1] = d->data[1];
    index = readU16( buf );
  }

  if( d->id == FunctionVar )
  {
    buf[0] = d->data[1];
    buf[1] = d->data[2];
    index = readU16( buf );
  }

  return index;
}

struct FunctionEntry
{
	const char *name;
	int params;
};

static const FunctionEntry FunctionEntries[] =
{
  { "COUNT",           1 },     // 0
  { "IF",              0 },     // 1
  { "ISNV",            1 },     // 2
  { "ISERROR",         1 },     // 3
  { "SUM",             0 },     // 4
  { "AVERAGE",         0 },     // 5
  { "MIN",             0 },     // 6
  { "MAX",             0 },     // 7
  { "ROW",             0 },     // 8
  { "COLUMN",          0 },     // 9
  { "NOVALUE",         0 },     // 10
  { "NPV",             0 },     // 11
  { "STDEV",           0 },     // 12
  { "DOLLAR",          0 },     // 13
  { "FIXED",           0 },     // 14
  { "SIN",             1 },     // 15
  { "COS",             1 },     // 16
  { "TAN",             1 },     // 17
  { "ATAN",            1 },     // 18
  { "PI",              0 },     // 19
  { "SQRT",            1 },     // 20
  { "EXP",             1 },     // 21
  { "LN",              1 },     // 22
  { "LOG10",           1 },     // 23
  { "ABS",             1 },     // 24
  { "INT",             1 },     // 25
  { "SIGN",            1 },     // 26
  { "ROUND",           2 },     // 27
  { "LOOKUP",          0 },     // 28
  { "INDEX",           0 },     // 29
  { "REPT",            2 },     // 30
  { "MID",             3 },     // 31
  { "LEN",             1 },     // 32
  { "VALUE",           1 },     // 33
  { "TRUE",            0 },     // 34
  { "FALSE",           0 },     // 35
  { "AND",             0 },     // 36
  { "OR",              0 },     // 37
  { "NOT",             1 },     // 38
  { "MOD",             2 },     // 39
  { "DCOUNT",          3 },     // 40
  { "DSUM",            3 },     // 41
  { "DAVERAGE",        3 },     // 42
  { "DMIN",            3 },     // 43
  { "DMAX",            3 },     // 44
  { "DSTDEV",          3 },     // 45
  { "VAR",             0 },     // 46
  { "DVAR",            3 },     // 47
  { "TEXT",            2 },     // 48
  { "LINEST",          0 },     // 49
  { "TREND",           0 },     // 50
  { "LOGEST",           0 },     // 51
  { "GROWTH",          0 },     // 52
  { "GOTO",            0 },     // 53
  { "HALT",            0 },     // 54
  { "Unknown55",       0 },     // 55
  { "PV",              0 },     // 56
  { "FV",              0 },     // 57
  { "NPER",            0 },     // 58
  { "PMT",             0 },     // 59
  { "RATE",            0 },     // 60
  { "MIRR",            3 },     // 61
  { "IRR",             0 },     // 62
  { "RAND",            0 },     // 63
  { "MATCH",           0 },     // 64
  { "DATE",            3 },     // 65
  { "TIME",            3 },     // 66
  { "DAY",             1 },     // 67
  { "MONTH",           1 },     // 68
  { "YEAR",            1 },     // 69
  { "DAYOFWEEK",       0 },     // 70
  { "HOUR",            1 },     // 71
  { "MIN",             1 },     // 72
  { "SEC",             1 },     // 73
  { "NOW",             0 },     // 74
  { "AREAS",           1 },     // 75
  { "ROWS",            1 },     // 76
  { "COLUMNS",         1 },     // 77
  { "OFFSET",          0 },     // 78
  { "ABSREF",          2 },     // 79
  { "RELREF",          0 },     // 80
  { "ARGUMENT",        0 },     // 81
  { "SEARCH",          0 },     // 82
  { "TRANSPOSE",       1 },     // 83
  { "ERROR",           0 },     // 84
  { "STEP",            0 },     // 85
  { "TYPE",            1 },     // 86
  { "ECHO",            0 },
  { "SETNAME",         0 },
  { "CALLER",          0 },
  { "DEREF",           0 },
  { "WINDOWS",         0 },
  { "SERIES",          4 },
  { "DOCUMENTS",       0 },
  { "ACTIVECELL",      0 },
  { "SELECTION",       0 },
  { "RESULT",          0 },
  { "ATAN2",           2 },     // 97
  { "ASIN",            1 },     // 98
  { "ACOS",            1 },     // 99
  { "CHOOSE",          0 },     // 100
  { "HLOOKUP",         0 },     // 101
  { "VLOOKUP",         0 },     // 102
  { "LINKS",           0 },  
  { "INPUT",           0 },
  { "ISREF",           1 },     // 105
  { "GETFORMULA",      0 },
  { "GETNAME",         0 },
  { "SETVALUE",        0 },
  { "LOG",             0 },     // 109
  { "EXEC",            0 },
  { "CHAR",            1 },     // 111
  { "LOWER",           1 },     // 112
  { "UPPER",           1 },     // 113
  { "PROPER",          1 },     // 114
  { "LEFT",            0 },     // 115
  { "RIGHT",           0 },     // 116
  { "EXACT",           2 },     // 117
  { "TRIM",            1 },     // 118
  { "REPLACE",         4 },     // 119
  { "SUBSTITUTE",      0 },     // 120
  { "CODE",            1 },     // 121
  { "NAMES",           0 },
  { "DIRECTORY",       0 },
  { "FIND",            0 },     // 124
  { "CELL",            0 },     // 125
  { "ISERR",           1 },     // 126
  { "ISTEXT",          1 },     // 127
  { "ISNUMBER",        1 },     // 128
  { "ISBLANK",         1 },     // 129
  { "T",               1 },     // 130
  { "N",               1 },     // 131
  { "FOPEN",           0 },
  { "FCLOSE",          0 },
  { "FSIZE",           0 },
  { "FREADLN",         0 },
  { "FREAD",           0 },
  { "FWRITELN",        0 },
  { "FWRITE",          0 },
  { "FPOS",            0 },
  { "DATEVALUE",       1 },     // 140
  { "TIMEVALUE",       1 },     // 141
  { "SLN",             3 },     // 142
  { "SYD",             4 },     // 143
  { "DDB",             0 },     // 144
  { "GETDEF",          0 },
  { "REFTEXT",         0 },
  { "TEXTREF",         0 },
  { "INDIRECT",        0 },     // 148
  { "REGISTER",        0 },
  { "CALL",            0 },
  { "ADDBAR",          0 },
  { "ADDMENU",         0 },
  { "ADDCOMMAND",      0 },
  { "ENABLECOMMAND",   0 },
  { "CHECKCOMMAND",    0 },
  { "RENAMECOMMAND",   0 },
  { "SHOWBAR",         0 },
  { "DELETEMENU",      0 },
  { "DELETECOMMAND",   0 },
  { "GETCHARTITEM",    0 },
  { "DIALOGBOX",       0 },
  { "CLEAN",           1 },     // 162
  { "MDETERM",         1 },     // 163
  { "MINVERSE",        1 },     // 164
  { "MMULT",           2 },     // 165
  { "FILES",           0 },  
  { "IPMT",            0 },     // 167
  { "PPMT",            0 },     // 168
  { "COUNTA",          0 },     // 169
  { "CANCELKEY",       1 },	
  { "Unknown171",      0 },
  { "Unknown172",      0 },
  { "Unknown173",      0 },
  { "Unknown174",      0 },
  { "INITIATE",        0 },
  { "REQUEST",         0 },
  { "POKE",            0 },
  { "EXECUTE",         0 },
  { "TERMINATE",       0 },
  { "RESTART",         0 },
  { "HELP",            0 },
  { "GETBAR",          0 },  
  { "PRODUCT",         0 },     // 183
  { "FACT",            1 },     // 184
  { "GETCELL",         0 },  
  { "GETWORKSPACE",    0 },
  { "GETWINDOW",       0 },
  { "GETDOCUMENT",     0 },
  { "DPRODUCT",        3 },     // 189
  { "ISNONTEXT",       1 },     // 190
  { "GETNOTE",         0 },
  { "NOTE",            0 },
  { "STDEVP",          0 },     // 193
  { "VARP",            0 },     // 194
  { "DSTDEVP",         3 },     // 195
  { "DVARP",           3 },     // 196
  { "TRUNC",           0 },     // 197
  { "ISLOGICAL",       1 },     // 198
  { "DCOUNTA",         3 },     // 199
  { "DELETEBAR",       0 },
  { "UNREGISTER",      0 },
  { "Unknown202",      0 },
  { "Unknown203",      0 },
  { "USDOLLAR",        0 },
  { "FINDB",           0 },
  { "SEARCHB",         0 },
  { "REPLACEB",        0 },
  { "LEFTB",           0 },
  { "RIGHTB",          0 },
  { "MIDB",            0 },
  { "LENB",            0 },  
  { "ROUNDUP",         2 },     // 212
  { "ROUNDDOWN",       2 },     // 213
  { "ASC",             0 },
  { "DBCS",            0 },
  { "RANK",            0 },     // 216
  { "Unknown217",      0 },
  { "Unknown218",      0 },  
  { "ADDRESS",         0 },     // 219
  { "GETDIFFDATE360",  0 },     // 220
  { "CURRENTDATE",     0 },     // 221
  { "VBD",             0 },     // 222
  { "Unknown223",      0 },
  { "Unknown224",      0 }, 
  { "Unknown225",      0 },
  { "Unknown226",      0 },
  { "MEDIAN",          0 },     // 227
  { "SUMPRODUCT",      0 },     // 228
  { "SINH",            1 },     // 229
  { "COSH",            1 },     // 230
  { "TANH",            1 },     // 231
  { "ASINH",           1 },     // 232
  { "ACOSH",           1 },     // 233
  { "ATANH",           1 },     // 234
  { "DGET",            3 },     // 235
  { "CREATEOBJECT",    0 },
  { "VOLATILE",        0 },
  { "LASTERROR",       0 },
  { "CUSTOMUNDO",      0 },
  { "CUSTOMREPEAT",    0 },
  { "FORMULACONVERT",  0 },
  { "GETLINKINFO",     0 },
  { "TEXTBOX",         0 },  
  { "INFO",            1 },     // 244
  { "GROUP",           0 },
  { "GETOBJECT",       0 },  
  { "DB",              0 },     // 247
  { "PAUSE",           0 },
  { "Unknown249",      0 },
  { "Unknown250",      0 },
  { "RESUME",          0 },
  { "FREQUENCY",       2 },     // 252
  { "ADDTOOLBAR",      0 },
  { "DELETETOOLBAR",   0 },
  { "Unknown255",      0 }, 
  { "RESETTOOLBAR",    0 },
  { "EVALUATE",        0 },
  { "GETTOOLBAR",      0 },
  { "GETTOOL",         0 },
  { "SPELLINGCHECK",   0 },  
  { "ERRORTYPE",       1 },     // 261
  { "APPTITLE",        0 },
  { "WINDOWTITLE",     0 },
  { "SAVETOOLBAR",     0 },
  { "ENABLETOOL",      0 },
  { "PRESSTOOL",       0 },
  { "REGISTERID",      0 },
  { "GETWORKBOOK",     0 },
  { "AVEDEV",          0 },     // 269
  { "BETADIST",        0 },     // 270
  { "GAMMALN",         1 },     // 271
  { "BETAINV",         0 },     // 272
  { "BINOMDIST",       4 },     // 273
  { "CHIDIST",         2 },     // 274
  { "CHIINV",          2 },     // 275
  { "COMBIN",          2 },     // 276
  { "CONFIDENCE",      3 },     // 277
  { "CRITBINOM",       3 },     // 278
  { "EVEN",            1 },     // 279
  { "EXPONDIST",       3 },     // 280
  { "FDIST",           3 },     // 281
  { "FINV",            3 },     // 282
  { "FISHER",          1 },     // 283
  { "FISHERINV",       1 },     // 284
  { "FLOOR",           2 },     // 285
  { "GAMMADIST",       4 },     // 286
  { "GAMMAINV",        3 },     // 287
  { "CEIL",            2 },     // 288
  { "HYPGEOMDIST",     4 },     // 289
  { "LOGNORMDIST",     3 },     // 290
  { "LOGINV",          3 },     // 291
  { "NEGBINOMDIST",    3 },     // 292
  { "NORMDIST",        4 },     // 293
  { "NORMSDIST",       1 },     // 294
  { "NORMINV",         3 },     // 295
  { "NORMSINV",        1 },     // 296
  { "STANDARDIZE",     3 },     // 297
  { "ODD",             1 },     // 298
  { "PERMUT",          2 },     // 299
  { "POISSON",         3 },     // 300
  { "TDIST",           3 },     // 301
  { "WEIBULL",         4 },     // 302
  { "SUMXMY2",         2 },     // 303
  { "SUMX2MY2",        2 },     // 304
  { "SUMX2DY2",        2 },     // 305
  { "CHITEST",         2 },     // 306
  { "CORREL",          2 },     // 307
  { "COVAR",           2 },     // 308
  { "FORECAST",        3 },     // 309
  { "FTEST",           2 },     // 310
  { "INTERCEPT",       2 },     // 311
  { "PEARSON",         2 },     // 312
  { "RSQ",             2 },     // 313
  { "STEYX",           2 },     // 314
  { "SLOPE",           2 },     // 315
  { "TTEST",           4 },     // 316
  { "PROB",            0 },     // 317
  { "DEVSQ",           0 },     // 318
  { "GEOMEAN",         0 },     // 319
  { "HARMEAN",         0 },     // 320
  { "SUMSQ",           0 },     // 321
  { "KURT",            0 },     // 322
  { "SKEW",            0 },     // 323
  { "ZTEST",           0 },     // 324
  { "LARGE",           2 },     // 325
  { "SMALL",           2 },     // 326
  { "QUARTILE",        2 },     // 327
  { "PERCENTILE",      2 },     // 328
  { "PERCENTRANK",     0 },     // 329
  { "MODALVALUE",      0 },     // 330
  { "TRIMMEAN",        2 },     // 331
  { "TINV",            2 },     // 332
  { "Unknown333",      0 },
  { "MOVIECOMMAND",    0 },
  { "GETMOVIE",        0 },  
  { "CONCATENATE",     0 },     // 336
  { "POWER",           2 },     // 337
  { "PIVOTADDDATA",    0 },
  { "GETPIVOTTABLE",   0 },
  { "GETPIVOTFIELD",   0 },
  { "GETPIVOTITEM",    0 },  
  { "RADIANS",         1 },     // 342
  { "DEGREES",         1 },     // 343
  { "SUBTOTAL",        0 },     // 344
  { "SUMIF",           0 },     // 345
  { "COUNTIF",         2 },     // 346
  { "COUNTBLANK",      1 },     // 347
  { "SCENARIOGET",     0 },
  { "OPTIONSLISTSGET", 0 },
  { "ISPMT",           4 },
  { "DATEDIF",         3 },
  { "DATESTRING",      0 },
  { "NUMBERSTRING",    0 },
  { "ROMAN",           0 },     // 354
  { "OPENDIALOG",      0 },
  { "SAVEDIALOG",      0 },
  { "VIEWGET",         0 },
  { "GETPIVOTDATA",    2 },     // 358
  { "HYPERLINK",       1 },
  { "PHONETIC",        0 },
  { "AVERAGEA",        0 },     // 361
  { "MAXA",            0 },     // 362
  { "MINA",            0 },     // 363
  { "STDEVPA",         0 },     // 364
  { "VARPA",           0 },     // 365
  { "STDEVA",          0 },     // 366
  { "VARA",            0 },     // 367
};

const char* FormulaToken::functionName() const
{
  if( functionIndex() > 367 ) return 0;
  return FunctionEntries[ functionIndex() ].name;
}

unsigned FormulaToken::functionParams() const
{
  unsigned params = 0;

  if( d->id == Function )
  {
    if( functionIndex() > 367 ) return 0;
    params = FunctionEntries[ functionIndex() ].params;
  }

  if( d->id == FunctionVar )
  {
    params = (unsigned)d->data[0];
    params &= 0x7f;
  }

  return params;
}

unsigned FormulaToken::attr() const
{
  unsigned attr = 0;
  if( d->id == Attr )
  {
    attr = (unsigned) d->data[0];
  }
  return attr;
}

unsigned FormulaToken::nameIndex() const
{
  // FIXME check data size !
  unsigned ni = 0;
  unsigned char buf[2];

  if( d->id == NameX )
  if( d->ver == Excel97 )
  {
    buf[0] = d->data[2];
    buf[1] = d->data[3];
    ni = readU16( buf );
  }

  if( d->id == NameX )
  if( d->ver == Excel95 )
  {
    buf[0] = d->data[10];
    buf[1] = d->data[11];
    ni = readU16( buf );
  }

  return ni;
}


UString FormulaToken::area( unsigned row, unsigned col ) const
{
  // FIXME check data size !
  unsigned char buf[2];
  int row1Ref, row2Ref, col1Ref, col2Ref;
  bool row1Relative, col1Relative;
  bool row2Relative, col2Relative;

  if( version() == Excel97 )
  {
    buf[0] = d->data[0];
    buf[1] = d->data[1];
    row1Ref = readU16( buf );

    buf[0] = d->data[2];
    buf[1] = d->data[3];
    row2Ref = readU16( buf );

    buf[0] = d->data[4];
    buf[1] = d->data[5];
    col1Ref = readU16( buf );

    buf[0] = d->data[6];
    buf[1] = d->data[7];
    col2Ref = readU16( buf );

    row1Relative = col1Ref & 0x8000;
    col1Relative = col1Ref & 0x4000;
    col1Ref &= 0x3fff;

    row2Relative = col2Ref & 0x8000;
    col2Relative = col2Ref & 0x4000;
    col2Ref &= 0x3fff;
  }
  else
  {
    buf[0] = d->data[0];
    buf[1] = d->data[1];
    row1Ref = readU16( buf );

    buf[0] = d->data[2];
    buf[1] = d->data[3];
    row2Ref = readU16( buf );

    buf[0] = d->data[4];
    buf[1] = 0;
    col1Ref = readU16( buf );

    buf[0] = d->data[5];
    buf[1] = 0;
    col2Ref = readU16( buf );

    row1Relative = row2Ref & 0x8000;
    col1Relative = row2Ref & 0x4000;
    row1Ref &= 0x3fff;

    row2Relative = row2Ref & 0x8000;
    col2Relative = row2Ref & 0x4000;
    row2Ref &= 0x3fff;
  }

  UString result;
  result.append( UString("[") );  // OpenDocument format
  
  if( !col1Relative )
    result.append( UString("$") );
  result.append( Cell::columnLabel( col1Ref ) );  
  if( !row1Relative )
    result.append( UString("$") );
  result.append( UString::from( row1Ref+1 ) );  
  result.append( UString(":") );
  if( !col2Relative )
    result.append( UString("$") );
  result.append( Cell::columnLabel( col2Ref ) );  
  if( !row2Relative )
    result.append( UString("$") );
  result.append( UString::from( row2Ref+1 ) );  
  
  result.append( UString("]") );// OpenDocument format

  return result;  
}

UString FormulaToken::ref( unsigned row, unsigned col ) const
{
  // FIXME check data size !
  // FIXME handle shared formula
  unsigned char buf[2];
  int rowRef, colRef;
  bool rowRelative, colRelative;

  if( version() == Excel97 )
  {
    buf[0] = d->data[0];
    buf[1] = d->data[1];
    rowRef = readU16( buf );

    buf[0] = d->data[2];
    buf[1] = d->data[3];
    colRef = readU16( buf );

    rowRelative = colRef & 0x8000;
    colRelative = colRef & 0x4000;
    colRef &= 0x3fff;
  }
  else
  {
    buf[0] = d->data[0];
    buf[1] = d->data[1];
    rowRef = readU16( buf );

    buf[0] = d->data[2];
    buf[1] = 0;
    colRef = readU16( buf );

    rowRelative = rowRef & 0x8000;
    colRelative = rowRef & 0x4000;
    rowRef &= 0x3fff;
  }

  UString result;

  result.append( UString("[") );  // OpenDocument format
  
  if( !colRelative )
    result.append( UString("$") );
  result.append( Cell::columnLabel( colRef ) );  
  if( !rowRelative )
    result.append( UString("$") );
  result.append( UString::from( rowRef+1 ) );  
  
  result.append( UString("]") );// OpenDocument format

  return result;  
}

std::ostream& Swinder::operator<<( std::ostream& s,  Swinder::FormulaToken token )
{
  s << std::setw(2) << std::hex << token.id() << std::dec;
  // s  << "  Size: " << std::dec << token.size();
  s << "  ";
  
  switch( token.id() )
  {
    case FormulaToken::ErrorCode:
    case FormulaToken::Bool:
    case FormulaToken::Integer:
    case FormulaToken::Float:
    case FormulaToken::String:
      {
        Value v = token.value();
        s << v;
      }
      break;
      
    case FormulaToken::Function:
      s << "Function " << token.functionName();  
      break;
      
    default:  
      s << token.idAsString();
      break;
  }
  
  return s;
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
  Record* record = 0;
  
  if( type == BOFRecord::id )
    record = new BOFRecord();
    
  else if( type == EOFRecord::id )
    record = new EOFRecord();
    
  if( type == BackupRecord::id )
    record = new BackupRecord();
    
  if( type == BlankRecord::id )
    record = new BlankRecord();
    
  if( type == BoolErrRecord::id )
    record = new BoolErrRecord();
    
  if( type == BottomMarginRecord::id )
    record = new BottomMarginRecord();
    
  if( type == BoundSheetRecord::id )
    record = new BoundSheetRecord();
    
  if( type == CalcModeRecord::id )
    record = new CalcModeRecord();
    
  if( type == ColInfoRecord::id )
    record = new ColInfoRecord();
    
  if( type == DateModeRecord::id )
    record = new DateModeRecord();
    
  if( type == DimensionRecord::id )
    record = new DimensionRecord();
    
  if( type == ExternNameRecord::id )
    record = new ExternNameRecord();
    
  else if( type == FilepassRecord::id )
    record = new FilepassRecord();
    
  else if( type == FontRecord::id )
    record = new FontRecord();
    
  else if( type == FooterRecord::id )
    record = new FooterRecord();
    
  else if( type == FormatRecord::id )
    record = new FormatRecord();
    
  else if( type == FormulaRecord::id )
    record = new FormulaRecord();
    
  else if( type == HeaderRecord::id )
    record = new HeaderRecord();
    
  else if( type == LabelRecord::id )
    record = new LabelRecord();
    
  else if( type == LabelSSTRecord::id )
    record = new LabelSSTRecord();
    
  if( type == LeftMarginRecord::id )
    record = new LeftMarginRecord();
    
  else if( type == MergedCellsRecord::id )
    record = new MergedCellsRecord();
    
  else if( type == MulBlankRecord::id )
    record = new MulBlankRecord();
    
  else if( type == MulRKRecord::id )
    record = new MulRKRecord();
    
  if( type == NameRecord::id )
    record = new NameRecord();
    
  else if( type == NumberRecord::id )
    record = new NumberRecord();
    
  else if( type == PaletteRecord::id )
    record = new PaletteRecord();
    
  if( type == RightMarginRecord::id )
    record = new RightMarginRecord();
    
  else if( type == RKRecord::id )
    record = new RKRecord();
    
  else if( type == RowRecord::id )
    record = new RowRecord();
    
  else if( type == RStringRecord::id )
    record = new RStringRecord();
    
  else if( type == SSTRecord::id )
    record = new SSTRecord();
  
  else if( type == StringRecord::id )
    record = new StringRecord();
  
  else if( type == XFRecord::id )
    record = new XFRecord();
  
  else if( type == TopMarginRecord::id )
    record = new TopMarginRecord();
    
  return record;
}

void Record::setPosition( unsigned pos )
{
  stream_position = pos;
}
  
unsigned Record::position() const
{
  return stream_position;
}

void Record::setData( unsigned, const unsigned char* )
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

void BackupRecord::setData( unsigned size, const unsigned char* data )
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

void BoolErrRecord::setData( unsigned size, const unsigned char* data )
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

void BottomMarginRecord::setData( unsigned size, const unsigned char* data )
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

void CalcModeRecord::setData( unsigned size, const unsigned char* data )
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
  out << "       First Column : " << firstColumn() << std::endl;
  out << "        Last Column : " << lastColumn() << std::endl;
  out << "              Width : " << width() << std::endl;
  out << "           XF Index : " << xfIndex() << std::endl;
  out << "             Hidden : " << ( hidden() ? "Yes" : "No" ) << std::endl;
  out << "          Collapsed : " << ( collapsed() ? "Yes" : "No" ) << std::endl;
  out << "      Outline Level : " << outlineLevel() << std::endl;  
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

void DateModeRecord::setData( unsigned size, const unsigned char* data )
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

void DimensionRecord::setData( unsigned size, const unsigned char* data )
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

void EOFRecord::setData( unsigned,  const unsigned char* )
{
  // no data associated with EOF record
}

void EOFRecord::dump( std::ostream& out ) const
{
  out << "EOF" << std::endl;
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

void ExternNameRecord::setData( unsigned size, const unsigned char* data )
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

void ExternNameRecord::dump( std::ostream& out ) const
{
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

void FilepassRecord::setData( unsigned,  const unsigned char* )
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
    default: out << "Unkown " << escapement() << std::endl; break;
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

void FooterRecord::setData( unsigned size, const unsigned char* data )
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

void FormatRecord::setData( unsigned size, const unsigned char* data )
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

void FormulaRecord::setData( unsigned size, const unsigned char* data )
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
      if( token.size() > 1 )
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

void HeaderRecord::setData( unsigned size, const unsigned char* data )
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

void LeftMarginRecord::setData( unsigned size, const unsigned char* data )
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

void MergedCellsRecord::setData( unsigned size, const unsigned char* data )
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

void NameRecord::setData( unsigned size, const unsigned char* data )
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

void NameRecord::dump( std::ostream& out ) const
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

void PaletteRecord::setData( unsigned size, const unsigned char* data )
{
  if( size < 14 ) return;
  
  unsigned num = readU16( data );
  
  unsigned p = 2;
  for( unsigned i = 0; i < num; i++, p+=4 )
  {
    unsigned Qt::red = data[ p ];
    unsigned Qt::green = data[ p+1 ];
    unsigned Qt::blue = data[ p+2 ];
    d->colors.push_back( Color( Qt::red, Qt::green, Qt::blue ) );
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
    out << "R:" << std::setw(3) << c.Qt::red;
    out << "   G:" << std::setw(3) << c.Qt::green;
    out << "   B:" << std::setw(3) << c.Qt::blue << std::endl;
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

void RightMarginRecord::setData( unsigned size, const unsigned char* data )
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
void RKRecord::setData( unsigned size, const unsigned char* data )
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

void RowRecord::setData( unsigned size, const unsigned char* data )
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
void RStringRecord::setData( unsigned size, const unsigned char* data )
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

void SSTRecord::setData( unsigned size, const unsigned char* data )
{
  if( size < 8 ) return;
  
  d->total = readU32( data );
  d->count = readU32( data+4 );
  
  unsigned offset = 8;
  d->strings.clear();
  
  for( unsigned i = 0; i < d->count; i++ )
  {
    // check against size
    if (offset >= size) {
      std::cerr << "Warning: reached end of SST record, but not all strings have been read!" << std::endl;
      break;
    }
    
    EString es = EString::fromUnicodeString( data+offset, true, size - offset );
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
  out << "         Occurences : " << d->total << std::endl;
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

void StringRecord::setData( unsigned size, const unsigned char* data )
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

void TopMarginRecord::setData( unsigned size, const unsigned char* data )
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
  d->horizontalAlignment = Qt::DockLeft;
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
    case Qt::DockLeft:      result = "Left"; break;
    case Centered:  result = "Centered"; break;
    case Qt::DockRight:     result = "Right"; break;
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
    case Qt::DockTop:          result = "Top"; break;
    case VCentered:    result = "Centered"; break;
    case Qt::DockBottom:       result = "Bottom"; break;
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

void XFRecord::setData( unsigned size, const unsigned char* data )
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
    unsigned attributes = data[9];

    setIndentLevel( options & 0x0f );
    setShrinkContent( options & 0x10 );
  
    unsigned linestyle = readU16( data + 10 );
    unsigned Qt::color1 = readU16( data + 12 );
    // unsigned color2 = readU16( data + 14 );
    unsigned flag = readU16( data + 16 );
    unsigned fill = readU16( data + 18 );
  
    setLeftBorderStyle( linestyle & 0xf );
    setRightBorderStyle( ( linestyle >> 4 ) & 0xf );
    setTopBorderStyle( ( linestyle >> 8 ) & 0xf );
    setBottomBorderStyle( ( linestyle >> 12 ) & 0xf );
  
    setLeftBorderColor( Qt::color1 & 0x7f );
    setRightBorderColor( ( Qt::color1 >> 7 ) & 0x7f );
    setTopBorderColor( Qt::color1 & 0x7f );
    setBottomBorderColor( ( Qt::color1 >> 7 ) & 0x7f );
  
    setDiagonalTopLeft( Qt::color1 & 0x40 );
    setDiagonalBottomLeft( Qt::color1 & 0x40 );
    setDiagonalStyle( ( flag >> 4 ) & 0x1e  );
    setDiagonalColor( ( ( flag & 0x1f ) << 2 ) + (  ( Qt::color1 >> 14 ) & 3 ));
    
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
  
  // password protection flag
  // TODO: password hash for record decryption
  bool passwordProtected;

  // active sheet, all cell records will be stored here
  Sheet* activeSheet;
  
  // for FORMULA+STRING record pair
  Cell* formulaCell;
  
  // mapping from BOF pos to actual Sheet
  std::map<unsigned,Sheet*> bofMap;
  
  // shared-string table
  std::vector<UString> stringTable;
  
  // table of format
  std::map<unsigned,FormatRecord> formatTable;
  std::map<unsigned,UString> formatsTable;
  
  // table of font
  std::vector<FontRecord> fontTable;
  
  // table of Xformat
  std::vector<XFRecord> xfTable;
  
  // color table (from Palette record)
  std::vector<Color> colorTable;
  
  // mapping from font index to Swinder::FormatFont
  std::map<unsigned,FormatFont> fontCache;

  // for NAME and EXTERNNAME
  std::vector<UString> nameTable;
};

ExcelReader::ExcelReader()
{
  d = new ExcelReader::Private();
  
  d->workbook    = 0;
  d->activeSheet = 0;
  d->formulaCell = 0;
  
  d->passwordProtected = false;
  
  // initialize palette
  static const char *const default_palette[64-8] =	// default palette for all but the first 8 colors
  {
	  "#000000", "#ffffff", "#ff0000", "#00ff00", "#0000ff", "#ffff00", "#ff00ff",
	  "#00ffff", "#800000", "#008000", "#000080", "#808000", "#800080", "#008080",
	  "#c0c0c0", "#808080", "#9999ff", "#993366", "#ffffcc", "#ccffff", "#660066",
	  "#ff8080", "#0066cc", "#ccccff", "#000080", "#ff00ff", "#ffff00", "#00ffff",
	  "#800080", "#800000", "#008080", "#0000ff", "#00ccff", "#ccffff", "#ccffcc",
	  "#ffff99", "#99ccff", "#ff99cc", "#cc99ff", "#ffcc99", "#3366ff", "#33cccc",
	  "#99cc00", "#ffcc00", "#ff9900", "#ff6600", "#666699", "#969696", "#003366",
	  "#339966", "#003300", "#333300", "#993300", "#993366", "#333399", "#333333",
  };
  for( int i = 0; i < 64-8; i++ ) {
    d->colorTable.push_back(Color(default_palette[i]));
  }
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
  
  unsigned version = Swinder::Excel97;
  POLE::Stream* stream;
  stream = new POLE::Stream( &storage, "/Workbook" );
  if( stream->fail() )
  {
    delete stream;
    stream = new POLE::Stream( &storage, "/Book" );
    version = Swinder::Excel95;
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

  workbook->clear();  
  d->workbook = workbook;
  
  d->passwordProtected = false;
  
  // assume

  while( stream->tell() < stream_size )
  {
  
    // this is set by FILEPASS record
    // subsequent records will need to be decrypted
    // since we do not support it yet, we have to bail out
    if(d->passwordProtected)
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

	    // if the first read byte is a zero, remove it (at least that is what the old excel97 filter did...)
	    if (buffer[size] == 0) {
                memmove( buffer + size, buffer + size + 1, --next_size );
	    }

	    // and finally update size
	    size += next_size;
	}
    } while (next_type == 0x3C);

    // restore position in stream to the beginning of the next record
    stream->seek( saved_pos );
    
    // skip record type 0, this is just for filler
    if( type == 0 ) continue;
    
    // create the record using the factory
    Record* record = Record::create( type );

    if( record )
    {
      // setup the record and invoke handler
      record->setVersion( version );
      record->setData( size, buffer );
      record->setPosition( pos );
      
      handleRecord( record );
      
      // special handling to find Excel version
      if ( record->rtti() == BOFRecord::id )
      {
        BOFRecord* bof = static_cast<BOFRecord*>(record);
        if( bof ) if( bof->type() == BOFRecord::Workbook )
        version = bof->version();
      }

#ifdef SWINDER_XLS2RAW
      std::cout << "Record 0x";
      std::cout << std::setfill('0') << std::setw(4) << std::hex << record->rtti();
      std::cout << " ";
      std::cout << std::dec;
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
    case BottomMarginRecord::id: 
      handleBottomMargin( static_cast<BottomMarginRecord*>( record ) ); break;
    case BoundSheetRecord::id: 
      handleBoundSheet( static_cast<BoundSheetRecord*>( record ) ); break;
    case BOFRecord::id: 
      handleBOF( static_cast<BOFRecord*>( record ) ); break;
    case BoolErrRecord::id: 
      handleBoolErr( static_cast<BoolErrRecord*>( record ) ); break;
    case BlankRecord::id: 
      handleBlank( static_cast<BlankRecord*>( record ) ); break;
    case CalcModeRecord::id: 
      handleCalcMode( static_cast<CalcModeRecord*>( record ) ); break;
    case ColInfoRecord::id: 
      handleColInfo( static_cast<ColInfoRecord*>( record ) ); break;
    case ExternNameRecord::id: 
      handleExternName( static_cast<ExternNameRecord*>( record ) ); break;
    case FilepassRecord::id: 
      handleFilepass( static_cast<FilepassRecord*>( record ) ); break;
    case FormatRecord::id: 
      handleFormat( static_cast<FormatRecord*>( record ) ); break;
    case FormulaRecord::id: 
      handleFormula( static_cast<FormulaRecord*>( record ) ); break;
    case FontRecord::id: 
      handleFont( static_cast<FontRecord*>( record ) ); break;
    case FooterRecord::id: 
      handleFooter( static_cast<FooterRecord*>( record ) ); break;
    case HeaderRecord::id: 
      handleHeader( static_cast<HeaderRecord*>( record ) ); break;
    case LabelRecord::id: 
      handleLabel( static_cast<LabelRecord*>( record ) ); break;
    case LabelSSTRecord::id: 
      handleLabelSST( static_cast<LabelSSTRecord*>( record ) ); break;
    case LeftMarginRecord::id: 
      handleLeftMargin( static_cast<LeftMarginRecord*>( record ) ); break;
    case MergedCellsRecord::id: 
      handleMergedCells( static_cast<MergedCellsRecord*>( record ) ); break;
    case MulBlankRecord::id: 
      handleMulBlank( static_cast<MulBlankRecord*>( record ) ); break;
    case MulRKRecord::id: 
      handleMulRK( static_cast<MulRKRecord*>( record ) ); break;
    case NameRecord::id: 
      handleName( static_cast<NameRecord*>( record ) ); break;
    case NumberRecord::id: 
      handleNumber( static_cast<NumberRecord*>( record ) ); break;
    case PaletteRecord::id: 
      handlePalette( static_cast<PaletteRecord*>( record ) ); break;
    case RightMarginRecord::id: 
      handleRightMargin( static_cast<RightMarginRecord*>( record ) ); break;
    case RKRecord::id: 
      handleRK( static_cast<RKRecord*>( record ) ); break;
    case RowRecord::id: 
      handleRow( static_cast<RowRecord*>( record ) ); break;
    case RStringRecord::id: 
      handleRString( static_cast<RStringRecord*>( record ) ); break;
    case SSTRecord::id: 
      handleSST( static_cast<SSTRecord*>( record ) ); break;
    case StringRecord::id: 
      handleString( static_cast<StringRecord*>( record ) ); break;
    case TopMarginRecord::id: 
      handleTopMargin( static_cast<TopMarginRecord*>( record ) ); break;
    case XFRecord::id: 
      handleXF( static_cast<XFRecord*>( record ) ); break;
    default:
      break;  
  }
}

void ExcelReader::handleBottomMargin( BottomMarginRecord* record )
{
  if( !record ) return;

  if( !d->activeSheet ) return;

  // convert from inches to points
  double margin = record->bottomMargin() * 72.0;
  d->activeSheet->setBottomMargin( margin );
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
  
  Cell* cell = d->activeSheet->cell( column, row, true );
  if( cell )
  {
    cell->setValue( record->value() );
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

void ExcelReader::handleCalcMode( CalcModeRecord* record )
{
  if( !record ) return;
  
  d->workbook->setAutoCalc( record->autoCalc() );
}
  
void ExcelReader::handleColInfo( ColInfoRecord* record )
{
  if( !record ) return;
  
  if( !d->activeSheet ) return;
  
  unsigned firstColumn = record->firstColumn();
  unsigned lastColumn = record->lastColumn();
  unsigned xfIndex = record->xfIndex();
  unsigned width = record->width();
  bool hidden = record->hidden();
  
  for( unsigned i = firstColumn; i <= lastColumn; i++ )
  {
    Column* column = d->activeSheet->column( i, true );
    if( column )
    {
      column->setWidth( width / 120 );
      column->setFormat( convertFormat( xfIndex ) );
      column->setVisible( !hidden );
    }
  }  
}

void ExcelReader::handleDateMode( DateModeRecord* record )
{
  if( !record ) return;
  
  // FIXME FIXME what to do ??
  std::cerr << "WARNING: Workbook uses unsupported 1904 Date System " << std::endl;
}

void ExcelReader::handleDimension( DimensionRecord* record )
{
  if( !record ) return;
  
  // in the mean time we don't need to handle this because we don't care
  // about the used range of the sheet  
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

void ExcelReader::handleLeftMargin( LeftMarginRecord* record )
{
  if( !record ) return;

  if( !d->activeSheet ) return;

  // convert from inches to points
  double margin = record->leftMargin() * 72.0;
  d->activeSheet->setLeftMargin( margin );
}

void ExcelReader::handleFormat( FormatRecord* record )
{
  if( !record ) return;

  d->formatTable[ record->index() ] = *record;
  d->formatsTable[ record->index() ] = record->formatString();
}

void ExcelReader::handleFormula( FormulaRecord* record )
{
  if( !record ) return;

  if( !d->activeSheet ) return;
  
  unsigned column = record->column();
  unsigned row = record->row();  
  unsigned xfIndex = record->xfIndex();
  Value value = record->result();
  
  UString formula = decodeFormula( row, column, record->tokens() );
  
  Cell* cell = d->activeSheet->cell( column, row, true );
  if( cell )
  {
    cell->setValue( value );
    if( !formula.isEmpty() )
      cell->setFormula( formula );
    cell->setFormat( convertFormat( xfIndex ) );
    
    // if value is string, real value is in subsequent String record
    if( value.isString() )
      d->formulaCell = cell;
  }
}

void ExcelReader::handleExternName( ExternNameRecord* record )
{
  if( !record ) return;

  d->nameTable.push_back( record->externName() );
}

void ExcelReader::handleFilepass( FilepassRecord* record )
{
  if( !record ) return;
  
  d->passwordProtected = true;
}

void ExcelReader::handleFont( FontRecord* record )
{
  if( !record ) return;

  d->fontTable.push_back( *record );

  // font #4 is never used, so add a dummy one
  if( d->fontTable.size() == 4 )
    d->fontTable.push_back( FontRecord() );
}

void ExcelReader::handleFooter( FooterRecord* record )
{
  if( !record ) return;

  if( !d->activeSheet ) return;

  UString footer = record->footer();
  UString left, center, right;
  int pos = -1, len = 0;

  // left part
  pos = footer.find( UString("&L") );
  if( pos >= 0 )
  {
    pos += 2;
    len = footer.find( UString("&C") ) - pos;
    if( len > 0 )
    {
      left = footer.substr( pos, len );
      footer = footer.substr( pos+len, footer.length() );
    }
  }

  // center part
  pos = footer.find( UString("&C") );
  if( pos >= 0 )
  {
    pos += 2;
    len = footer.find( UString("&R") ) - pos;
    if( len > 0 )
    {
      center = footer.substr( pos, len );
      footer = footer.substr( pos+len, footer.length() );
    }
  }

  // right part
  pos = footer.find( UString("&R") );
  if( pos >= 0 )
  {
    pos += 2;
    right = footer.substr( pos, footer.length() - pos );
  }

  d->activeSheet->setLeftFooter( left );
  d->activeSheet->setCenterFooter( center );
  d->activeSheet->setRightFooter( right );
}

void ExcelReader::handleHeader( HeaderRecord* record )
{
  if( !record ) return;

  if( !d->activeSheet ) return;

  UString header = record->header();
  UString left, center, right;
  int pos = -1, len = 0;

  // left part of the header
  pos = header.find( UString("&L") );
  if( pos >= 0 )
  {
    pos += 2;
    len = header.find( UString("&C") ) - pos;
    if( len > 0 )
    {
      left = header.substr( pos, len );
      header = header.substr( pos+len, header.length() );
    }
  }

  // center part of the header
  pos = header.find( UString("&C") );
  if( pos >= 0 )
  {
    pos += 2;
    len = header.find( UString("&R") ) - pos;
    if( len > 0 )
    {
      center = header.substr( pos, len );
      header = header.substr( pos+len, header.length() );
    }
  }

  // right part of the header
  pos = header.find( UString("&R") );
  if( pos >= 0 )
  {
    pos += 2;
    right = header.substr( pos, header.length() - pos );
  }

  d->activeSheet->setLeftHeader( left );
  d->activeSheet->setCenterHeader( center );
  d->activeSheet->setRightHeader( right );
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

void ExcelReader::handleMergedCells( MergedCellsRecord* record )
{
  if( !record ) return;
  
  if( !d->activeSheet ) return;
  
  for( unsigned i = 0; i < record->count(); i++ )
  {
    unsigned firstRow = record->firstRow( i );
    unsigned lastRow = record->lastRow( i );
    unsigned firstColumn = record->firstColumn( i );
    unsigned lastColumn = record->lastColumn( i );
    
    Cell* cell = d->activeSheet->cell( firstColumn, firstRow, true );
    if( cell )
    {
      cell->setColumnSpan( lastColumn - firstColumn + 1 );
      cell->setRowSpan( lastRow - firstRow + 1 );
    }
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

void ExcelReader::handleName( NameRecord* record )
{
  if( !record ) return;

  d->nameTable.push_back( record->definedName() );
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

void ExcelReader::handlePalette( PaletteRecord* record )
{
  if( !record ) return;
  
  d->colorTable.clear();
  for( unsigned i = 0; i < record->count(); i++ )
    d->colorTable.push_back( record->color( i ) );
}
  
void ExcelReader::handleRightMargin( RightMarginRecord* record )
{
  if( !record ) return;
  
  if( !d->activeSheet ) return;
  
  // convert from inches to points
  double margin = record->rightMargin() * 72.0;
  d->activeSheet->setRightMargin( margin );  
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

void ExcelReader::handleRow( RowRecord* record )
{
  if( !record ) return;
  
  if( !d->activeSheet ) return;

  unsigned index = record->row();  
  unsigned xfIndex = record->xfIndex();
  unsigned height = record->height();
  bool hidden = record->hidden();
  
  Row* row = d->activeSheet->row( index, true );
  if( row )
  {
    row->setHeight( height / 20.0 );
    row->setFormat( convertFormat( xfIndex ) );
    row->setVisible( !hidden );
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

void ExcelReader::handleString( StringRecord* record )
{
  if( !record ) return;
  
  if( !d->activeSheet ) return;
  if( !d->formulaCell ) return;
  
  d->formulaCell->setValue( record->value() );
  
  d->formulaCell = 0;
}

void ExcelReader::handleTopMargin( TopMarginRecord* record )
{
  if( !record ) return;

  if( !d->activeSheet ) return;


  // convert from inches to points
  double margin = record->topMargin() * 72.0;
  d->activeSheet->setTopMargin( margin );
}

FormatFont ExcelReader::convertFont( unsigned fontIndex )
{  
  // speed-up trick: check in the cache first  
  FormatFont font = d->fontCache[ fontIndex ];
  if( font.isNull() && ( fontIndex < d->fontTable.size() ))
  {
    FontRecord fr = d->fontTable[ fontIndex ];
    font.setFontSize( fr.height() / 20.0 );
    font.setFontFamily( fr.fontName() );
    font.setColor( convertColor( fr.colorIndex() ) );
    font.setBold( fr.boldness() > 500 );
    font.setItalic( fr.italic() );
    font.setStrikeout( fr.strikeout() );    
    font.setSubscript( fr.escapement() == FontRecord::Subscript );
    font.setSuperscript( fr.escapement() == FontRecord::Superscript );
    font.setUnderline( fr.underline() != FontRecord::None );
    
    // put in the cache for further use
    d->fontCache[ fontIndex ] = font;    
  }  
  
  return font;
}

Color ExcelReader::convertColor( unsigned colorIndex )
{  
  if( ( colorIndex >= 8 ) && ( colorIndex < 0x40 ) )
    if( colorIndex-8 < d->colorTable.size() )
      return d->colorTable[ colorIndex-8 ];
  
  // FIXME the following colors depend on system color settings
  // 0x0040  system window text colour for border lines    
  // 0x0041  system window background colour for pattern background
  // 0x7fff  system window text colour for fonts
  if( colorIndex == 0x40 ) return Color( 0, 0, 0 );
  if( colorIndex == 0x41 ) return Color( 255, 255, 255 );
  if( colorIndex == 0x7fff ) return Color( 0, 0, 0 );
  
  // fallback: just "black"
  Color color;

  // standard colors: black, white, red, green, blue,
  // yellow, magenta, cyan
  switch( colorIndex )
  {
    case 0:   color = Color( 0, 0, 0 ); break; 
    case 1:   color = Color( 255, 255, 255 ); break; 
    case 2:   color = Color( 255, 0, 0 ); break;
    case 3:   color = Color( 0, 255, 0 ); break;
    case 4:   color = Color( 0, 0, 255 ); break;
    case 5:   color = Color( 255, 255, 0 ); break;
    case 6:   color = Color( 255, 0, 255 ); break;
    case 7:   color = Color( 0, 255, 255 ); break;
    default:  break;
  }
  
  return color;
}

// convert border style, e.g MediumDashed to a Pen
static Pen convertBorderStyle( unsigned style )
{
  Pen pen;
  switch( style )
  {
  case XFRecord::NoLine:
    pen.width = 0;
    pen.style = Pen::NoLine;
    break;
  case XFRecord::Thin:
    pen.width = 1;
    pen.style = Pen::SolidLine;
    break;
  case XFRecord::Medium:
    pen.width = 3;
    pen.style = Pen::SolidLine;
    break;
  case XFRecord::Dashed:
    pen.width = 1;
    pen.style = Pen::DashLine;
    break;
  case XFRecord::Dotted:
    pen.width = 1;
    pen.style = Pen::DotLine;
    break;
  case XFRecord::Thick:
    pen.width = 4;
    pen.style = Pen::SolidLine;
    break;
  case XFRecord::Double:
    // FIXME no equivalent ?
    pen.width = 4;
    pen.style = Pen::SolidLine;
    break;
  case XFRecord::Hair:
    // FIXME no equivalent ?
    pen.width = 1;
    pen.style = Pen::DotLine;
    break;
  case XFRecord::MediumDashed:
    pen.width = 3;
    pen.style = Pen::DashLine;
    break;
  case XFRecord::ThinDashDotted:
    pen.width = 1;
    pen.style = Pen::DashDotLine;
    break;
  case XFRecord::MediumDashDotted:
    pen.width = 3;
    pen.style = Pen::DashDotLine;
    break;
  case XFRecord::ThinDashDotDotted:
    pen.width = 1;
    pen.style = Pen::DashDotDotLine;
    break;
  case XFRecord::MediumDashDotDotted:
    pen.width = 3;
    pen.style = Pen::DashDotDotLine;
    break;
  case XFRecord::SlantedMediumDashDotted:
    // FIXME no equivalent ?
    pen.width = 3;
    pen.style = Pen::DashDotLine;
    break;
  default:
    // fallback, simple solid line
    pen.width = 1;
    pen.style = Pen::SolidLine;
    break;    
  };
  
  return pen;
}

unsigned convertPatternStyle( unsigned pattern )
{
  switch ( pattern )
  {
    case 0x00: return FormatBackground::EmptyPattern;
    case 0x01: return FormatBackground::SolidPattern;
    case 0x02: return FormatBackground::Dense4Pattern;
    case 0x03: return FormatBackground::Dense3Pattern;
    case 0x04: return FormatBackground::Dense5Pattern;
    case 0x05: return FormatBackground::HorPattern;
    case 0x06: return FormatBackground::VerPattern;
    case 0x07: return FormatBackground::FDiagPattern;
    case 0x08: return FormatBackground::BDiagPattern;
    case 0x09: return FormatBackground::Dense1Pattern;
    case 0x0A: return FormatBackground::Dense2Pattern;
    case 0x0B: return FormatBackground::HorPattern;
    case 0x0C: return FormatBackground::VerPattern;
    case 0x0D: return FormatBackground::FDiagPattern;
    case 0x0E: return FormatBackground::BDiagPattern;
    case 0x0F: return FormatBackground::CrossPattern;
    case 0x10: return FormatBackground::DiagCrossPattern;
    case 0x11: return FormatBackground::Dense6Pattern;
    case 0x12: return FormatBackground::Dense7Pattern;
    default: return FormatBackground::SolidPattern; // fallback
  }
}

// big task: convert Excel XFormat into Swinder::Format
Format ExcelReader::convertFormat( unsigned xfIndex )
{
  Format format;

  if( xfIndex >= d->xfTable.size() ) return format;

  XFRecord xf = d->xfTable[ xfIndex ];
  
  UString valueFormat = d->formatsTable[xf.formatIndex()];
  if( valueFormat.isEmpty() )
    switch( xf.formatIndex() )
    {
      case  0:  valueFormat = "General"; break;
      case  1:  valueFormat = "0"; break;
      case  2:  valueFormat = "0.00"; break;
      case  3:  valueFormat = "#,##0"; break;
      case  4:  valueFormat = "#,##0.00"; break;
      case  5:  valueFormat = "\"$\"#,##0_);(\"S\"#,##0)"; break;
      case  6:  valueFormat = "\"$\"#,##0_);[Red](\"S\"#,##0)"; break;
      case  7:  valueFormat = "\"$\"#,##0.00_);(\"S\"#,##0.00)"; break;
      case  8:  valueFormat = "\"$\"#,##0.00_);[Red](\"S\"#,##0.00)"; break;
      case  9:  valueFormat = "0%"; break;
      case 10:  valueFormat = "0.00%"; break;
      case 11:  valueFormat = "0.00E+00"; break;
      case 12:  valueFormat = "#?/?"; break;
      case 13:  valueFormat = "#\?\?/\?\?"; break;
      case 14:  valueFormat = "M/D/YY"; break;
      case 15:  valueFormat = "D-MMM-YY"; break;
      case 16:  valueFormat = "D-MMM"; break;
      case 17:  valueFormat = "MMM-YY"; break;
      case 18:  valueFormat = "h:mm AM/PM"; break;
      case 19:  valueFormat = "h:mm:ss AM/PM"; break;
      case 20:  valueFormat = "h:mm"; break;
      case 21:  valueFormat = "h:mm:ss"; break;
      case 22:  valueFormat = "M/D/YY h:mm"; break;
      case 37:  valueFormat = "_(#,##0_);(#,##0)"; break;
      case 38:  valueFormat = "_(#,##0_);[Red](#,##0)"; break;
      case 39:  valueFormat = "_(#,##0.00_);(#,##0)"; break;
      case 40:  valueFormat = "_(#,##0.00_);[Red](#,##0)"; break;
      case 41:  valueFormat = "_(\"$\"*#,##0_);_(\"$\"*#,##0_);_(\"$\"*\"-\");(@_)"; break;
      case 42:  valueFormat = "_(*#,##0_);(*(#,##0);_(*\"-\");_(@_)"; break;
      case 43:  valueFormat = "_(\"$\"*#,##0.00_);_(\"$\"*#,##0.00_);_(\"$\"*\"-\");(@_)"; break;
      case 44:  valueFormat = "_(\"$\"*#,##0.00_);_(\"$\"*#,##0.00_);_(\"$\"*\"-\");(@_)"; break;
      case 45:  valueFormat = "mm:ss"; break;
      case 46:  valueFormat = "[h]:mm:ss"; break;
      case 47:  valueFormat = "mm:ss.0"; break;
      case 48:  valueFormat = "##0.0E+0"; break;
      case 49:  valueFormat = "@"; break;
      default: valueFormat = "General"; break;
    };
  format.setValueFormat( valueFormat );
    
  format.setFont( convertFont( xf.fontIndex() ) );
  
  FormatAlignment alignment;
  switch( xf.horizontalAlignment() )
  {
    case XFRecord::Left:     
      alignment.setAlignX( Format::Left ); break;
    case XFRecord::Right:    
      alignment.setAlignX( Format::Right ); break;
    case XFRecord::Centered: 
      alignment.setAlignX( Format::Center ); break;
    default: break;
    // FIXME still unsupported: Repeat, Justified, Filled, Distributed
  };
  switch( xf.verticalAlignment() )
  {
    case XFRecord::Top:
      alignment.setAlignY( Format::Top ); break;
    case XFRecord::VCentered:
      alignment.setAlignY( Format::Middle ); break;
    case XFRecord::Bottom:
      alignment.setAlignY( Format::Bottom ); break;
    default: break;
    // FIXME still unsupported: Justified, Distributed
  }
  alignment.setWrap( xf.textWrap() );
  format.setAlignment( alignment );

  FormatBorders borders;
    
  Pen pen;
  pen = convertBorderStyle( xf.leftBorderStyle() );
  pen.color = convertColor( xf.leftBorderColor() );
  borders.setLeftBorder( pen );
  
  pen = convertBorderStyle( xf.rightBorderStyle() );
  pen.color = convertColor( xf.rightBorderColor() );
  borders.setRightBorder( pen );
  
  pen = convertBorderStyle( xf.topBorderStyle() );
  pen.color = convertColor( xf.topBorderColor() );
  borders.setTopBorder( pen );
  
  pen = convertBorderStyle( xf.bottomBorderStyle() );
  pen.color = convertColor( xf.bottomBorderColor() );
  borders.setBottomBorder( pen );
  
  format.setBorders( borders );

  FormatBackground background;
  background.setForegroundColor( convertColor( xf.patternForeColor() ) );
  background.setBackgroundColor( convertColor( xf.patternBackColor() ) );
  background.setPattern( convertPatternStyle( xf.fillPattern() ) );
  format.setBackground( background );

  return format;
}

void ExcelReader::handleXF( XFRecord* record )
{
  if( !record ) return;
  
  d->xfTable.push_back( *record );  
}

typedef std::vector<UString> UStringStack;

void mergeTokens( UStringStack* stack, int count, UString mergeString )
{
  if( !stack ) return;
  if( !stack->size() ) return;
  
  UString s1, s2;
	
  while(count)
  {
	count--;
    
    UString last = (*stack)[ stack->size()-1 ];
    UString tmp = last;
    tmp.append( s1 );
    s1 = tmp;

	if( count )
    {
      tmp = mergeString;
      tmp.append( s1 );
      s1 = tmp;
    }

    stack->resize( stack->size()-1 );
  }

  stack->push_back( s1 );
}

#ifdef SWINDER_XLS2RAW
void dumpStack( std::vector<UString> stack )
{
  std::cout << std::endl;
  std::cout << "Stack now is: " ;
  if( !stack.size() )
  std::cout << "(empty)" ;
 
  for( unsigned i = 0; i < stack.size(); i++ )
    std::cout << "  " << i << ": " << stack[i].ascii() << std::endl;
  std::cout << std::endl;
}
#endif

UString ExcelReader::decodeFormula( unsigned row, unsigned col, const FormulaTokens& tokens )
{
  UStringStack stack;
  
  for( unsigned c=0; c < tokens.size(); c++ )
  {
    FormulaToken token = tokens[c];

#ifdef SWINDER_XLS2RAW
    std::cout << "Token " << c << ": ";
    std::cout <<  token.id() << "  "; 
    std::cout << token.idAsString() << std::endl;
#endif

    switch( token.id() )
    {
      case FormulaToken::Add:  
        mergeTokens( &stack, 2, UString("+") );
        break;
        
      case FormulaToken::Sub:  
        mergeTokens( &stack, 2, UString("-") );
        break;
        
      case FormulaToken::Mul:  
        mergeTokens( &stack, 2, UString("*") );
        break;
        
      case FormulaToken::Div:  
        mergeTokens( &stack, 2, UString("/") );
        break;
        
      case FormulaToken::Power:  
        mergeTokens( &stack, 2, UString("^") );
        break;
        
      case FormulaToken::Concat:  
        mergeTokens( &stack, 2, UString("&") );
        break;
        
      case FormulaToken::LT:  
        mergeTokens( &stack, 2, UString("<") );
        break;
        
      case FormulaToken::LE:  
        mergeTokens( &stack, 2, UString("<=") );
        break;
        
      case FormulaToken::EQ:  
        mergeTokens( &stack, 2, UString("=") );
        break;
        
      case FormulaToken::GE:  
        mergeTokens( &stack, 2, UString(">=") );
        break;
        
      case FormulaToken::GT:  
        mergeTokens( &stack, 2, UString(">") );
        break;
        
      case FormulaToken::NE:  
        mergeTokens( &stack, 2, UString("<>") );
        break;
      
      case FormulaToken::Intersect:  
        mergeTokens( &stack, 2, UString(" ") );
        break;
        
      case FormulaToken::List:  
        mergeTokens( &stack, 2, UString(";") );
        break;
      
      case FormulaToken::Range:  
        mergeTokens( &stack, 2, UString(";") );
        break;
      
      case FormulaToken::UPlus:
        {
          UString str( "+" );
          str.append( stack[ stack.size()-1 ] );
          stack[ stack.size()-1 ] = str;
        }
        break;
    
      case FormulaToken::UMinus:  
        {
          UString str( "-" );
          str.append( stack[ stack.size()-1 ] );
          stack[ stack.size()-1 ] = str;
        }
        break;
    
      case FormulaToken::Percent:  
        stack[ stack.size()-1 ].append( UString("%") );
        break;
    
      case FormulaToken::Paren:  
        {
          UString str( "(" );
          str.append( stack[ stack.size()-1 ] );
          str.append( UString(")") );
          stack[ stack.size()-1 ] = str;
        }
        break;
    
      case FormulaToken::MissArg:
        // just ignore
        stack.push_back( UString(" ") );
        break;
    
      case FormulaToken::String:
        {
          UString str( '\"' );
          str.append( token.value().toString() );
          str.append( UString( '\"' ) );
          stack.push_back( str );
        }
        break;
    
      case FormulaToken::Bool:
        if( token.value().asBoolean() )
          stack.push_back( UString( "TRUE" ) );
        else  
          stack.push_back( UString( "FALSE" ) );
        break;
        
      case FormulaToken::Integer:
        stack.push_back( UString::from( token.value().asInteger() ) );
        break;
        
      case FormulaToken::Float:
        stack.push_back( UString::from( token.value().asFloat() ) );
        break;
        
      case FormulaToken::Array:
        // FIXME handle this !
        break;
      
      case FormulaToken::Ref:
        stack.push_back( token.ref( row, col ) );
        break;
      
      case FormulaToken::Area:
        stack.push_back( token.area( row, col ) );
        break;

      case FormulaToken::Function:
        {
          mergeTokens( &stack, token.functionParams(), UString(";") );
          if( stack.size() )
          {
            UString str( token.functionName() ? token.functionName() : "??" );
            str.append( UString("(") );
            str.append( stack[ stack.size()-1 ] );
            str.append( UString(")") );
            stack[ stack.size()-1 ] = str;
          }
        }
        break;

      case FormulaToken::FunctionVar:
        if( token.functionIndex() != 255 )
        {
          mergeTokens( &stack, token.functionParams(), UString(";") );
          if( stack.size() )
          {
            UString str;
            if( token.functionIndex() != 255 )
              str = token.functionName() ? token.functionName() : "??";
            str.append( UString("(") );
            str.append( stack[ stack.size()-1 ] );
            str.append( UString(")") );
            stack[ stack.size()-1 ] = str;
          }
        }
        else
        {
          unsigned count = token.functionParams()-1;
          mergeTokens( &stack, count, UString(";") );
          if( stack.size() )
          {
            UString str;
            str.append( UString("(") );
            str.append( stack[ stack.size()-1 ] );
            str.append( UString(")") );
            stack[ stack.size()-1 ] = str;
          }
        }
        break;

      case FormulaToken::Attr:
        if( token.attr() & 0x10 )  // SUM
        {
          mergeTokens( &stack, 1, UString(";") );
          if( stack.size() )
          {
            UString str( "SUM" );
            str.append( UString("(") );
            str.append( stack[ stack.size()-1 ] );
            str.append( UString(")") );
            stack[ stack.size()-1 ] = str;
          }
        }
        break;

      case FormulaToken::NameX:
        if( token.nameIndex() > 0 )
        if( token.nameIndex() <= d->nameTable.size() )
          stack.push_back( d->nameTable[ token.nameIndex()-1 ] );
        break;

      case FormulaToken::NatFormula:
      case FormulaToken::Sheet:
      case FormulaToken::EndSheet:
      case FormulaToken::ErrorCode:
      case FormulaToken::Name:
      case FormulaToken::MemArea:
      case FormulaToken::MemErr:
      case FormulaToken::MemNoMem:
      case FormulaToken::MemFunc:
      case FormulaToken::RefErr:
      case FormulaToken::AreaErr:
      case FormulaToken::RefN:
      case FormulaToken::AreaN:
      case FormulaToken::MemAreaN:
      case FormulaToken::MemNoMemN:
      case FormulaToken::Ref3d:
      case FormulaToken::Area3d:
      case FormulaToken::RefErr3d:
      case FormulaToken::AreaErr3d:
      default:
        // FIXME handle this !
        stack.push_back( UString("Unknown") );
        break;
    };

#ifdef SWINDER_XLS2RAW
    dumpStack( stack );
#endif

  }
  
  UString result;
  for( unsigned i = 0; i < stack.size(); i++ )
    result.append( stack[i] );
  
#ifdef SWINDER_XLS2RAW
  std::cout << "FORMULA Result: " << result << std::endl;
#endif
  return result;
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
