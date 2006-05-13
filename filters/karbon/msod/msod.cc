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
*/

#include <kdebug.h>
#include <QDataStream>
#include <QFile>
#include <q3ptrlist.h>
#include <q3pointarray.h>
#include <QRect>
#include <QSize>
#include <msod.h>
#include <zlib.h>

const int Msod::s_area = 30505;

Msod::Msod(
    unsigned dpi) :
        KWmf(dpi)
{
    m_dpi = dpi;
    m_images.setAutoDelete(true);
    m_opt = new Options(*this);
    m_shape.data = 0L;
    m_shape.length = 0;
}

Msod::~Msod()
{
    delete [] m_shape.data;
    delete m_opt;
}

void Msod::drawShape(
    unsigned shapeType,
    quint32 bytes,
    QDataStream &operands)
{
    static const char *funcTab[] =
    {
        "UNKNOWN",                  // Unknown
        "RECTANGLE",                // Rectangle
        "ROUNDRECTANGLE",           // Roundrectangle
        "ELLIPSE",                  // Ellipse
        "DIAMOND",                  // Diamond
        "ISOCELESTRIANGLE",         // Isocelestriangle
        "RIGHTTRIANGLE",            // Righttriangle
        "PARALLELOGRAM",            // Parallelogram
        "TRAPEZOID",                // Trapezoid
        "HEXAGON",                  // Hexagon
        "OCTAGON",                  // Octagon
        "PLUS",                     // Plus
        "STAR",                     // Star
        "ARROW",                    // Arrow
        "THICKARROW",               // Thickarrow
        "HOMEPLATE",                // Homeplate
        "CUBE",                     // Cube
        "BALLOON",                  // Balloon
        "SEAL",                     // Seal
        "ARC",                      // Arc
        "LINE",                     // Line
        "PLAQUE",                   // Plaque
        "CAN",                      // Can
        "DONUT",                    // Donut
        "TEXTSIMPLE",               // Textsimple
        "TEXTOCTAGON",              // Textoctagon
        "TEXTHEXAGON",              // Texthexagon
        "TEXTCURVE",                // Textcurve
        "TEXTWAVE",                 // Textwave
        "TEXTRING",                 // Textring
        "TEXTONCURVE",              // Textoncurve
        "TEXTONRING",               // Textonring
        "STRAIGHTCONNECTOR1",       // Straightconnector1
        "BENTCONNECTOR2",           // Bentconnector2
        "BENTCONNECTOR3",           // Bentconnector3
        "BENTCONNECTOR4",           // Bentconnector4
        "BENTCONNECTOR5",           // Bentconnector5
        "CURVEDCONNECTOR2",         // Curvedconnector2
        "CURVEDCONNECTOR3",         // Curvedconnector3
        "CURVEDCONNECTOR4",         // Curvedconnector4
        "CURVEDCONNECTOR5",         // Curvedconnector5
        "CALLOUT1",                 // Callout1
        "CALLOUT2",                 // Callout2
        "CALLOUT3",                 // Callout3
        "ACCENTCALLOUT1",           // Accentcallout1
        "ACCENTCALLOUT2",           // Accentcallout2
        "ACCENTCALLOUT3",           // Accentcallout3
        "BORDERCALLOUT1",           // bordercallout1
        "BORDERCALLOUT2",           // Bordercallout2
        "BORDERCALLOUT3",           // Bordercallout3
        "ACCENTBORDERCALLOUT1",     // Accentbordercallout1
        "ACCENTBORDERCALLOUT2",     // Accentbordercallout2
        "ACCENTBORDERCALLOUT3",     // Accentbordercallout3
        "RIBBON",                   // Ribbon
        "RIBBON2",                  // Ribbon2
        "CHEVRON",                  // Chevron
        "PENTAGON",                 // Pentagon
        "NOSMOKING",                // Nosmoking
        "SEAL8",                    // Seal8
        "SEAL16",                   // Seal16
        "SEAL32",                   // Seal32
        "WEDGERECTCALLOUT",         // Wedgerectcallout
        "WEDGERRECTCALLOUT",        // Wedgerrectcallout
        "WEDGEELLIPSECALLOUT",      // Wedgeellipsecallout
        "WAVE",                     // Wave
        "FOLDEDCORNER",             // Foldedcorner
        "LEFTARROW",                // Leftarrow
        "DOWNARROW",                // Downarrow
        "UPARROW",                  // Uparrow
        "LEFTRIGHTARROW",           // Leftrightarrow
        "UPDOWNARROW",              // Updownarrow
        "IRREGULARSEAL1",           // Irregularseal1
        "IRREGULARSEAL2",           // Irregularseal2
        "LIGHTNINGBOLT",            // Lightningbolt
        "HEART",                    // Heart
        "PICTUREFRAME",             // PictureFrame
        "QUADARROW",                // Quadarrow
        "LEFTARROWCALLOUT",         // Leftarrowcallout
        "RIGHTARROWCALLOUT",        // Rightarrowcallout
        "UPARROWCALLOUT",           // Uparrowcallout
        "DOWNARROWCALLOUT",         // Downarrowcallout
        "LEFTRIGHTARROWCALLOUT",    // Leftrightarrowcallout
        "UPDOWNARROWCALLOUT",       // Updownarrowcallout
        "QUADARROWCALLOUT",         // Quadarrowcallout
        "BEVEL",                    // Bevel
        "LEFTBRACKET",              // Leftbracket
        "RIGHTBRACKET",             // Rightbracket
        "LEFTBRACE",                // Leftbrace
        "RIGHTBRACE",               // Rightbrace
        "LEFTUPARROW",              // Leftuparrow
        "BENTUPARROW",              // Bentuparrow
        "BENTARROW",                // Bentarrow
        "SEAL24",                   // Seal24
        "STRIPEDRIGHTARROW",        // Stripedrightarrow
        "NOTCHEDRIGHTARROW",        // Notchedrightarrow
        "BLOCKARC",                 // Blockarc
        "SMILEYFACE",               // Smileyface
        "VERTICALSCROLL",           // Verticalscroll
        "HORIZONTALSCROLL",         // Horizontalscroll
        "CIRCULARARROW",            // Circulararrow
        "NOTCHEDCIRCULARARROW",     // Notchedcirculararrow
        "UTURNARROW",               // Uturnarrow
        "CURVEDRIGHTARROW",         // Curvedrightarrow
        "CURVEDLEFTARROW",          // Curvedleftarrow
        "CURVEDUPARROW",            // Curveduparrow
        "CURVEDDOWNARROW",          // Curveddownarrow
        "CLOUDCALLOUT",             // Cloudcallout
        "ELLIPSERIBBON",            // Ellipseribbon
        "ELLIPSERIBBON2",           // Ellipseribbon2
        "FLOWCHARTPROCESS",         // Flowchartprocess
        "FLOWCHARTDECISION",        // Flowchartdecision
        "FLOWCHARTINPUTOUTPUT",     // Flowchartinputoutput
        "FLOWCHARTPREDEFINEDPROCESS",   // Flowchartpredefinedprocess
        "FLOWCHARTINTERNALSTORAGE",   // Flowchartinternalstorage
        "FLOWCHARTDOCUMENT",        // Flowchartdocument
        "FLOWCHARTMULTIDOCUMENT",   // Flowchartmultidocument
        "FLOWCHARTTERMINATOR",      // Flowchartterminator
        "FLOWCHARTPREPARATION",     // Flowchartpreparation
        "FLOWCHARTMANUALINPUT",     // Flowchartmanualinput
        "FLOWCHARTMANUALOPERATION",   // Flowchartmanualoperation
        "FLOWCHARTCONNECTOR",       // Flowchartconnector
        "FLOWCHARTPUNCHEDCARD",     // Flowchartpunchedcard
        "FLOWCHARTPUNCHEDTAPE",     // Flowchartpunchedtape
        "FLOWCHARTSUMMINGJUNCTION",   // Flowchartsummingjunction
        "FLOWCHARTOR",              // Flowchartor
        "FLOWCHARTCOLLATE",         // Flowchartcollate
        "FLOWCHARTSORT",            // Flowchartsort
        "FLOWCHARTEXTRACT",         // Flowchartextract
        "FLOWCHARTMERGE",           // Flowchartmerge
        "FLOWCHARTOFFLINESTORAGE",   // Flowchartofflinestorage
        "FLOWCHARTONLINESTORAGE",   // Flowchartonlinestorage
        "FLOWCHARTMAGNETICTAPE",    // Flowchartmagnetictape
        "FLOWCHARTMAGNETICDISK",    // Flowchartmagneticdisk
        "FLOWCHARTMAGNETICDRUM",    // Flowchartmagneticdrum
        "FLOWCHARTDISPLAY",         // Flowchartdisplay
        "FLOWCHARTDELAY",           // Flowchartdelay
        "TEXTPLAINTEXT",            // Textplaintext
        "TEXTSTOP",                 // Textstop
        "TEXTTRIANGLE",             // Texttriangle
        "TEXTTRIANGLEINVERTED",     // Texttriangleinverted
        "TEXTCHEVRON",              // Textchevron
        "TEXTCHEVRONINVERTED",      // Textchevroninverted
        "TEXTRINGINSIDE",           // Textringinside
        "TEXTRINGOUTSIDE",          // Textringoutside
        "TEXTARCHUPCURVE",          // Textarchupcurve
        "TEXTARCHDOWNCURVE",        // Textarchdowncurve
        "TEXTCIRCLECURVE",          // Textcirclecurve
        "TEXTBUTTONCURVE",          // Textbuttoncurve
        "TEXTARCHUPPOUR",           // Textarchuppour
        "TEXTARCHDOWNPOUR",         // Textarchdownpour
        "TEXTCIRCLEPOUR",           // Textcirclepour
        "TEXTBUTTONPOUR",           // Textbuttonpour
        "TEXTCURVEUP",              // Textcurveup
        "TEXTCURVEDOWN",            // Textcurvedown
        "TEXTCASCADEUP",            // Textcascadeup
        "TEXTCASCADEDOWN",          // Textcascadedown
        "TEXTWAVE1",                // Textwave1
        "TEXTWAVE2",                // Textwave2
        "TEXTWAVE3",                // Textwave3
        "TEXTWAVE4",                // Textwave4
        "TEXTINFLATE",              // Textinflate
        "TEXTDEFLATE",              // Textdeflate
        "TEXTINFLATEBOTTOM",        // Textinflatebottom
        "TEXTDEFLATEBOTTOM",        // Textdeflatebottom
        "TEXTINFLATETOP",           // Textinflatetop
        "TEXTDEFLATETOP",           // Textdeflatetop
        "TEXTDEFLATEINFLATE",       // Textdeflateinflate
        "TEXTDEFLATEINFLATEDEFLATE",   // Textdeflateinflatedeflate
        "TEXTFADERIGHT",            // Textfaderight
        "TEXTFADELEFT",             // Textfadeleft
        "TEXTFADEUP",               // Textfadeup
        "TEXTFADEDOWN",             // Textfadedown
        "TEXTSLANTUP",              // Textslantup
        "TEXTSLANTDOWN",            // Textslantdown
        "TEXTCANUP",                // Textcanup
        "TEXTCANDOWN",              // Textcandown
        "FLOWCHARTALTERNATEPROCESS",   // Flowchartalternateprocess
        "FLOWCHARTOFFPAGECONNECTOR",   // Flowchartoffpageconnector
        "CALLOUT90",                // Callout90
        "ACCENTCALLOUT90",          // Accentcallout90
        "BORDERCALLOUT90",          // Bordercallout90
        "ACCENTBORDERCALLOUT90",    // Accentbordercallout90
        "LEFTRIGHTUPARROW",         // Leftrightuparrow
        "SUN",                      // Sun
        "MOON",                     // Moon
        "BRACKETPAIR",              // Bracketpair
        "BRACEPAIR",                // Bracepair
        "SEAL4",                    // Seal4
        "DOUBLEWAVE",               // Doublewave
        "ACTIONBUTTONBLANK",        // Actionbuttonblank
        "ACTIONBUTTONHOME",         // Actionbuttonhome
        "ACTIONBUTTONHELP",         // Actionbuttonhelp
        "ACTIONBUTTONINFORMATION",  // Actionbuttoninformation
        "ACTIONBUTTONFORWARDNEXT",  // Actionbuttonforwardnext
        "ACTIONBUTTONBACKPREVIOUS", // Actionbuttonbackprevious
        "ACTIONBUTTONEND",          // Actionbuttonend
        "ACTIONBUTTONBEGINNING",    // Actionbuttonbeginning
        "ACTIONBUTTONRETURN",       // Actionbuttonreturn
        "ACTIONBUTTONDOCUMENT",     // Actionbuttondocument
        "ACTIONBUTTONSOUND",        // Actionbuttonsound
        "ACTIONBUTTONMOVIE",        // Actionbuttonmovie
        "HOSTCONTROL",              // Hostcontrol
        "TEXTBOX",                  // Textbox
    };
    struct
    {
        quint32 spid;                  // The shape id
        union
        {
            quint32 info;
            struct
            {
                quint32 fGroup : 1;    // This shape is a group shape
                quint32 fChild : 1;    // Not a top-level shape
                quint32 fPatriarch : 1; // This is the topmost group shape.
                                        // Exactly one of these per drawing.
                quint32 fDeleted : 1;  // The shape has been deleted
                quint32 fOleShape : 1; // The shape is an OLE object
                quint32 fHaveMaster : 1; // Shape has a hspMaster property
                quint32 fFlipH : 1;    // Shape is flipped horizontally
                quint32 fFlipV : 1;    // Shape is flipped vertically
                quint32 fConnector : 1; // Connector type of shape
                quint32 fHaveAnchor : 1; // Shape has an anchor of some kind
                quint32 fBackground : 1; // Background shape
                quint32 fHaveSpt : 1;  // Shape has a shape type property
                quint32 reserved : 20; // Not yet used
            } fields;
        } grfPersistent;
    } data;

    // Scan lookup table for operation.

    operands >> data.spid;
    operands >> data.grfPersistent.info;
    bytes -= 8;
	kDebug(s_area) << "shape-id: " << data.spid << " type: " << funcTab[shapeType] << " (" << shapeType << ")" <<
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
        " operands: " << bytes << endl;
    if (data.grfPersistent.fields.fDeleted)
        return;
    if ((!m_isRequiredDrawing) && (m_requestedShapeId != data.spid))
        return;

    // An active shape! Let's draw it...

    switch (shapeType)
    {
    case 0:
        if (m_opt->m_pVertices)
        {
            gotPolyline(m_dc, *m_opt->m_pVertices);
        }
        break;
    case 1:
        if (bytes > 7)
        {
            QPoint topLeft;
            QSize size;

            topLeft = normalisePoint(operands);
            size = normaliseSize(operands);

            QRect rect(topLeft, size);
            Q3PointArray points(4);

            points.setPoint(0, topLeft);
            points.setPoint(1, rect.topRight());
            points.setPoint(2, rect.bottomRight());
            points.setPoint(3, rect.bottomLeft());
            gotRectangle(m_dc, points);
        }
    case 20:
        if (bytes > 7)
        {
            QPoint lineFrom;
            QPoint lineTo;

            lineTo = normalisePoint(operands);

            Q3PointArray points(2);

            points.setPoint(0, lineFrom);
            points.setPoint(1, lineTo);
            gotPolyline(m_dc, points);
        }
        break;
    default:
        break;
    }
}

void Msod::invokeHandler(
    Header &op,
    quint32 bytes,
    QDataStream &operands)
{
    typedef void (Msod::*method)(Header &op, quint32 bytes, QDataStream &operands);

    typedef struct
    {
        const char *name;
        quint16 opcode;
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
            kWarning(s_area) << "invokeHandler: unsupported opcode: " <<
                funcTab[i].name <<
                " operands: " << bytes << endl;
        else
            kWarning(s_area) << "invokeHandler: unsupported opcode: 0x" <<
                QString::number(op.opcode.fields.fbt, 16) <<
                " operands: " << bytes << endl;

        // Skip data we cannot use.

        skip(bytes, operands);
    }
    else
    {
        kDebug(s_area) << "invokeHandler: opcode: " << funcTab[i].name <<
            " operands: " << bytes << endl;

        // We don't invoke the handler directly on the incoming operands, but
        // via a temporary datastream. This adds overhead, but eliminates the
        // need for the individual handlers to read *exactly* the right amount
        // of data (thus speeding development, and possibly adding some
        // future-proofing).

        if (bytes)
        {
            QByteArray *record = new QByteArray(bytes);
            QDataStream *body;

            operands.readRawBytes(record->data(), bytes);
            body = new QDataStream(*record, QIODevice::ReadOnly);
            body->setByteOrder(QDataStream::LittleEndian);
            (this->*result)(op, bytes, *body);
            delete body;
            delete record;
        }
        else
        {
            QDataStream *body = new QDataStream();

            (this->*result)(op, bytes, *body);
            delete body;
        }
    }
}

QPoint Msod::normalisePoint(
    QDataStream &operands)
{
    quint16 x;
    quint16 y;

    operands >> x >> y;
    return QPoint(x / m_dpi, y / m_dpi);
}

QSize Msod::normaliseSize(
    QDataStream &operands)
{
    quint16 width;
    quint16 height;

    operands >> width >> height;
    return QSize(width / m_dpi, height / m_dpi);
}

bool Msod::parse(
    unsigned shapeId,
    const QString &file,
    const char *delayStream)
{
    QFile in(file);
    if (!in.open(QIODevice::ReadOnly))
    {
        kError(s_area) << "Unable to open input file!" << endl;
        in.close();
        return false;
    }
    QDataStream stream(&in);
    bool result = parse(shapeId, stream, in.size(), delayStream);
    in.close();
    return result;
}

bool Msod::parse(
    unsigned shapeId,
    QDataStream &stream,
    unsigned size,
    const char *delayStream)
{
    stream.setByteOrder(QDataStream::LittleEndian); // Great, I love Qt !
    m_requestedShapeId = shapeId;
    m_isRequiredDrawing = false;
    m_delayStream = delayStream;

    // Read bits.

    walk(size, stream);
    return true;
}

void Msod::opAlignrule(
    Header &,
    quint32,
    QDataStream &)
{
}

void Msod::opAnchor(
    Header &,
    quint32,
    QDataStream &)
{
}

void Msod::opArcrule(
    Header &,
    quint32,
    QDataStream &)
{
}

void Msod::opBlip(Header &, quint32 bytes, QDataStream &operands)
{
    typedef enum
    {
        msobiWMF = 0x216,       // Metafile header then compressed WMF.
        msobiEMF = 0x3D4,       // Metafile header then compressed EMF.
        msobiPICT = 0x542,      // Metafile header then compressed PICT.
        msobiPNG = 0x6E0,       // One byte tag then PNG data.
        msobiJPEG = 0x46A,      // One byte tag then JFIF data.
        msobiDIB = 0x7A8,       // One byte tag then DIB data.
        msobiClient = 0x800     // Clients should set this bit.
    } MSOBI;
    typedef enum
    {
        msocompressionDeflate,
        msocompressionNone = 254,
        msocompressionTest
    } MSOBLIPCOMPRESSION;

    bool hasPrimaryId;
    quint32 length = 0;
    struct
    {
        quint32 cb;
        struct
        {
            quint32 x;
            quint32 y;
            quint32 w;
            quint32 h;
        } bounds;
        struct
        {
            quint32 w;
            quint32 h;
        } ptSize;
        quint32 cbSave;
        quint8 compression;
        quint8 filter;
    } data;

    // Skip any explicit primary header (m_rgbUidprimary).

    switch (m_blipType)
    {
    case msoblipEMF:
        hasPrimaryId = (m_blipType ^ msobiEMF) != 0;
        break;
    case msoblipWMF:
        hasPrimaryId = (m_blipType ^ msobiWMF) != 0;
        break;
    case msoblipPICT:
        hasPrimaryId = (m_blipType ^ msobiPICT) != 0;
        break;
    case msoblipJPEG:
        hasPrimaryId = (m_blipType ^ msobiJPEG) != 0;
        break;
    case msoblipPNG:
        hasPrimaryId = (m_blipType ^ msobiPNG) != 0;
        break;
    case msoblipDIB:
        hasPrimaryId = (m_blipType ^ msobiDIB) != 0;
        break;
    default:
        hasPrimaryId = (m_blipType ^ msobiClient) != 0;
        break;
    }
    if (hasPrimaryId)
    {
        length += 16;
        skip(16, operands);
    }

    // Process the rest of the header.

    data.compression = msocompressionNone;
    switch (m_blipType)
    {
    case msoblipEMF:
    case msoblipWMF:
    case msoblipPICT:
        length += 34;
        operands >> data.cb;
        operands >> data.bounds.x >> data.bounds.y >> data.bounds.w >> data.bounds.h;
        operands >> data.ptSize.w >> data.ptSize.h;
        operands >> data.cbSave;
        operands >> data.compression >> data.filter;
        break;
    case msoblipJPEG:
    case msoblipPNG:
    case msoblipDIB:
        // Skip the "tag".
        length += 1;
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
    image->length = bytes - length;
    image->data = new char[image->length];
    operands.readRawBytes((char *)image->data, image->length);
    if (data.compression == msocompressionDeflate)
    {
        const char *tmp;
        uLongf destLen = data.cb;
        int result;

        tmp = new char[data.cb];
        result = uncompress((quint8 *)tmp, &destLen, (quint8 *)image->data, image->length);
        if (result != Z_OK)
        {
            kError(s_area) << "opBlip: uncompress failed: " << result << endl;
        }
        if (destLen != data.cb)
        {
            kError(s_area) << "opBlip: uncompressed " << destLen << " instead of " << data.cb << endl;
        }
        delete [] image->data;
        image->data = tmp;
        image->length = destLen;
    }
    m_images.resize(m_images.size() + 1);
    m_images.insert(m_images.size() - 1, image);
}

// FBSE - File Blip Store Entry

void Msod::opBse(Header &op, quint32, QDataStream &operands)
{
    struct
    {
        quint8 btWin32;     // Required type on Win32.
        quint8 btMacOS;     // Required type on Mac.
        quint8 rgbUid[16];  // Identifier of blip.
        quint16 tag;        // currently unused.
        quint32 size;       // Blip size in stream.
        quint32 cRef;       // Reference count on the blip.
        quint32 foDelay;    // File offset in the delay stream.
        quint8 usage;       // How this blip is used (MSOBLIPUSAGE).
        quint8 cbName;      // length of the blip name.
        quint8 unused2;     // for the future.
        quint8 unused3;     // for the future.
    } data;
    unsigned i;

    // Work out the type of the BLIP.

    m_blipType = static_cast<MSOBLIPTYPE>(op.opcode.fields.inst);
    operands >> data.btWin32;
    operands >> data.btMacOS;
    for (i = 0; i < 16; i++)
        operands >> data.rgbUid[i];
    operands >> data.tag >> data.size;
    operands >> data.cRef >> data.foDelay;
    operands >> data.usage >> data.cbName;
    operands >> data.unused2 >> data.unused2;

    // If the Blip is not in this drawing file, process it "manually".

    if (m_delayStream)
    {
        // The m_pib refers to images by number, which includes images
        // that are no longer here. Thus, we fake these out so that any
        // references to non-deleted images are still valid (!!!).

        if (data.size && data.cRef)
        {
            QByteArray bytes;
            bytes.setRawData(m_delayStream + data.foDelay, data.size);
            QDataStream stream(bytes, QIODevice::ReadOnly);
            stream.setByteOrder(QDataStream::LittleEndian);
            walk(data.size, stream);
            bytes.resetRawData(m_delayStream + data.foDelay, data.size);
        }
        else
        {
            m_images.resize(m_images.size() + 1);
            m_images.insert(m_images.size() - 1, 0L);
        }
    }
}

void Msod::opBstorecontainer(Header &, quint32 bytes, QDataStream &operands)
{
    walk(bytes, operands);
}

void Msod::opCalloutrule(
    Header &,
    quint32,
    QDataStream &)
{
}

void Msod::opChildanchor(
    Header &,
    quint32,
    QDataStream &)
{
}

void Msod::opClientanchor(Header &, quint32, QDataStream &operands)
{
    struct
    {
        quint32 unknown;
    } data;

    operands >> data.unknown;
    kDebug(s_area) << "client anchor: " << data.unknown << endl;
}

void Msod::opClientdata(Header &, quint32, QDataStream &operands)
{
    struct
    {
        quint32 unknown;
    } data;

    operands >> data.unknown;
    kDebug(s_area) << "client data: " << data.unknown << endl;
}

void Msod::opClientrule(
    Header &,
    quint32,
    QDataStream &)
{
}

void Msod::opClienttextbox(
    Header &,
    quint32,
    QDataStream &operands)
{
    struct
    {
        quint32 unknown;
    } data;

    operands >> data.unknown;
    kDebug(s_area) << "client textbox: 0x" << QString::number(data.unknown,16) << endl;
}

void Msod::opClsid(
    Header &,
    quint32,
    QDataStream &)
{
}

void Msod::opColormru(
    Header &,
    quint32,
    QDataStream &)
{
}

void Msod::opConnectorrule(
    Header &,
    quint32,
    QDataStream &)
{
}

void Msod::opDeletedpspl(
    Header &,
    quint32,
    QDataStream &)
{
}

// FDG - File DG

void Msod::opDg(Header &, quint32, QDataStream &operands)
{
    struct
    {
        quint32 csp;        // The number of shapes in this drawing.
        quint32 spidCur;    // The last shape ID given to an SP in this DG.
    } data;

    operands >> data.csp >> data.spidCur;
    kDebug(s_area) << "drawing id: " << data.spidCur << endl;
    m_isRequiredDrawing = (m_requestedShapeId == data.spidCur);
    if (m_isRequiredDrawing)
    {
        kDebug(s_area) << "found requested drawing" << endl;
    }
}

void Msod::opDgcontainer(Header &, quint32 bytes, QDataStream &operands)
{
    walk(bytes, operands);
}

// FDGG - File DGG

void Msod::opDgg(Header &, quint32, QDataStream &operands)
{
    struct
    {
        quint32 spidMax;    // The current maximum shape ID.
        quint32 cidcl;      // The number of ID clusters (FIDCLs).
        quint32 cspSaved;   // The total number of shapes saved.
                        // (including deleted shapes, if undo
                        // information was saved).
        quint32 cdgSaved;   // The total number of drawings saved.
    } data;

    // File ID Cluster - used to save IDCLs

    struct
    {
        quint32 dgid;       // DG owning the SPIDs in this cluster
        quint32 cspidCur;   // number of SPIDs used so far
    } data1;
    unsigned i;

    operands >> data.spidMax >> data.cidcl >> data.cspSaved >> data.cdgSaved;
    kDebug(s_area) << data.cspSaved << " shapes in " <<
        data.cidcl - 1 << " clusters in " <<
        data.cdgSaved << " drawings" << endl;
    for (i = 0; i < data.cidcl - 1; i++)
    {
        operands >> data1.dgid >> data1.cspidCur;
    }
}

void Msod::opDggcontainer(Header &, quint32 bytes, QDataStream &operands)
{
    walk(bytes, operands);
}

void Msod::opOleobject(
    Header &,
    quint32,
    QDataStream &)
{
}

void Msod::opOpt(Header &, quint32 bytes, QDataStream &operands)
{
    m_opt->walk(bytes, operands);
}

void Msod::opRegroupitems(
    Header &,
    quint32,
    QDataStream &)
{
}

void Msod::opSelection(
    Header &,
    quint32,
    QDataStream &)
{
}

void Msod::opSolvercontainer(Header &, quint32 bytes, QDataStream &operands)
{
    walk(bytes, operands);
}

void Msod::opSp(Header &op, quint32 bytes, QDataStream &operands)
{
    // We want to defer the act of drawing a shape until we have seen any options
    // that may affect it. Thus, we merely store the data away, and let opSpContainer
    // do all the ahrd work.

    m_shape.type = op.opcode.fields.inst;
    m_shape.length = bytes;
    m_shape.data = new char [bytes];
    operands.readRawBytes(m_shape.data, bytes);
}

void Msod::opSpcontainer(Header &, quint32 bytes, QDataStream &operands)
{
    walk(bytes, operands);

    // Having gathered all the information for this shape, we can now draw it.

    QByteArray  a;

    a.setRawData(m_shape.data, m_shape.length);
    QDataStream s(a, QIODevice::ReadOnly);
    s.setByteOrder(QDataStream::LittleEndian); // Great, I love Qt !
    drawShape(m_shape.type, m_shape.length, s);
    a.resetRawData(m_shape.data, m_shape.length);
    delete [] m_shape.data;
    m_shape.data = 0L;
}

void Msod::opSpgr(Header &, quint32, QDataStream &operands)
{
    struct
    {
        quint32 x;
        quint32 y;
        quint32 w;
        quint32 h;
    } data;

    operands >> data.x >> data.y >> data.w >> data.h;
}

void Msod::opSpgrcontainer(Header &, quint32 bytes, QDataStream &operands)
{
    walk(bytes, operands);
}

void Msod::opSplitmenucolors(Header &, quint32, QDataStream &operands)
{
    struct
    {
        quint32 fill;
        quint32 line;
        quint32 shadow;
        quint32 threeDee;
    } data;

    operands >> data.fill >> data.line >> data.shadow >> data.threeDee;
}

void Msod::opTextbox(
    Header &,
    quint32,
    QDataStream &)
{
}

void Msod::skip(quint32 bytes, QDataStream &operands)
{
    if ((int)bytes < 0)
    {
        kError(s_area) << "skip: " << (int)bytes << endl;
        return;
    }
    if (bytes)
    {
        quint32 i;
        quint8 discard;

        kDebug(s_area) << "skip: " << bytes << endl;
        for (i = 0; i < bytes; i++)
        {
            operands >> discard;
        }
    }
}

void Msod::walk(quint32 bytes, QDataStream &operands)
{
    Header op;
    quint32 length = 0;

    // Stop parsing when there are no more records. Note that we stop as soon
    // as we cannot get a complete header.
    while (length + 8 <= bytes)
    {
        operands >> op.opcode.info >> op.cbLength;

        // If we get some duff data, protect ourselves.
        if (length + op.cbLength + 8 > bytes)
        {
            op.cbLength = bytes - length - 8;
        }
        length += op.cbLength + 8;
        if (op.opcode.fields.fbt == 0x200)
        {
            // This appears to be an EOF marker.
            break;
        }

        // Package the arguments...

        invokeHandler(op, op.cbLength, operands);
    }

    // Eat unexpected data that the caller may expect us to consume.
    skip(bytes - length, operands);
}

Msod::Options::Options(
    Msod &parent) :
        m_parent(parent)
{
    m_pVertices = 0L;
    initialise();
}

Msod::Options::~Options()
{
    delete m_pVertices;
}

double Msod::Options::from1616ToDouble(quint32 value)
{
    return (value >> 16) + 65535.0 / (double)(value & 0xffff);
}

void Msod::Options::initialise()
{
    m_rotation = 0.0;

    m_lTxid = 0;

    m_pib = 0;
    m_pibName = QString::null;
    m_pibFlags = 0;
    m_pictureId = 0;
    m_fNoHitTestPicture = false;
    m_pictureGray = false;
    m_pictureBiLevel = false;
    m_pictureActive = false;

    m_geoLeft = 0;
    m_geoTop = 0;
    m_geoRight = 21600;
    m_geoBottom = 21600;
    m_shapePath = 1;
    delete m_pVertices;
    m_pVertices = 0L;
    m_fShadowOK = true;
    m_f3DOK = true;
    m_fLineOK = true;
    m_fGTextOK = false;
    m_fFillShadeShapeOK = false;
    m_fFillOK = true;

    m_fFilled = true;
    m_fHitTestFill = true;
    m_fillShape = true;
    m_fillUseRect = false;
    m_fNoFillHitTest = false;

    m_lineColor = 0;
    m_lineBackColor = 0xffffff;
    m_lineType = 0;
    m_lineWidth = 9525;

    m_fArrowheadsOK = false;
    m_fLine = true;
    m_fHitTestLine = true;
    m_lineFillShape = true;
    m_fNoLineDrawDash = false;

    m_bWMode = 1;

    m_fOleIcon = false;
    m_fPreferRelativeResize = false;
    m_fLockShapeType = false;
    m_fDeleteAttachedObject = false;
    m_fBackground = false;
}

void Msod::Options::walk(quint32 bytes, QDataStream &operands)
{
    Header op;
    quint16 length = 0;
    quint16 complexLength = 0;

    // Reset all options to default values.

    initialise();

    // First process all simple options, and add all complex options to a list.

    Q3PtrList<Header> complexOpts;
    complexOpts.setAutoDelete(true);
    bool unsupported;
    while (length + complexLength < (int)bytes)
    {
        operands >> op.opcode.info >> op.value;
        length += 6;

        // Defer processing of complex options.

        if (op.opcode.fields.fComplex)
        {
            complexLength += op.value;
            complexOpts.append(new Header(op));
            continue;
        }

        // Now squirrel away the option value.

        unsupported = false;
        switch (op.opcode.fields.pid)
        {
        case 4:
            m_rotation = from1616ToDouble(op.value);
            break;
        case 128:
            m_lTxid = op.value;
    kDebug(s_area) << "textbox: 0x" << QString::number(op.value,16) << endl;
            break;
        case 260:
            if (op.opcode.fields.fBid)
            {
                m_pib = op.value;
                if (m_parent.m_isRequiredDrawing)
                {
                    Image *image = m_parent.m_images[m_pib - 1];

                    // If it is an embedded WMF we don't bother with the
                    // part; we just extract it as more vector graphics.

                    if (image->extension == "wmf")
                    {
                        QByteArray  a;
                        a.setRawData(image->data, image->length);
                        QDataStream s(a, QIODevice::ReadOnly);
                        m_parent.KWmf::parse(s, image->length);
                        a.resetRawData(image->data, image->length);
                    }
                    else
                    {
                        m_parent.gotPicture(
                            m_pib,
                            image->extension,
                            image->length,
                            image->data);
                    }
                }
            }
            else
            {
                kError(s_area) << "Cannot handle IMsoBlip" << endl;
            }
            break;
        case 262:
            m_pibFlags = op.value;
            break;
        case 267:
            m_pictureId = op.value;
            break;
        case 319:
            m_fNoHitTestPicture = (op.value & 0x0008) != 0;
            m_pictureGray = (op.value & 0x0004) != 0;
            m_pictureBiLevel = (op.value & 0x0002) != 0;
            m_pictureActive = (op.value & 0x0001) != 0;
            break;
        case 320:
            m_geoLeft = op.value;
            break;
        case 321:
            m_geoTop = op.value;
            break;
        case 322:
            m_geoRight = op.value;
            break;
        case 323:
            m_geoBottom = op.value;
            break;
        case 324:
            m_shapePath = op.value;
            break;
        case 383:
            m_fShadowOK = (op.value & 0x0020) != 0;
            m_f3DOK = (op.value & 0x0010) != 0;
            m_fLineOK = (op.value & 0x0008) != 0;
            m_fGTextOK = (op.value & 0x0004) != 0;
            m_fFillShadeShapeOK = (op.value & 0x0002) != 0;
            m_fFillOK = (op.value & 0x0001) != 0;
            break;
        case 447:
            m_fFilled = (op.value & 0x0010) != 0;
            m_fHitTestFill = (op.value & 0x0008) != 0;
            m_fillShape = (op.value & 0x0004) != 0;
            m_fillUseRect = (op.value & 0x0002) != 0;
            m_fNoFillHitTest = (op.value & 0x0001) != 0;
            break;
        case 448:
            m_lineColor = op.value;
            break;
        case 450:
            m_lineBackColor = op.value;
            break;
        case 452:
            m_lineType = op.value;
            break;
        case 459:
            m_lineWidth = op.value;
            break;
        case 511:
            m_fArrowheadsOK = (op.value & 0x0010) != 0;
            m_fLine = (op.value & 0x0008) != 0;
            m_fHitTestLine = (op.value & 0x0004) != 0;
            m_lineFillShape = (op.value & 0x0002) != 0;
            m_fNoLineDrawDash = (op.value & 0x0001) != 0;
            break;
        case 772:
            m_bWMode = op.value;
            break;
        case 831:
            m_fOleIcon = (op.value & 0x0010) != 0;
            m_fPreferRelativeResize = (op.value & 0x0008) != 0;
            m_fLockShapeType = (op.value & 0x0004) != 0;
            m_fDeleteAttachedObject = (op.value & 0x0002) != 0;
            m_fBackground = (op.value & 0x0001) != 0;
            break;
        default:
            unsupported = true;
            kWarning(s_area) << "unsupported simple option: " <<
                op.opcode.fields.pid << endl;
            break;
        }
        if (!unsupported)
            kDebug(s_area) << "simple option: " <<
                op.opcode.fields.pid << endl;
    }

    // Now empty the list of complex options.

    while (complexOpts.count())
    {
        qint16 t16;
        unsigned i;

        op = *complexOpts.getFirst();
        complexOpts.removeFirst();
        unsupported = false;
        switch (op.opcode.fields.pid)
        {
        case 261:
            while (true)
            {
                operands >> t16;
                if (!t16)
                    break;
                m_pibName += QChar(t16);
            };
            break;
        case 325:
            m_pVertices = new Q3PointArray(op.value / 4);
            for (i = 0; i < m_pVertices->count(); i++)
            {
                m_pVertices->setPoint(i, m_parent.normalisePoint(operands));
            };
            break;
        case 326:
            operands >> t16;
            i = t16;
            operands >> t16;
            operands >> t16;
            m_parent.skip(i * t16, operands);
            break;
        default:
            unsupported = true;
            kWarning(s_area) << "unsupported complex option: " <<
                op.opcode.fields.pid << " operands: " << op.value << endl;
            m_parent.skip(op.value, operands);
            break;
        }
        if (!unsupported)
            kDebug(s_area) << "complex option: " <<
                op.opcode.fields.pid << " operands: " << op.value  << endl;
        complexLength -= op.value;
    }
}
