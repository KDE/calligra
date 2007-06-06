/*
 *  Copyright (c) 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_exiv2_io.h"

#include <exiv2/exif.hpp>

#include <QIODevice>
#include <QByteArray>
#include <QVariant>
#include <QDateTime>
#include <QDate>
#include <QTime>

#include <kdebug.h>

#include <kis_meta_data_store.h>
#include <kis_meta_data_entry.h>
#include <kis_meta_data_value.h>
#include <kis_meta_data_schema.h>

struct KisExiv2IO::Private {
};

KisMetaData::Value exivValueToKMDValue( const Exiv2::Value::AutoPtr value )
{
    switch(value->typeId())
    {
        case Exiv2::invalid6:
        case Exiv2::undefined:
        case Exiv2::invalidTypeId:
        case Exiv2::lastTypeId:
        case Exiv2::directory:
//             kDebug() << "Invalid value : " << value.typeId() << " value = " << value.toString() << endl;
            return KisMetaData::Value();
        case Exiv2::unsignedByte:
        case Exiv2::unsignedShort:
        case Exiv2::unsignedLong:
        case Exiv2::signedShort:
        case Exiv2::signedLong:
        {
            if(value->count() ==1)
            {
                return KisMetaData::Value( (int)value->toLong() );
            } else {
                QList<KisMetaData::Value> array;
                for(int i = 0; i < value->count(); i++)
                    array.push_back( KisMetaData::Value((int)value->toLong(i) ) );
                return KisMetaData::Value( array, KisMetaData::Value::UnorderedArray );
            }
        }
        case Exiv2::asciiString:
        case Exiv2::string:
        case Exiv2::comment: // look at kexiv2 for the problem about decoding correctly that tag
        case Exiv2::unsignedRational: // TODO: check that exiv correctly convert it
        case Exiv2::signedRational:
            return KisMetaData::Value( value->toString().c_str() );
        case Exiv2::date:
        case Exiv2::time:
            return KisMetaData::Value(QDateTime::fromString(value->toString().c_str(), Qt::ISODate));
    }
}

Exiv2::Value* variantToExivValue( const QVariant& variant )
{
    switch(variant.type())
    {
        case QVariant::Bool:
        case QVariant::Int:
        case QVariant::UInt:
            return new Exiv2::ValueType<int32_t>(variant.toInt(0));
        case QVariant::Date:
        {
            QDate d = variant.toDate();
            return new Exiv2::DateValue(d.year(), d.month(), d.day());
        }
        case QVariant::Time:
        {
            QTime t = variant.toTime();
            return new Exiv2::TimeValue(t.hour(), t.minute(), t.second(), t.hour(), t.minute());
        }
        case QVariant::DateTime:
        {
            QDateTime d = variant.toDateTime();
            return new Exiv2::StringValue(d.toString("yyyy:MM:dd hh:mm:ss").ascii());
        }
        case QVariant::String:
        default:
            return new Exiv2::StringValue(variant.toString().ascii());
    }
}

Exiv2::Value* kmdValueToExivValue( const KisMetaData::Value& value )
{
    switch(value.type())
    {
        case KisMetaData::Value::Invalid:
            return &*Exiv2::Value::create( Exiv2::invalidTypeId);
        case KisMetaData::Value::Variant:
        {
            return variantToExivValue( value.asVariant() );
        }
    }
    return 0;
}


KisExiv2IO::KisExiv2IO() : d(new Private)
{
}

bool KisExiv2IO::saveTo(KisMetaData::Store* store, QIODevice* ioDevice)
{
    Exiv2::ExifData exifData;
    
    for(QHash<QString, KisMetaData::Entry>::const_iterator it = store->begin();
        it != store->end(); ++it )
    {
        const KisMetaData::Entry& entry = *it;
        QString exivKey = "";
        if(entry.schema()->uri() == KisMetaData::Schema::UriTIFF)
        {
            exivKey = "Exif.Image." + entry.name();
        } else if(entry.schema()->uri() == KisMetaData::Schema::UriEXIF)
        { // Seperate between exif and gps
            if( entry.name().left(3) == "GPS")
            {
                exivKey = "Exif.GPS." + entry.name();
            } else
            {
                exivKey = "Exif.Photo." + entry.name();
            }
            exifData.add(Exiv2::ExifKey(exivKey.ascii()), kmdValueToExivValue( entry.value() ) );
        } else if(entry.schema()->uri() == KisMetaData::Schema::UriDublinCore)
        {
            if(entry.name() == "description")
            {
                exivKey = "Exif.Image.ImageDescription";
            } else if(entry.name() == "creator")
            {
                exivKey = "Exif.Image.Artist";
            } else if(entry.name() == "rights")
            {
                exivKey = "Exif.Image.Copyright";
            }
        } else if(entry.schema()->uri() == KisMetaData::Schema::UriXMP)
        {
            if(entry.name() == "ModifyDate")
            {
                exivKey = "Exif.Image.DateTime";
            } else if( entry.name() == "CreatorTool")
            {
                exivKey = "Exif.Image.Software";
            }
        }
        if(exivKey == "")
        {
            kdDebug() << entry.qualifiedName() << " is unsavable to EXIF" << endl;
        } else {
            exifData.add(Exiv2::ExifKey(exivKey.ascii()), kmdValueToExivValue( entry.value() ) );
        }
    }
    
    Exiv2::DataBuf rawData = exifData.copy();
    ioDevice->write( (const char*) rawData.pData_, rawData.size_);
}

bool KisExiv2IO::canSaveAllEntries(KisMetaData::Store* /*store*/)
{
    return false; // It's a known fact that exif can't save all information, but TODO: write the check
}

bool KisExiv2IO::loadFrom(KisMetaData::Store* store, QIODevice* ioDevice)
{
    QByteArray arr = ioDevice->readAll();
    Exiv2::ExifData exifData;
    exifData.load((const Exiv2::byte*)arr.data(), arr.size());
    const KisMetaData::Schema* tiffSchema = store->createSchema(KisMetaData::Schema::UriTIFF, "tiff");
    const KisMetaData::Schema* exifSchema = store->createSchema(KisMetaData::Schema::UriEXIF, "exif");
    const KisMetaData::Schema* dcSchema = store->createSchema(KisMetaData::Schema::UriDublinCore, "dc");
    const KisMetaData::Schema* xmpSchema = store->createSchema(KisMetaData::Schema::UriXMP, "xmp");
    for(Exiv2::ExifMetadata::const_iterator it = exifData.begin();
        it != exifData.end(); ++it)
    {

        if(it->key() == "Exif.Photo.StripOffsets" or it->key() == "RowsPerStrip" or it->key() == "StripByteCounts" or it->key() == "JPEGInterchangeFormat" or it->key() == "JPEGInterchangeFormatLength")
        {
            kDebug() << it->key().c_str() << " is ignored" << endl;
        } if(it->key() == "Exif.Photo.Makernote") {
            kDebug() << " Makernote tag is currently ignored" << endl; // TODO: don't ignore makernotes
        } else if(it->key() == "Exif.Image.DateTime")
        { // load as xmp:ModifyDate
            store->addEntry(KisMetaData::Entry("ModifyDate", xmpSchema, exivValueToKMDValue(it->getValue())));
        } else if(it->key() == "Exif.Image.ImageDescription")
        { // load as "dc:description"
            store->addEntry(KisMetaData::Entry("description", dcSchema, exivValueToKMDValue(it->getValue()) ));
        } else if(it->key() == "Exif.Image.Software")
        { // load as "xmp:CreatorTool"
            store->addEntry(KisMetaData::Entry("CreatorTool", xmpSchema, exivValueToKMDValue(it->getValue()) ));
        } else if(it->key() == "Exif.Image.Artist")
        { // load as dc:creator
            store->addEntry(KisMetaData::Entry("creator", dcSchema, exivValueToKMDValue(it->getValue()) ));
        } else if(it->key() == "Exif.Image.Copyright")
        { // load as dc:rights
            store->addEntry(KisMetaData::Entry("rights", dcSchema, exivValueToKMDValue(it->getValue()) ));
        } else if(it->groupName() == "Image") {
            // Tiff tags
            store->addEntry(KisMetaData::Entry(it->tagName().c_str(), tiffSchema, exivValueToKMDValue(it->getValue()))) ;
        } else if(it->groupName() == "Photo" or (it->groupName() == "GPS") ) {
            // Exif tags (and GPS tags)
            store->addEntry(KisMetaData::Entry(it->tagName().c_str(), exifSchema, exivValueToKMDValue(it->getValue()) ));
        } else {
            kDebug() << "Unknown exif tag, can't load: " << it->key().c_str() << endl;
        }
    }
    store->debugDump();
}
