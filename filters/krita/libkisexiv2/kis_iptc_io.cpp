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

#include "kis_iptc_io.h"

#include <exiv2/iptc.hpp>

#include "kis_exiv2.h"

#include <kis_meta_data_store.h>
#include <kis_meta_data_entry.h>
#include <kis_meta_data_value.h>
#include <kis_meta_data_schema.h>

struct KisIptcIO::Private {
};

// ---- Implementation of KisExifIO ----//
KisIptcIO::KisIptcIO() : d(new Private)
{
}

bool KisIptcIO::saveTo(KisMetaData::Store* store, QIODevice* ioDevice) const
{
    return false;
}

bool KisIptcIO::canSaveAllEntries(KisMetaData::Store* store) const
{
    return false;
}

#define ADD_ENTRY(name, schema) \
    store->addEntry(KisMetaData::Entry(name, schema, exivValueToKMDValue(it->getValue())));

bool KisIptcIO::loadFrom(KisMetaData::Store* store, QIODevice* ioDevice) const
{
    kDebug() << "Loading IPTC Tags" << endl;
    ioDevice->open(QIODevice::ReadOnly);
    QByteArray arr = ioDevice->readAll();
    Exiv2::IptcData iptcData;
    iptcData.load((const Exiv2::byte*)arr.data(), arr.size());
    kDebug() << "There are " << iptcData.count() << " entries in the IPTC section" << endl;
    // Get schemas
    const KisMetaData::Schema* xmpRightsSchema = KisMetaData::SchemaRegistry::instance()->schemaFromUri(KisMetaData::Schema::XMPRightsSchemaUri);
    const KisMetaData::Schema* dcSchema = KisMetaData::SchemaRegistry::instance()->schemaFromUri(KisMetaData::Schema::DublinCoreSchemaUri);
    const KisMetaData::Schema* photoshopSchema = KisMetaData::SchemaRegistry::instance()->schemaFromUri(KisMetaData::Schema::PhotoshopSchemaUri);
    const KisMetaData::Schema* iptcSchema = KisMetaData::SchemaRegistry::instance()->schemaFromUri(KisMetaData::Schema::IPTCSchemaUri);
    for(Exiv2::IptcMetadata::const_iterator it = iptcData.begin();
        it != iptcData.end(); ++it)
    {
        kDebug() << "Reading info for key " << it->key().c_str() << endl;
        if(it->tagName() == "City" or it->tagName() == "Country" or it->tagName() == "DateCreated" or it->tagName() == "Headline" or it->tagName() == "Instructions" or it->tagName() == "Source")
        {
            ADD_ENTRY(it->tagName().c_str(), photoshopSchema)
        } else if(it->tagName() == "JobID")
        {
            ADD_ENTRY("TransmissionReference", photoshopSchema);
        } else if(it->tagName() == "Provider")
        {
            ADD_ENTRY("Credit", photoshopSchema);
        } else if(it->tagName() == "Province-State")
        {
            ADD_ENTRY("State", photoshopSchema);
        } else if(it->tagName() == "RightsUsageTerms")
        {
            ADD_ENTRY("UsageTerms", xmpRightsSchema);
        } else if(it->tagName() == "CopyrightNotice")
        {
            ADD_ENTRY("rights", dcSchema);
        } else if(it->tagName() == "Description")
        {
            ADD_ENTRY("description", dcSchema);
        } else if(it->tagName() == "Keywords")
        {
            ADD_ENTRY("subject", dcSchema);
        } else if(it->tagName() == "DescriptionWriter")
        {
            ADD_ENTRY("CaptionWriter", photoshopSchema);
        } else if(it->tagName() == "Title")
        {
            ADD_ENTRY("title", dcSchema);
        } else if(it->tagName() == "Creator")
        {
            ADD_ENTRY("creator", dcSchema);
        } else if(it->tagName() == "CreatorContactInfo")
        {
            // TODO
        } else if(it->tagName() == "CreatorJobTitle" )
        {
            ADD_ENTRY("AuthorsPosition", photoshopSchema)
        } else {
            ADD_ENTRY(it->tagName().c_str(), iptcSchema);
        }
    }
    return false;
}
