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
*/

#include <kdebug.h>
#include <qdatastream.h>
#include <qfile.h>
#include <qlist.h>
#include <qpointarray.h>
#include <msod.h>

Msod::Msod(
    unsigned dpi)
{
//    m_dpi = dpi;
    m_images.setAutoDelete(true);
}

Msod::~Msod()
{
}

double Msod::from1616ToDouble(U32 value)
{
    return (value >> 16) + 65535.0 / (double)(value & 0xffff);
}

//
//
//

void Msod::invokeHandler(
    MSOFBH &op,
    U32 byteOperands,
    QDataStream &operands)
{
    typedef void (Msod::*method)(MSOFBH &op, U32 byteOperands, QDataStream &operands);

    typedef struct
    {
        const char *name;
        unsigned short opcode;
        method handler;
    } opcodeEntry;

    static const opcodeEntry funcTab[] =
    {
        { "ALIGNRULE",          0xF013, &Msod::opAlignrule },
        { "ANCHOR",             0xF00E, &Msod::opAnchor },
        { "ARCRULE",            0xF014, &Msod::opArcrule },
        { "BSE",                0xF007, &Msod::opBse },
        { "BSTORECONTAINER",    0xF001, &Msod::opBstorecontainer },
        { "CALLOUTRULE",        0xF017, &Msod::opCalloutrule },
        { "CHILDANCHOR",        0xF00F, &Msod::opChildanchor },
        { "CLIENTANCHOR",       0xF010, &Msod::opClientanchor },
        { "CLIENTDATA",         0xF011, &Msod::opClientdata },
        { "CLIENTRULE",         0xF015, &Msod::opClientrule },
        { "CLIENTTEXTBOX",      0xF00D, &Msod::opClienttextbox },
        { "CLSID",              0xF016, &Msod::opClsid },
        { "COLORMRU",           0xF11A, &Msod::opColormru },
        { "CONNECTORRULE",      0xF012, &Msod::opConnectorrule },
        { "DELETEDPSPL",        0xF11D, &Msod::opDeletedpspl },
        { "DG",                 0xF008, &Msod::opDg },
        { "DGCONTAINER",        0xF002, &Msod::opDgcontainer },
        { "DGG",                0xF006, &Msod::opDgg },
        { "DGGCONTAINER",       0xF000, &Msod::opDggcontainer },
        { "OLEOBJECT",          0xF11F, &Msod::opOleobject },
        { "OPT",                0xF00B, &Msod::opOpt },
        { "REGROUPITEMS",       0xF118, &Msod::opRegroupitems },
        { "SELECTION",          0xF119, &Msod::opSelection },
        { "SOLVERCONTAINER",    0xF005, &Msod::opSolvercontainer },
        { "SP",                 0xF00A, &Msod::opSp },
        { "SPCONTAINER",        0xF004, &Msod::opSpcontainer },
        { "SPGR",               0xF009, &Msod::opSpgr },
        { "SPGRCONTAINER",      0xF003, &Msod::opSpgrcontainer },
        { "SPLITMENUCOLORS",    0xF11E, &Msod::opSplitmenucolors },
        { "TEXTBOX",            0xF00C, &Msod::opTextbox },
        { NULL,                 0,      0 },
        { "BLIP",               0,      &Msod::opBlip }
    };
    unsigned i;
    method result;

    // Scan lookup table for operation.

    for (i = 0; funcTab[i].name; i++)
    {
        if (funcTab[i].opcode == op.opcode.fields.fbt)
        {
            break;
        }
    }

    // Invoke handler.

    result = funcTab[i].handler;
    if (!result && (op.opcode.fields.fbt >= 0xF018) && (0xF117 >= op.opcode.fields.fbt))
        result = funcTab[++i].handler;
    if (!result)
    {
        if (funcTab[i].name)
            kdWarning(s_area) << "invokeHandler: unsupported opcode: " <<
                funcTab[i].name <<
                " operands: " << byteOperands << endl;
        else
            kdWarning(s_area) << "invokeHandler: unsupported opcode: 0x" <<
                QString::number(op.opcode.fields.fbt, 16) <<
                " operands: " << byteOperands << endl;

        // Skip data we cannot use.

        skip(byteOperands, operands);
    }
    else
    {
        kdDebug(s_area) << "invokeHandler: opcode: " << funcTab[i].name <<
            " operands: " << byteOperands << endl;
        (this->*result)(op, byteOperands, operands);
    }
}

//
//
//

bool Msod::parse(
    unsigned shapeId,
    const QString &fileIn,
    const char *delayStream)
{
    QFile in(fileIn);
    if (!in.open(IO_ReadOnly))
    {
        kdError(s_area) << "Unable to open: " << fileIn << endl;
        in.close();
        return false;
    }
    QDataStream st(&in);
    st.setByteOrder(QDataStream::LittleEndian); // Great, I love Qt !
    m_requestedShapeId = shapeId;
    m_isRequiredDrawing = false;
    m_delayStream = delayStream;

    // Read bits.

//    while (!st.eof())
    {
        if (in.size() & 1)
        {
            m_dggError = 1;
        }
        else
        {
            m_dggError = 0;
        }
        walk(in.size() - m_dggError, st);
    }
    in.close();
    return true;
}

void Msod::opAlignrule(MSOFBH &, U32, QDataStream &)
{
}

void Msod::opAnchor(MSOFBH &, U32, QDataStream &)
{
}

void Msod::opArcrule(MSOFBH &, U32, QDataStream &)
{
}

void Msod::opBlip(MSOFBH &, U32 byteOperands, QDataStream &operands)
{
    U32 data = 0;;
    U8 isNotCompressed = (U8)true;

    // Skip any explicit primary header (m_rgbUidprimary)..

    if (m_blipHasPrimaryId)
    {
        data += 16;
        skip(16, operands);
    }
    switch (m_blipType)
    {
    case msoblipEMF:
    case msoblipWMF:
    case msoblipPICT:
        data += 18;
        skip(18, operands);
        operands >> isNotCompressed;
        break;
    case msoblipJPEG:
    case msoblipPNG:
    case msoblipDIB:
        // Skip the "tag".
        data += 1;
        skip(1, operands);
        break;
    default:
        break;
    }

    // Work out the file type.

    Image *image = new Image();
    switch (m_blipType)
    {
    case msoblipEMF:
        image->extension = "emf";
        break;
    case msoblipWMF:
        image->extension = "wmf";
        break;
    case msoblipPICT:
        image->extension = "pic";
        break;
    case msoblipJPEG:
        image->extension = "jpg";
        break;
    case msoblipPNG:
        image->extension = "png";
        break;
    case msoblipDIB:
        image->extension = "dib";
        break;
    default:
        image->extension = "img";
        break;
    }
    image->length = byteOperands - data;
    image->data = new char[image->length];
    operands.readRawBytes((char *)image->data, image->length);
    m_images.resize(m_images.size() + 1);
    m_images.insert(m_images.size() - 1, image);
}

// FBSE - File Blip Store Entry

void Msod::opBse(MSOFBH &op, U32 byteOperands, QDataStream &operands)
{
    struct
    {
        U8 btWin32;     // Required type on Win32.
        U8 btMacOS;     // Required type on Mac.
        U8 rgbUid[16];  // Identifier of blip.
        U16 tag;        // currently unused.
        U32 size;       // Blip size in stream.
        U32 cRef;       // Reference count on the blip.
        U32 foDelay;    // File offset in the delay stream.
        U8 usage;       // How this blip is used (MSOBLIPUSAGE).
        U8 cbName;      // length of the blip name.
        U8 unused2;     // for the future.
        U8 unused3;     // for the future.
    } data;
    unsigned i;

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

    m_blipType = static_cast<MSOBLIPTYPE>(op.opcode.fields.inst);
    switch (m_blipType)
    {
    case msoblipEMF:
        m_blipHasPrimaryId = (op.opcode.fields.inst ^ msobiEMF) == 1;
        break;
    case msoblipWMF:
        m_blipHasPrimaryId = (op.opcode.fields.inst ^ msobiWMF) == 1;
        break;
    case msoblipPICT:
        m_blipHasPrimaryId = (op.opcode.fields.inst ^ msobiPICT) == 1;
        break;
    case msoblipJPEG:
        m_blipHasPrimaryId = (op.opcode.fields.inst ^ msobiJPEG) == 1;
        break;
    case msoblipPNG:
        m_blipHasPrimaryId = (op.opcode.fields.inst ^ msobiPNG) == 1;
        break;
    case msoblipDIB:
        m_blipHasPrimaryId = (op.opcode.fields.inst ^ msobiDIB) == 1;
        break;
    default:
        kdWarning(s_area) << "opBlip: unknown Blip signature: " <<
            m_blipType << endl;
        m_blipHasPrimaryId = (op.opcode.fields.inst ^ msobiClient) == 1;
        break;
    }
    if (m_blipHasPrimaryId)
    {
        kdWarning(s_area) << "opBlip: Blip has primary header!" << endl;
    }

    operands >> data.btWin32 >> data.btMacOS;
    for (i = 0; i < sizeof(data.rgbUid); i++)
        operands >> data.rgbUid[i];
    operands >> data.tag >> data.size;
    operands >> data.cRef >> data.foDelay;
    operands >> data.usage >> data.cbName;
    operands >> data.unused2 >> data.unused2;

    // If the Blip is not in this drawing file, process it "manually".

    if (data.foDelay)
    {
        QByteArray bytes;
        bytes.setRawData(m_delayStream + data.foDelay, data.size);
        QDataStream stream(bytes, IO_ReadOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        walk(data.size, stream);
        bytes.resetRawData(m_delayStream + data.foDelay, data.size);
    }
    skip(byteOperands - sizeof(data), operands);
}

void Msod::opBstorecontainer(MSOFBH &, U32 byteOperands, QDataStream &operands)
{
    walk(byteOperands, operands);
}

void Msod::opCalloutrule(MSOFBH &, U32, QDataStream &)
{
}

void Msod::opChildanchor(MSOFBH &, U32, QDataStream &)
{
}

void Msod::opClientanchor(MSOFBH &, U32 byteOperands, QDataStream &operands)
{
    struct
    {
        U32 unknown;
    } data;

    operands >> data.unknown;
    skip(byteOperands - sizeof(data), operands);
}

void Msod::opClientdata(MSOFBH &, U32 byteOperands, QDataStream &operands)
{
    struct
    {
        U32 unknown;
    } data;

    operands >> data.unknown;
    skip(byteOperands - sizeof(data), operands);
}

void Msod::opClientrule(MSOFBH &, U32, QDataStream &)
{
}

void Msod::opClienttextbox(MSOFBH &, U32, QDataStream &)
{
}

void Msod::opClsid(MSOFBH &, U32, QDataStream &)
{
}

void Msod::opColormru(MSOFBH &, U32, QDataStream &)
{
}

void Msod::opConnectorrule(MSOFBH &, U32, QDataStream &)
{
}

void Msod::opDeletedpspl(MSOFBH &, U32, QDataStream &)
{
}

// FDG - File DG

void Msod::opDg(MSOFBH &, U32 byteOperands, QDataStream &operands)
{
    struct
    {
        U32 csp;        // The number of shapes in this drawing.
        U32 spidCur;    // The last shape ID given to an SP in this DG.
    } data;

    operands >> data.csp >> data.spidCur;
    m_isRequiredDrawing = (m_requestedShapeId == data.spidCur);
    if (m_isRequiredDrawing)
    {
        kdDebug(s_area) << "found requested drawing" << endl;
    }
    skip(byteOperands - sizeof(data), operands);
}

void Msod::opDgcontainer(MSOFBH &, U32 byteOperands, QDataStream &operands)
{
    walk(byteOperands, operands);
}

// FDGG - File DGG

void Msod::opDgg(MSOFBH &, U32 byteOperands, QDataStream &operands)
{
    struct
    {
        U32 spidMax;    // The current maximum shape ID.
        U32 cidcl;      // The number of ID clusters (FIDCLs).
        U32 cspSaved;   // The total number of shapes saved.
                        // (including deleted shapes, if undo
                        // information was saved).
        U32 cdgSaved;   // The total number of drawings saved.
    } data;

    // File ID Cluster - used to save IDCLs

    struct
    {
        U32 dgid;       // DG owning the SPIDs in this cluster
        U32 cspidCur;   // number of SPIDs used so far
    } data1;
    unsigned i;

    operands >> data.spidMax >> data.cidcl >> data.cspSaved >> data.cdgSaved;
    for (i = 0; i < data.cidcl - 1; i++)
    {
        operands >> data1.dgid >> data1.cspidCur;
    }
    skip(byteOperands - sizeof(data) - (data.cidcl - 1) * sizeof(data1), operands);
}

void Msod::opDggcontainer(MSOFBH &, U32 byteOperands, QDataStream &operands)
{
    walk(byteOperands, operands);
}

void Msod::opOleobject(MSOFBH &, U32, QDataStream &)
{
}

void Msod::opOpt(MSOFBH &, U32 byteOperands, QDataStream &operands)
{
    Option option;
    U16 length = 0;
    U16 complexLength = 0;

    double m_rotation = 0.0;
    U32 m_pib = 0;
    QString m_pibName = QString::null;
    U32 m_pibFlags = 0;
    U32 m_pictureId = 0;
    bool m_fNoHitTestPicture = false;
    bool m_pictureGray = false;
    bool m_pictureBiLevel = false;
    bool m_pictureActive = false;

    bool m_fFilled = true;
    bool m_fHitTestFill = true;
    bool m_fillShape = true;
    bool m_fillUseRect = false;
    bool m_fNoFillHitTest = false;

    U32 m_lineWidth = 9525;

    bool m_fArrowheadsOK = false;
    bool m_fLine = true;
    bool m_fHitTestLine = true;
    bool m_lineFillShape = true;
    bool m_fNoLineDrawDash = false;

    U32 m_bWMode = 1;

    bool m_fOleIcon = false;
    bool m_fPreferRelativeResize = false;
    bool m_fLockShapeType = false;
    bool m_fDeleteAttachedObject = false;
    bool m_fBackground = false;

    // First process all simple options, and add all complex options to a list.

    QList<Option> complexOpts;
    complexOpts.setAutoDelete(true);
    while (length + complexLength < (int)byteOperands)
    {
        operands >> option.opcode.info >> option.value;
        length += 4;

        // Defer processing of complex options.

        if (option.opcode.fields.fComplex)
        {
            complexLength += option.value;
            complexOpts.append(new Option(option));
            continue;
        }

        // Now squirrel away the option value.

        switch (option.opcode.fields.pid)
        {
        case 4:
            m_rotation = from1616ToDouble(option.value);
            break;
        case 260:
            if (option.opcode.fields.fBid)
            {
                m_pib = option.value;
                if (m_isRequiredDrawing)
                {
                    Image *image = m_images[m_pib - 1];
                    gotPicture(
                        m_pib,
                        image->extension,
                        image->length,
                        image->data);
                }
            }
            else
            {
                kdError(s_area) << "Cannot handle IMsoBlip" << endl;
            }
            break;
        case 262:
            m_pibFlags = option.value;
            break;
        case 267:
            m_pictureId = option.value;
            break;
        case 319:
            m_fNoHitTestPicture = (option.value & 0x0008) != 0;
            m_pictureGray = (option.value & 0x0004) != 0;
            m_pictureBiLevel = (option.value & 0x0002) != 0;
            m_pictureActive = (option.value & 0x0001) != 0;
            break;
        case 447:
            m_fFilled = (option.value & 0x0010) != 0;
            m_fHitTestFill = (option.value & 0x0008) != 0;
            m_fillShape = (option.value & 0x0004) != 0;
            m_fillUseRect = (option.value & 0x0002) != 0;
            m_fNoFillHitTest = (option.value & 0x0001) != 0;
            break;
        case 459:
            m_lineWidth = option.value;
            break;
        case 511:
            m_fArrowheadsOK = (option.value & 0x0010) != 0;
            m_fLine = (option.value & 0x0008) != 0;
            m_fHitTestLine = (option.value & 0x0004) != 0;
            m_lineFillShape = (option.value & 0x0002) != 0;
            m_fNoLineDrawDash = (option.value & 0x0001) != 0;
            break;
        case 772:
            m_bWMode = option.value;
            break;
        case 831:
            m_fOleIcon = (option.value & 0x0010) != 0;
            m_fPreferRelativeResize = (option.value & 0x0008) != 0;
            m_fLockShapeType = (option.value & 0x0004) != 0;
            m_fDeleteAttachedObject = (option.value & 0x0002) != 0;
            m_fBackground = (option.value & 0x0001) != 0;
            break;
        default:
            kdDebug(s_area) << "opOpt: unsupported simple option: " <<
                option.opcode.fields.pid << endl;
            break;
        }
    }

    // Now empty the list of complex options.

    while (complexOpts.count())
    {
        Q_INT16 c;

        option = *complexOpts.getFirst();
        complexOpts.removeFirst();
        switch (option.opcode.fields.pid)
        {
        case 261:
            while (true)
            {
                operands >> c;
                complexLength -= 2;
                if (!c)
                    break;
                m_pibName += QChar(c);
            };
            break;
        default:
            kdDebug(s_area) << "opOpt: unsupported complex option: " <<
                option.opcode.fields.pid << endl;
            break;
        }
    }

    skip(complexLength, operands);
}

void Msod::opRegroupitems(MSOFBH &, U32, QDataStream &)
{
}

void Msod::opSelection(MSOFBH &, U32, QDataStream &)
{
}

void Msod::opSolvercontainer(MSOFBH &, U32 byteOperands, QDataStream &operands)
{
    walk(byteOperands, operands);
}

void Msod::opSp(MSOFBH &op, U32 byteOperands, QDataStream &operands)
{
    typedef void (Msod::*method)(MSOFBH &op, U32 byteOperands, QDataStream &operands);

    typedef struct
    {
        const char *name;
        method handler;
    } shapeEntry;

    static const shapeEntry funcTab[] =
    {
        { NULL,                     0 },
        { "RECTANGLE",              &Msod::shpRectangle },
        { "ROUNDRECTANGLE",         0 /* &Msod::shpRoundrectangle */ },
        { "ELLIPSE",                0 /* &Msod::shpEllipse */ },
        { "DIAMOND",                0 /* &Msod::shpDiamond */ },
        { "ISOCELESTRIANGLE",       0 /* &Msod::shpIsocelestriangle */ },
        { "RIGHTTRIANGLE",          0 /* &Msod::shpRighttriangle */ },
        { "PARALLELOGRAM",          0 /* &Msod::shpParallelogram */ },
        { "TRAPEZOID",              0 /* &Msod::shpTrapezoid */ },
        { "HEXAGON",                0 /* &Msod::shpHexagon */ },
        { "OCTAGON",                0 /* &Msod::shpOctagon */ },
        { "PLUS",                   0 /* &Msod::shpPlus */ },
        { "STAR",                   0 /* &Msod::shpStar */ },
        { "ARROW",                  0 /* &Msod::shpArrow */ },
        { "THICKARROW",             0 /* &Msod::shpThickarrow */ },
        { "HOMEPLATE",              0 /* &Msod::shpHomeplate */ },
        { "CUBE",                   0 /* &Msod::shpCube */ },
        { "BALLOON",                0 /* &Msod::shpBalloon */ },
        { "SEAL",                   0 /* &Msod::shpSeal */ },
        { "ARC",                    0 /* &Msod::shpArc */ },
        { "LINE",                   0 /* &Msod::shpLine */ },
        { "PLAQUE",                 0 /* &Msod::shpPlaque */ },
        { "CAN",                    0 /* &Msod::shpCan */ },
        { "DONUT",                  0 /* &Msod::shpDonut */ },
        { "TEXTSIMPLE",             0 /* &Msod::shpTextsimple */ },
        { "TEXTOCTAGON",            0 /* &Msod::shpTextoctagon */ },
        { "TEXTHEXAGON",            0 /* &Msod::shpTexthexagon */ },
        { "TEXTCURVE",              0 /* &Msod::shpTextcurve */ },
        { "TEXTWAVE",               0 /* &Msod::shpTextwave */ },
        { "TEXTRING",               0 /* &Msod::shpTextring */ },
        { "TEXTONCURVE",            0 /* &Msod::shpTextoncurve */ },
        { "TEXTONRING",             0 /* &Msod::shpTextonring */ },
        { "STRAIGHTCONNECTOR1",     0 /* &Msod::shpStraightconnector1 */ },
        { "BENTCONNECTOR2",         0 /* &Msod::shpBentconnector2 */ },
        { "BENTCONNECTOR3",         0 /* &Msod::shpBentconnector3 */ },
        { "BENTCONNECTOR4",         0 /* &Msod::shpBentconnector4 */ },
        { "BENTCONNECTOR5",         0 /* &Msod::shpBentconnector5 */ },
        { "CURVEDCONNECTOR2",       0 /* &Msod::shpCurvedconnector2 */ },
        { "CURVEDCONNECTOR3",       0 /* &Msod::shpCurvedconnector3 */ },
        { "CURVEDCONNECTOR4",       0 /* &Msod::shpCurvedconnector4 */ },
        { "CURVEDCONNECTOR5",       0 /* &Msod::shpCurvedconnector5 */ },
        { "CALLOUT1",               0 /* &Msod::shpCallout1 */ },
        { "CALLOUT2",               0 /* &Msod::shpCallout2 */ },
        { "CALLOUT3",               0 /* &Msod::shpCallout3 */ },
        { "ACCENTCALLOUT1",         0 /* &Msod::shpAccentcallout1 */ },
        { "ACCENTCALLOUT2",         0 /* &Msod::shpAccentcallout2 */ },
        { "ACCENTCALLOUT3",         0 /* &Msod::shpAccentcallout3 */ },
        { "BORDERCALLOUT1",         0 /* &Msod::shpbordercallout1 */ },
        { "BORDERCALLOUT2",         0 /* &Msod::shpBordercallout2 */ },
        { "BORDERCALLOUT3",         0 /* &Msod::shpBordercallout3 */ },
        { "ACCENTBORDERCALLOUT1",   0 /* &Msod::shpAccentbordercallout1 */ },
        { "ACCENTBORDERCALLOUT2",   0 /* &Msod::shpAccentbordercallout2 */ },
        { "ACCENTBORDERCALLOUT3",   0 /* &Msod::shpAccentbordercallout3 */ },
        { "RIBBON",                 0 /* &Msod::shpRibbon */ },
        { "RIBBON2",                0 /* &Msod::shpRibbon2 */ },
        { "CHEVRON",                0 /* &Msod::shpChevron */ },
        { "PENTAGON",               0 /* &Msod::shpPentagon */ },
        { "NOSMOKING",              0 /* &Msod::shpNosmoking */ },
        { "SEAL8",                  0 /* &Msod::shpSeal8 */ },
        { "SEAL16",                 0 /* &Msod::shpSeal16 */ },
        { "SEAL32",                 0 /* &Msod::shpSeal32 */ },
        { "WEDGERECTCALLOUT",       0 /* &Msod::shpWedgerectcallout */ },
        { "WEDGERRECTCALLOUT",      0 /* &Msod::shpWedgerrectcallout */ },
        { "WEDGEELLIPSECALLOUT",    0 /* &Msod::shpWedgeellipsecallout */ },
        { "WAVE",                   0 /* &Msod::shpWave */ },
        { "FOLDEDCORNER",           0 /* &Msod::shpFoldedcorner */ },
        { "LEFTARROW",              0 /* &Msod::shpLeftarrow */ },
        { "DOWNARROW",              0 /* &Msod::shpDownarrow */ },
        { "UPARROW",                0 /* &Msod::shpUparrow */ },
        { "LEFTRIGHTARROW",         0 /* &Msod::shpLeftrightarrow */ },
        { "UPDOWNARROW",            0 /* &Msod::shpUpdownarrow */ },
        { "IRREGULARSEAL1",         0 /* &Msod::shpIrregularseal1 */ },
        { "IRREGULARSEAL2",         0 /* &Msod::shpIrregularseal2 */ },
        { "LIGHTNINGBOLT",          0 /* &Msod::shpLightningbolt */ },
        { "HEART",                  0 /* &Msod::shpHeart */ },
        { "PICTUREFRAME",           &Msod::shpPictureFrame },
        { "QUADARROW",              0 /* &Msod::shpQuadarrow */ },
        { "LEFTARROWCALLOUT",       0 /* &Msod::shpLeftarrowcallout */ },
        { "RIGHTARROWCALLOUT",      0 /* &Msod::shpRightarrowcallout */ },
        { "UPARROWCALLOUT",         0 /* &Msod::shpUparrowcallout */ },
        { "DOWNARROWCALLOUT",       0 /* &Msod::shpDownarrowcallout */ },
        { "LEFTRIGHTARROWCALLOUT",  0 /* &Msod::shpLeftrightarrowcallout */ },
        { "UPDOWNARROWCALLOUT",     0 /* &Msod::shpUpdownarrowcallout */ },
        { "QUADARROWCALLOUT",       0 /* &Msod::shpQuadarrowcallout */ },
        { "BEVEL",                  0 /* &Msod::shpBevel */ },
        { "LEFTBRACKET",            0 /* &Msod::shpLeftbracket */ },
        { "RIGHTBRACKET",           0 /* &Msod::shpRightbracket */ },
        { "LEFTBRACE",              0 /* &Msod::shpLeftbrace */ },
        { "RIGHTBRACE",             0 /* &Msod::shpRightbrace */ },
        { "LEFTUPARROW",            0 /* &Msod::shpLeftuparrow */ },
        { "BENTUPARROW",            0 /* &Msod::shpBentuparrow */ },
        { "BENTARROW",              0 /* &Msod::shpBentarrow */ },
        { "SEAL24",                 0 /* &Msod::shpSeal24 */ },
        { "STRIPEDRIGHTARROW",      0 /* &Msod::shpStripedrightarrow */ },
        { "NOTCHEDRIGHTARROW",      0 /* &Msod::shpNotchedrightarrow */ },
        { "BLOCKARC",               0 /* &Msod::shpBlockarc */ },
        { "SMILEYFACE",             0 /* &Msod::shpSmileyface */ },
        { "VERTICALSCROLL",         0 /* &Msod::shpVerticalscroll */ },
        { "HORIZONTALSCROLL",       0 /* &Msod::shpHorizontalscroll */ },
        { "CIRCULARARROW",          0 /* &Msod::shpCirculararrow */ },
        { "NOTCHEDCIRCULARARROW",   0 /* &Msod::shpNotchedcirculararrow */ },
        { "UTURNARROW",             0 /* &Msod::shpUturnarrow */ },
        { "CURVEDRIGHTARROW",       0 /* &Msod::shpCurvedrightarrow */ },
        { "CURVEDLEFTARROW",        0 /* &Msod::shpCurvedleftarrow */ },
        { "CURVEDUPARROW",          0 /* &Msod::shpCurveduparrow */ },
        { "CURVEDDOWNARROW",        0 /* &Msod::shpCurveddownarrow */ },
        { "CLOUDCALLOUT",           0 /* &Msod::shpCloudcallout */ },
        { "ELLIPSERIBBON",          0 /* &Msod::shpEllipseribbon */ },
        { "ELLIPSERIBBON2",         0 /* &Msod::shpEllipseribbon2 */ },
        { "FLOWCHARTPROCESS",       0 /* &Msod::shpFlowchartprocess */ },
        { "FLOWCHARTDECISION",      0 /* &Msod::shpFlowchartdecision */ },
        { "FLOWCHARTINPUTOUTPUT",   0 /* &Msod::shpFlowchartinputoutput */ },
        { "FLOWCHARTPREDEFINEDPROCESS", 0 /* &Msod::shpFlowchartpredefinedprocess */ },
        { "FLOWCHARTINTERNALSTORAGE", 0 /* &Msod::shpFlowchartinternalstorage */ },
        { "FLOWCHARTDOCUMENT",      0 /* &Msod::shpFlowchartdocument */ },
        { "FLOWCHARTMULTIDOCUMENT", 0 /* &Msod::shpFlowchartmultidocument */ },
        { "FLOWCHARTTERMINATOR",    0 /* &Msod::shpFlowchartterminator */ },
        { "FLOWCHARTPREPARATION",   0 /* &Msod::shpFlowchartpreparation */ },
        { "FLOWCHARTMANUALINPUT",   0 /* &Msod::shpFlowchartmanualinput */ },
        { "FLOWCHARTMANUALOPERATION", 0 /* &Msod::shpFlowchartmanualoperation */ },
        { "FLOWCHARTCONNECTOR",     0 /* &Msod::shpFlowchartconnector */ },
        { "FLOWCHARTPUNCHEDCARD",   0 /* &Msod::shpFlowchartpunchedcard */ },
        { "FLOWCHARTPUNCHEDTAPE",   0 /* &Msod::shpFlowchartpunchedtape */ },
        { "FLOWCHARTSUMMINGJUNCTION", 0 /* &Msod::shpFlowchartsummingjunction */ },
        { "FLOWCHARTOR",            0 /* &Msod::shpFlowchartor */ },
        { "FLOWCHARTCOLLATE",       0 /* &Msod::shpFlowchartcollate */ },
        { "FLOWCHARTSORT",          0 /* &Msod::shpFlowchartsort */ },
        { "FLOWCHARTEXTRACT",       0 /* &Msod::shpFlowchartextract */ },
        { "FLOWCHARTMERGE",         0 /* &Msod::shpFlowchartmerge */ },
        { "FLOWCHARTOFFLINESTORAGE", 0 /* &Msod::shpFlowchartofflinestorage */ },
        { "FLOWCHARTONLINESTORAGE", 0 /* &Msod::shpFlowchartonlinestorage */ },
        { "FLOWCHARTMAGNETICTAPE",  0 /* &Msod::shpFlowchartmagnetictape */ },
        { "FLOWCHARTMAGNETICDISK",  0 /* &Msod::shpFlowchartmagneticdisk */ },
        { "FLOWCHARTMAGNETICDRUM",  0 /* &Msod::shpFlowchartmagneticdrum */ },
        { "FLOWCHARTDISPLAY",       0 /* &Msod::shpFlowchartdisplay */ },
        { "FLOWCHARTDELAY",         0 /* &Msod::shpFlowchartdelay */ },
        { "TEXTPLAINTEXT",          0 /* &Msod::shpTextplaintext */ },
        { "TEXTSTOP",               0 /* &Msod::shpTextstop */ },
        { "TEXTTRIANGLE",           0 /* &Msod::shpTexttriangle */ },
        { "TEXTTRIANGLEINVERTED",   0 /* &Msod::shpTexttriangleinverted */ },
        { "TEXTCHEVRON",            0 /* &Msod::shpTextchevron */ },
        { "TEXTCHEVRONINVERTED",    0 /* &Msod::shpTextchevroninverted */ },
        { "TEXTRINGINSIDE",         0 /* &Msod::shpTextringinside */ },
        { "TEXTRINGOUTSIDE",        0 /* &Msod::shpTextringoutside */ },
        { "TEXTARCHUPCURVE",        0 /* &Msod::shpTextarchupcurve */ },
        { "TEXTARCHDOWNCURVE",      0 /* &Msod::shpTextarchdowncurve */ },
        { "TEXTCIRCLECURVE",        0 /* &Msod::shpTextcirclecurve */ },
        { "TEXTBUTTONCURVE",        0 /* &Msod::shpTextbuttoncurve */ },
        { "TEXTARCHUPPOUR",         0 /* &Msod::shpTextarchuppour */ },
        { "TEXTARCHDOWNPOUR",       0 /* &Msod::shpTextarchdownpour */ },
        { "TEXTCIRCLEPOUR",         0 /* &Msod::shpTextcirclepour */ },
        { "TEXTBUTTONPOUR",         0 /* &Msod::shpTextbuttonpour */ },
        { "TEXTCURVEUP",            0 /* &Msod::shpTextcurveup */ },
        { "TEXTCURVEDOWN",          0 /* &Msod::shpTextcurvedown */ },
        { "TEXTCASCADEUP",          0 /* &Msod::shpTextcascadeup */ },
        { "TEXTCASCADEDOWN",        0 /* &Msod::shpTextcascadedown */ },
        { "TEXTWAVE1",              0 /* &Msod::shpTextwave1 */ },
        { "TEXTWAVE2",              0 /* &Msod::shpTextwave2 */ },
        { "TEXTWAVE3",              0 /* &Msod::shpTextwave3 */ },
        { "TEXTWAVE4",              0 /* &Msod::shpTextwave4 */ },
        { "TEXTINFLATE",            0 /* &Msod::shpTextinflate */ },
        { "TEXTDEFLATE",            0 /* &Msod::shpTextdeflate */ },
        { "TEXTINFLATEBOTTOM",      0 /* &Msod::shpTextinflatebottom */ },
        { "TEXTDEFLATEBOTTOM",      0 /* &Msod::shpTextdeflatebottom */ },
        { "TEXTINFLATETOP",         0 /* &Msod::shpTextinflatetop */ },
        { "TEXTDEFLATETOP",         0 /* &Msod::shpTextdeflatetop */ },
        { "TEXTDEFLATEINFLATE",     0 /* &Msod::shpTextdeflateinflate */ },
        { "TEXTDEFLATEINFLATEDEFLATE", 0 /* &Msod::shpTextdeflateinflatedeflate */ },
        { "TEXTFADERIGHT",          0 /* &Msod::shpTextfaderight */ },
        { "TEXTFADELEFT",           0 /* &Msod::shpTextfadeleft */ },
        { "TEXTFADEUP",             0 /* &Msod::shpTextfadeup */ },
        { "TEXTFADEDOWN",           0 /* &Msod::shpTextfadedown */ },
        { "TEXTSLANTUP",            0 /* &Msod::shpTextslantup */ },
        { "TEXTSLANTDOWN",          0 /* &Msod::shpTextslantdown */ },
        { "TEXTCANUP",              0 /* &Msod::shpTextcanup */ },
        { "TEXTCANDOWN",            0 /* &Msod::shpTextcandown */ },
        { "FLOWCHARTALTERNATEPROCESS", 0 /* &Msod::shpFlowchartalternateprocess */ },
        { "FLOWCHARTOFFPAGECONNECTOR", 0 /* &Msod::shpFlowchartoffpageconnector */ },
        { "CALLOUT90",              0 /* &Msod::shpCallout90 */ },
        { "ACCENTCALLOUT90",        0 /* &Msod::shpAccentcallout90 */ },
        { "BORDERCALLOUT90",        0 /* &Msod::shpBordercallout90 */ },
        { "ACCENTBORDERCALLOUT90",  0 /* &Msod::shpAccentbordercallout90 */ },
        { "LEFTRIGHTUPARROW",       0 /* &Msod::shpLeftrightuparrow */ },
        { "SUN",                    0 /* &Msod::shpSun */ },
        { "MOON",                   0 /* &Msod::shpMoon */ },
        { "BRACKETPAIR",            0 /* &Msod::shpBracketpair */ },
        { "BRACEPAIR",              0 /* &Msod::shpBracepair */ },
        { "SEAL4",                  0 /* &Msod::shpSeal4 */ },
        { "DOUBLEWAVE",             0 /* &Msod::shpDoublewave */ },
        { "ACTIONBUTTONBLANK",      0 /* &Msod::shpActionbuttonblank */ },
        { "ACTIONBUTTONHOME",       0 /* &Msod::shpActionbuttonhome */ },
        { "ACTIONBUTTONHELP",       0 /* &Msod::shpActionbuttonhelp */ },
        { "ACTIONBUTTONINFORMATION", 0 /* &Msod::shpActionbuttoninformation */ },
        { "ACTIONBUTTONFORWARDNEXT", 0 /* &Msod::shpActionbuttonforwardnext */ },
        { "ACTIONBUTTONBACKPREVIOUS", 0 /* &Msod::shpActionbuttonbackprevious */ },
        { "ACTIONBUTTONEND",        0 /* &Msod::shpActionbuttonend */ },
        { "ACTIONBUTTONBEGINNING",  0 /* &Msod::shpActionbuttonbeginning */ },
        { "ACTIONBUTTONRETURN",     0 /* &Msod::shpActionbuttonreturn */ },
        { "ACTIONBUTTONDOCUMENT",   0 /* &Msod::shpActionbuttondocument */ },
        { "ACTIONBUTTONSOUND",      0 /* &Msod::shpActionbuttonsound */ },
        { "ACTIONBUTTONMOVIE",      0 /* &Msod::shpActionbuttonmovie */ },
        { "HOSTCONTROL",            0 /* &Msod::shpHostcontrol */ },
        { "TEXTBOX",                0 /* &Msod::shpTextbox */ },
    };
    unsigned i;
    method result;

    // Scan lookup table for operation.

    if (op.opcode.fields.inst < sizeof(funcTab)/sizeof(funcTab[0]))
    {
        i = op.opcode.fields.inst;
    }
    else
    {
        i = 0;
    }

    // Invoke handler.

    result = funcTab[i].handler;
    if (!result)
    {
        if (funcTab[i].name)
            kdWarning(s_area) << "opSp: unsupported shape: " <<
                funcTab[i].name <<
                " operands: " << byteOperands << endl;
        else
            kdWarning(s_area) << "opSp: unsupported shape: " <<
                op.opcode.fields.inst <<
                " operands: " << byteOperands << endl;

        // Skip data we cannot use.

        skip(byteOperands, operands);
    }
    else
    {
        struct
        {
            U32 spid;                       // The shape id
            union
            {
                U32 info;
                struct
                {
                    U32 fGroup : 1;         // This shape is a group shape
                    U32 fChild : 1;         // Not a top-level shape
                    U32 fPatriarch : 1;     // This is the topmost group shape.
                                            // Exactly one of these per drawing.
                    U32 fDeleted : 1;       // The shape has been deleted
                    U32 fOleShape : 1;      // The shape is an OLE object
                    U32 fHaveMaster : 1;    // Shape has a hspMaster property
                    U32 fFlipH : 1;         // Shape is flipped horizontally
                    U32 fFlipV : 1;         // Shape is flipped vertically
                    U32 fConnector : 1;     // Connector type of shape
                    U32 fHaveAnchor : 1;    // Shape has an anchor of some kind
                    U32 fBackground : 1;    // Background shape
                    U32 fHaveSpt : 1;       // Shape has a shape type property
                    U32 reserved : 20;      // Not yet used
                } fields;
            } grfPersistent;
        } data;

        operands >> data.spid >> data.grfPersistent.info;
        byteOperands -= sizeof(data);
        kdDebug(s_area) << "opSp: opcode: " << funcTab[i].name <<
            (data.grfPersistent.fields.fGroup ? " group" : "") <<
            (data.grfPersistent.fields.fChild ? " child" : "") <<
            (data.grfPersistent.fields.fPatriarch ? " patriarch" : "") <<
            (data.grfPersistent.fields.fDeleted ? " deleted" : "") <<
            (data.grfPersistent.fields.fOleShape ? " oleshape" : "") <<
            (data.grfPersistent.fields.fHaveMaster ? " master" : "") <<
            (data.grfPersistent.fields.fFlipH ? " flipv" : "") <<
            (data.grfPersistent.fields.fConnector ? " connector" : "") <<
            (data.grfPersistent.fields.fHaveAnchor ? " anchor" : "") <<
            (data.grfPersistent.fields.fBackground ? " background" : "") <<
            (data.grfPersistent.fields.fHaveSpt ? " spt" : "") <<
            " operands: " << byteOperands << endl;
        if (data.grfPersistent.fields.fDeleted || !m_isRequiredDrawing)
            skip(byteOperands, operands);
        else
            (this->*result)(op, byteOperands, operands);
    }
}

void Msod::opSpcontainer(MSOFBH &, U32 byteOperands, QDataStream &operands)
{
    walk(byteOperands, operands);
}

void Msod::opSpgr(MSOFBH &, U32 byteOperands, QDataStream &operands)
{
    struct
    {
        U32 x;
        U32 y;
        U32 w;
        U32 h;
    } data;

    operands >> data.x >> data.y >> data.w >> data.h;
    skip(byteOperands - sizeof(data), operands);
}

void Msod::opSpgrcontainer(MSOFBH &, U32 byteOperands, QDataStream &operands)
{
    walk(byteOperands, operands);
}

void Msod::opSplitmenucolors(MSOFBH &, U32 byteOperands, QDataStream &operands)
{
    struct
    {
        U32 fill;
        U32 line;
        U32 shadow;
        U32 threeDee;
    } data;

    operands >> data.fill >> data.line >> data.shadow >> data.threeDee;
    skip(byteOperands - sizeof(data), operands);
    skip(m_dggError, operands);
}

void Msod::opTextbox(MSOFBH &, U32, QDataStream &)
{
}

void Msod::shpPictureFrame(MSOFBH &, U32 byteOperands, QDataStream &operands)
{
    skip(byteOperands, operands);
}

void Msod::shpRectangle(MSOFBH &, U32 byteOperands, QDataStream &operands)
{
    skip(byteOperands, operands);
}

void Msod::skip(U32 byteOperands, QDataStream &operands)
{
    if ((int)byteOperands < 0)
    {
        kdError(s_area) << "skip: " << (int)byteOperands << endl;
        return;
    }
    if (byteOperands)
    {
        U32 i;
        U8 discard;

        kdDebug(s_area) << "skip: " << byteOperands << endl;
        for (i = 0; i < byteOperands; i++)
        {
            operands >> discard;
        }
    }
}

void Msod::walk(U32 byteOperands, QDataStream &stream)
{
    MSOFBH op;
    U32 length = 0;

    while (length < byteOperands)
    {
        stream >> op.opcode.info >> op.cbLength;
        if (op.opcode.fields.fbt == 0)
            break;

        // Package the arguments...

        invokeHandler(op, op.cbLength, stream);
        length += op.cbLength + 8;
    }
}
