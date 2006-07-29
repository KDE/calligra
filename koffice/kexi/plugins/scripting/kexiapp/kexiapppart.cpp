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
    this->addFunction0<Kross::Api::Variant>("identifier", item, &::KexiPart::Item::identifier );

    this->addFunction1<void, Kross::Api::Variant>("setIdentifier", item, &::KexiPart::Item::setIdentifier );

    this->addFunction0<Kross::Api::Variant>("mimeType", item, &::KexiPart::Item::mimeType );
    this->addFunction1<void, Kross::Api::Variant>("setMimeType", item, &::KexiPart::Item::setMimeType );

    this->addFunction0<Kross::Api::Variant>("name", item, &::KexiPart::Item::name );
    this->addFunction1<void, Kross::Api::Variant>("setName", item, &::KexiPart::Item::setName );

    this->addFunction0<Kross::Api::Variant>("caption", item, &::KexiPart::Item::caption );
    this->addFunction1<void, Kross::Api::Variant>("setCaption", item, &::KexiPart::Item::setCaption );

    this->addFunction0<Kross::Api::Variant>("description", item, &::KexiPart::Item::description );
    this->addFunction1<void, Kross::Api::Variant>("setDescription", item, &::KexiPart::Item::setDescription );
}
