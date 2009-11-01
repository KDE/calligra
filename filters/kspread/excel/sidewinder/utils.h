/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003-2005 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006,2009 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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
   Boston, MA 02110-1301, USA
 */
#ifndef SWINDER_UTILS_H
#define SWINDER_UTILS_H

#include "value.h"

namespace Swinder {

Value errorAsValue( int errorCode );

static inline unsigned long readU16( const void* p )
{
    const unsigned char* ptr = (const unsigned char*) p;
    return ptr[0]+(ptr[1]<<8);
}

static inline long readS16( const void* p )
{
    long val = readU16( p );
    if (val & 0x8000)
        val = val - 0x10000;
    return val;
}

static inline long readS8( const void* p )
{
    const unsigned char* ptr = (const unsigned char*) p;
    long val = *ptr;
    if (val & 0x80)
        val = val - 0x100;
    return val;
}

static inline unsigned long readU32( const void* p )
{
    const unsigned char* ptr = (const unsigned char*) p;
    return ptr[0]+(ptr[1]<<8)+(ptr[2]<<16)+(ptr[3]<<24);
}

typedef double& data_64;
static inline void convert_64 (data_64 convert)
{
    register unsigned char temp;
    register unsigned int u_int_temp;
    temp = ((unsigned char*)&convert)[0];
    ((unsigned char*)&convert)[0] = ((unsigned char*)&convert)[3];
    ((unsigned char*)&convert)[3] = temp;
    temp = ((unsigned char*)&convert)[1];
    ((unsigned char*)&convert)[1] = ((unsigned char*)&convert)[2];
    ((unsigned char*)&convert)[2] = temp;
    temp = ((unsigned char*)&convert)[4];
    ((unsigned char*)&convert)[4] = ((unsigned char*)&convert)[7];
    ((unsigned char*)&convert)[7] = temp;
    temp = ((unsigned char*)&convert)[5];
    ((unsigned char*)&convert)[5] = ((unsigned char*)&convert)[6];
    ((unsigned char*)&convert)[6] = temp;

    u_int_temp = ((unsigned int *)&convert)[0];
    ((unsigned int *)&convert)[0] = ((unsigned int *)&convert)[1];
    ((unsigned int *)&convert)[1] = u_int_temp;
}

static inline bool isLittleEndian(void)
{
    long i = 0x44332211;
    unsigned char* a = (unsigned char*) &i;
    return ( *a == 0x11 );
}


// FIXME check that double is 64 bits
static inline double readFloat64( const void*p )
{
    const double* ptr = (const double*) p;
    double num = 0.0;
    num = *ptr;

    if( !isLittleEndian() )
        convert_64( num );

    return num;
}

} // namespace Swinder

#endif // SWINDER_UTILS_H
