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

    This file is a description of structures used in the storage of Microsoft
    Office art. The specification for this is the Microsoft Office 97 Drawing
    File Format published in MSDN.
*/

#include <kdebug.h>
#include <msdrawing.h>
#include <mswordgenerated.h>

const int MsDrawing::s_area = 30513;

void MsDrawing::getDrawing(
    const U8 *in,
    U32 count,
    U32 shapeId,
    const U8 *pictureStream,
    MSOBLIPTYPE *pictureType,
    U32 *pictureLength,
    const U8 **pictureData)
{
    *pictureType = msoblipERROR;
    *pictureLength = 0;
    *pictureData = 0L;
    MsDrawing::walkFile(
        in,
        count,
        shapeId,
        pictureStream,
        pictureType,
        pictureLength,
        pictureData);
}

unsigned MsDrawing::read(const U8 *in, FBSE *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += MsWordGenerated::read(in + bytes, &out->btWin32);
    bytes += MsWordGenerated::read(in + bytes, &out->btMacOS);
    for (i = 0; i < 51; i++);
        bytes += MsWordGenerated::read(in + bytes, &out->rgbUid[i]);
    bytes += MsWordGenerated::read(in + bytes, &out->tag);
    bytes += MsWordGenerated::read(in + bytes, &out->size);
    bytes += MsWordGenerated::read(in + bytes, &out->cRef);
    bytes += MsWordGenerated::read(in + bytes, &out->foDelay);
    bytes += MsWordGenerated::read(in + bytes, &out->usage);
    bytes += MsWordGenerated::read(in + bytes, &out->cbName);
    bytes += MsWordGenerated::read(in + bytes, &out->unused2);
    bytes += MsWordGenerated::read(in + bytes, &out->unused3);
    return bytes;
} // FBSE

unsigned MsDrawing::read(const U8 *in, FDG *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += MsWordGenerated::read(in + bytes, &out->csp);
    bytes += MsWordGenerated::read(in + bytes, &out->spidCur);
    return bytes;
} // FDG

unsigned MsDrawing::read(const U8 *in, FDGG *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += MsWordGenerated::read(in + bytes, &out->spidMax);
    bytes += MsWordGenerated::read(in + bytes, &out->cidcl);
    bytes += MsWordGenerated::read(in + bytes, &out->cspSaved);
    bytes += MsWordGenerated::read(in + bytes, &out->cdgSaved);
    return bytes;
} // FDGG

unsigned MsDrawing::read(const U8 *in, MSOBFH *out)
{
    U32 shifterU32;
    U16 shifterU16;
    U8 shifterU8;
    unsigned bytes;
    int i;

    i = bytes = shifterU8 = shifterU16 = shifterU32 = 0;
    bytes += MsWordGenerated::read(in + bytes, &shifterU32);
    out->ver = shifterU32;
    shifterU32 >>= 4;
    out->inst = shifterU32;
    shifterU32 >>= 12;
    out->fbt = shifterU32;
    shifterU32 >>= 16;
    bytes += MsWordGenerated::read(in + bytes, &out->cbLength);
    return bytes;
} // MSOBF

unsigned MsDrawing::walkFile(
    const U8 *in,
    U32 count,
    U32 shapeId,
    const U8 *pictureStream,
    MSOBLIPTYPE *pictureType,
    U32 *pictureLength,
    const U8 **pictureData)
{
    unsigned bytes = 0;
    MSOBFH data;
    FBSE bse;
    FDG dg;

    while (bytes < count)
    {
        bytes += MsDrawing::read(in + bytes, &data);
        switch (data.fbt)
        {
        case msofbtDggContainer:
            bytes += MsDrawing::walkFile(
                        in + bytes,
                        data.cbLength,
                        shapeId,
                        pictureStream,
                        pictureType,
                        pictureLength,
                        pictureData);

            // TBD: There is an odd number of bytes being passed in...discard
            // it.

            if (count % MsWordGenerated::sizeof_U32)
                bytes++;
            break;
        case msofbtBstoreContainer:
        case msofbtDgContainer:
            bytes += MsDrawing::walkFile(
                        in + bytes,
                        data.cbLength,
                        shapeId,
                        pictureStream,
                        pictureType,
                        pictureLength,
                        pictureData);
            break;
        case msofbtDgg:
            MsDrawing::read(in + bytes, &dg);
            bytes += data.cbLength;
            break;
        case msofbtBSE:
            MsDrawing::read(in + bytes, &bse);
            if (bse.btWin32 != bse.btMacOS)
            {
                // TBD: work out how to support this.

                kdError() << "MsDrawing:walkFile: inconsistent Blip type: " <<
                    bse.btWin32 << " != " << bse.btMacOS << endl;
            }
            *pictureType = static_cast<MsDrawing::MSOBLIPTYPE>(bse.btWin32);
            walkFile(
                pictureStream + bse.foDelay,
                bse.size,
                shapeId,
                0L,
                0L,
                pictureLength,
                pictureData);
            bytes += data.cbLength;
            break;
        case msofbtDg:
            MsDrawing::read(in + bytes, &dg);
            kdDebug(s_area) << "MsDrawing:walkFile: found DG: " << dg.spidCur << endl;
            if (dg.spidCur == shapeId)
            {
                // We are done!

                bytes = count;
            }
            else
            {
                bytes += data.cbLength;
            }
            break;
        case msofbtSplitMenuColors:
            bytes += data.cbLength;
            break;
        default:
            if ((data.fbt >= msofbtBlipFirst) ||
                (data.fbt <= msofbtBlipLast))
            {
                int headerBytesToSkip = -1;

                switch (data.inst)
                {
                case msobiWMF:
                case msobiEMF:
                case msobiPICT:
                    headerBytesToSkip = 34;
                    break;
                case msobiPNG:
                case msobiJFIF:
                //case msobiJPEG:
                case msobiDIB:
                    headerBytesToSkip = 17;
                    break;
                default:
                    kdError(s_area) << "MsDrawing:walkFile: unknown Blip signature: " <<
                        data.inst << endl;
                    break;
                }

                // Only return data we can vouch for!

                if (headerBytesToSkip >= 0)
                {
                    *pictureLength = data.cbLength - headerBytesToSkip;
                    *pictureData = in + bytes + headerBytesToSkip;
                }
            }
            else
            {
                kdDebug(s_area) << "MsDrawing:walkFile: unsupported opcode: 0x" <<
                    QString::number(data.fbt, 16) << endl;
            }
            bytes += data.cbLength;
            break;
        }
    }
    return bytes;
}
