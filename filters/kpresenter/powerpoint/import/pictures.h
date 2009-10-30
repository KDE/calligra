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

/**
 * Save the next picture record in the 'Pictures' stream into the ODF store.
 * The parameter @position is used to give the picture a unique name.
 * The pictures are saved in the currently opened folder of the ODF store.
 * It is customary to switch to the folder 'Pictures' before calling this
 * function.
 * @return The name under which the image is saved or an empty string when
 *         an error occurred.
 **/
std::string savePicture(POLE::Stream& stream, int position, KoStore* store, QString& mimetype);

#endif
