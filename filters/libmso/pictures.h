/* This file is part of the KDE project
   Copyright (C) 2009 KO GmbH <jos.van.den.oever@kogmbh.com>

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
   Boston, MA 02110-1301, USA.
*/
#ifndef PICTURES_H
#define PICTURES_H

#include <pole.h>
#include <KoStore.h>
#include <KoXmlWriter.h>
#include "generated/simpleParser.h"

struct PictureReference {
    QString name;
    QString mimetype;
    QByteArray uid;
};

/**
 * Save the next picture record in the 'Pictures' stream into the ODF store.
 *
 * The pictures are saved in the currently opened folder of the ODF store.  It
 * is customary to switch to the folder 'Pictures' before calling this
 * function.
 *
 * @return The name under which the image is saved or an empty string when an
 *         error occurred.
 **/
PictureReference savePicture(POLE::Stream& stream, KoStore* store);

PictureReference savePicture(const MSO::OfficeArtBStoreContainerFileBlock& a,
                             KoStore* store);

/**
 * Look in blipStore for the id mapping to this object.
 *
 * @param dgg container for OfficeArt records that contain document-wide data
 * @param pib specifies the BLIP to display in the picture shape
 * @param offset into the associated OfficeArtBStoreDelay record
 * @return unique identifier of the pixel data in the BLIP
 **/
QByteArray getRgbUid(const MSO::OfficeArtDggContainer& dgg, quint32 pib, quint32& offset);

/**
 * Save pictures into the ODF store and write the appropriate manifest entry.
 *
 * @param ODF store
 * @param manifest writer
 * @param rgfb list of records that specifies the BLIP data.  The rgfb list is
 * either part of the OfficeArtBStoreContainer or OfficeArtBStoreDelay.
 * @return map of picture names vs. MD4 digests of the picture data.
 **/
QMap<QByteArray, QString> createPictures(KoStore* store, KoXmlWriter* manifest, const QList<MSO::OfficeArtBStoreContainerFileBlock>* rgfb);


#endif
