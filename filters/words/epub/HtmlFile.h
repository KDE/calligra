/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef HTMLFILE_H
#define HTMLFILE_H

#include <QHash>

#include <KoFilter.h>

#include "FileCollector.h"

class QString;

class KoStore;

class HtmlFile : public FileCollector
{
public:
    HtmlFile();
    ~HtmlFile() override;

    // When you have created all the content and added it using
    // addContentFile(), call this function once and it will write the
    // html to the disk.
    KoFilter::ConversionStatus writeHtml(const QString &fileName);

private:
    KoFilter::ConversionStatus writeMetaInf(KoStore *htmlStore);

private:
};

#endif // HTMLFILE_H
