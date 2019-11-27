/* Swinder - Portable library for spreadsheet
   Copyright (C) 2011 Sebastian Sauer <sebsauer@kdab.com>

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

#ifndef SWINDER_XMLTK_H
#define SWINDER_XMLTK_H

#include <QString>
#include <QStringList>

#include "utils.h"

namespace Swinder
{

enum XmlTkTags {
    XmlTkBackWallThickness = 0x0035,
    XmlTkBaseTimeUnitFrt = 0x005F,
    XmlTkColorMappingOverride = 0x0034,
    XmlTkDispBlanksAsFrt = 0x0066,
    XmlTkFloorThicknessFrt = 0x0036,
    XmlTkFormatCodeFrt = 0x0064,
    XmlTkHeightPercent = 0x0065,
    XmlTkLogBaseFrt = 0x0000,
    XmlTkMajorUnitFrt = 0x0053,
    XmlTkMajorUnitTypeFrt = 0x006A,
    XmlTkMaxFrt = 0x0055,
    XmlTkMinFrt = 0x0056,
    XmlTkMinorUnitFrt = 0x0054,
    XmlTkMinorUnitTypeFrt = 0x006B,
    XmlTkNoMultiLvlLbl = 0x002E,
    XmlTkOverlay = 0x002F,
    XmlTkPerspectiveFrt = 0x004D,
    XmlTkPieComboFrom12Frt = 0x005E,
    XmlTkRAngAxOffFrt = 0x0050,
    XmlTkRotXFrt = 0x004E,
    XmlTkRotYFrt = 0x004F,
    XmlTkShowDLblsOverMax = 0x005B,
    XmlTkSpb = 0x001E,
    XmlTkStartSurface = 0x0059,
    XmlTkStyle = 0x0003,
    XmlTkSymbolFrt = 0x0022,
    XmlTkThemeOverride = 0x0033,
    XmlTkTickLabelPositionFrt = 0x005C,
    XmlTkTickLabelSkipFrt = 0x0051,
    XmlTkTickMarkSkipFrt = 0x0052,
    XmlTkTpb = 0x0020
};

QByteArray xmlTkTagName(XmlTkTags tag) {
    switch (tag) {
        case XmlTkBackWallThickness: return "BackWallThickness";
        case XmlTkBaseTimeUnitFrt: return "BaseTimeUnitFrt";
        case XmlTkColorMappingOverride: return "ColorMappingOverride";
        case XmlTkDispBlanksAsFrt: return "DispBlanksAsFrt";
        case XmlTkFloorThicknessFrt: return "FloorThicknessFrt";
        case XmlTkFormatCodeFrt: return "FormatCodeFrt";
        case XmlTkHeightPercent: return "HeightPercent";
        case XmlTkLogBaseFrt: return "LogBaseFrt";
        case XmlTkMajorUnitFrt: return "MajorUnitFrt";
        case XmlTkMajorUnitTypeFrt: return "MajorUnitTypeFrt";
        case XmlTkMaxFrt: return "MaxFrt";
        case XmlTkMinFrt: return "MinFrt";
        case XmlTkMinorUnitFrt: return "MinorUnitFrt";
        case XmlTkMinorUnitTypeFrt: return "MinorUnitTypeFrt";
        case XmlTkNoMultiLvlLbl: return "NoMultiLvlLbl";
        case XmlTkOverlay: return "Overlay";
        case XmlTkPerspectiveFrt: return "PerspectiveFrt";
        case XmlTkPieComboFrom12Frt: return "PieComboFrom12Frt";
        case XmlTkRAngAxOffFrt: return "RAngAxOffFrt";
        case XmlTkRotXFrt: return "RotXFrt";
        case XmlTkRotYFrt: return "RotYFrt";
        case XmlTkShowDLblsOverMax: return "ShowDLblsOverMax";
        case XmlTkSpb: return "Spb";
        case XmlTkStartSurface: return "StartSurface";
        case XmlTkStyle: return "Style";
        case XmlTkSymbolFrt: return "SymbolFrt";
        case XmlTkThemeOverride: return "ThemeOverride";
        case XmlTkTickLabelPositionFrt: return "TickLabelPositionFrt";
        case XmlTkTickLabelSkipFrt: return "TickLabelSkipFrt";
        case XmlTkTickMarkSkipFrt: return "TickMarkSkipFrt";
        case XmlTkTpb: return "Tpb";
    }
    return QByteArray();
}

class XmlTk {
public:
    unsigned m_xmlTkTag;
    virtual QString value() const = 0;
    virtual QString type() const = 0;
    virtual unsigned size() const = 0;
    explicit XmlTk(const unsigned char* data) {
        m_xmlTkTag = readU16(data + 2);
    }
    virtual ~XmlTk() {}
};

class XmlTkBool : public XmlTk {
public:
    bool m_value;
    QString value() const override { return QString::number(m_value); }
    QString type() const override { return "bool"; }
    unsigned size() const override { return 6; }
    explicit XmlTkBool(const unsigned char* data) : XmlTk(data) {
        m_value = readU8(data + 4);
    }
};

class XmlTkDouble : public XmlTk {
public:
    double m_value;
    QString value() const override { return QString::number(m_value); }
    QString type() const override { return "double"; }
    unsigned size() const override { return 12; }
    explicit XmlTkDouble(const unsigned char* data) : XmlTk(data) {
        m_value = readFloat64(data + 4);
    }
};

class XmlTkDWord : public XmlTk {
public:
    int m_value;
    QString value() const override { return QString::number(m_value); }
    QString type() const override { return "dword"; }
    unsigned size() const override { return 8; }
    explicit XmlTkDWord(const unsigned char* data) : XmlTk(data) {
        m_value = readS32(data + 4);
    }
};

class XmlTkString : public XmlTk {
public:
    QString m_value;
    QString value() const override { return m_value; }
    QString type() const override { return "string"; }
    unsigned size() const override { return 8 + m_size; }
    explicit XmlTkString(const unsigned char* data) : XmlTk(data) {
        m_cchValue = readU32(data + 4);
        m_value = readUnicodeChars(data + 8, m_cchValue, -1, 0, &m_size);
    }
private:
    unsigned m_cchValue;
    unsigned m_size;
};

class XmlTkToken : public XmlTk {
public:
    unsigned m_value;
    QString value() const override { return QString::number(m_value); }
    QString type() const override { return "token"; }
    unsigned size() const override { return 6; }
    explicit XmlTkToken(const unsigned char* data) : XmlTk(data) {
        m_value = readU16(data + 4);
    }
};

class XmlTkBlob : public XmlTk {
public:
    QList<XmlTk*> m_tokens;
    QString value() const override {
        QStringList tokens;
        foreach(XmlTk* t, m_tokens)
            tokens.append( QString("%1 %2 %3").arg(t->m_xmlTkTag, 0, 16).arg(t->type()).arg(t->value()) );
        return QString("[%1]").arg(tokens.join(", "));
    }
    QString type() const override { return "blob"; }
    unsigned size() const override { return 8 + m_cbBlob; }
    explicit XmlTkBlob(const unsigned char* data);
    ~XmlTkBlob() override { qDeleteAll(m_tokens); }
private:
    unsigned m_cbBlob;
};

class XmlTkBegin : public XmlTk {
public:
    QString value() const override { return QString(); }
    QString type() const override { return "begin"; }
    unsigned size() const override { return 4; }
    explicit XmlTkBegin(const unsigned char* data) : XmlTk(data) {}
};

class XmlTkEnd : public XmlTk {
public:
    QString value() const override { return QString(); }
    QString type() const override { return "end"; }
    unsigned size() const override { return 4; }
    explicit XmlTkEnd(const unsigned char* data) : XmlTk(data) {}
};

XmlTk* parseXmlTk(const unsigned char* data) {
    unsigned drType = readU8(data);
    switch (drType) {
        case 0x00:
            return new XmlTkBegin(data);
        case 0x01:
            return new XmlTkEnd(data);
        case 0x02:
            return new XmlTkBool(data);
        case 0x03:
            return new XmlTkDouble(data);
        case 0x04:
            return new XmlTkDWord(data);
        case 0x05:
            return new XmlTkString(data);
        case 0x06:
            return new XmlTkToken(data);
        case 0x07:
            return new XmlTkBlob(data);
        default:
            qCDebug(lcSidewinder) << "Error in " << __FUNCTION__ << ": Unhandled drType " << QString::number(drType, 16);
            break;
    }
    return 0;
}

QList<XmlTk*> parseXmlTkChain(const unsigned char* data, int size) {
    QList<XmlTk*> tokens;
    for (int p = 0; p + 4 < size;) {
        XmlTk *t = parseXmlTk(data + p);
        if (!t) break;
        tokens.append(t);
        p += t->size();
    }
    return tokens;
}

XmlTkBlob::XmlTkBlob(const unsigned char* data) : XmlTk(data) {
    m_cbBlob = readU32(data + 4);
    parseXmlTkChain(data + 8, m_cbBlob);
}

} // namespace Swinder

#endif // SWINDER_CHARTSUBSTREAMHANDLER_H
 
