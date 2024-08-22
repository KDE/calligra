/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "OdfParser.h"

// Calligra
#include <KoStore.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>

#include "OdfReaderDebug.h"

OdfParser::OdfParser() = default;

OdfParser::~OdfParser() = default;

KoFilter::ConversionStatus OdfParser::parseMetadata(KoStore &odfStore,
                                                    // out parameters:
                                                    QHash<QString, QString> *metadata)
{
    if (!odfStore.open("meta.xml")) {
        debugOdfReader << "Cannot open meta.xml";
        return KoFilter::FileNotFound;
    }

    // FIXME: Convert this to the KoXmlStreamReader.
    KoXmlDocument doc;
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!doc.setContent(odfStore.device(), true, &errorMsg, &errorLine, &errorColumn)) {
        debugOdfReader << "Error occurred while parsing meta.xml " << errorMsg << " in Line: " << errorLine << " Column: " << errorColumn;
        odfStore.close();
        return KoFilter::ParsingError;
    }

    KoXmlNode childNode = doc.documentElement();
    childNode = KoXml::namedItemNS(childNode, KoXmlNS::office, "meta");
    KoXmlElement element;
    forEachElement(element, childNode)
    {
        metadata->insert(element.tagName(), element.text());
    }

    odfStore.close();
    return KoFilter::OK;
}

KoFilter::ConversionStatus OdfParser::parseManifest(KoStore &odfStore,
                                                    // out parameters:
                                                    QHash<QString, QString> *manifest)
{
    if (!odfStore.open("META-INF/manifest.xml")) {
        debugOdfReader << "Cannot to open manifest.xml.";
        return KoFilter::FileNotFound;
    }

    // FIXME: Convert this to the KoXmlStreamReader.
    KoXmlDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    if (!doc.setContent(odfStore.device(), true, &errorMsg, &errorLine, &errorColumn)) {
        debugOdfReader << "Error occurred while parsing meta.xml " << errorMsg << " in Line: " << errorLine << " Column: " << errorColumn;
        return KoFilter::ParsingError;
    }

    KoXmlNode childNode = doc.documentElement();
    KoXmlElement nodeElement;
    forEachElement(nodeElement, childNode)
    {
        // Normalize the file name, i.e. remove trailing slashes.
        QString path = nodeElement.attribute("full-path");
        if (path.endsWith(QLatin1Char('/')))
            path.chop(1);
        QString type = nodeElement.attribute("media-type");

        manifest->insert(path, type);
    }

    odfStore.close();
    return KoFilter::OK;
}
