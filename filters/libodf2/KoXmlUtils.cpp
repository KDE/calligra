/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2014 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "KoXmlUtils.h"

// Qt
#include <QString>

// libodf2
#include "KoXmlStreamReader.h"

void readCharacterData(KoXmlStreamReader &reader, QString &result)
{
    while (!reader.atEnd() && !reader.isEndElement()) {
        reader.readNext();

        if (reader.isCharacters()) {
            // debugOdf2 << "Found character data";
            result.append(reader.text());
        } else if (reader.isStartElement()) {
            // Collect character data recursively and read past the end element.
            readCharacterData(reader, result);
            reader.readNext();
        }
    }
}
