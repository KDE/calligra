/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "OdfReaderAsciiContext.h"

// Calligra
#include <KoOdfStyleManager.h>
#include <KoXmlReader.h>

#include <QFile>

// ----------------------------------------------------------------
//                     class OdfReaderAsciiContext

OdfReaderAsciiContext::OdfReaderAsciiContext(KoStore *store, QFile &file)
    : OdfReaderContext(store)
    , outStream(&file)
{
}

OdfReaderAsciiContext::~OdfReaderAsciiContext() = default;
