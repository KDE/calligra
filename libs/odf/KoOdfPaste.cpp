/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoOdfPaste.h"

#include <QBuffer>
#include <QByteArray>
#include <QMimeData>
#include <QString>

#include <OdfDebug.h>

#include <KoOdfReadStore.h>
#include <KoStore.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>

KoOdfPaste::KoOdfPaste() = default;

KoOdfPaste::~KoOdfPaste() = default;

bool KoOdfPaste::paste(KoOdf::DocumentType documentType, const QMimeData *data)
{
    QByteArray arr = data->data(KoOdf::mimeType(documentType));
    return paste(documentType, arr);
}

bool KoOdfPaste::paste(KoOdf::DocumentType documentType, const QByteArray &bytes)
{
    if (bytes.isEmpty())
        return false;

    QBuffer buffer;
    buffer.setData(bytes);
    KoStore *store = KoStore::createStore(&buffer, KoStore::Read);
    // FIXME: Use shared_ptr or smth like these to auto delete store on return
    //  and delete all next "delete store;".

    KoOdfReadStore odfStore(store); // KoOdfReadStore does not delete the store on destruction

    QString errorMessage;
    if (!odfStore.loadAndParse(errorMessage)) {
        warnOdf << "loading and parsing failed:" << errorMessage;
        delete store;
        return false;
    }

    KoXmlElement content = odfStore.contentDoc().documentElement();
    KoXmlElement realBody(KoXml::namedItemNS(content, KoXmlNS::office, "body"));

    if (realBody.isNull()) {
        warnOdf << "No body tag found";
        delete store;
        return false;
    }

    KoXmlElement body = KoXml::namedItemNS(realBody, KoXmlNS::office, KoOdf::bodyContentElement(documentType, false));

    if (body.isNull()) {
        warnOdf << "No" << KoOdf::bodyContentElement(documentType, true) << "tag found";
        delete store;
        return false;
    }

    bool retval = process(body, odfStore);
    delete store;
    return retval;
}
