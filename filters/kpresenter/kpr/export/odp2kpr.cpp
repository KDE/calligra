/* This file is part of the KDE project
   Copyright (C) 2006, 2007 Laurent Montel <montel@kde.org>

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

#include <kgenericfactory.h>
#include <KoStoreDevice.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <odp2kpr.h>
#include <KoGlobal.h>
#include <KoXmlNS.h>
#include <QXmlInputSource>
#include <QXmlSimpleReader>
#include <KoDom.h>
#include <KoDocumentInfo.h>
#include <KoOdfReadStore.h>

typedef KGenericFactory<Odp2Kpr> Odp2KprFactory;
K_EXPORT_COMPONENT_FACTORY(libodp2kpr, Odp2KprFactory("kofficefilters"))

Odp2Kpr::Odp2Kpr(QObject *parent, const QStringList&) :
        KoFilter(parent)
{
}

KoFilter::ConversionStatus Odp2Kpr::convert(const QByteArray& from, const QByteArray& to)
{
    if (to != "application/x-kpresenter"
            || from != "application/vnd.oasis.opendocument.presentation")
        return KoFilter::NotImplemented;

    KoStoreDevice* inpdev = m_chain->storageFile("content.xml", KoStore::Read);
    if (!inpdev) {
        kError(31000) << "Unable to open input stream";
        return KoFilter::StorageCreationError;
    }

    // Parse presentation content.xml
    QXmlInputSource source(inpdev);
    QXmlSimpleReader reader;
    KoOdfReadStore::setupXmlReader(reader, true /*namespaceProcessing*/);
    QString errorMsg;
    int errorLine, errorColumn;
    bool ok = doc.setContent(&source, &reader, &errorMsg, &errorLine, &errorColumn);
    if (!ok) {
        kError(31000) << "Parsing error! Aborting!" << endl
        << " In line: " << errorLine << ", column: " << errorColumn << endl
        << " Error message: " << errorMsg;
        return KoFilter::ParsingError;
    }

    QDomElement docElem = doc.documentElement();
    QDomElement realBody(KoDom::namedItemNS(docElem, KoXmlNS::office, "body"));
    if (realBody.isNull()) {
        kError(31000) << "No office:body found!";
        //setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No office:body tag found." ) );
        return KoFilter::WrongFormat;
    }

    QDomElement body = KoDom::namedItemNS(realBody, KoXmlNS::office, "presentation");
    if (body.isNull()) {
        kError(32001) << "No office:text found!";
        return KoFilter::WrongFormat;
    }

    //now we can transform it.

    //Parse meta.xml directly
    inpdev = m_chain->storageFile("meta.xml", KoStore::Read);

    KoXmlDocument metaDoc;
    QString errorMessage;
    KoDocumentInfo info;
    if (KoOdfReadStore::loadAndParse(inpdev, metaDoc, errorMessage, "meta.xml" /*just for debug message*/)) {
        info.loadOasis(metaDoc);
    }

    // Write output file
    KoStoreDevice* out = m_chain->storageFile("root", KoStore::Write);
    if (!out) {
        kError(30502) << "Unable to open output file!";
        return KoFilter::StorageCreationError;
    }
    QByteArray cstring = outdoc.toByteArray(); // utf-8 already
    out->write(cstring.data(), cstring.length());
    out->close();

    //Write documentinfo.xml
    out = m_chain->storageFile("documentinfo.xml", KoStore::Write);
    if (!out) {
        kError(30502) << "Unable to open output file!";
        return KoFilter::StorageCreationError;
    }
    QDomDocument doc = info.save();
    QByteArray s = doc.toByteArray(); // this is already Utf8!
    out->write(cstring.data(), cstring.length());
    out->close();
    return KoFilter::OK;
}

#include <odp2kpr.moc>
