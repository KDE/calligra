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

#include <QCoreApplication>
#include <QUrl>
#include <qjson/parser.h>
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

int Stash::availableSpace() const
{
    return m_bytesAvailable;
}

void Stash::testCall()
{
    QUrl url("https://www.deviantart.com/api/draft15/placebo");
    QNetworkRequest request(url);
    m_callMap[m_requestor->get(request)] = Placebo;
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
    // TODO remember to store the results and include the cursor if we have one from previously...
    QUrl url("https://www.deviantart.com/api/draft15/stash/delta");
    QNetworkRequest request(url);
    m_callMap[m_requestor->get(request)] = Delta;
}


void Stash::fetch(const QString &id)
{

}

void Stash::slotFinished(int id, QNetworkReply::NetworkError error, const QByteArray &data)
{
    Call currentCall = m_callMap[id];
    switch(currentCall)
    {
        case Placebo:
            testCallFinished(error, data);
            break;
        case Submit:
            submitCallFinished(error, data);
            break;
        case Update:
            updateCallFinished(error, data);
            break;
        case Move:
            moveCallFinished(error, data);
            break;
        case RenameFolder:
            renameFolderCallFinished(error, data);
            break;
        case UpdateAvailableSpace:
            updateAvailableSpaceCallFinished(error, data);
            break;
        case Delta:
            deltaCallFinished(error, data);
            break;
        case Fetch:
            fetchCallFinished(error, data);
            break;
        default:
            qDebug() << "Unknown call or successful completion of call after expected ending (no call currently set)";
            break;
    }
    m_callMap.remove(id);
}

void Stash::testCallFinished(QNetworkReply::NetworkError error, const QByteArray& data)
{
    QJson::Parser parser;
    bool ok(false);
    QVariantMap result = parser.parse(data, &ok).toMap();
    if(ok && result.contains("status")) {
        emit callFinished(Placebo, (result.value("status").toString() == QLatin1String("success")));
    }
}

void Stash::submitCallFinished(QNetworkReply::NetworkError error, const QByteArray& data)
{

}

void Stash::updateCallFinished(QNetworkReply::NetworkError error, const QByteArray& data)
{

}

void Stash::moveCallFinished(QNetworkReply::NetworkError error, const QByteArray& data)
{

}

void Stash::renameFolderCallFinished(QNetworkReply::NetworkError error, const QByteArray& data)
{

}

void Stash::updateAvailableSpaceCallFinished(QNetworkReply::NetworkError error, const QByteArray& data)
{

}

void Stash::deltaCallFinished(QNetworkReply::NetworkError error, const QByteArray& data)
{
    QJson::Parser parser;
    bool ok(false);
    QVariantMap result = parser.parse(data, &ok).toMap();
    qDebug() << result;
    if(ok && result.contains("status")) {
        emit callFinished(Delta, (error == QNetworkReply::NoError));
    }
}

void Stash::fetchCallFinished(QNetworkReply::NetworkError error, const QByteArray& data)
{

}

#include "stash.moc"
