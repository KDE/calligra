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
class QPointArray;
#include <kwmf.h>
#include <qptrvector.h>

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
        Q_UINT32 bytes,
        QDataStream &operands);

public:

    // Common Header (MSOBFH)
    typedef struct
    {
        union
        {
            Q_UINT32 info;
            struct
            {
                Q_UINT32 ver: 4;
                Q_UINT32 inst: 12;
                Q_UINT32 fbt: 16;
            } fields;
        } opcode;
        Q_UINT32 cbLength;
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
    QPtrVector<Image> m_images;

    // Opcode handling and painter methods.

    void walk(
        Q_UINT32 bytes,
        QDataStream &operands);
    void skip(
        Q_UINT32 bytes,
        QDataStream &operands);
    void invokeHandler(
        Header &op,
        Q_UINT32 bytes,
        QDataStream &operands);

    void opAlignrule(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opAnchor(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opArcrule(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opBlip(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opBse(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opBstorecontainer(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opCalloutrule(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opChildanchor(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opClientanchor(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opClientdata(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opClientrule(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opClienttextbox(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opClsid(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opColormru(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opConnectorrule(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opDeletedpspl(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opDg(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opDgcontainer(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opDgg(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opDggcontainer(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opOleobject(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opOpt(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opRegroupitems(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSelection(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSolvercontainer(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSp(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSpcontainer(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSpgr(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSpgrcontainer(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSplitmenucolors(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opTextbox(Header &op, Q_UINT32 bytes, QDataStream &operands);

    // Option handling.

    class Options
    {
    public:
        Options(Msod &parent);
        ~Options();
        void walk(
            Q_UINT32 bytes,
            QDataStream &operands);

        double m_rotation;

        Q_UINT32 m_lTxid;

        Q_UINT32 m_pib;
        QString m_pibName;
        Q_UINT32 m_pibFlags;
        Q_UINT32 m_pictureId;
        bool m_fNoHitTestPicture;
        bool m_pictureGray;
        bool m_pictureBiLevel;
        bool m_pictureActive;

        Q_UINT32 m_geoLeft;
        Q_UINT32 m_geoTop;
        Q_UINT32 m_geoRight;
        Q_UINT32 m_geoBottom;
        Q_UINT32 m_shapePath;
        QPointArray *m_pVertices;
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

        Q_UINT32 m_lineColor;
        Q_UINT32 m_lineBackColor;
        Q_UINT32 m_lineType;
        Q_UINT32 m_lineWidth;

        bool m_fArrowheadsOK;
        bool m_fLine;
        bool m_fHitTestLine;
        bool m_lineFillShape;
        bool m_fNoLineDrawDash;

        Q_UINT32 m_bWMode;

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
                Q_UINT16 info;
                struct
                {
                    Q_UINT16 pid: 14;
                    Q_UINT16 fBid: 1;
                    Q_UINT16 fComplex: 1;
                } fields;
            } opcode;
            Q_UINT32 value;
        } Header;

        void initialise();
        double from1616ToDouble(Q_UINT32 value);
    };
    friend class Msod::Options;

    Options *m_opt;
};

#endif
