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
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

DESCRIPTION

    This is based on code originally written by Stefan Taferner
    (taferner@kde.org) and also borrows from libwmf (by Martin Vermeer and
    Caolan McNamara).
*/

#include <kdebug.h>
#include <math.h>
#include <qdatastream.h>
#include <qfile.h>
#include <qpointarray.h>
#include <wmf.h>

#define PI (3.14159265358979323846)

Wmf::Wmf(
    unsigned dpi)
{
    m_dpi = dpi;
    m_objectHandles = new WinObjHandle*[s_maxHandles];
}

Wmf::~Wmf()
{
    delete[] m_objectHandles;
}

//
//
//

void Wmf::brushSet(
    unsigned colour,
    unsigned style)
{
    m_dc.m_brushColour = colour;
    m_dc.m_brushStyle = style;
}

//
//
//

unsigned short Wmf::calcCheckSum(
    WmfPlaceableHeader *pheader)
{
    S16 result = 0;
    S16 *ptr = (S16 *)pheader;

    // XOR in each of the S16s.

    for (unsigned i = 0; i < sizeof(WmfPlaceableHeader)/sizeof(S16); i++)
    {
        result ^= ptr[i];
    }
    return result;
}

//-----------------------------------------------------------------------------
unsigned Wmf::getColour(
    S32 colour)
{
    unsigned red, green, blue;

    red = colour & 255;
    green = (colour >> 8) & 255;
    blue = (colour >> 16) & 255;
    return (red << 16) + (green << 8) + blue;
}

void Wmf::genericArc(
    QString type,
    QDataStream &operands)
{
    QPoint topLeft;
    QPoint bottomRight;
    QPoint start;
    QPoint end;

    topLeft = normalisePoint(operands);
    bottomRight = normalisePoint(operands);
    start = normalisePoint(operands);
    end = normalisePoint(operands);

    // WMF defines arcs with the major and minor axes of an ellipse, and two points.
    // From each point draw a line to the center of the ellipse: the intercepts define
    // the ends of the arc.

    QRect ellipse(topLeft, bottomRight);
    QPoint centre = ellipse.center();
    double startAngle = atan2(centre.y() - start.y(), centre.x() - start.x());
    double stopAngle = atan2(centre.y() - end.y(), centre.x() - end.x());

    startAngle = 180 * startAngle / PI;
    stopAngle = 180 * stopAngle / PI;

    gotEllipse(m_dc, type, centre, ellipse.size() / 2, startAngle, stopAngle);
}

int Wmf::handleIndex(void) const
{
    int i;

    for (i = 0; i < s_maxHandles; i++)
    {
        if (!m_objectHandles[i])
            return i;
    }
    kdError(s_area) << "handle table full !" << endl;
    return -1;
}

//-----------------------------------------------------------------------------
Wmf::WinObjPenHandle *Wmf::handleCreatePen(void)
{
    WinObjPenHandle *handle = new WinObjPenHandle;
    int idx = handleIndex();

    if (idx >= 0)
        m_objectHandles[idx] = handle;
    return handle;
}

//-----------------------------------------------------------------------------
Wmf::WinObjBrushHandle *Wmf::handleCreateBrush(void)
{
    WinObjBrushHandle *handle = new WinObjBrushHandle;
    int idx = handleIndex();

    if (idx >= 0)
        m_objectHandles[idx] = handle;
    return handle;
}

//-----------------------------------------------------------------------------
void Wmf::handleDelete(int idx)
{
    if (idx >= 0 && idx < s_maxHandles && m_objectHandles[idx])
    {
        delete m_objectHandles[idx];
        m_objectHandles[idx] = NULL;
    }
}

//
//
//

void Wmf::invokeHandler(
    S16 opcode,
    U32 wordOperands,
    QDataStream &operands)
{
    typedef void (Wmf::*method)(U32 wordOperands, QDataStream &operands);

    typedef struct
    {
        const char *name;
        unsigned short opcode;
        method handler;
    } opcodeEntry;

    static const opcodeEntry funcTab[] =
    {
        { "ANIMATEPALETTE",       0x0436, 0 },
        { "ARC",                  0x0817, &Wmf::opArc },
        { "BITBLT",               0x0922, 0 },
        { "CHORD",                0x0830, 0 },
        { "CREATEBRUSHINDIRECT",  0x02FC, &Wmf::opBrushCreateIndirect },
        { "CREATEFONTINDIRECT",   0x02FB, 0 },
        { "CREATEPALETTE",        0x00F7, 0 },
        { "CREATEPATTERNBRUSH",   0x01F9, 0 },
        { "CREATEPENINDIRECT",    0x02FA, &Wmf::opPenCreateIndirect },
        { "CREATEREGION",         0x06FF, 0 },
        { "DELETEOBJECT",         0x01F0, &Wmf::opObjectDelete },
        { "DIBBITBLT",            0x0940, 0 },
        { "DIBCREATEPATTERNBRUSH",0x0142, 0 },
        { "DIBSTRETCHBLT",        0x0b41, 0 },
        { "ELLIPSE",              0x0418, &Wmf::opEllipse },
        { "ESCAPE",               0x0626, &Wmf::opNoop },
        { "EXCLUDECLIPRECT",      0x0415, 0 },
        { "EXTFLOODFILL",         0x0548, 0 },
        { "EXTTEXTOUT",           0x0a32, 0 },
        { "FILLREGION",           0x0228, 0 },
        { "FLOODFILL",            0x0419, 0 },
        { "FRAMEREGION",          0x0429, 0 },
        { "INTERSECTCLIPRECT",    0x0416, 0 },
        { "INVERTREGION",         0x012A, 0 },
        { "LINETO",               0x0213, &Wmf::opLineTo },
        { "MOVETO",               0x0214, &Wmf::opMoveTo },
        { "OFFSETCLIPRGN",        0x0220, 0 },
        { "OFFSETVIEWPORTORG",    0x0211, 0 },
        { "OFFSETWINDOWORG",      0x020F, 0 },
        { "PAINTREGION",          0x012B, 0 },
        { "PATBLT",               0x061D, 0 },
        { "PIE",                  0x081A, &Wmf::opPie },
        { "POLYGON",              0x0324, &Wmf::opPolygon },
        { "POLYLINE",             0x0325, &Wmf::opPolyline },
        { "POLYPOLYGON",          0x0538, 0 },
        { "REALIZEPALETTE",       0x0035, 0 },
        { "RECTANGLE",            0x041B, &Wmf::opRectangle },
        { "RESIZEPALETTE",        0x0139, 0 },
        { "RESTOREDC",            0x0127, &Wmf::opRestoreDc },
        { "ROUNDRECT",            0x061C, 0 },
        { "SAVEDC",               0x001E, &Wmf::opSaveDc },
        { "SCALEVIEWPORTEXT",     0x0412, 0 },
        { "SCALEWINDOWEXT",       0x0410, 0 },
        { "SELECTCLIPREGION",     0x012C, 0 },
        { "SELECTOBJECT",         0x012D, &Wmf::opObjectSelect },
        { "SELECTPALETTE",        0x0234, 0 },
        { "SETBKCOLOR",           0x0201, 0 },
        { "SETBKMODE",            0x0102, 0 },
        { "SETDIBTODEV",          0x0d33, 0 },
        { "SETMAPMODE",           0x0103, 0 },
        { "SETMAPPERFLAGS",       0x0231, 0 },
        { "SETPALENTRIES",        0x0037, 0 },
        { "SETPIXEL",             0x041F, 0 },
        { "SETPOLYFILLMODE",      0x0106, &Wmf::opPolygonSetFillMode },
        { "SETRELABS",            0x0105, 0 },
        { "SETROP2",              0x0104, 0 },
        { "SETSTRETCHBLTMODE",    0x0107, 0 },
        { "SETTEXTALIGN",         0x012E, 0 },
        { "SETTEXTCHAREXTRA",     0x0108, 0 },
        { "SETTEXTCOLOR",         0x0209, 0 },
        { "SETTEXTJUSTIFICATION", 0x020A, 0 },
        { "SETVIEWPORTEXT",       0x020E, 0 },
        { "SETVIEWPORTORG",       0x020D, 0 },
        { "SETWINDOWEXT",         0x020C, &Wmf::opWindowSetExt },
        { "SETWINDOWORG",         0x020B, &Wmf::opWindowSetOrg },
        { "STRETCHBLT",           0x0B23, 0 },
        { "STRETCHDIB",           0x0f43, 0 },
        { "TEXTOUT",              0x0521, 0 },
        { NULL,                   0,      0 }
    };
    unsigned i;
    method result;

    // Scan lookup table for operation.

    for (i = 0; funcTab[i].name; i++)
    {
        if (funcTab[i].opcode == opcode)
        {
            break;
        }
    }

    // Invoke handler.

    result = funcTab[i].handler;
    if (!result)
    {
        if (funcTab[i].name)
            kdError(s_area) << "invokeHandler: unsupported opcode: " <<
                funcTab[i].name <<
                " operands: " << wordOperands << endl;
        else
            kdError(s_area) << "invokeHandler: unsupported opcode: 0x" <<
                QString::number(opcode, 16) <<
                " operands: " << wordOperands << endl;

        // Skip data we cannot use.

        for (i = 0; i < wordOperands; i++)
        {
            S16 discard;

            operands >> discard;
        }
    }
    else
    {
        kdDebug(s_area) << "invokeHandler: opcode: " << funcTab[i].name <<
            " operands: " << wordOperands << endl;
        (this->*result)(wordOperands, operands);
    }
}

QPoint Wmf::normalisePoint(
    QDataStream &operands)
{
    S16 x;
    S16 y;

    operands >> x >> y;
    return QPoint((x - m_windowOrgX) * m_windowFlipX / m_dpi, (y - m_windowOrgY) * m_windowFlipY / m_dpi);
}

QSize Wmf::normaliseSize(
    QDataStream &operands)
{
    S16 width;
    S16 height;

    operands >> width >> height;
    return QSize(width / m_dpi, height / m_dpi);
}

//
//
//

bool Wmf::parse(
    const QString &file)
{
    bool isPlaceable;
    bool isEnhanced;

    QFile in(file);
    if (!in.open(IO_ReadOnly))
    {
        kdError(s_area) << "Unable to open input file!" << endl;
        in.close();
        return false;
    }
    for (int i = 0; i < s_maxHandles; i++)
        m_objectHandles[i] = NULL;

    QDataStream st(&in);
    st.setByteOrder(QDataStream::LittleEndian); // Great, I love Qt !
    WmfPlaceableHeader pheader;
    WmfEnhMetaHeader eheader;
    WmfMetaHeader header;
    S16 checksum;
    int filePos;

    //----- Read placeable metafile header

    st >> pheader.key;
    isPlaceable = (pheader.key == (S32)APMHEADER_KEY);
    if (isPlaceable)
    {
        st >> pheader.hmf;
        st >> pheader.bbox.left;
        st >> pheader.bbox.top;
        st >> pheader.bbox.right;
        st >> pheader.bbox.bottom;
        st >> pheader.inch;
        st >> pheader.reserved;
        st >> pheader.checksum;
        checksum = calcCheckSum(&pheader);
        if (pheader.checksum != checksum)
            isPlaceable = false;
        m_dpi = (unsigned)((double)pheader.inch / m_dpi);
        m_windowOrgX = pheader.bbox.left;
        m_windowOrgY = pheader.bbox.top;
        if (pheader.bbox.right > pheader.bbox.left)
            m_windowFlipX = 1;
        else
            m_windowFlipX = -1;
        if (pheader.bbox.bottom > pheader.bbox.top)
            m_windowFlipY = 1;
        else
            m_windowFlipY = -1;
    }
    else
    {
        m_dpi = (unsigned)((double)576 / m_dpi);
        in.at(0);
        m_windowOrgX = 0;
        m_windowOrgY = 0;
        m_windowFlipX = 1;
        m_windowFlipY = 1;
    }

    //----- Read as enhanced metafile header

    filePos = in.at();
    st >> eheader.iType;
    st >> eheader.nSize;
    st >> eheader.rclBounds.left;
    st >> eheader.rclBounds.top;
    st >> eheader.rclBounds.right;
    st >> eheader.rclBounds.bottom;
    st >> eheader.rclFrame.left;
    st >> eheader.rclFrame.top;
    st >> eheader.rclFrame.right;
    st >> eheader.rclFrame.bottom;
    st >> eheader.dSignature;
    isEnhanced = (eheader.dSignature == ENHMETA_SIGNATURE);
    if (isEnhanced) // is it really enhanced ?
    {
        st >> eheader.nVersion;
        st >> eheader.nBytes;
        st >> eheader.nRecords;
        st >> eheader.nHandles;
        st >> eheader.sReserved;
        st >> eheader.nDescription;
        st >> eheader.offDescription;
        st >> eheader.nPalEntries;
        st >> eheader.szlDevice;
        st >> eheader.szlMillimeters;

        kdError(s_area) << "WMF Extended Header NOT YET IMPLEMENTED, SORRY.";
        /*
        if (mSingleStep)
        {
            debug("  iType=%d", eheader.iType);
            debug("  nSize=%d", eheader.nSize);
            debug("  rclBounds=(%ld;%ld;%ld;%ld)",
                    eheader.rclBounds.left, eheader.rclBounds.top,
                    eheader.rclBounds.right, eheader.rclBounds.bottom);
            debug("  rclFrame=(%ld;%ld;%ld;%ld)",
                    eheader.rclFrame.left, eheader.rclFrame.top,
                    eheader.rclFrame.right, eheader.rclFrame.bottom);
            debug("  dSignature=%d", eheader.dSignature);
            debug("  nVersion=%d", eheader.nVersion);
            debug("  nBytes=%d", eheader.nBytes);
        }
        debug("NOT YET IMPLEMENTED, SORRY.");
        */
        return false;
    }
    else // no, not enhanced
    {
        //    debug("WMF Header");
        //----- Read as enhanced metafile header
        in.at(filePos);
        st >> header.mtType;
        st >> header.mtHeaderSize;
        st >> header.mtVersion;
        st >> header.mtSize;
        st >> header.mtNoObjects;
        st >> header.mtMaxRecord;
        st >> header.mtNoParameters;
        /*
        if (mSingleStep)
        {
            debug("  mtType=%u", header.mtType);
            debug("  mtHeaderSize=%u", header.mtHeaderSize);
            debug("  mtVersion=%u", header.mtVersion);
            debug("  mtSize=%ld", header.mtSize);
        }
        */
    }

    //----- Read bits

    while (!st.eof())
    {
        S32 wordCount;
        S16 opcode;

        st >> wordCount;
        st >> opcode;
        if (opcode == 0)
            break;

        // Package the arguments...

        wordCount -= 3;
        invokeHandler(opcode, wordCount, st);
    }
    in.close();
    return true;
}

void Wmf::opArc(
    U32 /*wordOperands*/,
    QDataStream &operands)
{
    genericArc("arc", operands);
}

void Wmf::opBrushCreateIndirect(
    U32 /*wordOperands*/,
    QDataStream &operands)
{
    static Qt::BrushStyle hatchedStyleTab[] =
    {
        Qt::HorPattern,
        Qt::FDiagPattern,
        Qt::BDiagPattern,
        Qt::CrossPattern,
        Qt::DiagCrossPattern
    };
    static Qt::BrushStyle styleTab[] =
    {
        Qt::SolidPattern,
        Qt::NoBrush,
        Qt::FDiagPattern,   // hatched
        Qt::Dense4Pattern,  // should be custom bitmap pattern
        Qt::HorPattern,     // should be BS_INDEXED (?)
        Qt::VerPattern,     // should be device-independend bitmap
        Qt::Dense6Pattern,  // should be device-independend packed-bitmap
        Qt::Dense2Pattern,  // should be BS_PATTERN8x8
        Qt::Dense3Pattern   // should be device-independend BS_DIBPATTERN8x8
    };
    Qt::BrushStyle style;
    WinObjBrushHandle *handle = handleCreateBrush();
    S16 arg;
    S32 colour;
    S16 discard;

    operands >> arg >> colour;
    handle->m_colour = getColour(colour);
    if (arg == 2)
    {
        operands >> arg;
        if (arg >= 0 && arg < 6)
        {
            style = hatchedStyleTab[arg];
        }
        else
        {
            kdError(s_area) << "createBrushIndirect: invalid hatched brush " << arg << endl;
            style = Qt::SolidPattern;
        }
    }
    else
    if (arg >= 0 && arg < 9)
    {
        style = styleTab[arg];
        operands >> discard;
    }
    else
    {
        kdError(s_area) << "createBrushIndirect: invalid brush " << arg << endl;
        style = Qt::SolidPattern;
        operands >> discard;
    }
    handle->m_style = style;
}

void Wmf::opEllipse(
    U32 /*wordOperands*/,
    QDataStream &operands)
{
    QPoint topLeft;
    QPoint bottomRight;

    topLeft = normalisePoint(operands);
    bottomRight = normalisePoint(operands);

    QRect ellipse(topLeft, bottomRight);

    gotEllipse(m_dc, "full", ellipse.center(), ellipse.size() / 2, 0, 0);
}

void Wmf::opLineTo(
    U32 /*wordOperands*/,
    QDataStream &operands)
{
    QPoint lineTo;

    lineTo = normalisePoint(operands);
    QPointArray points(2);
    points.setPoint(0, m_lineFrom);
    points.setPoint(1, lineTo);
    gotPolyline(m_dc, points);

    // Remember this point for next time.

    m_lineFrom = lineTo;
}

void Wmf::opMoveTo(
    U32 /*wordOperands*/,
    QDataStream &operands)
{
    m_lineFrom = normalisePoint(operands);
}

void Wmf::opNoop(
    U32 wordOperands,
    QDataStream &operands)
{
    skip(wordOperands, operands);
}

//-----------------------------------------------------------------------------
void Wmf::opObjectDelete(
    U32 /*wordOperands*/,
    QDataStream &operands)
{
    S16 idx;

    operands >> idx;
    handleDelete(idx);
}

//-----------------------------------------------------------------------------
void Wmf::opObjectSelect(
    U32 /*wordOperands*/,
    QDataStream &operands)
{
    S16 idx;

    operands >> idx;
    if (idx >= 0 && idx < s_maxHandles && m_objectHandles[idx])
        m_objectHandles[idx]->apply(*this);
}

//
//
//

void Wmf::opPenCreateIndirect(
    U32 /*wordOperands*/,
    QDataStream &operands)
{
    static Qt::PenStyle styleTab[] =
    {
        Qt::SolidLine,
        Qt::DashLine,
        Qt::DotLine,
        Qt::DashDotLine,
        Qt::DashDotDotLine,
        Qt::NoPen,
        Qt::SolidLine,  // PS_INSIDEFRAME
        Qt::SolidLine,  // PS_USERSTYLE
        Qt::SolidLine   // PS_ALTERNATE
    };
    WinObjPenHandle *handle = handleCreatePen();
    S16 arg;
    S32 colour;

    operands >> arg;
    if (arg >= 0 && arg < 8)
    {
        handle->m_style = styleTab[arg];
    }
    else
    {
        kdError(s_area) << "createPenIndirect: invalid pen " << arg << endl;
        handle->m_style = Qt::SolidLine;
    }
    operands >> arg;
    handle->m_width = arg;
    operands >> arg >> colour;
    handle->m_colour = getColour(colour);
}

void Wmf::opPie(
    U32 /*wordOperands*/,
    QDataStream &operands)
{
    genericArc("pie", operands);
}

void Wmf::opPolygonSetFillMode(
    U32 /*wordOperands*/,
    QDataStream &operands)
{
    S16 tmp;

    operands >> tmp;
    m_dc.m_winding = tmp != 0;
}

void Wmf::opPolygon(
    U32 /*wordOperands*/,
    QDataStream &operands)
{
    S16 tmp;

    operands >> tmp;
    QPointArray points(tmp);

    for (int i = 0; i < tmp; i++)
    {
        points.setPoint(i, normalisePoint(operands));
    }
    gotPolygon(m_dc, points);
}

void Wmf::opPolyline(
    U32 /*wordOperands*/,
    QDataStream &operands)
{
    S16 tmp;

    operands >> tmp;
    QPointArray points(tmp);

    for (int i = 0; i < tmp; i++)
    {
        points.setPoint(i, normalisePoint(operands));
    }
    gotPolyline(m_dc, points);
}

void Wmf::opRectangle(
    U32 /*wordOperands*/,
    QDataStream &operands)
{
    QPoint topLeft;
    QSize size;

    topLeft = normalisePoint(operands);
    size = normaliseSize(operands);
    QRect rect(topLeft, size);
    QPointArray points(4);

    points.setPoint(0, topLeft);
    points.setPoint(1, rect.topRight());
    points.setPoint(2, rect.bottomRight());
    points.setPoint(3, rect.bottomLeft());
    gotRectangle(m_dc, points);
}

void Wmf::opRestoreDc(
    U32 /*wordOperands*/,
    QDataStream &operands)
{
    S16 pop;
    S16 i;

    operands >> pop;
    for (i = 0; i < pop; i++)
    {
        m_dc = m_savedDcs.pop();
    }
}

void Wmf::opSaveDc(
    U32 /*wordOperands*/,
    QDataStream &/*operands*/)
{
    m_savedDcs.push(m_dc);

    // TBD: reinitialise m_dc.
}

void Wmf::opWindowSetOrg(
    U32 /*wordOperands*/,
    QDataStream &operands)
{
    S16 top;
    S16 left;

    operands >> top >> left;
    m_windowOrgX = left;
    m_windowOrgY = top;
}

void Wmf::opWindowSetExt(
    U32 /*wordOperands*/,
    QDataStream &operands)
{
    S16 height;
    S16 width;

    operands >> height >> width;
    if (width > 0)
        m_windowFlipX = 1;
    else
        m_windowFlipX = -1;
    if (height > 0)
        m_windowFlipY = 1;
    else
        m_windowFlipY = -1;
}

void Wmf::penSet(
    unsigned colour,
    unsigned style,
    unsigned width)
{
    m_dc.m_penColour = colour;
    m_dc.m_penStyle = style;
    m_dc.m_penWidth = width;
}

void Wmf::skip(
    U32 wordOperands,
    QDataStream &operands)
{
    if ((int)wordOperands < 0)
    {
        kdError(s_area) << "skip: " << (int)wordOperands << endl;
        return;
    }
    if (wordOperands)
    {
        U32 i;
        S16 discard;

        kdDebug(s_area) << "skip: " << wordOperands << endl;
        for (i = 0; i < wordOperands; i++)
        {
            operands >> discard;
        }
    }
}

Wmf::DrawContext::DrawContext()
{
    // TBD: initalise with proper values.
    m_brushColour = 0x808080;
    m_brushStyle = 1;
    m_penColour = 0x808080;
    m_penStyle = 1;
    m_penWidth = 1;
}

void Wmf::WinObjBrushHandle::apply(
    Wmf &p)
{
    p.brushSet(m_colour, m_style);
}

void Wmf::WinObjPenHandle::apply(
    Wmf &p)
{
    p.penSet(m_colour, m_style, m_width);
}
