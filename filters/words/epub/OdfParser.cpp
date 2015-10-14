/* This file is part of the KDE project

   Copyright (C) 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
   Copyright (C) 2012 Inge Wallin            <inge@lysator.liu.se>

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


// Own
#include "OdfParser.h"

// Qt
#include <QSvgGenerator>
#include <QBuffer>
#include <QPainter>

// KF5
#include <kpluginfactory.h>

// Calligra
#include <KoFilterChain.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>

#include "SharedExportDebug.h"

OdfParser::OdfParser()
{
}

OdfParser::~OdfParser()
{
}



KoFilter::ConversionStatus OdfParser::parseMetadata(KoStore *odfStore,
                                                    QHash<QString, QString> &metadata)
{
    if (!odfStore->open("meta.xml")) {
        debugSharedExport << "Cannot open meta.xml";
        return KoFilter::FileNotFound;
    }

    KoXmlDocument doc;
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!doc.setContent(odfStore->device(), true, &errorMsg, &errorLine, &errorColumn)) {
        debugSharedExport << "Error occurred while parsing meta.xml "
                 << errorMsg << " in Line: " << errorLine
                 << " Column: " << errorColumn;
        odfStore->close();
        return KoFilter::ParsingError;
    }

    KoXmlNode childNode = doc.documentElement();
    childNode = KoXml::namedItemNS(childNode, KoXmlNS::office, "meta");
    KoXmlElement element;
    forEachElement (element, childNode) {
        metadata.insert(element.tagName(), element.text());
    }

    odfStore->close();
    return KoFilter::OK;
}


KoFilter::ConversionStatus OdfParser::parseManifest(KoStore *odfStore,
                                                    QHash<QString, QString> &manifest)
{
    if (!odfStore->open("META-INF/manifest.xml")) {
        debugSharedExport << "Cannot to open manifest.xml.";
        return KoFilter::FileNotFound;
    }

    KoXmlDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    if (!doc.setContent(odfStore->device(), true, &errorMsg, &errorLine, &errorColumn)) {
        debugSharedExport << "Error occurred while parsing meta.xml "
                 << errorMsg << " in Line: " << errorLine
                 << " Column: " << errorColumn;
        return KoFilter::ParsingError;
    }

    KoXmlNode childNode = doc.documentElement();
    KoXmlElement nodeElement;
    forEachElement (nodeElement, childNode) {
        // Normalize the file name, i.e. remove trailing slashes.
        QString path = nodeElement.attribute("full-path");
        if (path.endsWith(QLatin1Char('/')))
            path.chop(1);
        QString type = nodeElement.attribute("media-type");

        manifest.insert(path, type);
    }

    odfStore->close();
    return KoFilter::OK;
}
