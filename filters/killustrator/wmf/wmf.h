/*
    Copyright (C) 2000, S.R.Haque <shaheedhaque@hotmail.com>.
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aS32 with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

DESCRIPTION

    This is a generic parser for Windows MetaFiles (WMFs). The output is
    a series of callbacks (a.k.a. virtual functions) which the caller can
    override as required.

    This is based on code originally written by Stefan Taferner
    <taferner@kde.org>.
*/

#ifndef WMF_H
#define WMF_H

#include <qdatastream.h>
#include <qpointarray.h>
#include <qrect.h>

class Wmf
{
public:

    // Construction.

    Wmf(
        unsigned dpi);
    virtual ~Wmf();

    // Called to parse the given file.

    bool parse(
        const QString &file);

    // Should be protected...

    void brushSet(
        unsigned colour,
        unsigned style);
    void penSet(
        unsigned colour,
        unsigned style,
        unsigned width);

protected:
    // Override to get results of parsing.

    virtual void gotPolygon(
        unsigned penColour,
        unsigned penStyle,
        unsigned penWidth,
        unsigned brushColour,
        unsigned brushStyle,
        const QPointArray &points) = 0;
    virtual void gotPolyline(
        unsigned penColour,
        unsigned penStyle,
        unsigned penWidth,
        const QPointArray &points) = 0;

private:
    // Debug support.

    static const int s_area = 30504;

    // Use unambiguous names for Microsoft types.

    typedef short S16;
    typedef int S32;

    typedef struct _RECT
    {
        S16 left;
        S16 top;
        S16 right;
        S16 bottom;
    } RECT;

    typedef struct _RECTL
    {
        S32 left;
        S32 top;
        S32 right;
        S32 bottom;
    } RECTL;

    struct WmfEnhMetaHeader
    {
        S32 iType;                  // Record type EMR_HEADER
        S32 nSize;                  // Record size in bytes.  This may be greater
                                    // than the sizeof(ENHMETAHEADER).
        RECTL rclBounds;            // Inclusive-inclusive bounds in device units
        RECTL rclFrame;             // Inclusive-inclusive Picture Frame of metafile
                                    // in .01 mm units
        S32 dSignature;             // Signature.  Must be ENHMETA_SIGNATURE.
        S32 nVersion;               // Version number
        S32 nBytes;                 // Size of the metafile in bytes
        S32 nRecords;               // Number of records in the metafile
        S16 nHandles;               // Number of handles in the handle table
                                    // Handle index zero is reserved.
        S16 sReserved;              // Reserved.  Must be zero.
        S32 nDescription;           // Number of chars in the unicode description string
                                    // This is 0 if there is no description string
        S32 offDescription;         // Offset to the metafile description record.
                                    // This is 0 if there is no description string
        S32 nPalEntries;            // Number of entries in the metafile palette.
        S32 szlDevice;              // Size of the reference device in pels
        S32 szlMillimeters;         // Size of the reference device in millimeters
    };
    #define ENHMETA_SIGNATURE       0x464D4520

    struct WmfMetaHeader
    {
        S16 mtType;
        S16 mtHeaderSize;
        S16 mtVersion;
        S32 mtSize;
        S16 mtNoObjects;
        S32 mtMaxRecord;
        S16 mtNoParameters;
    };

    struct WmfPlaceableHeader
    {
        S32 key;
        S16 hmf;
        RECT bbox;
        S16 inch;
        S32 reserved;
        S16 checksum;
    };
    #define APMHEADER_KEY 0x9AC6CDD7L

/*
    struct WmfMetaRecord
    {
        S32 rdSize;                 // Record size (in words) of the function
        S16 rdFunction;             // Record function number
        S16 rdParm[1];              // WORD array of parameters
    };

    struct WmfEnhMetaRecord
    {
        S32 iType;                  // Record type EMR_xxx
        S32 nSize;                  // Record size in bytes
        S32 dParm[1];               // DWORD array of parameters
    };
*/

    bool m_isPlaceable;
    bool m_isEnhanced;
    bool m_winding;
    QRect m_boundingBox;
    QRect m_windowBox;
    unsigned m_brushColour;
    unsigned m_brushStyle;
    unsigned m_penColour;
    unsigned m_penStyle;
    unsigned m_penWidth;
    bool m_useWorldMatrix;
    bool m_calcBBox;
    int m_dpi;

    // Windows handle management.

    class WinObjHandle
    {
    public:
        virtual ~WinObjHandle () {}
        virtual void apply(Wmf &p) = 0;
    };

    class WinObjBrushHandle: public WinObjHandle
    {
    public:
        virtual void apply(Wmf &p);
        unsigned m_colour;
        unsigned m_style;
    };

    class WinObjPenHandle: public WinObjHandle
    {
    public:
        virtual void apply(Wmf &p);
        unsigned m_colour;
        unsigned m_style;
        unsigned m_width;
    };

    int handleIndex(void) const;
    WinObjPenHandle *handleCreatePen(void);
    WinObjBrushHandle *handleCreateBrush(void);
    void handleDelete(int idx);
    static const int s_maxHandles = 64;
    WinObjHandle **m_objectHandles;

    unsigned getColour(S32 colour);

    unsigned short calcCheckSum(
        WmfPlaceableHeader *pheader);

    // Opcode handling and Metafile painter methods.

    void invokeHandler(
        S16 opcode,
        S32 wordOperands,
        QDataStream &operands);
/*
    // draw multiple polygons
    void polypolygon(S32 wordOperands, QDataStream &operands);
*/
    // create a logical brush
    void brushCreateIndirect(S32 wordOperands, QDataStream &operands);
    // create a logical pen
    void penCreateIndirect(S32 wordOperands, QDataStream &operands);

    // Free object handle
    void objectDelete(S32 wordOperands, QDataStream &operands);
    // Activate object handle
    void objectSelect(S32 wordOperands, QDataStream &operands);

    // draw polygon
    void polygon(S32 wordOperands, QDataStream &operands);
    // set polygon fill mode
    void polygonSetFillMode(S32 wordOperands, QDataStream &operands);

    // draw series of lines
    void polyline(S32 wordOperands, QDataStream &operands);
    // set window origin
    void windowSetOrg(S32 wordOperands, QDataStream &operands);
    // set window extents
    void windowSetExt(S32 wordOperands, QDataStream &operands);
/*
    // set background pen color
    void setBkColor(S32 wordOperands, QDataStream &operands);
    // set background pen mode
    void setBkMode(S32 wordOperands, QDataStream &operands);
    // draw line to coord
    void lineTo(S32 wordOperands, QDataStream &operands);
    // move pen to coord
    void moveTo(S32 wordOperands, QDataStream &operands);
    // draw ellipse
    void ellipse(S32 wordOperands, QDataStream &operands);
    // Set raster operation mode
    void setRop(S32 wordOperands, QDataStream &operands);
    // Escape (enhanced command set)
    void escape(S32 wordOperands, QDataStream &operands);
    // save drawing context
    void saveDC(S32 wordOperands, QDataStream &operands);
    // restore drawing context
    void restoreDC(S32 wordOperands, QDataStream &operands);
*/
    // do nothing
    void noop(S32 /*wordOperands*/, QDataStream &/*operands*/) {}

};

#endif
