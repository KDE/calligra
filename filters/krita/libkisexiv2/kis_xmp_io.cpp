/*
 *  Copyright (c) 2008 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
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

#include "kis_xmp_io.h"

#include <string>
#include <exiv2/xmp.hpp>

#include "kis_exiv2.h"

#include <kis_meta_data_store.h>
#include <kis_meta_data_entry.h>
#include <kis_meta_data_value.h>
#include <kis_meta_data_schema.h>

#include <kis_debug.h>

KisXMPIO::KisXMPIO()
{
}

KisXMPIO::~KisXMPIO()
{
}

bool KisXMPIO::saveTo(KisMetaData::Store* store, QIODevice* ioDevice, HeaderType headerType) const
{
    Q_UNUSED(store);
    Q_UNUSED(ioDevice);
    Q_UNUSED(headerType);
    return false;
}

bool KisXMPIO::loadFrom(KisMetaData::Store* store, QIODevice* ioDevice) const
{
    ioDevice->open(QIODevice::ReadOnly);
    dbgFile << "Load XMP Data";
    std::string xmpPacket_;
    QByteArray arr = ioDevice->readAll();
    xmpPacket_.assign(arr.data(), arr.length());
    dbgFile << xmpPacket_.length();
//     dbgFile << xmpPacket_.c_str();
    Exiv2::XmpData xmpData_;
    Exiv2::XmpParser::decode(xmpData_, xmpPacket_);
    for (Exiv2::XmpData::iterator it = xmpData_.begin(); it != xmpData_.end(); ++it) {
        dbgFile << it->key().c_str();
        Exiv2::XmpKey key(it->key());
        dbgFile << key.groupName().c_str() << " " << key.tagName().c_str();
        const KisMetaData::Schema* schema = KisMetaData::SchemaRegistry::instance()->schemaFromPrefix(key.groupName().c_str());
        Q_ASSERT(schema);
        KisMetaData::Value v = exivValueToKMDValue(it->getValue());
        store->addEntry(KisMetaData::Entry(schema, key.tagName().c_str(), v));
    }
    return true;
}
