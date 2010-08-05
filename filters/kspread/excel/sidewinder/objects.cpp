/* Swinder - Portable library for spreadsheet
   Copyright (C) 2009 Sebastian Sauer <sebsauer@kdab.com>

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

#include "objects.h"

using namespace Swinder;

const char* DrawingObject::propertyName(Property p)
{
    switch(p) {
        case DrawingObject::pid: return "pid"; break;
        case DrawingObject::itxid: return "itxid"; break;
        case 0x0158: return "cxk"; break;
        case 0x0181: return "fillColor"; break;
        case 0x0183: return "fillBackColor"; break;
        case 0x0185: return "fillCrMod"; break;
        case 0x01bf: return "fillStyleBooleanProperties"; break;
        case 0x01c0: return "lineColor"; break;
        case 0x01c3: return "lineCrMod"; break;
        case 0x0201: return "shadowColor"; break;
        case 0x0203: return "shadowCrMod"; break;
        case 0x023f: return "shadowStyleBooleanProperties"; break;
        case 0x03bf: return "groupShapeBooleanProperties"; break;
        case 0x7f: return "pictureContrast"; break;
        case 0x00bf: return "textBooleanProperties"; break;
        case 0x01ff: return "lineStyleBooleanProperties"; break;
        case 0x0380: return "wzName"; break;
    }
    return "Unknown";
}
    
DrawingObject::DrawingObject() : m_colL(0), m_dxL(0), m_rwT(0), m_dyT(0), m_colR(0), m_dxR(0), m_rwB(0), m_dyB(0), m_gotClientData(false) {}
DrawingObject::~DrawingObject() {}
DrawingObject::DrawingObject(const DrawingObject& other) { *this = other; }

void DrawingObject::operator=(const DrawingObject& other)
{
    m_properties = other.m_properties;
    m_colL = other.m_colL;
    m_dxL = other.m_dxL;
    m_rwT = other.m_rwT;
    m_dyT = other.m_dyT;
    m_colR = other.m_colR;
    m_dxR = other.m_dxR;
    m_rwB = other.m_rwB;
    m_dyB = other.m_dyB;
    m_gotClientData = other.m_gotClientData;
}

void DrawingObject::readHeader(const unsigned char* data, unsigned *recVer, unsigned *recInstance, unsigned *recType, unsigned long *recLen)
{
    const unsigned recVerAndInstance = readU16(data); // 4 bits version and 12 bits number of differentiate atoms in this record
    if(recVer) {
        const unsigned rv = recVerAndInstance;
        *recVer = rv >> 12;
    }
    if(recInstance) {
        const unsigned ri = recVerAndInstance;
        *recInstance = ri >> 4;
    }
    if(recType) {
        *recType = readU16(data + 2);
    }
    if(recLen) {
        *recLen = readU32(data + 4);
    }
}

// read a drawing object (container or atom) and handle/dispatch according to the recType.
unsigned long DrawingObject::handleObject(unsigned size, const unsigned char* data, bool* recordHandled)
{
    unsigned recVer = 0;
    unsigned recInstance = 0;
    unsigned recType = 0;
    unsigned long recLen = 0;
    if(recordHandled) *recordHandled = true;
    readHeader(data, &recVer, &recInstance, &recType, &recLen);
    switch(recType) {
        case 0x0: break; // NOPE
        case 0xF003: // OfficeArtSpgrContainer
        case 0xF004: { // OfficeArtSpContainer
            unsigned long offset = 8;
            while(offset <= recLen) { // recursive
                offset += handleObject(size, data + offset);
            }
        } break;
        case 0xF008: { // OfficeArtFDG
            unsigned long csp = readU32(data + 8);
            unsigned long spid = readU32(data + 12); // MSOSPID, shape-identifier of the last shape in the drawing
            std::cout << "OfficeArtFDG, number of shapes=" << csp << " shapeId of last shape=" << spid << std::endl;
        } break;
        case 0xF009: { // OfficeArtFSPGR
            unsigned long xLeft = readU32(data + 8);
            unsigned long yTop = readU32(data + 12);
            unsigned long xRight = readU32(data + 16);
            unsigned long yBottom = readU32(data + 20);
            std::cout << "OfficeArtFSPGR xLeft=" << xLeft << " yTop=" << yTop << " xRight=" << xRight << " yBottom=" << yBottom << std::endl;
        } break;
        case 0xF00A: { // OfficeArtFSP
            unsigned long spid = readU32(data + 8); // MSOSPID, shape-identifier of the last shape in the drawing
            const unsigned long opts = readU16(data + 12);
            const bool fGroup = opts & 0x01;
            const bool fChild = opts & 0x02;
            const bool fPatriarch = opts & 0x04;
            const bool fDeleted = opts & 0x08;
            const bool fOleShape = opts & 0x10;
            const bool fHaveMaster = opts & 0x20;
            const bool fFlipH = opts & 0x60;
            const bool fFlipV = opts & 0xC0;
            const bool fConnector = opts & 0x180;
            const bool fHaveAnchor = opts & 0x300;
            const bool fBackground = opts & 0x600;
            const bool fHaveSpt = opts & 0xC00;
            std::cout << "OfficeArtFSP, shape-identifier=" << spid << " fGroup=" << fGroup << " fChild=" << fChild << " fPatriarch=" << fPatriarch << " fDeleted=" << fDeleted << " fOleShape=" << fOleShape << " fHaveMaster=" << fHaveMaster << " fFlipH=" << fFlipH << " fFlipV=" << fFlipV << " fConnector=" << fConnector << " fHaveAnchor=" << fHaveAnchor << " fBackground=" << fBackground << " fHaveSpt=" << fHaveSpt << std::endl;
        } break;
        case 0xF11D: // OfficeArtFPSPL
            //printf("OfficeArtFPSPL %i\n",recLen);
            break;
        case 0xF00B: { // OfficeArtFOPT
            printf("OfficeArtFPSPL\n");
            //const unsigned char* startComplexData = data + 8 + recInstance * 6;
            for(uint i = 0; i < recInstance; ++i) {
                const unsigned long opidOpts = readU16(data + 8 + i * 6);
                const unsigned long opid = opidOpts & 0x3FFF;
                const bool fBid = opidOpts & 0x04000; // BLIP identifier?
                const bool fComplex = opidOpts & 0x08000; // Complex property?
                const unsigned long op = readS32(data + 10 + i * 6);
                if(fComplex) { // op specifies the size of the property in the ComplexData
                    //TODO
                } else { // op specifies the value
                    m_properties[opid] = op;
                }
                std::cout << "MsoDrawingRecord: opid=" << opid << " (" << DrawingObject::propertyName((DrawingObject::Property)opid) << ") fBid=" << fBid << " fComplex=" << fComplex << " op=" << op << std::endl;
            }
            std::cout << "MsoDrawingRecord: complexDataLength=" << recLen-(recInstance * 6) << std::endl;
            //TODO read complexData
        } break;
        case 0xF121: // OfficeArtSecondaryFOPT
            std::cout << "OfficeArtSecondaryFOPT" << std::endl;
            break;
        case 0xF122: // OfficeArtTertiaryFOPT
            std::cout << "OfficeArtTertiaryFOPT" << std::endl;
            break;
        case 0xF00F: { // OfficeArtChildAnchor
            unsigned long xLeft = readU32(data + 8);
            unsigned long yTop = readU32(data + 12);
            unsigned long xRight = readU32(data + 16);
            unsigned long yBottom = readU32(data + 20);
            std::cout << "OfficeArtChildAnchor xLeft=" << xLeft << " yTop=" << yTop << " xRight=" << xRight << " yBottom=" << yBottom << std::endl;
        } break;
        case 0xF010: // OfficeArtChildAnchorHF, OfficeArtChildAnchorSheet or OfficeArtChildAnchorChart
            // If this record is in the Worksheet, Macro Sheet, or Dialog Sheet substream, the OfficeArtClientAnchor structure
            // mentioned in [MS-ODRAW] refers to the OfficeArtClientAnchorSheet structure. If this record appears in the Chart
            // Sheet substream, the OfficeArtClientAnchor structure refers to the OfficeArtClientAnchorChart structure.
            switch(recLen) {
                case 8:
                    printf("TODO: OfficeArtChildAnchorHF\n");
                    break;
                case 18: {
                    //const unsigned long opts = readU16(data + 8);
                    //const bool fMove = opts & 0x01;
                    //const bool fSize = opts & 0x02;

                    /*
                    colL (2 bytes): A Col256U that specifies the column of the cell under the top left corner of the bounding rectangle of the shape.
                    dxL (2 bytes): A signed integer that specifies the x coordinate of the top left corner of the bounding rectangle relative to the corner of the underlying cell. The value is expressed as 1024th‘s of that cell‘s width.
                    rwT (2 bytes): A RwU that specifies the row of the cell under the top left corner of the bounding rectangle of the shape.
                    dyT (2 bytes): A signed integer that specifies the y coordinate of the top left corner of the bounding rectangle relative to the corner of the underlying cell. The value is expressed as 1024th‘s of that cell‘s height.
                    colR (2 bytes): A Col256U that specifies the column of the cell under the bottom right corner of the bounding rectangle of the shape.
                    dxR (2 bytes): A signed integer that specifies the x coordinate of the bottom right corner of the bounding rectangle relative to the corner of the underlying cell. The value is expressed as 1024th‘s of that cell‘s width.
                    rwB (2 bytes): A RwU that specifies the row of the cell under the bottom right corner of the bounding rectangle of the shape.
                    dyB (2 bytes): A signed integer that specifies the y coordinate of the bottom right corner of the bounding rectangle relative to the corner of the underlying cell. The value is expressed as 1024th‘s of that cell‘s height.
                    */
                    m_colL = readU16(data + 10);
                    m_dxL = readU16(data + 12);
                    m_rwT = readU16(data + 14);
                    m_dyT = readU16(data + 16);
                    m_colR = readU16(data + 18);
                    m_dxR = readU16(data + 20);
                    m_rwB = readU16(data + 22);
                    m_dyB = readU16(data + 24);
                    std::cout << "OfficeArtChildAnchorSheet or OfficeArtChildAnchorChart colL=" << m_colL << " dxL=" << m_dxL << " rwT=" << m_rwT << " dyT=" << m_dyT << " colR=" << m_colR << " dxR=" << m_dxR << " rwB=" << m_rwB << " dyB=" << m_dyB << " recLen=" << recLen << std::endl;
                } break;
                default:
                    std::cout << "Unhandled OfficeArtChildAnchor type=" << recLen << std::endl;
                    break;
            }
            break;
        case 0xF011: // OfficeArtClientData
            //printf("OfficeArtClientData\n");
            m_gotClientData = true;
            break;
        case 0xF11E: // OfficeArtSplitMenuColorContainer
            printf("OfficeArtSplitMenuColorContainer\n");
            break;
        default:
            std::cout << "DrawingObject: Unhandled record type=" << recType << " size=" << recLen << std::endl;
            if(recordHandled) *recordHandled = false;
            break;
    }
    return 8 + recLen;
}
