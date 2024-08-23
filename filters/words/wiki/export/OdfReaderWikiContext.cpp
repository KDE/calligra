/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "OdfReaderWikiContext.h"

// Calligra
#include <KoOdfStyle.h>
#include <KoOdfStyleManager.h>
#include <KoOdfStyleProperties.h>
#include <KoXmlReader.h>

#include <QFile>

// ----------------------------------------------------------------
//                     class OdfReaderWikiContext

OdfReaderWikiContext::OdfReaderWikiContext(KoStore *store, QFile &file)
    : OdfReaderContext(store)
    , outStream(&file)
    , listLevelCounter(0)
{
}

OdfReaderWikiContext::~OdfReaderWikiContext() = default;

void OdfReaderWikiContext::pushStyle(KoOdfStyle *style)
{
    styleStack.push(style);
}

KoOdfStyle *OdfReaderWikiContext::popStyle()
{
    return styleStack.pop();
}

void OdfReaderWikiContext::pushListStyle(KoOdfListStyle *style)
{
    listStyleStack.push(style);
}

KoOdfListStyle *OdfReaderWikiContext::popListStyle()
{
    return listStyleStack.pop();
}
