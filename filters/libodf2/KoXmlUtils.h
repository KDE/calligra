/* This file is part of the KDE project

   Copyright (C) 2014 Inge Wallin <inge@lysator.liu.se>

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

#ifndef KOXMLUTILS_H
#define KOXMLUTILS_H


#include "koodf2_export.h"


class QString;
class KoXmlStreamReader;


/**
 * @brief Read and collect the text (character data) from an element.
 *
 * This function is recursive and disregards everything from child elements
 * except any text content.
 *
 * @param reader The XML stream reader that we will read from. This should point to a start element.
 * @param result The result will be accumulated into this string.
 *
 */

void KOODF2_EXPORT readCharacterData(KoXmlStreamReader &reader, QString &result);


#endif /* KOXMLUTILS_H */
