/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Kaushik Pendurthi <kaushiksjce@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "VirtualKeyBoard.h"
#include "VirtualKeyBoardLanguageDownloader.h"

#ifdef Q_WS_MAEMO_5
#include <QtMaemo5/QMaemo5InformationBox>
#endif


DownloadManager::DownloadManager()
{
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(downloadLanguageFinished(QNetworkReply*)));
}
void DownloadManager::execute(QString language,QString pathToBeStoredIn,QString comment)
{
    QString args("http://212.213.221.115/vkb/VirtualKeyBoardHeaders/");
    args.append(language);
    QNetworkRequest request(QUrl::fromEncoded(args.toLocal8Bit()));
    QNetworkReply *reply = manager.get(request);
    languageDownload.append(reply);
    setLanguagePath=pathToBeStoredIn;
    setComment=comment;
}

void DownloadManager::downloadLanguageFinished(QNetworkReply *reply)
{
    QUrl url = reply->url();
    if (reply->error()) {
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(0,i18n("please check the connection and try again"),
                                           QMaemo5InformationBox::DefaultTimeout);
#endif
    } else {
        QString path = url.path();
        QString filename = QFileInfo(path).fileName();
        QFile file(setLanguagePath+filename);

        if (!file.open(QIODevice::WriteOnly)) {
            return ;
        }
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(0,setComment,//i18n("language downloaded successfully\nRestart the keyboard for using it"),
                                           QMaemo5InformationBox::DefaultTimeout);
#endif
        file.write(reply->readAll());
        file.close();
        if(setLanguagePath.contains(".headers"))
            emit downloadedSuccessfully();
    }

    languageDownload.removeAll(reply);
    reply->deleteLater();
}
