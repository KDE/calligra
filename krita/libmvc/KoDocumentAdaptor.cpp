/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>
   Copyright (C) 2006 Fredrik Edemar <f_edemar@linux.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KoDocumentAdaptor.h"

#include <QList>

#include "KoDocument.h"
#include "KoDocumentInfoDlg.h"
#include "KoDocumentInfo.h"
#include "KoView.h"
#include <kdebug.h>

KoDocumentAdaptor::KoDocumentAdaptor(KoDocument *doc)
    : QDBusAbstractAdaptor(doc)
{
    setAutoRelaySignals(true);
    m_document = doc;
}

KoDocumentAdaptor::~KoDocumentAdaptor()
{
}

void KoDocumentAdaptor::openUrl(const QString & url)
{
    m_document->openUrl(KUrl(url));
}

bool KoDocumentAdaptor::isLoading()
{
    return m_document->isLoading();
}

QString KoDocumentAdaptor::url()
{
    return m_document->url().url();
}

bool KoDocumentAdaptor::isModified()
{
    return m_document->isModified();
}

void KoDocumentAdaptor::save()
{
    m_document->save();
}

void KoDocumentAdaptor::saveAs(const QString & url)
{
    m_document->saveAs(KUrl(url));
    m_document->waitSaveComplete(); // see ReadWritePart
}

void KoDocumentAdaptor::setOutputMimeType(const QByteArray& mimetype)
{
    m_document->setOutputMimeType(mimetype);
}

QString KoDocumentAdaptor::documentInfoAuthorName() const
{
    return m_document->documentInfo()->authorInfo("creator");
}

QString KoDocumentAdaptor::documentInfoEmail() const
{
    return m_document->documentInfo()->authorInfo("email");
}

QString KoDocumentAdaptor::documentInfoCompanyName() const
{
    return m_document->documentInfo()->authorInfo("company");
}

QString KoDocumentAdaptor::documentInfoTelephone() const
{
    kDebug(30003) << " Keep compatibility with calligra <= 1.3 : use documentInfoTelephoneWork";
    return documentInfoTelephoneWork();
}

QString KoDocumentAdaptor::documentInfoTelephoneWork() const
{
    return m_document->documentInfo()->authorInfo("telephone-work");
}

QString KoDocumentAdaptor::documentInfoTelephoneHome() const
{
    return m_document->documentInfo()->authorInfo("telephone-home");
}


QString KoDocumentAdaptor::documentInfoFax() const
{
    return m_document->documentInfo()->authorInfo("fax");

}
QString KoDocumentAdaptor::documentInfoCountry() const
{
    return m_document->documentInfo()->authorInfo("country");

}
QString KoDocumentAdaptor::documentInfoPostalCode() const
{
    return m_document->documentInfo()->authorInfo("postal-code");

}
QString KoDocumentAdaptor::documentInfoCity() const
{
    return m_document->documentInfo()->authorInfo("city");
}

QString KoDocumentAdaptor::documentInfoInitial() const
{
    return m_document->documentInfo()->authorInfo("initial");
}

QString KoDocumentAdaptor::documentInfoAuthorPostion() const
{
    return m_document->documentInfo()->authorInfo("position");
}

QString KoDocumentAdaptor::documentInfoStreet() const
{
    return m_document->documentInfo()->authorInfo("street");
}

QString KoDocumentAdaptor::documentInfoTitle() const
{
    return m_document->documentInfo()->aboutInfo("title");
}

QString KoDocumentAdaptor::documentInfoAbstract() const
{
    return m_document->documentInfo()->aboutInfo("comments");
}

QString KoDocumentAdaptor::documentInfoKeywords() const
{
    return m_document->documentInfo()->aboutInfo("keywords");
}

QString KoDocumentAdaptor::documentInfoSubject() const
{
    return m_document->documentInfo()->aboutInfo("subject");
}
void KoDocumentAdaptor::setDocumentInfoKeywords(const QString & text)
{
    m_document->documentInfo()->setAboutInfo("keywords", text);
}

void KoDocumentAdaptor::setDocumentInfoSubject(const QString & text)
{
    m_document->documentInfo()->setAboutInfo("subject", text);
}

void KoDocumentAdaptor::setDocumentInfoAuthorName(const QString & text)
{
    m_document->documentInfo()->setAuthorInfo("creator", text);
}

void KoDocumentAdaptor::setDocumentInfoEmail(const QString &text)
{
    m_document->documentInfo()->setAuthorInfo("email", text);
}

void KoDocumentAdaptor::setDocumentInfoCompanyName(const QString &text)
{
    m_document->documentInfo()->setAuthorInfo("company", text);
}

void KoDocumentAdaptor::setDocumentInfoAuthorPosition(const QString &text)
{
    m_document->documentInfo()->setAuthorInfo("position", text);
}

void KoDocumentAdaptor::setDocumentInfoTelephone(const QString &text)
{
    kDebug(30003) << "Keep compatibility with calligra <= 1.3 : use setDocumentInfoTelephoneWork";
    setDocumentInfoTelephoneWork(text);
}

void KoDocumentAdaptor::setDocumentInfoTelephoneWork(const QString &text)
{
    m_document->documentInfo()->setAuthorInfo("telephone-work", text);
}

void KoDocumentAdaptor::setDocumentInfoTelephoneHome(const QString &text)
{
    m_document->documentInfo()->setAuthorInfo("telephone", text);
}

void KoDocumentAdaptor::setDocumentInfoFax(const QString &text)
{
    m_document->documentInfo()->setAuthorInfo("fax", text);
}

void KoDocumentAdaptor::setDocumentInfoCountry(const QString &text)
{
    m_document->documentInfo()->setAuthorInfo("country", text);
}

void KoDocumentAdaptor::setDocumentInfoTitle(const QString & text)
{
    m_document->documentInfo()->setAboutInfo("title", text);
}

void KoDocumentAdaptor::setDocumentInfoPostalCode(const QString &text)
{
    m_document->documentInfo()->setAuthorInfo("postal-code", text);
}

void KoDocumentAdaptor::setDocumentInfoCity(const QString & text)
{
    m_document->documentInfo()->setAuthorInfo("city", text);
}

void KoDocumentAdaptor::setDocumentInfoInitial(const QString & text)
{
    m_document->documentInfo()->setAuthorInfo("initial", text);
}

void KoDocumentAdaptor::setDocumentInfoStreet(const QString &text)
{
    m_document->documentInfo()->setAuthorInfo("street", text);
}

void KoDocumentAdaptor::setDocumentInfoAbstract(const QString &text)
{
    m_document->documentInfo()->setAboutInfo("comments", text);
}


#include <KoDocumentAdaptor.moc>
