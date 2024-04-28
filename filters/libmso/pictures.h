/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 KO GmbH <jos.van.den.oever@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef PICTURES_H
#define PICTURES_H

#include "generated/simpleParser.h"
#include <pole.h>

#include <QMap>

class KoStore;
class KoXmlWriter;
class QImage;

struct PictureReference {
    QString name;
    QString mimetype;
    QByteArray uid;
};

enum OfficeArtBlipType {
    officeArtBlipEMF = 0xF01A,
    officeArtBlipWMF = 0xF01B,
    officeArtBlipPICT = 0xF01C,
    officeArtBlipJPEG = 0xF01D,
    officeArtBlipPNG = 0xF01E,
    officeArtBlipDIB = 0xF01F,
    officeArtBlipTIFF = 0xF029,
    officeArtBlipJPEG2 = 0xF02A // same handling as JPEG
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
PictureReference savePicture(POLE::Stream &stream, KoStore *store);

PictureReference savePicture(const MSO::OfficeArtBStoreContainerFileBlock &a, KoStore *store);

/**
 * Look in blipStore for the id mapping to this object.
 *
 * @param dgg container for OfficeArt records that contain document-wide data
 * @param pib specifies the BLIP to display in the picture shape
 * @param offset into the associated OfficeArtBStoreDelay record
 * @return unique identifier of the pixel data in the BLIP
 **/
QByteArray getRgbUid(const MSO::OfficeArtDggContainer &dgg, quint32 pib, quint32 &offset);

/**
 * Save pictures into the ODF store and write the appropriate manifest entry.
 *
 * @param store ODF store
 * @param manifest writer
 * @param rgfb list of records that specifies the BLIP data.  The rgfb list is
 * either part of the OfficeArtBStoreContainer or OfficeArtBStoreDelay.
 * @return map of picture names vs. MD4 digests of the picture data.
 **/
QMap<QByteArray, QString> createPictures(KoStore *store, KoXmlWriter *manifest, const QList<MSO::OfficeArtBStoreContainerFileBlock> *rgfb);

/**
 * Note: Copied from filters/libkowmf/qwmf.cc, the name is confusing as
 * the method convert the data into BMP and then into QImage
 *
 * Converts dib to QImage
 * @param bmp QImage input which will be filled with converted dib pixels
 * @param dib dib bytes
 * @param size size of the dib in bytes
 * @return if the conversion was successful
 **/
bool dibToBmp(QImage &bmp, const char *dib, long size);

#endif
