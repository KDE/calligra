/***************************************************************************
 * kexiapppart.cpp
 * This file is part of the KDE project
 * copyright (C)2006 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "kexiapppart.h"

#include "core/kexipart.h"
#include "core/kexipartitem.h"
//#include "core/kexiproject.h"

using namespace Kross::KexiApp;

KexiAppPartItem::KexiAppPartItem(KexiPart::Item* item)
    : Kross::Api::Class<KexiAppPartItem>("KexiAppPartItem")
{
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,int> >
        ("identifier", item, &::KexiPart::Item::identifier );
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,void>, Kross::Api::ProxyValue<Kross::Api::Variant,int> >
        ("setIdentifier", item, &::KexiPart::Item::setIdentifier );

    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,QCString> >
        ("mimeType", item, &::KexiPart::Item::mimeType );
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,void>, Kross::Api::ProxyValue<Kross::Api::Variant,QCString> >
        ("setMimeType", item, &::KexiPart::Item::setMimeType );

    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,QString> >
        ("name", item, &::KexiPart::Item::name );
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,void>, Kross::Api::ProxyValue<Kross::Api::Variant,QString> >
        ("setName", item, &::KexiPart::Item::setName );

    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,QString> >
        ("caption", item, &::KexiPart::Item::caption );
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,void>, Kross::Api::ProxyValue<Kross::Api::Variant,QString> >
        ("setCaption", item, &::KexiPart::Item::setCaption );

    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,QString> >
        ("description", item, &::KexiPart::Item::description );
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,void>, Kross::Api::ProxyValue<Kross::Api::Variant,QString> >
        ("setDescription", item, &::KexiPart::Item::setDescription );
}
