// This file is part of the KDE project
// SPDX-FileLicenseText: 2019 Dag Andersen <danders@get2net.dk>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "KoEmbeddedDocumentLoader.h"

#include "KoDocumentBase.h"
#include "KoOdfLoadingContext.h"
#include "OdfDebug.h"

#include <QList>
#include <QString>
#include <QUrl>

#include <KoStore.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>

#define INTERNAL_PROTOCOL "intern"
#define STORE_PROTOCOL "tar"

class Q_DECL_HIDDEN KoEmbeddedDocumentLoader::Private
{
public:
    Private() = default;

    KoDocumentBase *doc;
};

KoEmbeddedDocumentLoader::KoEmbeddedDocumentLoader(KoDocumentBase *doc)
    : d(new Private())
{
    d->doc = doc;
}

KoEmbeddedDocumentLoader::~KoEmbeddedDocumentLoader()
{
    delete d;
}

bool isInternalUrl(const QString &url)
{
    return url.startsWith(STORE_PROTOCOL) || url.startsWith(INTERNAL_PROTOCOL) || QUrl::fromUserInput(url).isRelative();
}

bool KoEmbeddedDocumentLoader::loadEmbeddedDocument(const KoXmlElement &element, KoOdfLoadingContext &context)
{
    if (!element.hasAttributeNS(KoXmlNS::xlink, "href")) {
        errorOdf << "Object element has no valid xlink:href attribute";
        return false;
    }
    QString url = element.attributeNS(KoXmlNS::xlink, "href");
    // It can happen that the url is empty e.g. when it is a
    // presentation:placeholder.
    if (url.isEmpty()) {
        return true;
    }
    QString tmpURL;
    if (url[0] == '#') {
        url.remove(0, 1);
    }
    if (QUrl::fromUserInput(url).isRelative()) {
        if (url.startsWith("./")) {
            tmpURL = QString(INTERNAL_PROTOCOL) + ":/" + url.mid(2);
        } else {
            tmpURL = QString(INTERNAL_PROTOCOL) + ":/" + url;
        }
    } else {
        tmpURL = url;
    }
    KoStore *store = context.store();
    QString path = tmpURL;
    if (tmpURL.startsWith(INTERNAL_PROTOCOL)) {
        path = store->currentPath();
        if (!path.isEmpty() && !path.endsWith('/')) {
            path += '/';
        }
        QString relPath = QUrl::fromUserInput(tmpURL).path();
        path += QStringView{relPath}.mid(1); // remove leading '/'
    }
    if (!path.endsWith('/')) {
        path += '/';
    }
    const QString mimeType = context.mimeTypeForPath(path);
    // debugOdf << "path for manifest file=" << path << "mimeType=" << mimeType;
    if (mimeType.isEmpty()) {
        // debugOdf << "Manifest doesn't have media-type for" << path;
        return false;
    }
    bool res = true;
    if (isInternalUrl(tmpURL)) {
        store->pushDirectory();
        Q_ASSERT(tmpURL.startsWith(INTERNAL_PROTOCOL));
        QString relPath = QUrl::fromUserInput(tmpURL).path().mid(1);
        store->enterDirectory(relPath);
        res = d->doc->loadOasisFromStore(store);
        store->popDirectory();
        d->doc->setStoreInternal(true);
    } else {
        // TODO
        //          // Reference to an external document.
        //          dpc->setStoreInternal(false);
        //          QUrl url = QUrl::fromUserInput(tmpURL);
        //          if (!url.isLocalFile()) {
        //              //QApplication::restoreOverrideCursor();
        //
        //              // For security reasons we need to ask confirmation if the
        //              // url is remote.
        //              int result = KMessageBox::warningTwoActionsCancel(
        //                  0, i18n("This document contains an external link to a remote document\n%1", tmpURL),
        //                                                           i18n("Confirmation Required"), KGuiItem(i18n("Download")), KGuiItem(i18n("Skip")));
        //
        //              if (result == KMessageBox::Cancel) {
        //                  //d->m_parent->setErrorMessage("USER_CANCELED");
        //                  return false;
        //              }
        //              if (result == KMessageBox::PrimaryAction) {
        //                  res = openUrl(url);
        //              }
        //              // and if == No, res will still be false so we'll use a kounavail below
        //          } else {
        //              res = openUrl(url);
        //          }
    }
    return res;
}
