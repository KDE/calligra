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
    aU32 with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

DESCRIPTION

    This is a generic parser for Microsoft Office Drawings (MSODs). The
    specification for this is the Microsoft Office 97 Drawing File Format
    published in MSDN. The output is a series of callbacks (a.k.a. virtual
    functions) which the caller can override as required.
*/

#ifndef MSOD_H
#define MSOD_H

class QString;
class QPointArray;
#include <qvector.h>

class Msod
{
public:

    // Construction.

    Msod(
        unsigned dpi);
    virtual ~Msod();

    // Called to parse the given file. We extract a drawing by shapeId.
    // If the drawing is not found, the return value will be false.

    bool parse(
        unsigned shapeId,
        const QString &fileIn,
        const char *delayStream);

    class DrawContext
    {
    public:
        DrawContext();
        bool m_winding;
        unsigned m_brushColour;
        unsigned m_brushStyle;
        unsigned m_penColour;
        unsigned m_penStyle;
        unsigned m_penWidth;
    };

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

    virtual void gotEllipse(
        const DrawContext &dc,
        QString type,
        QPoint topLeft,
        QSize halfAxes,
        unsigned startAngle,
        unsigned stopAngle) = 0;
    virtual void gotPicture(
        unsigned id,
        QString extension,
        unsigned length,
        const char *data) = 0;
    virtual void gotPolygon(
        const DrawContext &dc,
        const QPointArray &points) = 0;
    virtual void gotPolyline(
        const DrawContext &dc,
        const QPointArray &points) = 0;
    virtual void gotRectangle(
        const DrawContext &dc,
        const QPointArray &points) = 0;

private:
    // Debug support.

    static const int s_area = 30505;

    // Use unambiguous names for Microsoft types.

    typedef unsigned char U8;
    typedef unsigned short U16;
    typedef unsigned int U32;

    int m_dpi;
    DrawContext m_dc;
    unsigned m_dggError;
    unsigned m_requestedShapeId;
    bool m_isRequiredDrawing;
    const char *m_delayStream;

    QPoint normalisePoint(
        QDataStream &operands);
    QSize normaliseSize(
        QDataStream &operands);

    // Common Header (MSOBFH)

    typedef struct
    {
        union
        {
            U32 info;
            struct
            {
                U32 ver:4;
                U32 inst: 12;
                U32 fbt: 16;
            } fields;
        } opcode;
        U32 cbLength;
    } MSOFBH;

    typedef enum
    {
        msoblipERROR,               // An error occured during loading.
        msoblipUNKNOWN,             // An unknown blip type.
        msoblipEMF,                 // Windows Enhanced Metafile.
        msoblipWMF,                 // Windows Metafile.
        msoblipPICT,                // Macintosh PICT.
        msoblipJPEG,                // JFIF.
        msoblipPNG,                 // PNG.
        msoblipDIB,                 // Windows DIB.
        msoblipFirstClient = 32,    // First client defined blip type.
        msoblipLastClient  = 255    // Last client defined blip type.
    } MSOBLIPTYPE;

    MSOBLIPTYPE m_blipType;
    unsigned m_imageId;
    class Image
    {
    public:
        QString extension;
        unsigned length;
        const char *data;
        Image() { data = 0L; }
        ~Image() { delete [] data; }
    };
    QVector<Image> m_images;

    // Opcode handling and Metafile painter methods.

    void walk(U32 byteOperands, QDataStream &stream);
    void skip(U32 byteOperands, QDataStream &operands);
    void invokeHandler(
        MSOFBH &op,
        U32 wordOperands,
        QDataStream &operands);

    void opAlignrule(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opAnchor(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opArcrule(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opBlip(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opBse(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opBstorecontainer(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opCalloutrule(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opChildanchor(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opClientanchor(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opClientdata(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opClientrule(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opClienttextbox(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opClsid(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opColormru(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opConnectorrule(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opDeletedpspl(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opDg(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opDgcontainer(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opDgg(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opDggcontainer(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opOleobject(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opOpt(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opRegroupitems(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opSelection(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opSolvercontainer(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opSp(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opSpcontainer(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opSpgr(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opSpgrcontainer(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opSplitmenucolors(MSOFBH &op, U32 byteOperands, QDataStream &operands);
    void opTextbox(MSOFBH &op, U32 byteOperands, QDataStream &operands);                                                               // do nothing

    void shpLine(MSOFBH &op, U32 byteOperands, QDataStream &operands);                                                               // do nothing
    void shpPictureFrame(MSOFBH &op, U32 byteOperands, QDataStream &operands);                                                               // do nothing
    void shpRectangle(MSOFBH &op, U32 byteOperands, QDataStream &operands);                                                               // do nothing

    // Option handling.

    typedef struct
    {
        union
        {
            U16 info;
            struct
            {
                U16 pid: 14;
                U16 fBid: 1;
                U16 fComplex: 1;
            } fields;
        } opcode;
        U32 value;
    } Option;

    double from1616ToDouble(U32 value);
};

#endif
