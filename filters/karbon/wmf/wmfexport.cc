/* This file is part of the KDE project
 * Copyright (c) 2003 thierry lorthiois (lorthioist@wanadoo.fr)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
*/

#include <config.h>
#include <qdom.h>
#include <qcstring.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <koFilterChain.h>
#include <koStoreDevice.h>
#include <core/vdocument.h>

#include "wmfexport.h"

typedef KGenericFactory<WMFExport, KoFilter> WMFExportFactory;
K_EXPORT_COMPONENT_FACTORY( libwmfexport, WMFExportFactory( "wmfexport" ) );


WMFExport::WMFExport( KoFilter *, const char *, const QStringList&) :
        KoFilter() 
{
}

WMFExport::~WMFExport()
{
}

KoFilter::ConversionStatus WMFExport::convert( const QCString& from, const QCString& to )
{
    if( to != "application/x-karbon" || from != "image/x-wmf" ) 
    return KoFilter::NotImplemented;


    return KoFilter::OK;
}


#include <wmfexport.moc>
