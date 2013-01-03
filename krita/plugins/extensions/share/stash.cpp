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

Stash::Stash(O2DeviantART *deviant, QObject *parent)
    : QObject(parent)
{
    m_requestor = new O2Requestor(&m_networkAccessManager, deviant, this);
    connect(m_requestor, SIGNAL(finished(int,QNetworkReply::NetworkError,QByteArray)), SLOT(slotFinished(int,QNetworkReply::NetworkError,QByteArray)));
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

    QUrl url("https://www.deviantart.com/api/draft15/placebo");
    QNetworkRequest request(url);
    int id = m_requestor->get(request);
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

void Stash::slotFinished(int id, QNetworkReply::NetworkError error, const QByteArray &data)
{
    qDebug() << "Data in" << Q_FUNC_INFO << QString(data);
}

#include "stash.moc"
