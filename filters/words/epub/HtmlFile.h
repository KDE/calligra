/* This file is part of the KDE project
   Copyright (C) 2012 Inge Wallin <inge@lysator.liu.se>

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
    KoFilter::ConversionStatus  writeHtml(const QString &fileName);

private:
    KoFilter::ConversionStatus  writeMetaInf(KoStore *htmlStore);

private:
};

#endif // HTMLFILE_H
