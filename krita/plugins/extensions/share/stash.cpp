/*
 * Copyright (c) 2012 Boudewijn Rempt (boud@valdyas.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <stash.h>

#include <QUrl>
#include <kis_image.h>

Stash::Stash(QObject *parent)
    : QObject(parent)
{

}

Stash::~Stash()
{

}

QList<Submission> Stash::submissions() const
{
    return m_submissions;
}

int Stash::bytesAvailable() const
{
    return m_bytesAvailable;
}

void Stash::testCall()
{
    qDebug() << "testCall";

    Q_ASSERT(m_currentReply.isNull());
    m_currentCall = Placebo;

    QUrl url("https://www.deviantart.com/api/draft15/placebo");
    QNetworkRequest request(url);
    m_currentReply = m_networkAccessManager.post(request, url.encodedQuery());
    connect(m_currentReply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(slotError(QNetworkReply::NetworkError)));
    connect(m_currentReply, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_currentReply, SIGNAL(finished()), SLOT(slotFinished()));
}

void Stash::submit(KisImageWSP image, const QString &title, const QString &comments, const QStringList &keywords, const QString &folder)
{

}

void Stash::update(const QString &stashid, const QString &title, const QString comments, const QStringList& keywords)
{

}


void Stash::move(const QString &stashid, const QString folder)
{

}


void Stash::renameFolder(const QString &folderId, const QString &folder)
{

}


void Stash::updateAvailableSpace()
{

}


void Stash::delta()
{

}


void Stash::fetch(const QString &id)
{

}

void Stash::slotReadyRead()
{
}

void Stash::slotError(QNetworkReply::NetworkError code)
{
    qDebug() << "slotError" << code;
    emit callFinished(m_currentCall, false);
    m_currentCall = None;
    m_currentReply->deleteLater();
}

void Stash::slotFinished()
{
    qDebug() << "slotFinished" << m_currentCall << m_currentReply->readAll();
    emit callFinished(m_currentCall, true);
    m_currentCall = None;
    m_currentReply->deleteLater();
}

#include "stash.moc"
