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
#include "kis_exif_io.h"

#include <kdebug.h>

#include <kdemacros.h>

static bool isInit = false;

// TODO: There must be a better way to do that !
struct KisExiv2
{
    KisExiv2()
    {
        KisMetaData::IOBackendRegistry::instance()->add(new KisIptcIO);
        KisMetaData::IOBackendRegistry::instance()->add(new KisExifIO);
    }
};

static KisExiv2* exiv2 = new KisExiv2();

// void initKisExiv2()
// {
//     if(not isInit)
//     {
//         KisMetaData::IOBackendRegistry::instance()->add(new KisIptcIO);
//         KisMetaData::IOBackendRegistry::instance()->add(new KisExifIO);
//         isInit = true;
//     }
// }
