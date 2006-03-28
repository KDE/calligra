/*
 *  Copyright (c) 2006 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "kis_exif_io.h"

#include <kdebug.h>

extern "C" {
#include <libexif/exif-tag.h>
#include <libexif/exif-log.h>
}

KisExifIO::KisExifIO(KisExifInfo* ei) : m_exifInfo(ei)
{
}

void KisExifIO::readExifFromFile( const char* fileName)
{
    readExifData( exif_data_new_from_file(fileName) );
}

void KisExifIO::readExifFromMem( const unsigned char* data , unsigned int size)
{
    readExifData( exif_data_new_from_data(data, size) );
}

void KisExifIO::saveExifToMem( unsigned char** data, unsigned int *size)
{
    ExifData* exifData = exif_data_new();
    writeExifData( exifData );
    exif_data_save_data( exifData, data, size);
}

ExifValue::ExifType KisExifIO::format2type(ExifFormat format)
{
    switch(format)
    {
        case EXIF_FORMAT_BYTE:
            return ExifValue::EXIF_TYPE_BYTE;
        case EXIF_FORMAT_ASCII:
            return ExifValue::EXIF_TYPE_ASCII;
        case EXIF_FORMAT_SHORT:
            return ExifValue::EXIF_TYPE_SHORT;
        case EXIF_FORMAT_LONG:
            return ExifValue::EXIF_TYPE_LONG;
        case EXIF_FORMAT_RATIONAL:
            return ExifValue::EXIF_TYPE_RATIONAL;
        case EXIF_FORMAT_SBYTE:
            return ExifValue::EXIF_TYPE_SBYTE;
        case EXIF_FORMAT_SSHORT:
            return ExifValue::EXIF_TYPE_SSHORT;
        case EXIF_FORMAT_SLONG:
            return ExifValue::EXIF_TYPE_SLONG;
        case EXIF_FORMAT_SRATIONAL:
            return ExifValue::EXIF_TYPE_SRATIONAL;
        case EXIF_FORMAT_FLOAT:
            return ExifValue::EXIF_TYPE_FLOAT;
        case EXIF_FORMAT_DOUBLE:
            return ExifValue::EXIF_TYPE_DOUBLE;
        default:
        case EXIF_FORMAT_UNDEFINED:
            return ExifValue::EXIF_TYPE_UNDEFINED;
    }
}

void KisExifIO::readExifData( ExifData* exifData)
{
    ExifValue::ByteOrder bO;
    if(exif_data_get_byte_order( exifData) == EXIF_BYTE_ORDER_MOTOROLA)
    {
        bO = ExifValue::BYTE_ORDER_MOTOROLA;
    } else {
        bO = ExifValue::BYTE_ORDER_INTEL;
    }
    static ExifIfd ifds[5] = {
        EXIF_IFD_0,
        EXIF_IFD_1,
        EXIF_IFD_EXIF,
        EXIF_IFD_INTEROPERABILITY,
        EXIF_IFD_GPS
    };
    for(int ifd = 1; ifd < 5; ifd ++)
    {
        ExifContent* content = exifData->ifd[ifds[ifd]];
        kDebug() << "There are " << content->count << " values in ifd=" << ifd << endl;
        for (uint i = 0; i < content->count; i++)
        {
            ExifEntry* entry = content->entries[i];
            QString tagname = exif_tag_get_name ( entry->tag );
            //         QString tagname = exif_tag_get_name_in_ifd ( entry->tag, EXIF_IFD_0 ); TODO: would be better to rely on 0.6.13 when it becomes more common, as it supports better other IFD (GPS and interoperrabilibity)
            ExifValue value( format2type(entry->format), entry->data, entry->size, ifds[ifd], entry->components, bO );
            m_exifInfo->setValue( tagname, value);
        }
    }
    
}

ExifFormat KisExifIO::type2format( ExifValue::ExifType type)
{
    switch(type)
    {
        case ExifValue::EXIF_TYPE_BYTE:
            return EXIF_FORMAT_BYTE;
        case ExifValue::EXIF_TYPE_ASCII:
            return EXIF_FORMAT_ASCII;
        case ExifValue::EXIF_TYPE_SHORT:
            return EXIF_FORMAT_SHORT;
        case ExifValue::EXIF_TYPE_LONG:
            return EXIF_FORMAT_LONG;
        case ExifValue::EXIF_TYPE_RATIONAL:
            return EXIF_FORMAT_RATIONAL;
        case ExifValue::EXIF_TYPE_SBYTE:
            return EXIF_FORMAT_SBYTE;
        case ExifValue::EXIF_TYPE_SSHORT:
            return EXIF_FORMAT_SSHORT;
        case ExifValue::EXIF_TYPE_SLONG:
            return EXIF_FORMAT_SLONG;
        case ExifValue::EXIF_TYPE_SRATIONAL:
            return EXIF_FORMAT_SRATIONAL;
        case ExifValue::EXIF_TYPE_FLOAT:
            return EXIF_FORMAT_FLOAT;
        case ExifValue::EXIF_TYPE_DOUBLE:
            return EXIF_FORMAT_DOUBLE;
        default:
        case ExifValue::EXIF_TYPE_UNDEFINED:
            return EXIF_FORMAT_UNDEFINED;
    }
}


void KisExifIO::writeExifData( ExifData* exifData)
{
    ExifValue::ByteOrder bO;
    if(exif_data_get_byte_order( exifData) == EXIF_BYTE_ORDER_MOTOROLA)
    {
        bO = ExifValue::BYTE_ORDER_MOTOROLA;
    } else {
        bO = ExifValue::BYTE_ORDER_INTEL;
    }
        
    for( KisExifInfo::evMap::const_iterator it = m_exifInfo->begin(); it != m_exifInfo->end(); ++it)
    {
        ExifValue ev = it.data();
        if(ev.ifd() != -1)
        {
            ExifEntry * entry = exif_entry_new();
            ExifContent* content = exifData->ifd[ev.ifd()];
            exif_content_add_entry(content, entry);
            ExifTag tag = exif_tag_from_name( it.key().ascii());
            entry->components = ev.components();
            entry->format = type2format( ev.type());
            entry->tag = tag;
            ev.convertToData(&entry->data, &entry->size, bO);
        }
    }
}
