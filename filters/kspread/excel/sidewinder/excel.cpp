/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003-2005 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
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

#include "excel.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <stdlib.h>
#include <string.h>
#include <sstream>

#include <QBuffer>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QtEndian>

#include "pole.h"
#include "swinder.h"
#include "utils.h"
#include "globalssubstreamhandler.h"
#include "worksheetsubstreamhandler.h"
#include "chartsubstreamhandler.h"

//#define SWINDER_XLS2RAW

// Use anonymous namespace to cover following functions
namespace
{

// RK value is special encoded integer or floating-point
// see any documentation of Excel file format for detail description
static inline void decodeRK(unsigned rkvalue, bool& isInteger,
                            int& i, double& f)
{
    double factor = (rkvalue & 0x01) ? 0.01 : 1;
    if (rkvalue & 0x02) {
        // FIXME check that int is 32 bits ?
        isInteger = true;
        i = *((int*) & rkvalue) >> 2;
        if (rkvalue & 0x01) {
            if (i % 100 == 0) {
                i /= 100;
            } else {
                isInteger = false;
                f = i * 0.01;
            }
        }
    } else {
        // TODO ensure double takes 8 bytes
        isInteger = false;
        rkvalue = qFromLittleEndian<quint32>(rkvalue);
        unsigned char* s = (unsigned char*) & rkvalue;
        unsigned char* r = (unsigned char*) & f;
        r[0] = r[1] = r[2] = r[3] = 0;
        r[4] = s[0] & 0xfc;
        r[5] = s[1]; r[6] = s[2];  r[7] = s[3];
        f *= factor;
    }
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
    QString str;
    unsigned size;
    std::map<unsigned, unsigned> formatRuns;
};

EString::EString()
{
    d = new EString::Private();
    d->unicode  = false;
    d->richText = false;
    d->str      = QString::null;
    d->size     = 0;
}

EString::EString(const EString& es)
{
    d = new EString::Private();
    operator=(es);
}

EString& EString::operator=(const EString & es)
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

void EString::setUnicode(bool u)
{
    d->unicode = u;
}

bool EString::richText() const
{
    return d->richText;
}

void EString::setRichText(bool r)
{
    d->richText = r;
}

QString EString::str() const
{
    return d->str;
}

void EString::setStr(const QString& str)
{
    d->str = str;
}

std::map<unsigned, unsigned> EString::formatRuns() const
{
    return d->formatRuns;
}

void EString::setFormatRuns(const std::map<unsigned, unsigned>& formatRuns)
{
    d->formatRuns = formatRuns;
}

unsigned EString::size() const
{
    return d->size;
}

void EString::setSize(unsigned s)
{
    d->size = s;
}

// FIXME use maxsize for sanity check
EString EString::fromUnicodeString(const void* p, bool longString, unsigned /* maxsize */, const unsigned* continuePositions, unsigned continuePositionsOffset)
{
    const unsigned char* data = (const unsigned char*) p;
    QString str = QString::null;

    unsigned offset = longString ? 2 : 1;
    unsigned len = longString ? readU16(data) : data[0];
    unsigned char flag = data[ offset ];
    offset++; // for flag (1 byte)

    bool unicode = flag & 0x01;
    bool asianPhonetics = flag & 0x04;
    bool richText = flag & 0x08;
    unsigned formatRuns = 0;
    unsigned asianPhoneticsSize = 0;

    if (richText) {
        formatRuns = readU16(data + offset);
        offset += 2;
    }

    if (asianPhonetics) {
        asianPhoneticsSize = readU32(data + offset);
        offset += 4;
    }

    // find out total bytes used in this string
    unsigned size = offset;
    if (richText) size += (formatRuns * 4);
    if (asianPhonetics) size += asianPhoneticsSize;

    str = QString();
    for (unsigned k = 0; k < len; k++) {
        unsigned uchar;
        if (unicode) {
            uchar = readU16(data + offset);
            offset += 2;
            size += 2;
        } else {
            uchar = data[offset++];
            size++;
        }
        str.append(QString(QChar(uchar)));
        if (continuePositions && offset == *continuePositions - continuePositionsOffset && k < len - 1) {
            unicode = data[offset] & 1;
            size++;
            offset++;
            continuePositions++;
        }
    }

    // read format runs
    std::map<unsigned, unsigned> formatRunsMap;
    for (unsigned k = 0; k < formatRuns; k++) {
        unsigned index = readU16(data + offset);
        unsigned font = readU16(data + offset + 2);
        if (index < len)
            formatRunsMap[index] = font;
        offset += 4;
    }

    EString result;
    result.setUnicode(unicode);
    result.setRichText(richText);
    result.setSize(size);
    result.setStr(str);
    result.setFormatRuns(formatRunsMap);

    return result;
}

// FIXME use maxsize for sanity check
EString EString::fromByteString(const void* p, bool longString,
                                unsigned /* maxsize */)
{
    const unsigned char* data = (const unsigned char*) p;
    QString str = QString::null;

    unsigned offset = longString ? 2 : 1;
    unsigned len = longString ? readU16(data) : data[0];

    char* buffer = new char[ len+1 ];
    memcpy(buffer, data + offset, len);
    buffer[ len ] = 0;
    str = QString(buffer);
    delete[] buffer;

    unsigned size = offset + len;

    EString result;
    result.setUnicode(false);
    result.setRichText(false);
    result.setSize(size);
    result.setStr(str);

    return result;
}



// why different ? see BoundSheetRecord
EString EString::fromSheetName(const void* p, unsigned datasize)
{
    const unsigned char* data = (const unsigned char*) p;
    QString str = QString::null;

    bool richText = false;
    // unsigned formatRuns = 0;

    unsigned len = data[0];
    unsigned flag = data[1];
    bool unicode = flag & 1;

    if (len > datasize - 2) len = datasize - 2;
    if (len == 0) return EString();

    unsigned offset = 2;

    if (!unicode) {
        char* buffer = new char[ len+1 ];
        memcpy(buffer, data + offset, len);
        buffer[ len ] = 0;
        str = QString(buffer);
        delete[] buffer;
    } else {
        for (unsigned k = 0; k < len; k++) {
            unsigned uchar = readU16(data + offset + k * 2);
            str.append(QString(QChar(uchar)));
        }
    }

    EString result;
    result.setUnicode(unicode);
    result.setRichText(richText);
    result.setSize(datasize);
    result.setStr(str);

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

void CellInfo::setRow(unsigned r)
{
    info->row = r;
}

unsigned CellInfo::column() const
{
    return info->column;
}

void CellInfo::setColumn(unsigned c)
{
    info->column = c;
}

unsigned CellInfo::xfIndex() const
{
    return info->xfIndex;
}

void CellInfo::setXfIndex(unsigned i)
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

void ColumnSpanInfo::setFirstColumn(unsigned c)
{
    spaninfo->firstColumn = c;
}

unsigned ColumnSpanInfo::lastColumn() const
{
    return spaninfo->lastColumn;
}

void ColumnSpanInfo::setLastColumn(unsigned c)
{
    spaninfo->lastColumn = c;
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
BOFRecord::BOFRecord(Swinder::Workbook *book):
        Record(book)
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

void BOFRecord::setData(unsigned size, const unsigned char* data, const unsigned int*)
{
    if (size < 4) return;

    d->version  = readU16(data);
    d->type     = readU16(data + 2);
    if (size >= 8) {
        d->build    = readU16(data + 4);
        d->year     = readU16(data + 6);
        if (size >= 16) {
            d->history  = readU32(data + 8);
            d->rversion = readU32(data + 12);
        }
    }
}

unsigned BOFRecord::version() const
{
    unsigned ver = UnknownExcel;
    switch (d->version) {
    case 0x0500 : ver = Excel95; break;
    case 0x0600 : ver = Excel97; break;
    default: break;
    }
    return ver;
}

const char* BOFRecord::versionAsString() const
{
    const char *result = "Unknown";
    switch (version()) {
    case Excel95 : result = "Excel95"; break;
    case Excel97 : result = "Excel97"; break;
    default: break;
    }
    return result;
}

unsigned BOFRecord::type() const
{
    unsigned result = UnknownType;
    switch (d->type) {
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
    switch (type()) {
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

void BOFRecord::dump(std::ostream& out) const
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

// ========== EXTERNBOOK ==========

const unsigned int ExternBookRecord::id = 0x01ae;

class ExternBookRecord::Private
{
public:
    unsigned sheetCount;
    QString name;
};


ExternBookRecord::ExternBookRecord(Workbook *book)
    : Record(book), d(new Private)
{
    d->sheetCount = 0;
}

ExternBookRecord::~ExternBookRecord()
{
    delete d;
}

QString ExternBookRecord::bookName() const
{
    return d->name;
}

void ExternBookRecord::setData(unsigned size, const unsigned char* data, const unsigned int*)
{
    if (size < 4) return;

    d->sheetCount = readU16(data);
    if (data[2] == 0x01 && data[3] == 0x04) { // self-referencing supporting link
        d->name = QString("\004");
    } else if (data[2] == 0x01 && data[3] == ':') { // add-in referencing type of supporting link
        d->name = QString(":");
    } else {
        d->name = EString::fromUnicodeString(data + 2, true, size - 2).str();
        if (d->name.length() > 2 && d->name[0] == 0x0001) {
            if (d->name[1] == 0x0001) {
                // 'unc-volume'
                d->name = "unc://" + d->name.mid(3).replace(0x0003, '/');
            } else if (d->name[1] == 0x0002) {
                // relative to drive volume
                d->name = d->name.mid(2).replace(0x0003, '/');
            } else if (d->name[1] == 0x0005) {
                // full url
                d->name = d->name.mid(3);
            } else {
                // TODO other options
                d->name = d->name.mid(2).replace(0x0003, '/');
            }
        }
    }
}

void ExternBookRecord::dump(std::ostream& out) const
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
    QString externName;
};


ExternNameRecord::ExternNameRecord(Workbook *book)
    : Record(book), d(new Private)
{
    d->optionFlags = 0;
    d->sheetIndex = 0;
}

ExternNameRecord::~ExternNameRecord()
{
    delete d;
}

void ExternNameRecord::setSheetIndex(unsigned sheetIndex)
{
    d->sheetIndex = sheetIndex;
}

unsigned ExternNameRecord::sheetIndex() const
{
    return d->sheetIndex;
}

void ExternNameRecord::setExternName(const QString& name)
{
    d->externName = name;
}

QString ExternNameRecord::externName() const
{
    return d->externName;
}

void ExternNameRecord::setData(unsigned size, const unsigned char* data, const unsigned int*)
{
    if (size < 6) return;

    if (version() == Excel97) {
        d->optionFlags = readU16(data);
        d->sheetIndex = readU16(data + 2);
        d->externName = EString::fromUnicodeString(data + 6, false, size).str();
    }

    if (version() == Excel95) {
        d->optionFlags = 0;
        d->sheetIndex = 0;
        d->externName = EString::fromByteString(data + 6, false, size).str();
    }
}

void ExternNameRecord::dump(std::ostream& /*out*/) const
{
}

// ========== FORMULA ==========

const unsigned int FormulaRecord::id = 0x0006;

class FormulaRecord::Private
{
public:
    Value result;
    FormulaTokens tokens;
    bool shared;
};

FormulaRecord::FormulaRecord(Workbook *book):
        Record(book)
{
    d = new FormulaRecord::Private();
    d->shared = true;
}

FormulaRecord::~FormulaRecord()
{
    delete d;
}

Value FormulaRecord::result() const
{
    return d->result;
}

void FormulaRecord::setResult(const Value& r)
{
    d->result = r;
}

FormulaTokens FormulaRecord::tokens() const
{
    return d->tokens;
}

bool FormulaRecord::isShared() const
{
    return d->shared;
}

void FormulaRecord::setData(unsigned size, const unsigned char* data, const unsigned int*)
{
    if (size < 20) return;

    // cell
    setRow(readU16(data));
    setColumn(readU16(data + 2));
    setXfIndex(readU16(data + 4));

    // val
    if (readU16(data + 12) != 0xffff) {
        // Floating-point
        setResult(Value(readFloat64(data + 6)));
    } else {
        switch (data[6]) {
        case 0: // string, real value in subsequent string record
            setResult(Value(Value::String));
            break;
        case 1: // boolean
            setResult(Value(data[8] ? true : false));
            break;
        case 2: // error code
            setResult(errorAsValue(data[8]));
            break;
        case 3: // empty
            setResult(Value::empty());
            break;
        default: // fallback
            setResult(Value::empty());
            break;
        };
    }

    unsigned opts = readU16(data + 14);
    //const bool fAlwaysCalc = opts & 0x01;
    //const bool reserved1 = opts & 0x02;
    //const bool fFill = opts & 0x04;
    d->shared = opts & 0x08;
    //const bool reserved2 = opts & 0x10;
    //const bool fClearErrors = opts & 0x20;

    // 4 bytes chn...

    FormulaDecoder decoder;
    d->tokens = decoder.decodeFormula(size, 20, data, version());
}

void FormulaRecord::dump(std::ostream& out) const
{
    out << "FORMULA" << std::endl;
    out << "                Row : " << row() << std::endl;
    out << "             Column : " << column() << std::endl;
    out << "           XF Index : " << xfIndex() << std::endl;
    out << "             Result : " << result() << std::endl;

    FormulaTokens ts = tokens();
    out << "             Tokens : " << ts.size() << std::endl;
    for (unsigned i = 0; i < ts.size(); i++)
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

SharedFormulaRecord::SharedFormulaRecord(Workbook *book):
        Record(book)
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

void SharedFormulaRecord::setData(unsigned size, const unsigned char* data, const unsigned int*)
{
    if (size < 8) return;

    // maybe read range
    d->numCells = data[7];

    unsigned formula_len = readU16(data + 8);

    // reconstruct all tokens
    d->tokens.clear();
    for (unsigned j = 10; j < size;) {
        unsigned ptg = data[j++];
        ptg = ((ptg & 0x40) ? (ptg | 0x20) : ptg) & 0x3F;
        FormulaToken token(ptg);
        token.setVersion(version());

        if (token.id() == FormulaToken::String) {
            // find bytes taken to represent the string
            EString estr = (version() == Excel97) ?
                           EString::fromUnicodeString(data + j, false, formula_len) :
                           EString::fromByteString(data + j, false, formula_len);
            token.setData(estr.size(), data + j);
            j += estr.size();
        } else {
            // normal, fixed-size token
            if (token.size() > 1) {
                token.setData(token.size(), data + j);
                j += token.size();
            }
        }

        d->tokens.push_back(token);
    }
}

void SharedFormulaRecord::dump(std::ostream& out) const
{
    out << "SHAREDFMLA" << std::endl;
    // range
    out << "          Num cells : " << d->numCells << std::endl;

    FormulaTokens ts = tokens();
    out << "             Tokens : " << ts.size() << std::endl;
    for (unsigned i = 0; i < ts.size(); i++)
        out << "                       " << ts[i]  << std::endl;

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

MulRKRecord::MulRKRecord(Workbook *book):
        Record(book), CellInfo(), ColumnSpanInfo()
{
    d = new MulRKRecord::Private();
}

MulRKRecord::~MulRKRecord()
{
    delete d;
}

unsigned MulRKRecord::xfIndex(unsigned i) const
{
    if (i >= d->xfIndexes.size()) return 0;
    return d->xfIndexes[ i ];
}

bool MulRKRecord::isInteger(unsigned i) const
{
    if (i >= d->isIntegers.size()) return true;
    return d->isIntegers[ i ];
}

int MulRKRecord::asInteger(unsigned i) const
{
    if (i >= d->intValues.size()) return 0;
    return d->intValues[ i ];
}

double MulRKRecord::asFloat(unsigned i) const
{
    if (i >= d->floatValues.size()) return 0.0;
    return d->floatValues[ i ];
}

unsigned MulRKRecord::encodedRK(unsigned i) const
{
    if (i >= d->rkValues.size()) return 0;
    return d->rkValues[ i ];
}

void MulRKRecord::setData(unsigned size, const unsigned char* data, const unsigned int*)
{
    if (size < 6) return;

    setRow(readU16(data));

    setFirstColumn(readU16(data + 2));
    setLastColumn(readU16(data + size - 2));

    d->xfIndexes.clear();
    d->isIntegers.clear();
    d->intValues.clear();
    d->floatValues.clear();
    for (unsigned i = 4; i < size - 2; i += 6) {
        d->xfIndexes.push_back(readU16(data + i));
        unsigned rk = readU32(data + i + 2);
        d->rkValues.push_back(rk);
        bool isInteger = true; int iv = 0; double fv = 0.0;
        decodeRK(rk, isInteger, iv, fv);

        d->isIntegers.push_back(isInteger);
        d->intValues.push_back(isInteger ? iv : (int)fv);
        d->floatValues.push_back(!isInteger ? fv : (double)iv);
    }

    // FIXME sentinel !
}

void MulRKRecord::dump(std::ostream& out) const
{
    out << "MULRK" << std::endl;
    out << "                Row : " << row() << std::endl;
    out << "       First Column : " << firstColumn() << std::endl;
    out << "        Last Column : " << lastColumn() << std::endl;
    for (unsigned c = firstColumn(); c <= lastColumn(); c++) {
        out << "          Column  " << c << " : " << asFloat(c - firstColumn());
        out << "  Encoded: " << std::hex << encodedRK(c - firstColumn());
        out << "  Xf: " << std::dec << xfIndex(c - firstColumn());
        out << std::endl;
    }
}

// ========== NAME ==========

const unsigned int NameRecord::id = 0x0018; // Lbl record

class NameRecord::Private
{
public:
    unsigned optionFlags;
    QString definedName;
    int sheetIndex; // 0 for global
    bool builtin;
};


NameRecord::NameRecord(Workbook *book)
    : Record(book)
{
    d = new Private;
    d->optionFlags = 0;
}

NameRecord::~NameRecord()
{
    delete d;
}

void NameRecord::setDefinedName(const QString& name)
{
    d->definedName = name;
}

QString NameRecord::definedName() const
{
    return d->definedName;
}

unsigned NameRecord::sheetIndex() const
{
    return d->sheetIndex;
}

bool NameRecord::isBuiltin() const
{
    return d->builtin;
}

void NameRecord::setData(unsigned size, const unsigned char* data, const unsigned int*)
{
    if (size < 14) {
        setIsValid(false);
        return;
    }

    d->optionFlags = readU16(data);
    //const bool fHidden = d->optionFlags & 0x01;
    //const bool fFunc = d->optionFlags & 0x02;
    //const bool fOB = d->optionFlags & 0x04;
    //const bool fProc = d->optionFlags & 0x08;
    //const bool fCalcExp = d->optionFlags & 0x10;
    d->builtin = d->optionFlags & 0x20;
    // 6 bits fGrp
    //const bool reserved1 = d->optionFlags & 0x1800;
    //const bool fPublished = d->optionFlags & 0x3000;
    //const bool fWorkbookParam = d->optionFlags & 0x6000;
    //const bool reserved2 = d->optionFlags & 0xC000;

    const unsigned len = readU8(data + 3); // cch
    const unsigned cce = readU16(data + 4); // len of rgce
    // 2 bytes reserved
    d->sheetIndex = readU16(data + 8); // if !=0 then its a local name
    // 4 bytes reserved

    if (version() == Excel95) {
        char* buffer = new char[ len+1 ];
        memcpy(buffer, data + 14, len);
        buffer[ len ] = 0;
        d->definedName = QString(buffer);
        delete[] buffer;
    } else  if (version() == Excel97) {
        if (d->builtin) { // field is for a build-in name
            const unsigned opts = readU8(data + 14);
            const bool fHighByte = opts & 0x01;
            const unsigned id = fHighByte ? readU16(data + 15) : readU8(data + 15) + 0x0*256;
            switch(id) {
                case 0x00: d->definedName = "Consolidate_Area"; break;
                case 0x01: d->definedName = "Auto_Open"; break;
                case 0x02: d->definedName = "Auto_Close"; break;
                case 0x03: d->definedName = "Extract"; break;
                case 0x04: d->definedName = "Database"; break;
                case 0x05: d->definedName = "Criteria"; break;
                case 0x06: d->definedName = "Print_Area"; break;
                case 0x07: d->definedName = "Print_Titles"; break;
                case 0x08: d->definedName = "Recorder"; break;
                case 0x09: d->definedName = "Data_Form"; break;
                case 0x0A: d->definedName = "Auto_Activate"; break;
                case 0x0B: d->definedName = "Auto_Deactivate"; break;
                case 0x0C: d->definedName = "Sheet_Title"; break;
                case 0x0D: d->definedName = "_FilterDatabase"; break;
                default: break;
            }
        } else { // must satisfy same restrictions then name field on XLNameUnicodeString
            const unsigned opts = readU8(data + 14);
            const bool fHighByte = opts & 0x01;
            Q_ASSERT((opts << 1) == 0x0);

            // XLUnicodeStringNoCch
            QString str = QString();
            if (fHighByte) {
                for (unsigned k = 0; k < len*2; k++) {
                    unsigned zc = readU16(data + 15 + k * 2);
                    str.append(QString(zc));
                }
            } else {
                for (unsigned k = 0; k < len; k++) {
                    unsigned char uc = readU8(data + 15 + k) + 0x0 * 256;
                    str.append(QString(uc));
                }
            }

            // This is rather illogical and seems there is nothing in the specs about this,
            // but the string "_xlfn." may in front of the string we are looking for. So,
            // remove that one and ignore whatever it means...
            if (str.startsWith("_xlfn."))
                str = str.mid(6);

            d->definedName = str;
        }
    } else {
        setIsValid(false);
    }

    // rgce, NamedParsedFormula
    if(cce >= 1) {
        /*
        FormulaDecoder decoder;
        m_formula = decoder.decodeNamedFormula(cce, data + size - cce, version());
        std::cout << ">>" << m_formula.ascii() << std::endl;
        */
        const unsigned char* startNamedParsedFormula = data + size - cce;
        unsigned ptg = readU8(startNamedParsedFormula);
        ptg = ((ptg & 0x40) ? (ptg | 0x20) : ptg) & 0x3F;
        FormulaToken t(ptg);
        t.setVersion(version());
        t.setData(cce - 1, startNamedParsedFormula + 1);
        m_formula = t;
    }

    std::cout << "NameRecord name=" << d->definedName << " iTab=" << d->sheetIndex << " fBuiltin=" << d->builtin << " formula=" << m_formula.id() << " (" << m_formula.idAsString() << ")" << std::endl;
}

void NameRecord::dump(std::ostream& /*out*/) const
{
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

RKRecord::RKRecord(Workbook *book):
        Record(book), CellInfo()
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
    if (d->integer)
        return d->i;
    else
        return (int)d->f;
}

double RKRecord::asFloat() const
{
    if (!d->integer)
        return d->f;
    else
        return (double)d->i;
}

void RKRecord::setInteger(int i)
{
    d->integer = true;
    d->i = i;
    d->f = (double)i;
}

void RKRecord::setFloat(double f)
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
void RKRecord::setData(unsigned size, const unsigned char* data, const unsigned int*)
{
    if (size < 10) return;

    setRow(readU16(data));
    setColumn(readU16(data + 2));
    setXfIndex(readU16(data + 4));

    int i = 0; double f = 0.0;
    d->rk = readU32(data + 6);
    decodeRK(d->rk, d->integer, i, f);
    if (d->integer) setInteger(i);
    else setFloat(f);
}

void RKRecord::dump(std::ostream& out) const
{
    out << "RK" << std::endl;
    out << "                Row : " << row() << std::endl;
    out << "             Column : " << column() << std::endl;
    out << "           XF Index : " << xfIndex() << std::endl;
    out << "              Value : " << asFloat() << std::endl;
    out << "         Encoded RK : 0x" << std::hex << encodedRK() << std::endl;
    out << std::dec;
}

// ========== RSTRING ==========

const unsigned int RStringRecord::id = 0x00d6;

class RStringRecord::Private
{
public:
    QString label;
};

RStringRecord::RStringRecord(Workbook *book):
        Record(book), CellInfo()
{
    d = new RStringRecord::Private();
    d->label = QString::null;
}

RStringRecord::~RStringRecord()
{
    delete d;
}

QString RStringRecord::label() const
{
    return d->label;
}

void RStringRecord::setLabel(const QString& l)
{
    d->label = l;
}

// FIXME formatting runs ? in EString perhaps ?
void RStringRecord::setData(unsigned size, const unsigned char* data, const unsigned int*)
{
    if (size < 6) return;

    setRow(readU16(data));
    setColumn(readU16(data + 2));
    setXfIndex(readU16(data + 4));

    // FIXME check Excel97
    QString label = (version() >= Excel97) ?
                    EString::fromUnicodeString(data + 6, true, size - 6).str() :
                    EString::fromByteString(data + 6, true, size - 6).str();
    setLabel(label);
}

void RStringRecord::dump(std::ostream& out) const
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
    std::vector<QString> strings;
    std::vector<std::map<unsigned, unsigned> > formatRuns;
};

SSTRecord::SSTRecord(Workbook *book):
        Record(book)
{
    d = new SSTRecord::Private();
    d->total = 0;
    d->count = 0;
}

SSTRecord::~SSTRecord()
{
    delete d;
}

QString sstrecord_get_plain_string(const unsigned char* data, unsigned length)
{
    char* buffer = new char[ length+1 ];
    memcpy(buffer, data, length);
    buffer[ length ] = 0;
    QString str = QString(buffer);
    delete[] buffer;
    return str;
}

void SSTRecord::setData(unsigned size, const unsigned char* data, const unsigned int* continuePositions)
{
    if (size < 8) return;

    d->total = readU32(data);
    d->count = readU32(data + 4);

    unsigned offset = 8;
    unsigned int nextContinuePosIdx = 0;
    unsigned int nextContinuePos = continuePositions[0];

    d->strings.clear();
    for (unsigned i = 0; i < d->count; i++) {
        // check against size
        if (offset >= size) {
            std::cerr << "Warning: reached end of SST record, but not all strings have been read!" << std::endl;
            break;
        }

        EString es = EString::fromUnicodeString(data + offset, true, size - offset, continuePositions + nextContinuePosIdx, offset);
        d->strings.push_back(es.str());
        d->formatRuns.push_back(es.formatRuns());
        offset += es.size();
        while (nextContinuePos < offset) nextContinuePos = continuePositions[++nextContinuePosIdx];
    }


    // sanity check, adjust to safer condition
    if (d->count > d->strings.size()) {
        std::cerr << "Warning: mismatch number of string in SST record, expected " << d->count << ", got " << d->strings.size() << "!" << std::endl;
        d->count = d->strings.size();
    }
}

unsigned SSTRecord::count() const
{
    return d->count;
}

// why not just string() ? to avoid easy confusion with std::string
QString SSTRecord::stringAt(unsigned index) const
{
    if (index >= count()) return QString::null;
    return d->strings[ index ];
}

std::map<unsigned, unsigned> SSTRecord::formatRunsAt(unsigned index) const
{
    if (index >= count()) return std::map<unsigned, unsigned>();
    return d->formatRuns[ index ];
}

void SSTRecord::dump(std::ostream& out) const
{
    out << "SST" << std::endl;
    out << "         Occurrences : " << d->total << std::endl;
    out << "              Count : " << count() << std::endl;
    for (unsigned i = 0; i < count(); i++)
        out << "         String #" << std::setw(2) << i << " : " <<
        stringAt(i) << std::endl;
}

// ========== Obj ==========

const unsigned ObjRecord::id = 0x5D;

ObjRecord::ObjRecord(Workbook *book) : Record(book), m_object(0) {}
ObjRecord::~ObjRecord()
{
    delete m_object;
}

void ObjRecord::dump(std::ostream& out) const
{
    out << "Obj" << std::endl;
    if (m_object) {
        out << "  id: " << m_object->id() << std::endl;
        out << "  type: " << m_object->type() << std::endl;
    }
}

void ObjRecord::setData(unsigned size, const unsigned char* data, const unsigned* /* continuePositions */)
{
    if (size < 4) {
        setIsValid(false);
        return;
    }

    // FtCmo struct
    const unsigned char* startFtCmo = data;
    const unsigned long ftcmo = readU16(startFtCmo);
    const unsigned long cbcmo = readU16(startFtCmo + 2);
    if (ftcmo !=  0x15 || cbcmo !=  0x12) {
        std::cerr << "ObjRecord::setData: invalid ObjRecord" << std::endl;
        setIsValid(false);
        return;
    }

    // cmo struct
    const unsigned long ot = readU16(startFtCmo + 4);
    const unsigned long id = readU16(startFtCmo + 6);
    //const unsigned long opts = readU16(startFtCmo + 8);
    //const bool fLocked = opts & 0x01;
    //const bool reserved = opts & 0x02;
    //const bool fDefaultSize = opts & 0x04;
    //const bool fPublished = opts & 0x08;
    //const bool fPrint = opts & 0x10;
    //const bool unused1 = opts & 0x20;
    //const bool unused2 = opts & 0x60;
    //const bool fDisabled = opts & 0xC0;
    //const bool fUIObj = opts & 0x180;
    //const bool fRecalcObj = opts & 0x300;
    //const bool unused3 = opts & 0x600;
    //const bool unused4 = opts & 0xC00;
    //const bool fRecalcObjAlways = opts & 0x1800;
    //const bool unused5 = opts & 0x3000;
    //const bool unused6 = opts & 0x6000;
    //const bool unused7 = opts & 0xC000;
    //const unsigned long unused8 = readU32(startFtCmo + 10);
    //const unsigned long unused9 = readU32(startFtCmo + 14);
    //const unsigned long unused10 = readU32(startFtCmo + 18);

    bool fDde = false; // dynamic data exchange reference?
    bool fCtl = false; // ActiveX control?
    bool fPrstm = false; // false=embedded store or true=control stream

    const unsigned char* startPict = data + 22;
    switch (ot) {
    case Object::Group: // gmo
        printf("ObjRecord::setData group\n");
        startPict += 6;
        break;
    case Object::Picture: { // pictFormat and pictFlags
        m_object = new PictureObject(id);
        //const unsigned long ft = readU16(startPict);
        //const unsigned long cb = readU16(startPict + 2);
        const unsigned long cf = readU16(startPict + 4);
        switch (cf) {
        case 0x0002:
            static_cast<PictureObject*>(m_object)->setType(PictureObject::EnhancedMetafile);
            break;
        case 0x0009:
            static_cast<PictureObject*>(m_object)->setType(PictureObject::Bitmap);
            break;
        case 0xFFFF:
            static_cast<PictureObject*>(m_object)->setType(PictureObject::Unspecified);
            break;
        default:
            std::cerr << "ObjRecord::setData: invalid ObjRecord Picture" << std::endl;
            setIsValid(false);
            delete m_object;
            m_object = 0;
            return;
        }
        const unsigned long ft2 = readU16(startPict + 6);
        Q_ASSERT(ft2 == 0x0008); Q_UNUSED(ft2);
        const unsigned long cb2 = readU16(startPict + 8);
        Q_ASSERT(cb2 == 0x0002); Q_UNUSED(cb2);
        const unsigned long opts2 = readU16(startPict + 10);
        //const bool fAutoPict = opts2 & 0x01;
        fDde = opts2 & 0x02; // dynamic data exchange reference?
        //const bool dPrintCalc = opts2 & 0x04;
        //const bool fIcon = opts2 & 0x08;
        fCtl = opts2 & 0x10; // ActiveX control?
        Q_ASSERT(!(fCtl && fDde));
        fPrstm = opts2 & 0x20;
        //const bool unused1 = opts2 & 0x60;
        //const bool fCamera = opts2 & 0xC0;
        //const bool fDefaultSize = opts2 & 0x180;
        //const bool fAutoload = opts2 & 0x300;
        //const bool unused2 = opts2 & 0x600;
        //const bool unused3 = opts2 & 0xC00;
        //const bool unused4 = opts2 & 0x1800;
        //const bool unused5 = opts2 & 0x3000;
        //const bool unused6 = opts2 & 0x6000;
        //const bool unused7 = opts2 & 0xC000;
        std::cout << "ObjRecord::setData picture id=" << id << " fDde=" << fDde << " FCtl=" << fCtl << " fPrstm=" << fPrstm << std::endl;
        startPict += 12;
    }
    break;
    case Object::Checkbox: // cbls
        printf("ObjRecord::setData checkbox\n");
        startPict += 16;
        break;
    case Object::RadioButton: // cbls and rbo
        printf("ObjRecord::setData RadioButton\n");
        startPict += 26;
        break;
    case Object::SpinControl: // sbs
        printf("ObjRecord::setData SpinControl\n");
        startPict += 24;
        break;
    case Object::Scrollbar: // sbs
        printf("ObjRecord::setData Scrollbar\n");
        startPict += 24;
        break;
    case Object::List: // sbs
        printf("ObjRecord::setData List\n");
        startPict += 24;
        break;
    case Object::DropdownList: // sbs
        printf("ObjRecord::setData DropdownList\n");
        startPict += 24;
        break;
    case Object::Note: { // nts
        std::cout << "ObjRecord::setData note id=" << id << std::endl;
        m_object = new NoteObject(id);
        const unsigned long ft = readU16(startPict);
        const unsigned long cb = readU16(startPict + 2);
        startPict += 20; // skip guid
        if (ft != 0x000D || cb != 0x0016) {
            std::cerr << "ObjRecord::setData: invalid ObjRecord note with id=" << id << std::endl;
            setIsValid(false);
            delete m_object;
            m_object = 0;
            return;
        }
        //const unsigned long isShared = readU16(startPict); // 0x0000 = Not shared, 0x0001 = Shared.
        //Q_ASSERT( isShared == 0x0000 || isShared == 0x0001 );
        startPict += 6; // includes 4 unused bytes

        //the TxO record that contains the text comes after this record...
        //static_cast<NoteObject*>(m_object)->setNote(  );
    }
    break;

    case Object::Chart:
        std::cout << "ObjRecord::setData chart id=" << id << std::endl;
        m_object = new ChartObject(id);
        break;
    case Object::Rectangle: printf("ObjRecord::setData Rectangle\n"); break;
    case Object::Line: printf("ObjRecord::setData Line\n"); break;
    case Object::Oval: printf("ObjRecord::setData Oval\n"); break;
    case Object::Arc: printf("ObjRecord::setData Arc\n"); break;
    case Object::Text: printf("ObjRecord::setData Text\n"); break;
    case Object::Button: printf("ObjRecord::setData Button\n"); break;
    case Object::Polygon: printf("ObjRecord::setData Polygon\n"); break;
    case Object::EditBox: printf("ObjRecord::setData EditBox\n"); break;
    case Object::Label: printf("ObjRecord::setData Label\n"); break;
    case Object::DialogBox: printf("ObjRecord::setData DialogBox\n"); break;
    case Object::GroupBox: printf("ObjRecord::setData GroupBox\n"); break;
    case Object::OfficeArt: printf("ObjRecord::setData OfficeArt\n"); break;

    default:
        std::cerr << "ObjRecord::setData: Unexpected objecttype " << ot << " in ObjRecord" << std::endl;
        setIsValid(false);
        delete m_object;
        m_object = 0;
        return;
    }

    {
        // FtMacro. Specs say it's optional by not when it's used. So, we need to check it by assuming
        // a valid FtMacro starts with 0x0004... The following code is untested. The only thing we are
        // interested in here is seeking to the structs after this one anyway.
        const unsigned long ft = readU16(startPict);
        if (ft == 0x0004) {
            const unsigned long cmFmla = readU16(startPict + 2);
            startPict += 4;
            int sizeFmla = 0;
            if (cmFmla > 0x0000) { // ObjectParseFormula
                const unsigned long cce = readU16(startPict) >> 1; // 15 bits cce + 1 bit reserved
                // 4 bytes unused
                sizeFmla = 2 + 4 + cce;
                //startPict += sizeFmla;
            }
            // skip embedInfo cause we are not a FtPictFmla
            startPict += cmFmla - sizeFmla - 0; // padding
        }
    }

    // pictFmla
    if (ot == Object::Picture && readU16(startPict) == 0x0009 /* checks ft */) {
        //const unsigned long cb = readU16(startPict + 2);
        startPict += 4;

        /* from the specs;

        fmla (variable): An ObjFmla that specifies the location of the data for the object associated with
        the Obj record that contains this FtPictFmla. If the pictFlags.fDde field of the Obj record that
        contains this FtPictFmla is 1, fmla MUST refer to a name which is defined in an ExternName record
        whose fOle field is 1. If the pictFlags.fCamera field of the Obj record that contains this FtPictFmla
        is 1, fmla MUST refer to a range. Otherwise, the fmla.cce field of this fmla MUST be 0x5 and the
        fmla.rgce field of this fmla MUST contain a PtgTbl followed by four bytes that are undefined and
        MUST be ignored.
        */

        // fmla variable, an ObjFmla struct
        FormulaToken token;
        const unsigned long cbFmla = readU16(startPict);
        int cbFmlaSize = 0;
        int embedInfoSize = 0;
        if (cbFmla > 0x0000) { // fmla variable, optional ObjectParsedFormula struct
            const unsigned long cce = readU16(startPict + cbFmlaSize + 2) >> 1; // 15 bits cce + 1 bit reserved
            cbFmlaSize += 2 + 2 + 4; // 4 bytes unused

            // rgce
            unsigned ptg = readU8(startPict + cbFmlaSize);
            cbFmlaSize += 1;
            ptg = ((ptg & 0x40) ? (ptg | 0x20) : ptg) & 0x3F;
            token = FormulaToken(ptg);
            token.setVersion(version());
            std::cout << "ObjRecord::setData: Picture is of type id=" << token.id() << " name=" << token.idAsString() << std::endl;
            if (token.size() > 0) {
                token.setData(token.size(), startPict + cbFmlaSize);
                cbFmlaSize += token.size();
            }

            if (cce == 0x5 && token.id() == FormulaToken::Table) {
                cbFmlaSize += 4;
            }

            // embededInfo variable, an optional PictFmlaEmbedInfo
            if (token.id() == FormulaToken::Table) {
                const unsigned ttb = readU8(startPict + cbFmlaSize);
                if (ttb == 0x03) {
                    const unsigned cbClass = readU8(startPict + cbFmlaSize + embedInfoSize + 1);
                    //const unsigned reserved = readU8(startPict + cbFmlaSize + embedInfoSize + 2);
                    embedInfoSize += 3;
                    if (cbClass > 0x0000) { // strClass specifies the class name of the embedded control
                        unsigned size = 0;
                        QString className = readUnicodeString(startPict + cbFmlaSize + embedInfoSize, cbClass, -1, 0, &size);
                        embedInfoSize += size;

                        //TODO
                        std::cout << "ObjRecord::setData: className=" << qPrintable(className) << std::endl;
                    }
                }
            }
        }
        startPict += cbFmla + 2;

        // IPosInCtlStm variable
        if (token.id() == FormulaToken::Table) {
            const unsigned int iposInCtlStm = readU32(startPict);
            if (fPrstm) { // iposInCtlStm specifies the zero-based offset of this object's data within the control stream.
                const unsigned int cbBufInCtlStm = readU32(startPict + 4);
                startPict += 8;
                static_cast<PictureObject*>(m_object)->setControlStream(iposInCtlStm, cbBufInCtlStm);
            } else { // The object‘s data MUST reside in an embedding storage.
                std::stringstream out;
                out << std::setw(8) << std::setfill('0') << std::uppercase << std::hex << iposInCtlStm;
                static_cast<PictureObject*>(m_object)->setEmbeddedStorage(out.str());
            }
        }

        // key variable, PictFmlaKey struct
        if (fCtl) {
            std::string key;
            const unsigned int cbKey = readU32(startPict);
            startPict += 4;
            for (uint i = 0; i < cbKey; ++i) {
                if (key.size() > 0) key += ".";
                key = readU32(startPict);
                startPict += 4;
            }
            //fmlaLinkedCell
            //fmlaListFillRange
            std::cout << "ObjRecord::setData: Runtime license key is: " << key.c_str() << std::endl;
        }
    }

    // linkFmla
    // checkBox
    // radionButton
    // edit
    // list
    // gbo
}

// ========== TxO ==========

const unsigned TxORecord::id = 0x1B6;

TxORecord::TxORecord(Workbook *book) : Record(book) {}
TxORecord::~TxORecord() {}

void TxORecord::dump(std::ostream& out) const
{
    out << "TxO" << std::endl;
}

void TxORecord::setData(unsigned size, const unsigned char* data, const unsigned* /* continuePositions */)
{
    //const unsigned long opts1 = readU16(data);
    //const bool reserved1 = opts1 & 0x01;
    //const unsigned int hAlignment = opts1 & 0x000e; // 3 bits
    //const unsigned int vAlignment = opts1 & 0x0070; // 3 bits
    //const unsigned long rot = readU16(data + 2);
    // 4 bytes reserved

    // controlInfo (6 bytes): An optional ControlInfo structure that specifies the properties for some
    // form controls. The field MUST exist if and only if the value of cmo.ot in the preceding Obj
    // record is 0, 5, 7, 11, 12, or 14.

    const unsigned long cchText = readU16(data + 14);
    const unsigned char* startPict = data + 16;
    const unsigned char* endPict = data + size;
    if(cchText > 0) {
        //const unsigned long cbRuns = readU16(startPict);
        const unsigned long cbFmla = readU16(startPict + 2); // fmla, ObjFmla structure
        startPict += 4 + cbFmla;
    } else {
        //const unsigned long ifntEmpty = readU16(data + 18); // FontIndex
        startPict += 2;
    }

    const unsigned opts = readU8(startPict);
    const bool fHighByte = opts & 0x01;

    // this seems to assert with some documents...
    //Q_ASSERT((opts << 1) == 0x0);

    // XLUnicodeStringNoCch
    m_text = QString();
    if(fHighByte) {
        for (unsigned k = 1; startPict + k + 1 < endPict; k += 2) {
            unsigned zc = readU16(startPict + k);
            if (!QChar(zc).isPrint()) {
                m_text.clear();
                break;
            }
            m_text.append(QChar(zc));
        }
    } else {
        for (unsigned k = 1; startPict + k < endPict; k += 1) {
            unsigned char uc = readU8(startPict + k) + 0x0*256;
            if (!QChar(uc).isPrint()) {
                m_text.clear();
                break;
            }
            m_text.append(QChar(uc));
        }
    }

    std::cout << "TxORecord::setData size=" << size << " text=" << qPrintable(m_text) << std::endl;
}

// ========== MsoDrawing ==========

const unsigned MsoDrawingRecord::id = 0xEC;

class MsoDrawingRecord::Private
{
public:
    MSO::OfficeArtDgContainer container;
};

MsoDrawingRecord::MsoDrawingRecord(Workbook *book)
    : Record(book)
{
    d = new MsoDrawingRecord::Private();
}

MsoDrawingRecord::~MsoDrawingRecord()
{
    delete d;
}

const MSO::OfficeArtDgContainer& MsoDrawingRecord::dgContainer() const
{
    return d->container;
}

void MsoDrawingRecord::dump(std::ostream& out) const
{
    out << "MsoDrawingRecord" << std::endl;
}

void MsoDrawingRecord::setData(unsigned size, const unsigned char* data, const unsigned* continuePositions)
{
    QByteArray byteArr = QByteArray::fromRawData(reinterpret_cast<const char*>(data), size);
    QBuffer buff(&byteArr);
    buff.open(QIODevice::ReadOnly);
    LEInputStream in(&buff);

    MSO::OfficeArtDgContainer container;

    // First try to parse a OfficeArtDgContainer and if that fails try to parse a single OfficeArtSpgrContainerFileBlock. Note
    // that the xls-specs say that the rgChildRec of a MsoDrawing-record always is a OfficeArtDgContainer but that's just wrong
    // since at some documents it's direct the OfficeArtSpContainer we are interested in.
    LEInputStream::Mark _m = in.setMark();
    try {
        MSO::parseOfficeArtDgContainer(in, container);
    } catch(const IOException&) {
        in.rewind(_m);
        container.groupShape = QSharedPointer<MSO::OfficeArtSpgrContainer>(new MSO::OfficeArtSpgrContainer(&container));
        container.groupShape->rgfb.append(MSO::OfficeArtSpgrContainerFileBlock(&container));
        try {
            parseOfficeArtSpgrContainerFileBlock(in, container.groupShape->rgfb.last());
        } catch(const IOException& e) {
            std::cerr << "Invalid MsoDrawingRecord record: " << qPrintable(e.msg) << std::endl;
            setIsValid(false);
            return;
        } catch(...) {
            std::cerr << "Invalid MsoDrawingRecord record: Unexpected error" << std::endl;
            setIsValid(false);
            return;
        }
    }

    // Be sure we got at least something useful we can work with.
    if(!container.groupShape) {
        std::cerr << "Invalid MsoDrawingRecord record: Expected groupShape missing in the container." << std::endl;
        setIsValid(false);
        return;
    }
    
    // Finally remember the container to be able to extract later content out of it.
    d->container = container;
}

// ========== MsoDrawingGroup ==========

const unsigned MsoDrawingGroupRecord::id = 0xEB;

class MsoDrawingGroupRecord::Private
{
public:
    MSO::OfficeArtDggContainer container;
    QList< MsoDrawingBlibItem* > items;
};

MsoDrawingGroupRecord::MsoDrawingGroupRecord(Workbook *book) : Record(book)
{
    d = new Private();
}

MsoDrawingGroupRecord::~MsoDrawingGroupRecord()
{
    delete d;
}

const MSO::OfficeArtDggContainer& MsoDrawingGroupRecord::dggContainer() const
{
    return d->container;
}

QList<MsoDrawingBlibItem*> MsoDrawingGroupRecord::blibItems() const
{
    return d->items;
}

void MsoDrawingGroupRecord::dump(std::ostream& out) const
{
    out << "MsoDrawingGroupRecord" << std::endl;
}

void MsoDrawingGroupRecord::setData(unsigned size, const unsigned char* data, const unsigned* continuePositions)
{
    printf("MsoDrawingGroupRecord::setData size=%i data=%i continuePositions=%i\n",size,*data,*continuePositions);
    if(size < 32) {
        setIsValid(false);
        return;
    }
    
    QByteArray byteArr = QByteArray::fromRawData(reinterpret_cast<const char*>(data), size);
    QBuffer buff(&byteArr);
    buff.open(QIODevice::ReadOnly);
    LEInputStream lei(&buff);
    
    try {
        MSO::parseOfficeArtDggContainer(lei, d->container);
    } catch (const IOException& e) {
        std::cerr << "Invalid MsoDrawingGroup record:" << qPrintable(e.msg) << std::endl;
        setIsValid(false);
        return;
    }
    
    if(d->container.blipStore.data() && m_workbook->store()) {
        m_workbook->store()->enterDirectory("Pictures");
    
        foreach(MSO::OfficeArtBStoreContainerFileBlock fb, d->container.blipStore->rgfb) {
            PictureReference ref = savePicture(fb, m_workbook->store());
            if (ref.name.length() == 0) {
                std::cerr << "Empty name in picture reference for picture with uid=" << ref.uid << " mimetype=" << ref.mimetype << std::endl;
                continue;
            }
            d->items << new MsoDrawingBlibItem(ref);
        }
    
        m_workbook->store()->leaveDirectory();
    }
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

XFRecord::XFRecord(Workbook *book):  Record(book)
{
    d = new XFRecord::Private();
    d->fontIndex           = 0;
    d->formatIndex         = 0;
    d->locked              = false;
    d->formulaHidden       = false;
    d->parentStyle         = 0;
    d->horizontalAlignment = General;
    d->verticalAlignment   = General;
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

XFRecord::XFRecord(const XFRecord& xf):  Record(xf.m_workbook)
{
    d = new XFRecord::Private();
    operator=(xf);
}

XFRecord& XFRecord::operator=(const XFRecord & xf)
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

void XFRecord::setFontIndex(unsigned fi)
{
    d->fontIndex = fi;
}

unsigned XFRecord::formatIndex() const
{
    return d->formatIndex;
}

void XFRecord::setFormatIndex(unsigned fi)
{
    d->formatIndex = fi;
}

bool XFRecord::locked() const
{
    return d->locked;
}

void XFRecord::setLocked(bool l)
{
    d->locked = l;
}

bool XFRecord::formulaHidden() const
{
    return d->formulaHidden;
}

void XFRecord::setFormulaHidden(bool f)
{
    d->formulaHidden = f;
}

unsigned XFRecord::parentStyle() const
{
    return d->parentStyle;
}

void XFRecord::setParentStyle(unsigned p)
{
    d->parentStyle = p;
}

unsigned XFRecord::horizontalAlignment() const
{
    return d->horizontalAlignment;
}

void XFRecord::setHorizontalAlignment(unsigned ha)
{
    d->horizontalAlignment = ha;
}

const char* XFRecord::horizontalAlignmentAsString() const
{
    const char *result = "Unknown";
    switch (horizontalAlignment()) {
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

void XFRecord::setVerticalAlignment(unsigned va)
{
    d->verticalAlignment = va;
}

const char* XFRecord::verticalAlignmentAsString() const
{
    const char *result = "Unknown";
    switch (verticalAlignment()) {
    case General:      result = "General"; break;
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

void XFRecord::setTextWrap(bool wrap)
{
    d->textWrap = wrap;
}

unsigned XFRecord::rotationAngle() const
{
    return d->rotationAngle;
}

void XFRecord::setRotationAngle(unsigned angle)
{
    d->rotationAngle = angle;
}

bool XFRecord::stackedLetters() const
{
    return d->stackedLetters;
}

void XFRecord::setStackedLetters(bool stacked)
{
    d->stackedLetters = stacked;
}

unsigned XFRecord::indentLevel() const
{
    return d->indentLevel;
}

void XFRecord::setIndentLevel(unsigned i)
{
    d->indentLevel = i;
}

bool XFRecord::shrinkContent() const
{
    return d->shrinkContent;
}

void XFRecord::setShrinkContent(bool s)
{
    d->shrinkContent = s;
}

unsigned XFRecord::leftBorderStyle() const
{
    return d->leftBorderStyle;
}

void XFRecord::setLeftBorderStyle(unsigned style)
{
    d->leftBorderStyle = style;
}

unsigned XFRecord::leftBorderColor() const
{
    return d->leftBorderColor;
}

void XFRecord::setLeftBorderColor(unsigned color)
{
    d->leftBorderColor = color;
}

unsigned XFRecord::rightBorderStyle() const
{
    return d->rightBorderStyle;
}

void XFRecord::setRightBorderStyle(unsigned style)
{
    d->rightBorderStyle = style;
}

unsigned XFRecord::rightBorderColor() const
{
    return d->rightBorderColor;
}

void XFRecord::setRightBorderColor(unsigned color)
{
    d->rightBorderColor = color;
}

unsigned XFRecord::topBorderStyle() const
{
    return d->topBorderStyle;
}

void XFRecord::setTopBorderStyle(unsigned style)
{
    d->topBorderStyle = style;
}

unsigned XFRecord::topBorderColor() const
{
    return d->topBorderColor;
}

void XFRecord::setTopBorderColor(unsigned color)
{
    d->topBorderColor = color;
}

unsigned XFRecord::bottomBorderStyle() const
{
    return d->bottomBorderStyle;
}

void XFRecord::setBottomBorderStyle(unsigned style)
{
    d->bottomBorderStyle = style;
}

unsigned XFRecord::bottomBorderColor() const
{
    return d->bottomBorderColor;
}

void XFRecord::setBottomBorderColor(unsigned color)
{
    d->bottomBorderColor = color;
}

bool XFRecord::diagonalTopLeft() const
{
    return d->diagonalTopLeft;
}

void XFRecord::setDiagonalTopLeft(bool dd)
{
    d->diagonalTopLeft = dd;
}

bool XFRecord::diagonalBottomLeft() const
{
    return d->diagonalBottomLeft;
}

void XFRecord::setDiagonalBottomLeft(bool dd)
{
    d->diagonalBottomLeft = dd;
}

unsigned XFRecord::diagonalStyle() const
{
    return d->diagonalStyle;
}

void XFRecord::setDiagonalStyle(unsigned style)
{
    d->diagonalStyle = style;
}

unsigned XFRecord::diagonalColor() const
{
    return d->diagonalColor;
}

void XFRecord::setDiagonalColor(unsigned color)
{
    d->diagonalColor = color;
}

unsigned XFRecord::fillPattern() const
{
    return d->fillPattern;
}

void XFRecord::setFillPattern(unsigned pattern)
{
    d->fillPattern = pattern;
}

unsigned XFRecord::patternForeColor() const
{
    return d->patternForeColor;
}

void XFRecord::setPatternForeColor(unsigned color)
{
    d->patternForeColor = color;
}

unsigned XFRecord::patternBackColor() const
{
    return d->patternBackColor;
}

void XFRecord::setPatternBackColor(unsigned color)
{
    d->patternBackColor = color;
}

void XFRecord::setData(unsigned size, const unsigned char* data, const unsigned int*)
{
    unsigned recordSize = (version() == Excel97) ? 20 : 16;
    if (size < recordSize) return;

    setFontIndex(readU16(data));
    setFormatIndex(readU16(data + 2));

    unsigned protection = readU16(data + 4) & 7;
    setLocked(protection & 1);
    setFormulaHidden(protection & 2);

    setParentStyle(readU16(data + 4) >> 4);

    unsigned align = data[6];
    setHorizontalAlignment(align & 0x07);
    setVerticalAlignment((align >> 4) + 1/*enum starts at 1*/);
    setTextWrap(align & 0x08);

    if (version() == Excel97) {
        unsigned angle = data[7];
        //qDebug() << d->globalIndex << angle;
        setRotationAngle((angle != 255) ? angle : 0);
        setStackedLetters(angle == 255);

        unsigned options = data[8];
//     unsigned attributes = data[9];

        setIndentLevel(options & 0x0f);
        setShrinkContent(options & 0x10);

        unsigned linestyle = readU16(data + 10);
        unsigned color1 = readU16(data + 12);
        unsigned color2 = readU16(data + 14);
        unsigned flag = readU16(data + 16);
        unsigned fill = readU16(data + 18);

        setLeftBorderStyle(linestyle & 0xf);
        setRightBorderStyle((linestyle >> 4) & 0xf);
        setTopBorderStyle((linestyle >> 8) & 0xf);
        setBottomBorderStyle((linestyle >> 12) & 0xf);

        setLeftBorderColor(color1 & 0x7f);//127
        setRightBorderColor((color1 >> 7) & 0x7f);
        setTopBorderColor(color2 & 0x7f);
        setBottomBorderColor((color2 >> 7) & 0x7f);

        const unsigned grbitDiag = color1 >> 14;
        setDiagonalTopLeft(grbitDiag == 0x1 || grbitDiag == 0x3);
        setDiagonalBottomLeft(grbitDiag == 0x2 || grbitDiag == 0x3);
        setDiagonalStyle((flag >> 4) & 0x1e);
        setDiagonalColor(((flag & 0x1f) << 2) + ((color2 >> 14) & 3));

        setFillPattern((flag >> 10) & 0x3f);
        setPatternForeColor(fill & 0x7f);
        setPatternBackColor((fill >> 7) & 0x7f);
    } else {
        unsigned orient = data[7] & 3;
        switch (orient) {
            case 0: break;
            case 1: setStackedLetters(true); break;
            case 2: setRotationAngle(90); break;
            case 3: setRotationAngle(180); break; // 90 degrees clockwise
        }
        unsigned data1 = readU32(data + 8);
        unsigned data2 = readU32(data + 12);

        setPatternForeColor(data1 & 0x7f);
        setPatternBackColor((data1 >> 7) & 0x7f);
        setFillPattern((data1 >> 16) & 0x3f);

        setBottomBorderStyle((data1 >> 22) & 0x07);
        setBottomBorderColor((data1 >> 25) & 0x7f);

        setTopBorderStyle(data2 & 0x07);
        setLeftBorderStyle((data2 >> 3) & 0x07);
        setRightBorderStyle((data2 >> 6) & 0x07);

        setTopBorderColor((data2 >> 9) & 0x7f);
        setLeftBorderColor((data2 >> 16) & 0x7f);
        setRightBorderColor((data2 >> 23) & 0x7f);
    }
}

void XFRecord::dump(std::ostream& out) const
{
    out << "XF" << std::endl;
    out << "       Parent Style : " << parentStyle() << std::endl;
    out << "         Font Index : " << fontIndex() << std::endl;
    out << "       Format Index : " << formatIndex() << std::endl;
    out << "             Locked : " << (locked() ? "Yes" : "No") << std::endl;
    out << " Formula Visibility : " << (formulaHidden() ? "Hidden" : "Visible") << std::endl;
    out << "   Horizontal Align : " << horizontalAlignmentAsString() << std::endl;
    out << "     Vertical Align : " << verticalAlignmentAsString() << std::endl;
    out << "          Text Wrap : " << (textWrap() ? "yes" : "no") << std::endl;
    out << "          Rotation  : " << rotationAngle() << std::endl;
    out << "    Stacked Letters : " << (stackedLetters() ? "yes" : "no") << std::endl;
    out << "       Indent Level : " << indentLevel() << std::endl;
    out << "      Shrink To Fit : " << (shrinkContent() ? "yes" : "no") << std::endl;
    out << "        Left Border : Style " << leftBorderStyle();
    out << " Color: " << leftBorderColor() << std::endl;
    out << "       Right Border : Style " << rightBorderStyle();
    out << " Color: " << rightBorderColor() << std::endl;
    out << "         Top Border : Style " << topBorderStyle();
    out << " Color: " << topBorderColor() << std::endl;
    out << "      Bottom Border : Style " << bottomBorderStyle();
    out << " Color: " << bottomBorderColor() << std::endl;
    out << "     Diagonal Lines : " ;
    if (diagonalTopLeft()) out << "TopLeft ";
    if (diagonalBottomLeft()) out << "BottomLeft ";
    out << "Style " << diagonalStyle() << " Color: " << diagonalColor() << std::endl;
    out << "       Fill Pattern : " << fillPattern() << std::endl;
    out << "         Fill Color : Fore " << patternForeColor() << " Back: "
    << patternBackColor() << std::endl;
}

// ========== BkHimRecord ==========

const unsigned BkHimRecord::id = 0x00e9;

class BkHimRecord::Private
{
public:
    Format format;
    QString imagePath;
};

BkHimRecord::BkHimRecord(Workbook *book)
    : Record(book), d(new Private)
{
}

BkHimRecord::~BkHimRecord()
{
    delete d;
}

BkHimRecord::BkHimRecord( const BkHimRecord& record )
    : Record(record), d(new Private)
{
    *this = record;
}

BkHimRecord& BkHimRecord::operator=( const BkHimRecord& record )
{
    *d = *record.d;
    return *this;
}

QString BkHimRecord::formatToString(Format format)
{
    switch (format) {
        case WindowsBitMap: return QString("WindowsBitMap");
        case NativeFormat: return QString("NativeFormat");
        default: return QString("Unknown: %1").arg(format);
    }
}

BkHimRecord::Format BkHimRecord::format() const
{
    return d->format;
}

void BkHimRecord::setFormat(Format format )
{
    d->format = format;
}

QString BkHimRecord::imagePath() const
{
    return d->imagePath;
}

void BkHimRecord::setImagePath(QString imagePath )
{
    d->imagePath = imagePath;
}

void BkHimRecord::setData( unsigned size, const unsigned char* data, const unsigned int* )
{
    unsigned curOffset = 0;
    if (size < 8) {
        setIsValid(false);
        return;
    }
    setFormat(static_cast<Format>(readU16(data + curOffset)));
    curOffset += 2;

    //16 reserved bits
    curOffset += 2;

    quint32 imageSize = readU32(data + curOffset);
    curOffset += 4;

    static int counter = 1; //we need unique file names
    QString filename = QString("Pictures/sheetBackground%1").arg(counter++);
    if(format() == WindowsBitMap) {
        filename.append(QString(".bmp"));
    }
    setImagePath(filename);

    KoStore *store = m_workbook->store();
    Q_ASSERT(store);
    if(store->open(filename)) {
        //Excel doesn't include the file header, only the pixmap header,
        //we need to convert it to a standard BMP header
        //see http://www.fileformat.info/format/bmp/egff.htm for details

        //quint32 headerSize = readU32(data + curOffset); // this header size is always 12
        curOffset += 4;
        const quint16 width = readU16(data + curOffset); //in px
        curOffset += 2;
        const qint16 height = readU16(data + curOffset); // in px
        curOffset += 2;
        //const qint16 planes = data + curOffset; //must be 1
        curOffset += 2;
        qint16 bitsPerPixel = readU16(data + curOffset); //usually 24
        curOffset += 2;

        //For the standard header see wikipedia or
        //http://www.fastgraph.com/help/bmp_header_format.html
        QByteArray newHeader;
        newHeader.fill(0x0, 54);

        int currentHeaderOffset = 0;

        //signature
        newHeader[0] = 0x42;
        newHeader[1] = 0x4d;
        currentHeaderOffset += 2;

        char* newHeaderChar = newHeader.data();

        //size
        imageSize -= 12; //remove the header size
        const qint32 fileSize = qToLittleEndian(imageSize + 54);
        memcpy(newHeaderChar + currentHeaderOffset, reinterpret_cast<const char*>(&fileSize), 4);
        currentHeaderOffset += 4;

        //4 reserved bytes
        currentHeaderOffset += 4;

        //offset to the start of the image, the size of this new header: always 54 bytes
        const qint32 startImageData = qToLittleEndian(qint32(54));
        memcpy(newHeaderChar + currentHeaderOffset, reinterpret_cast<const char*>(&startImageData), 4);
        currentHeaderOffset += 4;

        const quint32 sizeOfBitmapInfoHeader = qToLittleEndian(qint32(40));
        memcpy(newHeaderChar + currentHeaderOffset, reinterpret_cast<const char*>(&sizeOfBitmapInfoHeader), 4);
        currentHeaderOffset += 4;

        const quint32 imageWidth = qToLittleEndian(qint32(width));
        memcpy(newHeaderChar + currentHeaderOffset, reinterpret_cast<const char*>(&imageWidth), 4);
        currentHeaderOffset += 4;

        const quint32 imageHeight = qToLittleEndian(qint32(height));
        memcpy(newHeaderChar + currentHeaderOffset, reinterpret_cast<const char*>(&imageHeight), 4);
        currentHeaderOffset += 4;

        const quint32 planes = qToLittleEndian(quint16(1));
        memcpy(newHeaderChar + currentHeaderOffset, reinterpret_cast<const char*>(&planes), 2);
        currentHeaderOffset += 2;

        bitsPerPixel = qToLittleEndian(bitsPerPixel);
        memcpy(newHeaderChar + currentHeaderOffset, reinterpret_cast<const char*>(&bitsPerPixel), 2);
        currentHeaderOffset += 2;

        //compresion type, for this case always 0
        currentHeaderOffset += 4;

        //size of image bits
        const quint32 litEndimageSize = qToLittleEndian(imageSize);
        memcpy(newHeaderChar + currentHeaderOffset, reinterpret_cast<const char*>(&litEndimageSize), 4);
        currentHeaderOffset += 4;

        //we leave the remaining bits to zero

        store->write(newHeaderChar, 54);
        store->write((const char*)(data + curOffset), imageSize);
        store->close();
    } else {
        std::cerr << "BkHimRecord: Failed to open file=" << filename << std::endl;
    }
}

void BkHimRecord::dump( std::ostream& out ) const
{
    out << "BkHim" << std::endl;
    out << "             Format : " << formatToString(format()) << std::endl;
    out << "          ImagePath : " << imagePath() << std::endl;
}

static Record* createBkHimRecord(Workbook *book)
{
    return new BkHimRecord(book);
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
    std::map<unsigned, FormatFont> fontCache;

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

static Record* createBOFRecord(Workbook *book)
{
    return new BOFRecord(book);
}
static Record* createExternBookRecord(Workbook *book)
{
    return new ExternBookRecord(book);
}
static Record* createExternNameRecord(Workbook *book)
{
    return new ExternNameRecord(book);
}
static Record* createFormulaRecord(Workbook *book)
{
    return new FormulaRecord(book);
}
static Record* createSharedFormulaRecord(Workbook *book)
{
    return new SharedFormulaRecord(book);
}
static Record* createMulRKRecord(Workbook *book)
{
    return new MulRKRecord(book);
}
static Record* createNameRecord(Workbook *book)
{
    return new NameRecord(book);
}
static Record* createRKRecord(Workbook *book)
{
    return new RKRecord(book);
}
static Record* createRStringRecord(Workbook *book)
{
    return new RStringRecord(book);
}
static Record* createSSTRecord(Workbook *book)
{
    return new SSTRecord(book);
}
static Record* createXFRecord(Workbook *book)
{
    return new XFRecord(book);
}
static Record* createObjRecord(Workbook *book)
{
    return new ObjRecord(book);
}

static Record* createTxORecord(Workbook *book)
{
    return new TxORecord(book);
}

static Record* createRecordMsoDrawingRecord(Workbook *book)
{
    return new MsoDrawingRecord(book);
}

static Record* createMsoDrawingGroupRecord(Workbook *book)
{
    return new MsoDrawingGroupRecord(book);
}

static void registerAllRecordClasses()
{
    registerRecordClasses();
    RecordRegistry::registerRecordClass(BOFRecord::id, createBOFRecord);
    RecordRegistry::registerRecordClass(ExternBookRecord::id, createExternBookRecord);
    RecordRegistry::registerRecordClass(ExternNameRecord::id, createExternNameRecord);
    RecordRegistry::registerRecordClass(FormulaRecord::id, createFormulaRecord);
    RecordRegistry::registerRecordClass(SharedFormulaRecord::id, createSharedFormulaRecord);
    RecordRegistry::registerRecordClass(MulRKRecord::id, createMulRKRecord);
    RecordRegistry::registerRecordClass(NameRecord::id, createNameRecord);
    RecordRegistry::registerRecordClass(RKRecord::id, createRKRecord);
    RecordRegistry::registerRecordClass(RStringRecord::id, createRStringRecord);
    RecordRegistry::registerRecordClass(SSTRecord::id, createSSTRecord);
    RecordRegistry::registerRecordClass(XFRecord::id, createXFRecord);
    RecordRegistry::registerRecordClass(ObjRecord::id, createObjRecord);
    RecordRegistry::registerRecordClass(TxORecord::id, createTxORecord);
    RecordRegistry::registerRecordClass(MsoDrawingRecord::id, createRecordMsoDrawingRecord);
    RecordRegistry::registerRecordClass(MsoDrawingGroupRecord::id, createMsoDrawingGroupRecord);
    RecordRegistry::registerRecordClass(BkHimRecord::id, createBkHimRecord);
}

void printEntries(POLE::Storage &storage, const std::string path = "/", int level = 0)
{
    std::cout << std::setw(level) << "PATH=" << path << std::endl;
    std::list<std::string> entries = storage.entries(path);
    for (std::list<std::string>::iterator it = entries.begin(); it != entries.end(); ++it)  {
        std::cout << std::setw(level + 1) << "ENTRY=" << *it << std::endl;
        std::string p = path == "/" ? "/" + *it + "/" : path + "/" + *it + "/";
        if (storage.isDirectory(p)) {
            printEntries(storage, p, level + 1);
        }
    }
}

bool ExcelReader::load(Workbook* workbook, const char* filename)
{
    registerAllRecordClasses();

    POLE::Storage storage(filename);
    if (!storage.open()) {
        std::cerr << "Cannot open " << filename << std::endl;
        return false;
    }

#ifdef SWINDER_XLS2RAW
    std::cout << "Streams:" << std::endl;
    printEntries(storage);
#endif

    unsigned streamVersion = Swinder::Excel97;
    POLE::Stream* stream;
    stream = new POLE::Stream(&storage, "/Workbook");
    if (stream->fail()) {
        delete stream;
        stream = new POLE::Stream(&storage, "/Book");
        streamVersion = Swinder::Excel95;
    }

    if (stream->fail()) {
        std::cerr << filename << " is not Excel workbook" << std::endl;
        delete stream;
        return false;
    }

    unsigned int buffer_size = 65536;     // current size of the buffer
    unsigned char *buffer = (unsigned char *) malloc(buffer_size);
    unsigned char small_buffer[128];  // small, fixed size buffer

    { // read document meta information
        POLE::Stream *summarystream = new POLE::Stream(&storage, "/SummaryInformation");
        const unsigned long streamStartPosition = summarystream->tell();
        unsigned bytes_read = summarystream->read(buffer, 8);
        //const unsigned long byteorder = readU16( buffer ); // must be 0xFFFE
        //const unsigned long version = readU16( buffer + 2 ); // must be 0x0000 or 0x0001
        //const unsigned long systemId = readU32( buffer + 4 );

        summarystream->seek(summarystream->tell() + 16);   // skip CLSID
        bytes_read = summarystream->read(buffer, 4);
        const unsigned long numPropertySets = bytes_read == 4 ? readU32(buffer) : 0;   // must be 0x00000001 or 0x00000002
        for (uint i = 0; i < numPropertySets; ++ i) {
            summarystream->seek(summarystream->tell() + 16);   // skip FMTIDO
            bytes_read = summarystream->read(buffer, 4);
            if (bytes_read != 4) break;
            const unsigned long firstPropertyOffset = readU32(buffer);

            const unsigned long p = summarystream->tell();
            const unsigned long propertysetStartPosition = streamStartPosition + firstPropertyOffset;
            summarystream->seek(propertysetStartPosition);

            bytes_read = summarystream->read(buffer, 8);
            if (bytes_read != 8) break;
            //unsigned long size = readU32( buffer );
            unsigned long propertyCount = readU32(buffer + 4);
            for (uint i = 0; i < propertyCount; ++i) {
                bytes_read = summarystream->read(buffer, 8);
                if (bytes_read != 8) break;
                Workbook::PropertyType propertyId = Workbook::PropertyType(readU32(buffer));

                // Offset (4 bytes): An unsigned integer representing the offset in bytes from the beginning of
                // the PropertySet packet to the beginning of the Property field for the property represented.
                // MUST be a multiple of 4 bytes.
                unsigned long propertyOffset = readU32(buffer + 4);

                unsigned long p2 = summarystream->tell();

                summarystream->seek(propertysetStartPosition + propertyOffset);
                bytes_read = summarystream->read(buffer, 4);
                if (bytes_read != 4) break;
                unsigned long type = readU16(buffer);
                //unsigned long padding = readU16( buffer + 2 );
                switch (propertyId) {
                case Workbook::PIDSI_TITLE:
                case Workbook::PIDSI_SUBJECT:
                case Workbook::PIDSI_AUTHOR:
                case Workbook::PIDSI_KEYWORDS:
                case Workbook::PIDSI_COMMENTS:
                case Workbook::PIDSI_TEMPLATE:
                case Workbook::PIDSI_LASTAUTHOR:
                case Workbook::PIDSI_REVNUMBER:
                case Workbook::PIDSI_EDITTIME:
                case Workbook::PIDSI_LASTPRINTED_DTM:
                case Workbook::PIDSI_CREATE_DTM:
                case Workbook::PIDSI_LASTSAVED_DTM:
                case Workbook::PIDSI_APPNAME:
                    switch (type) {
                    case 0x001E: { //VT_LPSTR
                        bytes_read = summarystream->read(buffer, 4);
                        if (bytes_read != 4) break;
                        const unsigned long length = readU32(buffer);
                        bytes_read = summarystream->read(buffer, length);
                        if (bytes_read != length) break;
                        QString u = readByteString(buffer, length);
                        const QChar *c = reinterpret_cast<const QChar*>(u.data());
                        QString s = QString(c, u.length());
                        workbook->setProperty(propertyId, s);
                    } break;
                    case 0x0040: { //VT_FILETIME
                        bytes_read = summarystream->read(buffer, 8);
                        if (bytes_read != 8) break;
                        const unsigned long dwLowDateTime = readU32(buffer);
                        const unsigned long dwHighDateTime = readU32(buffer + 4);
                        long long int time = dwHighDateTime;
                        time <<= 32;
                        time += (unsigned long) dwLowDateTime;
                        time -= 116444736000000000LL;
                        QString s(QDateTime::fromTime_t(time / 10000000.0).toString(Qt::ISODate));
                        workbook->setProperty(propertyId, s);
                    } break;
                    default:
                        std::cout << "Ignoring property with known id=" << propertyId << " and unknown type=" << type;
                        break;
                    }
                    break;
                default:
                    if (propertyId != 0x0001 /* GKPIDDSI_CODEPAGE */ &&
                            propertyId != 0x0013 /* GKPIDDSI_SHAREDDOC */) {
                         std::cout << "Ignoring property with unknown id=" << propertyId << " and type=" << type << std::endl;
                    }
                    break;
                }
                summarystream->seek(p2);
            }
            summarystream->seek(p);
        }
        delete summarystream;
    }

    { // read CompObj stream
        POLE::Stream *combObjStream = new POLE::Stream(&storage, "/CompObj");

        // header
        unsigned bytes_read = combObjStream->read(buffer, 28);
      unsigned long length = 0;
      bool hasCombObjStream = bytes_read == 28;
      if(hasCombObjStream) {
          //const unsigned long version = readU32( buffer + 5 );
          //printf(">>>> combObjStream->fullName=%s\n",combObjStream->fullName().c_str());
          //printEntries(storage,"CompObj");

          // AnsiUserType
          bytes_read = combObjStream->read( buffer, 4 );
          length = readU32( buffer );
          bytes_read = combObjStream->read( buffer, length );
          if(bytes_read != length) hasCombObjStream = false;
      }
      if(hasCombObjStream) {
          QString ansiUserType = readByteString(buffer, length);
          printf( "length=%lu ansiUserType=%s\n",length, qPrintable(ansiUserType) );

          // AnsiClipboardFormat
          bytes_read = combObjStream->read( buffer, 4 );
          const unsigned long markerOrLength = readU32( buffer );
          switch (markerOrLength) {
            case 0x00000000: break; // Must not be present, do nothing...
            case 0xFFFFFFFF: // fall through
            case 0xFFFFFFFE: { // must be 4 bytes and contains a clipboard identifier
              bytes_read = combObjStream->read( buffer, 4 );
              //const unsigned long standardFormat = readU32( buffer );
              // switch(standardFormat) {
              //   case 0x00000002: standardFormat=CF_BITMAP;
              //   case 0x00000003: standardFormat=CF_METAFILEPICT
              //   case 0x00000008: standardFormat=CF_DIB
              //   case 0x0000000E: standardFormat=CF_ENHMETAFILE
              // }
              //TODO...
            }
            break;
            default:
              bytes_read = combObjStream->read( buffer, markerOrLength );
              QString ansiString = readByteString(buffer, markerOrLength);
              //TODO...
              //printf( "markerOrLength=%i ansiString=%s\n",markerOrLength,ansiString.ascii() );
          }
          //TODO Reserved1, UnicodeMarker, UnicodeUserType, UnicodeClipboardFormat, Reserved2
      }
        delete combObjStream;
    }

    const unsigned long stream_size = stream->size();
    unsigned int continuePositionsSize = 128; // size of array for continue positions
    unsigned int *continuePositions = (unsigned int *) malloc(continuePositionsSize * sizeof(int));

    workbook->clear();
    d->workbook = workbook;
    d->globals = new GlobalsSubStreamHandler(workbook, streamVersion);
    d->handlerStack.clear();

    while (stream->tell() < stream_size) {
        const int percent = int(stream->tell() / double(stream_size) * 100.0 + 0.5);
        workbook->emitProgress(percent);

        // this is set by FILEPASS record
        // subsequent records will need to be decrypted
        // since we do not support it yet, we have to bail out
        if (d->globals->passwordProtected() && !d->globals->encryptionTypeSupported()) {
            d->workbook->setPasswordProtected(true);
            break;
        }

        // get record type and data size
        unsigned long pos = stream->tell();
        unsigned bytes_read = stream->read(buffer, 4);
        if (bytes_read != 4) break;

        unsigned long type = readU16(buffer);
        unsigned long size = readU16(buffer + 2);
        d->globals->decryptionSkipBytes(4);

        unsigned int continuePositionsCount = 0;

        // verify buffer is large enough to hold the record data
        if (size > buffer_size) {
            buffer = (unsigned char *) realloc(buffer, size);
            buffer_size = size;
        }

        // load actual record data
        bytes_read = stream->read(buffer, size);
        if (bytes_read != size) break;
        d->globals->decryptRecord(type, size, buffer);

        // save current position in stream, to be able to restore the position later on
        unsigned long saved_pos;
        // repeatedly check if the next record is type 0x3C, a continuation record
        unsigned long next_type; // the type of the next record
        do {
            saved_pos = stream->tell();

            bytes_read = stream->read(small_buffer, 4);
            if (bytes_read != 4) break;

            next_type = readU16(small_buffer);
            unsigned long next_size = readU16(small_buffer + 2);

            if(next_type == MsoDrawingRecord::id || next_type == MsoDrawingGroupRecord::id) {
                if(type != next_type)
                    break;
            } else if(next_type == 0x3C) {
                // 0x3C are continues records which are always just merged...
            } else {
                break; // and abort merging of records
            }

            // compress multiple records into one.
            continuePositions[continuePositionsCount++] = size;
            if (continuePositionsCount >= continuePositionsSize) {
                continuePositionsSize *= 2;
                continuePositions = (unsigned int *) realloc(continuePositions, continuePositionsSize * sizeof(int));
            }

            // first verify the buffer is large enough to hold all the data
            if ((size + next_size) > buffer_size) {
                buffer = (unsigned char *) realloc(buffer, size + next_size);
                buffer_size = size + next_size;
            }

            // next read the data of the record
            bytes_read = stream->read(buffer + size, next_size);
            if (bytes_read != next_size) {
                std::cout << "ERROR!" << std::endl;
                break;
            }
            d->globals->decryptionSkipBytes(4);
            d->globals->decryptRecord(next_type, next_size, buffer+size);

            // and finally update size
            size += next_size;
        } while (true);

        // append total size as last continue position
        continuePositions[continuePositionsCount] = size;

        // restore position in stream to the beginning of the next record
        stream->seek(saved_pos);

        // skip record type 0, this is just for filler
        if (type == 0) continue;

        // create the record using the factory
        Record* record = Record::create(type, workbook);

        if (!record) {
//#ifdef SWINDER_XLS2RAW
            std::cout << "Unhandled Record 0x";
            std::cout << std::setfill('0') << std::setw(4) << std::hex << type;
            std::cout << std::dec;
            std::cout << " (" << type << ")";
            std::cout << std::endl;
            std::cout << std::endl;
//#endif
        } else {
            // setup the record and invoke handler
            record->setVersion(d->globals->version());
            record->setData(size, buffer, continuePositions);
            record->setPosition(pos);

#ifdef SWINDER_XLS2RAW
            std::cout << std::setfill('0') << std::setw(8) << std::dec << record->position() << " ";
            if (!record->isValid()) std::cout << "Invalid ";
            std::cout << "Record 0x";
            std::cout << std::setfill('0') << std::setw(4) << std::hex << record->rtti();
            std::cout << " (";
            std::cout << std::dec;
            std::cout << record->rtti() << ") ";
            record->dump(std::cout);
            std::cout << std::endl;
#endif

            if (record->isValid()) {
                if (record->rtti() == BOFRecord::id)
                    handleRecord(record);
                if (!d->handlerStack.empty() && d->handlerStack.back())
                    d->handlerStack.back()->handleRecord(record);
                if (record->rtti() == EOFRecord::id)
                    handleRecord(record);
            }

            delete record;
        }
    }

    free(buffer);
    free(continuePositions);
    delete d->globals;
    delete stream;

    storage.close();

    return true;
}

void ExcelReader::handleRecord(Record* record)
{
    if (!record) return;

    unsigned type = record->rtti();
    if (type == BOFRecord::id)
        handleBOF(static_cast<BOFRecord*>(record));
    else if (type == EOFRecord::id)
        handleEOF(static_cast<EOFRecord*>(record));
}

void ExcelReader::handleBOF(BOFRecord* record)
{
    if (!record) return;

    if (record->type() == BOFRecord::Workbook) {
        d->handlerStack.push_back(d->globals);
    } else if (record->type() == BOFRecord::Worksheet) {
        // find the sheet and make it active
        // which sheet ? look from from previous BoundSheet
        Sheet* sheet = d->globals->sheetFromPosition(record->position());
        if (sheet) d->activeSheet = sheet;
        d->handlerStack.push_back(new WorksheetSubStreamHandler(sheet, d->globals));
    } else if (record->type() == BOFRecord::Chart) {
        SubStreamHandler* parentHandler = d->handlerStack.empty() ? 0 : d->handlerStack.back();
        d->handlerStack.push_back(new Swinder::ChartSubStreamHandler(d->globals, parentHandler));
    } else {
        std::cout << "ExcelReader::handleBOF Unhandled type=" << record->type() << std::endl;
    }
}

void ExcelReader::handleEOF(EOFRecord* record)
{
    if (!record) return;
    if (d->handlerStack.empty()) return;

    SubStreamHandler* handler = d->handlerStack.back();
    d->handlerStack.pop_back();
    if (handler != d->globals) delete handler;
}

MsoDrawingBlibItem::MsoDrawingBlibItem(const PictureReference &picture)
    : m_picture(picture)
{
}

#ifdef SWINDER_XLS2RAW

#include <iostream>
#include <QApplication>

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    if (argc < 2) {
        std::cout << "Usage: sidewinder filename" << std::endl;
        return 0;
    }

    char* filename = argv[1];
    std::cout << "Checking " << filename << std::endl;

    Workbook* workbook = new Workbook();
    ExcelReader* reader = new ExcelReader();
    reader->load(workbook, filename);
    workbook->dumpStats();
    delete reader;
    delete workbook;

    return 0;
}

#endif  // XLS2RAW
