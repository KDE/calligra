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
#include <QBuffer>
#include <qjson/parser.h>
#include <KTemporaryFile>
#include <QImageWriter>
#include <complex>
#include <kis_image.h>

Stash::Stash(O2DeviantART *deviant, QObject *parent)
    : QObject(parent)
{
    m_requestor = new O2Requestor(&m_networkAccessManager, deviant, this);
    connect(m_requestor, SIGNAL(finished(int,QNetworkReply::NetworkError,QByteArray)), SLOT(slotFinished(int,QNetworkReply::NetworkError,QByteArray)));
    connect(m_requestor, SIGNAL(uploadProgress(int,qint64,qint64)), SLOT(uploadProgress(int,qint64,qint64)));
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

void Stash::submit(KisImageWSP image, const QString &filename, const QString &title, const QString &comments, const QStringList &keywords, const QString &folder)
{
    QUrl url("https://www.deviantart.com/api/draft15/stash/submit");
    url.addQueryItem("title", title);
    url.addQueryItem("artist_comments", comments);
    url.addQueryItem("keywords", keywords.join(","));
    if(!folder.isEmpty())
        url.addQueryItem("folderid", folder);
    QNetworkRequest request(url);

    //QString bound="margin"; //name of the boundary
    //according to rfc 1867 we need to put this string here:
    QByteArray data(QString("--margin\r\n").toAscii());
    data.append("Content-Disposition: form-data; name=\"action\"\r\n\r\n");
    data.append("submit\r\n");   //our script's name, as I understood. Please, correct me if I'm wrong
    data.append("--margin\r\n");   //according to rfc 1867
    data.append(QString("Content-Disposition: form-data; name=\"uploaded\"; filename=\"%1\"\r\n").arg(filename).toAscii());  //name of the input is "uploaded" in my form, next one is a file name.
    data.append("Content-Type: image/jpeg\r\n\r\n"); //data type

    QByteArray tmpData;
    QBuffer buffer(&tmpData, this);
    buffer.open(QIODevice::WriteOnly);
    QImage imagedata = image->convertToQImage(image->bounds(), image->profile());
    imagedata.save(&buffer, "PNG");
    data.append(tmpData);

    data.append("\r\n");
    data.append("--margin--\r\n");  //closing boundary according to rfc 1867
    request.setRawHeader(QString("Content-Type").toAscii(),QString("multipart/form-data; boundary=margin").toAscii());
    request.setRawHeader(QString("Content-Length").toAscii(), QString::number(data.length()).toAscii());

    m_callMap[m_requestor->post(request, data)] = Submit;
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
    qDebug() << "submit call finished";
    qDebug() << data;
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
    if(!ok)
        emit callFinished(Delta, (error == QNetworkReply::NoError));

    // TODO handle has_more and cursor
    m_submissions.clear();
    if(result.contains("entries")) {
        foreach(const QVariant& var, result.value("entries").toList()) {
            QVariantMap entry = var.toMap();
            Submission sub;
            sub.id = entry.value("stashid").toString();
            sub.folderId = entry.value("folderid").toString();
            QVariantMap meta = entry.value("metadata").toMap();
            sub.artist_comments = meta.value("artist_comments").toString();
            sub.description = meta.value("description").toString();
            sub.isFolder = meta.value("is_folder").toBool();
            sub.title = meta.value("title").toString();
            m_submissions.append(sub);
            qDebug() << "-----------------------\n" << meta;
        }
    }
    emit callFinished(Delta, true);
    emit submissionsChanged();
}

void Stash::fetchCallFinished(QNetworkReply::NetworkError error, const QByteArray& data)
{

}

void Stash::uploadProgress(int id, qint64 bytesSent, qint64 bytesTotal)
{
    qDebug() << id << bytesSent << "/" << bytesTotal;
}

#include "stash.moc"
