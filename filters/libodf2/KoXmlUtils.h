/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2014 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
