/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2009,2010 Sebastian Sauer <sebsauer@kdab.com>

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
#include "formulas.h"
#include "records.h"

// libmso
//#include <generated/simpleParser.h>
//#include <generated/leinputstream.h>
#include "generated/simpleParser.h"
#include "generated/leinputstream.h"
#include "drawstyle.h"
#include "ODrawToOdf.h"
#include "pictures.h"

class QTextDocument;

namespace Swinder
{

class Object;

// rich-text, unicode, far-east support Excel string

class EString
{
public:

    EString();

    EString(const EString&);

    EString& operator=(const EString&);

    ~EString();

    bool unicode() const;

    void setUnicode(bool u);

    bool richText() const;

    void setRichText(bool r);

    QString str() const;

    void setStr(const QString& str);

    std::map<unsigned, unsigned> formatRuns() const;

    void setFormatRuns(const std::map<unsigned, unsigned>& formatRuns);

    // space allocate for the string, not length (use string.length() for that)
    unsigned size() const;
    void setSize(unsigned size);   // HACKS


    static EString fromUnicodeString(const void* p, bool longString, unsigned maxsize = 0, const unsigned *continuePositions = 0, unsigned continuePositionsOffset = 0);

    static EString fromSheetName(const void* p, unsigned maxsize = 0);

    // from the buffer
    // longstring means 16-bit string length, usually for label
    // longstring=false is normally for sheet name
    static EString fromByteString(const void* p, bool longString, unsigned maxsize = 0);

private:
    class Private;
    Private* d;
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
    virtual void setRow(unsigned r);

    /**
     * Sets the column associated with the cell information. It is zero based,
     * so the first column is 0.
     *
     * \sa setRow, column
     */
    virtual void setColumn(unsigned c);

    /**
     * Sets the XF index for formatting of the cell.
     *
     * \sa xfIndex
     */
    virtual void setXfIndex(unsigned i);

private:
    // no copy or assign
    CellInfo(const CellInfo&);
    CellInfo& operator=(const CellInfo&);

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
    virtual void setFirstColumn(unsigned c);

    /**
     * Sets the last column associated with the span information.
     * Column index is zero based, so the first column is 0.
     *
     * \sa setFirstColumn, lastColumn
     */
    virtual void setLastColumn(unsigned c);

private:
    // no copy or assign
    ColumnSpanInfo(const ColumnSpanInfo&);
    ColumnSpanInfo& operator=(const ColumnSpanInfo&);

    class Private;
    Private* spaninfo;
};

/**
  \brief External book record
 */
class ExternBookRecord : public Record
{
public:

    static const unsigned int id;

    unsigned int rtti() const override {
        return this->id;
    }

    explicit ExternBookRecord(Workbook *book);

    ~ExternBookRecord() override;

    unsigned sheetCount() const;

    QString bookName() const;

    void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions) override;



    const char* name() const override {
        return "EXTERNBOOK";
    }

    void dump(std::ostream& out) const override;

private:
    // no copy or assign
    ExternBookRecord(const ExternBookRecord&);
    ExternBookRecord& operator=(const ExternBookRecord&);

    class Private;
    Private *d;
};

class ExternNameRecord : public Record
{
public:

    static const unsigned int id;

    unsigned int rtti() const override {
        return this->id;
    }

    explicit ExternNameRecord(Workbook *book);

    ~ExternNameRecord() override;

    // one-based sheet index
    // if 0 means AddIn function
    unsigned sheetIndex() const;

    void setSheetIndex(unsigned sheetIndex);

    QString externName() const;

    void setExternName(const QString& name);

    void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions) override;



    const char* name() const override {
        return "EXTERNNAME";
    }

    void dump(std::ostream& out) const override;

private:
    // no copy or assign
    ExternNameRecord(const ExternNameRecord&);
    ExternNameRecord& operator=(const ExternNameRecord&);

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

    unsigned int rtti() const override {
        return this->id;
    }

    /**
     * Creates a new formula record.
     */
    explicit FormulaRecord(Workbook *book);

    /**
     * Destroy the record.
     */
    ~FormulaRecord() override;

    /**
     * Gets the result of the formula.
     */
    Value result() const;

    /**
     * Sets the result of the formula.
     */
    void setResult(const Value& v);

    FormulaTokens tokens() const;
    void addToken(const FormulaToken& token);

    /**
     * Returns true if this formula is a share formula.
     */
    bool isShared() const;

    void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions) override;
    void writeData(XlsRecordOutputStream &out) const override;

    const char* name() const override {
        return "FORMULA";
    }

    void dump(std::ostream& out) const override;

private:
    // no copy or assign
    FormulaRecord(const FormulaRecord&);
    FormulaRecord& operator=(const FormulaRecord&);

    class Private;
    Private* d;
};


/**
  \brief Shared Formula.

  Information about a shared formula, the formula itself and its range.
 */
class SharedFormulaRecord : public Record
{
public:
    static const unsigned int id;

    unsigned int rtti() const override {
        return this->id;
    }

    /**
     * Creates a new shared formula record.
     */
    explicit SharedFormulaRecord(Workbook *book);

    /**
     * Destroy the record.
     */
    ~SharedFormulaRecord() override;

    FormulaTokens tokens() const;

    void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions) override;

    const char* name() const override {
        return "SHAREDFMLA";
    }

    void dump(std::ostream& out) const override;

private:
    // no copy or assign
    SharedFormulaRecord(const SharedFormulaRecord&);
    SharedFormulaRecord& operator=(const SharedFormulaRecord&);

    class Private;
    Private* d;
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

    unsigned int rtti() const override {
        return this->id;
    }

    /**
     * Creates a new MulRK record.
     */
    explicit MulRKRecord(Workbook *book);

    /**
     * Destroys the record.
     */
    ~MulRKRecord() override;

    void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions) override;

    /**
     Returns XF index of ith column.
     */
    unsigned xfIndex(unsigned i) const;

    /**
     * Returns true if the record holds an integer value.
     *
     * \sa asInteger
     */
    bool isInteger(unsigned i) const;

    /**
     * Returns the integer value specified by the record. It is only valid
     * when isInteger returns true.
     *
     * \sa isInteger, asFloat
     */
    int asInteger(unsigned i) const;

    /**
     * Returns the floating-point value specified by the record. It is only valid
     * when isInteger returns false.
     *
     * \sa asInteger
     */
    double asFloat(unsigned i) const;

    unsigned encodedRK(unsigned i) const;

    const char* name() const override {
        return "MULRK";
    }

    void dump(std::ostream& out) const override;

private:
    // no copy or assign
    MulRKRecord(const MulRKRecord&);
    MulRKRecord& operator=(const MulRKRecord&);

    class Private;
    Private *d;

    // from CellInfo, we don't need it
    // mark as private so nobody can call them
    unsigned column() const override {
        return CellInfo::column();
    }
    unsigned xfIndex() const override {
        return CellInfo::xfIndex();
    }
};


class NameRecord : public Record
{
public:
    FormulaToken m_formula;

    static const unsigned int id;

    unsigned int rtti() const override {
        return this->id;
    }

    explicit NameRecord(Workbook *book);

    ~NameRecord() override;

    QString definedName() const;

    void setDefinedName(const QString& name);

    unsigned sheetIndex() const;

    bool isBuiltin() const;

    void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions) override;

    const char* name() const override {
        return "NAME";
    }

    void dump(std::ostream& out) const override;

private:
    // no copy or assign
    NameRecord(const NameRecord&);
    NameRecord& operator=(const NameRecord&);

    class Private;
    Private *d;
};

/**
  Class RKRecord represents a cell that contains an RK value,
  i.e encoded integer or floating-point value.

 */
class RKRecord : public Record, public CellInfo
{
public:

    static const unsigned int id;

    unsigned int rtti() const override {
        return this->id;
    }

    /**
     * Creates a new RK record.
     */
    explicit RKRecord(Workbook *book);

    /**
     * Destroys the record.
     */
    ~RKRecord() override;

    void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions) override;

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
    void setInteger(int i);

    /**
     * Sets the floating-point value to be specified by the record.
     *
     * \sa setFloat
     */
    void setFloat(double f);

    unsigned encodedRK() const;

    const char* name() const override {
        return "RK";
    }

    void dump(std::ostream& out) const override;

private:
    // no copy or assign
    RKRecord(const RKRecord&);
    RKRecord& operator=(const RKRecord&);

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

    unsigned int rtti() const override {
        return this->id;
    }

    /**
     * Creates a new Label record.
     */
    explicit RStringRecord(Workbook *book);

    /**
     * Destroys the record.
     */
    ~RStringRecord() override;

    /**
     * Returns the label string.
     *
     * \sa setLabel
     */
    QString label() const;

    /**
     * Sets the label string.
     *
     * \sa label
     */
    void setLabel(const QString& l);

    void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions) override;

    const char* name() const override {
        return "RSTRING";
    }

    void dump(std::ostream& out) const override;

private:
    // no copy or assign
    RStringRecord(const RStringRecord&);
    RStringRecord& operator=(const RStringRecord&);

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

    unsigned int rtti() const override {
        return this->id;
    }

    /**
     * Creates a new SST record.
     */
    explicit SSTRecord(Workbook *book);

    /**
     * Destroys the record.
     */
    ~SSTRecord() override;

    void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions) override;
    void writeData(XlsRecordOutputStream& out) const override;

    /**
      Returns the number of available string in this string table.
     */
    unsigned count() const;

    unsigned useCount() const;
    void setUseCount(unsigned count);

    void setExtSSTRecord(ExtSSTRecord* esst);

    /**
      Returns the string at specified index.
      Note that index must be less than count().
      If index is not valid, this will return an empty QString.
     */
    QString stringAt(unsigned index) const;

    /**
      Returns format runs for the string at specified index.
      Format runs are a mapping from character-index -> font index.
     */
    std::map<unsigned, unsigned> formatRunsAt(unsigned index) const;

    unsigned addString(const QString& string);

    const char* name() const override {
        return "SST";
    }

    void dump(std::ostream& out) const override;

private:
    // no copy or assign
    SSTRecord(const SSTRecord&);
    SSTRecord& operator=(const SSTRecord&);

    class Private;
    Private *d;
};

class ObjRecord : public Record
{
public:
    Object *m_object;
    static const unsigned id;
    explicit ObjRecord(Workbook *book);
    ~ObjRecord() override;
    unsigned rtti() const override {
        return this->id;
    }
    const char* name() const override {
        return "Obj";
    }
    void dump(std::ostream&) const override;
    void setData(unsigned size, const unsigned char* data, const unsigned* continuePositions) override;
};

class TxORecord : public Record
{
public:
    enum HorizontalAlignment {
        Left = 1,
        Centered = 2,
        Right = 3,
        Justified = 4,
        Distributed = 7
    };
    enum VerticalAlignment {
        Top = 1,
        VCentered = 2,
        Bottom = 3,
        VJustified = 4,
        VDistributed = 7
    };

    static const unsigned id;
    explicit TxORecord(Workbook *book=0);
    // allowing copies for the hack for text support in shapes
    TxORecord(const TxORecord&);
    ~TxORecord() override;
    TxORecord& operator=(const TxORecord&);
    unsigned rtti() const override {
        return this->id;
    }
    const char* name() const override {
        return "TxO";
    }
    void dump(std::ostream&) const override;
    void setData(unsigned size, const unsigned char* data, const unsigned* continuePositions) override;
    const QString &text() const;
    TxORecord::HorizontalAlignment hAlign() const;
    TxORecord::VerticalAlignment vAlign() const;
    const QTextDocument *richText() const;
private:
    class Private;
    Private *d;
};

class MsoDrawingRecord : public Record
{
public:
    static const unsigned id;
    explicit MsoDrawingRecord(Workbook *book);
    ~MsoDrawingRecord() override;

    unsigned rtti() const override {
        return this->id;
    }
    const char* name() const override {
        return "MsoDrawing";
    }
    void dump(std::ostream&) const override;
    void setData(unsigned size, const unsigned char* data, const unsigned* continuePositions) override;

    const MSO::OfficeArtDgContainer& dgContainer() const;
private:
    // no copy or assign
    MsoDrawingRecord(const MsoDrawingRecord&);
    MsoDrawingRecord& operator=(const MsoDrawingRecord&);

    class Private;
    Private *d;
};


class MsoDrawingGroupRecord : public Record
{
public:
    static const unsigned id;
    explicit MsoDrawingGroupRecord(Workbook *book);
    ~MsoDrawingGroupRecord() override;
    unsigned rtti() const override {
        return this->id;
    }
    const char* name() const override {
        return "MsoDrawingGroup";
    }
    void dump(std::ostream&) const override;
    void setData(unsigned size, const unsigned char* data, const unsigned* continuePositions) override;

    const QMap<QByteArray,QString> pictureNames() const;
    const MSO::OfficeArtDggContainer& dggContainer() const;
private:
    // no copy or assign
    MsoDrawingGroupRecord(const MsoDrawingGroupRecord&);
    MsoDrawingGroupRecord& operator=(const MsoDrawingGroupRecord&);

    class Private;
    Private *d;
};

class ExcelReader
{
public:
    ExcelReader();
    virtual ~ExcelReader();
    bool load(Workbook* workbook, const char* filename);

protected:
    virtual void handleRecord(Record* record);

private:
    void handleBOF(BOFRecord* record);
    void handleEOF(EOFRecord* record);

    // no copy or assign
    ExcelReader(const ExcelReader&);
    ExcelReader& operator=(const ExcelReader&);

    class Private;
    Private* d;
};

class BkHimRecord : public Record
{
public:
    static const unsigned id;

    unsigned rtti() const override { return this->id; }

    explicit BkHimRecord(Workbook *book);
    ~BkHimRecord() override;

    BkHimRecord( const BkHimRecord& record );
    BkHimRecord& operator=( const BkHimRecord& record );

    enum Format {
        WindowsBitMap = 0x0009,
        NativeFormat = 0x000e
    };

    static QString formatToString(Format format);

    Format format() const;
    void setFormat( Format format );

    QString imagePath() const;
    void setImagePath( const QString &imagePath );

    unsigned imageSize() const;
    void setImageSize( unsigned imageSize );

    void setData( unsigned size, const unsigned char* data, const unsigned* continuePositions ) override;

    const char* name() const override { return "BkHim"; }

    void dump( std::ostream& out ) const override;

private:
    class Private;
    Private * const d;
};

} // namespace Swinder



#endif // SWINDER_EXCEL_H
