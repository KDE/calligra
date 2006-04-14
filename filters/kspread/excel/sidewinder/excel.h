/* Swinder - Portable library for spreadsheet
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA
*/

#ifndef SWINDER_EXCEL_H
#define SWINDER_EXCEL_H

#include <string>
#include <iostream>
#include <vector>

#include "swinder.h"

namespace Swinder
{

/**
    Supported Excel document version.
*/
enum { UnknownExcel = 0, Excel95, Excel97, Excel2000 };

class Record;

// rich-text, unicode, far-east support Excel string

class EString
{
public:

  EString();
  
  EString( const EString& );
  
  EString& operator=( const EString& );
  
  ~EString();
  
  bool unicode() const;
  
  void setUnicode( bool u );
  
  bool richText() const;
  
  void setRichText( bool r );
    
  UString str() const;
  
  void setStr( const UString& str );
  
  // space allocate for the string, not length (use string.length() for that) 
  unsigned size() const;
  void setSize( unsigned size ); // HACKS
  
  
  static EString fromUnicodeString( const void* p, bool longString, unsigned maxsize = 0 );
  
  static EString fromSheetName( const void* p, unsigned maxsize = 0 );
  
  // from the buffer
  // longstring means 16-bit string length, usually for label
  // longstring=false is normally for sheet name
  static EString fromByteString( const void* p, bool longString, unsigned maxsize = 0 );
  
private:
  class Private;
  Private* d;
};

class FormulaToken
{
public:

  enum
  {
    // should match Excel's PTG
    Unused      = 0x00,
    Matrix      = 0x01,
    Table       = 0x02,
    Add         = 0x03,
    Sub         = 0x04,
    Mul         = 0x05,
    Div         = 0x06,
    Power       = 0x07,
    Concat      = 0x08,
    LT          = 0x09,
    LE          = 0x0a,
    EQ          = 0x0b,
    GE          = 0x0c,
    GT          = 0x0d,
    NE          = 0x0e,
    Intersect   = 0x0f,
    List        = 0x10,
    Range       = 0x11,
    UPlus       = 0x12,
    UMinus      = 0x13,
    Percent     = 0x14,
    Paren       = 0x15,
    MissArg     = 0x16,
    String      = 0x17,
    NatFormula  = 0x18,
    Attr        = 0x19,
    Sheet       = 0x1a,
    EndSheet    = 0x1b,
    ErrorCode   = 0x1c,
    Bool        = 0x1d,
    Integer     = 0x1e,
    Float       = 0x1f,
    Array       = 0x20,
    Function    = 0x21,
    FunctionVar = 0x22,
    Name        = 0x23,
    Ref         = 0x24,
    Area        = 0x25,
    MemArea     = 0x26,
    MemErr      = 0x27,
    MemNoMem    = 0x28,
    MemFunc     = 0x29,
    RefErr      = 0x2a,
    AreaErr     = 0x2b,
    RefN        = 0x2c,
    AreaN       = 0x2d,
    MemAreaN    = 0x2e,
    MemNoMemN   = 0x2f,
    NameX       = 0x39,
    Ref3d       = 0x3a,
    Area3d      = 0x3b,
    RefErr3d    = 0x3c,
    AreaErr3d   = 0x3d
  };

  FormulaToken();
  FormulaToken( unsigned id );
  FormulaToken( const FormulaToken& );
  ~FormulaToken();
  
  // token id, excluding token class
  unsigned id() const;
  const char* idAsString() const;
  
  // Excel version
  unsigned version() const;
  void setVersion( unsigned version );  // Excel version
  
  // size of data, EXCLUDING the byte for token id
  unsigned size() const;
  void setData( unsigned size, const unsigned char* data );
  
  // only when id returns ErrorCode, Bool, Integer, Float, or String
  Value value() const;
  
  // only when id is Function or FunctionVar
  unsigned functionIndex() const;
  const char* functionName() const;  // for non external function
  unsigned functionParams() const;
  
  // only when id is Ref or Ref3d
  UString ref( unsigned row, unsigned col ) const;

  // only when id is Area or Area3d
  UString area( unsigned row, unsigned col ) const;

  // only when id is Ref3d or Area3d
  unsigned externSheetRef() const;

  // only when id is Attr
  unsigned attr() const;

  // only when id is NameX
  unsigned nameIndex() const;

  // only when id is Matrix
  unsigned refRow() const;
  unsigned refColumn() const;

private:
  class Private;
  Private *d;  
};

typedef std::vector<FormulaToken> FormulaTokens;

std::ostream& operator<<( std::ostream& s, FormulaToken token );

/**
  Class Record represents a base class for all other type record,
  hence do not use this class in real life.
   
 */
class Record
{
public:

  /**
    Static ID of the record. Subclasses should override this value
    with the id of the record they handle.
  */  
  static const unsigned int id;
  
  virtual unsigned int rtti(){
	  return this->id;
  }
  
  /**
    Creates a new generic record.
  */  
  Record();
  
  /**
    Destroys the record.
  */
  virtual ~Record();
  
  /**
   * Record factory, create a new record of specified type.
   */
  static Record* create( unsigned type );
  
  void setVersion( unsigned v ){ ver = v; };
  
  unsigned version(){ return ver; };

  /**
    Sets the data for this record.
   */
  virtual void setData( unsigned size, const unsigned char* data );
  
  /**
    Sets the position of the record in the OLE stream. Somehow this is
    required to process BoundSheet and BOF(Worksheet) properly.
   */
  void setPosition( unsigned pos );
  
  /**
    Gets the position of this record in the OLE stream. 
   */
  unsigned position() const;
  
  /**
    Returns the name of the record. For debugging only.
   */
  virtual const char* name(){ return "Unknown"; }

  /**
    Dumps record information to output stream. For debugging only.
   */
  virtual void dump( std::ostream& out ) const;
  
protected:

   // position of this record in the OLE stream
   unsigned stream_position;

   // in which version does this record denote ?
   unsigned ver;

private:
   // no copy or assign
   Record( const Record& );
   Record& operator=( const Record& );
   
};

/**
  Class CellInfo represents a base class for records which provide information
  about cells. Some important records like BlankRecord, LabelRecord, and others
  inherit this class.   
 */
class CellInfo
{
public:

  /**
   * Creates a new cell information.
   */
  CellInfo();

  /**
   * Destroys the cell information.
   */
  virtual ~CellInfo();

  /**
   * Returns the row associated with the cell information. It is zero based,
   * so the first row is 0.
   *
   * \sa setRow, column
   */
  virtual unsigned row() const;

  /**
   * Returns the column associated with the cell information. It is zero based,
   * so the first column is 0.
   *
   * \sa setColumn, row
   */
  virtual unsigned column() const;

  /**
   * Returns the XF index for formatting of the cell.
   *
   * \sa setXfIndex
   */
  virtual unsigned xfIndex() const;

  /**
   * Sets the row associated with the cell information. It is zero based,
   * so the first row is 0.
   *
   * \sa setColumn, row
   */
  virtual void setRow( unsigned r );

  /**
   * Sets the column associated with the cell information. It is zero based,
   * so the first column is 0.
   *
   * \sa setRow, column
   */
  virtual void setColumn( unsigned c );

  /**
   * Sets the XF index for formatting of the cell.
   *
   * \sa xfIndex
   */
  virtual void setXfIndex( unsigned i );

private:
   // no copy or assign
   CellInfo( const CellInfo& );
   CellInfo& operator=( const CellInfo& );

   class Private;
   Private* info;
};

/**
  Class CellInfo represents a base class for records which provide information
  about a span of columns. The information, like formatting index, should 
  apply to columns (as specified) from firstColumn and lastColumn.
 */
class ColumnSpanInfo
{
public:

  /**
   * Creates a new column span information.
   */
  ColumnSpanInfo();

  /**
   * Destroys the column span information.
   */
  virtual ~ColumnSpanInfo();

  /**
   * Returns the first column associated with the span information. 
   * Column index is zero based, so the first column is 0.
   *
   * \sa lastColumn, setFirstColumn
   */
  virtual unsigned firstColumn() const;

  /**
   * Returns the last column associated with the span information. 
   * Column index is zero based, so the first column is 0.
   *
   * \sa firstColumn, setLastColumn
   */
  virtual unsigned lastColumn() const;

  /**
   * Sets the first column associated with the span information. 
   * Column index is zero based, so the first column is 0.
   *
   * \sa setLastColumn, firstColumn
   */
  virtual void setFirstColumn( unsigned c );

  /**
   * Sets the last column associated with the span information. 
   * Column index is zero based, so the first column is 0.
   *
   * \sa setFirstColumn, lastColumn
   */
  virtual void setLastColumn( unsigned c );

private:
   // no copy or assign
   ColumnSpanInfo( const ColumnSpanInfo& );
   ColumnSpanInfo& operator=( const ColumnSpanInfo& );

   class Private;
   Private* spaninfo;
};

/**
  \brief Backup upon save.
  
  Class BackupRecord represents Backup record, which determines whether 
  Microsoft Excel makes a backup of the file while saving.
 */
class BackupRecord : public Record
{
public:

  static const unsigned int id;

  /**
   * Creates a new Backup record.
   */
  BackupRecord();
  
  /**
   * Destroy the record.
   */
  ~BackupRecord();
  
  unsigned int rtti(){
	  return this->id;
  }
  /**
   * Returns true if a backup is made when saving the file.
   * 
   * \sa setBackup
   */
  bool backup() const;
  
  /**
   * If r is true, a backup will be made when saving the file.
   * 
   * \sa backup
   */
  void setBackup( bool r );

  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "BACKUP"; }

  virtual void dump( std::ostream& out ) const;

private:
  // no copy or assign
  BackupRecord( const BackupRecord& );
  BackupRecord& operator=( const BackupRecord& );

  class Private;
  Private* d;
};


/**
  \brief Beginning of file/set of records.
  
  Class BOFRecord represents BOF (Beginning of File) record, which
  is used to mark beginning of a set of records (following the BOF record).
  For each BOF record, there should also be one corresponding EOF record.

  Note that the name "BOF" is rather misleading. This is because of
  historical reason only.

  \sa EOFRecord

 */
class BOFRecord : public Record
{
public:

  /**
    Static ID of the BOF record.
  */
  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
    Supported BOF type.
  */
  enum { UnknownType = 0, Workbook, Worksheet, Chart, VBModule, MacroSheet, Workspace };

  /**
   * Creates a new BOF record.
   */
  BOFRecord();

  /**
    Destroys the record.
  */
  virtual ~BOFRecord();

  virtual void setData( unsigned size, const unsigned char* data );

  /**
    Returns the version, like Excel95, Excel97, and so on.

    Note that it is possible to use expression like 'version() >= Excel97'
    but always do that carefully.
  */
  unsigned version() const;

  /**
    Returns the version as string, something like "Excel97".
  */
  const char* versionAsString() const;

  /**
    Returns type of the BOF record, like Workbook, Chart, and so on.
  */
  unsigned type() const;

  /**
    Returns BOF type as string, something like "Worksheet".
  */
  const char* typeAsString() const;

  virtual const char* name(){ return "BOF"; }

  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   BOFRecord( const BOFRecord& );
   BOFRecord& operator=( const BOFRecord& );

   class Private;
   Private *d;
};

/**
  \brief Blank cell.
  
  Class BlankRecord represents a blank cell. It contains information
  about cell address and formatting.
 */
class BlankRecord : public Record, public CellInfo
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new Blank record.
   */
  BlankRecord();

  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "BLANK"; }

  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   BlankRecord( const BlankRecord& );
   BlankRecord& operator=( const BlankRecord& );
};

/**
  \brief Boolean value or error code.
  
  Class BOFRecord represents BoolErr record, which
  is used to store boolean value or error code of a cell.
 */
class BoolErrRecord : public Record, public CellInfo
{
public:

  /**
    Static ID of the BoolErr record.
  */
  static const unsigned int id;

  unsigned int rtti(){ return this->id; }

  /**
   * Creates a new BoolErr record.
   */
  BoolErrRecord();

  /**
   * Destroys the BoolErr record.
   */
  virtual ~BoolErrRecord();

  /**
   * Returns value of the cell, could be either boolean or error.
   */
  Value value() const;

  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "BOOLERR"; }

  virtual void dump( std::ostream& out ) const;

private:
  // no copy or assign
  BoolErrRecord( const BoolErrRecord& );
  BoolErrRecord& operator=( const BoolErrRecord& );

  class Private;
  Private* d;
};

/**
  \brief Bottom margin.
  
  Class BottomMarginRecord holds information about bottom margin 
  (in inches).
 */
class BottomMarginRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }
  /**
   * Creates a new BottomMargin record.
   */
  BottomMarginRecord();
  
  /**
   * Destroy the record.
   */
  ~BottomMarginRecord();
  
  /**
   * Gets the bottom margin (in inches).
   */
  double bottomMargin() const;
  
  /**
   * Sets the new bottom margin (in inches).
   */
  void setBottomMargin( double m );

  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "BOTTOMMARGIN"; }

  virtual void dump( std::ostream& out ) const;

private:
  // no copy or assign
  BottomMarginRecord( const BottomMarginRecord& );
  BottomMarginRecord& operator=( const BottomMarginRecord& );

  class Private;
  Private* d;
};

/**
  \brief Sheet information.
  
  Class BoundSheetRecord represents BoundSheet record, which defines a sheet
  within the workbook. There must be exactly one BoundSheet record for 
  each sheet.
  
  BoundSheet record stores information about sheet type, sheet name, and 
  the corresponding BOF record.
  
  \sa BOFRecord
 */
 
// TODO support for strong visible

class BoundSheetRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new BoundSheet record.
   */
  BoundSheetRecord();

  /**
   * Destroys the BoundSheet record.
   */
  virtual ~BoundSheetRecord();
  
  /**
   * Type of the sheet.
   */
  enum { Worksheet=0, Chart=2, VBModule=6 };
  
  /**
   * Sets the type of the BoundSheet. Possible values are 
   * BoundSheet::Worksheet, BoundSheet::Chart and BoundSheet::VBModule.
   */
  void setType( unsigned type );
  
  /**
   * Returns the type of the BoundSheet. Possible values are 
   * BoundSheet::Worksheet, BoundSheet::Chart and BoundSheet::VBModule.
   */
  unsigned type() const;
  
  /**
   * Returns the type of the BoundSheet as string. For example, if 
   * type of BoundSheet is BoundSheet::Chart, then this function returns
   * "Chart".
   */
  const char* typeAsString() const;
  
  /**
   * Sets the visibility of the sheet.
   */
  void setVisible( bool visible );
  
  /**
   * Returns true if the sheet is visible.
   */
  bool visible() const;
  
  /**
   * Sets the name of the sheet.
   */
  void setSheetName( const UString& name );
  
  /**
   * Returns the name of the sheet.
   */
  UString sheetName() const;
  
  /**
   * Sets the position of the BOF record associated with this BoundSheet.
   */
  void setBofPosition( unsigned pos );
  
  /**
   * Returns the position of the BOF record associated with this BoundSheet.
   */
  unsigned bofPosition() const;

  virtual void setData( unsigned size, const unsigned char* data );
  
  virtual const char* name(){ return "BOUNDSHEET"; }
  
  virtual void dump( std::ostream& out ) const;

private:
  // no copy or assign
  BoundSheetRecord( const BoundSheetRecord& );
  BoundSheetRecord& operator=( const BoundSheetRecord& );

  class Private;
  Private* d;
};

/**
  \brief Automatic recalculation mode.
  
  Class CalcModeRecord represents CalcMode record, which specifies whether 
  to (re)calculate formulas manually or automatically.
 */
class CalcModeRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new CalcMode record.
   */
  CalcModeRecord();
  
  /**
   * Destroy the record.
   */
  ~CalcModeRecord();
  
  /**
   * Returns true if formulas calculation is performed automatically.
   * 
   * \sa setAutoCalc
   */
  bool autoCalc() const;
  
  /**
   * If r is true, formulas calculation will be performed automatically.
   * 
   * \sa autoCalc
   */
  void setAutoCalc( bool r );

  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "CALCMODE"; }

  virtual void dump( std::ostream& out ) const;

private:
  // no copy or assign
  CalcModeRecord( const CalcModeRecord& );
  CalcModeRecord& operator=( const CalcModeRecord& );

  class Private;
  Private* d;
};

/**
  \brief Columns width and format.
  
  Class ColInfoRecord represents ColInfo record, which provides information
  (such as column width and formatting) for a span of columns.
 */
class ColInfoRecord : public Record, public ColumnSpanInfo
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new ColInfo record.
   */
  ColInfoRecord();

  /**
   * Destroys the record.
   */
  virtual ~ColInfoRecord();
  
  /**
   * Returns the XF index for the formatting of the column(s).
   *
   * \sa setXfIndex
   */
  unsigned xfIndex() const;
  
  /**
   * Sets the XF index for the formatting of the column(s).
   *
   * \sa xfIndex
   */
  void setXfIndex( unsigned i );
  
  /**
   * Returns the width of the column(s), specified in 1/256 of 
   * a character width. The exact width (in pt or inch) could only be 
   * calculated given the base character width for the column format.
   *
   * \sa setWidth
   */
  unsigned width() const;
  
  /**
   * Sets the width of the column(s), specified in 1/256 of 
   * a character width. The exact width (in pt or inch) could only be 
   * calculated given the base character width for the column format.
   *
   * \sa width
   */
  void setWidth( unsigned w );
  
  /**
   * Returns true if the columns should be hidden, i.e not visible.
   *
   * \sa setHidden
   */
  bool hidden() const;
  
  /**
   * Sets whether columns should be hidden or visible.
   *
   * \sa hidden
   */
  void setHidden( bool h );

  /**
   * Returns true if the columns should be collapsed.
   *
   * \sa setCollapsed
   */
  bool collapsed() const;
  
  /**
   * Sets whether columns should be collapsed or not.
   *
   * \sa collapsed
   */
  void setCollapsed( bool c );
  
  /**
   * Returns the outline level of the columns. If it is 0, then
   * the columns are not outlined.
   *
   * \sa setOutlineLevel
   */
  unsigned outlineLevel() const;
  
  /**
   * Sets the outline level of the columns. If it is 0, then
   * the columns are not outlined.
   *
   * \sa outlineLevel
   */
  void setOutlineLevel( unsigned l );
  
  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "COLINFO"; }

  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   ColInfoRecord( const ColInfoRecord& );
   ColInfoRecord& operator=( const ColInfoRecord& );

   class Private;
   Private *d;
};

/**
  \brief Date reference.
  
  Class DateModeRecord represents DateMode record, which specifies
  reference date for displaying date value of given serial number.
  If base1904 is true, the reference date is 1st of January, 1904 (in which
  serial number 1 means 2nd of January, 1904). Otherwise, the reference
  date is 31st of December, 1899 (in which serial number 1 means
  1st of January, 1900).
 */
class DateModeRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new DateMode record.
   */
  DateModeRecord();
  
  /**
   * Destroy the record.
   */
  ~DateModeRecord();
  
  /**
   * Returns true if the reference date is 1st of January, 1904 or false
   * if the reference date is 31st of December, 1899.
   * 
   * \sa setBase1904
   */
  bool base1904() const;
  
  /**
   * If r is true, sets the reference date to 1st of January, 1904. Else,
   * sets the reference date to 31st of December, 1899.
   * 
   * \sa base1904
   */
  void setBase1904( bool r );

  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "DATEMODE"; }

  virtual void dump( std::ostream& out ) const;

private:
  // no copy or assign
  DateModeRecord( const DateModeRecord& );
  DateModeRecord& operator=( const DateModeRecord& );

  class Private;
  Private* d;
};

/**
  \brief Range of used area.
  
  Class DimensionRecord represents Dimension record, which contains the 
  range address of the used area in the current sheet.
 */
class DimensionRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new Dimension record.
   */
  DimensionRecord();
  
  /**
   * Destroys the record.
   */
  ~DimensionRecord();
  
  /**
   * Returns index to the first used row.
   *
   * \sa setFirstRow, lastRow
   */
  unsigned firstRow() const;
  
  /**
   * Sets index to the first used row.
   *
   * \sa firstRow, setLastRow
   */
  void setFirstRow( unsigned r );
  
  /**
   * Returns index to the last used row.
   *
   * \sa setLastRow, firstRow
   */
  unsigned lastRow() const;
  
  /**
   * Sets index to the last used row.
   *
   * \sa lastRow, setFirstRow
   */
  void setLastRow( unsigned r );
  
  /**
   * Returns index to the first used column.
   *
   * \sa setFirstColumn, lastColumn
   */
  unsigned firstColumn() const;
  
  /**
   * Sets index to the first used column.
   *
   * \sa firstColumn, setLastColumn
   */
  void setFirstColumn( unsigned r );
  
  /**
   * Returns index to the last used column.
   *
   * \sa setLastColumn, firstColumn
   */
  unsigned lastColumn() const;
  
  /**
   * Sets index to the last used column.
   *
   * \sa lastColumn, setFirstColumn
   */
  void setLastColumn( unsigned r );
  
  virtual const char* name(){ return "DIMENSION"; }
  
  virtual void setData( unsigned size, const unsigned char* data );
  
  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   DimensionRecord( const DimensionRecord& );
   DimensionRecord& operator=( const DimensionRecord& );
   
   class Private;
   Private *d;
};

class ExternNameRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
    return this->id;
  }

  ExternNameRecord();

  ~ExternNameRecord();

  // one-based sheet index
  // if 0 means AddIn function
  unsigned sheetIndex() const;

  void setSheetIndex( unsigned sheetIndex );

  UString externName() const;

  void setExternName( const UString& name );

  virtual void setData( unsigned size, const unsigned char* data );



  virtual const char* name(){ return "EXTERNNAME"; }

  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   ExternNameRecord( const ExternNameRecord& );
   ExternNameRecord& operator=( const ExternNameRecord& );
   
   class Private;
   Private *d;
};

class ExternSheetRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
    return this->id;
  }

  ExternSheetRecord();

  ~ExternSheetRecord();

  unsigned count() const;

  unsigned refIndex(unsigned i) const;
  unsigned firstSheet(unsigned i) const;
  unsigned lastSheet(unsigned i) const;

  UString refName() const;

  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "EXTERNSHEET"; }

  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   ExternSheetRecord( const ExternNameRecord& );
   ExternSheetRecord& operator=( const ExternNameRecord& );
   
   class Private;
   Private *d;
};


/**
  \brief End of record set.
  
  Class EOFRecord represents EOF record, which marks the end of records
  for a specific object. EOF record should be always in pair with BOF Record.

  \sa BOFRecord

 */
class EOFRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new EOF record.
   */
  EOFRecord();

  /**
   * Destroy the record.
   */
  virtual ~EOFRecord();
  
  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "EOF"; }

  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   EOFRecord( const EOFRecord& );
   EOFRecord& operator=( const EOFRecord& );
};

class FilepassRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
    return this->id;
  }

  /**
   * Creates a new FILEPASS record.
   */
  FilepassRecord();

  /**
   * Destroy the record.
   */
  virtual ~FilepassRecord();
  
  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "FILEPASS"; }

  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   FilepassRecord( const FilepassRecord& );
   FilepassRecord& operator=( const FilepassRecord& );
};

/**
  \brief Font information.
  
  Class FontRecord represents Font record, which has the information
  about specific font used in the document. Several Font records creates
  a font table, whose index will be referred in XFormat records.
  
  A note about weirdness: font index #4 is never used. Hence, the first Font
  record will be index #0, the second is #1, the third is #2, the fourth is
  #3, but the fourth will be index #5.

  \sa XFRecord

 */
class FontRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new Font record.
   */
  FontRecord();
  
  /**
   * Creates a copy of another Font record.
   */
  FontRecord( const FontRecord& fr );
  
  /**
   * Assigns from another Font record.
   */
  FontRecord& operator=( const FontRecord& fr );

  /**
   * Destroy the record.
   */
  virtual ~FontRecord();
  
  enum { 
    Normal = 0, 
    Superscript = 1,
    Subscript = 2 };
    
  enum { 
    None = 0, 
    Single = 1, 
    Double = 2, 
    SingleAccounting = 0x21, 
    DoubleAccounting = 0x22 };
  
  unsigned height() const;
  void setHeight( unsigned h );
  
  /**
   * Returns the name of font, e.g "Arial".
   *
   * \sa setFontName
   */
  UString fontName() const;
  
  /**
   * Sets the name of the font.
   *
   * \sa fontName
   */
  void setFontName( const UString& fn );

  // FIXME what is this font family ? index ?  
  unsigned fontFamily() const;
  void setFontFamily( unsigned f );
  
  // FIXME and character set index ?
  unsigned characterSet() const;
  void setCharacterSet( unsigned s );
  
  /**
   * Returns index of the color of the font.
   *
   * \sa setColorIndex
   */
  unsigned colorIndex() const;
  
  /**
   * Sets the index of the color of the font.
   *
   * \sa colorIndex
   */
  void setColorIndex( unsigned c );
  
  /**
   * Returns the boldness of the font. Standard values are 400 for normal
   * and 700 for bold.
   *
   * \sa setBoldness
   */
  unsigned boldness() const;
  
  /**
   * Sets the boldness of the font. Standard values are 400 for normal
   * and 700 for bold.
   *
   * \sa boldness
   */
  void setBoldness( unsigned b );
  
  /**
   * Returns true if italic has been set.
   *
   * \sa setItalic
   */  
  bool italic() const;
  
  /**
   * If i is true, italic is set on; otherwise italic is set off.
   *
   * \sa italic
   */  
  void setItalic( bool i );
  
  /**
   * Returns true if strikeout has been set.
   *
   * \sa setStrikeout
   */
  bool strikeout() const;
  
  /**
   * If s is true, strikeout is set on; otherwise strikeout is set off.
   *
   * \sa strikeout
   */
  void setStrikeout( bool s );
  
  /**
   * Returns Font::Superscript if superscript is set, or Font::Subscript
   * if subscript is set, or Font::Normal in other case.
   *
   * \sa setEscapement
   */
  unsigned escapement() const;
  
  /**
   * Sets the superscript or subscript. If s is Font::Superscript, then
   * superscript is set. If s is Font::Subscript, then subscript is set.
   *
   * \sa escapement
   */
  void setEscapement( unsigned s );
  
  /**
   * Returns the underline style of the font. Possible values are
   * Font::None, Font::Single, Font::Double, Font::SingleAccounting and
   * Font::DoubleAccounting.
   *
   * \sa setUnderline
   */  
  unsigned underline() const;
  
  /**
   * Sets the underline style of the font. Possible values are
   * Font::None, Font::Single, Font::Double, Font::SingleAccounting and
   * Font::DoubleAccounting.
   *
   * \sa underline
   */  
  void setUnderline( unsigned u );
  
  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "FONT"; }

  virtual void dump( std::ostream& out ) const;

private:
   class Private;
   Private *d;
};

/**
  \brief Sheet footer.
  
  Class FooterRecord holds information about sheet footer.
  
 */
class FooterRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new Footer record.
   */
  FooterRecord();
  
  /**
   * Destroy the record.
   */
  ~FooterRecord();
  
  /**
   * Gets the footer.
   */
  UString footer() const;
  
  /**
   * Sets the footer.
   */
  void setFooter( const UString& f );
  
  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "FOOTER"; }

  virtual void dump( std::ostream& out ) const;

private:
  // no copy or assign
  FooterRecord( const FooterRecord& );
  FooterRecord& operator=( const FooterRecord& );

  class Private;
  Private* d;
};

/**
  \brief Number formatting string.
  
  Class FormatRecord contains information about a number format. 
  All Format records occur together in a sequential list.
  An XFRecord might refer to the specific Format record using
  an index to that list.

  \sa XFRecord

 */
class FormatRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new Format record.
   */
  FormatRecord();
  
  /**
   * Destroys the Format record.
   */
  ~FormatRecord();
  
  /**
   * Creates a copy of Format record.
   */
  FormatRecord( const FormatRecord& fr );
  
  /**
   * Assigns from another Format record.
   */
  FormatRecord& operator=( const FormatRecord& fr );
  
  /**
   * Returns the index of the format. Each format specified by Format record
   * has unique index which will be referred by XF Record.
   *
   * \sa setIndex
   */
  unsigned index() const;
  
  /**
   * Sets the index of the format. Each format specified by Format record
   * has unique index which will be referred by XF Record.
   *
   * \sa index
   */
  void setIndex( unsigned i );

  /**
   * Returns the formatting string of the format, e.g "0.00" for 2 decimal
   * places number formatting.
   *
   * \sa setFormatString
   */
  UString formatString() const;
  
  /**
   * Sets the formatting string of the format.
   *
   * \sa formatString
   */
  void setFormatString( const UString& fs );
    
  virtual const char* name(){ return "FORMAT"; }
  
  virtual void setData( unsigned size, const unsigned char* data );

  virtual void dump( std::ostream& out ) const;

private:
   class Private;
   Private *d;
};

/**
  \brief Formula.
  
  Class FormulaRecord holds Formula record.
  
 */
class FormulaRecord : public Record, public CellInfo
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new formula record.
   */
  FormulaRecord();
  
  /**
   * Destroy the record.
   */
  ~FormulaRecord();
  
  /**
   * Gets the result of the formula.
   */
  Value result() const;
  
  /**
   * Sets the result of the formula.
   */
  void setResult( const Value& v );
  
  FormulaTokens tokens() const;
  
  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "FORMULA"; }

  virtual void dump( std::ostream& out ) const;

private:
  // no copy or assign
  FormulaRecord( const FormulaRecord& );
  FormulaRecord& operator=( const FormulaRecord& );

  class Private;
  Private* d;
};

/**
  \brief Sheet header.
  
  Class HeaderRecord holds information about sheet header.  
 */

class HeaderRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new Header record.
   */
  HeaderRecord();
  
  /**
   * Destroy the record.
   */
  ~HeaderRecord();
  
  /**
   * Gets the header.
   */
  UString header() const;
  
  /**
   * Sets the header.
   */
  void setHeader( const UString& h );
  
  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "HEADER"; }

  virtual void dump( std::ostream& out ) const;

private:
  // no copy or assign
  HeaderRecord( const HeaderRecord& );
  HeaderRecord& operator=( const HeaderRecord& );

  class Private;
  Private* d;
};


/**
  Class LabelRecord represents a cell that contains a string.
   
  In Excel 97 and later version, it is replaced by LabelSSTRecord. However,
  Excel 97 can still load LabelRecord.

  \sa LabelSSTRecord

 */
class LabelRecord : public Record, public CellInfo
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new Label record.
   */
  LabelRecord();

  /**
   * Destroys the record.
   */
  virtual ~LabelRecord();

  /**
   * Returns the label string.
   */
  UString label() const;
  
  /**
   * Sets the label string.
   */
  void setLabel( const UString& l );

  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "LABEL"; }
  
  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   LabelRecord( const LabelRecord& );
   LabelRecord& operator=( const LabelRecord& );

   class Private;
   Private *d;
};

/**
  Class LabelSSTRecord represents a cell that contains a string. The actual
  string is store in a global SST (Shared String Table), see SSTRecord for
  details. This record only provide an index, which should be used to get
  the string in the corresponding SST.

  \sa SSTRecord

 */
class LabelSSTRecord : public Record, public CellInfo
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new LabelSST record.
   */
  LabelSSTRecord();

  /**
   * Destroys the record.
   */
  virtual ~LabelSSTRecord();

  /**
   * Returns the SST index. This is the index to the global SST which hold
   * every label strings used in SST record. 
   */
  unsigned sstIndex() const;

  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "LABELSST"; }
  
  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   LabelSSTRecord( const LabelSSTRecord& );
   LabelSSTRecord& operator=( const LabelSSTRecord& );

   class Private;
   Private *d;
};

/**
  Class LeftMarginRecord holds information about left margin.
  
 */
class LeftMarginRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new LeftMargin record.
   */
  LeftMarginRecord();
  
  /**
   * Destroy the record.
   */
  ~LeftMarginRecord();
  
  /**
   * Gets the left margin (in inches).
   */
  double leftMargin() const;
  
  /**
   * Sets the new left margin (in inches).
   */
  void setLeftMargin( double m );

  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "LEFTMARGIN"; }

  virtual void dump( std::ostream& out ) const;

private:
  // no copy or assign
  LeftMarginRecord( const LeftMarginRecord& );
  LeftMarginRecord& operator=( const LeftMarginRecord& );

  class Private;
  Private* d;
};


/**
  Class MergedCellsRecord represents MergedCells record, which contains
  a list of all merged cells in the current sheets. Each entry in this list
  define the range of cells that should be merged, namely firstRow, lastRow, 
  firstColumn and lastColumn.
 */
 
class MergedCellsRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new MergedCells record.
   */
  MergedCellsRecord();

  /**
   * Destroys the record.
   */
  virtual ~MergedCellsRecord();
  
  /**
   * Returns the total number of merged cells in the list.
   */
  unsigned count() const;
  
  /**
   * Returns the index to first row in the i-th position in the list.
   */
  unsigned firstRow( unsigned i ) const;
  
  /**
   * Returns the index to last row in the i-th position in the list.
   */
  unsigned lastRow( unsigned i ) const;
  
  /**
   * Returns the index to first column in the i-th position in the list.
   */
  unsigned firstColumn( unsigned i ) const;
  
  /**
   * Returns the index to last column in the i-th position in the list.
   */
  unsigned lastColumn( unsigned i ) const;

  virtual void setData( unsigned size, const unsigned char* data );
  
  virtual const char* name(){ return "MERGEDCELLS"; }

  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   MergedCellsRecord( const MergedCellsRecord& );
   MergedCellsRecord& operator=( const MergedCellsRecord& );

   class Private;
   Private *d;
};

/**
  Class MulBlankRecord represents a cell range containing blank cells. 
  All cells are located in the same row.
  
  \sa BlankRecord
 */
 
class MulBlankRecord : public Record, public CellInfo, public ColumnSpanInfo
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new MulBlank record.
   */
  MulBlankRecord();

  /**
   * Destroys the record.
   */
  virtual ~MulBlankRecord();

  virtual void setData( unsigned size, const unsigned char* data );

  /**
   * Returns XF index of ith column.
   */
  unsigned xfIndex( unsigned i ) const;

  virtual const char* name(){ return "MULBLANK"; }

  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   MulBlankRecord( const MulBlankRecord& );
   MulBlankRecord& operator=( const MulBlankRecord& );

   class Private;
   Private *d;

   // from CellInfo, we don't need it
   // mark as private so nobody can call them
   virtual unsigned column() const { return CellInfo::column(); }
   virtual unsigned xfIndex() const { return CellInfo::xfIndex(); }
};

/**
  Class MulRKRecord represents a cell range containing RK value cells. 
  These cells are located in the same row.
  
  \sa RKRecord
 */

class MulRKRecord : public Record, public CellInfo, public ColumnSpanInfo
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new MulRK record.
   */
  MulRKRecord();

  /**
   * Destroys the record.
   */
  virtual ~MulRKRecord();

  virtual void setData( unsigned size, const unsigned char* data );

  /**
   Returns XF index of ith column.
   */
  unsigned xfIndex( unsigned i ) const;
  
  /**
   * Returns true if the record holds an integer value.
   *
   * \sa asInteger
   */
  bool isInteger( unsigned i ) const;
  
  /**
   * Returns the integer value specified by the record. It is only valid
   * when isInteger returns true.
   *
   * \sa isInteger, asFloat
   */
  int asInteger( unsigned i ) const;
  
  /**
   * Returns the floating-point value specified by the record. It is only valid
   * when isInteger returns false.
   *
   * \sa asInteger
   */
  double asFloat( unsigned i ) const;
  
  unsigned encodedRK( unsigned i ) const;
  
  virtual const char* name(){ return "MULRK"; }

  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   MulRKRecord( const MulRKRecord& );
   MulRKRecord& operator=( const MulRKRecord& );

   class Private;
   Private *d;

   // from CellInfo, we don't need it
   // mark as private so nobody can call them
   virtual unsigned column() const { return CellInfo::column(); }
   virtual unsigned xfIndex() const { return CellInfo::xfIndex(); }
};


class NameRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
    return this->id;
  }

  NameRecord();

  ~NameRecord();

  UString definedName() const;

  void setDefinedName( const UString& name );

  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "NAME"; }

  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   NameRecord( const NameRecord& );
   NameRecord& operator=( const NameRecord& );
   
   class Private;
   Private *d;
};


/**
  Class NumberRecord represents a cell that contains a floating point value. 

 */
class NumberRecord : public Record, public CellInfo
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new Number record.
   */
  NumberRecord();

  /**
   * Destroys the record.
   */
  virtual ~NumberRecord();

  virtual void setData( unsigned size, const unsigned char* data );
  
  /**
   * Returns the floating-point value specified by the record.
   *
   * \sa setNumber
   */
  double number() const;
  
  /**
   * Sets the floating-point value specified by the record.
   *
   * \sa number
   */
  void setNumber( double f );

  virtual const char* name(){ return "NUMBER"; }
  
  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   NumberRecord( const NumberRecord& );
   NumberRecord& operator=( const NumberRecord& );

   class Private;
   Private *d;
};

/**
  Class PaletteRecord lists colors.
  
 */
class PaletteRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new Palette record.
   */
  PaletteRecord();
  
  /**
   * Destroy the record.
   */
  ~PaletteRecord();

  /**
   * Gets the n-th color.
   */
  Color color( unsigned i ) const;

  /**
   * Returns the number of colors in the palette.
   */
  unsigned count() const;
  
  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "PALETTE"; }

  virtual void dump( std::ostream& out ) const;

private:
  // no copy or assign
  PaletteRecord( const PaletteRecord& );
  PaletteRecord& operator=( const PaletteRecord& );

  class Private;
  Private* d;
};



/**
  Class RightMarginRecord holds information about right margin.
  
 */
class RightMarginRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new RightMargin record.
   */
  RightMarginRecord();
  
  /**
   * Destroy the record.
   */
  ~RightMarginRecord();
  
  /**
   * Gets the right margin (in inches).
   */
  double rightMargin() const;
  
  /**
   * Sets the new right margin (in inches).
   */
  void setRightMargin( double m );

  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "RIGHTMARGIN"; }

  virtual void dump( std::ostream& out ) const;

private:
  // no copy or assign
  RightMarginRecord( const RightMarginRecord& );
  RightMarginRecord& operator=( const RightMarginRecord& );

  class Private;
  Private* d;
};



/**
  Class RKRecord represents a cell that contains an RK value, 
  i.e encoded integer or floating-point value.

 */
class RKRecord : public Record, public CellInfo
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new RK record.
   */
  RKRecord();

  /**
   * Destroys the record.
   */
  virtual ~RKRecord();

  virtual void setData( unsigned size, const unsigned char* data );
  
  /**
   * Returns true if the record holds an integer value.
   *
   * \sa asInteger, isFloat
   */
  bool isInteger() const;
  
  /**
   * Returns true if the record holds a floating-point value.
   *
   * \sa asFloat, isInteger
   */
  bool isFloat() const;
    
  /**
   * Returns the integer value specified by the record. It is only valid
   * when isInteger returns true.
   *
   * \sa isInteger, asFloat
   */
  int asInteger() const;
  
  /**
   * Returns the floating-point value specified by the record. It is only valid
   * when isFloat returns true.
   *
   * \sa isFloat, asInteger
   */
  double asFloat() const;
  
  /**
   * Sets the integer value to be specified by the record. 
   *
   * \sa setFloat
   */
  void setInteger( int i );
  
  /**
   * Sets the floating-point value to be specified by the record. 
   *
   * \sa setFloat
   */
  void setFloat( double f );

  unsigned encodedRK() const;
  
  virtual const char* name(){ return "RK"; }
  
  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   RKRecord( const RKRecord& );
   RKRecord& operator=( const RKRecord& );

   class Private;
   Private *d;
};

/**
  Class RowRecord represents Row record, which provides information
  (such as row height and formatting) for a span of columns.
 */
class RowRecord : public Record, public ColumnSpanInfo
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new Row record.
   */
  RowRecord();

  /**
   * Destroys the record.
   */
  virtual ~RowRecord();
  
  /**
   * Returns the index of the row.
   *
   * \sa setRow
   */
  unsigned row() const;
  
  /**
   * Sets the index of the row.
   *
   * \sa row
   */
  void setRow( unsigned r );
    
  /**
   * Returns the XF index for the formatting of the cells.
   *
   * \sa setXfIndex
   */
  unsigned xfIndex() const;
  
  /**
   * Sets the XF index for the formatting of the cells.
   *
   * \sa xfIndex
   */
  void setXfIndex( unsigned i );
  
  /**
   * Returns the height of the row, specified in twips (1/20 pt).
   *
   * \sa setHeight
   */
  unsigned height() const;
  
  /**
   * Sets the height of the row, specified in twips (1/20 pt).
   *
   * \sa height
   */
  void setHeight( unsigned h );
  
  /**
   * Returns true if the row should be hidden, i.e not visible.
   *
   * \sa setHidden
   */
  bool hidden() const;
  
  /**
   * Sets whether row should be hidden or visible.
   *
   * \sa hidden
   */
  void setHidden( bool h );

  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "ROW"; }

  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   RowRecord( const RowRecord& );
   RowRecord& operator=( const RowRecord& );

   class Private;
   Private *d;
};


/**
  Class RStringRecord represents a cell that contains rich-text string.
   
  In Excel 97 and later version, it is replaced by LabelSSTRecord. However,
  Excel 97 is still able to load RStringRecord.

  \sa LabelRecord
  \sa LabelSSTRecord

 */
class RStringRecord : public Record, public CellInfo
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new Label record.
   */
  RStringRecord();

  /**
   * Destroys the record.
   */
  virtual ~RStringRecord();

  /**
   * Returns the label string.
   *
   * \sa setLabel
   */
  UString label() const;
  
  /**
   * Sets the label string.
   *
   * \sa label
   */
  void setLabel( const UString& l );

  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "RSTRING"; }
  
  virtual void dump( std::ostream& out ) const;

private:
   // no copy or assign
   RStringRecord( const RStringRecord& );
   RStringRecord& operator=( const RStringRecord& );

   class Private;
   Private *d;
};


/**
  Class SSTRecord represents SST record, which holds the shared string
  table of the workbook.

  \sa LabelSSTRecord
  
 */
class SSTRecord : public Record
{
public:

  static const unsigned int id;
  
  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new SST record.
   */ 
  SSTRecord();
  
  /**
   * Destroys the record.
   */ 
  virtual ~SSTRecord();

  virtual void setData( unsigned size, const unsigned char* data );
  
  /**
    Returns the number of available string in this string table.
   */  
  unsigned count() const;
  
  /**
    Returns the string at specified index. 
    Note that index must be less than count().
    If index is not valid, this will return UString::null.    
   */  
  UString stringAt( unsigned index ) const;
  
  virtual const char* name(){ return "SST"; }
  
  virtual void dump( std::ostream& out ) const;
  
private:
   // no copy or assign
   SSTRecord( const SSTRecord& );
   SSTRecord& operator=( const SSTRecord& );

   class Private;
   Private *d;
};


/**
  Class String represents a string record, which holds the value of 
  calculation if a formula returns a string. This record must appear 
  right after the associated formula record.

  \sa FormulaRecord
  
 */
class StringRecord : public Record
{
public:

  static const unsigned int id;
  
  unsigned int rtti(){ return this->id; }

  /**
   * Creates a new string record.
   */ 
  StringRecord();
  
  /**
   * Destroys the record.
   */ 
  virtual ~StringRecord();

  virtual void setData( unsigned size, const unsigned char* data );
  
  /**
    Returns the string (in Unicode).
   */  
  UString ustring() const;
  
  /**
    Returns the string as a value.
   */  
  Value value() const;
  
  virtual const char* name(){ return "STRING"; }
  
  virtual void dump( std::ostream& out ) const;
  
private:
   // no copy or assign
   StringRecord( const SSTRecord& );
   StringRecord& operator=( const SSTRecord& );

   class Private;
   Private *d;
};

/**
  Class SupbookRecord stores references to external workbook.
  
 */
class SupbookRecord : public Record
{
public:

  static const unsigned int id;

  typedef enum 
  {
	  UnknownRef,
	  ExternalRef,
	  InternalRef,
	  AddInRef,
	  ObjectLink
  } ReferenceType;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new Supbook record.
   */
  SupbookRecord();

  /**
   * Destroy the record.
   */
  ~SupbookRecord();

  /**
   * Returns the type of the reference.
   */
  ReferenceType referenceType() const;

  /**
   * Sets the type of the reference.
   */
  void setReferenceType(ReferenceType type);

  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "SUPBOOK"; }

  virtual void dump( std::ostream& out ) const;

private:
  // no copy or assign
  SupbookRecord( const SupbookRecord& );
  SupbookRecord& operator=( const SupbookRecord& );

  class Private;
  Private* d;
};

/**
  Class TopMarginRecord holds information about top margin.
  
 */
class TopMarginRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new TopMargin record.
   */
  TopMarginRecord();
  
  /**
   * Destroy the record.
   */
  ~TopMarginRecord();
  
  /**
   * Gets the top margin (in inches).
   */
  double topMargin() const;
  
  /**
   * Sets the new top margin (in inches).
   */
  void setTopMargin( double m );

  virtual void setData( unsigned size, const unsigned char* data );

  virtual const char* name(){ return "TOPMARGIN"; }

  virtual void dump( std::ostream& out ) const;

private:
  // no copy or assign
  TopMarginRecord( const TopMarginRecord& );
  TopMarginRecord& operator=( const TopMarginRecord& );

  class Private;
  Private* d;
};

/**
  Class XFRecord holds information of XF (eXtended Format) which specifies
  many kind of properties of a specific cell. It will be referred 
  by record derived from CellInfo, in the member function xfIndex().
        
 */
class XFRecord : public Record
{
public:

  static const unsigned int id;

  unsigned int rtti(){
	  return this->id;
  }

  /**
   * Creates a new XF record.
   */
  XFRecord();
  
  /**
   * Creates a copy of XF record.
   */
  XFRecord( const XFRecord& xf );
  
  /**
   * Assigns from another XF record.
   */
  XFRecord& operator=( const XFRecord& xf );
  
  /**
   * Destroy the record.
   */
  ~XFRecord();
  
  /**
   * Gets the index of the font for use in this XFormat. The index
   * refers to the font table.
   *
   * \sa setFontIndex, FontRecord
   */
  unsigned fontIndex() const;
  
  /**
   * Sets the index of the font for use in this XFormat. The index
   * refers to the font table.
   *
   * \sa fontIndex, FontRecord
   */
  void setFontIndex( unsigned fi );

  /**
   * Gets the index of the number format for use in this XFormat. The index
   * refers to the format table.
   *
   * \sa setFormatIndex, FormatRecord
   */
  unsigned formatIndex() const;
  
  /**
   * Sets the index of the number format for use in this XFormat. The index
   * refers to the format table.
   *
   * \sa formatIndex, FormatRecord
   */
  void setFormatIndex( unsigned fi );
  
  /**
   * Returns true if the cells using this format should be locked.
   *
   * \sa setLocked
   */  
  bool locked() const;
  
  /**
   * Sets whether the cells using this format should be locked or not.
   *
   * \sa locked
   */  
  void setLocked( bool l );
  
  /**
   * Returns true if the formula of the cells using this format 
   * should be hidden from user.
   *
   * \sa setFormulaHidden
   */  
  bool formulaHidden() const;

  /**
   * Sets whether the formula of the cells using this format 
   * should be hidden or should be visible.
   *
   * \sa formulaHidden
   */  
  void setFormulaHidden( bool f );
  
  /**
   * Returns the index of the parent stlye of this format.
   * This refers to the index of the XFormat table which is constructed 
   * from a series of XFormat records.
   *
   * \sa setParentStyle
   */  
  unsigned parentStyle() const;
  
  /**
   * Sets the index of the parent stlye of this format.
   * This refers to the index of the XFormat table which is constructed 
   * from a series of XFormat records.
   *
   * \sa parentStyle
   */  
  void setParentStyle( unsigned ps );

  enum { 
    General = 0, 
    Left, 
    Centered, 
    Right, 
    Filled, 
    Justified,
    CenteredSelection,
    Distributed };
  
  /**
   * Gets the horizontal alignment, e.g Left.
   */
  unsigned horizontalAlignment() const;
  
  /**
   * Sets the horizontal alignment, e.g Left.
   */
  void setHorizontalAlignment( unsigned ha );
  
  /**
   * Returns human-readable string representation of the horizontal alignment.
     For example, XFRecord::Left will return "Left".
   */
  const char* horizontalAlignmentAsString() const; 
  
  enum { 
    Top = 0, 
    VCentered = 1, 
    Bottom = 2, 
    VJustified = 3, 
    VDistributed = 4 };
  
  /**
   * Gets the vertical alignment, e.g Bottom.
   *
   * \sa setVerticalAlignment
   */
  unsigned verticalAlignment() const;
  
  /**
   * Sets the vertical alignment, e.g Top.
   *
   * \sa verticalAlignment
   */
  void setVerticalAlignment( unsigned va );
  
  /**
   * Returns human-readable string representation of the vertical alignment.
     For example, XFRecord::Top will return "Top".
   */
  const char* verticalAlignmentAsString() const; 
  
  /**
   * Returns true if text is wrapped at right border.
   *
   * \sa setTextWrap
   */
  bool textWrap() const;
  
  /**
   * Sets whether text should be wrapped at right border.
   *
   * \sa textWrap
   */
  void setTextWrap( bool wrap );
  
  /**
   * Returns the rotation angle of the text. If it is between 1 to 90,
   * the text is rotated 1 to 90 degrees counterclockwise. If it is between
   * 91 to 180, the text is rotated 1 to 90 degrees clockwise.
   *
   * \sa setRotationAngle
   */
  unsigned rotationAngle() const;
  
  /**
   * Sets the rotation angle of the text. If it is between 1 to 90,
   * the text is rotated 1 to 90 degrees counterclockwise. If it is between
   * 91 to 180, the text is rotated 1 to 90 degrees clockwise.
   *
   * \sa setRotationAngle
   */
  void setRotationAngle( unsigned angle );
  
  /**
   * Returns true if the letters for text are not rotated, but
   * instead stacked top-to-bottom.
   *
   * \sa setStackedLetters
   */
  bool stackedLetters() const;
  
  /**
   * Sets whether the letters for text should be stacked top-to-bottom.
   *
   * \sa stackedLetters
   */
  void setStackedLetters( bool stacked );

  /**
   * Returns indent level.
   *
   * \sa indentLevel
   */
  unsigned indentLevel() const;
  
  /**
   * Sets indent level.
   *
   * \sa indentLevel
   */
  void setIndentLevel( unsigned i );
  
  /**
   * Returns true if content should be shrunk to fit into cell.
   *
   * \sa setShrinkContent
   */
  bool shrinkContent() const;
  
  /**
   * Sets whether content should be shrunk to fit into cell.
   *
   * \sa shrinkContent
   */
  void setShrinkContent( bool s );
  
  enum
  {
    NoLine = 0,
    Thin = 1,
    Medium = 2,
    Dashed = 3,
    Dotted = 4,
    Thick = 5,
    Double = 6,
    Hair = 7,
    MediumDashed = 8,
    ThinDashDotted = 9,
    MediumDashDotted = 10,
    ThinDashDotDotted = 11,
    MediumDashDotDotted = 12,
    SlantedMediumDashDotted = 13    
  };
  
  /**
   * Returns the line style for left border.
   *
   * \sa setLeftBorderStyle, leftBorderColor
   */
  unsigned leftBorderStyle() const;
  
  /**
   * Sets the line style for left border.
   *
   * \sa leftBorderStyle, setLeftBorderColor
   */
  void setLeftBorderStyle( unsigned style );
  
  /**
   * Returns the color for left border. This is an index to color palette
   * specified in Palette record.
   *
   * \sa setLeftBorderColor, leftBorderStyle
   */
  unsigned leftBorderColor() const;
  
  /**
   * Sets the color for left border. This is an index to color palette
   * specified in Palette record.
   *
   * \sa leftBorderColor, setLeftBorderStyle
   */
  void setLeftBorderColor( unsigned color );

  /**
   * Returns the line style for right border.
   *
   * \sa setRightBorderStyle, rightBorderColor
   */
  unsigned rightBorderStyle() const;
  
  /**
   * Sets the line style for right border.
   *
   * \sa rightBorderStyle, setRightBorderColor
   */
  void setRightBorderStyle( unsigned style );
  
  /**
   * Returns the color for right border. This is an index to color palette
   * specified in Palette record.
   *
   * \sa setRightBorderColor, rightBorderStyle
   */
  unsigned rightBorderColor() const;
  
  /**
   * Sets the color for right border. This is an index to color palette
   * specified in Palette record.
   *
   * \sa rightBorderColor, setRightBorderStyle
   */
  void setRightBorderColor( unsigned color );
  
  /**
   * Returns the line style for top border.
   *
   * \sa setTopBorderStyle, topBorderColor
   */
  unsigned topBorderStyle() const;
  
  /**
   * Sets the line style for top border.
   *
   * \sa topBorderStyle, setTopBorderColor
   */
  void setTopBorderStyle( unsigned style );
  
  /**
   * Returns the color for top border. This is an index to color palette
   * specified in Palette record.
   *
   * \sa setTopBorderColor, topBorderStyle
   */
  unsigned topBorderColor() const;
  
  /**
   * Sets the color for top border. This is an index to color palette
   * specified in Palette record.
   *
   * \sa topBorderColor, setTopBorderStyle
   */
  void setTopBorderColor( unsigned color );
  
  /**
   * Returns the line style for bottom border.
   *
   * \sa setBottomBorderStyle, bottomBorderColor
   */
  unsigned bottomBorderStyle() const;
  
  /**
   * Sets the line style for bottom border.
   *
   * \sa bottomBorderStyle, setBottomBorderColor
   */
  void setBottomBorderStyle( unsigned style );
  
  /**
   * Returns the color for bottom border. This is an index to color palette
   * specified in Palette record.
   *
   * \sa setBottomBorderColor, bottomBorderStyle
   */
  unsigned bottomBorderColor() const;
  
  /**
   * Sets the color for bottom border. This is an index to color palette
   * specified in Palette record.
   *
   * \sa bottomBorderColor, setBottomBorderStyle
   */
  void setBottomBorderColor( unsigned color );
  
  /**
   * Returns true if there is a diagonal line from top left to right bottom.
   *
   * \sa diagonalStyle, diagonalColor, setDiagonalTopLeft
   */
  bool diagonalTopLeft() const;
  
  /**
   * Sets whether there should be a diagonal line from top left to right bottom.
   *
   * \sa diagonalTopLeft, setDiagonalStlye, setDiagonalColor
   */
  void setDiagonalTopLeft( bool d );
  
  /**
   * Returns true if there is a diagonal line from bottom left to right top.
   *
   * \sa diagonalStyle, diagonalColor, setDiagonalBottomLeft
   */
  bool diagonalBottomLeft() const;
  
  /**
   * Sets whether there should be a diagonal line from bottom left to right top.
   *
   * \sa diagonalBottomLeft, setDiagonalStlye, setDiagonalColor
   */
  void setDiagonalBottomLeft( bool d );
  
  /**
   * Returns the diagonal line style.
   *
   * \sa diagonalTopLeft, diagonalBottomLeft, setDiagonalStyle
   */
  unsigned diagonalStyle() const;
  
  /**
   * Sets the line style for diagonal line.
   *
   * \sa diagonalBorderStyle, setDiagonalTopLeft, setDiagonalBottomLeft
   */
  void setDiagonalStyle( unsigned style );
  
  /**
   * Returns the color for diagonal line. This is an index to color palette
   * specified in Palette record.
   *
   * \sa setDiagonalColor, diagonalStyle
   */
  unsigned diagonalColor() const;
  
  /**
   * Sets the color for diagonal line. This is an index to color palette
   * specified in Palette record.
   *
   * \sa diagonalColor, setDiagonalStyle
   */
  void setDiagonalColor( unsigned color );
  
  /**
   * Returns fill pattern.
   *
   * \sa setFillPattern
   */
  unsigned fillPattern() const;
  
  /**
   * Sets fill pattern.
   *
   * \sa fillPattern
   */
  void setFillPattern( unsigned pattern );
  
  /**
   * Returns the fill foreground color. This is an index to color palette
   * specified in Palette record.
   *
   * \sa setPatternForeColor, patternBackColor
   */
  unsigned patternForeColor() const;
  
  /**
   * Sets the fill foreground color. This is an index to color palette
   * specified in Palette record.
   *
   * \sa patternForeColor, setPatternBackColor
   */
  void setPatternForeColor( unsigned color );
  
  /**
   * Returns the fill background color. This is an index to color palette
   * specified in Palette record.
   *
   * \sa setPatternBackColor, patternForeColor
   */
  unsigned patternBackColor() const;    
  
  /**
   * Sets the fill background color. This is an index to color palette
   * specified in Palette record.
   *
   * \sa patternBackColor, setPatternForeColor
   */
  void setPatternBackColor( unsigned color );
  
  virtual const char* name(){ return "XF"; }
  
  virtual void setData( unsigned size, const unsigned char* data );
  
  virtual void dump( std::ostream& out ) const;

private:
  class Private;
  Private* d;
};


class ExcelReader
{
public:
  ExcelReader();
  virtual ~ExcelReader();
  bool load( Workbook* workbook, const char* filename );
  
protected:  
  virtual void handleRecord( Record* record );
    
private:  
  void handleBoundSheet( BoundSheetRecord* record );
  void handleBOF( BOFRecord* record );
  void handleBoolErr( BoolErrRecord* record );
  void handleBottomMargin( BottomMarginRecord* record );
  void handleBlank( BlankRecord* record );
  void handleCalcMode( CalcModeRecord* record );
  void handleColInfo( ColInfoRecord* record );
  void handleDateMode( DateModeRecord* record );
  void handleDimension( DimensionRecord* record );
  void handleExternName( ExternNameRecord* record );
  void handleExternSheet( ExternSheetRecord* record );
  void handleFilepass( FilepassRecord* record );
  void handleFormat( FormatRecord* record );
  void handleFormula( FormulaRecord* record );
  void handleFont( FontRecord* record );
  void handleFooter( FooterRecord* record );
  void handleHeader( HeaderRecord* record );
  void handleLabel( LabelRecord* record );
  void handleLabelSST( LabelSSTRecord* record );
  void handleLeftMargin( LeftMarginRecord* record );
  void handleMergedCells( MergedCellsRecord* record );
  void handleMulBlank( MulBlankRecord* record );
  void handleMulRK( MulRKRecord* record );
  void handleName( NameRecord* record );
  void handleNumber( NumberRecord* record );
  void handlePalette( PaletteRecord* record );
  void handleRightMargin( RightMarginRecord* record );
  void handleRString( RStringRecord* record );
  void handleRK( RKRecord* record );
  void handleRow( RowRecord* record );
  void handleSST( SSTRecord* record );
  void handleString( StringRecord* record );
  void handleSupbook( SupbookRecord* record );
  void handleTopMargin( TopMarginRecord* record );
  void handleXF( XFRecord* record );    
  
  Color convertColor( unsigned colorIndex );
  FormatFont convertFont( unsigned fontIndex );
  Format convertFormat( unsigned xfIndex );
  UString decodeFormula( unsigned row, unsigned col, 
    const FormulaTokens& tokens, bool openDocumentFormat=false );
  
  // no copy or assign
  ExcelReader( const ExcelReader& );
  ExcelReader& operator=( const ExcelReader& );
  
  class Private;
  Private* d;
};


} // namespace Swinder



#endif // SWINDER_EXCEL_H
