/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2014 Lassi Nieminen <lassniem@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DOCXSTYLEHELPER_H
#define DOCXSTYLEHELPER_H

class KoXmlWriter;
class KoOdfStyleProperties;
class KoOdfStyleManager;

#include <QString>

namespace DocxStyleHelper
{
void handleTextStyles(KoOdfStyleProperties *properties, KoXmlWriter *writer);
void handleParagraphStyles(KoOdfStyleProperties *properties, KoXmlWriter *writer);
void inheritTextStyles(KoOdfStyleProperties *destinationProperties, const QString &parent, KoOdfStyleManager *manager);
};

#endif // DOCXSTYLEHELPER_H
