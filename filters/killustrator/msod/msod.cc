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
#include <qpointarray.h>
#include <msod.h>

Msod::Msod(
    unsigned dpi)
{
//    m_dpi = dpi;
}

Msod::~Msod()
{
}

//
//
//

void Msod::invokeHandler(
    U16 opcode,
    U32 byteOperands,
    QDataStream &operands)
{
    typedef void (Msod::*method)(U32 byteOperands, QDataStream &operands);

    typedef struct
    {
        const char *name;
        unsigned short opcode;
        method handler;
    } opcodeEntry;

    static const opcodeEntry funcTab[] =
    {
        { "ALIGNRULE",          0xF013, &Msod::alignrule },
        { "ANCHOR",             0xF00E, &Msod::anchor },
        { "ARCRULE",            0xF014, &Msod::arcrule },
        { "BSE",                0xF007, &Msod::bse },
        { "BSTORECONTAINER",    0xF001, &Msod::bstorecontainer },
        { "CALLOUTRULE",        0xF017, &Msod::calloutrule },
        { "CHILDANCHOR",        0xF00F, &Msod::childanchor },
        { "CLIENTANCHOR",       0xF010, &Msod::clientanchor },
        { "CLIENTDATA",         0xF011, &Msod::clientdata },
        { "CLIENTRULE",         0xF015, &Msod::clientrule },
        { "CLIENTTEXTBOX",      0xF00D, &Msod::clienttextbox },
        { "CLSID",              0xF016, &Msod::clsid },
        { "COLORMRU",           0xF11A, &Msod::colormru },
        { "CONNECTORRULE",      0xF012, &Msod::connectorrule },
        { "DELETEDPSPL",        0xF11D, &Msod::deletedpspl },
        { "DG",                 0xF008, &Msod::dg },
        { "DGCONTAINER",        0xF002, &Msod::dgcontainer },
        { "DGG",                0xF006, &Msod::dgg },
        { "DGGCONTAINER",       0xF000, &Msod::dggcontainer },
        { "OLEOBJECT",          0xF11F, &Msod::oleobject },
        { "OPT",                0xF00B, &Msod::opt },
        { "REGROUPITEMS",       0xF118, &Msod::regroupitems },
        { "SELECTION",          0xF119, &Msod::selection },
        { "SOLVERCONTAINER",    0xF005, &Msod::solvercontainer },
        { "SP",                 0xF00A, &Msod::sp },
        { "SPCONTAINER",        0xF004, &Msod::spcontainer },
        { "SPGR",               0xF009, &Msod::spgr },
        { "SPGRCONTAINER",      0xF003, &Msod::spgrcontainer },
        { "SPLITMENUCOLORS",    0xF11E, &Msod::splitmenucolors },
        { "TEXTBOX",            0xF00C, &Msod::textbox },
        { NULL,                 0,      0 },
        { "BLIP",               0,      &Msod::blip }
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
    if (!result && (opcode >= 0xF018) && (0xF117 >= opcode))
        result = funcTab[i + 1].handler;
    if (!result)
    {
        if (funcTab[i].name)
            kdError(s_area) << "invokeHandler: unsupported opcode: " <<
                funcTab[i].name <<
                " operands: " << byteOperands << endl;
        else
            kdError(s_area) << "invokeHandler: unsupported opcode: 0x" <<
                QString::number(opcode, 16) <<
                " operands: " << byteOperands << endl;

        // Skip data we cannot use.

        skip(byteOperands, operands);
    }
    else
    {
        kdDebug(s_area) << "invokeHandler: opcode: " << funcTab[i].name <<
            " operands: " << byteOperands << endl;
        (this->*result)(byteOperands, operands);
    }
}

//
//
//

bool Msod::parse(
    const QString &file)
{
    QFile in(file);
    if (!in.open(IO_ReadOnly))
    {
        kdError(s_area) << "Unable to open input file!" << endl;
        in.close();
        return false;
    }

    QDataStream st(&in);
    st.setByteOrder(QDataStream::LittleEndian); // Great, I love Qt !

    // Read bits.

    while (!st.eof())
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

void Msod::alignrule(U32 byteOperands, QDataStream &operands)
{
}

void Msod::anchor(U32 byteOperands, QDataStream &operands)
{
}

void Msod::arcrule(U32 byteOperands, QDataStream &operands)
{
}

void Msod::blip(U32 byteOperands, QDataStream &operands)
{
}

// FBSE - File Blip Store Entry

void Msod::bse(U32 byteOperands, QDataStream &operands)
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

    operands >> data.btWin32 >> data.btWin32;
    for (i = 0; i < sizeof(data.rgbUid); i++)
        operands >> data.rgbUid[i];
    operands >> data.tag >> data.size;
    operands >> data.cRef >> data.foDelay;
    operands >> data.usage >> data.cbName;
    operands >> data.unused2 >> data.unused2;
    skip(byteOperands - sizeof(data), operands);
}

void Msod::bstorecontainer(U32 byteOperands, QDataStream &operands)
{
    walk(byteOperands, operands);
}

void Msod::calloutrule(U32 byteOperands, QDataStream &operands)
{
}

void Msod::childanchor(U32 byteOperands, QDataStream &operands)
{
}

void Msod::clientanchor(U32 byteOperands, QDataStream &operands)
{
    struct
    {
        U32 unknown;
    } data;

    operands >> data.unknown;
    skip(byteOperands - sizeof(data), operands);
}

void Msod::clientdata(U32 byteOperands, QDataStream &operands)
{
    struct
    {
        U32 unknown;
    } data;

    operands >> data.unknown;
    skip(byteOperands - sizeof(data), operands);
}

void Msod::clientrule(U32 byteOperands, QDataStream &operands)
{
}

void Msod::clienttextbox(U32 byteOperands, QDataStream &operands)
{
}

void Msod::clsid(U32 byteOperands, QDataStream &operands)
{
}

void Msod::colormru(U32 byteOperands, QDataStream &operands)
{
}

void Msod::connectorrule(U32 byteOperands, QDataStream &operands)
{
}

void Msod::deletedpspl(U32 byteOperands, QDataStream &operands)
{
}

// FDG - File DG

void Msod::dg(U32 byteOperands, QDataStream &operands)
{
    struct
    {
        U32 csp;        // The number pof shapes in this drawing.
        U32 spidCur;    // The last shape ID given to an SP in this DG.
    } data;

    operands >> data.csp >> data.spidCur;
    skip(byteOperands - sizeof(data), operands);
}

void Msod::dgcontainer(U32 byteOperands, QDataStream &operands)
{
    walk(byteOperands, operands);
}

// FDGG - File DGG

void Msod::dgg(U32 byteOperands, QDataStream &operands)
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

void Msod::dggcontainer(U32 byteOperands, QDataStream &operands)
{
    walk(byteOperands, operands);
}

void Msod::oleobject(U32 byteOperands, QDataStream &operands)
{
}

void Msod::opt(U32 byteOperands, QDataStream &operands)
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
    U16 value;
    U16 length = 0;
    U16 complexLength = 0;

    skip(byteOperands, operands);
    return;
    /*
    while (length + complexLength < byteOperands)
    {
        operands >> opcode.info >> value;
        length += 4;
        kdError(s_area) << "opt: fComplex: " << opcode.fields.fComplex << endl;
        kdError(s_area) << "opt: fBid: " << opcode.fields.fBid << endl;
        kdError(s_area) << "opt: pid: " << opcode.fields.pid << endl;
        if (opcode.fields.fComplex)
        {
            complexLength += value;
            kdError(s_area) << "opt: complex: " << complexLength << endl;
        }
        kdError(s_area) << "opt: " << length << " " << byteOperands << endl;
    }
    skip(complexLength, operands);
    */
}

void Msod::regroupitems(U32 byteOperands, QDataStream &operands)
{
}

void Msod::selection(U32 byteOperands, QDataStream &operands)
{
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

void Msod::solvercontainer(U32 byteOperands, QDataStream &operands)
{
    walk(byteOperands, operands);
}

void Msod::sp(U32 byteOperands, QDataStream &operands)
{
}

void Msod::spcontainer(U32 byteOperands, QDataStream &operands)
{
    walk(byteOperands, operands);
}

void Msod::spgr(U32 byteOperands, QDataStream &operands)
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

void Msod::spgrcontainer(U32 byteOperands, QDataStream &operands)
{
    walk(byteOperands, operands);
}

void Msod::splitmenucolors(U32 byteOperands, QDataStream &operands)
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

void Msod::textbox(U32 byteOperands, QDataStream &operands)
{
}

void Msod::walk(U32 byteOperands, QDataStream &stream)
{
    // Common Header (MSOBFH)

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
    U32 length = 0;

    while (length < byteOperands)
    {
        stream >> opcode.info >> cbLength;
        if (opcode.fields.fbt == 0)
            break;

        // Package the arguments...

        invokeHandler(opcode.fields.fbt, cbLength , stream);
        length += cbLength + 8;
    }
}
