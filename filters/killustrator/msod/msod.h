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
class QDatastream;
class QPointArray;

class Msod
{
public:

    // Construction.

    Msod(
        unsigned dpi);
    virtual ~Msod();

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

    typedef unsigned char U8;
    typedef unsigned short U16;
    typedef unsigned int U32;

    unsigned m_dggError;

    static unsigned walkFile(
        const U8 *in,
        U32 count,
        U32 shapeId,
        const U8 *pictureStream,
//        MSOBLIPTYPE *pictureType,
        U32 *pictureLength,
        const U8 **pictureData);


    // Extract a picture from Drawing File by shapeId. If the picture is found,
    // the pictureLength will be a non-zero value.

    static void getDrawing(
        const U8 *in,
        U32 count,
        U32 shapeId,
        const U8 *pictureStream,
//        MSOBLIPTYPE *pictureType,
        U32 *pictureLength,
        const U8 **pictureData);

    // GEL provided types...

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

    // Blip signature as encoded in the MSOFBH.inst

    typedef enum
    {
        msobiUNKNOWN = 0,
        msobiWMF = 0x216,       // Metafile header then compressed WMF
        msobiEMF = 0x3D4,       // Metafile header then compressed EMF
        msobiPICT = 0x542,      // Metafile header then compressed PICT
        msobiPNG = 0x6E0,       // One byte tag then PNG data
        msobiJFIF = 0x46A,      // One byte tag then JFIF data
        msobiJPEG = msobiJFIF,
        msobiDIB = 0x7A8,       // One byte tag then DIB data
        msobiClient = 0x800     // Clients should set this bit
    } MSOBI;

    // Opcode handling and Metafile painter methods.

    void walk(U32 byteOperands, QDataStream &stream);
    void skip(U32 byteOperands, QDataStream &operands);
    void invokeHandler(
        U16 opcode,
        U32 wordOperands,
        QDataStream &operands);

    void alignrule(U32 byteOperands, QDataStream &operands);
    void anchor(U32 byteOperands, QDataStream &operands);
    void arcrule(U32 byteOperands, QDataStream &operands);
    void blip(U32 byteOperands, QDataStream &operands);
    void bse(U32 byteOperands, QDataStream &operands);
    void bstorecontainer(U32 byteOperands, QDataStream &operands);
    void calloutrule(U32 byteOperands, QDataStream &operands);
    void childanchor(U32 byteOperands, QDataStream &operands);
    void clientanchor(U32 byteOperands, QDataStream &operands);
    void clientdata(U32 byteOperands, QDataStream &operands);
    void clientrule(U32 byteOperands, QDataStream &operands);
    void clienttextbox(U32 byteOperands, QDataStream &operands);
    void clsid(U32 byteOperands, QDataStream &operands);
    void colormru(U32 byteOperands, QDataStream &operands);
    void connectorrule(U32 byteOperands, QDataStream &operands);
    void deletedpspl(U32 byteOperands, QDataStream &operands);
    void dg(U32 byteOperands, QDataStream &operands);
    void dgcontainer(U32 byteOperands, QDataStream &operands);
    void dgg(U32 byteOperands, QDataStream &operands);
    void dggcontainer(U32 byteOperands, QDataStream &operands);
    void oleobject(U32 byteOperands, QDataStream &operands);
    void opt(U32 byteOperands, QDataStream &operands);
    void regroupitems(U32 byteOperands, QDataStream &operands);
    void selection(U32 byteOperands, QDataStream &operands);
    void solvercontainer(U32 byteOperands, QDataStream &operands);
    void sp(U32 byteOperands, QDataStream &operands);
    void spcontainer(U32 byteOperands, QDataStream &operands);
    void spgr(U32 byteOperands, QDataStream &operands);
    void spgrcontainer(U32 byteOperands, QDataStream &operands);
    void splitmenucolors(U32 byteOperands, QDataStream &operands);
    void textbox(U32 byteOperands, QDataStream &operands);                                                               // do nothing
};

#endif
