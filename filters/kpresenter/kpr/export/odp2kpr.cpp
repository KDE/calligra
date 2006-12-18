/* This file is part of the KDE project
   Copyright (C) 2006 Laurent Montel <montel@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include <kgenericfactory.h>
#include <KoStoreDevice.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <kprodp.h>

typedef KGenericFactory<Odp2Kpr> Odp2KprFactory;
K_EXPORT_COMPONENT_FACTORY( libodp2kpr, KprOdpFactory( "kofficefilters" ) )

Odp2Kpr::Odp2Kpr(QObject *parent,const QStringList&) :
    KoFilter(parent)
{
}

KoFilter::ConversionStatus Odp2Kpr::convert( const QByteArray& from, const QByteArray& to )
{
    if ( to != "application/x-kpresenter"
         || from != "application/vnd.oasis.opendocument.presentation" )
        return KoFilter::NotImplemented;

    KoStoreDevice* inpdev = m_chain->storageFile( "content.xml", KoStore::Read );
    if ( !inpdev )
    {
        kError(31000) << "Unable to open input stream" << endl;
        return KoFilter::StorageCreationError;
    }


    return KoFilter::OK;
}

#include <odp2kpr.moc>
