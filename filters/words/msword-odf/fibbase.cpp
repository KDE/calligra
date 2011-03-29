/* This file is part of the KOffice project
   Copyright (C) 2011 Matus Uzak <matus.uzak@ixonos.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include <kdebug.h>

#include "generated/leinputstream.h"
#include "fibbase.h"

FibBase::FibBase(void)
{
    clear();
}

FibBase::FibBase(LEInputStream& in)
{
    clear();
    if (!read(in)) {
        kError(30513) << "FibBase reading FAILED!";
    }
}

bool FibBase::read(LEInputStream& in)
{
    LEInputStream::Mark m = in.setMark();
    quint8 shifterU8;
    quint16 shifterU16;

    try {
        wIdent = in.readuint16();
        nFib = in.readuint16();
        unused = in.readuint16();
        lid = in.readuint16();
        pnNext = in.readuint16();

        shifterU16 = in.readuint16();
        fDot = shifterU16;
        shifterU16 >>= 1;
        fGlsy = shifterU16;
        shifterU16 >>= 1;
        fComplex = shifterU16;
        shifterU16 >>= 1;
        fHasPic = shifterU16;
        shifterU16 >>= 1;
        cQuickSaves = shifterU16;
        shifterU16 >>= 4;
        fEncrypted = shifterU16;
        shifterU16 >>= 1;
        fWhichTblStm = shifterU16;
        shifterU16 >>= 1;
        fReadOnlyRecommended = shifterU16;
        shifterU16 >>= 1;
        fWriteReservation = shifterU16;
        shifterU16 >>= 1;
        fExtChar = shifterU16;
        shifterU16 >>= 1;
        fLoadOverride = shifterU16;
        shifterU16 >>= 1;
        fFarEast = shifterU16;
        shifterU16 >>= 1;
        fObfuscated = shifterU16;

        nFibBack = in.readuint16();
        lKey = in.readuint32();
        envr = in.readuint8();

        shifterU8 = in.readuint8();
        fMac = shifterU8;
        shifterU8 >>= 1;
        fEmptySpecial = shifterU8;
        shifterU8 >>= 1;
        fLoadOverridePage = shifterU8;
        shifterU8 >>= 1;
        reserved1 = shifterU8;
        shifterU8 >>= 1;
        reserved2 = shifterU8;
        shifterU8 >>= 1;
        fSpare0 = shifterU8;

        reserved3 = in.readuint16();
        reserved4 = in.readuint16();
        reserved5 = in.readuint32();
        reserved6 = in.readuint32();
    } catch (IOException _e) {
        kError(30513) << _e.msg;
        return false;
    }
    try {
        in.rewind(m);
    } catch (IOException _e) {
        kDebug(30513) << _e.msg;
    }
    return true;
}

void FibBase::clear(void)
{
    wIdent=0;
    nFib=0;
    unused=0;
    lid=0;
    pnNext=0;
    fDot=0;
    fGlsy=0;
    fComplex=0;
    fHasPic=0;
    cQuickSaves=0;
    fEncrypted=0;
    fWhichTblStm=0;
    fReadOnlyRecommended=0;
    fWriteReservation=0;
    fExtChar=0;
    fLoadOverride=0;
    fFarEast=0;
    fObfuscated=0;
    nFibBack=0;
    lKey=0;
    envr=0;
    fMac=0;
    fEmptySpecial=0;
    fLoadOverridePage=0;
    reserved1=0;
    reserved2=0;
    fSpare0=0;
    reserved3=0;
    reserved4=0;
    reserved5=0;
    reserved6=0;
}
