/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <jos.van.den.oever@kogmbh.com>
   SPDX-FileCopyrightText: 2010 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <OkularOdpGenerator.h>

#include <QDebug>
#include <QImage>
#include <QMimeDatabase>
#include <QMimeType>
#include <QPainter>

#include <KoDocumentEntry.h>
#include <KoDocumentInfo.h>
#include <KoGlobal.h>
#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <KoPart.h>

#include <okular/core/page.h>

#include <KLocalizedString>

OkularOdpGenerator::OkularOdpGenerator(QObject *parent, const QVariantList &args)
    : Okular::Generator(parent, args)
{
}

OkularOdpGenerator::~OkularOdpGenerator() = default;

bool OkularOdpGenerator::loadDocument(const QString &fileName, QVector<Okular::Page *> &pages)
{
    const QString mimetype = QMimeDatabase().mimeTypeForFile(fileName).name();

    QString error;
    KoDocumentEntry documentEntry = KoDocumentEntry::queryByMimeType(mimetype);
    KoPart *part = documentEntry.createKoPart(&error);

    if (!error.isEmpty()) {
        qWarning() << "Error creating document" << mimetype << error;
        return false;
    }

    auto doc = qobject_cast<KoPADocument *>(part->document());
    m_doc = doc;
    const QUrl url = QUrl::fromLocalFile(fileName);
    doc->setCheckAutoSaveFile(false);
    doc->setAutoErrorHandlingEnabled(false); // show error dialogs
    if (!doc->openUrl(url)) {
        return false;
    }
    doc->setReadWrite(false);
    doc->setAutoSave(0);

    int pageCount = m_doc->pageCount();
    for (int i = 0; i < pageCount; i++) {
        KoPAPageBase *kprpage = m_doc->pages().value(i);
        if (!kprpage) {
            continue;
        }
        QSize size = kprpage->size().toSize();

        auto page = new Okular::Page(i, size.width(), size.height(), Okular::Rotation0);
        pages.append(page);
    }

    const KoDocumentInfo *documentInfo = m_doc->documentInfo();
    m_documentInfo.set(Okular::DocumentInfo::MimeType, mimetype);
    m_documentInfo.set(Okular::DocumentInfo::Producer, documentInfo->originalGenerator());
    m_documentInfo.set(Okular::DocumentInfo::Title, documentInfo->aboutInfo("title"));
    m_documentInfo.set(Okular::DocumentInfo::Subject, documentInfo->aboutInfo("subject"));
    m_documentInfo.set(Okular::DocumentInfo::Keywords, documentInfo->aboutInfo("keyword"));
    m_documentInfo.set(Okular::DocumentInfo::Description, documentInfo->aboutInfo("description"));
    m_documentInfo.set("language", KoGlobal::languageFromTag(documentInfo->aboutInfo("language")), i18n("Language"));

    const QString creationDate = documentInfo->aboutInfo("creation-date");
    if (!creationDate.isEmpty()) {
        QDateTime t = QDateTime::fromString(creationDate, Qt::ISODate);
        m_documentInfo.set(Okular::DocumentInfo::CreationDate, QLocale().toString(t, QLocale::ShortFormat));
    }
    m_documentInfo.set(Okular::DocumentInfo::Creator, documentInfo->aboutInfo("initial-creator"));

    const QString modificationDate = documentInfo->aboutInfo("date");
    if (!modificationDate.isEmpty()) {
        QDateTime t = QDateTime::fromString(modificationDate, Qt::ISODate);
        m_documentInfo.set(Okular::DocumentInfo::ModificationDate, QLocale().toString(t, QLocale::ShortFormat));
    }
    m_documentInfo.set(Okular::DocumentInfo::Author, documentInfo->aboutInfo("creator"));

    return true;
}

bool OkularOdpGenerator::doCloseDocument()
{
    delete m_doc;
    m_doc = nullptr;

    m_documentInfo = Okular::DocumentInfo();

    return true;
}

bool OkularOdpGenerator::canGeneratePixmap() const
{
    return true;
}

void OkularOdpGenerator::generatePixmap(Okular::PixmapRequest *request)
{
    QPixmap *pix;
    if (!m_doc) {
        pix = new QPixmap(request->width(), request->height());
        QPainter painter(pix);
        painter.fillRect(0, 0, request->width(), request->height(), Qt::white);
    } else {
        KoPAPageBase *page = m_doc->pages().value(request->pageNumber());
        pix = new QPixmap(page->thumbnail(QSize(request->width(), request->height())));
    }

    request->page()->setPixmap(request->observer(), pix);

    signalPixmapRequestDone(request);
}

Okular::DocumentInfo OkularOdpGenerator::generateDocumentInfo(const QSet<Okular::DocumentInfo::Key> &keys) const
{
    Q_UNUSED(keys);

    return m_documentInfo;
}
