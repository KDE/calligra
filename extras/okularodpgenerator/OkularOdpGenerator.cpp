/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>
   Copyright (C) 2010 Sven Langkamp <sven.langkamp@gmail.com>

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

#include <OkularOdpGenerator.h>

#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QMimeDatabase>
#include <QMimeType>

#include <KoDocumentEntry.h>
#include <KoPart.h>
#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <KoDocumentInfo.h>
#include <KoGlobal.h>

#include <okular/core/page.h>

#include <KLocalizedString>


OkularOdpGenerator::OkularOdpGenerator( QObject *parent, const QVariantList &args )
    : Okular::Generator( parent, args )
{
    m_doc = 0;
}

OkularOdpGenerator::~OkularOdpGenerator()
{
}

bool OkularOdpGenerator::loadDocument( const QString &fileName, QVector<Okular::Page*> &pages )
{
    const QString mimetype = QMimeDatabase().mimeTypeForFile(fileName).name();

    QString error;
    KoDocumentEntry documentEntry = KoDocumentEntry::queryByMimeType(mimetype);
    KoPart *part = documentEntry.createKoPart(&error);

    if (!error.isEmpty()) {
        qWarning() << "Error creating document" << mimetype << error;
        return 0;
    }

    KoPADocument* doc = qobject_cast<KoPADocument*>(part->document());
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
    for(int i = 0; i < pageCount; i++) {
        KoPAPageBase* kprpage = m_doc->pages().value(i);
        if (!kprpage) {
            continue;
        }
        QSize size = kprpage->size().toSize();

        Okular::Page * page = new Okular::Page( i, size.width(), size.height(), Okular::Rotation0 );
        pages.append(page);
    }

    const KoDocumentInfo *documentInfo = m_doc->documentInfo();
    m_documentInfo.set( Okular::DocumentInfo::MimeType, mimetype );
    m_documentInfo.set( Okular::DocumentInfo::Producer, documentInfo->originalGenerator() );
    m_documentInfo.set( Okular::DocumentInfo::Title,       documentInfo->aboutInfo("title") );
    m_documentInfo.set( Okular::DocumentInfo::Subject,     documentInfo->aboutInfo("subject") );
    m_documentInfo.set( Okular::DocumentInfo::Keywords,     documentInfo->aboutInfo("keyword") );
    m_documentInfo.set( Okular::DocumentInfo::Description, documentInfo->aboutInfo("description") );
    m_documentInfo.set( "language",    KoGlobal::languageFromTag(documentInfo->aboutInfo("language")),  i18n("Language"));

    const QString creationDate = documentInfo->aboutInfo("creation-date");
    if (!creationDate.isEmpty()) {
        QDateTime t = QDateTime::fromString(creationDate, Qt::ISODate);
        m_documentInfo.set( Okular::DocumentInfo::CreationDate, QLocale().toString(t, QLocale::ShortFormat) );
    }
    m_documentInfo.set( Okular::DocumentInfo::Creator,  documentInfo->aboutInfo("initial-creator") );

    const QString modificationDate = documentInfo->aboutInfo("date");
    if (!modificationDate.isEmpty()) {
        QDateTime t = QDateTime::fromString(modificationDate, Qt::ISODate);
        m_documentInfo.set( Okular::DocumentInfo::ModificationDate, QLocale().toString(t, QLocale::ShortFormat) );
    }
    m_documentInfo.set( Okular::DocumentInfo::Author, documentInfo->aboutInfo("creator") );

    return true;
}

bool OkularOdpGenerator::doCloseDocument()
{
    delete m_doc;
    m_doc = 0;

    m_documentInfo = Okular::DocumentInfo();

    return true;
}

bool OkularOdpGenerator::canGeneratePixmap() const
{
    return true;
}

void OkularOdpGenerator::generatePixmap( Okular::PixmapRequest *request )
{
    QPixmap* pix;
    if (!m_doc) {
        pix = new QPixmap(request->width(), request->height());
        QPainter painter(pix);
        painter.fillRect(0 ,0 , request->width(), request->height(), Qt::white);
    } else {
        KoPAPageBase* page = m_doc->pages().value(request->pageNumber());
        pix = new QPixmap(page->thumbnail(QSize(request->width(), request->height())));
    }

   request->page()->setPixmap( request->observer(), pix );

    signalPixmapRequestDone( request );
}

Okular::DocumentInfo OkularOdpGenerator::generateDocumentInfo( const QSet<Okular::DocumentInfo::Key> &keys ) const
{
    Q_UNUSED(keys);

    return m_documentInfo;
}
