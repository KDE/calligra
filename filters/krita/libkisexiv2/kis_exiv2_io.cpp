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

// ---- Generic convertion functions ---- //

// Convert an exiv value to a KisMetaData value
KisMetaData::Value exivValueToKMDValue( const Exiv2::Value::AutoPtr value )
{
    switch(value->typeId())
    {
        case Exiv2::invalid6:
        case Exiv2::invalidTypeId:
        case Exiv2::lastTypeId:
        case Exiv2::directory:
            kDebug() << "Invalid value : " << value->typeId() << " value = " << value->toString().c_str() << endl;
            return KisMetaData::Value();
        case Exiv2::undefined:
        {
            kDebug() << "Undefined value : " << value->typeId() << " value = " << value->toString().c_str() << endl;
            QByteArray array( value->count() ,0);
            value->copy((Exiv2::byte*)array.data(), Exiv2::invalidByteOrder);
            return KisMetaData::Value(array);
        }
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
            return KisMetaData::Value( value->toString().c_str() );
        case Exiv2::unsignedRational:
            return KisMetaData::Value( KisMetaData::UnsignedRational( value->toRational().first , value->toRational().second ) );
        case Exiv2::signedRational:
            return KisMetaData::Value( KisMetaData::SignedRational( value->toRational().first , value->toRational().second ));
        case Exiv2::date:
        case Exiv2::time:
            return KisMetaData::Value(QDateTime::fromString(value->toString().c_str(), Qt::ISODate));
    }
    kDebug() << "Unknown type id : " << value->typeId() << " value = " << value->toString().c_str() << endl;
    Q_ASSERT(false); // This point must never be reached !
    return KisMetaData::Value();
}

// Convert a QtVariant to an Exiv value
Exiv2::Value* variantToExivValue( const QVariant& variant, Exiv2::TypeId type )
{
    switch(type)
    {
        case Exiv2::undefined:
            return new Exiv2::DataValue( (Exiv2::byte*)variant.toByteArray().data(), variant.toByteArray().size() );
        case Exiv2::unsignedShort:
            return new Exiv2::ValueType<uint16_t>(variant.toInt(0));
        case Exiv2::unsignedLong:
            return new Exiv2::ValueType<uint32_t>(variant.toUInt(0));
        case Exiv2::signedShort:
            return new Exiv2::ValueType<int16_t>(variant.toInt(0));
        case Exiv2::signedLong:
            return new Exiv2::ValueType<int32_t>(variant.toInt(0));
        case Exiv2::asciiString:
            if(variant.type() == QVariant::DateTime)
            {
                return new Exiv2::AsciiValue(qPrintable(variant.toDateTime().toString("yyyy:MM:dd hh:mm:ss")));
            }
            else
                return new Exiv2::AsciiValue(qPrintable(variant.toString()));
        case Exiv2::string:
        {
            if(variant.type() == QVariant::DateTime)
            {
                return new Exiv2::StringValue(qPrintable(variant.toDateTime().toString("yyyy:MM:dd hh:mm:ss")));
            }
            else
                return new Exiv2::StringValue(qPrintable(variant.toString()));
        }
        case Exiv2::comment:
            return new Exiv2::CommentValue(qPrintable(variant.toString()));
        default:
            Q_ASSERT(false);
            return 0;
    }
}

// Convert a KisMetaData to an Exiv value
Exiv2::Value* kmdValueToExivValue( const KisMetaData::Value& value, Exiv2::TypeId type )
{
    switch(value.type())
    {
        case KisMetaData::Value::Invalid:
            return &*Exiv2::Value::create( Exiv2::invalidTypeId);
        case KisMetaData::Value::Variant:
        {
            return variantToExivValue( value.asVariant(), type );
        }
        case KisMetaData::Value::SignedRational:
            Q_ASSERT( type == Exiv2::signedRational );
            return new Exiv2::ValueType<Exiv2::Rational>(Exiv2::Rational( value.asSignedRational().numerator, value.asSignedRational().denominator ) );
        case KisMetaData::Value::UnsignedRational:
        {
            Q_ASSERT( type == Exiv2::unsignedRational );
            return new Exiv2::ValueType<Exiv2::URational>(Exiv2::URational( value.asUnsignedRational().numerator, value.asUnsignedRational().denominator ) );
        }
        default:
            Q_ASSERT(false);
    }
    return 0;
}

// ---- Exception convertion functions ---- //

// convert ExifVersion and FlashpixVersion to a KisMetaData value
KisMetaData::Value exifVersionToKMDValue(const Exiv2::Value::AutoPtr value)
{
    const Exiv2::DataValue* dvalue = dynamic_cast<const Exiv2::DataValue*>(&*value);
    Q_ASSERT(dvalue);
    QByteArray array(dvalue->count(),0);
    dvalue->copy( (Exiv2::byte*)array.data());
    return KisMetaData::Value(QString(array));
}

// convert from KisMetaData value to ExifVersion and FlashpixVersion
Exiv2::Value* kmdValueToExifVersion(const KisMetaData::Value& value)
{
    Exiv2::DataValue* dvalue = new Exiv2::DataValue;
    QString ver = value.asVariant().toString();
    dvalue->read( (const Exiv2::byte*)ver.toAscii().data(), ver.size());
    return dvalue;
}

// Convert an exif array of integer string to a KisMetaData array of integer
KisMetaData::Value exifArrayToKMDIntOrderedArray(const Exiv2::Value::AutoPtr value)
{
    QList<KisMetaData::Value> v;
    const Exiv2::DataValue* dvalue = dynamic_cast<const Exiv2::DataValue*>(&*value);
    Q_ASSERT(dvalue);
    QByteArray array(dvalue->count(),0);
    dvalue->copy( (Exiv2::byte*)array.data());
    for(int i = 0; i < array.size(); i++)
    {
        QChar c((char)array[i]);
        v.push_back(KisMetaData::Value(QString(c).toInt(0)));
    }
    return KisMetaData::Value(v, KisMetaData::Value::OrderedArray);
}

// Convert a KisMetaData array of integer to an exif array of integer string
Exiv2::Value* kmdIntOrderedArrayToExifArray(const KisMetaData::Value& value)
{
    QList<KisMetaData::Value> v = value.asArray();
    QString s;
    for(QList<KisMetaData::Value>::iterator it = v.begin();
        it != v.end(); ++it)
    {
        int val = it->asVariant().toInt(0);
        s += QString::number(val);
    }
    return new Exiv2::DataValue((const Exiv2::byte*)s.toAscii().data(), s.toAscii().size());
}

QDateTime exivValueToDateTime( const Exiv2::Value::AutoPtr value )
{
    return QDateTime::fromString(value->toString().c_str(), Qt::ISODate);
}

KisMetaData::Value exifOECFToKMDOECFStructure(const Exiv2::Value::AutoPtr value)
{
    QMap<QString, KisMetaData::Value> oecfStructure;
    const Exiv2::DataValue* dvalue = dynamic_cast<const Exiv2::DataValue*>(&*value);
    Q_ASSERT(dvalue);
    QByteArray array(dvalue->count(),0);
    dvalue->copy( (Exiv2::byte*)array.data());
    int columns = (reinterpret_cast<quint16*>(array.data()))[0];
    int rows = (reinterpret_cast<quint16*>(array.data()))[1];
    oecfStructure["Columns"] = KisMetaData::Value(columns);
    oecfStructure["Rows"] = KisMetaData::Value(rows);
    int index = 4;
    QList<KisMetaData::Value> names;
    for(int i = 0; i < columns; i++)
    {
        int lastIndex = array.indexOf((char)0, index);
        QString name = array.mid(index, lastIndex - index );
        if(index != lastIndex)
        {
            index = lastIndex + 1;
            kDebug() << "Name [" << i << "] = " << name << endl;
            names.append( KisMetaData::Value(name) );
        } else {
            names.append( KisMetaData::Value("") );
        }
    }
    oecfStructure["Names"] = KisMetaData::Value(names, KisMetaData::Value::OrderedArray);
    QList<KisMetaData::Value> values;
    qint16* dataIt = reinterpret_cast<qint16*>(array.data() + index );
    for(int i = 0; i < columns; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            values.append(KisMetaData::Value(KisMetaData::SignedRational( dataIt[0], dataIt[1] ) ) );
            dataIt += 8;
        }
    }
    oecfStructure["Values"] = KisMetaData::Value(values, KisMetaData::Value::OrderedArray);
    return KisMetaData::Value(oecfStructure);
}

Exiv2::Value* kmdOECFStructureToExifOECF(const KisMetaData::Value& value)
{
    QMap<QString, KisMetaData::Value> oecfStructure = value.asStructure();
    quint16 columns = oecfStructure["Columns"].asVariant().toInt(0);
    quint16 rows = oecfStructure["Rows"].asVariant().toInt(0);
    
    QList<KisMetaData::Value> names = oecfStructure["Names"].asArray();
    QList<KisMetaData::Value> values = oecfStructure["Values"].asArray();
    Q_ASSERT(columns*rows == values.size());
    int length = 4 + rows*columns*8; // The 4 byte for storing rows/columns and the rows*columns*sizeof(rational)
    bool saveNames = (not names.empty() and names[0].asVariant().toString().size() > 0);
    if(saveNames)
    {
        for(int i = 0; i < columns; i++)
        {
            length += names[i].asVariant().toString().size() + 1;
        }
    }
    QByteArray array(length, 0);
    (reinterpret_cast<quint16*>(array.data()))[0] = columns;
    (reinterpret_cast<quint16*>(array.data()))[1] = rows;
    int index = 4;
    if(saveNames)
    {
        for(int i = 0; i < columns; i++)
        {
            QByteArray name = names[i].asVariant().toString().toAscii();
            name.append((char)0);
            memcpy( array.data() + index, name.data(), name.size());
            index += name.size();
        }
    }
    qint16* dataIt = reinterpret_cast<qint16*>(array.data() + index );
    for(QList<KisMetaData::Value>::iterator it = values.begin();
        it != values.end(); it++)
    {
        dataIt[0] = it->asSignedRational().numerator;
        dataIt[1] = it->asSignedRational().denominator;
        dataIt+=2;
    }
    return new Exiv2::DataValue((const Exiv2::byte*)array.data(), array.size());
}

KisMetaData::Value deviceSettingDescriptionExifToKMD(const Exiv2::Value::AutoPtr value)
{
    QMap<QString, KisMetaData::Value> deviceSettingStructure;
    const Exiv2::DataValue* dvalue = dynamic_cast<const Exiv2::DataValue*>(&*value);
    Q_ASSERT(dvalue);
    QByteArray array(dvalue->count(),0);
    dvalue->copy( (Exiv2::byte*)array.data());
    int columns = (reinterpret_cast<quint16*>(array.data()))[0];
    int rows = (reinterpret_cast<quint16*>(array.data()))[1];
    deviceSettingStructure["Columns"] = KisMetaData::Value(columns);
    deviceSettingStructure["Rows"] = KisMetaData::Value(rows);
    QList<KisMetaData::Value> settings;
    int index = 4;
    for(int i = 0; i < columns * rows; i++)
    {
        int lastIndex = array.indexOf((char)0, index);
        QString setting = array.mid(index, lastIndex - index );
        index = lastIndex + 2;
        kDebug() << "Setting [" << i << "] = " << setting << endl;
        settings.append( KisMetaData::Value(setting) );
    }
    deviceSettingStructure["Settings"] = KisMetaData::Value(settings, KisMetaData::Value::OrderedArray);
    return KisMetaData::Value(deviceSettingStructure);
}

Exiv2::Value* deviceSettingDescriptionKMDToExif(const KisMetaData::Value& value)
{
    QMap<QString, KisMetaData::Value> deviceSettingStructure = value.asStructure();
    quint16 columns = deviceSettingStructure["Columns"].asVariant().toInt(0);
    quint16 rows = deviceSettingStructure["Rows"].asVariant().toInt(0);
    
    QList<KisMetaData::Value> settings = deviceSettingStructure["Settings"].asArray();
    Q_ASSERT(columns*rows == settings.size());
    QByteArray array(4,0);
    (reinterpret_cast<quint16*>(array.data()))[0] = columns;
    (reinterpret_cast<quint16*>(array.data()))[1] = rows;
    for(int i = 0; i < columns * rows; i++)
    {
        QByteArray setting = settings[i].asVariant().toString().toAscii();
        setting.append((char)0);
        setting.append((char)0);
        array.append( setting);
    }
    return new Exiv2::DataValue((const Exiv2::byte*)array.data(), array.size());
}


// ---- Implementation of KisExiv2IO ----//
KisExiv2IO::KisExiv2IO() : d(new Private)
{
}

bool KisExiv2IO::saveTo(KisMetaData::Store* store, QIODevice* ioDevice)
{
    ioDevice->open(QIODevice::WriteOnly);
    Exiv2::ExifData exifData;
    
    for(QHash<QString, KisMetaData::Entry>::const_iterator it = store->begin();
        it != store->end(); ++it )
    {
        const KisMetaData::Entry& entry = *it;
        QString exivKey = "";
        if(entry.schema()->uri() == KisMetaData::Schema::TIFFSchema->uri())
        {
            exivKey = "Exif.Image." + entry.name();
        } else if(entry.schema()->uri() == KisMetaData::Schema::EXIFSchema->uri())
        { // Seperate between exif and gps
            if( entry.name().left(3) == "GPS")
            {
                exivKey = "Exif.GPS." + entry.name();
            } else
            {
                exivKey = "Exif.Photo." + entry.name();
            }
        } else if(entry.schema()->uri() == KisMetaData::Schema::DublinCoreSchema->uri())
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
        } else if(entry.schema()->uri() == KisMetaData::Schema::XMPSchema->uri())
        {
            if(entry.name() == "ModifyDate")
            {
                exivKey = "Exif.Image.DateTime";
            } else if( entry.name() == "CreatorTool")
            {
                exivKey = "Exif.Image.Software";
            }
        } else if(entry.schema()->uri() == KisMetaData::Schema::MakerNoteSchema->uri())
        {
            if(entry.name() == "RawData")
            {
                exivKey = "Exif.Photo.MakerNote";
            }
        }
        if(exivKey == "")
        {
            kDebug() << entry.qualifiedName() << " is unsavable to EXIF" << endl;
        } else {
            Exiv2::ExifKey exifKey(qPrintable(exivKey));
            Exiv2::Value* v;
            if(exivKey == "Exif.Photo.ExifVersion" or exivKey == "Exif.Photo.FlashpixVersion")
            {
                v = kmdValueToExifVersion( entry.value() );
            } else if(exivKey == "Exif.Photo.FileSource") {
                char s[] = { 0x03 };
                v = new Exiv2::DataValue((const Exiv2::byte*)s, 1);
            } else if(exivKey == "Exif.Photo.SceneType") {
                char s[] = { 0x01 };
                v = new Exiv2::DataValue((const Exiv2::byte*)s, 1);
            } else if(exivKey == "Exif.Photo.ComponentsConfiguration") {
                v = kmdIntOrderedArrayToExifArray(entry.value());
            } else if(exivKey == "Exif.Image.Artist")
            { // load as dc:creator
                KisMetaData::Value creator = entry.value().asArray()[0];
                v = kmdValueToExivValue( creator, Exiv2::ExifTags::tagType( exifKey.tag(), exifKey.ifdId()  ) );
            } else if(exivKey == "Exif.Photo.OECF") {
                v = kmdOECFStructureToExifOECF( entry.value() );
            } else if(exivKey == "Exif.Photo.DeviceSettingDescription") {
                v = deviceSettingDescriptionKMDToExif( entry.value() );
            } else {
                v = kmdValueToExivValue( entry.value(), Exiv2::ExifTags::tagType( exifKey.tag(), exifKey.ifdId()  ) );
            }
            if( v and v->typeId() != Exiv2::invalidTypeId )
            {
//                 kDebug() << "Saving key " << exivKey << " of KMD value " << entry.value() << endl;
                exifData.add(exifKey, v );
            } else {
                kDebug() << "No exif value was created for " << entry.qualifiedName() << " as " << exivKey << " of KMD value " << entry.value() << endl;
            }
        }
    }
    
    Exiv2::DataBuf rawData = exifData.copy();
    ioDevice->write( (const char*) rawData.pData_, rawData.size_);
    ioDevice->close();
    return true;
}

bool KisExiv2IO::canSaveAllEntries(KisMetaData::Store* /*store*/)
{
    return false; // It's a known fact that exif can't save all information, but TODO: write the check
}

bool KisExiv2IO::loadFrom(KisMetaData::Store* store, QIODevice* ioDevice)
{
    ioDevice->open(QIODevice::ReadOnly);
    QByteArray arr = ioDevice->readAll();
    Exiv2::ExifData exifData;
    exifData.load((const Exiv2::byte*)arr.data(), arr.size());
    kDebug() << "There are " << exifData.count() << " entries in the exif section" << endl;
    const KisMetaData::Schema* tiffSchema = store->createSchema(KisMetaData::Schema::TIFFSchema);
    const KisMetaData::Schema* exifSchema = store->createSchema(KisMetaData::Schema::EXIFSchema);
    const KisMetaData::Schema* dcSchema = store->createSchema(KisMetaData::Schema::DublinCoreSchema);
    const KisMetaData::Schema* xmpSchema = store->createSchema(KisMetaData::Schema::XMPSchema);
    for(Exiv2::ExifMetadata::const_iterator it = exifData.begin();
        it != exifData.end(); ++it)
    {
        kDebug() << "Reading info for key " << it->key().c_str() << endl;
        if(it->key() == "Exif.Photo.StripOffsets" or it->key() == "RowsPerStrip" or it->key() == "StripByteCounts" or it->key() == "JPEGInterchangeFormat" or it->key() == "JPEGInterchangeFormatLength")
        {
            kDebug() << it->key().c_str() << " is ignored" << endl;
        } if(it->key() == "Exif.Photo.MakerNote") {
            const KisMetaData::Schema* makerNoteSchema = store->createSchema(KisMetaData::Schema::MakerNoteSchema);
            store->addEntry(KisMetaData::Entry("RawData", makerNoteSchema, exivValueToKMDValue(it->getValue())));
        } else if(it->key() == "Exif.Image.DateTime")
        { // load as xmp:ModifyDate
            store->addEntry( KisMetaData::Entry("ModifyDate", xmpSchema, KisMetaData::Value(exivValueToDateTime(it->getValue())) ));
        } else if(it->key() == "Exif.Image.ImageDescription")
        { // load as "dc:description"
            store->addEntry( KisMetaData::Entry("description", dcSchema, exivValueToKMDValue(it->getValue())) );
        } else if(it->key() == "Exif.Image.Software")
        { // load as "xmp:CreatorTool"
            store->addEntry(KisMetaData::Entry("CreatorTool", xmpSchema, exivValueToKMDValue(it->getValue()) ));
        } else if(it->key() == "Exif.Image.Artist")
        { // load as dc:creator
            QList<KisMetaData::Value> creators;
            creators.push_back(exivValueToKMDValue(it->getValue()));
            store->addEntry(KisMetaData::Entry("creator", dcSchema, KisMetaData::Value(creators, KisMetaData::Value::OrderedArray) ));
        } else if(it->key() == "Exif.Image.Copyright")
        { // load as dc:rights
            store->addEntry(KisMetaData::Entry("rights", dcSchema, exivValueToKMDValue(it->getValue()) ));
        } else if(it->groupName() == "Image") {
            // Tiff tags
            store->addEntry(KisMetaData::Entry(it->tagName().c_str(), tiffSchema, exivValueToKMDValue(it->getValue()))) ;
        } else if(it->groupName() == "Photo" or (it->groupName() == "GPS") ) {
            // Exif tags (and GPS tags)
            KisMetaData::Value v;
            if(it->key() == "Exif.Photo.ExifVersion" or it->key() == "Exif.Photo.FlashpixVersion")
            {
                v = exifVersionToKMDValue(it->getValue());
            } else if(it->key() == "Exif.Photo.FileSource") {
                v = KisMetaData::Value(3);
            } else if(it->key() == "Exif.Photo.SceneType") {
                v = KisMetaData::Value(1);
            } else if(it->key() == "Exif.Photo.ComponentsConfiguration") {
                v = exifArrayToKMDIntOrderedArray(it->getValue());
            } else if(it->key() == "Exif.Photo.OECF") {
                v = exifOECFToKMDOECFStructure(it->getValue());
            } else if(it->key() == "Exif.Photo.DateTimeDigitized" or it->key() == "Exif.Photo.DateTimeOriginal") {
                v = KisMetaData::Value(exivValueToDateTime(it->getValue()));
            } else if(it->key() == "Exif.Photo.DeviceSettingDescription" ) {
                v = deviceSettingDescriptionExifToKMD(it->getValue());
            }
            else {
                v = exivValueToKMDValue(it->getValue());
            }
            store->addEntry(KisMetaData::Entry(it->tagName().c_str(), exifSchema, v ));
        } else if(it->groupName() == "Thumbnail") {
            kDebug() << "Ignoring thumbnail tag : " << it->key().c_str() << endl;
        } else {
            kDebug() << "Unknown exif tag, can't load: " << it->key().c_str() << endl;
        }
    }
    store->debugDump();
    ioDevice->close();
    return true;
}
