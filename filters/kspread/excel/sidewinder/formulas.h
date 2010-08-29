/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003-2005 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006,2009 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
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
   Boston, MA 02110-1301, USA
 */
#ifndef SWINDER_FORMULAS_H
#define SWINDER_FORMULAS_H

#include "value.h"

#include <vector>

#include <QtCore/QRect>
#include <QtCore/QString>

namespace Swinder
{

class FormulaToken
{
public:

    enum {
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
        Union       = 0x10,
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
        AreaErr3d   = 0x3d,

        AttrChoose  = 0x04
    };

    FormulaToken();
    FormulaToken(unsigned id);
    FormulaToken(const FormulaToken&);
    ~FormulaToken();

    // token id, excluding token class
    unsigned id() const;
    const char* idAsString() const;

    // Excel version
    unsigned version() const;
    void setVersion(unsigned version);    // Excel version

    // size of data, EXCLUDING the byte for token id
    unsigned size() const;
    void setData(unsigned size, const unsigned char* data);

    // only when id returns ErrorCode, Bool, Integer, Float, or String
    Value value() const;

    // only when id is Function or FunctionVar
    unsigned functionIndex() const;
    const char* functionName() const;  // for non external function
    unsigned functionParams() const;

    static unsigned functionIndex(const QString& functionName);
    static unsigned functionParams(const QString& functionName);
    static bool fixedFunctionParams(const QString& functionName);

    // only when id is Ref
    QString ref(unsigned row, unsigned col) const;
    // only when id is RefN
    QString refn(unsigned row, unsigned col) const;
    // only when id is Ref3d
    QString ref3d(const std::vector<QString>& externSheets, unsigned row, unsigned col) const;

    // only when id is Array
    QString array(unsigned row, unsigned col) const;

    // only when id is Area or AreaN
    QString area(unsigned row, unsigned col, bool relative = false) const;
    // only when id is Area3d
    QString area3d(const std::vector<QString>& externSheets, unsigned row, unsigned col) const;
    // only when id is Area3d, assumes all references to be absolute
    std::pair<unsigned, QRect> filterArea3d() const;
    // only when id is MemArea
    QString areaMap(unsigned row, unsigned col);

    // only when id is Attr
    unsigned attr() const;

    // only when id is Name
    unsigned long nameIndex() const;
    // only when id is NameX
    unsigned long nameXIndex() const;

    // only when id is Matrix (tExp)
    std::pair<unsigned, unsigned> baseFormulaRecord() const;

    void operator=(const FormulaToken& token);

private:
    class Private;
    Private *d;
};

typedef std::vector<FormulaToken> FormulaTokens;

std::ostream& operator<<(std::ostream& s, FormulaToken token);

class DataTableRecord;
class FormulaToken;
typedef std::vector<FormulaToken> FormulaTokens;

class FormulaDecoder
{
public:
    FormulaDecoder() {}
    virtual ~FormulaDecoder() {}

    FormulaTokens decodeFormula(unsigned size, unsigned pos, const unsigned char* data, unsigned version);

    QString decodeFormula(unsigned row, unsigned col, bool isShared, const FormulaTokens& tokens);
    QString dataTableFormula(unsigned row, unsigned col, const DataTableRecord* record);

    virtual const std::vector<QString>& externSheets() const { return m_externSheets; }
    virtual QString nameFromIndex(unsigned /*index*/) const { return QString(); }
    virtual QString externNameFromIndex(unsigned /*index*/) const { return QString(); }
    virtual FormulaTokens sharedFormulas(const std::pair<unsigned, unsigned>& /*formulaCellPos*/) const { return FormulaTokens(); }
    virtual DataTableRecord* tableRecord(const std::pair<unsigned, unsigned>& /*formulaCellPos*/) const { return 0; }
protected:
    std::vector<QString> m_externSheets;
};

} // namespace Swinder

#endif // SWINDER_FORMULAS_H
