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

    This file is a description of most structures used in the on-disk format
    of Microsoft Word 97 documents. The only structures missing are those
    which are tricky to autogenerate.
*/

#include <mswordgenerated.h>
#include <string.h>

unsigned MsWordGenerated::read(const U8 *in, U8 *out, unsigned count)
{
    memcpy(out, in, count);
    return count;
}

unsigned MsWordGenerated::read(const U8 *in, U16 *out, unsigned count)
{
    for (unsigned i = 0; i < count; i++)
    {
        *out = (U16)((*(in + 1) << 8) + (*in));
        out++;
        in += 2;
    }
    return count + count;
}

unsigned MsWordGenerated::read(const U8 *in, U32 *out, unsigned count)
{
    for (unsigned i = 0; i < count; i++)
    {
        *out = (U32)((*(in + 3) << 24) + (*(in + 2) << 16) + (*(in + 1) << 8) + (*in));
        out++;
        in += 4;
    }
    return count + count + count + count;
}

unsigned MsWordGenerated::read(const U8 *in, DTTM *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
    }
    return bytes;
} // DTTM

unsigned MsWordGenerated::read(const U8 *in, SHD *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
    }
    return bytes;
} // SHD

unsigned MsWordGenerated::read(const U8 *in, DOPTYPOGRAPHY *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 155);
    }
    return bytes;
} // DOPTYPOGRAPHY

unsigned MsWordGenerated::read(const U8 *in, PRM *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
    }
    return bytes;
} // PRM

unsigned MsWordGenerated::read(const U8 *in, ATRD *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 13);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
    }
    return bytes;
} // ATRD

unsigned MsWordGenerated::read(const U8 *in, ANLD *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 6);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 32);
    }
    return bytes;
} // ANLD

unsigned MsWordGenerated::read(const U8 *in, ANLV *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 6);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
    }
    return bytes;
} // ANLV

unsigned MsWordGenerated::read(const U8 *in, ASUMY *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
    }
    return bytes;
} // ASUMY

unsigned MsWordGenerated::read(const U8 *in, ASUMYI *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
    }
    return bytes;
} // ASUMYI

unsigned MsWordGenerated::read(const U8 *in, BTE *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
    }
    return bytes;
} // BTE

unsigned MsWordGenerated::read(const U8 *in, BKD *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 4);
    }
    return bytes;
} // BKD

unsigned MsWordGenerated::read(const U8 *in, BKF *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
    }
    return bytes;
} // BKF

unsigned MsWordGenerated::read(const U8 *in, BKL *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
    }
    return bytes;
} // BKL

unsigned MsWordGenerated::read(const U8 *in, BRC *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
    }
    return bytes;
} // BRC

unsigned MsWordGenerated::read(const U8 *in, BRC10 *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
    }
    return bytes;
} // BRC10

unsigned MsWordGenerated::read(const U8 *in, CHP *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (DTTM *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 6);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
        bytes += read(in + bytes, (DTTM *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 16);
        bytes += read(in + bytes, (SHD *)(ptr + bytes), 1);
        bytes += read(in + bytes, (BRC *)(ptr + bytes), 1);
    }
    return bytes;
} // CHP

//unsigned MsWordGenerated::read(const U8 *in, CHPX *out, unsigned count)
//{
//    U8 *ptr = (U8 *)out;
//    unsigned bytes = 0;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), cb);
//    }
//    return bytes;
//} // CHPX

unsigned MsWordGenerated::read(const U8 *in, DCS *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
    }
    return bytes;
} // DCS

unsigned MsWordGenerated::read(const U8 *in, DOGRID *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
    }
    return bytes;
} // DOGRID

unsigned MsWordGenerated::read(const U8 *in, DOP *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 6);
        bytes += read(in + bytes, (DTTM *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
    }
    return bytes;
} // DOP

unsigned MsWordGenerated::read(const U8 *in, DOP102 *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
    }
    return bytes;
} // DOP102

unsigned MsWordGenerated::read(const U8 *in, DOP105 *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (DOPTYPOGRAPHY *)(ptr + bytes), 1);
        bytes += read(in + bytes, (DOGRID *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (ASUMYI *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 30);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 5);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 4);
    }
    return bytes;
} // DOP105

unsigned MsWordGenerated::read(const U8 *in, FLD *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
    }
    return bytes;
} // FLD

unsigned MsWordGenerated::read(const U8 *in, FSPA *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 5);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
    }
    return bytes;
} // FSPA

//unsigned MsWordGenerated::read(const U8 *in, FFN *out, unsigned count)
//{
//    U8 *ptr = (U8 *)out;
//    unsigned bytes = 0;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
//        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 36);
//    }
//    return bytes;
//} // FFN

unsigned MsWordGenerated::read(const U8 *in, FIB *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 7);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 16);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 22);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 186);
    }
    return bytes;
} // FIB

unsigned MsWordGenerated::read(const U8 *in, FIBFCLCB *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
    }
    return bytes;
} // FIBFCLCB

unsigned MsWordGenerated::read(const U8 *in, FRD *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
    }
    return bytes;
} // FRD

//unsigned MsWordGenerated::read(const U8 *in, CHPXFKP *out, unsigned count)
//{
//    U8 *ptr = (U8 *)out;
//    unsigned bytes = 0;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
//    }
//    return bytes;
//} // CHPXFKP

//unsigned MsWordGenerated::read(const U8 *in, PAPXFKP *out, unsigned count)
//{
//    U8 *ptr = (U8 *)out;
//    unsigned bytes = 0;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (FC *)(ptr + bytes), fkp.crun+1);
//        bytes += read(in + bytes, (BX *)(ptr + bytes), fkp.crun);
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
//    }
//    return bytes;
//} // PAPXFKP

unsigned MsWordGenerated::read(const U8 *in, LVLF *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 12);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
    }
    return bytes;
} // LVLF

unsigned MsWordGenerated::read(const U8 *in, LSPD *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
    }
    return bytes;
} // LSPD

unsigned MsWordGenerated::read(const U8 *in, LSTF *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 9);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
    }
    return bytes;
} // LSTF

unsigned MsWordGenerated::read(const U8 *in, LFO *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 4);
    }
    return bytes;
} // LFO

unsigned MsWordGenerated::read(const U8 *in, LFOLVL *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 4);
    }
    return bytes;
} // LFOLVL

unsigned MsWordGenerated::read(const U8 *in, OLST *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (ANLV *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 32);
    }
    return bytes;
} // OLST

unsigned MsWordGenerated::read(const U8 *in, NUMRM *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (DTTM *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 18);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 9);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 32);
    }
    return bytes;
} // NUMRM

unsigned MsWordGenerated::read(const U8 *in, PGD *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
    }
    return bytes;
} // PGD

unsigned MsWordGenerated::read(const U8 *in, PHE *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
    }
    return bytes;
} // PHE

unsigned MsWordGenerated::read(const U8 *in, PAP *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 10);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 6);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (LSPD *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (PHE *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 10);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (BRC *)(ptr + bytes), 6);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (SHD *)(ptr + bytes), 1);
        bytes += read(in + bytes, (DCS *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (ANLD *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (DTTM *)(ptr + bytes), 1);
        bytes += read(in + bytes, (NUMRM *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 129);
    }
    return bytes;
} // PAP

//unsigned MsWordGenerated::read(const U8 *in, PAPX *out, unsigned count)
//{
//    U8 *ptr = (U8 *)out;
//    unsigned bytes = 0;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 3);
//        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
//        bytes += read(in + bytes, (character array *)(ptr + bytes), 1);
//    }
//    return bytes;
//} // PAPX

unsigned MsWordGenerated::read(const U8 *in, PICF *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 5);
    }
    return bytes;
} // PICF

unsigned MsWordGenerated::read(const U8 *in, PCD *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (PRM *)(ptr + bytes), 1);
    }
    return bytes;
} // PCD

//unsigned MsWordGenerated::read(const U8 *in, PLCF *out, unsigned count)
//{
//    U8 *ptr = (U8 *)out;
//    unsigned bytes = 0;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 0);
//    }
//    return bytes;
//} // PLCF

unsigned MsWordGenerated::read(const U8 *in, PRM2 *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
    }
    return bytes;
} // PRM2

unsigned MsWordGenerated::read(const U8 *in, RS *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 8);
    }
    return bytes;
} // RS

unsigned MsWordGenerated::read(const U8 *in, RR *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
    }
    return bytes;
} // RR

unsigned MsWordGenerated::read(const U8 *in, SED *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
    }
    return bytes;
} // SED

unsigned MsWordGenerated::read(const U8 *in, SEP *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 10);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (BRC *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (DTTM *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 6);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 11);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 91);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (OLST *)(ptr + bytes), 1);
    }
    return bytes;
} // SEP

//unsigned MsWordGenerated::read(const U8 *in, SEPX *out, unsigned count)
//{
//    U8 *ptr = (U8 *)out;
//    unsigned bytes = 0;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
//        bytes += read(in + bytes, (U8 *)(ptr + bytes), 0);
//    }
//    return bytes;
//} // SEPX

unsigned MsWordGenerated::read(const U8 *in, STSHI *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 9);
    }
    return bytes;
} // STSHI

//unsigned MsWordGenerated::read(const U8 *in, STD *out, unsigned count)
//{
//    U8 *ptr = (U8 *)out;
//    unsigned bytes = 0;
//
//    for (unsigned i = 0; i < count; i++)
//    {
//        bytes += read(in + bytes, (U16 *)(ptr + bytes), 6);
//    }
//    return bytes;
//} // STD

unsigned MsWordGenerated::read(const U8 *in, TBD *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 1);
    }
    return bytes;
} // TBD

unsigned MsWordGenerated::read(const U8 *in, TC *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (BRC *)(ptr + bytes), 4);
    }
    return bytes;
} // TC

unsigned MsWordGenerated::read(const U8 *in, TLP *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
    }
    return bytes;
} // TLP

unsigned MsWordGenerated::read(const U8 *in, TAP *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U8 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (TLP *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 2);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 130);
        bytes += read(in + bytes, (TC *)(ptr + bytes), 64);
        bytes += read(in + bytes, (SHD *)(ptr + bytes), 64);
        bytes += read(in + bytes, (BRC *)(ptr + bytes), 6);
    }
    return bytes;
} // TAP

unsigned MsWordGenerated::read(const U8 *in, FTXBXS *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 3);
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 1);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 2);
    }
    return bytes;
} // FTXBXS

unsigned MsWordGenerated::read(const U8 *in, WKB *out, unsigned count)
{
    U8 *ptr = (U8 *)out;
    unsigned bytes = 0;

    for (unsigned i = 0; i < count; i++)
    {
        bytes += read(in + bytes, (U16 *)(ptr + bytes), 4);
        bytes += read(in + bytes, (U32 *)(ptr + bytes), 1);
    }
    return bytes;
} // WKB

