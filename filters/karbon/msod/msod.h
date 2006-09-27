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
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

DESCRIPTION

    This is a generic parser for Microsoft Office Drawings (MSODs). The
    specification for this is the Microsoft Office 97 Drawing File Format
    published in MSDN. The output is a series of callbacks (a.k.a. virtual
    functions) which the caller can override as required.
*/

#ifndef MSOD_H
#define MSOD_H

class QString;
class Q3PointArray;
#include <kwmf.h>
#include <q3ptrvector.h>
#include <QPolygon>

class Msod :
    private KWmf
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
        const QString &file,
        const char *delayStream = 0L);
    bool parse(
        unsigned shapeId,
        QDataStream &stream,
        unsigned size,
        const char *delayStream = 0L);

    typedef KWmf::DrawContext DrawContext;

    // Should be protected...

    void brushSet(
        unsigned color,
        unsigned style);
    void penSet(
        unsigned color,
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
        const QPolygon &points) = 0;
    virtual void gotPolyline(
        const DrawContext &dc,
        const QPolygon &points) = 0;
    virtual void gotRectangle(
        const DrawContext &dc,
        const QPolygon &points) = 0;

private:
    Msod(const Msod &);
    const Msod &operator=(const Msod &);

    // Debug support.

    static const int s_area;

private:
    int m_dpi;
    DrawContext m_dc;
    unsigned m_dggError;
    unsigned m_requestedShapeId;
    bool m_isRequiredDrawing;
    const char *m_delayStream;
    struct
    {
        unsigned type;
        char *data;
        unsigned length;
    } m_shape;

    QPoint normalisePoint(
        QDataStream &operands);
    QSize normaliseSize(
        QDataStream &operands);
    void drawShape(
        unsigned shapeType,
        quint32 bytes,
        QDataStream &operands);

public:

    // Common Header (MSOBFH)
    typedef struct
    {
        union
        {
            quint32 info;
            struct
            {
                quint32 ver: 4;
                quint32 inst: 12;
                quint32 fbt: 16;
            } fields;
        } opcode;
        quint32 cbLength;
    } Header;

private:
    typedef enum
    {
        msoblipERROR,               // An error occurred during loading.
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
    Q3PtrVector<Image> m_images;

    // Opcode handling and painter methods.

    void walk(
        quint32 bytes,
        QDataStream &operands);
    void skip(
        quint32 bytes,
        QDataStream &operands);
    void invokeHandler(
        Header &op,
        quint32 bytes,
        QDataStream &operands);

    void opAlignrule(Header &op, quint32 bytes, QDataStream &operands);
    void opAnchor(Header &op, quint32 bytes, QDataStream &operands);
    void opArcrule(Header &op, quint32 bytes, QDataStream &operands);
    void opBlip(Header &op, quint32 bytes, QDataStream &operands);
    void opBse(Header &op, quint32 bytes, QDataStream &operands);
    void opBstorecontainer(Header &op, quint32 bytes, QDataStream &operands);
    void opCalloutrule(Header &op, quint32 bytes, QDataStream &operands);
    void opChildanchor(Header &op, quint32 bytes, QDataStream &operands);
    void opClientanchor(Header &op, quint32 bytes, QDataStream &operands);
    void opClientdata(Header &op, quint32 bytes, QDataStream &operands);
    void opClientrule(Header &op, quint32 bytes, QDataStream &operands);
    void opClienttextbox(Header &op, quint32 bytes, QDataStream &operands);
    void opClsid(Header &op, quint32 bytes, QDataStream &operands);
    void opColormru(Header &op, quint32 bytes, QDataStream &operands);
    void opConnectorrule(Header &op, quint32 bytes, QDataStream &operands);
    void opDeletedpspl(Header &op, quint32 bytes, QDataStream &operands);
    void opDg(Header &op, quint32 bytes, QDataStream &operands);
    void opDgcontainer(Header &op, quint32 bytes, QDataStream &operands);
    void opDgg(Header &op, quint32 bytes, QDataStream &operands);
    void opDggcontainer(Header &op, quint32 bytes, QDataStream &operands);
    void opOleobject(Header &op, quint32 bytes, QDataStream &operands);
    void opOpt(Header &op, quint32 bytes, QDataStream &operands);
    void opRegroupitems(Header &op, quint32 bytes, QDataStream &operands);
    void opSelection(Header &op, quint32 bytes, QDataStream &operands);
    void opSolvercontainer(Header &op, quint32 bytes, QDataStream &operands);
    void opSp(Header &op, quint32 bytes, QDataStream &operands);
    void opSpcontainer(Header &op, quint32 bytes, QDataStream &operands);
    void opSpgr(Header &op, quint32 bytes, QDataStream &operands);
    void opSpgrcontainer(Header &op, quint32 bytes, QDataStream &operands);
    void opSplitmenucolors(Header &op, quint32 bytes, QDataStream &operands);
    void opTextbox(Header &op, quint32 bytes, QDataStream &operands);

    // Option handling.

    class Options
    {
    public:
        Options(Msod &parent);
        ~Options();
        void walk(
            quint32 bytes,
            QDataStream &operands);

        double m_rotation;

        quint32 m_lTxid;

        quint32 m_pib;
        QString m_pibName;
        quint32 m_pibFlags;
        quint32 m_pictureId;
        bool m_fNoHitTestPicture;
        bool m_pictureGray;
        bool m_pictureBiLevel;
        bool m_pictureActive;

        quint32 m_geoLeft;
        quint32 m_geoTop;
        quint32 m_geoRight;
        quint32 m_geoBottom;
        quint32 m_shapePath;
        Q3PointArray *m_pVertices;
        bool m_fShadowOK;
        bool m_f3DOK;
        bool m_fLineOK;
        bool m_fGTextOK;
        bool m_fFillShadeShapeOK;
        bool m_fFillOK;

        bool m_fFilled;
        bool m_fHitTestFill;
        bool m_fillShape;
        bool m_fillUseRect;
        bool m_fNoFillHitTest;

        quint32 m_lineColor;
        quint32 m_lineBackColor;
        quint32 m_lineType;
        quint32 m_lineWidth;

        bool m_fArrowheadsOK;
        bool m_fLine;
        bool m_fHitTestLine;
        bool m_lineFillShape;
        bool m_fNoLineDrawDash;

        quint32 m_bWMode;

        bool m_fOleIcon;
        bool m_fPreferRelativeResize;
        bool m_fLockShapeType;
        bool m_fDeleteAttachedObject;
        bool m_fBackground;

    private:
        Msod &m_parent;

        typedef struct
        {
            union
            {
                quint16 info;
                struct
                {
                    quint16 pid: 14;
                    quint16 fBid: 1;
                    quint16 fComplex: 1;
                } fields;
            } opcode;
            quint32 value;
        } Header;

        void initialise();
        double from1616ToDouble(quint32 value);
    };
    friend class Msod::Options;

    Options *m_opt;
};

#endif
