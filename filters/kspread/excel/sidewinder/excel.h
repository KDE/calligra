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
#include "objects.h"

// libmso
//#include <generated/simpleParser.h>
//#include <generated/leinputstream.h>
#include "generated/simpleParser.h"
#include "generated/leinputstream.h"
#include "drawstyle.h"
#include "ODrawToOdf.h"
#include "pictures.h"

namespace Swinder
{
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

    unsigned int rtti() const {
        return this->id;
    }

    /**
      Supported BOF type.
    */
    enum { UnknownType = 0, Workbook, Worksheet, Chart, VBModule, MacroSheet, Workspace };

    /**
     * Creates a new BOF record.
     */
    BOFRecord(Swinder::Workbook *book);

    /**
      Destroys the record.
    */
    virtual ~BOFRecord();

    virtual void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions);

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

    virtual const char* name() const {
        return "BOF";
    }

    virtual void dump(std::ostream& out) const;

private:
    // no copy or assign
    BOFRecord(const BOFRecord&);
    BOFRecord& operator=(const BOFRecord&);

    class Private;
    Private *d;
};

/**
  \brief External book record
 */
class ExternBookRecord : public Record
{
public:

    static const unsigned int id;

    unsigned int rtti() const {
        return this->id;
    }

    ExternBookRecord(Workbook *book);

    ~ExternBookRecord();

    unsigned sheetCount() const;

    QString bookName() const;

    virtual void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions);



    virtual const char* name() const {
        return "EXTERNBOOK";
    }

    virtual void dump(std::ostream& out) const;

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

    unsigned int rtti() const {
        return this->id;
    }

    ExternNameRecord(Workbook *book);

    ~ExternNameRecord();

    // one-based sheet index
    // if 0 means AddIn function
    unsigned sheetIndex() const;

    void setSheetIndex(unsigned sheetIndex);

    QString externName() const;

    void setExternName(const QString& name);

    virtual void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions);



    virtual const char* name() const {
        return "EXTERNNAME";
    }

    virtual void dump(std::ostream& out) const;

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

    unsigned int rtti() const {
        return this->id;
    }

    /**
     * Creates a new formula record.
     */
    FormulaRecord(Workbook *book);

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
    void setResult(const Value& v);

    FormulaTokens tokens() const;

    /**
     * Returns true if this formula is a share formula.
     */
    bool isShared() const;

    virtual void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions);

    virtual const char* name() const {
        return "FORMULA";
    }

    virtual void dump(std::ostream& out) const;

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

    unsigned int rtti() const {
        return this->id;
    }

    /**
     * Creates a new shared formula record.
     */
    SharedFormulaRecord(Workbook *book);

    /**
     * Destroy the record.
     */
    ~SharedFormulaRecord();

    FormulaTokens tokens() const;

    virtual void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions);

    virtual const char* name() const {
        return "SHAREDFMLA";
    }

    virtual void dump(std::ostream& out) const;

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

    unsigned int rtti() const {
        return this->id;
    }

    /**
     * Creates a new MulRK record.
     */
    MulRKRecord(Workbook *book);

    /**
     * Destroys the record.
     */
    virtual ~MulRKRecord();

    virtual void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions);

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

    virtual const char* name() const {
        return "MULRK";
    }

    virtual void dump(std::ostream& out) const;

private:
    // no copy or assign
    MulRKRecord(const MulRKRecord&);
    MulRKRecord& operator=(const MulRKRecord&);

    class Private;
    Private *d;

    // from CellInfo, we don't need it
    // mark as private so nobody can call them
    virtual unsigned column() const {
        return CellInfo::column();
    }
    virtual unsigned xfIndex() const {
        return CellInfo::xfIndex();
    }
};


class NameRecord : public Record
{
public:
    FormulaToken m_formula;

    static const unsigned int id;

    unsigned int rtti() const {
        return this->id;
    }

    NameRecord(Workbook *book);

    ~NameRecord();

    QString definedName() const;

    void setDefinedName(const QString& name);

    unsigned sheetIndex() const;

    bool isBuiltin() const;

    virtual void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions);

    virtual const char* name() const {
        return "NAME";
    }

    virtual void dump(std::ostream& out) const;

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

    unsigned int rtti() const {
        return this->id;
    }

    /**
     * Creates a new RK record.
     */
    RKRecord(Workbook *book);

    /**
     * Destroys the record.
     */
    virtual ~RKRecord();

    virtual void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions);

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

    virtual const char* name() const {
        return "RK";
    }

    virtual void dump(std::ostream& out) const;

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

    unsigned int rtti() const {
        return this->id;
    }

    /**
     * Creates a new Label record.
     */
    RStringRecord(Workbook *book);

    /**
     * Destroys the record.
     */
    virtual ~RStringRecord();

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

    virtual void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions);

    virtual const char* name() const {
        return "RSTRING";
    }

    virtual void dump(std::ostream& out) const;

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

    unsigned int rtti() const {
        return this->id;
    }

    /**
     * Creates a new SST record.
     */
    SSTRecord(Workbook *book);

    /**
     * Destroys the record.
     */
    virtual ~SSTRecord();

    virtual void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions);

    /**
      Returns the number of available string in this string table.
     */
    unsigned count() const;

    /**
      Returns the string at specified index.
      Note that index must be less than count().
      If index is not valid, this will return QString::null.
     */
    QString stringAt(unsigned index) const;

    /**
      Returnsformat runs for the string at specified index.
      Format runs are a mapping from character-index -> font index.
     */
    std::map<unsigned, unsigned> formatRunsAt(unsigned index) const;

    virtual const char* name() const {
        return "SST";
    }

    virtual void dump(std::ostream& out) const;

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
    ObjRecord(Workbook *book);
    virtual ~ObjRecord();
    virtual unsigned rtti() const {
        return this->id;
    }
    virtual const char* name() const {
        return "Obj";
    }
    virtual void dump(std::ostream&) const;
    virtual void setData(unsigned size, const unsigned char* data, const unsigned* continuePositions);
};

class TxORecord : public Record
{
public:
    QString m_text;
    static const unsigned id;
    TxORecord(Workbook *book);
    virtual ~TxORecord();
    virtual unsigned rtti() const {
        return this->id;
    }
    virtual const char* name() const {
        return "TxO";
    }
    virtual void dump(std::ostream&) const;
    virtual void setData(unsigned size, const unsigned char* data, const unsigned* continuePositions);
};

class MsoDrawingRecord : public Record, public DrawingObject
{
public:
    //MSO::OfficeArtDgContainer m_container;
    static const unsigned id;
    MsoDrawingRecord(Workbook *book) : Record(book) {}
    virtual ~MsoDrawingRecord() {}
    virtual unsigned rtti() const {
        return this->id;
    }
    virtual const char* name() const {
        return "MsoDrawing";
    }
    virtual void dump(std::ostream&) const;
    virtual void setData(unsigned size, const unsigned char* data, const unsigned* continuePositions);
};

class MsoDrawingBlibItem
{
public:
    //enum Type { Picture, ... };
    PictureReference m_picture;
    explicit MsoDrawingBlibItem(const PictureReference &picture);
};

class Picture
{
public:
    /// The unique identifier of the picture.
    QString m_id;
    /// The path and filename of the picture-file in the KoStore. So, can be something like "Pictures/TheUniqueIdentifierOfThePicture.jpg" for example.
    QString m_filename;
    /// The position of the picture in the sheet.
    unsigned long m_colL, m_dxL, m_rwT, m_dyT, m_colR, m_dxR, m_rwB, m_dyB;
    Picture(MsoDrawingRecord *record, const PictureReference &picture);
};

class MsoDrawingGroupRecord : public Record
{
public:
    QList< MsoDrawingBlibItem* > m_items;
    static const unsigned id;
    MsoDrawingGroupRecord(Workbook *book);
    virtual ~MsoDrawingGroupRecord();
    virtual unsigned rtti() const {
        return this->id;
    }
    virtual const char* name() const {
        return "MsoDrawingGroup";
    }
    virtual void dump(std::ostream&) const;
    virtual void setData(unsigned size, const unsigned char* data, const unsigned* continuePositions);
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

    unsigned int rtti() const {
        return this->id;
    }

    /**
     * Creates a new XF record.
     */
    XFRecord(Workbook *book);

    /**
     * Creates a copy of XF record.
     */
    XFRecord(const XFRecord& xf);

    /**
     * Assigns from another XF record.
     */
    XFRecord& operator=(const XFRecord& xf);

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
    void setFontIndex(unsigned fi);

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
    void setFormatIndex(unsigned fi);

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
    void setLocked(bool l);

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
    void setFormulaHidden(bool f);

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
    void setParentStyle(unsigned ps);

    enum {
        General = 0,
        Left,
        Centered,
        Right,
        Filled,
        Justified,
        CenteredSelection,
        Distributed
    };

    /**
     * Gets the horizontal alignment, e.g Left.
     */
    unsigned horizontalAlignment() const;

    /**
     * Sets the horizontal alignment, e.g Left.
     */
    void setHorizontalAlignment(unsigned ha);

    /**
     * Returns human-readable string representation of the horizontal alignment.
       For example, XFRecord::Left will return "Left".
     */
    const char* horizontalAlignmentAsString() const;

    enum {
        Top = 1,
        VCentered = 2,
        Bottom = 3,
        VJustified = 4,
        VDistributed = 5
    };

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
    void setVerticalAlignment(unsigned va);

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
    void setTextWrap(bool wrap);

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
    void setRotationAngle(unsigned angle);

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
    void setStackedLetters(bool stacked);

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
    void setIndentLevel(unsigned i);

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
    void setShrinkContent(bool s);

    enum {
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
    void setLeftBorderStyle(unsigned style);

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
    void setLeftBorderColor(unsigned color);

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
    void setRightBorderStyle(unsigned style);

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
    void setRightBorderColor(unsigned color);

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
    void setTopBorderStyle(unsigned style);

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
    void setTopBorderColor(unsigned color);

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
    void setBottomBorderStyle(unsigned style);

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
    void setBottomBorderColor(unsigned color);

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
    void setDiagonalTopLeft(bool d);

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
    void setDiagonalBottomLeft(bool d);

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
    void setDiagonalStyle(unsigned style);

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
    void setDiagonalColor(unsigned color);

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
    void setFillPattern(unsigned pattern);

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
    void setPatternForeColor(unsigned color);

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
    void setPatternBackColor(unsigned color);

    virtual const char* name() const {
        return "XF";
    }

    virtual void setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions);

    virtual void dump(std::ostream& out) const;

private:
    class Private;
    Private* d;
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

    virtual unsigned rtti() const { return this->id; }

    BkHimRecord(Workbook *book);
    virtual ~BkHimRecord();

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
    void setImagePath( QString imagePath );

    unsigned imageSize() const;
    void setImageSize( unsigned imageSize );

    virtual void setData( unsigned size, const unsigned char* data, const unsigned* continuePositions );

    virtual const char* name() const { return "BkHim"; }

    virtual void dump( std::ostream& out ) const;

private:
    class Private;
    Private * const d;
};

} // namespace Swinder



#endif // SWINDER_EXCEL_H
